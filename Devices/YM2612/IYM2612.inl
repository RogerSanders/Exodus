//----------------------------------------------------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------------------------------------------------
enum class IYM2612::IYM2612DataSource
{
	RawRegister = 1,

	TestData,
	LFOEnabled,
	LFOData,
	TimerAData,
	TimerBData,
	CH3Mode,
	TimerBReset,
	TimerAReset,
	TimerBEnable,
	TimerAEnable,
	TimerBLoad,
	TimerALoad,
	DACData,
	DACEnabled,
	DetuneData,
	MultipleData,
	TotalLevelData,
	KeyScaleData,
	AttackRateData,
	AmplitudeModulationEnabled,
	DecayRateData,
	SustainRateData,
	SustainLevelData,
	ReleaseRateData,
	SSGData,
	SSGEnabled,
	SSGAttack,
	SSGAlternate,
	SSGHold,

	FrequencyData,
	BlockData,
	FrequencyDataChannel3,
	BlockDataChannel3,
	FeedbackData,
	AlgorithmData,
	OutputLeft,
	OutputRight,
	AMSData,
	PMSData,

	KeyState,

	StatusRegister,
	BusyFlag,
	TimerBOverflow,
	TimerAOverflow,

	TimerACurrentCounter,
	TimerBCurrentCounter,

	ExternalClockRate,
	FMClockDivider,
	EGClockDivider,
	OutputClockDivider,
	TimerAClockDivider,
	TimerBClockDivider,

	AudioLoggingEnabled,
	AudioLoggingPath,
	ChannelAudioLoggingEnabled,
	ChannelAudioLoggingPath,
	OperatorAudioLoggingEnabled,
	OperatorAudioLoggingPath
};

//----------------------------------------------------------------------------------------------------------------------
enum IYM2612::Channels :unsigned int
{
	CHANNEL1 = 0,
	CHANNEL2 = 1,
	CHANNEL3 = 2,
	CHANNEL4 = 3,
	CHANNEL5 = 4,
	CHANNEL6 = 5
};

//----------------------------------------------------------------------------------------------------------------------
enum IYM2612::Operators:unsigned int
{
	OPERATOR1 = 0,
	OPERATOR2 = 1,
	OPERATOR3 = 2,
	OPERATOR4 = 3
};

//----------------------------------------------------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------------------------------------------------
struct IYM2612::RegisterDataContext :public IGenericAccess::DataContext
{
	RegisterDataContext(unsigned int aregisterNo = 0)
	:registerNo(aregisterNo)
	{}

	unsigned int registerNo;
};

//----------------------------------------------------------------------------------------------------------------------
struct IYM2612::ChannelDataContext :public IGenericAccess::DataContext
{
	ChannelDataContext(unsigned int achannelNo = 0)
	:channelNo(achannelNo)
	{}

	unsigned int channelNo;
};

//----------------------------------------------------------------------------------------------------------------------
struct IYM2612::OperatorDataContext :public IGenericAccess::DataContext
{
	OperatorDataContext(unsigned int achannelNo = 0, unsigned int aoperatorNo = 0)
	:channelNo(achannelNo), operatorNo(aoperatorNo)
	{}

	unsigned int channelNo;
	unsigned int operatorNo;
};

