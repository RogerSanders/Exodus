#ifndef __IYM2612_H__
#define __IYM2612_H__
#include "GenericAccess/GenericAccess.pkg"
#include <string>

class IYM2612 :public virtual IGenericAccess
{
public:
	// Enumerations
	enum class IYM2612DataSource;
	enum Channels :unsigned int;
	enum Operators :unsigned int;

	// Structures
	struct RegisterDataContext;
	struct ChannelDataContext;
	struct OperatorDataContext;

	// Constants
	static const unsigned int ChannelCount = 6;
	static const unsigned int OperatorCount = 4;
	static const unsigned int PartCount = 2;
	static const unsigned int RegisterCountPerPart = 0x100;
	static const unsigned int RegisterCountTotal = RegisterCountPerPart * PartCount;

public:
	// Interface version functions
	static inline unsigned int ThisIYM2612Version() { return 1; }
	virtual unsigned int GetIYM2612Version() const = 0;

	// Clock setting functions
	inline double GetExternalClockRate() const;
	inline void SetExternalClockRate(double data);
	inline unsigned int GetFMClockDivider() const;
	inline void SetFMClockDivider(unsigned int data);
	inline unsigned int GetEGClockDivider() const;
	inline void SetEGClockDivider(unsigned int data);
	inline unsigned int GetOutputClockDivider() const;
	inline void SetOutputClockDivider(unsigned int data);
	inline unsigned int GetTimerAClockDivider() const;
	inline void SetTimerAClockDivider(unsigned int data);
	inline unsigned int GetTimerBClockDivider() const;
	inline void SetTimerBClockDivider(unsigned int data);

	// Raw register functions
	inline unsigned int GetRegisterData(unsigned int registerNo) const;
	inline void SetRegisterData(unsigned int registerNo, unsigned int data);

	// Common FM register functions
	inline unsigned int GetTestData() const;
	inline void SetTestData(unsigned int data);
	inline bool GetLFOEnabled() const;
	inline void SetLFOEnabled(bool data);
	inline unsigned int GetLFOData() const;
	inline void SetLFOData(unsigned int data);
	inline unsigned int GetTimerAData() const;
	inline void SetTimerAData(unsigned int data);
	inline unsigned int GetTimerBData() const;
	inline void SetTimerBData(unsigned int data);
	inline unsigned int GetCH3Mode() const;
	inline void SetCH3Mode(unsigned int data);
	inline bool GetTimerBReset() const;
	inline void SetTimerBReset(bool data);
	inline bool GetTimerAReset() const;
	inline void SetTimerAReset(bool data);
	inline bool GetTimerBEnable() const;
	inline void SetTimerBEnable(bool data);
	inline bool GetTimerAEnable() const;
	inline void SetTimerAEnable(bool data);
	inline bool GetTimerBLoad() const;
	inline void SetTimerBLoad(bool data);
	inline bool GetTimerALoad() const;
	inline void SetTimerALoad(bool data);
	inline unsigned int GetDACData() const;
	inline void SetDACData(unsigned int data);
	inline bool GetDACEnabled() const;
	inline void SetDACEnabled(bool data);

	// FM operator register functions
	inline unsigned int GetDetuneData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetDetuneData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetMultipleData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetMultipleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetTotalLevelData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetKeyScaleData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetAttackRateData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetAttackRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline bool GetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, bool data);
	inline unsigned int GetDecayRateData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetDecayRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetSustainRateData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSustainRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetSustainLevelData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetReleaseRateData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline unsigned int GetSSGData(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSSGData(unsigned int channelNo, unsigned int operatorNo, unsigned int data);
	inline bool GetSSGEnabled(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, bool data);
	inline bool GetSSGAttack(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSSGAttack(unsigned int channelNo, unsigned int operatorNo, bool data);
	inline bool GetSSGAlternate(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, bool data);
	inline bool GetSSGHold(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetSSGHold(unsigned int channelNo, unsigned int operatorNo, bool data);

	// FM channel register functions
	inline unsigned int GetFrequencyData(unsigned int channelNo) const;
	inline void SetFrequencyData(unsigned int channelNo, unsigned int data);
	inline unsigned int GetBlockData(unsigned int channelNo) const;
	inline void SetBlockData(unsigned int channelNo, unsigned int data);
	inline unsigned int GetFrequencyDataChannel3(unsigned int operatorNo) const;
	inline void SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int data);
	inline unsigned int GetBlockDataChannel3(unsigned int operatorNo) const;
	inline void SetBlockDataChannel3(unsigned int operatorNo, unsigned int data);
	inline unsigned int GetFeedbackData(unsigned int channelNo) const;
	inline void SetFeedbackData(unsigned int channelNo, unsigned int data);
	inline unsigned int GetAlgorithmData(unsigned int channelNo) const;
	inline void SetAlgorithmData(unsigned int channelNo, unsigned int data);
	inline bool GetOutputLeft(unsigned int channelNo) const;
	inline void SetOutputLeft(unsigned int channelNo, bool data);
	inline bool GetOutputRight(unsigned int channelNo) const;
	inline void SetOutputRight(unsigned int channelNo, bool data);
	inline unsigned int GetAMSData(unsigned int channelNo) const;
	inline void SetAMSData(unsigned int channelNo, unsigned int data);
	inline unsigned int GetPMSData(unsigned int channelNo) const;
	inline void SetPMSData(unsigned int channelNo, unsigned int data);

	// Key on/off functions
	inline bool GetKeyState(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetKeyState(unsigned int channelNo, unsigned int operatorNo, bool data);

	// Status register functions
	inline unsigned int GetStatusRegister() const;
	inline void SetStatusRegister(unsigned int data);
	inline bool GetBusyFlag() const;
	inline void SetBusyFlag(bool data);
	inline bool GetTimerBOverflow() const;
	inline void SetTimerBOverflow(bool data);
	inline bool GetTimerAOverflow() const;
	inline void SetTimerAOverflow(bool data);

	// Timer functions
	inline unsigned int GetTimerACurrentCounter() const;
	inline void SetTimerACurrentCounter(unsigned int data);
	inline unsigned int GetTimerBCurrentCounter() const;
	inline void SetTimerBCurrentCounter(unsigned int data);

	// Audio logging functions
	inline bool IsAudioLoggingEnabled() const;
	inline void SetAudioLoggingEnabled(bool data);
	inline bool IsChannelAudioLoggingEnabled(unsigned int channelNo) const;
	inline void SetChannelAudioLoggingEnabled(unsigned int channelNo, bool data);
	inline bool IsOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo, bool data);
	inline std::wstring GetAudioLoggingOutputPath() const;
	inline void SetAudioLoggingOutputPath(const std::wstring& data);
	inline std::wstring GetChannelAudioLoggingOutputPath(unsigned int channelNo) const;
	inline void SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& data);
	inline std::wstring GetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo) const;
	inline void SetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo, const std::wstring& data);
};

#include "IYM2612.inl"
#endif
