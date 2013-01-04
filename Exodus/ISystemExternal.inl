//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum ISystemExternal::FileType
{
	FILETYPE_ZIP,
	FILETYPE_XML
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ISystemExternal::StateInfo
{
	StateInfo()
	:valid(false), debuggerState(false), screenshotPresent(false)
	{}

	bool valid;
	bool debuggerState;
	std::wstring creationDate;
	std::wstring creationTime;
	std::wstring comments;
	bool screenshotPresent;
	std::wstring screenshotFilename;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::LoadedModuleInfo
{
	LoadedModuleInfo()
	:systemModule(false), programModule(false)
	{}

	//Internal data
	unsigned int moduleID;

	//External information
	std::wstring fileDir;
	std::wstring fileName;

	//Required metadata
	bool systemModule;
	bool programModule;
	std::wstring systemClassName;
	std::wstring className;
	std::wstring instanceName;
	std::wstring displayName;
	//##TODO## Consider making this mandatory
	//unsigned int versionNumber;

	//Optional metadata
	std::wstring productionYear;
	std::wstring manufacturerCode;
	std::wstring manufacturerDisplayName;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::ModuleRelationship
{
	//Saved module info
	unsigned int savedModuleID;
	std::wstring savedModuleClassName;
	std::wstring savedModuleInstanceName;

	//Loaded module info
	bool foundMatch;
	unsigned int loadedModuleID;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::SavedRelationshipImportConnector
{
	unsigned int moduleID;
	std::wstring className;
	std::wstring instanceNameLocal;
	std::wstring instanceNameRemote;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::SavedRelationshipExportConnector
{
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::SavedRelationshipModule
{
	unsigned int moduleID;
	std::wstring systemClassName;
	std::wstring className;
	std::wstring instanceName;
	std::wstring fileDir;
	std::wstring fileName;
	std::list<SavedRelationshipExportConnector> exportedConnectors;
	std::list<SavedRelationshipImportConnector> importedConnectors;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::ConnectorMapping
{
	unsigned int connectorID;
	std::wstring importingModuleConnectorInstanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::ConnectorInfo
{
	//Internal data
	unsigned int connectorID;

	//Exporting module info
	unsigned int exportingModuleID;
	std::wstring connectorClassName;
	std::wstring exportingModuleConnectorInstanceName;
	std::wstring systemClassName;

	//Importing module info
	bool connectorUsed;
	unsigned int importingModuleID;
	std::wstring importingModuleConnectorInstanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::ConnectorDefinitionImport
{
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemExternal::ConnectorDefinitionExport
{
	std::wstring className;
	std::wstring instanceName;
};
