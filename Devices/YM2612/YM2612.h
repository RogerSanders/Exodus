/*----------------------------------------------------------------------------                              ----------*\
Description:
This core emulates the Yamaha YM2612(OPN2) FM synthesizer chip. Extensive hardware testing
has been done on the YM2612 in order to confirm the behaviour of the real chip, and this
emulation core has been built using those results.

Things to do:
-Test and implement the saved results within operator circuits present in the MAME core.
-Investigate new information on the embedded DAC and implement more accurate handling of
the DAC stage.
-Revamp the way CSM overflow events are tracked. We need to record overflow events using
a cycle count rather than a timeslice value. Rounding error can introduce some 1-cycle
errors in CSM timing events in the current implementation.
-Make the render process cycle-accurate. Rather than advancing by time steps, we need to
advance by cycles.
-Try toggling SSG-EG mode off when the output is currently inverted in an SSG-EG pattern,
and see if the output inversion is also immediately disabled.
-Figure out what we're going to do with the multiplexed output stream from this chip
long-term. Do we keep it around?
-Research and implement full support for all the behaviour of the YM2612 test register
-Implement support for the busy flag. We will need to perform tests on the actual waiting
times the chip requires. The integrated YM2612 will also need to be tested in case its
waiting times are different. Note also that the DAC registers in particular must be
tested. The DAC data is the register which is most likely to be hammered, and we have no
documentation on its waiting times whatsoever, as the YM2608 didn't have it. It is likely
however that the waiting time for a DAC data write is the same as changing the left/right
flags.

Debug options to add:
-Create a panel where each operator and channel can be either muted or disabled. Muted
only prevents sound being output to the accumulator while still allowing modulators to
modulate other operators, while disabling an operator forces it to always output 0. Also
add options to disable DAC, and disable output to either speaker for channel and master
outputs. Suggest rather than muted and disabled, simply call them disable as carrier and
disable as modulator.

References:
-YM2608(OPNA) Application Manual, Yamaha
-Genesis Sound Software Manual, Sega Enterprises, 1992, scans by antime
-MAME Envelope Generator Implementation, Jarek Burczynski
-YM2151 Users Manual, Yamaha
-YM2413 Application Manual, Yamaha, scans by Ricardo Bittencourt
-Y8950 Application Manual, Yamaha, scans by Albert Beevendorp
-YM2203(OPN) Datasheet, Yamaha
-YM3015 Datasheet, Yamaha LSI
-YM3016 Datasheet, Yamaha LSI
-YM2612 Test Register Notes, Charles MacDonald
-YM2151	Test Register Notes, Jarek Burczynski
\*--------------------------------------------------------------------------------------------------------------------*/
#include "IYM2612.h"
#ifndef __YM2612_H__
#define __YM2612_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <list>
#include <set>
#include <map>
#include "Device/Device.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include "AudioStream/AudioStream.pkg"
#include "Stream/Stream.pkg"

class YM2612 :public Device, public GenericAccessBase<IYM2612>
{
public:
	// Constructors
	YM2612(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);

	// Interface version functions
	virtual unsigned int GetIYM2612Version() const;

	// Initialization functions
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	void Reset(double accessTime);

	// Reference functions
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual void RemoveReference(IBusInterface* target);

	// Line functions
	virtual unsigned int GetLineID(const Marshal::In<std::wstring>& lineName) const;
	virtual Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	// Clock source functions
	virtual unsigned int GetClockSourceID(const Marshal::In<std::wstring>& clockSourceName) const;
	virtual Marshal::Ret<std::wstring> GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	// Execute functions
	virtual void BeginExecution();
	virtual void SuspendExecution();
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	// Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	// Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

	// Data read/write functions
	using IGenericAccess::ReadGenericData;
	using IGenericAccess::WriteGenericData;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue);

	// Data locking functions
	virtual bool GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const;
	virtual bool SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state);

