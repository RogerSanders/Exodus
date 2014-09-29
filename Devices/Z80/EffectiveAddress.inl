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
:data(0), indexState(IndexState::None), mode(Mode::Unset)
{}

//----------------------------------------------------------------------------------------
//Decode functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::SetMode(Mode amode)
{
	mode = amode;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::Decode8BitRegister(unsigned int adata)
{
	switch(adata & 0x07)
	{
	case 0:	//000
		mode = Mode::B;
		break;
	case 1:	//001
		mode = Mode::C;
		break;
	case 2:	//010
		mode = Mode::D;
		break;
	case 3:	//011
		mode = Mode::E;
		break;
	case 4:	//100
		mode = Mode::H;
		break;
	case 5:	//101
		mode = Mode::L;
		break;
	case 7:	//111
		mode = Mode::A;
		break;
	default:
		return false;
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::Decode16BitRegister(unsigned int adata)
{
	switch(adata & 0x03)
	{
	default:
		DebugAssert(false);
		break;
	case 0:	//00
		mode = Mode::BC;
		break;
	case 1:	//01
		mode = Mode::DE;
		break;
	case 2:	//10
		mode = Mode::HL;
		break;
	case 3:	//11
		mode = Mode::SP;
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(Bitcount asize, const Z80Word& location, const Z80* cpu, bool transparent)
{
	mode = Mode::Immediate;
	data.Resize(asize);
	cpu->ReadMemory(location, data, transparent);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(const Data& adata)
{
	mode = Mode::Immediate;
	data.Resize(adata.GetBitCount());
	data = adata;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(unsigned int adata)
{
	mode = Mode::Immediate;
	data.Resize(BITCOUNT_BYTE);
	data = adata;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& aaddress)
{
	mode = Mode::AddressIndirect;
	address = aaddress;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& location, const Z80* cpu, bool transparent)
{
	Z80Word tempAddress;
	cpu->ReadMemory(location, tempAddress, transparent);
	mode = Mode::AddressIndirect;
	address = tempAddress;
}

//----------------------------------------------------------------------------------------
//Effective address info
//----------------------------------------------------------------------------------------
EffectiveAddress::Mode EffectiveAddress::GetMode() const
{
	return mode;
}

//----------------------------------------------------------------------------------------
unsigned int EffectiveAddress::ExtensionSize()
{
	unsigned int extensionSize = 0;
	if(mode == Mode::Immediate)
	{
		extensionSize += data.GetByteSize();
	}
	else if(mode == Mode::AddressIndirect)
	{
		extensionSize += address.GetByteSize();
	}
	return extensionSize;
}

//----------------------------------------------------------------------------------------
//Index state functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::SetIndexState(IndexState aindexState, const Z80Byte& aindexOffset)
{
	indexState = aindexState;
	indexOffset = aindexOffset;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::UsesIndexOffset() const
{
	if(indexState != IndexState::None)
	{
		if((mode == Mode::HLIndirect) || (mode == Mode::HLPostInc) || (mode == Mode::HLPostDec))
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
Z80Byte EffectiveAddress::GetIndexOffset() const
{
	return indexOffset;
}

} //Close namespace Z80
