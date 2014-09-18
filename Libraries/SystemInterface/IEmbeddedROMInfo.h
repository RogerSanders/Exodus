#ifndef __IEMBEDDEDROMINFO_H__
#define __IEMBEDDEDROMINFO_H__
#include <ExodusExtensionInterface/ExodusExtensionInterface.pkg>
#include <InteropSupport/InteropSupport.pkg>
#include <string>

class IEmbeddedROMInfo
{
public:
	//Constructors
	virtual ~IEmbeddedROMInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIEmbeddedROMInfoVersion();
	virtual unsigned int GetIEmbeddedROMInfoVersion() const = 0;

	//Getters
	inline std::wstring GetName() const;
	inline std::wstring GetDisplayName() const;
	virtual unsigned int GetModuleID() const = 0;
	virtual IDevice* GetTargetDevice() const = 0;
	virtual unsigned int GetTargetInterfaceNo() const = 0;
	virtual unsigned int GetROMRegionSize() const = 0;
	virtual unsigned int GetROMEntryBitCount() const = 0;
	inline std::wstring GetFilePath() const;

	//Setters
	inline void SetName(const std::wstring& name);
	inline void SetDisplayName(const std::wstring& displayName);
	virtual void SetModuleID(unsigned int moduleID) = 0;
	virtual void SetTargetDevice(IDevice* targetDevice) = 0;
	virtual void SetTargetInterfaceNo(unsigned int targetInterfaceNo) = 0;
	virtual void SetROMRegionSize(unsigned int romRegionSize) = 0;
	virtual void SetROMEntryBitCount(unsigned int romEntryBitCount) = 0;
	inline void SetFilePath(const std::wstring& filePath);

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller) = 0;
	virtual void SetFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) = 0;
};

#include "IEmbeddedROMInfo.inl"
#endif
