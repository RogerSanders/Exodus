#ifndef __IGUIEXTENSIONINTERFACE_H__
#define __IGUIEXTENSIONINTERFACE_H__
#include <string>

class IGUIExtensionInterface
{
public:
	//Constructors
	virtual ~IGUIExtensionInterface() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGUIExtensionInterfaceVersion();
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const = 0;

	//Main window functions
	virtual void* GetMainWindowHandle() const = 0;

	//Module functions
	inline bool LoadModuleFromFile(const std::wstring& fileDir, const std::wstring& fileName);
	virtual void UnloadModule(unsigned int moduleID) = 0;
	virtual void UnloadAllModules() = 0;

	//Global preference functions
	inline std::wstring GetGlobalPreferencePathModules() const;
	inline std::wstring GetGlobalPreferencePathSavestates() const;
	inline std::wstring GetGlobalPreferencePathPersistentState() const;
	inline std::wstring GetGlobalPreferencePathWorkspaces() const;
	inline std::wstring GetGlobalPreferencePathCaptures() const;
	inline std::wstring GetGlobalPreferencePathAssemblies() const;
	inline std::wstring GetGlobalPreferenceInitialSystem() const;
	inline std::wstring GetGlobalPreferenceInitialWorkspace() const;
	virtual bool GetGlobalPreferenceEnableThrottling() const = 0;
	virtual bool GetGlobalPreferenceRunWhenProgramModuleLoaded() const = 0;
	virtual bool GetEnablePersistentState() const = 0;

	//Assembly functions
	inline bool LoadAssembly(const std::wstring& filePath);

protected:
	//Module functions
	virtual bool LoadModuleFromFileInternal(const wchar_t* fileDir, const wchar_t* fileName) = 0;

	//Global preference functions
	virtual const wchar_t* GetGlobalPreferencePathModulesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathSavestatesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathPersistentStateInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathWorkspacesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathCapturesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferencePathAssembliesInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferenceInitialSystemInternal() const = 0;
	virtual const wchar_t* GetGlobalPreferenceInitialWorkspaceInternal() const = 0;

	//Assembly functions
	virtual bool LoadAssemblyInternal(const wchar_t* filePath) = 0;
};

#include "IGUIExtensionInterface.inl"
#endif
