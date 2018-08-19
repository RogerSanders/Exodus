#include "GenericAccessCommandInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessCommandInfo::GetIGenericAccessCommandInfoVersion() const
{
	return ThisIGenericAccessCommandInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Command info functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessCommandInfo::GetID() const
{
	return _id;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessCommandInfo::GetCanExecute() const
{
	return _canExecute;
}
