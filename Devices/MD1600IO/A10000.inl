//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class A10000::Ports
{
	Port1 = 0,
	Port2 = 1,
	Port3 = 2
};

//----------------------------------------------------------------------------------------
enum class A10000::PortLine
{
	D0,
	D1,
	D2,
	D3,
	TL,
	TR,
	TH
};

//----------------------------------------------------------------------------------------
enum class A10000::LineID
{
	LINE_PORT1_D0 = 1,  //IO
	LINE_PORT1_D1,      //IO
	LINE_PORT1_D2,      //IO
	LINE_PORT1_D3,      //IO
	LINE_PORT1_TL,      //IO
	LINE_PORT1_TR,      //IO
	LINE_PORT1_TH,      //IO
	LINE_PORT2_D0,      //IO
	LINE_PORT2_D1,      //IO
	LINE_PORT2_D2,      //IO
	LINE_PORT2_D3,      //IO
	LINE_PORT2_TL,      //IO
	LINE_PORT2_TR,      //IO
	LINE_PORT2_TH,      //IO
	LINE_PORT3_D0,      //IO
	LINE_PORT3_D1,      //IO
	LINE_PORT3_D2,      //IO
	LINE_PORT3_D3,      //IO
	LINE_PORT3_TL,      //IO
	LINE_PORT3_TR,      //IO
	LINE_PORT3_TH,      //IO

	LINE_JAP,           //I
	LINE_NTSC,          //I
	LINE_DISK,          //I
	LINE_HL,            //O
	LINE_HWVERSION      //I
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct A10000::InputLineState
{
	bool lineAssertedD0;
	bool lineAssertedD1;
	bool lineAssertedD2;
	bool lineAssertedD3;
	bool lineAssertedTL;
	bool lineAssertedTR;
	bool lineAssertedTH;
};

//----------------------------------------------------------------------------------------
struct A10000::LineAccess
{
	LineAccess(LineID alineLD, const Data& astate, double aaccessTime)
	:lineID(alineLD), state(astate), accessTime(aaccessTime)
	{}

