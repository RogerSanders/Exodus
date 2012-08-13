//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum SN76489::ClockID
{
	CLOCK_CLOCK = 1
};

//----------------------------------------------------------------------------------------
enum SN76489::LockedRegisterKey
{
	LOCKEDREG_NONE,
	LOCKEDREG_C0V,
	LOCKEDREG_C0D,
	LOCKEDREG_C1V,
	LOCKEDREG_C1D,
	LOCKEDREG_C2V,
	LOCKEDREG_C2D,
	LOCKEDREG_C3V,
	LOCKEDREG_C3D
};

//----------------------------------------------------------------------------------------
enum SN76489::DebugWindow
{
	DEBUGWINDOW_REGISTERS,
	DEBUGWINDOW_LOGGING,
	DEBUGWINDOW_PROPERTIES
};

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
Data SN76489::GetVolumeRegister(unsigned int channelNo, const AccessTarget& accessTarget)
{
	Data data(volumeRegisterBitCount);
	data = reg.Read((channelNo * 2) + 0, accessTarget);
	return data;
}

//----------------------------------------------------------------------------------------
void SN76489::SetVolumeRegister(unsigned int channelNo, const Data& adata, const AccessTarget& accessTarget)
{
	reg.Write((channelNo * 2) + 0, adata, accessTarget);
}

//----------------------------------------------------------------------------------------
Data SN76489::GetToneRegister(unsigned int channelNo, const AccessTarget& accessTarget)
{
	Data data(toneRegisterBitCount);
	data = reg.Read((channelNo * 2) + 1, accessTarget);
	return data;
}

//----------------------------------------------------------------------------------------
void SN76489::SetToneRegister(unsigned int channelNo, const Data& adata, const AccessTarget& accessTarget)
{
	reg.Write((channelNo * 2) + 1, adata, accessTarget);
}
