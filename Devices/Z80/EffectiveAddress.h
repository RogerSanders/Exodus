#ifndef __Z80_EFFECTIVEADDRESS_H__
#define __Z80_EFFECTIVEADDRESS_H__
#include "Z80.h"
namespace Z80 {

class EffectiveAddress
{
public:
	//Enumerations
	enum class IndexState;
	enum class Mode;

	//Constructors
	inline EffectiveAddress();

	//Decode functions
	inline void SetMode(Mode amode);
	inline bool Decode8BitRegister(unsigned int adata);
	inline bool Decode16BitRegister(unsigned int adata);
	inline void BuildImmediateData(Bitcount asize, const Z80Word& location, const Z80* cpu, bool transparent);
	inline void BuildImmediateData(const Data& adata);
	inline void BuildQuickData(unsigned int adata);
	inline void BuildAbsoluteAddress(const Z80Word& aaddress);
	inline void BuildAbsoluteAddress(const Z80Word& location, const Z80* cpu, bool transparent);

	//Effective address info
	inline Mode GetMode() const;
	inline unsigned int ExtensionSize();

	//Index state functions
	inline void SetIndexState(IndexState aindexState, const Z80Byte& aindexOffset);
	inline bool UsesIndexOffset() const;

	//Data transfer
	double Read(Z80* cpu, const Z80Word& pc, Data& target) const;
	double Write(Z80* cpu, const Z80Word& pc, const Data& target) const;

	//Disassembly functions
	std::wstring Disassemble() const;
	std::wstring DisassembleImmediateAsPCDisplacement(const Z80Word& location) const;

private:
	//Index state functions
	inline Z80Byte GetIndexOffset() const;

private:
	Mode mode;
	Data data;
	Z80Word address;
	IndexState indexState;
	Z80Byte indexOffset;
};

} //Close namespace Z80
#include "EffectiveAddress.inl"
#endif
