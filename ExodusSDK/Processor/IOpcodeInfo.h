#ifndef __IOPCODEINFO_H__
#define __IOPCODEINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IOpcodeInfo
{
public:
	//Constructors
	virtual ~IOpcodeInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIOpcodeInfoVersion() { return 1; }
	virtual unsigned int GetIOpcodeInfoVersion() const = 0;

	//Opcode info functions
	virtual bool GetIsValidOpcode() const = 0;
	virtual void SetIsValidOpcode(bool state) = 0;
	virtual unsigned int GetOpcodeSize() const = 0;
	virtual void SetOpcodeSize(unsigned int state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetOpcodeNameDisassembly() const = 0;
	virtual void SetOpcodeNameDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetOpcodeArgumentsDisassembly() const = 0;
	virtual void SetOpcodeArgumentsDisassembly(const MarshalSupport::Marshal::In<std::wstring>& state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisassemblyComment() const = 0;
	virtual void SetDisassemblyComment(const MarshalSupport::Marshal::In<std::wstring>& state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetLabelTargetLocations() const = 0;
	virtual void SetLabelTargetLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetResultantPCLocations() const = 0;
	virtual void SetResultantPCLocations(const MarshalSupport::Marshal::In<std::set<unsigned int>>& state) = 0;
	virtual bool GetHasUndeterminedResultantPCLocation() const = 0;
	virtual void SetHasUndeterminedResultantPCLocation(bool state) = 0;
};

#endif
