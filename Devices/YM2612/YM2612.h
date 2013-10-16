/*--------------------------------------------------------------------------------------*\
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
\*--------------------------------------------------------------------------------------*/
#ifndef __YM2612_H__
#define __YM2612_H__
#include "SystemInterface/SystemInterface.pkg"
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <list>
#include <set>
#include <map>
#include "Device/Device.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include "AudioStream/AudioStream.pkg"
#include "Stream/Stream.pkg"

class YM2612 :public Device
{
public:
	//Constructors
	YM2612(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~YM2612();

	//Initialization functions
	virtual bool BuildDevice();
	virtual bool ValidateDevice();
	virtual void Initialize();
	void Reset(double accessTime);

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Clock source functions
	virtual unsigned int GetClockSourceID(const std::wstring& clockSourceName) const;
	virtual std::wstring GetClockSourceName(unsigned int clockSourceID) const;
	virtual void SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetClockSourceRate(unsigned int clockInput, double clockRate);

	//Execute functions
	virtual void BeginExecution();
	virtual void SuspendExecution();
	virtual UpdateMethod GetUpdateMethod() const;
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

	//Window functions
	void CreateMenuHandlers();
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

private:
	//Enumerations
	enum LineID;
	enum ClockID;
	enum AccessContext;
	enum LockedRegisterKey;
	enum TimerParam;
	enum Channels;
	enum Operators;

	//Structures
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
		{}

		Data attenuation;   //10-bit
		Data phaseCounter;  //20-bit
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

	//View and menu classes
	class DebugMenuHandler;
	class DebuggerViewModel;
	class RegistersViewModel;
	class LoggingViewModel;
	class OperatorViewModel;
	class DebuggerView;
	class RegistersView;
	class LoggingView;
	class OperatorView;
	friend class DebuggerViewModel;
	friend class RegistersViewModel;
	friend class LoggingViewModel;
	friend class OperatorViewModel;
	friend class DebuggerView;
	friend class RegistersView;
	friend class LoggingView;
	friend class OperatorView;

	//Typedefs
	typedef RandomTimeAccessBuffer<Data, double>::AccessTarget AccessTarget;

private:
	//Execute functions
	void RenderThread();

	//General operator functions
	void UpdateOperator(unsigned int channelNo, unsigned int operatorNo, bool updateEnvelopeGenerator);
	unsigned int CalculateKeyCode(unsigned int block, unsigned int fnumber) const;

	//Phase generator functions
	void UpdatePhaseGenerator(unsigned int channelNo, unsigned int operatorNo);
	unsigned int GetCurrentPhase(unsigned int channelNo, unsigned int operatorNo) const;
	unsigned int GetFrequencyData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	unsigned int GetBlockData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;

	//Envelope generator functions
	void UpdateEnvelopeGenerator(unsigned int channelNo, unsigned int operatorNo);
	void SetADSRPhase(unsigned int channelNo, unsigned int operatorNo, OperatorData::ADSRPhase phase);
	unsigned int GetOutputAttenuation(unsigned int channelNo, unsigned int operatorNo) const;
	unsigned int CalculateRate(unsigned int rateData, unsigned int rateKeyScale) const;
	unsigned int CalculateRateKeyScale(unsigned int keyScaleData, unsigned int keyCode) const;
	unsigned int ConvertTotalLevelToAttenuation(unsigned int totalLevel) const;
	unsigned int ConvertSustainLevelToAttenuation(unsigned int sustainLevel) const;

	//Operator unit functions
	unsigned int InversePow2(unsigned int num) const;
	int CalculateOperator(unsigned int phase, int phaseModulation, unsigned int attenuation) const;

	//Memory interface functions
	void RegisterSpecialUpdateFunction(unsigned int location, const Data& data, double accessTime, IDeviceContext* caller, unsigned int accessContext);
	bool CheckForLatchedWrite(unsigned int location, const Data& data, double accessTime);

	//Timer management functions
	void UpdateTimers(double timesliceProgress);

	//Raw register functions
	inline Data GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const;
	inline void SetRegisterData(unsigned int location, const Data& data, const AccessTarget& accessTarget);
	inline unsigned int AdjustAddressChannelBlock(unsigned int address, unsigned int channelNo) const;
	inline unsigned int AdjustAddressOperatorBlock(unsigned int address, unsigned int channelNo, unsigned int operatorNo) const;
	inline unsigned int GetAddressChannel3FrequencyBlock1(unsigned int operatorNo) const;
	inline unsigned int GetAddressChannel3FrequencyBlock2(unsigned int operatorNo) const;

	//Common FM register functions
	inline unsigned int GetTestData(const AccessTarget& accessTarget) const;	//21H
	inline void SetTestData(unsigned int data, const AccessTarget& accessTarget);
	inline bool GetLFOEnabled(const AccessTarget& accessTarget) const;	//22H
	inline void SetLFOEnabled(bool data, const AccessTarget& accessTarget);
	inline unsigned int GetLFOData(const AccessTarget& accessTarget) const;
	inline void SetLFOData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTimerAData(const AccessTarget& accessTarget) const;	//24H-25H
	inline void SetTimerAData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTimerBData(const AccessTarget& accessTarget) const;	//26H
	inline void SetTimerBData(unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetCH3Mode(const AccessTarget& accessTarget) const;	//27H
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
	inline unsigned int GetDACData(const AccessTarget& accessTarget) const;	//2AH
	inline void SetDACData(unsigned int data, const AccessTarget& accessTarget);
	inline bool GetDACEnabled(const AccessTarget& accessTarget) const;	//2BH
	inline void SetDACEnabled(bool data, const AccessTarget& accessTarget);

	//FM operator register functions
	inline unsigned int GetDetuneData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//30H-3FH
	inline void SetDetuneData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetMultipleData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetMultipleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//40H-4FH
	inline void SetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//50H-5FH
	inline void SetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetAttackRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetAttackRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//60H-6FH
	inline void SetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget);
	inline unsigned int GetDecayRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetDecayRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSustainRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//70H-7FH
	inline void SetSustainRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//80H-8FH
	inline void SetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetSSGData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;	//90H-9FH
	inline void SetSSGData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGAttack(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetSSGAttack(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget);
	inline bool GetSSGHold(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetSSGHold(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget);

	//FM channel register functions
	inline unsigned int GetFrequencyData(unsigned int channel, const AccessTarget& accessTarget) const;	//A0H-A7H
	inline void SetFrequencyData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetBlockData(unsigned int channel, const AccessTarget& accessTarget) const;
	inline void SetBlockData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetFrequencyDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const;	//A8H-AFH
	inline void SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetBlockDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const;
	inline void SetBlockDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetFeedbackData(unsigned int channel, const AccessTarget& accessTarget) const;	//B0H-B3H
	inline void SetFeedbackData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetAlgorithmData(unsigned int channel, const AccessTarget& accessTarget) const;
	inline void SetAlgorithmData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);
	inline bool GetOutputLeft(unsigned int channel, const AccessTarget& accessTarget) const;	//B4H-B7H
	inline void SetOutputLeft(unsigned int channel, bool data, const AccessTarget& accessTarget);
	inline bool GetOutputRight(unsigned int channel, const AccessTarget& accessTarget) const;
	inline void SetOutputRight(unsigned int channel, bool data, const AccessTarget& accessTarget);
	inline unsigned int GetAMSData(unsigned int channel, const AccessTarget& accessTarget) const;
	inline void SetAMSData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);
	inline unsigned int GetPMSData(unsigned int channel, const AccessTarget& accessTarget) const;
	inline void SetPMSData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget);

