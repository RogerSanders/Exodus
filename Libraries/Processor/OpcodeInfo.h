#ifndef __OPCODEINFO_H__
#define __OPCODEINFO_H__
#include "IOpcodeInfo.h"

class OpcodeInfo :public IOpcodeInfo
{
public:
	//Constructors
	inline OpcodeInfo();

	//Interface version functions
	virtual unsigned int GetIOpcodeInfoVersion() const;

	//Opcode info functions
	virtual bool GetIsValidOpcode() const;
	virtual void SetIsValidOpcode(bool state);
	virtual unsigned int GetOpcodeSize() const;
	virtual void SetOpcodeSize(unsigned int state);
	inline std::wstring GetOpcodeNameDisassembly() const;
	inline void SetOpcodeNameDisassembly(const std::wstring& state);
	inline std::wstring GetOpcodeArgumentsDisassembly() const;
	inline void SetOpcodeArgumentsDisassembly(const std::wstring& state);
	inline std::wstring GetDisassemblyComment() const;
	inline void SetDisassemblyComment(const std::wstring& state);
	inline std::set<unsigned int> GetLabelTargetLocations() const;
	inline void SetLabelTargetLocations(const std::set<unsigned int>& state);
	inline std::set<unsigned int> GetResultantPCLocations() const;
	inline void SetResultantPCLocations(const std::set<unsigned int>& state);
	virtual bool GetHasUndeterminedResultantPCLocation() const;
	virtual void SetHasUndeterminedResultantPCLocation(bool state);

protected:
	//Opcode info functions
	virtual void GetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetDisassemblyCommentInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetDisassemblyCommentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const;
	virtual void SetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller);
	virtual void GetResultantPCLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const;
	virtual void SetResultantPCLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller);

private:
	bool valid;
	unsigned int opcodeSize;
	std::wstring disassemblyOpcode;
	std::wstring disassemblyArguments;
	std::wstring disassemblyComment;
	std::set<unsigned int> labelTargetLocations;
	std::set<unsigned int> resultantPCLocations;
	bool undeterminedResultantPCLocation;
};

#include "OpcodeInfo.inl"
#endif
