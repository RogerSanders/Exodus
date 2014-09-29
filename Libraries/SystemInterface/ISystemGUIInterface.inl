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
struct ISystemGUIInterface::StateInfo :protected InteropSupport::IMarshallingObject
{
public:
	//Constructors
	StateInfo()
	:valid(false), debuggerState(false), screenshotPresent(false)
	{}

protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const StateInfo& sourceResolved = static_cast<const StateInfo&>(source);
		sourceResolved.MarshalToTarget(valid, debuggerState, InteropSupport::STLObjectTarget<std::wstring>(creationDate), InteropSupport::STLObjectTarget<std::wstring>(creationTime), InteropSupport::STLObjectTarget<std::wstring>(comments), screenshotPresent, InteropSupport::STLObjectTarget<std::wstring>(screenshotFilename));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(bool& validMarshaller, bool& debuggerStateMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& creationDateMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& creationTimeMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& commentsMarshaller, bool& screenshotPresentMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& screenshotFilenameMarshaller) const
	{
		validMarshaller = valid;
		debuggerStateMarshaller = debuggerState;
		creationDateMarshaller.MarshalFrom(creationDate);
		creationTimeMarshaller.MarshalFrom(creationTime);
		commentsMarshaller.MarshalFrom(comments);
		screenshotPresentMarshaller = screenshotPresent;
		screenshotFilenameMarshaller.MarshalFrom(screenshotFilename);
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
struct ISystemGUIInterface::ModuleRelationship :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const ModuleRelationship& sourceResolved = static_cast<const ModuleRelationship&>(source);
		sourceResolved.MarshalToTarget(savedModuleID, InteropSupport::STLObjectTarget<std::wstring>(savedModuleClassName), InteropSupport::STLObjectTarget<std::wstring>(savedModuleInstanceName), foundMatch, loadedModuleID);
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& savedModuleIDMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& savedModuleClassNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& savedModuleInstanceNameMarshaller, bool& foundMatchMarshaller, unsigned int& loadedModuleIDMarshaller) const
	{
		savedModuleIDMarshaller = savedModuleID;
		savedModuleClassNameMarshaller.MarshalFrom(savedModuleClassName);
		savedModuleInstanceNameMarshaller.MarshalFrom(savedModuleInstanceName);
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
struct ISystemGUIInterface::SavedRelationshipImportConnector :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const SavedRelationshipImportConnector& sourceResolved = static_cast<const SavedRelationshipImportConnector&>(source);
		sourceResolved.MarshalToTarget(moduleID, InteropSupport::STLObjectTarget<std::wstring>(className), InteropSupport::STLObjectTarget<std::wstring>(instanceNameLocal), InteropSupport::STLObjectTarget<std::wstring>(instanceNameRemote));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& moduleIDMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& classNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameLocalMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameRemoteMarshaller) const
	{
		moduleIDMarshaller = moduleID;
		classNameMarshaller.MarshalFrom(className);
		instanceNameLocalMarshaller.MarshalFrom(instanceNameLocal);
		instanceNameRemoteMarshaller.MarshalFrom(instanceNameRemote);
	}

public:
	unsigned int moduleID;
	std::wstring className;
	std::wstring instanceNameLocal;
	std::wstring instanceNameRemote;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SavedRelationshipExportConnector :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const SavedRelationshipExportConnector& sourceResolved = static_cast<const SavedRelationshipExportConnector&>(source);
		sourceResolved.MarshalToTarget(InteropSupport::STLObjectTarget<std::wstring>(className), InteropSupport::STLObjectTarget<std::wstring>(instanceName));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const InteropSupport::ISTLObjectTarget<std::wstring>& classNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller.MarshalFrom(className);
		instanceNameMarshaller.MarshalFrom(instanceName);
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SavedRelationshipModule :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const SavedRelationshipModule& sourceResolved = static_cast<const SavedRelationshipModule&>(source);
		sourceResolved.MarshalToTarget(moduleID, InteropSupport::STLObjectTarget<std::wstring>(systemClassName), InteropSupport::STLObjectTarget<std::wstring>(className), InteropSupport::STLObjectTarget<std::wstring>(instanceName), InteropSupport::STLObjectTarget<std::wstring>(filePath), InteropSupport::STLObjectTarget<std::list<SavedRelationshipExportConnector>>(exportedConnectors), InteropSupport::STLObjectTarget<std::list<SavedRelationshipImportConnector>>(importedConnectors));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& moduleIDMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& systemClassNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& classNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& filePathMarshaller, const InteropSupport::ISTLObjectTarget<std::list<SavedRelationshipExportConnector>>& exportedConnectorsMarshaller, const InteropSupport::ISTLObjectTarget<std::list<SavedRelationshipImportConnector>>& importedConnectorsMarshaller) const
	{
		moduleIDMarshaller = moduleID;
		systemClassNameMarshaller.MarshalFrom(systemClassName);
		classNameMarshaller.MarshalFrom(className);
		instanceNameMarshaller.MarshalFrom(instanceName);
		filePathMarshaller.MarshalFrom(filePath);
		exportedConnectorsMarshaller.MarshalFrom(exportedConnectors);
		importedConnectorsMarshaller.MarshalFrom(importedConnectors);
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
struct ISystemGUIInterface::ConnectorMapping :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const ConnectorMapping& sourceResolved = static_cast<const ConnectorMapping&>(source);
		sourceResolved.MarshalToTarget(connectorID, InteropSupport::STLObjectTarget<std::wstring>(importingModuleConnectorInstanceName));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(unsigned int& connectorIDMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& importingModuleConnectorInstanceNameMarshaller) const
	{
		connectorIDMarshaller = connectorID;
		importingModuleConnectorInstanceNameMarshaller.MarshalFrom(importingModuleConnectorInstanceName);
	}

public:
	unsigned int connectorID;
	std::wstring importingModuleConnectorInstanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ConnectorDefinitionImport :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const ConnectorDefinitionImport& sourceResolved = static_cast<const ConnectorDefinitionImport&>(source);
		sourceResolved.MarshalToTarget(InteropSupport::STLObjectTarget<std::wstring>(className), InteropSupport::STLObjectTarget<std::wstring>(instanceName));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const InteropSupport::ISTLObjectTarget<std::wstring>& classNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller.MarshalFrom(className);
		instanceNameMarshaller.MarshalFrom(instanceName);
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::ConnectorDefinitionExport :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source)
	{
		const ConnectorDefinitionExport& sourceResolved = static_cast<const ConnectorDefinitionExport&>(source);
		sourceResolved.MarshalToTarget(InteropSupport::STLObjectTarget<std::wstring>(className), InteropSupport::STLObjectTarget<std::wstring>(instanceName));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(const InteropSupport::ISTLObjectTarget<std::wstring>& classNameMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& instanceNameMarshaller) const
	{
		classNameMarshaller.MarshalFrom(className);
		instanceNameMarshaller.MarshalFrom(instanceName);
	}

public:
	std::wstring className;
	std::wstring instanceName;
};

