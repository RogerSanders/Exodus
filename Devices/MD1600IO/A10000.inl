//----------------------------------------------------------------------------------------
//Version register functions
//----------------------------------
//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
//|--------------------------------|
//|*R |*V |*U1|*U2|Hardware Version|
//----------------------------------
//R: The current state of the region jumper
//V: The current state of the video jumper
//U1: Reports whether the SegaCD is connected
//U2: Probably reports whether another physical line is connected
//----------------------------------------------------------------------------------------
bool A10000::GetRegionFlag() const
{
	return versionRegister.GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetRegionFlag(bool data)
{
	versionRegister.SetBit(7, data);
}

//----------------------------------------------------------------------------------------
bool A10000::GetVideoFlag() const
{
	return versionRegister.GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetVideoFlag(bool data)
{
	versionRegister.SetBit(6, data);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSegaCDFlag() const
{
	return versionRegister.GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetSegaCDFlag(bool data)
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
Data A10000::GetDataRegister(unsigned int portNo) const
{
	return dataRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegister(unsigned int portNo, const Data& data)
{
	dataRegisters[portNo] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetControlRegister(unsigned int portNo) const
{
	return controlRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegister(unsigned int portNo, const Data& data)
{
	controlRegisters[portNo] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetSerialControlRegister(unsigned int portNo) const
{
	return serialControlRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialControlRegister(unsigned int portNo, const Data& data)
{
	serialControlRegisters[portNo] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetTxDataRegister(unsigned int portNo) const
{
	return txDataRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::SetTxDataRegister(unsigned int portNo, const Data& data)
{
	txDataRegisters[portNo] = data;
}

//----------------------------------------------------------------------------------------
Data A10000::GetRxDataRegister(unsigned int portNo) const
{
	return rxDataRegisters[portNo];
}

//----------------------------------------------------------------------------------------
void A10000::SetRxDataRegister(unsigned int portNo, const Data& data)
{
	rxDataRegisters[portNo] = data;
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
bool A10000::GetDataRegisterHL(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterHL(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(7, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTH(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTH(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(6, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTR(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTR(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterTL(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterTL(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD3(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD3(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD2(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD2(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD1(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD1(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetDataRegisterD0(unsigned int portNo)
{
	return dataRegisters[portNo].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetDataRegisterD0(unsigned int portNo, bool state)
{
	dataRegisters[portNo].SetBit(0, state);
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
bool A10000::GetControlRegisterHL(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(7);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterHL(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(7, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTH(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(6);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTH(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(6, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTR(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTR(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterTL(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterTL(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD3(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD3(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD2(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD2(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD1(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD1(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetControlRegisterD0(unsigned int portNo)
{
	return controlRegisters[portNo].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetControlRegisterD0(unsigned int portNo, bool state)
{
	controlRegisters[portNo].SetBit(0, state);
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
unsigned int A10000::GetSerialBaudRate(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetDataSegment(6, 2);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialBaudRate(unsigned int portNo, unsigned int state)
{
	serialControlRegisters[portNo].SetDataSegment(6, 2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialInputEnabled(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(5);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialInputEnabled(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(5, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialOutputEnabled(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(4);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialOutputEnabled(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(4, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialInterruptEnabled(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(3);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialInterruptEnabled(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(3, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetSerialErrorFlag(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(2);
}

//----------------------------------------------------------------------------------------
void A10000::SetSerialErrorFlag(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(2, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetRxDataBufferFull(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(1);
}

//----------------------------------------------------------------------------------------
void A10000::SetRxDataBufferFull(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(1, state);
}

//----------------------------------------------------------------------------------------
bool A10000::GetTxDataBufferFull(unsigned int portNo)
{
	return serialControlRegisters[portNo].GetBit(0);
}

//----------------------------------------------------------------------------------------
void A10000::SetTxDataBufferFull(unsigned int portNo, bool state)
{
	serialControlRegisters[portNo].SetBit(0, state);
}
