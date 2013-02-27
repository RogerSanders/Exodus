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

	//Window functions
	virtual void AddFileOpenMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);

	//##TODO## We need access to the following elements in order to properly implement
	//this extension:
	//System:
	//-SystemRunning()
	//-StopSystem()
	//-RunSystem()
	//-Initialize()
	//-GetLoadedModuleIDs()
	//-GetLoadedModuleInfo()
	//ExodusInterface:
	//-UnloadModule()
	//-LoadModuleFromFile()
	//-Preference paths
	//Note that, for consistency, our generic ROM loader should NOT retrieve information
	//on the currently loaded modules from the system, but rather, it should request that
	//information from the ExodusInterface object. Note however, that there is a potential
	//race condition here involved in attempting to enumerate and query the loaded modules
	//without any kind of locking mechanism. It can be made safe, but it seems we really
	//want to issue a command to unload the target module, and drop the iteration method
	//we're using. Perhaps an UnloadAllInstancesOfTargetModule() method, which takes in
	//the path and name of the target module file, should be defined on ExodusInterface.

private:
	//Structures
	struct MegaDriveROMHeader
	{
		std::string segaString;        //0x100-0x110
		std::string copyrightString;   //0x110-0x120
		std::string gameTitleJapan;    //0x120-0x150
		std::string gameTitleOverseas; //0x150-0x180
		std::string versionString;     //0x180-0x18E
		unsigned short checksum;       //0x18E-0x190
		std::string controllerString;  //0x190-0x1A0
		unsigned int romLocationStart; //0x1A0-0x1A4
		unsigned int romLocationEnd;   //0x1A4-0x1A8
		unsigned int ramLocationStart; //0x1A8-0x1AC
		unsigned int ramLocationEnd;   //0x1AC-0x1B0
		std::string unknownString;     //0x1B0-0x1F0
		std::string regionString;      //0x1F0-0x200
		unsigned int fileSize;
	};

	//View and menu classes
	class FileOpenMenuHandler;
	friend class FileOpenMenuHandler;

private:
	//##TODO##
	void LoadROMFile();
	bool SaveOutputROMModule(IHeirarchicalStorageTree& tree, const std::wstring& fileDir, const std::wstring& fileName);
	void UnloadROMFile();
	bool BuildROMFileModule(IHeirarchicalStorageNode& node, std::wstring& romName);
	bool BuildROMFileModuleFromFile(const std::wstring& fileDir, const std::wstring& fileName, IHeirarchicalStorageNode& node, std::wstring& romName);
	bool LoadROMHeaderFromFile(const std::wstring& fileDir, const std::wstring& fileName, MegaDriveROMHeader& romHeader) const;

private:
	//Menu handling
	FileOpenMenuHandler* menuHandler;

	bool romLoaded;
	std::wstring loadedROMModuleFileDir;
	std::wstring loadedROMModuleFileName;
};

#endif
