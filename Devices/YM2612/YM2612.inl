//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum YM2612::Channels
{
	CHANNEL1 = 0,
	CHANNEL2 = 1,
	CHANNEL3 = 2,
	CHANNEL4 = 3,
	CHANNEL5 = 4,
	CHANNEL6 = 5
};

//----------------------------------------------------------------------------------------
enum YM2612::Operators
{
	OPERATOR1 = 0,
	OPERATOR2 = 1,
	OPERATOR3 = 2,
	OPERATOR4 = 3
};

//----------------------------------------------------------------------------------------
enum YM2612::LineID
{
	LINE_IRQ = 1,
	LINE_IC
};

//----------------------------------------------------------------------------------------
enum YM2612::ClockID
{
	CLOCK_0M = 1
};

//----------------------------------------------------------------------------------------
enum YM2612::AccessContext
{
	ACCESSCONTEXT_IRQ
};

//----------------------------------------------------------------------------------------
enum YM2612::TimerParam
{
	TIMERPARAMA_LOAD      = 0,
	TIMERPARAMA_ENABLE    = 1,
	TIMERPARAMA_OVERFLOW  = 2,
	TIMERPARAMA_RATE      = 3,
	TIMERPARAMA_COUNTER   = 4,
	TIMERPARAMB_LOAD      = 5,
	TIMERPARAMB_ENABLE    = 6,
	TIMERPARAMB_OVERFLOW  = 7,
	TIMERPARAMB_RATE      = 8,
	TIMERPARAMB_COUNTER   = 9
};

