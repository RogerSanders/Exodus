#ifndef __GENERICACCESSCOMMANDINFO_H__
#define __GENERICACCESSCOMMANDINFO_H__
#include "IGenericAccessCommandInfo.h"
#include <string>

class GenericAccessCommandInfo :public IGenericAccessCommandInfo
{
public:
	//Constructors
	template<class T> inline GenericAccessCommandInfo(T aid);

	//Interface version functions
	virtual unsigned int GetIGenericAccessCommandInfoVersion() const;

	//Command info functions
	virtual unsigned int GetID() const;
	inline GenericAccessCommandInfo* SetID(unsigned int aid);
	virtual bool GetCanExecute() const;
	inline GenericAccessCommandInfo* SetCanExecute(bool acanExecute);

private:
	unsigned int id;
	bool canExecute;
};

#include "GenericAccessCommandInfo.inl"
#endif
