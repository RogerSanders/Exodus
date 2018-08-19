//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class ISN76489::ISN76489DataSource
{
	Channel1VolumeRegister = 1,
	Channel2VolumeRegister,
	Channel3VolumeRegister,
	Channel4VolumeRegister,
	Channel1ToneRegister,
	Channel2ToneRegister,
	Channel3ToneRegister,
	Channel4ToneRegister,
	Channel4NoiseType,
	Channel4NoisePeriod,
	NoiseShiftRegister,
	LatchedChannelNo,
	VolumeRegisterLatched,
	ExternalClockRate,
	ExternalClockDivider,
	ShiftRegisterBitCount,
	ShiftRegisterDefaultValue,
	WhiteNoiseTappedBitMask,
	PeriodicNoiseTappedBitMask,
	AudioLoggingEnabled,
	AudioLoggingPath,
	Channel1AudioLoggingEnabled,
	Channel2AudioLoggingEnabled,
	Channel3AudioLoggingEnabled,
	Channel4AudioLoggingEnabled,
	Channel1AudioLoggingPath,
	Channel2AudioLoggingPath,
	Channel3AudioLoggingPath,
	Channel4AudioLoggingPath
};

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetVolumeRegisterExternal(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::Channel1VolumeRegister + channelNo, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetVolumeRegisterExternal(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::Channel1VolumeRegister + channelNo, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetToneRegisterExternal(unsigned int channelNo) const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::Channel1ToneRegister + channelNo, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetToneRegisterExternal(unsigned int channelNo, unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::Channel1ToneRegister + channelNo, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseShiftRegisterExternal() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::NoiseShiftRegister, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseShiftRegisterExternal(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::NoiseShiftRegister, 0, genericData);
}

//----------------------------------------------------------------------------------------
//Latched register functions
//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetLatchedChannelNoExternal() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::LatchedChannelNo, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetLatchedChannelNoExternal(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::LatchedChannelNo, 0, genericData);
}

//----------------------------------------------------------------------------------------
bool ISN76489::GetVolumeRegisterLatchedExternal() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::VolumeRegisterLatched, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetVolumeRegisterLatchedExternal(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::VolumeRegisterLatched, 0, genericData);
}

//----------------------------------------------------------------------------------------
//Device property functions
//----------------------------------------------------------------------------------------
double ISN76489::GetExternalClockRate() const
{
	GenericAccessDataValueDouble genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::ExternalClockRate, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetExternalClockRate(double data)
{
	GenericAccessDataValueDouble genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::ExternalClockRate, 0, genericData);
}

//----------------------------------------------------------------------------------------
double ISN76489::GetExternalClockDivider() const
{
	GenericAccessDataValueDouble genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::ExternalClockDivider, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetExternalClockDivider(double data)
{
	GenericAccessDataValueDouble genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::ExternalClockDivider, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetShiftRegisterBitCount() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::ShiftRegisterBitCount, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetShiftRegisterBitCount(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::ShiftRegisterBitCount, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetShiftRegisterDefaultValue() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::ShiftRegisterDefaultValue, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetShiftRegisterDefaultValue(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::ShiftRegisterDefaultValue, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseChannelWhiteNoiseTappedBitMask() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::WhiteNoiseTappedBitMask, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseChannelWhiteNoiseTappedBitMask(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::WhiteNoiseTappedBitMask, 0, genericData);
}

//----------------------------------------------------------------------------------------
unsigned int ISN76489::GetNoiseChannelPeriodicNoiseTappedBitMask() const
{
	GenericAccessDataValueUInt genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::PeriodicNoiseTappedBitMask, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetNoiseChannelPeriodicNoiseTappedBitMask(unsigned int data)
{
	GenericAccessDataValueUInt genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::PeriodicNoiseTappedBitMask, 0, genericData);
}

//----------------------------------------------------------------------------------------
//Audio logging functions
//----------------------------------------------------------------------------------------
bool ISN76489::IsAudioLoggingEnabled() const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::AudioLoggingEnabled, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetAudioLoggingEnabled(bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::AudioLoggingEnabled, 0, genericData);
}

//----------------------------------------------------------------------------------------
bool ISN76489::IsChannelAudioLoggingEnabled(unsigned int channelNo) const
{
	GenericAccessDataValueBool genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::Channel1AudioLoggingEnabled + channelNo, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool data)
{
	GenericAccessDataValueBool genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::Channel1AudioLoggingEnabled + channelNo, 0, genericData);
}

//----------------------------------------------------------------------------------------
std::wstring ISN76489::GetAudioLoggingOutputPath() const
{
	GenericAccessDataValueFilePath genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::AudioLoggingPath, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetAudioLoggingOutputPath(const std::wstring& data)
{
	GenericAccessDataValueFilePath genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::AudioLoggingPath, 0, genericData);
}

//----------------------------------------------------------------------------------------
std::wstring ISN76489::GetChannelAudioLoggingOutputPath(unsigned int channelNo) const
{
	GenericAccessDataValueFilePath genericData;
	ReadGenericData((unsigned int)ISN76489DataSource::Channel1AudioLoggingPath + channelNo, 0, genericData);
	return genericData.GetValue();
}

//----------------------------------------------------------------------------------------
void ISN76489::SetChannelAudioLoggingOutputPath(unsigned int channelNo, const std::wstring& data)
{
	GenericAccessDataValueFilePath genericData(data);
	WriteGenericData((unsigned int)ISN76489DataSource::Channel1AudioLoggingPath + channelNo, 0, genericData);
}