//----------------------------------------------------------------------------------------
enum YM2612::LockedRegisterKey
{
	LOCKEDREG_NONE,
	LOCKEDREG_CH1_OP1_TL,
	LOCKEDREG_CH1_OP2_TL,
	LOCKEDREG_CH1_OP3_TL,
	LOCKEDREG_CH1_OP4_TL,
	LOCKEDREG_CH2_OP1_TL,
	LOCKEDREG_CH2_OP2_TL,
	LOCKEDREG_CH2_OP3_TL,
	LOCKEDREG_CH2_OP4_TL,
	LOCKEDREG_CH3_OP1_TL,
	LOCKEDREG_CH3_OP2_TL,
	LOCKEDREG_CH3_OP3_TL,
	LOCKEDREG_CH3_OP4_TL,
	LOCKEDREG_CH4_OP1_TL,
	LOCKEDREG_CH4_OP2_TL,
	LOCKEDREG_CH4_OP3_TL,
	LOCKEDREG_CH4_OP4_TL,
	LOCKEDREG_CH5_OP1_TL,
	LOCKEDREG_CH5_OP2_TL,
	LOCKEDREG_CH5_OP3_TL,
	LOCKEDREG_CH5_OP4_TL,
	LOCKEDREG_CH6_OP1_TL,
	LOCKEDREG_CH6_OP2_TL,
	LOCKEDREG_CH6_OP3_TL,
	LOCKEDREG_CH6_OP4_TL,
	LOCKEDREG_CH1_OP1_SL,
	LOCKEDREG_CH1_OP2_SL,
	LOCKEDREG_CH1_OP3_SL,
	LOCKEDREG_CH1_OP4_SL,
	LOCKEDREG_CH2_OP1_SL,
	LOCKEDREG_CH2_OP2_SL,
	LOCKEDREG_CH2_OP3_SL,
	LOCKEDREG_CH2_OP4_SL,
	LOCKEDREG_CH3_OP1_SL,
	LOCKEDREG_CH3_OP2_SL,
	LOCKEDREG_CH3_OP3_SL,
	LOCKEDREG_CH3_OP4_SL,
	LOCKEDREG_CH4_OP1_SL,
	LOCKEDREG_CH4_OP2_SL,
	LOCKEDREG_CH4_OP3_SL,
	LOCKEDREG_CH4_OP4_SL,
	LOCKEDREG_CH5_OP1_SL,
	LOCKEDREG_CH5_OP2_SL,
	LOCKEDREG_CH5_OP3_SL,
	LOCKEDREG_CH5_OP4_SL,
	LOCKEDREG_CH6_OP1_SL,
	LOCKEDREG_CH6_OP2_SL,
	LOCKEDREG_CH6_OP3_SL,
	LOCKEDREG_CH6_OP4_SL,
	LOCKEDREG_CH1_OP1_AR,
	LOCKEDREG_CH1_OP2_AR,
	LOCKEDREG_CH1_OP3_AR,
	LOCKEDREG_CH1_OP4_AR,
	LOCKEDREG_CH2_OP1_AR,
	LOCKEDREG_CH2_OP2_AR,
	LOCKEDREG_CH2_OP3_AR,
	LOCKEDREG_CH2_OP4_AR,
	LOCKEDREG_CH3_OP1_AR,
	LOCKEDREG_CH3_OP2_AR,
	LOCKEDREG_CH3_OP3_AR,
	LOCKEDREG_CH3_OP4_AR,
	LOCKEDREG_CH4_OP1_AR,
	LOCKEDREG_CH4_OP2_AR,
	LOCKEDREG_CH4_OP3_AR,
	LOCKEDREG_CH4_OP4_AR,
	LOCKEDREG_CH5_OP1_AR,
	LOCKEDREG_CH5_OP2_AR,
	LOCKEDREG_CH5_OP3_AR,
	LOCKEDREG_CH5_OP4_AR,
	LOCKEDREG_CH6_OP1_AR,
	LOCKEDREG_CH6_OP2_AR,
	LOCKEDREG_CH6_OP3_AR,
	LOCKEDREG_CH6_OP4_AR,
	LOCKEDREG_CH1_OP1_DR,
	LOCKEDREG_CH1_OP2_DR,
	LOCKEDREG_CH1_OP3_DR,
	LOCKEDREG_CH1_OP4_DR,
	LOCKEDREG_CH2_OP1_DR,
	LOCKEDREG_CH2_OP2_DR,
	LOCKEDREG_CH2_OP3_DR,
	LOCKEDREG_CH2_OP4_DR,
	LOCKEDREG_CH3_OP1_DR,
	LOCKEDREG_CH3_OP2_DR,
	LOCKEDREG_CH3_OP3_DR,
	LOCKEDREG_CH3_OP4_DR,
	LOCKEDREG_CH4_OP1_DR,
	LOCKEDREG_CH4_OP2_DR,
	LOCKEDREG_CH4_OP3_DR,
	LOCKEDREG_CH4_OP4_DR,
	LOCKEDREG_CH5_OP1_DR,
	LOCKEDREG_CH5_OP2_DR,
	LOCKEDREG_CH5_OP3_DR,
	LOCKEDREG_CH5_OP4_DR,
	LOCKEDREG_CH6_OP1_DR,
	LOCKEDREG_CH6_OP2_DR,
	LOCKEDREG_CH6_OP3_DR,
	LOCKEDREG_CH6_OP4_DR,
	LOCKEDREG_CH1_OP1_SR,
	LOCKEDREG_CH1_OP2_SR,
	LOCKEDREG_CH1_OP3_SR,
	LOCKEDREG_CH1_OP4_SR,
	LOCKEDREG_CH2_OP1_SR,
	LOCKEDREG_CH2_OP2_SR,
	LOCKEDREG_CH2_OP3_SR,
	LOCKEDREG_CH2_OP4_SR,
	LOCKEDREG_CH3_OP1_SR,
	LOCKEDREG_CH3_OP2_SR,
	LOCKEDREG_CH3_OP3_SR,
	LOCKEDREG_CH3_OP4_SR,
	LOCKEDREG_CH4_OP1_SR,
	LOCKEDREG_CH4_OP2_SR,
	LOCKEDREG_CH4_OP3_SR,
	LOCKEDREG_CH4_OP4_SR,
	LOCKEDREG_CH5_OP1_SR,
	LOCKEDREG_CH5_OP2_SR,
	LOCKEDREG_CH5_OP3_SR,
	LOCKEDREG_CH5_OP4_SR,
	LOCKEDREG_CH6_OP1_SR,
	LOCKEDREG_CH6_OP2_SR,
	LOCKEDREG_CH6_OP3_SR,
	LOCKEDREG_CH6_OP4_SR,
	LOCKEDREG_CH1_OP1_RR,
	LOCKEDREG_CH1_OP2_RR,
	LOCKEDREG_CH1_OP3_RR,
	LOCKEDREG_CH1_OP4_RR,
	LOCKEDREG_CH2_OP1_RR,
	LOCKEDREG_CH2_OP2_RR,
	LOCKEDREG_CH2_OP3_RR,
	LOCKEDREG_CH2_OP4_RR,
	LOCKEDREG_CH3_OP1_RR,
	LOCKEDREG_CH3_OP2_RR,
	LOCKEDREG_CH3_OP3_RR,
	LOCKEDREG_CH3_OP4_RR,
	LOCKEDREG_CH4_OP1_RR,
	LOCKEDREG_CH4_OP2_RR,
	LOCKEDREG_CH4_OP3_RR,
	LOCKEDREG_CH4_OP4_RR,
	LOCKEDREG_CH5_OP1_RR,
	LOCKEDREG_CH5_OP2_RR,
	LOCKEDREG_CH5_OP3_RR,
	LOCKEDREG_CH5_OP4_RR,
	LOCKEDREG_CH6_OP1_RR,
	LOCKEDREG_CH6_OP2_RR,
	LOCKEDREG_CH6_OP3_RR,
	LOCKEDREG_CH6_OP4_RR,
	LOCKEDREG_CH1_OP1_SSGEG,
	LOCKEDREG_CH1_OP2_SSGEG,
	LOCKEDREG_CH1_OP3_SSGEG,
	LOCKEDREG_CH1_OP4_SSGEG,
	LOCKEDREG_CH2_OP1_SSGEG,
	LOCKEDREG_CH2_OP2_SSGEG,
	LOCKEDREG_CH2_OP3_SSGEG,
	LOCKEDREG_CH2_OP4_SSGEG,
	LOCKEDREG_CH3_OP1_SSGEG,
	LOCKEDREG_CH3_OP2_SSGEG,
	LOCKEDREG_CH3_OP3_SSGEG,
	LOCKEDREG_CH3_OP4_SSGEG,
	LOCKEDREG_CH4_OP1_SSGEG,
	LOCKEDREG_CH4_OP2_SSGEG,
	LOCKEDREG_CH4_OP3_SSGEG,
	LOCKEDREG_CH4_OP4_SSGEG,
	LOCKEDREG_CH5_OP1_SSGEG,
	LOCKEDREG_CH5_OP2_SSGEG,
	LOCKEDREG_CH5_OP3_SSGEG,
	LOCKEDREG_CH5_OP4_SSGEG,
	LOCKEDREG_CH6_OP1_SSGEG,
	LOCKEDREG_CH6_OP2_SSGEG,
	LOCKEDREG_CH6_OP3_SSGEG,
	LOCKEDREG_CH6_OP4_SSGEG,
	LOCKEDREG_CH1_OP1_DT,
	LOCKEDREG_CH1_OP2_DT,
	LOCKEDREG_CH1_OP3_DT,
	LOCKEDREG_CH1_OP4_DT,
	LOCKEDREG_CH2_OP1_DT,
	LOCKEDREG_CH2_OP2_DT,
	LOCKEDREG_CH2_OP3_DT,
	LOCKEDREG_CH2_OP4_DT,
	LOCKEDREG_CH3_OP1_DT,
	LOCKEDREG_CH3_OP2_DT,
	LOCKEDREG_CH3_OP3_DT,
	LOCKEDREG_CH3_OP4_DT,
	LOCKEDREG_CH4_OP1_DT,
	LOCKEDREG_CH4_OP2_DT,
	LOCKEDREG_CH4_OP3_DT,
	LOCKEDREG_CH4_OP4_DT,
	LOCKEDREG_CH5_OP1_DT,
	LOCKEDREG_CH5_OP2_DT,
	LOCKEDREG_CH5_OP3_DT,
	LOCKEDREG_CH5_OP4_DT,
	LOCKEDREG_CH6_OP1_DT,
	LOCKEDREG_CH6_OP2_DT,
	LOCKEDREG_CH6_OP3_DT,
	LOCKEDREG_CH6_OP4_DT,
	LOCKEDREG_CH1_OP1_MUL,
	LOCKEDREG_CH1_OP2_MUL,
	LOCKEDREG_CH1_OP3_MUL,
	LOCKEDREG_CH1_OP4_MUL,
	LOCKEDREG_CH2_OP1_MUL,
	LOCKEDREG_CH2_OP2_MUL,
	LOCKEDREG_CH2_OP3_MUL,
	LOCKEDREG_CH2_OP4_MUL,
	LOCKEDREG_CH3_OP1_MUL,
	LOCKEDREG_CH3_OP2_MUL,
	LOCKEDREG_CH3_OP3_MUL,
	LOCKEDREG_CH3_OP4_MUL,
	LOCKEDREG_CH4_OP1_MUL,
	LOCKEDREG_CH4_OP2_MUL,
	LOCKEDREG_CH4_OP3_MUL,
	LOCKEDREG_CH4_OP4_MUL,
	LOCKEDREG_CH5_OP1_MUL,
	LOCKEDREG_CH5_OP2_MUL,
	LOCKEDREG_CH5_OP3_MUL,
	LOCKEDREG_CH5_OP4_MUL,
	LOCKEDREG_CH6_OP1_MUL,
	LOCKEDREG_CH6_OP2_MUL,
	LOCKEDREG_CH6_OP3_MUL,
	LOCKEDREG_CH6_OP4_MUL,
	LOCKEDREG_CH1_OP1_KS,
	LOCKEDREG_CH1_OP2_KS,
	LOCKEDREG_CH1_OP3_KS,
	LOCKEDREG_CH1_OP4_KS,
	LOCKEDREG_CH2_OP1_KS,
	LOCKEDREG_CH2_OP2_KS,
	LOCKEDREG_CH2_OP3_KS,
	LOCKEDREG_CH2_OP4_KS,
	LOCKEDREG_CH3_OP1_KS,
	LOCKEDREG_CH3_OP2_KS,
	LOCKEDREG_CH3_OP3_KS,
	LOCKEDREG_CH3_OP4_KS,
	LOCKEDREG_CH4_OP1_KS,
	LOCKEDREG_CH4_OP2_KS,
	LOCKEDREG_CH4_OP3_KS,
	LOCKEDREG_CH4_OP4_KS,
	LOCKEDREG_CH5_OP1_KS,
	LOCKEDREG_CH5_OP2_KS,
	LOCKEDREG_CH5_OP3_KS,
	LOCKEDREG_CH5_OP4_KS,
	LOCKEDREG_CH6_OP1_KS,
	LOCKEDREG_CH6_OP2_KS,
	LOCKEDREG_CH6_OP3_KS,
	LOCKEDREG_CH6_OP4_KS,
	LOCKEDREG_CH1_OP1_AM,
	LOCKEDREG_CH1_OP2_AM,
	LOCKEDREG_CH1_OP3_AM,
	LOCKEDREG_CH1_OP4_AM,
	LOCKEDREG_CH2_OP1_AM,
	LOCKEDREG_CH2_OP2_AM,
	LOCKEDREG_CH2_OP3_AM,
	LOCKEDREG_CH2_OP4_AM,
	LOCKEDREG_CH3_OP1_AM,
	LOCKEDREG_CH3_OP2_AM,
	LOCKEDREG_CH3_OP3_AM,
	LOCKEDREG_CH3_OP4_AM,
	LOCKEDREG_CH4_OP1_AM,
	LOCKEDREG_CH4_OP2_AM,
	LOCKEDREG_CH4_OP3_AM,
	LOCKEDREG_CH4_OP4_AM,
	LOCKEDREG_CH5_OP1_AM,
	LOCKEDREG_CH5_OP2_AM,
	LOCKEDREG_CH5_OP3_AM,
	LOCKEDREG_CH5_OP4_AM,
	LOCKEDREG_CH6_OP1_AM,
	LOCKEDREG_CH6_OP2_AM,
	LOCKEDREG_CH6_OP3_AM,
	LOCKEDREG_CH6_OP4_AM,

