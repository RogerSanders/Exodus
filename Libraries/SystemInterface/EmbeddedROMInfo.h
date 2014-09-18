#ifndef __EMBEDDEDROMINFO_H__
#define __EMBEDDEDROMINFO_H__
#include "IEmbeddedROMInfo.h"

class EmbeddedROMInfo :public IEmbeddedROMInfo
{
public:
	//Constructors
	EmbeddedROMInfo();

	//Interface version functions
	virtual unsigned int GetIEmbeddedROMInfoVersion() const;

	//Getters
	std::wstring GetName() const;
	std::wstring GetDisplayName() const;
	virtual unsigned int GetModuleID() const;
	virtual IDevice* GetTargetDevice() const;
	virtual unsigned int GetTargetInterfaceNo() const;
	virtual unsigned int GetROMRegionSize() const;
	virtual unsigned int GetROMEntryBitCount() const;
	std::wstring GetFilePath() const;

	//Setters
	void SetName(const std::wstring& aname);
	void SetDisplayName(const std::wstring& adisplayName);
	virtual void SetModuleID(unsigned int amoduleID);
	virtual void SetTargetDevice(IDevice* atargetDevice);
	virtual void SetTargetInterfaceNo(unsigned int atargetInterfaceNo);
	virtual void SetROMRegionSize(unsigned int aromRegionSize);
	virtual void SetROMEntryBitCount(unsigned int aromEntryBitCount);
	void SetFilePath(const std::wstring& afilePath);

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller);
	virtual void SetFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller);

private:
	std::wstring name;
	std::wstring displayName;
	unsigned int moduleID;
	IDevice* targetDevice;
	unsigned int targetInterfaceNo;
	unsigned int romRegionSize;
	unsigned int romEntryBitCount;
	std::wstring filePath;
};

#endif
