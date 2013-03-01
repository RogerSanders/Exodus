#ifndef __ISYSTEMEXTENSIONINTERFACE_H__
#define __ISYSTEMEXTENSIONINTERFACE_H__
#include "ILogEntry.h"
#include "ILoadedModuleInfo.h"
#include "IConnectorInfo.h"
#include <list>

class ISystemExtensionInterface
{
public:
	//Constructors
	virtual ~ISystemExtensionInterface() = 0 {}

	//Interface version functions
	static inline unsigned int ThisISystemExtensionInterfaceVersion();
	virtual unsigned int GetISystemExtensionInterfaceVersion() const = 0;

	//Logging functions
	virtual void WriteLogEvent(const ILogEntry& entry) const = 0;

	//System interface functions
	virtual void Initialize() = 0;
	virtual void RunSystem() = 0;
	virtual void StopSystem() = 0;
	virtual bool SystemRunning() const = 0;

	//Loaded module info functions
	inline std::list<unsigned int> GetLoadedModuleIDs() const;
	virtual bool GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const = 0;

	//Connector info functions
	inline std::list<unsigned int> GetConnectorIDs() const;
	virtual bool GetConnectorInfo(unsigned int connectorID, IConnectorInfo& connectorInfo) const = 0;

protected:
	//Loaded module info functions
	virtual void GetLoadedModuleIDsInternal(unsigned int itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const = 0;

	//Connector info functions
	virtual void GetConnectorIDsInternal(unsigned int itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const = 0;
};

#include "ISystemExtensionInterface.inl"
#endif
