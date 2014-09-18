//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IYM2612::IYM2612DataSource
{
	DATASOURCE_RAWREGISTER = 1,

	DATASOURCE_TESTDATA,
	DATASOURCE_LFOENABLED,
	DATASOURCE_LFODATA,
	DATASOURCE_TIMERADATA,
	DATASOURCE_TIMERBDATA,
	DATASOURCE_CH3MODE,
	DATASOURCE_TIMERBRESET,
	DATASOURCE_TIMERARESET,
	DATASOURCE_TIMERBENABLE,
	DATASOURCE_TIMERAENABLE,
	DATASOURCE_TIMERBLOAD,
	DATASOURCE_TIMERALOAD,
	DATASOURCE_DACDATA,
	DATASOURCE_DACENABLED,
	DATASOURCE_DETUNEDATA,
	DATASOURCE_MULTIPLEDATA,
	DATASOURCE_TOTALLEVELDATA,
	DATASOURCE_KEYSCALEDATA,
	DATASOURCE_ATTACKRATEDATA,
	DATASOURCE_AMPLITUDEMODULATIONENABLED,
	DATASOURCE_DECAYRATEDATA,
	DATASOURCE_SUSTAINRATEDATA,
	DATASOURCE_SUSTAINLEVELDATA,
	DATASOURCE_RELEASERATEDATA,
	DATASOURCE_SSGDATA,
	DATASOURCE_SSGENABLED,
	DATASOURCE_SSGATTACK,
	DATASOURCE_SSGALTERNATE,
	DATASOURCE_SSGHOLD,

	DATASOURCE_FREQUENCYDATA,
	DATASOURCE_BLOCKDATA,
	DATASOURCE_FREQUENCYDATACHANNEL3,
	DATASOURCE_BLOCKDATACHANNEL3,
	DATASOURCE_FEEDBACKDATA,
	DATASOURCE_ALGORITHMDATA,
	DATASOURCE_OUTPUTLEFT,
	DATASOURCE_OUTPUTRIGHT,
	DATASOURCE_AMSDATA,
	DATASOURCE_PMSDATA,

	DATASOURCE_KEYSTATE,

	DATASOURCE_STATUSREGISTER,
	DATASOURCE_BUSYFLAG,
	DATASOURCE_TIMERBOVERFLOW,
	DATASOURCE_TIMERAOVERFLOW,

	DATASOURCE_TIMERACURRENTCOUNTER,
	DATASOURCE_TIMERBCURRENTCOUNTER,

	DATASOURCE_EXTERNALCLOCKRATE,
	DATASOURCE_FMCLOCKDIVIDER,
	DATASOURCE_EGCLOCKDIVIDER,
	DATASOURCE_OUTPUTCLOCKDIVIDER,
	DATASOURCE_TIMERACLOCKDIVIDER,
	DATASOURCE_TIMERBCLOCKDIVIDER,

	DATASOURCE_AUDIOLOGGINGENABLED,
	DATASOURCE_AUDIOLOGGINGPATH,
	DATASOURCE_CHANNELAUDIOLOGGINGENABLED,
	DATASOURCE_CHANNELAUDIOLOGGINGPATH,
	DATASOURCE_OPERATORAUDIOLOGGINGENABLED,
	DATASOURCE_OPERATORAUDIOLOGGINGPATH
};

//----------------------------------------------------------------------------------------
enum IYM2612::Channels
{
	CHANNEL1 = 0,
	CHANNEL2 = 1,
	CHANNEL3 = 2,
	CHANNEL4 = 3,
	CHANNEL5 = 4,
	CHANNEL6 = 5
};

//----------------------------------------------------------------------------------------
enum IYM2612::Operators
{
	OPERATOR1 = 0,
	OPERATOR2 = 1,
	OPERATOR3 = 2,
	OPERATOR4 = 3
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IYM2612::RegisterDataContext :public IGenericAccess::DataContext
{
	RegisterDataContext(unsigned int aregisterNo = 0)
	:registerNo(aregisterNo)
	{}

	unsigned int registerNo;
};

//----------------------------------------------------------------------------------------
struct IYM2612::ChannelDataContext :public IGenericAccess::DataContext
{
	ChannelDataContext(unsigned int achannelNo = 0)
	:channelNo(achannelNo)
	{}

