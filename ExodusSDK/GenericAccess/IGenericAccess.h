#ifndef __IGENERICACCESS_H__
#define __IGENERICACCESS_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IGenericAccessDataInfo.h"
#include "IGenericAccessCommandInfo.h"
#include <set>
#include <string>
class IGenericAccessPage;
using namespace MarshalSupport::Operators;

class IGenericAccess
{
public:
	// Nested types
	class DataContext
	{
	public:
		DataContext() = default;
		DataContext(const DataContext& source) = default;
		inline virtual ~DataContext() = 0;
	};

public:
	// Constructors
	inline virtual ~IGenericAccess() = 0;

	// Interface version functions
	static inline unsigned int ThisIGenericAccessVersion() { return 1; }
	virtual unsigned int GetIGenericAccessVersion() const = 0;

	// Data info functions
	virtual Marshal::Ret<std::set<unsigned int>> GetGenericDataIDList() const = 0;
	virtual const IGenericAccessDataInfo* GetGenericDataInfo(unsigned int dataID) const = 0;

	// Command info functions
	virtual Marshal::Ret<std::set<unsigned int>> GetGenericCommandIDList() const = 0;
	virtual const IGenericAccessCommandInfo* GetGenericCommandInfo(unsigned int commandID) const = 0;

	// Page info functions
	virtual unsigned int GetGenericAccessPageCount() const = 0;
	virtual const IGenericAccessPage* GetGenericAccessPage(unsigned int pageNo) const = 0;

	// Data read/write functions
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const = 0;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) = 0;
	virtual bool ReadGenericData(unsigned int dataID, const DataContext* dataContext, const Marshal::Out<std::wstring>& dataValue) const = 0;
	virtual bool WriteGenericData(unsigned int dataID, const DataContext* dataContext, const Marshal::In<std::wstring>& dataValue) = 0;
	virtual bool ApplyGenericDataValueLimitSettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const = 0;
	virtual bool ApplyGenericDataValueDisplaySettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const = 0;

	// Data locking functions
	virtual bool GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const = 0;
	virtual bool SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state) = 0;

	// Highlight functions
	virtual bool GetGenericDataHighlightState(unsigned int dataID, const DataContext* dataContext) const = 0;

	// Command execution functions
	virtual bool ExecuteGenericCommand(unsigned int commandID, const DataContext* dataContext) = 0;
};
IGenericAccess::DataContext::~DataContext() { }
IGenericAccess::~IGenericAccess() { }

#endif
