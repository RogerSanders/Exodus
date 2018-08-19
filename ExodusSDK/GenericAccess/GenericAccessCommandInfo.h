#ifndef __GENERICACCESSCOMMANDINFO_H__
#define __GENERICACCESSCOMMANDINFO_H__
#include "IGenericAccessCommandInfo.h"
#include <string>

class GenericAccessCommandInfo :public IGenericAccessCommandInfo
{
public:
	// Constructors
	template<class T> inline GenericAccessCommandInfo(T id);

	// Interface version functions
	virtual unsigned int GetIGenericAccessCommandInfoVersion() const;

	// Command info functions
	virtual unsigned int GetID() const;
	inline GenericAccessCommandInfo* SetID(unsigned int id);
	virtual bool GetCanExecute() const;
	inline GenericAccessCommandInfo* SetCanExecute(bool canExecute);

private:
	unsigned int _id;
	bool _canExecute;
};

#include "GenericAccessCommandInfo.inl"
#endif