	LOCKEDREG_CH1_FNUM,
	LOCKEDREG_CH2_FNUM,
	LOCKEDREG_CH3_FNUM,
	LOCKEDREG_CH4_FNUM,
	LOCKEDREG_CH5_FNUM,
	LOCKEDREG_CH6_FNUM,
	LOCKEDREG_CH1_BLOCK,
	LOCKEDREG_CH2_BLOCK,
	LOCKEDREG_CH3_BLOCK,
	LOCKEDREG_CH4_BLOCK,
	LOCKEDREG_CH5_BLOCK,
	LOCKEDREG_CH6_BLOCK,
	LOCKEDREG_CH1_ALGORITHM,
	LOCKEDREG_CH2_ALGORITHM,
	LOCKEDREG_CH3_ALGORITHM,
	LOCKEDREG_CH4_ALGORITHM,
	LOCKEDREG_CH5_ALGORITHM,
	LOCKEDREG_CH6_ALGORITHM,
	LOCKEDREG_CH1_FEEDBACK,
	LOCKEDREG_CH2_FEEDBACK,
	LOCKEDREG_CH3_FEEDBACK,
	LOCKEDREG_CH4_FEEDBACK,
	LOCKEDREG_CH5_FEEDBACK,
	LOCKEDREG_CH6_FEEDBACK,
	LOCKEDREG_CH1_AMS,
	LOCKEDREG_CH2_AMS,
	LOCKEDREG_CH3_AMS,
	LOCKEDREG_CH4_AMS,
	LOCKEDREG_CH5_AMS,
	LOCKEDREG_CH6_AMS,
	LOCKEDREG_CH1_PMS,
	LOCKEDREG_CH2_PMS,
	LOCKEDREG_CH3_PMS,
	LOCKEDREG_CH4_PMS,
	LOCKEDREG_CH5_PMS,
	LOCKEDREG_CH6_PMS,
	LOCKEDREG_CH1_LEFT,
	LOCKEDREG_CH2_LEFT,
	LOCKEDREG_CH3_LEFT,
	LOCKEDREG_CH4_LEFT,
	LOCKEDREG_CH5_LEFT,
	LOCKEDREG_CH6_LEFT,
	LOCKEDREG_CH1_RIGHT,
	LOCKEDREG_CH2_RIGHT,
	LOCKEDREG_CH3_RIGHT,
	LOCKEDREG_CH4_RIGHT,
	LOCKEDREG_CH5_RIGHT,
	LOCKEDREG_CH6_RIGHT,

