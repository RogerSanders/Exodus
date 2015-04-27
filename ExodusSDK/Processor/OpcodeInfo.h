#ifndef __OPCODEINFO_H__
#define __OPCODEINFO_H__
#include "IOpcodeInfo.h"

class OpcodeInfo :public IOpcodeInfo
{
public:
	//Constructors
	OpcodeInfo();

	//Interface version functions
	virtual unsigned int GetIOpcodeInfoVersion() const;

	//Opcode info functions
	virtual bool GetIsValidOpcode() const;
	virtual void SetIsValidOpcode(bool state);
	virtual unsigned int GetOpcodeSize() const;
	virtual void SetOpcodeSize(unsigned int state);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetOpcodeNameDisassembly() const;
	virtual void SetOpcodeNameDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetOpcodeArgumentsDisassembly() const;
	virtual void SetOpcodeArgumentsDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisassemblyComment() const;
	virtual void SetDisassemblyComment(const MarshalSupport::Marshal::In<std::wstring>& state);
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetLabelTargetLocations() const;
	virtual void SetLabelTargetLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state);
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetResultantPCLocations() const;
	virtual void SetResultantPCLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state);
	virtual bool GetHasUndeterminedResultantPCLocation() const;
	virtual void SetHasUndeterminedResultantPCLocation(bool state);

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

#endif