private:
	// Enumerations
	enum class LineID;
	enum class ClockID;
	enum class AccessContext;

	// Structures
	struct OperatorData
	{
		enum ADSRPhase
		{
			ADSR_ATTACK,
			ADSR_DECAY,
			ADSR_SUSTAIN,
			ADSR_RELEASE
		};

		OperatorData()
		:attenuation(10), phaseCounter(20)
		{ }

		Data attenuation;   // 10-bit
		Data phaseCounter;  // 20-bit
		ADSRPhase phase;
		bool keyon;
		bool csmKeyOn;
		bool keyonPrevious;
		bool ssgOutputInverted;
	};
	struct TimerStateLocking
	{
		bool rate;
		bool counter;
		bool load;
		bool enable;
		bool overflow;
	};
	struct RegisterLocking
	{
		RegisterLocking(unsigned int adataID, const std::wstring& alockedValue)
		:dataID(adataID), usingChannelDataContext(false), usingOperatorDataContext(false), lockedValue(alockedValue)
		{ }
		RegisterLocking(unsigned int adataID, const ChannelDataContext& achannelDataContext, const std::wstring& alockedValue)
		:dataID(adataID), channelDataContext(achannelDataContext), usingChannelDataContext(true), usingOperatorDataContext(false), lockedValue(alockedValue)
		{ }
		RegisterLocking(unsigned int adataID, const OperatorDataContext& aoperatorDataContext, const std::wstring& alockedValue)
		:dataID(adataID), operatorDataContext(aoperatorDataContext), usingChannelDataContext(false), usingOperatorDataContext(true), lockedValue(alockedValue)
		{ }

		const IGenericAccess::DataContext* GetDataContext() const
		{
			if (usingChannelDataContext)
			{
				return &channelDataContext;
			}
			if (usingOperatorDataContext)
			{
				return &operatorDataContext;
			}
			return 0;
		}

		unsigned int dataID;
		bool usingChannelDataContext;
		ChannelDataContext channelDataContext;
		bool usingOperatorDataContext;
		OperatorDataContext operatorDataContext;
		const IGenericAccess::DataContext* dataContext;
		std::wstring lockedValue;
	};

	// Typedefs
	typedef RandomTimeAccessBuffer<Data, double>::AccessTarget AccessTarget;

	// Constants
	static const unsigned int ChannelAddressOffsets[ChannelCount];
	static const unsigned int OperatorAddressOffsets[ChannelCount][OperatorCount];
	static const unsigned int Channel3OperatorFrequencyAddressOffsets[2][OperatorCount];

	// Envelope generator constants
	static const unsigned int RateBitCount = 6;
	static const unsigned int AttenuationBitCount = 10;
	static const unsigned int FnumDataBitCount = 11;
	static const unsigned int PhaseGeneratorOutputBitCount = 10;
	static const unsigned int KeyCodeBitCount = 5;
	static const unsigned int CounterShiftTable[1 << RateBitCount];
	static const unsigned int AttenuationIncrementTable[1 << RateBitCount][8];
	static const unsigned int PmsBitCount = 3;
	static const unsigned int PhaseModIndexBitCount = 5;
	static const unsigned int PhaseModIncrementTable[1 << PmsBitCount][1 << (PhaseModIndexBitCount - 2)];
	static const unsigned int DetuneBitCount = 3;
	static const unsigned int DetunePhaseIncrementTable[1 << KeyCodeBitCount][1 << (DetuneBitCount - 1)];

	// Operator unit constants
	static const unsigned int PhaseBitCount = 10;
	static const unsigned int SinTableBitCount = 8;
	static const unsigned int PowTableBitCount = 8;
	static const unsigned int SinTableOutputBitCount = 12;
	static const unsigned int PowTableOutputBitCount = 11;
	static const unsigned int OperatorOutputBitCount = 14;
	static const unsigned int AccumulatorOutputBitCount = 16;

