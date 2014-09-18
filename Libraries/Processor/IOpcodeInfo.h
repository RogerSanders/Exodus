#ifndef __IOPCODEINFO_H__
#define __IOPCODEINFO_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IOpcodeInfo
{
public:
	//Constructors
	virtual ~IOpcodeInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIOpcodeInfoVersion();
	virtual unsigned int GetIOpcodeInfoVersion() const = 0;

	//Opcode info functions
	virtual bool GetIsValidOpcode() const = 0;
	virtual void SetIsValidOpcode(bool state) = 0;
	virtual unsigned int GetOpcodeSize() const = 0;
	virtual void SetOpcodeSize(unsigned int state) = 0;
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
	virtual bool GetHasUndeterminedResultantPCLocation() const = 0;
	virtual void SetHasUndeterminedResultantPCLocation(bool state) = 0;

protected:
	//Opcode info functions
	virtual void GetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetOpcodeNameDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetOpcodeArgumentsDisassemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetDisassemblyCommentInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetDisassemblyCommentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const = 0;
	virtual void SetLabelTargetLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller) = 0;
	virtual void GetResultantPCLocationsInternal(const InteropSupport::ISTLObjectTarget<std::set<unsigned int>>& marshaller) const = 0;
	virtual void SetResultantPCLocationsInternal(const InteropSupport::ISTLObjectSource<std::set<unsigned int>>& marshaller) = 0;
};

#include "IOpcodeInfo.inl"
#endif