//----------------------------------------------------------------------------------------------------------------------
// Clock setting functions
//----------------------------------------------------------------------------------------------------------------------
double IYM2612::GetExternalClockRate() const
{
	GenericAccessDataValueDouble genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::ExternalClockRate, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetExternalClockRate(double data)
{
	GenericAccessDataValueDouble genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::ExternalClockRate, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetFMClockDivider() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::FMClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetFMClockDivider(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::FMClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetEGClockDivider() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::EGClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetEGClockDivider(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::EGClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetOutputClockDivider() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::OutputClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetOutputClockDivider(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::OutputClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerAClockDivider() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerAClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerAClockDivider(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerAClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBClockDivider() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBClockDivider(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Raw register functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetRegisterData(unsigned int registerNo) const
{
	GenericAccessDataValueUInt genericData;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)IYM2612DataSource::RawRegister, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetRegisterData(unsigned int registerNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)IYM2612DataSource::RawRegister, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Register functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTestData() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TestData, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTestData(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TestData, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetLFOEnabled() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::LFOEnabled, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetLFOEnabled(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::LFOEnabled, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetLFOData() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::LFOData, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetLFOData(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::LFOData, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerAData() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerAData, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerAData(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerAData, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBData() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBData, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBData(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBData, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetCH3Mode() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::CH3Mode, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetCH3Mode(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::CH3Mode, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerBReset() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBReset, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBReset(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBReset, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerAReset() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerAReset, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerAReset(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerAReset, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerBEnable() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBEnable, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBEnable(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBEnable, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerAEnable() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerAEnable, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerAEnable(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerAEnable, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerBLoad() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBLoad, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBLoad(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBLoad, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerALoad() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerALoad, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerALoad(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerALoad, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetDACData() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::DACData, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetDACData(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::DACData, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetDACEnabled() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::DACEnabled, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetDACEnabled(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::DACEnabled, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetDetuneData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::DetuneData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetDetuneData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::DetuneData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetMultipleData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::MultipleData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetMultipleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::MultipleData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTotalLevelData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::TotalLevelData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::TotalLevelData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetKeyScaleData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::KeyScaleData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::KeyScaleData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetAttackRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::AttackRateData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAttackRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::AttackRateData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::AmplitudeModulationEnabled, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::AmplitudeModulationEnabled, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetDecayRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::DecayRateData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetDecayRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::DecayRateData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetSustainRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SustainRateData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSustainRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SustainRateData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetSustainLevelData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SustainLevelData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SustainLevelData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetReleaseRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::ReleaseRateData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::ReleaseRateData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetSSGData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SSGData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSSGData(unsigned int channelNo, unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SSGData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetSSGEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SSGEnabled, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SSGEnabled, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetSSGAttack(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SSGAttack, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSSGAttack(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SSGAttack, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetSSGAlternate(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SSGAlternate, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SSGAlternate, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetSSGHold(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::SSGHold, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetSSGHold(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::SSGHold, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetFrequencyData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::FrequencyData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetFrequencyData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::FrequencyData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetBlockData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::BlockData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetBlockData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::BlockData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetFrequencyDataChannel3(unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::FrequencyDataChannel3, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::FrequencyDataChannel3, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetBlockDataChannel3(unsigned int operatorNo) const
{
	GenericAccessDataValueUInt genericData;
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::BlockDataChannel3, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetBlockDataChannel3(unsigned int operatorNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::BlockDataChannel3, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetFeedbackData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::FeedbackData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetFeedbackData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::FeedbackData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetAlgorithmData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::AlgorithmData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAlgorithmData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::AlgorithmData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetOutputLeft(unsigned int channelNo) const
{
	GenericAccessDataValueBool genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::OutputLeft, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetOutputLeft(unsigned int channelNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::OutputLeft, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetOutputRight(unsigned int channelNo) const
{
	GenericAccessDataValueBool genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::OutputRight, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetOutputRight(unsigned int channelNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::OutputRight, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetAMSData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::AMSData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAMSData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::AMSData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetPMSData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::PMSData, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetPMSData(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::PMSData, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Key on/off functions
//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetKeyState(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::KeyState, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetKeyState(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::KeyState, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Status register functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetStatusRegister() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::StatusRegister, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetStatusRegister(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::StatusRegister, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetBusyFlag() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::BusyFlag, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetBusyFlag(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::BusyFlag, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerBOverflow() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBOverflow, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBOverflow(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBOverflow, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::GetTimerAOverflow() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerAOverflow, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerAOverflow(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerAOverflow, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Timer functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerACurrentCounter() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerACurrentCounter, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerACurrentCounter(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerACurrentCounter, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBCurrentCounter() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::TimerBCurrentCounter, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetTimerBCurrentCounter(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::TimerBCurrentCounter, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
// Audio logging functions
//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::IsAudioLoggingEnabled() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::AudioLoggingEnabled, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAudioLoggingEnabled(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::AudioLoggingEnabled, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::IsChannelAudioLoggingEnabled(unsigned int channelNo) const
{
	GenericAccessDataValueBool genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::ChannelAudioLoggingEnabled, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::ChannelAudioLoggingEnabled, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
bool IYM2612::IsOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::OperatorAudioLoggingEnabled, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::OperatorAudioLoggingEnabled, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring IYM2612::GetAudioLoggingOutputPath() const
{
	GenericAccessDataValueFilePath genericData;
	ReadGenericData((unsigned int)IYM2612DataSource::AudioLoggingPath, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetAudioLoggingOutputPath(const std::wstring& data)
{
	GenericAccessDataValueFilePath genericData(data);
	WriteGenericData((unsigned int)IYM2612DataSource::AudioLoggingPath, 0, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring IYM2612::GetChannelAudioLoggingOutputPath(unsigned int channelNo) const
{
	GenericAccessDataValueFilePath genericData;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)IYM2612DataSource::ChannelAudioLoggingPath, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& data)
{
	GenericAccessDataValueFilePath genericData(data);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)IYM2612DataSource::ChannelAudioLoggingPath, &dataContext, genericData);
}

//----------------------------------------------------------------------------------------------------------------------
std::wstring IYM2612::GetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueFilePath genericData;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)IYM2612DataSource::OperatorAudioLoggingPath, &dataContext, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------------------------------------
void IYM2612::SetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo, const std::wstring& data)
{
	GenericAccessDataValueFilePath genericData(data);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)IYM2612DataSource::OperatorAudioLoggingPath, &dataContext, genericData);
}