	//Status register functions
	inline Data GetStatus() const;
	inline bool GetBusyFlag() const;
	inline void SetBusyFlag(bool astate);
	inline bool GetTimerBOverflow() const;
	inline void SetTimerBOverflow(bool astate);
	inline bool GetTimerAOverflow() const;
	inline void SetTimerAOverflow(bool astate);

	//Register locking
	void LockRegister(LockedRegisterKey key, const boost::function<void()>& function);
	void UnlockRegister(LockedRegisterKey key);
	bool IsRegisterLocked(LockedRegisterKey key);
	bool GetKeyStateLocked(unsigned int channelNo, unsigned int operatorNo);
	void SetKeyStateLocked(unsigned int channelNo, unsigned int operatorNo, bool state);
	bool GetTimerParamLocked(TimerParam param);
	void SetTimerParamLocked(TimerParam param, bool state);

	//Window functions
	void OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo);

private:
	//Constants
	static const unsigned int partCount = 2;
	static const unsigned int registerCountPerPart = 0x100;
	static const unsigned int registerCountTotal = registerCountPerPart * partCount;
	static const unsigned int channelCount = 6;
	static const unsigned int operatorCount = 4;

	//Envelope generator constants
	static const unsigned int rateBitCount = 6;
	static const unsigned int attenuationBitCount = 10;
	static const unsigned int fnumDataBitCount = 11;
	static const unsigned int phaseGeneratorOutputBitCount = 10;
	static const unsigned int keyCodeBitCount = 5;
	static const unsigned int counterShiftTable[1 << rateBitCount];
	static const unsigned int attenuationIncrementTable[1 << rateBitCount][8];
	static const unsigned int pmsBitCount = 3;
	static const unsigned int phaseModIndexBitCount = 5;
	static const unsigned int phaseModIncrementTable[1 << pmsBitCount][1 << (phaseModIndexBitCount - 2)];
	static const unsigned int detuneBitCount = 3;
	static const unsigned int detunePhaseIncrementTable[1 << keyCodeBitCount][1 << (detuneBitCount - 1)];

	//Operator unit constants
	static const unsigned int phaseBitCount = 10;
	static const unsigned int sinTableBitCount = 8;
	static const unsigned int powTableBitCount = 8;
	static const unsigned int sinTableOutputBitCount = 12;
	static const unsigned int powTableOutputBitCount = 11;
	static const unsigned int operatorOutputBitCount = 14;
	static const unsigned int accumulatorOutputBitCount = 16;
	unsigned int sinTable[1 << sinTableBitCount];
	unsigned int powTable[1 << powTableBitCount];

	//Clock settings
	double externalClockRate;
	double bexternalClockRate;
	unsigned int fmClockDivider;
	unsigned int egClockDivider;
	unsigned int outputClockDivider;
	unsigned int timerAClockDivider;
	unsigned int timerBClockDivider;

	//Menu handling
	DebugMenuHandler* menuHandler;

	//Bus interface
	mutable boost::mutex lineMutex;
	IBusInterface* memoryBus;
	volatile bool icLineState;
	bool bicLineState;

	//Registers
	mutable boost::mutex accessMutex;
	double lastAccessTime;
	RandomTimeAccessBuffer<Data, double> reg;
	unsigned int currentReg;
	unsigned int bcurrentReg;
	Data status;
	Data bstatus;

	//Temporary storage for fnum/block register latch behaviour
	bool latchedFrequencyDataPending[channelCount];
	std::vector<Data> latchedFrequencyData;
	bool latchedFrequencyDataPendingCH3[3];
	std::vector<Data> latchedFrequencyDataCH3;
	bool blatchedFrequencyDataPending[channelCount];
	std::vector<Data> blatchedFrequencyData;
	bool blatchedFrequencyDataPendingCH3[3];
	std::vector<Data> blatchedFrequencyDataCH3;

	//Timer status
	double lastTimesliceLength;
	double timersLastUpdateTime;
	double timersRemainingTime;
	double btimersRemainingTime;
	unsigned int timerARemainingCycles;
	unsigned int timerBRemainingCycles;
	unsigned int btimerARemainingCycles;
	unsigned int btimerBRemainingCycles;
	unsigned int timerACounter;
	unsigned int timerBCounter;
	unsigned int btimerACounter;
	unsigned int btimerBCounter;
	unsigned int timerAInitialCounter;
	unsigned int timerBInitialCounter;
	unsigned int btimerAInitialCounter;
	unsigned int btimerBInitialCounter;
	bool timerAEnable;
	bool timerBEnable;
	bool btimerAEnable;
	bool btimerBEnable;
	bool timerALoad;
	bool timerBLoad;
	bool btimerALoad;
	bool btimerBLoad;
	bool irqLineState;
	bool birqLineState;

	//This register records the times at which Timer A overflows. It is needed in order
	//to implement CSM support.
	RandomTimeAccessValue<bool, double> timerAOverflowTimes;

	//Render thread properties
	mutable boost::mutex renderThreadMutex;
	mutable boost::mutex timesliceMutex;
	boost::condition renderThreadUpdate;
	boost::condition renderThreadStopped;
	bool renderThreadActive;
	static const unsigned int maxPendingRenderOperationCount = 4;
	bool renderThreadLagging;
	boost::condition renderThreadLaggingStateChange;
	unsigned int pendingRenderOperationCount;
	std::list<RandomTimeAccessBuffer<Data, double>::Timeslice> regTimesliceList;
	std::list<RandomTimeAccessValue<bool, double>::Timeslice> timerATimesliceList;
	std::list<RandomTimeAccessBuffer<Data, double>::Timeslice> regTimesliceListUncommitted;
	std::list<RandomTimeAccessValue<bool, double>::Timeslice> timerATimesliceListUncommitted;
	double remainingRenderTime;
	int egRemainingRenderCycles;
	unsigned int outputSampleRate;
	AudioStream outputStream;
	std::vector<short> outputBuffer;

	//Render data
	unsigned int envelopeCycleCounter;
	OperatorData operatorData[channelCount][operatorCount];
	int operatorOutput[channelCount][operatorCount];
	int feedbackBuffer[channelCount][2];
	int cyclesUntilLFOIncrement;
	unsigned int currentLFOCounter;

	//Register locking
	mutable boost::mutex registerLockMutex;
	typedef std::pair<LockedRegisterKey, boost::function<void()> > LockedRegisterListEntry;
	typedef std::map<LockedRegisterKey, boost::function<void()> > LockedRegisterList;
	LockedRegisterList lockedRegisters;
	std::set<LockedRegisterKey> lockedRegisterIndex;
	bool keyStateLocking[channelCount][operatorCount];
	TimerStateLocking timerAStateLocking;
	TimerStateLocking timerBStateLocking;

	//Wave logging
	mutable boost::mutex waveLoggingMutex;
	bool wavLoggingEnabled;
	bool wavLoggingChannelEnabled[channelCount];
	bool wavLoggingOperatorEnabled[channelCount][operatorCount];
	std::wstring wavLoggingPath;
	std::wstring wavLoggingChannelPath[channelCount];
	std::wstring wavLoggingOperatorPath[channelCount][operatorCount];
	Stream::WAVFile wavLog;
	Stream::WAVFile wavLogChannel[channelCount];
	Stream::WAVFile wavLogOperator[channelCount][operatorCount];
};

#include "YM2612.inl"
#endif