	LineID lineID;
	Data state;
	double accessTime;
};

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int A10000::GetPortIndexForPort(Ports portNo)
{
	return ((unsigned int)portNo - (unsigned int)Ports::Port1);
}

//----------------------------------------------------------------------------------------
//Version register functions
//-------------------------------------
//|  7 |  6 | 5  | 4 | 3 | 2 | 1 | 0  |
//|-----------------------------------|
//|MODE|VMOD|DISK| - |Hardware Version|
//-------------------------------------
//MODE: The current state of the JAP line. This line is active low, so a value of 0 means
//      it's a domestic model, while a value of 1 means it's an overseas model.
//VMOD: The inverted state of the NTSC line. Returns 1 if the NTSC line is negated (PAL).
//DISK: The current state of the DISK line on the expansion port. Note that since DISK is
//      active low, this means that a value of 0 means a device is connected.
//----------------------------------------------------------------------------------------
bool A10000::GetOverseasFlag() const
{
	return versionRegister.GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetOverseasFlag(bool data)
{
	versionRegister.SetBit(7, data);
}

//----------------------------------------------------------------------------------------
bool A10000::GetPALFlag() const
{
	return versionRegister.GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetPALFlag(bool data)
{
	versionRegister.SetBit(6, data);
}

//----------------------------------------------------------------------------------------
bool A10000::GetNoDiskFlag() const
{
	return versionRegister.GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetNoDiskFlag(bool data)
{
	versionRegister.SetBit(5, data);
}

//----------------------------------------------------------------------------------------
unsigned int A10000::GetHardwareVersion() const
{
	return versionRegister.GetDataSegment(0, 4);
}

//----------------------------------------------------------------------------------------
void A10000::SetHardwareVersion(unsigned int data)
{
	versionRegister.SetDataSegment(0, 4, data);
}

//----------------------------------------------------------------------------------------
//Raw register functions
//----------------------------------------------------------------------------------------
Data A10000::GetVersionRegister() const
{
	return versionRegister;
}

//----------------------------------------------------------------------------------------
void A10000::SetVersionRegister(const Data& data)
{
	versionRegister = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetDataRegister(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegister(Ports portNo, const Data& data)
{
	dataRegisters[GetPortIndexForPort(portNo)] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetControlRegister(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegister(Ports portNo, const Data& data)
{
	controlRegisters[GetPortIndexForPort(portNo)] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetSerialControlRegister(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialControlRegister(Ports portNo, const Data& data)
{
	serialControlRegisters[GetPortIndexForPort(portNo)] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetTxDataRegister(Ports portNo) const
{
	return txDataRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::SetTxDataRegister(Ports portNo, const Data& data)
{
	txDataRegisters[GetPortIndexForPort(portNo)] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetRxDataRegister(Ports portNo) const
{
	return rxDataRegisters[GetPortIndexForPort(portNo)];
}

//----------------------------------------------------------------------------------------
void A10000::SetRxDataRegister(Ports portNo, const Data& data)
{
	rxDataRegisters[GetPortIndexForPort(portNo)] = data;
}

//----------------------------------------------------------------------------------------
//Data register functions
//
//This register stores the buffered data for each data line. The returned state is valid
//regardless of whether the line is currently configured as an input or an output.
//---------------------------------
//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|-------------------------------|
//|*U |TH |TR |TL |D3 |D2 |D1 |D0 |
//---------------------------------
//U: Unused. Returns the last value written to it. Effectively, this acts like a line
//which is always set as an output, and isn't mapped to any physical control port line.
//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterHL(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterHL(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(7, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTH(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTH(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(6, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTR(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTR(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTL(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTL(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD3(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD3(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD2(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD2(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD1(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD1(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD0(Ports portNo) const
{
	return dataRegisters[GetPortIndexForPort(portNo)].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD0(Ports portNo, bool state)
{
	dataRegisters[GetPortIndexForPort(portNo)].SetBit(0, state);
}

//----------------------------------------------------------------------------------------
//Control register functions
//
//The lower 7 bits of this register set the input/output of the 7 physical data lines for
//the port. If the bit for a line is set, that line is running as an output. If the bit
//is clear, that line is running as an input. The upper bit of this register controls the
//HL interrupt state. If this bit is set, and TH is set to an input, whenever TH is
//asserted, HL will also be asserted.
//---------------------------------
//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|-------------------------------|
//|HL |TH |TR |TL |D3 |D2 |D1 |D0 |
//---------------------------------
//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterHL(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterHL(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(7, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTH(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTH(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(6, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTR(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTR(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTL(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTL(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD3(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD3(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD2(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD2(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD1(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD1(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD0(Ports portNo) const
{
	return controlRegisters[GetPortIndexForPort(portNo)].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD0(Ports portNo, bool state)
{
	controlRegisters[GetPortIndexForPort(portNo)].SetBit(0, state);
}

//----------------------------------------------------------------------------------------
//Serial control register functions
//
//The serial control register allows serial transfer settings to be configured, as well
//as returning status flags on the current state of serial transfers.
//---------------------------------
//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|-------------------------------|
//| Rate  |SIE|SOE| I | E | R | F |
//---------------------------------
//Rate: The serial baud rate. Rates are as follows:
//      00 - 4800 bps
//      01 - 2400 bps
//      10 - 1200 bps
//      11 - 300 bps
//SIE: Serial input enable. If set, TR functions as the serial input pin
//SOE: Serial output enable. If set, TL functions as the serial output pin
//I: Interrput enable. If set, HL is asserted when a byte is received serially
//E: Read-only error flag. Set if an error occurs reading the current byte.
//R: Read-only RxData buffer full flag. Set if a valid byte has been read into RxData.
//F: Read-only TxData buffer full flag. Set if there is still data in TxData waiting to
//   be written.
//----------------------------------------------------------------------------------------
unsigned int A10000::GetSerialBaudRate(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetDataSegment(6, 2);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialBaudRate(Ports portNo, unsigned int state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetDataSegment(6, 2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialInputEnabled(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialInputEnabled(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialOutputEnabled(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialOutputEnabled(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialInterruptEnabled(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialInterruptEnabled(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialErrorFlag(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialErrorFlag(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetRxDataBufferFull(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetRxDataBufferFull(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetTxDataBufferFull(Ports portNo) const
{
	return serialControlRegisters[GetPortIndexForPort(portNo)].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetTxDataBufferFull(Ports portNo, bool state)
{
	serialControlRegisters[GetPortIndexForPort(portNo)].SetBit(0, state);
}