	LOCKEDREG_LFOENABLED,
	LOCKEDREG_LFOFREQ,
	LOCKEDREG_DACENABLED,
	LOCKEDREG_DACDATA,
	LOCKEDREG_CH3MODE,
	LOCKEDREG_CH3FNUM_OP1,
	LOCKEDREG_CH3FNUM_OP2,
	LOCKEDREG_CH3FNUM_OP3,
	LOCKEDREG_CH3FNUM_OP4,
	LOCKEDREG_CH3BLOCK_OP1,
	LOCKEDREG_CH3BLOCK_OP2,
	LOCKEDREG_CH3BLOCK_OP3,
	LOCKEDREG_CH3BLOCK_OP4,

	LOCKEDREG_TIMERA_LOAD,
	LOCKEDREG_TIMERA_ENABLE,
	LOCKEDREG_TIMERA_OVERFLOW,
	LOCKEDREG_TIMERA_RATE,
	LOCKEDREG_TIMERA_COUNTER,
	LOCKEDREG_TIMERB_LOAD,
	LOCKEDREG_TIMERB_ENABLE,
	LOCKEDREG_TIMERB_OVERFLOW,
	LOCKEDREG_TIMERB_RATE,
	LOCKEDREG_TIMERB_COUNTER
};

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
Data YM2612::GetRegisterData(unsigned int location, const AccessTarget& accessTarget) const
{
	return reg.Read(location, accessTarget);
}

