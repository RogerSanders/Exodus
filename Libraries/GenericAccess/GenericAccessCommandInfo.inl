//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessCommandInfo::GenericAccessCommandInfo(unsigned int aid)
:id(aid)
{
	//Initialize the general command info to defaults
	canExecute = true;
}

//----------------------------------------------------------------------------------------
//Command info functions
//----------------------------------------------------------------------------------------
GenericAccessCommandInfo* GenericAccessCommandInfo::SetID(unsigned int aid)
{
	id = aid;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessCommandInfo* GenericAccessCommandInfo::SetCanExecute(bool acanExecute)
{
	canExecute = acanExecute;
	return this;
}