//----------------------------------------------------------------------------------------
struct ISystemGUIInterface::SystemLogEntry :protected InteropSupport::IMarshallingObject
{
protected:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& asource)
	{
		const SystemLogEntry& sourceResolved = static_cast<const SystemLogEntry&>(asource);
		sourceResolved.MarshalToTarget(eventLevel, InteropSupport::STLObjectTarget<std::wstring>(text), InteropSupport::STLObjectTarget<std::wstring>(source), InteropSupport::STLObjectTarget<std::wstring>(eventLevelString), InteropSupport::STLObjectTarget<std::wstring>(eventTimeString));
	}

private:
	//Marshalling methods
	virtual void MarshalToTarget(ILogEntry::EventLevel& eventLevelMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& textMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& sourceMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& eventLevelStringMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& eventTimeStringMarshaller) const
	{
		eventLevelMarshaller = eventLevel;
		textMarshaller.MarshalFrom(text);
		sourceMarshaller.MarshalFrom(source);
		eventLevelStringMarshaller.MarshalFrom(eventLevelString);
		eventTimeStringMarshaller.MarshalFrom(eventTimeString);
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

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISystemGUIInterface::ThisISystemGUIInterfaceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Path functions
//----------------------------------------------------------------------------------------
void ISystemGUIInterface::SetCapturePath(const std::wstring& apath)
{
	return SetCapturePathInternal(InteropSupport::STLObjectSource<std::wstring>(apath));
}

//----------------------------------------------------------------------------------------
//Logging functions
//----------------------------------------------------------------------------------------
std::vector<ISystemGUIInterface::SystemLogEntry> ISystemGUIInterface::GetEventLog() const
{
	std::vector<ISystemGUIInterface::SystemLogEntry> result;
	GetEventLogInternal(InteropSupport::STLObjectTarget<std::vector<ISystemGUIInterface::SystemLogEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Embedded ROM functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemGUIInterface::GetEmbeddedROMIDs() const
{
	std::list<unsigned int> result;
	GetEmbeddedROMIDsInternal(InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::SetEmbeddedROMPath(unsigned int embeddedROMID, const std::wstring& filePath)
{
	return SetEmbeddedROMPathInternal(embeddedROMID, InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
//Module setting functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemGUIInterface::GetModuleSettingIDs(unsigned int moduleID) const
{
	std::list<unsigned int> result;
	GetModuleSettingIDsInternal(moduleID, InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Device registration
//----------------------------------------------------------------------------------------
void ISystemGUIInterface::UnregisterDevice(const std::wstring& deviceName)
{
	return UnregisterDeviceInternal(InteropSupport::STLObjectSource<std::wstring>(deviceName));
}

//----------------------------------------------------------------------------------------
//Extension registration
//----------------------------------------------------------------------------------------
void ISystemGUIInterface::UnregisterExtension(const std::wstring& extensionName)
{
	return UnregisterExtensionInternal(InteropSupport::STLObjectSource<std::wstring>(extensionName));
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
ISystemGUIInterface::KeyCode ISystemGUIInterface::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	return GetKeyCodeIDInternal(InteropSupport::STLObjectSource<std::wstring>(keyCodeName));
}

//----------------------------------------------------------------------------------------
std::wstring ISystemGUIInterface::GetKeyCodeName(KeyCode keyCode) const
{
	std::wstring result;
	GetKeyCodeNameInternal(keyCode, InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::list<IDevice*> ISystemGUIInterface::GetInputDeviceList() const
{
	std::list<IDevice*> result;
	GetInputDeviceListInternal(InteropSupport::STLObjectTarget<std::list<IDevice*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemGUIInterface::GetDeviceKeyCodeList(IDevice* targetDevice) const
{
	std::list<unsigned int> result;
	GetDeviceKeyCodeListInternal(targetDevice, InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::list<ISystemGUIInterface::KeyCode> ISystemGUIInterface::GetDeviceKeyCodePreferredDefaultMappingList(IDevice* targetDevice, unsigned int deviceKeyCode) const
{
	std::list<KeyCode> result;
	GetDeviceKeyCodePreferredDefaultMappingListInternal(targetDevice, deviceKeyCode, InteropSupport::STLObjectTarget<std::list<KeyCode>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::LoadState(const std::wstring& filePath, FileType fileType, bool debuggerState)
{
	return LoadStateInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), fileType, debuggerState);
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::SaveState(const std::wstring& filePath, FileType fileType, bool debuggerState)
{
	return SaveStateInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), fileType, debuggerState);
}

//----------------------------------------------------------------------------------------
ISystemGUIInterface::StateInfo ISystemGUIInterface::GetStateInfo(const std::wstring& filePath, FileType fileType) const
{
	StateInfo result;
	GetStateInfoInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), fileType, InteropSupport::STLObjectTarget<StateInfo>(result));
	return result;
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::LoadModuleRelationshipsNode(IHierarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const
{
	return LoadModuleRelationshipsNodeInternal(node, InteropSupport::STLObjectTarget<ModuleRelationshipMap>(relationshipMap));
}

//----------------------------------------------------------------------------------------
void ISystemGUIInterface::SaveModuleRelationshipsNode(IHierarchicalStorageNode& node, bool saveFilePathInfo, const std::wstring& relativePathBase) const
{
	return SaveModuleRelationshipsNodeInternal(node, saveFilePathInfo, InteropSupport::STLObjectSource<std::wstring>(relativePathBase));
}

//----------------------------------------------------------------------------------------
//Module loading and unloading
//----------------------------------------------------------------------------------------
void ISystemGUIInterface::LoadModuleSynchronous(const std::wstring& filePath, const ConnectorMappingList& connectorMappings)
{
	return LoadModuleSynchronousInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), InteropSupport::STLObjectSource<ConnectorMappingList>(connectorMappings));
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::LoadModule(const std::wstring& filePath, const ConnectorMappingList& connectorMappings)
{
	return LoadModuleInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), InteropSupport::STLObjectSource<ConnectorMappingList>(connectorMappings));
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::SaveSystem(const std::wstring& filePath)
{
	return SaveSystemInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::ReadModuleConnectorInfo(const std::wstring& filePath, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const
{
	return ReadModuleConnectorInfoInternal(InteropSupport::STLObjectSource<std::wstring>(filePath), InteropSupport::STLObjectTarget<std::wstring>(systemClassName), InteropSupport::STLObjectTarget<ConnectorImportList>(connectorsImported), InteropSupport::STLObjectTarget<ConnectorExportList>(connectorsExported));
}

//----------------------------------------------------------------------------------------
std::wstring ISystemGUIInterface::LoadModuleSynchronousCurrentModuleName() const
{
	std::wstring result;
	LoadModuleSynchronousCurrentModuleNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring ISystemGUIInterface::UnloadModuleSynchronousCurrentModuleName() const
{
	std::wstring result;
	UnloadModuleSynchronousCurrentModuleNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//View functions
//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::RestoreViewStateForSystem(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState) const
{
	return RestoreViewStateForSystemInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState);
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::RestoreViewStateForModule(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) const
{
	return RestoreViewStateForModuleInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, moduleID);
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::RestoreViewStateForDevice(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& deviceInstanceName) const
{
	return RestoreViewStateForDeviceInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, moduleID, InteropSupport::STLObjectSource<std::wstring>(deviceInstanceName));
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, const std::wstring& extensionInstanceName) const
{
	return RestoreViewStateForExtensionInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, InteropSupport::STLObjectSource<std::wstring>(extensionInstanceName));
}

//----------------------------------------------------------------------------------------
bool ISystemGUIInterface::RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& extensionInstanceName) const
{
	return RestoreViewStateForExtensionInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, moduleID, InteropSupport::STLObjectSource<std::wstring>(extensionInstanceName));
}
