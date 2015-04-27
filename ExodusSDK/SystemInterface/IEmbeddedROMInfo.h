#ifndef __IEMBEDDEDROMINFO_H__
#define __IEMBEDDEDROMINFO_H__
#include <ExtensionInterface/ExtensionInterface.pkg>
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IEmbeddedROMInfo
{
public:
	//Constructors
	virtual ~IEmbeddedROMInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIEmbeddedROMInfoVersion() { return 1; }
	virtual unsigned int GetIEmbeddedROMInfoVersion() const = 0;

	//Getters
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisplayName() const = 0;
	virtual unsigned int GetModuleID() const = 0;
	virtual IDevice* GetTargetDevice() const = 0;
	virtual unsigned int GetTargetInterfaceNo() const = 0;
	virtual unsigned int GetROMRegionSize() const = 0;
	virtual unsigned int GetROMEntryBitCount() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetFilePath() const = 0;

	//Setters
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& name) = 0;
	virtual void SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& displayName) = 0;
	virtual void SetModuleID(unsigned int moduleID) = 0;
	virtual void SetTargetDevice(IDevice* targetDevice) = 0;
	virtual void SetTargetInterfaceNo(unsigned int targetInterfaceNo) = 0;
	virtual void SetROMRegionSize(unsigned int romRegionSize) = 0;
	virtual void SetROMEntryBitCount(unsigned int romEntryBitCount) = 0;
	virtual void SetFilePath(const MarshalSupport::Marshal::In<std::wstring>& filePath) = 0;
};

#endif
