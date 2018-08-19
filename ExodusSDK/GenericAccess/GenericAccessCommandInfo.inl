//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
template<class T> GenericAccessCommandInfo::GenericAccessCommandInfo(T id)
:_id((unsigned int)id)
{
	// Initialize the general command info to defaults
	_canExecute = true;
}

//----------------------------------------------------------------------------------------------------------------------
// Command info functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessCommandInfo* GenericAccessCommandInfo::SetID(unsigned int id)
{
	_id = id;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessCommandInfo* GenericAccessCommandInfo::SetCanExecute(bool canExecute)
{
	_canExecute = canExecute;
	return this;
}
