#ifndef __EMBEDDEDROMINFO_H__
#define __EMBEDDEDROMINFO_H__
#include "IEmbeddedROMInfo.h"

class EmbeddedROMInfo :public IEmbeddedROMInfo
{
public:
	// Constructors
	EmbeddedROMInfo();

	// Interface version functions
	virtual unsigned int GetIEmbeddedROMInfoVersion() const;

	// Getters
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual Marshal::Ret<std::wstring> GetDisplayName() const;
	virtual unsigned int GetModuleID() const;
	virtual IDevice* GetTargetDevice() const;
	virtual unsigned int GetTargetInterfaceNo() const;
	virtual unsigned int GetROMRegionSize() const;
	virtual unsigned int GetROMEntryBitCount() const;
	virtual Marshal::Ret<std::wstring> GetFilePath() const;

	// Setters
	virtual void SetName(const Marshal::In<std::wstring>& name);
	virtual void SetDisplayName(const Marshal::In<std::wstring>& displayName);
	virtual void SetModuleID(unsigned int moduleID);
	virtual void SetTargetDevice(IDevice* targetDevice);
	virtual void SetTargetInterfaceNo(unsigned int targetInterfaceNo);
	virtual void SetROMRegionSize(unsigned int romRegionSize);
	virtual void SetROMEntryBitCount(unsigned int romEntryBitCount);
	virtual void SetFilePath(const Marshal::In<std::wstring>& filePath);

private:
	std::wstring _name;
	std::wstring _displayName;
	unsigned int _moduleID;
	IDevice* _targetDevice;
	unsigned int _targetInterfaceNo;
	unsigned int _romRegionSize;
	unsigned int _romEntryBitCount;
	std::wstring _filePath;
};

#endif