//----------------------------------------------------------------------------------------
void YM2612::SetRegisterData(unsigned int location, const Data& data, const AccessTarget& accessTarget)
{
	reg.Write(location, data, accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::AdjustAddressChannelBlock(unsigned int address, unsigned int channelNo) const
{
	unsigned int newAddress = address;

	switch(channelNo)
	{
	case CHANNEL1:
		newAddress = address + 0;
		break;
	case CHANNEL2:
		newAddress = address + 1;
		break;
	case CHANNEL3:
		newAddress = address + 2;
		break;
	case CHANNEL4:
		newAddress = registerCountPerPart + address + 0;
		break;
	case CHANNEL5:
		newAddress = registerCountPerPart + address + 1;
		break;
	case CHANNEL6:
		newAddress = registerCountPerPart + address + 2;
		break;
	}

	return newAddress;
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::AdjustAddressOperatorBlock(unsigned int address, unsigned int channelNo, unsigned int operatorNo) const
{
	unsigned int newAddress = AdjustAddressChannelBlock(address, channelNo);

	switch(operatorNo)
	{
	case OPERATOR1:
		newAddress += 0x0;
		break;
	case OPERATOR2:
		newAddress += 0x8;
		break;
	case OPERATOR3:
		newAddress += 0x4;
		break;
	case OPERATOR4:
		newAddress += 0xC;
		break;
	}

	return newAddress;
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetAddressChannel3FrequencyBlock1(unsigned int operatorNo) const
{
	unsigned int newAddress = 0;

	switch(operatorNo)
	{
	case OPERATOR1:
		newAddress = 0xA9;
		break;
	case OPERATOR2:
		newAddress = 0xAA;
		break;
	case OPERATOR3:
		newAddress = 0xA8;
		break;
	case OPERATOR4:
		newAddress = 0xA2;
		break;
	}

	return newAddress;
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetAddressChannel3FrequencyBlock2(unsigned int operatorNo) const
{
	unsigned int newAddress = 0;

	switch(operatorNo)
	{
	case OPERATOR1:
		newAddress = 0xAD;
		break;
	case OPERATOR2:
		newAddress = 0xAE;
		break;
	case OPERATOR3:
		newAddress = 0xAC;
		break;
	case OPERATOR4:
		newAddress = 0xA6;
		break;
	}

	return newAddress;
}

//----------------------------------------------------------------------------------------
//Register functions
//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//21H   |-------------------------------|
//      |             Test              |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetTestData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x21, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetTestData(unsigned int data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x21, GetRegisterData(0x21, accessTarget).SetData(data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//22H   |-------------------------------|
//      | /   /   /   / |EN | FREQ CONT |
//      ---------------------------------
//----------------------------------------------------------------------------------------
bool YM2612::GetLFOEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x22, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetLFOEnabled(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x22, GetRegisterData(0x22, accessTarget).SetBit(3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetLFOData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x22, accessTarget).GetDataSegment(0, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetLFOData(unsigned int data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x22, GetRegisterData(0x22, accessTarget).SetDataSegment(0, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//24H   |-------------------------------|
//      |    Timer A MSBs (bits 2-9)    |
//      ---------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//25H   |-------------------------------|
//      | /   /   /   /   /   / | Timer |
//      |                       |A LSBs |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetTimerAData(const AccessTarget& accessTarget) const
{
	Data data(10);
	data.SetUpperBits(8, GetRegisterData(0x24, accessTarget).GetData()); //Get MSBs
	data.SetLowerBits(2, GetRegisterData(0x25, accessTarget).GetDataSegment(0, 2)); //Get LSBs
	return data.GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerAData(unsigned int data, const AccessTarget& accessTarget)
{
	Data temp(10, data);
	SetRegisterData(0x24, GetRegisterData(0x24, accessTarget).SetData(temp.GetDataSegment(2, 8)), accessTarget);				//Set MSBs
	SetRegisterData(0x25, GetRegisterData(0x25, accessTarget).SetDataSegment(0, 2, temp.GetDataSegment(0, 2)), accessTarget);	//Set LSBs
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//26H   |-------------------------------|
//      |            Timer B            |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetTimerBData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x26, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerBData(unsigned int data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x26, GetRegisterData(0x26, accessTarget).SetData(data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//      |-------------------------------|
//      |       | Timer | Timer | Timer |
//27H   |  CH3  | Reset | Enable| Load  |
//      |  Mode |-------|-------|-------|
//      |       | B | A | B | A | B | A |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetCH3Mode(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetDataSegment(6, 2);
}

//----------------------------------------------------------------------------------------
void YM2612::SetCH3Mode(unsigned int data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetDataSegment(6, 2, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerBReset(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(5);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerBReset(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(5, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerAReset(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(4);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerAReset(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(4, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerBEnable(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerBEnable(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerAEnable(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerAEnable(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(2, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerBLoad(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerBLoad(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(1, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerALoad(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x27, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerALoad(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x27, GetRegisterData(0x27, accessTarget).SetBit(0, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//2AH   |-------------------------------|
//      |           DAC Data            |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetDACData(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x2A, accessTarget).GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetDACData(unsigned int data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x2A, GetRegisterData(0x2A, accessTarget).SetData(data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//2BH   |-------------------------------|
//      |DAC| /   /   /   /   /   /   / |
//      |EN |                           |
//      ---------------------------------
//----------------------------------------------------------------------------------------
bool YM2612::GetDACEnabled(const AccessTarget& accessTarget) const
{
	return GetRegisterData(0x2B, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void YM2612::SetDACEnabled(bool data, const AccessTarget& accessTarget)
{
	SetRegisterData(0x2B, GetRegisterData(0x2B, accessTarget).SetBit(7, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//30H-  |-------------------------------|
//3FH   | / |  Detune   |   Multiple    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetDetuneData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x30, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(4, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetDetuneData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x30, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(4, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetMultipleData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x30, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void YM2612::SetMultipleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x30, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 4, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//40H-  |-------------------------------|
//4FH   | / |        Total Level        |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x40, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 7);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTotalLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x40, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 7, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//50H-  |-------------------------------|
//5FH   |  KS   | / |    Attack Rate    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x50, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(6, 2);
}

//----------------------------------------------------------------------------------------
void YM2612::SetKeyScaleData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x50, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(6, 2, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetAttackRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x50, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void YM2612::SetAttackRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x50, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 5, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//60H-  |-------------------------------|
//6FH   |AM | /   / |    Decay Rate     |
//      ---------------------------------
//----------------------------------------------------------------------------------------
bool YM2612::GetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x60, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void YM2612::SetAmplitudeModulationEnabled(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x60, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(7, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetDecayRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x60, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void YM2612::SetDecayRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x60, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 5, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//70H-  |-------------------------------|
//7FH   | /   /   / |   Sustain Rate    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetSustainRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x70, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 5);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSustainRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x70, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 5, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//80H-  |-------------------------------|
//8FH   | Sustain Level | Release Rate  |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x80, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(4, 4);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSustainLevelData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x80, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(4, 4, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x80, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void YM2612::SetReleaseRateData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x80, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 4, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//      |-------------------------------|
//90H-  |               |    SSG-EG     |
//9FH   | /   /   /   / |---------------|
//      |               | E |ATT|ALT|HLD|
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetSSGData(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSSGData(unsigned int channelNo, unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 4, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetBit(3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSSGEnabled(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetSSGAttack(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetBit(2);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSSGAttack(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(2, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetBit(1);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSSGAlternate(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(1, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetSSGHold(unsigned int channelNo, unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetBit(0);
}

//----------------------------------------------------------------------------------------
void YM2612::SetSSGHold(unsigned int channelNo, unsigned int operatorNo, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressOperatorBlock(0x90, channelNo, operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(0, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//A0H-  |-------------------------------|
//A3H   |   Freq Num LSB's (bits 0-7)   |
//      ---------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//A4H-  |-------------------------------|
//A7H   | /   / |  Block    | Freq Num  |
//      |       | (octave)  |  MSB's    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetFrequencyData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int register1 = AdjustAddressChannelBlock(0xA0, channel);
	unsigned int register2 = AdjustAddressChannelBlock(0xA4, channel);

	Data data(11);
	data.SetLowerBits(8, GetRegisterData(register1, accessTarget).GetData());
	data.SetUpperBits(3, GetRegisterData(register2, accessTarget).GetDataSegment(0, 3));
	return data.GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetFrequencyData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int register1 = AdjustAddressChannelBlock(0xA0, channel);
	unsigned int register2 = AdjustAddressChannelBlock(0xA4, channel);

	Data temp(11, data);
	SetRegisterData(register1, GetRegisterData(register1, accessTarget).SetData(temp.GetDataSegment(0, 8)), accessTarget);
	SetRegisterData(register2, GetRegisterData(register2, accessTarget).SetDataSegment(0, 3, temp.GetDataSegment(8, 3)), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetBlockData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xA4, channel);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(3, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetBlockData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xA4, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(3, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//A8H-  |-------------------------------|
//ABH   |   Freq Num LSB's (bits 0-7)   |
//      ---------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//ACH-  |-------------------------------|
//AFH   | /   / |  Block    | Freq Num  |
//      |       | (octave)  |  MSB's    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetFrequencyDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int register1 = GetAddressChannel3FrequencyBlock1(operatorNo);
	unsigned int register2 = GetAddressChannel3FrequencyBlock2(operatorNo);

	Data data(11);
	data.SetLowerBits(8, GetRegisterData(register1, accessTarget).GetData());
	data.SetUpperBits(3, GetRegisterData(register2, accessTarget).GetDataSegment(0, 3));
	return data.GetData();
}

//----------------------------------------------------------------------------------------
void YM2612::SetFrequencyDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int register1 = GetAddressChannel3FrequencyBlock1(operatorNo);
	unsigned int register2 = GetAddressChannel3FrequencyBlock2(operatorNo);

	Data temp(11, data);
	SetRegisterData(register1, GetRegisterData(register1, accessTarget).SetData(temp.GetDataSegment(0, 8)), accessTarget);
	SetRegisterData(register2, GetRegisterData(register2, accessTarget).SetDataSegment(0, 3, temp.GetDataSegment(8, 3)), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetBlockDataChannel3(unsigned int operatorNo, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = GetAddressChannel3FrequencyBlock2(operatorNo);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(3, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetBlockDataChannel3(unsigned int operatorNo, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = GetAddressChannel3FrequencyBlock2(operatorNo);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(3, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//B0H-  |-------------------------------|
//B3H   | /   / | Feedback  | Algorithm |
//      ---------------------------------
//----------------------------------------------------------------------------------------
unsigned int YM2612::GetFeedbackData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB0, channel);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(3, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetFeedbackData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB0, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(3, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetAlgorithmData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB0, channel);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetAlgorithmData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB0, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ---------------------------------
//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//B4H-  |-------------------------------|
//B7H   | L | R |  AMS  | / |    PMS    |
//      ---------------------------------
//----------------------------------------------------------------------------------------
bool YM2612::GetOutputLeft(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	return GetRegisterData(registerNo, accessTarget).GetBit(7);
}

//----------------------------------------------------------------------------------------
void YM2612::SetOutputLeft(unsigned int channel, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(7, data), accessTarget);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetOutputRight(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	return GetRegisterData(registerNo, accessTarget).GetBit(6);
}

//----------------------------------------------------------------------------------------
void YM2612::SetOutputRight(unsigned int channel, bool data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetBit(6, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetAMSData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(4, 2);
}

//----------------------------------------------------------------------------------------
void YM2612::SetAMSData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(4, 2, data), accessTarget);
}

//----------------------------------------------------------------------------------------
unsigned int YM2612::GetPMSData(unsigned int channel, const AccessTarget& accessTarget) const
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	return GetRegisterData(registerNo, accessTarget).GetDataSegment(0, 3);
}

//----------------------------------------------------------------------------------------
void YM2612::SetPMSData(unsigned int channel, unsigned int data, const AccessTarget& accessTarget)
{
	unsigned int registerNo = AdjustAddressChannelBlock(0xB4, channel);
	SetRegisterData(registerNo, GetRegisterData(registerNo, accessTarget).SetDataSegment(0, 3, data), accessTarget);
}

//----------------------------------------------------------------------------------------
//      ------------------------------------
//      | 7  | 6 | 5 | 4 | 3 | 2 | 1  | 0  |
//      |----------------------------------|
//Status|    |                   |Overflow |
//Reg   |Busy| /   /   /   /   / |---------|
//      |    |                   | B  | A  |
//      ------------------------------------
//----------------------------------------------------------------------------------------
Data YM2612::GetStatus() const
{
	return status;
}

//----------------------------------------------------------------------------------------
bool YM2612::GetBusyFlag() const
{
	return status.GetBit(7);
}

//----------------------------------------------------------------------------------------
void YM2612::SetBusyFlag(bool astate)
{
	status.SetBit(7, astate);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerBOverflow() const
{
	return status.GetBit(1);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerBOverflow(bool astate)
{
	status.SetBit(1, astate);
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerAOverflow() const
{
	return status.GetBit(0);
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerAOverflow(bool astate)
{
	status.SetBit(0, astate);
}
