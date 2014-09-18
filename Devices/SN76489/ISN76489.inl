//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum ISN76489::ISN76489DataSource
{
	DATASOURCE_CHANNEL1_VOLUMEREGISTER = 1,
	DATASOURCE_CHANNEL2_VOLUMEREGISTER,
	DATASOURCE_CHANNEL3_VOLUMEREGISTER,
	DATASOURCE_CHANNEL4_VOLUMEREGISTER,
	DATASOURCE_CHANNEL1_TONEREGISTER,
	DATASOURCE_CHANNEL2_TONEREGISTER,
	DATASOURCE_CHANNEL3_TONEREGISTER,
	DATASOURCE_CHANNEL4_TONEREGISTER,
	DATASOURCE_CHANNEL4_NOISETYPE,
	DATASOURCE_CHANNEL4_NOISEPERIOD,
	DATASOURCE_NOISESHIFTREGISTER,
	DATASOURCE_LATCHEDCHANNELNO,
	DATASOURCE_VOLUMEREGISTERLATCHED,
	DATASOURCE_EXTERNALCLOCKRATE,
	DATASOURCE_EXTERNALCLOCKDIVIDER,
	DATASOURCE_SHIFTREGISTERBITCOUNT,
	DATASOURCE_SHIFTREGISTERDEFAULTVALUE,
	DATASOURCE_WHITENOISETAPPEDBITMASK,
	DATASOURCE_PERIODICNOISETAPPEDBITMASK,
	DATASOURCE_AUDIOLOGGINGENABLED,
	DATASOURCE_AUDIOLOGGINGPATH,
	DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED,
	DATASOURCE_CHANNEL2_AUDIOLOGGINGENABLED,
	DATASOURCE_CHANNEL3_AUDIOLOGGINGENABLED,
	DATASOURCE_CHANNEL4_AUDIOLOGGINGENABLED,
	DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH,
	DATASOURCE_CHANNEL2_AUDIOLOGGINGPATH,
	DATASOURCE_CHANNEL3_AUDIOLOGGINGPATH,
	DATASOURCE_CHANNEL4_AUDIOLOGGINGPATH
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISN76489::ThisISN76489Version()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetVolumeRegisterExternal(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)(DATASOURCE_CHANNEL1_VOLUMEREGISTER + channelNo), 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetVolumeRegisterExternal(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)(DATASOURCE_CHANNEL1_VOLUMEREGISTER + channelNo), 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetToneRegisterExternal(unsigned int channelNo) const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)(DATASOURCE_CHANNEL1_TONEREGISTER + channelNo), 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetToneRegisterExternal(unsigned int channelNo, unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)(DATASOURCE_CHANNEL1_TONEREGISTER + channelNo), 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseShiftRegisterExternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_NOISESHIFTREGISTER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseShiftRegisterExternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_NOISESHIFTREGISTER, 0, data);
}

//----------------------------------------------------------------------------------------
//Latched register functions
//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetLatchedChannelNoExternal() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_LATCHEDCHANNELNO, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetLatchedChannelNoExternal(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_LATCHEDCHANNELNO, 0, data);
}

//----------------------------------------------------------------------------------------
bool ISN76489::GetVolumeRegisterLatchedExternal() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_VOLUMEREGISTERLATCHED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetVolumeRegisterLatchedExternal(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_VOLUMEREGISTERLATCHED, 0, data);
}

//----------------------------------------------------------------------------------------
//Device property functions
//----------------------------------------------------------------------------------------
double ISN76489::GetExternalClockRate() const
{
	GenericAccessDataValueDouble data;
	ReadGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKRATE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetExternalClockRate(double adata)
{
	GenericAccessDataValueDouble data(adata);
	WriteGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKRATE, 0, data);
}

//----------------------------------------------------------------------------------------
double ISN76489::GetExternalClockDivider() const
{
	GenericAccessDataValueDouble data;
	ReadGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKDIVIDER, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetExternalClockDivider(double adata)
{
	GenericAccessDataValueDouble data(adata);
	WriteGenericData((unsigned int)DATASOURCE_EXTERNALCLOCKDIVIDER, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetShiftRegisterBitCount() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_SHIFTREGISTERBITCOUNT, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetShiftRegisterBitCount(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SHIFTREGISTERBITCOUNT, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetShiftRegisterDefaultValue() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_SHIFTREGISTERDEFAULTVALUE, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetShiftRegisterDefaultValue(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_SHIFTREGISTERDEFAULTVALUE, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseChannelWhiteNoiseTappedBitMask() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_WHITENOISETAPPEDBITMASK, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseChannelWhiteNoiseTappedBitMask(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_WHITENOISETAPPEDBITMASK, 0, data);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseChannelPeriodicNoiseTappedBitMask() const
{
	GenericAccessDataValueUInt data;
	ReadGenericData((unsigned int)DATASOURCE_PERIODICNOISETAPPEDBITMASK, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseChannelPeriodicNoiseTappedBitMask(unsigned int adata)
{
	GenericAccessDataValueUInt data(adata);
	WriteGenericData((unsigned int)DATASOURCE_PERIODICNOISETAPPEDBITMASK, 0, data);
}

//----------------------------------------------------------------------------------------
//Audio logging functions
//----------------------------------------------------------------------------------------
bool ISN76489::IsAudioLoggingEnabled() const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGENABLED, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetAudioLoggingEnabled(bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGENABLED, 0, data);
}

//----------------------------------------------------------------------------------------
bool ISN76489::IsChannelAudioLoggingEnabled(unsigned int channelNo) const
{
	GenericAccessDataValueBool data;
	ReadGenericData((unsigned int)(DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED + channelNo), 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool adata)
{
	GenericAccessDataValueBool data(adata);
	WriteGenericData((unsigned int)(DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED + channelNo), 0, data);
}

//----------------------------------------------------------------------------------------
std::wstring ISN76489::GetAudioLoggingOutputPath() const
{
	GenericAccessDataValueFilePath data;
	ReadGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGPATH, 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetAudioLoggingOutputPath(const std::wstring& adata)
{
	GenericAccessDataValueFilePath data(adata);
	WriteGenericData((unsigned int)DATASOURCE_AUDIOLOGGINGPATH, 0, data);
}

//----------------------------------------------------------------------------------------
std::wstring ISN76489::GetChannelAudioLoggingOutputPath(unsigned int channelNo) const
{
	GenericAccessDataValueFilePath data;
	ReadGenericData((unsigned int)(DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH + channelNo), 0, data);
	return data.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& adata)
{
	GenericAccessDataValueFilePath data(adata);
	WriteGenericData((unsigned int)(DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH + channelNo), 0, data);
}
