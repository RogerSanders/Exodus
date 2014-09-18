#include "GenericAccessDataValueBase.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
template<class B> unsigned int GenericAccessDataValueBase<B>::GetIGenericAccessDataValueVersion() const
{
	return ThisIGenericAccessDataValueVersion();
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
template<class B> void GenericAccessDataValueBase<B>::GetValueStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetValueString());
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
template<class B> bool GenericAccessDataValueBase<B>::SetValueString(const std::wstring& value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFilePath(const std::wstring& value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFolderPath(const std::wstring& value)
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueStringInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	return SetValueString(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	return SetValueFilePath(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessDataValueBase<B>::SetValueFolderPathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	return SetValueFolderPath(marshaller.MarshalTo());
}
