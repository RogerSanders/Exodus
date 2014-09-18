#ifndef __MEGADRIVEROMLOADER_H__
#define __MEGADRIVEROMLOADER_H__
#include "Extension/Extension.pkg"

class MegaDriveROMLoader :public Extension
{
public:
	//Constructors
	MegaDriveROMLoader(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~MegaDriveROMLoader();

	//ROM loading functions
	void LoadROMFile();
	void UnloadROMFile();

	//Window functions
	virtual bool RegisterSystemMenuHandler();
	virtual void AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment);

private:
	//Structures
	struct MegaDriveROMHeader;

	//View and menu classes
	class FileOpenMenuHandler;
	friend class FileOpenMenuHandler;

private:
	//ROM loading functions
	void UnloadROMFileFromModulePath(const std::wstring& targetROMModulePath) const;

	//ROM module generation
	bool BuildROMFileModuleFromFile(const std::wstring& filePath, IHierarchicalStorageNode& node, std::wstring& romName);
	bool SaveOutputROMModule(IHierarchicalStorageTree& tree, const std::wstring& filePath);

	//ROM analysis functions
	bool LoadROMHeaderFromFile(const std::wstring& filePath, MegaDriveROMHeader& romHeader) const;
	static bool AutoDetectRegionCode(const MegaDriveROMHeader& romHeader, std::wstring& regionCode);
	static bool AutoDetectBackupRAMSupport(const MegaDriveROMHeader& romHeader, unsigned int& sramStartLocation, unsigned int& sramByteSize, bool& linkedToEvenAddress, bool& linkedToOddAddress, bool& sram16Bit, std::vector<unsigned char>& initialRAMData);
	static bool StringStartsWith(const std::string& targetString, const std::string& compareString);

private:
	//Menu handling
	FileOpenMenuHandler* menuHandler;

	//Loaded ROM info
	bool selectionMadeThisSession;
	std::list<std::wstring> currentlyLoadedROMModuleFilePaths;
};

#include "MegaDriveROMLoader.inl"
#endif
