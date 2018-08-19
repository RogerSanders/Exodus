#ifndef __Z80_EFFECTIVEADDRESS_H__
#define __Z80_EFFECTIVEADDRESS_H__
#include "Z80.h"
namespace Z80 {

class EffectiveAddress
{
public:
	// Enumerations
	enum class IndexState;
	enum class Mode;

	// Constructors
	inline EffectiveAddress();

	// Decode functions
	inline void SetMode(Mode mode);
	inline bool Decode8BitRegister(unsigned int data);
	inline bool Decode16BitRegister(unsigned int data);
	inline void BuildImmediateData(Bitcount size, const Z80Word& location, const Z80* cpu, bool transparent);
	inline void BuildImmediateData(const Data& data);
	inline void BuildQuickData(unsigned int data);
	inline void BuildAbsoluteAddress(const Z80Word& address);
	inline void BuildAbsoluteAddress(const Z80Word& location, const Z80* cpu, bool transparent);

	// Effective address info
	inline Mode GetMode() const;
	inline unsigned int ExtensionSize();

	// Index state functions
	inline void SetIndexState(IndexState indexState, const Z80Byte& indexOffset);
	inline bool UsesIndexOffset() const;

	// Data transfer
	double Read(Z80* cpu, const Z80Word& pc, Data& target) const;
	double Write(Z80* cpu, const Z80Word& pc, const Data& target) const;

	// Disassembly functions
	std::wstring Disassemble() const;
	std::wstring DisassembleImmediateAsPCDisplacement(const Z80Word& location) const;

private:
	// Index state functions
	inline Z80Byte GetIndexOffset() const;

private:
	Mode _mode;
	Data _data;
	Z80Word _address;
	IndexState _indexState;
	Z80Byte _indexOffset;
};

} // Close namespace Z80
#include "EffectiveAddress.inl"
#endif
