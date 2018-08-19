#ifndef __OPCODEINFO_H__
#define __OPCODEINFO_H__
#include "IOpcodeInfo.h"

class OpcodeInfo :public IOpcodeInfo
{
public:
	// Constructors
	OpcodeInfo();

	// Interface version functions
	virtual unsigned int GetIOpcodeInfoVersion() const;

	// Opcode info functions
	virtual bool GetIsValidOpcode() const;
	virtual void SetIsValidOpcode(bool state);
	virtual unsigned int GetOpcodeSize() const;
	virtual void SetOpcodeSize(unsigned int state);
	virtual Marshal::Ret<std::wstring> GetOpcodeNameDisassembly() const;
	virtual void SetOpcodeNameDisassembly(const Marshal::In<std::wstring>& state);
	virtual Marshal::Ret<std::wstring> GetOpcodeArgumentsDisassembly() const;
	virtual void SetOpcodeArgumentsDisassembly(const Marshal::In<std::wstring>& state);
	virtual Marshal::Ret<std::wstring> GetDisassemblyComment() const;
	virtual void SetDisassemblyComment(const Marshal::In<std::wstring>& state);
	virtual Marshal::Ret<std::set<unsigned int>> GetLabelTargetLocations() const;
	virtual void SetLabelTargetLocations(const Marshal::In<std::set<unsigned int>>& state);
	virtual Marshal::Ret<std::set<unsigned int>> GetResultantPCLocations() const;
	virtual void SetResultantPCLocations(const Marshal::In<std::set<unsigned int>>& state);
	virtual bool GetHasUndeterminedResultantPCLocation() const;
	virtual void SetHasUndeterminedResultantPCLocation(bool state);

private:
	bool _valid;
	unsigned int _opcodeSize;
	std::wstring _disassemblyOpcode;
	std::wstring _disassemblyArguments;
	std::wstring _disassemblyComment;
	std::set<unsigned int> _labelTargetLocations;
	std::set<unsigned int> _resultantPCLocations;
	bool _undeterminedResultantPCLocation;
};

#endif
