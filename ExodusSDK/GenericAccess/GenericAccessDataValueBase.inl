#include "GenericAccessDataValueBase.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
template<class B> unsigned int GenericAccessDataValueBase<B>::GetIGenericAccessDataValueVersion() const
{
	return ThisIGenericAccessDataValueVersion();
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueBool(bool value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueInt(int value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueUInt(unsigned int value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFloat(float value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueDouble(double value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueString(const MarshalSupport::Marshal::In<std::wstring>& value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFilePath(const MarshalSupport::Marshal::In<std::wstring>& value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFolderPath(const MarshalSupport::Marshal::In<std::wstring>& value)
{
	return false;
}
