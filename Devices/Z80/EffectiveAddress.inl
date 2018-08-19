#include "Z80.h"
#include "Debug/Debug.pkg"
namespace Z80 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class EffectiveAddress::IndexState
{
	None,
	IX,
	IY
};

//----------------------------------------------------------------------------------------
enum class EffectiveAddress::Mode
{
	Unset,

	A,
	B,
	C,
	D,
	E,
	H,
	L,

	AF,
	BC,
	DE,
	HL,

	AF2,
	BC2,
	DE2,
	HL2,

	I,
	R,
	SP,
	PC,
	BCIndirect,
	DEIndirect,
	HLIndirect,
	SPIndirect,
	SPPreDec,
	HLPostInc,
	DEPostInc,
	SPPostInc,
	HLPostDec,
	DEPostDec,
	AddressIndirect,
	Immediate
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EffectiveAddress::EffectiveAddress()
:_data(0), _indexState(IndexState::None), _mode(Mode::Unset)
{}

//----------------------------------------------------------------------------------------
//Decode functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::SetMode(Mode mode)
{
	_mode = mode;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::Decode8BitRegister(unsigned int data)
{
	switch (data & 0x07)
	{
	case 0:	//000
		_mode = Mode::B;
		break;
	case 1:	//001
		_mode = Mode::C;
		break;
	case 2:	//010
		_mode = Mode::D;
		break;
	case 3:	//011
		_mode = Mode::E;
		break;
	case 4:	//100
		_mode = Mode::H;
		break;
	case 5:	//101
		_mode = Mode::L;
		break;
	case 7:	//111
		_mode = Mode::A;
		break;
	default:
		return false;
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::Decode16BitRegister(unsigned int data)
{
	switch (data & 0x03)
	{
	default:
		DebugAssert(false);
		break;
	case 0:	//00
		_mode = Mode::BC;
		break;
	case 1:	//01
		_mode = Mode::DE;
		break;
	case 2:	//10
		_mode = Mode::HL;
		break;
	case 3:	//11
		_mode = Mode::SP;
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(Bitcount size, const Z80Word& location, const Z80* cpu, bool transparent)
{
	_mode = Mode::Immediate;
	_data.Resize(size);
	cpu->ReadMemory(location, _data, transparent);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(const Data& data)
{
	_mode = Mode::Immediate;
	_data.Resize(data.GetBitCount());
	_data = data;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(unsigned int data)
{
	_mode = Mode::Immediate;
	_data.Resize(BITCOUNT_BYTE);
	_data = data;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& address)
{
	_mode = Mode::AddressIndirect;
	_address = address;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& location, const Z80* cpu, bool transparent)
{
	Z80Word tempAddress;
	cpu->ReadMemory(location, tempAddress, transparent);
	_mode = Mode::AddressIndirect;
	_address = tempAddress;
}

//----------------------------------------------------------------------------------------
//Effective address info
//----------------------------------------------------------------------------------------
EffectiveAddress::Mode EffectiveAddress::GetMode() const
{
	return _mode;
}

//----------------------------------------------------------------------------------------
unsigned int EffectiveAddress::ExtensionSize()
{
	unsigned int extensionSize = 0;
	if (_mode == Mode::Immediate)
	{
		extensionSize += _data.GetByteSize();
	}
	else if (_mode == Mode::AddressIndirect)
	{
		extensionSize += _address.GetByteSize();
	}
	return extensionSize;
}

//----------------------------------------------------------------------------------------
//Index state functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::SetIndexState(IndexState indexState, const Z80Byte& indexOffset)
{
	_indexState = indexState;
	_indexOffset = indexOffset;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::UsesIndexOffset() const
{
	if (_indexState != IndexState::None)
	{
		if ((_mode == Mode::HLIndirect) || (_mode == Mode::HLPostInc) || (_mode == Mode::HLPostDec))
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
Z80Byte EffectiveAddress::GetIndexOffset() const
{
	return _indexOffset;
}

} //Close namespace Z80
