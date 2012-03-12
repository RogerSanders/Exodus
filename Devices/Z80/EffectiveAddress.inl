#include "Z80.h"
#include "Debug/Debug.pkg"
namespace Z80 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum EffectiveAddress::IndexState
{
	INDEX_NONE,
	INDEX_IX,
	INDEX_IY
};

//----------------------------------------------------------------------------------------
enum EffectiveAddress::Mode
{
	MODE_UNSET,

	MODE_A,
	MODE_B,
	MODE_C,
	MODE_D,
	MODE_E,
	MODE_H,
	MODE_L,

	MODE_AF,
	MODE_BC,
	MODE_DE,
	MODE_HL,

	MODE_AF2,
	MODE_BC2,
	MODE_DE2,
	MODE_HL2,

	MODE_I,
	MODE_R,
	MODE_SP,
	MODE_PC,
	MODE_BC_INDIRECT,
	MODE_DE_INDIRECT,
	MODE_HL_INDIRECT,
	MODE_SP_INDIRECT,
	MODE_SP_PREDEC,
	MODE_HL_POSTINC,
	MODE_DE_POSTINC,
	MODE_SP_POSTINC,
	MODE_HL_POSTDEC,
	MODE_DE_POSTDEC,
	MODE_ADDRESS_INDIRECT,
	MODE_IMMEDIATE
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EffectiveAddress::EffectiveAddress()
:data(0), indexState(INDEX_NONE), mode(MODE_UNSET)
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
		mode = MODE_B;
		break;
	case 1:	//001
		mode = MODE_C;
		break;
	case 2:	//010
		mode = MODE_D;
		break;
	case 3:	//011
		mode = MODE_E;
		break;
	case 4:	//100
		mode = MODE_H;
		break;
	case 5:	//101
		mode = MODE_L;
		break;
	case 7:	//111
		mode = MODE_A;
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
		ReleaseAssert(false);
		break;
	case 0:	//00
		mode = MODE_BC;
		break;
	case 1:	//01
		mode = MODE_DE;
		break;
	case 2:	//10
		mode = MODE_HL;
		break;
	case 3:	//11
		mode = MODE_SP;
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(Bitcount asize, const Z80Word& location, Z80* cpu, bool transparent)
{
	mode = MODE_IMMEDIATE;
	data.Resize(asize);
	cpu->ReadMemory(location, data, transparent);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(const Data& adata)
{
	mode = MODE_IMMEDIATE;
	data.Resize(adata.GetBitCount());
	data = adata;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(unsigned int adata)
{
	mode = MODE_IMMEDIATE;
	data.Resize(BITCOUNT_BYTE);
	data = adata;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& aaddress)
{
	mode = MODE_ADDRESS_INDIRECT;
	address = aaddress;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddress(const Z80Word& location, Z80* cpu, bool transparent)
{
	Z80Word tempAddress;
	cpu->ReadMemory(location, tempAddress, transparent);
	mode = MODE_ADDRESS_INDIRECT;
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
	if(mode == MODE_IMMEDIATE)
	{
		extensionSize += data.GetByteSize();
	}
	else if(mode == MODE_ADDRESS_INDIRECT)
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
	if(indexState != INDEX_NONE)
	{
		if((mode == MODE_HL_INDIRECT) || (mode == MODE_HL_POSTINC) || (mode == MODE_HL_POSTDEC))
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
