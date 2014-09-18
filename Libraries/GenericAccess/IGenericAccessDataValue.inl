//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IGenericAccessDataValue::DataType
{
	DATATYPE_BOOL,
	DATATYPE_INT,
	DATATYPE_UINT,
	DATATYPE_FLOAT,
	DATATYPE_DOUBLE,
	DATATYPE_STRING,
	DATATYPE_FILEPATH,
	DATATYPE_FOLDERPATH
};

//----------------------------------------------------------------------------------------
enum IGenericAccessDataValue::IntDisplayMode
{
	INTDISPLAYMODE_BINARY,
	INTDISPLAYMODE_OCTAL,
	INTDISPLAYMODE_DECIMAL,
	INTDISPLAYMODE_HEXADECIMAL
};

//----------------------------------------------------------------------------------------
enum IGenericAccessDataValue::FloatDisplayMode
{
	FLOATDISPLAYMODE_FIXED,
	FLOATDISPLAYMODE_SCIENTIFIC
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessDataValue::ThisIGenericAccessDataValueVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataValue::GetValueString() const
{
	std::wstring value;
	GetValueStringInternal(InteropSupport::STLObjectTarget<std::wstring>(value));
	return value;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
template<class T> bool IGenericAccessDataValue::SetValue(const T& value)
{
	//##TODO## Enable our static assertion method below, and replace the missing semicolon
	//on the return statement, once we have C++11 support.
	//std::static_assert(false, L"IGenericAccessDataValue::SetValue called with an unsupported type!");
	return false //;
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<bool>(const bool& value)
{
	return SetValueBool(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<volatile bool>(const volatile bool& value)
{
	return SetValueBool(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<int>(const int& value)
{
	return SetValueInt(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<volatile int>(const volatile int& value)
{
	return SetValueInt(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<unsigned int>(const unsigned int& value)
{
	return SetValueUInt(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<volatile unsigned int>(const volatile unsigned int& value)
{
	return SetValueUInt(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<float>(const float& value)
{
	return SetValueFloat(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<volatile float>(const volatile float& value)
{
	return SetValueFloat(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<double>(const double& value)
{
	return SetValueDouble(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<volatile double>(const volatile double& value)
{
	return SetValueDouble(value);
}

//----------------------------------------------------------------------------------------
template<> bool IGenericAccessDataValue::SetValue<std::wstring>(const std::wstring& value)
{
	return SetValueStringInternal(InteropSupport::STLObjectSource<std::wstring>(value));
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValueString(const std::wstring& value)
{
	return SetValueStringInternal(InteropSupport::STLObjectSource<std::wstring>(value));
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValueFilePath(const std::wstring& value)
{
	return SetValueFilePathInternal(InteropSupport::STLObjectSource<std::wstring>(value));
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValueFolderPath(const std::wstring& value)
{
	return SetValueFolderPathInternal(InteropSupport::STLObjectSource<std::wstring>(value));
}
