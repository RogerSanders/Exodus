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
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisplayName() const;
	virtual unsigned int GetModuleID() const;
	virtual IDevice* GetTargetDevice() const;
	virtual unsigned int GetTargetInterfaceNo() const;
	virtual unsigned int GetROMRegionSize() const;
	virtual unsigned int GetROMEntryBitCount() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetFilePath() const;

	//Setters
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname);
	virtual void SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName);
	virtual void SetModuleID(unsigned int amoduleID);
	virtual void SetTargetDevice(IDevice* atargetDevice);
	virtual void SetTargetInterfaceNo(unsigned int atargetInterfaceNo);
	virtual void SetROMRegionSize(unsigned int aromRegionSize);
	virtual void SetROMEntryBitCount(unsigned int aromEntryBitCount);
	virtual void SetFilePath(const MarshalSupport::Marshal::In<std::wstring>& afilePath);

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
