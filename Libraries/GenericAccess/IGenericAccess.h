#ifndef __IGENERICACCESS_H__
#define __IGENERICACCESS_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IGenericAccessDataInfo.h"
#include "IGenericAccessCommandInfo.h"
#include <set>
#include <string>
class IGenericAccessPage;

//##TODO## Make this more generic than just being "IDeviceAccess". We can use this exact
//same interface for extensions, and for the system platform itself. To make this work, we
//need to rename it again, and shift it into a library of its own. I suggest we call it
//IGenericAccess. The library should be called GenericAccess, and it should contain both
//the interfaces and the implementation.
class IGenericAccess
{
public:
	//Nested types
	class DataContext
	{
	public:
		virtual ~DataContext() = 0 {}
	};

public:
	//Constructors
	virtual ~IGenericAccess() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessVersion() { return 1; }
	virtual unsigned int GetIGenericAccessVersion() const = 0;

	//Data info functions
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetGenericDataIDList() const = 0;
	virtual const IGenericAccessDataInfo* GetGenericDataInfo(unsigned int dataID) const = 0;

	//Command info functions
	virtual MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetGenericCommandIDList() const = 0;
	virtual const IGenericAccessCommandInfo* GetGenericCommandInfo(unsigned int commandID) const = 0;

	//Page info functions
	virtual unsigned int GetGenericAccessPageCount() const = 0;
	virtual const IGenericAccessPage* GetGenericAccessPage(unsigned int pageNo) const = 0;

	//Data read/write functions
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const = 0;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) = 0;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, const MarshalSupport::Marshal::Out<std::wstring>& dataValue) const = 0;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, const MarshalSupport::Marshal::In<std::wstring>& dataValue) = 0;
	virtual bool ApplyGenericDataValueLimitSettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const = 0;
	virtual bool ApplyGenericDataValueDisplaySettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const = 0;

	//Data locking functions
	virtual bool GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const = 0;
	virtual bool SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state) = 0;

	//Highlight functions
	virtual bool GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const = 0;

	//Command execution functions
	virtual bool ExecuteGenericCommand(unsigned int commandID, const DataContext* dataContext) = 0;
};

#endif
