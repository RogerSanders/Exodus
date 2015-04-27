#ifndef __GENERICACCESSBASE_H__
#define __GENERICACCESSBASE_H__
#include "IGenericAccess.h"
#include <map>
#include <vector>

template<class B> class GenericAccessBase :public B
{
public:
	//Constructors
	virtual ~GenericAccessBase();

	//Interface version functions
	virtual unsigned int GetIGenericAccessVersion() const;

	//Data info functions
	MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetGenericDataIDList() const;
	virtual const IGenericAccessDataInfo* GetGenericDataInfo(unsigned int dataID) const;
	bool AddGenericDataInfo(const IGenericAccessDataInfo* dataInfo);

	//Command info functions
	MarshalSupport::Marshal::Ret<std::set<unsigned int>> GetGenericCommandIDList() const;
	virtual const IGenericAccessCommandInfo* GetGenericCommandInfo(unsigned int commandID) const;
	bool AddGenericCommandInfo(const IGenericAccessCommandInfo* commandInfo);

	//Page info functions
	virtual unsigned int GetGenericAccessPageCount() const;
	virtual const IGenericAccessPage* GetGenericAccessPage(unsigned int pageNo) const;
	bool AddGenericAccessPage(const IGenericAccessPage* page);

	//Data read/write functions
	using B::ReadGenericData;
	using B::WriteGenericData;
	bool ReadGenericData(unsigned int dataID, const typename B::DataContext* dataContext, const MarshalSupport::Marshal::Out<std::wstring>& dataValue) const;
	bool WriteGenericData(unsigned int dataID, const typename B::DataContext* dataContext, const MarshalSupport::Marshal::In<std::wstring>& dataValue);
	virtual bool ApplyGenericDataValueLimitSettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const;
	virtual bool ApplyGenericDataValueDisplaySettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const;

	//Data locking functions
	virtual bool GetGenericDataLocked(unsigned int dataID, const typename B::DataContext* dataContext) const;
	virtual bool SetGenericDataLocked(unsigned int dataID, const typename B::DataContext* dataContext, bool state);

	//Highlight functions
	virtual bool GetGenericDataHighlightState(unsigned int dataID, const typename B::DataContext* dataContext) const;

	//Command execution functions
	virtual bool ExecuteGenericCommand(unsigned int commandID, const typename B::DataContext* dataContext);

private:
	std::map<unsigned int, const IGenericAccessDataInfo*> genericDataList;
	std::map<unsigned int, const IGenericAccessCommandInfo*> genericCommandList;
	std::vector<const IGenericAccessPage*> genericPageList;
};

#include "GenericAccessBase.inl"
#endif
