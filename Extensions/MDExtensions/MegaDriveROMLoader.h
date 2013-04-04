#ifndef __MEGADRIVEROMLOADER_H__
#define __MEGADRIVEROMLOADER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Extension/Extension.pkg"

class MegaDriveROMLoader :public Extension
{
public:
	//Constructors
	MegaDriveROMLoader(const std::wstring& ainstanceName, unsigned int amoduleID);
	~MegaDriveROMLoader();

	//ROM loading functions
	void LoadROMFile();
	void UnloadROMFile();

	//Window functions
	virtual void AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);

private:
	//Structures
	struct MegaDriveROMHeader;

	//View and menu classes
	class FileOpenMenuHandler;
	friend class FileOpenMenuHandler;

private:
	//ROM module generation
	bool BuildROMFileModule(IHeirarchicalStorageNode& node, std::wstring& romName);
	bool BuildROMFileModuleFromFile(const std::wstring& fileDir, const std::wstring& fileName, IHeirarchicalStorageNode& node, std::wstring& romName);
	bool SaveOutputROMModule(IHeirarchicalStorageTree& tree, const std::wstring& fileDir, const std::wstring& fileName);

	//ROM analysis functions
	bool LoadROMHeaderFromFile(const std::wstring& fileDir, const std::wstring& fileName, MegaDriveROMHeader& romHeader) const;
	static bool AutoDetectRegionCode(const MegaDriveROMHeader& romHeader, std::wstring& regionCode);
	static bool AutoDetectBackupRAMSupport(const MegaDriveROMHeader& romHeader, unsigned int& sramStartLocation, unsigned int& sramByteSize, bool& linkedToEvenAddress, bool& linkedToOddAddress, bool& sram16Bit, std::vector<unsigned char>& initialRAMData);
	static bool StringStartsWith(const std::string& targetString, const std::string& compareString);

private:
	//Menu handling
	FileOpenMenuHandler* menuHandler;

	bool romLoaded;
	std::wstring loadedROMModuleFileDir;
	std::wstring loadedROMModuleFileName;
};

#include "MegaDriveROMLoader.inl"
#endif