	unsigned int channelNo;
};

//----------------------------------------------------------------------------------------
struct IYM2612::OperatorDataContext :public IGenericAccess::DataContext
{
	OperatorDataContext(unsigned int achannelNo = 0, unsigned int aoperatorNo = 0)
	:channelNo(achannelNo), operatorNo(aoperatorNo)
	{}

	unsigned int channelNo;
	unsigned int operatorNo;
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IYM2612::ThisIYM2612Version()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Clock setting functions
//----------------------------------------------------------------------------------------
double IYM2612::GetExternalClockRate() const
{
	GenericAccessDataValueDouble data;
	ReadGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKRATE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetExternalClockRate(double adata)
{
	GenericAccessDataValueDouble data(adata);
	WriteGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKRATE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetFMClockDivider() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_FMCLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetFMClockDivider(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_FMCLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetEGClockDivider() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_EGCLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetEGClockDivider(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_EGCLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetOutputClockDivider() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_OUTPUTCLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetOutputClockDivider(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_OUTPUTCLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerAClockDivider() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERACLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerAClockDivider(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERACLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBClockDivider() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBCLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBClockDivider(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBCLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetRegisterData(unsigned int registerNo) const
{
	GenericAccessDataValueUInt data;
	RegisterDataContext dataContext(registerNo);
	ReadGenericData((unsigned int)DATASOURCE_RAWREGISTER, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetRegisterData(unsigned int registerNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	RegisterDataContext dataContext(registerNo);
	WriteGenericData((unsigned int)DATASOURCE_RAWREGISTER, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTestData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TESTDATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTestData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TESTDATA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetLFOEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_LFOENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetLFOEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_LFOENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetLFOData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_LFODATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetLFOData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_LFODATA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerAData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERADATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerAData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERADATA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBDATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBDATA, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetCH3Mode() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_CH3MODE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetCH3Mode(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_CH3MODE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerBReset() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBRESET, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBReset(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBRESET, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerAReset() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERARESET, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerAReset(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERARESET, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerBEnable() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBENABLE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBEnable(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBENABLE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerAEnable() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERAENABLE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerAEnable(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERAENABLE, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerBLoad() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBLOAD, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBLoad(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBLOAD, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerALoad() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERALOAD, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerALoad(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERALOAD, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetDACData() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_DACDATA, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetDACData(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_DACDATA, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetDACEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_DACENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetDACEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_DACENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetDetuneData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_DETUNEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetDetuneData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_DETUNEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetMultipleData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_MULTIPLEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetMultipleData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_MULTIPLEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTotalLevelData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_TOTALLEVELDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_TOTALLEVELDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetKeyScaleData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_KEYSCALEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_KEYSCALEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetAttackRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_ATTACKRATEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAttackRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_ATTACKRATEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_AMPLITUDEMODULATIONENABLED, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_AMPLITUDEMODULATIONENABLED, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetDecayRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_DECAYRATEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetDecayRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_DECAYRATEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetSustainRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SUSTAINRATEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSustainRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SUSTAINRATEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetSustainLevelData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SUSTAINLEVELDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SUSTAINLEVELDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetReleaseRateData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_RELEASERATEDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_RELEASERATEDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetSSGData(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SSGDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSSGData(unsigned int channelNo, unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SSGDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetSSGEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SSGENABLED, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SSGENABLED, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetSSGAttack(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SSGATTACK, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSSGAttack(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SSGATTACK, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetSSGAlternate(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SSGALTERNATE, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SSGALTERNATE, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetSSGHold(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_SSGHOLD, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetSSGHold(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_SSGHOLD, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetFrequencyData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_FREQUENCYDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetFrequencyData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_FREQUENCYDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetBlockData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_BLOCKDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetBlockData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_BLOCKDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetFrequencyDataChannel3(unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_FREQUENCYDATACHANNEL3, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_FREQUENCYDATACHANNEL3, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetBlockDataChannel3(unsigned int operatorNo) const
{
	GenericAccessDataValueUInt data;
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_BLOCKDATACHANNEL3, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetBlockDataChannel3(unsigned int operatorNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	OperatorDataContext dataContext(CHANNEL3, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_BLOCKDATACHANNEL3, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetFeedbackData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_FEEDBACKDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetFeedbackData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_FEEDBACKDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetAlgorithmData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_ALGORITHMDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAlgorithmData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_ALGORITHMDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetOutputLeft(unsigned int channelNo) const
{
	GenericAccessDataValueBool data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_OUTPUTLEFT, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetOutputLeft(unsigned int channelNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_OUTPUTLEFT, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetOutputRight(unsigned int channelNo) const
{
	GenericAccessDataValueBool data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_OUTPUTRIGHT, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetOutputRight(unsigned int channelNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_OUTPUTRIGHT, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetAMSData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_AMSDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAMSData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_AMSDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetPMSData(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_PMSDATA, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetPMSData(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_PMSDATA, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Key on/off functions
//----------------------------------------------------------------------------------------
bool IYM2612::GetKeyState(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_KEYSTATE, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetKeyState(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_KEYSTATE, &dataContext, data);
}

//----------------------------------------------------------------------------------------
//Status register functions
//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetStatusRegister() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_STATUSREGISTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetStatusRegister(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_STATUSREGISTER, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetBusyFlag() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_BUSYFLAG, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetBusyFlag(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_BUSYFLAG, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerBOverflow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBOVERFLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBOverflow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBOVERFLOW, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::GetTimerAOverflow() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERAOVERFLOW, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerAOverflow(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERAOVERFLOW, 0, data);
}

//----------------------------------------------------------------------------------------
//Timer functions
//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerACurrentCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERACURRENTCOUNTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerACurrentCounter(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERACURRENTCOUNTER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int IYM2612::GetTimerBCurrentCounter() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_TIMERBCURRENTCOUNTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetTimerBCurrentCounter(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_TIMERBCURRENTCOUNTER, 0, data);
}

//----------------------------------------------------------------------------------------
//Audio logging functions
//----------------------------------------------------------------------------------------
bool IYM2612::IsAudioLoggingEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAudioLoggingEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::IsChannelAudioLoggingEnabled(unsigned int channelNo) const
{
	GenericAccessDataValueBool data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_CHANNELAUDIOLOGGINGENABLED, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_CHANNELAUDIOLOGGINGENABLED, &dataContext, data);
}

//----------------------------------------------------------------------------------------
bool IYM2612::IsOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueBool data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_OPERATORAUDIOLOGGINGENABLED, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetOperatorAudioLoggingEnabled(unsigned int channelNo, unsigned int operatorNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_OPERATORAUDIOLOGGINGENABLED, &dataContext, data);
}

//----------------------------------------------------------------------------------------
std::wstring IYM2612::GetAudioLoggingOutputPath() const
{
	GenericAccessDataValueFilePath data;
	ReadGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGPATH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetAudioLoggingOutputPath(const std::wstring& adata)
{
	GenericAccessDataValueFilePath data(adata);
	WriteGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGPATH, 0, data);
}

//----------------------------------------------------------------------------------------
std::wstring IYM2612::GetChannelAudioLoggingOutputPath(unsigned int channelNo) const
{
	GenericAccessDataValueFilePath data;
	ChannelDataContext dataContext(channelNo);
	ReadGenericData((unsigned int)DATASOURCE_CHANNELAUDIOLOGGINGPATH, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& adata)
{
	GenericAccessDataValueFilePath data(adata);
	ChannelDataContext dataContext(channelNo);
	WriteGenericData((unsigned int)DATASOURCE_CHANNELAUDIOLOGGINGPATH, &dataContext, data);
}

//----------------------------------------------------------------------------------------
std::wstring IYM2612::GetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo) const
{
	GenericAccessDataValueFilePath data;
	OperatorDataContext dataContext(channelNo, operatorNo);
	ReadGenericData((unsigned int)DATASOURCE_OPERATORAUDIOLOGGINGPATH, &dataContext, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void IYM2612::SetOperatorAudioLoggingOutputPath(unsigned int channelNo, unsigned int operatorNo, const std::wstring& adata)
{
	GenericAccessDataValueFilePath data(adata);
	OperatorDataContext dataContext(channelNo, operatorNo);
	WriteGenericData((unsigned int)DATASOURCE_OPERATORAUDIOLOGGINGPATH, &dataContext, data);
}
