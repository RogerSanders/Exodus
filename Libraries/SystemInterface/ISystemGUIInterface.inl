//Disable warning about the presence of virtual functions without a virtual destructor.
//Our structures below use virtual functions to create code barriers between assemblies so
//that they can be marshalled, but they are never derived from, so a virtual destructor is
//not required.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4265)
#endif

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class ISystemGUIInterface::FileType
{
	ZIP,
	XML
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::StateInfo
{
public:
	//Constructors
	StateInfo()
	:valid(false), debuggerState(false), screenshotPresent(false)
	{}
	StateInfo(MarshalSupport::marshal_object_t, const StateInfo& source)
	{
		source.MarshalToTarget(valid, debuggerState, creationDate, creationTime, comments, screenshotPresent, screenshotFilename);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(bool& validMarshaller, bool& debuggerStateMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& creationDateMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& creationTimeMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& commentsMarshaller, bool& screenshotPresentMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& screenshotFilenameMarshaller) const
	{
		validMarshaller = valid;
		debuggerStateMarshaller = debuggerState;
		creationDateMarshaller = creationDate;
		creationTimeMarshaller = creationTime;
		commentsMarshaller = comments;
		screenshotPresentMarshaller = screenshotPresent;
		screenshotFilenameMarshaller = screenshotFilename;
	}

public:
	bool valid;
	bool debuggerState;
	std::wstring creationDate;
	std::wstring creationTime;
	std::wstring comments;
	bool screenshotPresent;
	std::wstring screenshotFilename;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ModuleRelationship
{
public:
	//Constructors
	ModuleRelationship()
	{}
	ModuleRelationship(MarshalSupport::marshal_object_t, const ModuleRelationship& source)
	{
		source.MarshalToTarget(savedModuleID, savedModuleClassName, savedModuleInstanceName, foundMatch, loadedModuleID);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& savedModuleIDMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& savedModuleClassNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& savedModuleInstanceNameMarshaller, bool& foundMatchMarshaller, unsigned int& loadedModuleIDMarshaller) const
	{
		savedModuleIDMarshaller = savedModuleID;
		savedModuleClassNameMarshaller = savedModuleClassName;
		savedModuleInstanceNameMarshaller = savedModuleInstanceName;
		foundMatchMarshaller = foundMatch;
		loadedModuleIDMarshaller = loadedModuleID;
	}

public:
	//Saved module info
	unsigned int savedModuleID;
	std::wstring savedModuleClassName;
	std::wstring savedModuleInstanceName;

	//Loaded module info
	bool foundMatch;
	unsigned int loadedModuleID;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SavedRelationshipImportConnector
{
public:
	//Constructors
	SavedRelationshipImportConnector()
	{}
	SavedRelationshipImportConnector(MarshalSupport::marshal_object_t, const SavedRelationshipImportConnector& source)
	{
		source.MarshalToTarget(moduleID, className, instanceNameLocal, instanceNameRemote);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& moduleIDMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& classNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameLocalMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameRemoteMarshaller) const
	{
		moduleIDMarshaller = moduleID;
		classNameMarshaller = className;
		instanceNameLocalMarshaller = instanceNameLocal;
		instanceNameRemoteMarshaller = instanceNameRemote;
	}

public:
	unsigned int moduleID;
	std::wstring className;
	std::wstring instanceNameLocal;
	std::wstring instanceNameRemote;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SavedRelationshipExportConnector
{
public:
	//Constructors
	SavedRelationshipExportConnector()
	{}
	SavedRelationshipExportConnector(MarshalSupport::marshal_object_t, const SavedRelationshipExportConnector& source)
	{
		source.MarshalToTarget(className, instanceName);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const MarshalSupport::Marshal::Out<std::wstring>& classNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller = className;
		instanceNameMarshaller = instanceName;
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SavedRelationshipModule
{
public:
	//Constructors
	SavedRelationshipModule()
	{}
	SavedRelationshipModule(MarshalSupport::marshal_object_t, const SavedRelationshipModule& source)
	{
		source.MarshalToTarget(moduleID, systemClassName, className, instanceName, filePath, exportedConnectors, importedConnectors);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& moduleIDMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& systemClassNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& classNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& filePathMarshaller, const MarshalSupport::Marshal::Out<std::list<SavedRelationshipExportConnector>>& exportedConnectorsMarshaller, const MarshalSupport::Marshal::Out<std::list<SavedRelationshipImportConnector>>& importedConnectorsMarshaller) const
	{
		moduleIDMarshaller = moduleID;
		systemClassNameMarshaller = systemClassName;
		classNameMarshaller = className;
		instanceNameMarshaller = instanceName;
		filePathMarshaller = filePath;
		exportedConnectorsMarshaller = exportedConnectors;
		importedConnectorsMarshaller = importedConnectors;
	}

public:
	unsigned int moduleID;
	std::wstring systemClassName;
	std::wstring className;
	std::wstring instanceName;
	std::wstring filePath;
	std::list<SavedRelationshipExportConnector> exportedConnectors;
	std::list<SavedRelationshipImportConnector> importedConnectors;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ConnectorMapping
{
public:
	//Constructors
	ConnectorMapping()
	{}
	ConnectorMapping(MarshalSupport::marshal_object_t, const ConnectorMapping& source)
	{
		source.MarshalToTarget(connectorID, importingModuleConnectorInstanceName);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& connectorIDMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& importingModuleConnectorInstanceNameMarshaller) const
	{
		connectorIDMarshaller = connectorID;
		importingModuleConnectorInstanceNameMarshaller = importingModuleConnectorInstanceName;
	}

public:
	unsigned int connectorID;
	std::wstring importingModuleConnectorInstanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ConnectorDefinitionImport
{
public:
	//Constructors
	ConnectorDefinitionImport()
	{}
	ConnectorDefinitionImport(MarshalSupport::marshal_object_t, const ConnectorDefinitionImport& source)
	{
		source.MarshalToTarget(className, instanceName);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const MarshalSupport::Marshal::Out<std::wstring>& classNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller = className;
		instanceNameMarshaller = instanceName;
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ConnectorDefinitionExport
{
public:
	//Constructors
	ConnectorDefinitionExport()
	{}
	ConnectorDefinitionExport(MarshalSupport::marshal_object_t, const ConnectorDefinitionExport& source)
	{
		source.MarshalToTarget(className, instanceName);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const MarshalSupport::Marshal::Out<std::wstring>& classNameMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller = className;
		instanceNameMarshaller = instanceName;
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SystemLogEntry
{
public:
	//Constructors
	SystemLogEntry()
	{}
	SystemLogEntry(MarshalSupport::marshal_object_t, const SystemLogEntry& sourceObject)
	{
		sourceObject.MarshalToTarget(eventLevel, text, source, eventLevelString, eventTimeString);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(ILogEntry::EventLevel& eventLevelMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& textMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& sourceMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& eventLevelStringMarshaller, const MarshalSupport::Marshal::Out<std::wstring>& eventTimeStringMarshaller) const
	{
		eventLevelMarshaller = eventLevel;
		textMarshaller = text;
		sourceMarshaller = source;
		eventLevelStringMarshaller = eventLevelString;
		eventTimeStringMarshaller = eventTimeString;
	}

public:
	//##FIX## Improve the definition of a log entry. We should have a list of source path
	//elements, and a timestamp which records integer components for the time, and builds
	//a string from those elements on demand. We also shouldn't have any string
	//representation of the event level stored or associated with the log entry, that
	//should be built by the GUI.
	ILogEntry::EventLevel eventLevel;
	std::wstring text;
	std::wstring source;
	std::wstring eventLevelString;
	std::wstring eventTimeString;
};

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