private:
	// Execute functions
	void RenderThread();

	// General operator functions
	void UpdateOperator(unsigned int channelNo, unsigned int operatorNo, bool updateEnvelopeGenerator);
	unsigned int CalculateKeyCode(unsigned int block, unsigned int fnumber) const;

	// Phase generator functions
	void UpdatePhaseGenerator(unsigned int channelNo, unsigned int operatorNo, unsigned int channelAddressOffset, unsigned int operatorAddressOffset);
	unsigned int GetCurrentPhase(unsigned int channelNo, unsigned int operatorNo) const;
	unsigned int GetFrequencyData(unsigned int channelNo, unsigned int operatorNo, unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	unsigned int GetBlockData(unsigned int channelNo, unsigned int operatorNo, unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;

	// Envelope generator functions
	void UpdateEnvelopeGenerator(unsigned int channelNo, unsigned int operatorNo, unsigned int channelAddressOffset, unsigned int operatorAddressOffset);
	void SetADSRPhase(unsigned int channelNo, unsigned int operatorNo, unsigned int channelAddressOffset, unsigned int operatorAddressOffset, OperatorData::ADSRPhase phase);
	unsigned int GetOutputAttenuation(unsigned int channelNo, unsigned int operatorNo, unsigned int channelAddressOffset, unsigned int operatorAddressOffset) const;
	unsigned int CalculateRate(unsigned int rateData, unsigned int rateKeyScale) const;
	unsigned int CalculateRateKeyScale(unsigned int keyScaleData, unsigned int keyCode) const;
	unsigned int ConvertTotalLevelToAttenuation(unsigned int totalLevel) const;
	unsigned int ConvertSustainLevelToAttenuation(unsigned int sustainLevel) const;

	// Operator unit functions
	unsigned int InversePow2(unsigned int num) const;
	int CalculateOperator(unsigned int phase, int phaseModulation, unsigned int attenuation) const;

	// Memory interface functions
	void RegisterSpecialUpdateFunction(unsigned int location, const Data& data, double accessTime, IDeviceContext* caller, unsigned int accessContext);
	bool CheckForLatchedWrite(unsigned int location, const Data& data, double accessTime);

	// Timer management functions
	void UpdateTimers(double timesliceProgress);

	// Raw register functions
	inline Data GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const;
	inline void SetRegisterData(unsigned int location, const Data& data, const AccessTarget& accessTarget);
	inline unsigned int GetChannelBlockAddressOffset(unsigned int channelNo) const;
	inline unsigned int GetOperatorBlockAddressOffset(unsigned int channelNo, unsigned int operatorNo) const;
	inline unsigned int GetAddressChannel3FrequencyBlock1(unsigned int operatorNo) const;
	inline unsigned int GetAddressChannel3FrequencyBlock2(unsigned int operatorNo) const;

	// Common FM register functions
	inline unsigned int GetTestData(const AccessTarget& accessTarget) const;	// 21H
	inline void SetTestData(unsigned int data, const AccessTarget& accessTarget);
	inline bool GetLFOEnabled(const AccessTarget& accessTarget) const;	// 22H
	inline void SetLFOEnabled(bool data, const AccessTarget& accessTarget);
	inline unsigned int GetLFOData(const AccessTarget& accessTarget) const;
	inline void SetLFOData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTimerAData(const AccessTarget& accessTarget) const;	// 24H-25H
	inline void SetTimerAData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTimerBData(const AccessTarget& accessTarget) const;	// 26H
	inline void SetTimerBData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetCH3Mode(const AccessTarget& accessTarget) const;	// 27H
	inline void SetCH3Mode(unsigned int data, const AccessTarget& accessTarget);
	inline bool GetTimerBReset(const AccessTarget& accessTarget) const;
	inline void SetTimerBReset(bool data, const AccessTarget& accessTarget);
	inline bool GetTimerAReset(const AccessTarget& accessTarget) const;
	inline void SetTimerAReset(bool data, const AccessTarget& accessTarget);
	inline bool GetTimerBEnable(const AccessTarget& accessTarget) const;
	inline void SetTimerBEnable(bool data, const AccessTarget& accessTarget);
	inline bool GetTimerAEnable(const AccessTarget& accessTarget) const;
	inline void SetTimerAEnable(bool data, const AccessTarget& accessTarget);
	inline bool GetTimerBLoad(const AccessTarget& accessTarget) const;
	inline void SetTimerBLoad(bool data, const AccessTarget& accessTarget);
	inline bool GetTimerALoad(const AccessTarget& accessTarget) const;
	inline void SetTimerALoad(bool data, const AccessTarget& accessTarget);
	inline unsigned int GetDACData(const AccessTarget& accessTarget) const;	// 2AH
	inline void SetDACData(unsigned int data, const AccessTarget& accessTarget);
	inline bool GetDACEnabled(const AccessTarget& accessTarget) const;	// 2BH
	inline void SetDACEnabled(bool data, const AccessTarget& accessTarget);

	// FM operator register functions
	inline unsigned int GetDetuneData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 30H-3FH
	inline void SetDetuneData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetMultipleData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetMultipleData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTotalLevelData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 40H-4FH
	inline void SetTotalLevelData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetKeyScaleData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 50H-5FH
	inline void SetKeyScaleData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetAttackRateData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetAttackRateData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetAmplitudeModulationEnabled(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 60H-6FH
	inline void SetAmplitudeModulationEnabled(unsigned int operatorAddressOffset, bool data, const AccessTarget& accessTarget);
	inline unsigned int GetDecayRateData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetDecayRateData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSustainRateData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 70H-7FH
	inline void SetSustainRateData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSustainLevelData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 80H-8FH
	inline void SetSustainLevelData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetReleaseRateData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetReleaseRateData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSSGData(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;	// 90H-9FH
	inline void SetSSGData(unsigned int operatorAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetSSGEnabled(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetSSGEnabled(unsigned int operatorAddressOffset, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGAttack(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetSSGAttack(unsigned int operatorAddressOffset, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGAlternate(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetSSGAlternate(unsigned int operatorAddressOffset, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGHold(unsigned int operatorAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetSSGHold(unsigned int operatorAddressOffset, bool data, const AccessTarget& accessTarget);

	// FM channel register functions
	inline unsigned int GetFrequencyData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;	// A0H-A7H
	inline void SetFrequencyData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetBlockData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetBlockData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetFrequencyDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const;	// A8H-AFH
	inline void SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetBlockDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetBlockDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetFeedbackData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;	// B0H-B3H
	inline void SetFeedbackData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetAlgorithmData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetAlgorithmData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetOutputLeft(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;	// B4H-B7H
	inline void SetOutputLeft(unsigned int channelAddressOffset, bool data, const AccessTarget& accessTarget);
	inline bool GetOutputRight(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetOutputRight(unsigned int channelAddressOffset, bool data, const AccessTarget& accessTarget);
	inline unsigned int GetAMSData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetAMSData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetPMSData(unsigned int channelAddressOffset, const AccessTarget& accessTarget) const;
	inline void SetPMSData(unsigned int channelAddressOffset, unsigned int data, const AccessTarget& accessTarget);

	// Status register functions
	inline Data GetStatusRegister() const;
	inline void SetStatusRegister(unsigned int data);
	inline bool GetBusyFlag() const;
	inline void SetBusyFlag(bool state);
	inline bool GetTimerBOverflow() const;
	inline void SetTimerBOverflow(bool state);
	inline bool GetTimerAOverflow() const;
	inline void SetTimerAOverflow(bool state);

	// Audio logging functions
	void SetAudioLoggingEnabled(bool state);
	void SetChannelAudioLoggingEnabled(unsigned int channelNo, bool state);
	void SetOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo, bool state);
	static bool ToggleLoggingEnabledState(Stream::WAVFile& wavFile, const std::wstring& fileName, bool currentState, bool newState, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec);

private:
	// Calculated internal lookup tables
	unsigned int sinTable[1 << SinTableBitCount];
	unsigned int powTable[1 << PowTableBitCount];

	// Clock settings
	double _externalClockRate;
	double _bexternalClockRate;
	unsigned int _fmClockDivider;
	unsigned int _egClockDivider;
	unsigned int _outputClockDivider;
	unsigned int _timerAClockDivider;
	unsigned int _timerBClockDivider;

	// Bus interface
	mutable std::mutex _lineMutex;
	IBusInterface* _memoryBus;
	volatile bool _icLineState;
	bool _bicLineState;

	// Registers
	mutable std::mutex _accessMutex;
	double _lastAccessTime;
	RandomTimeAccessBuffer<Data, double> _reg;
	unsigned int _currentReg;
	unsigned int _bcurrentReg;
	Data _status;
	Data _bstatus;

	// Temporary storage for fnum/block register latch behaviour
	bool _latchedFrequencyDataPending[ChannelCount];
	std::vector<Data> _latchedFrequencyData;
	bool _latchedFrequencyDataPendingCH3[3];
	std::vector<Data> _latchedFrequencyDataCH3;
	bool _blatchedFrequencyDataPending[ChannelCount];
	std::vector<Data> _blatchedFrequencyData;
	bool _blatchedFrequencyDataPendingCH3[3];
	std::vector<Data> _blatchedFrequencyDataCH3;

	// Timer status
	double _lastTimesliceLength;
	double _timersLastUpdateTime;
	double _timersRemainingTime;
	double _btimersRemainingTime;
	unsigned int _timerARemainingCycles;
	unsigned int _timerBRemainingCycles;
	unsigned int _btimerARemainingCycles;
	unsigned int _btimerBRemainingCycles;
	unsigned int _timerACounter;
	unsigned int _timerBCounter;
	unsigned int _btimerACounter;
	unsigned int _btimerBCounter;
	unsigned int _timerAInitialCounter;
	unsigned int _timerBInitialCounter;
	unsigned int _btimerAInitialCounter;
	unsigned int _btimerBInitialCounter;
	bool _timerAEnable;
	bool _timerBEnable;
	bool _btimerAEnable;
	bool _btimerBEnable;
	bool _timerALoad;
	bool _timerBLoad;
	bool _btimerALoad;
	bool _btimerBLoad;
	bool _irqLineState;
	bool _birqLineState;

	// This register records the times at which Timer A overflows. It is needed in order to
	// implement CSM support.
	RandomTimeAccessValue<bool, double> _timerAOverflowTimes;

	// Render thread properties
	mutable std::mutex _renderThreadMutex;
	mutable std::mutex _timesliceMutex;
	std::condition_variable _renderThreadUpdate;
	std::condition_variable _renderThreadStopped;
	bool _renderThreadActive;
	static const unsigned int maxPendingRenderOperationCount = 4;
	bool _renderThreadLagging;
	std::condition_variable _renderThreadLaggingStateChange;
	unsigned int _pendingRenderOperationCount;
	std::list<RandomTimeAccessBuffer<Data, double>::Timeslice> _regTimesliceList;
	std::list<RandomTimeAccessValue<bool, double>::Timeslice> _timerATimesliceList;
	std::list<RandomTimeAccessBuffer<Data, double>::Timeslice> _regTimesliceListUncommitted;
	std::list<RandomTimeAccessValue<bool, double>::Timeslice> _timerATimesliceListUncommitted;
	double _remainingRenderTime;
	int _egRemainingRenderCycles;
	unsigned int _outputSampleRate;
	AudioStream _outputStream;
	std::vector<short> _outputBuffer;

	// Render data
	unsigned int _envelopeCycleCounter;
	OperatorData _operatorData[ChannelCount][OperatorCount];
	int _operatorOutput[ChannelCount][OperatorCount];
	int _feedbackBuffer[ChannelCount][2];
	int _cyclesUntilLFOIncrement;
	unsigned int _currentLFOCounter;

	// Register locking
	mutable std::mutex _registerLockMutex;
	bool _keyStateLocking[ChannelCount][OperatorCount];
	bool _rawRegisterLocking[RegisterCountTotal];
	std::map<unsigned int, std::list<RegisterLocking>> _lockedRegisterState;
	TimerStateLocking _timerAStateLocking;
	TimerStateLocking _timerBStateLocking;

	// Wave logging
	mutable std::mutex _waveLoggingMutex;
	bool _wavLoggingEnabled;
	bool _wavLoggingChannelEnabled[ChannelCount];
	bool _wavLoggingOperatorEnabled[ChannelCount][OperatorCount];
	std::wstring _wavLoggingPath;
	std::wstring _wavLoggingChannelPath[ChannelCount];
	std::wstring _wavLoggingOperatorPath[ChannelCount][OperatorCount];
	Stream::WAVFile _wavLog;
	Stream::WAVFile _wavLogChannel[ChannelCount];
	Stream::WAVFile _wavLogOperator[ChannelCount][OperatorCount];
};

#include "YM2612.inl"
#endif
