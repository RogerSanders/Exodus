//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IGenericAccessDataValue::DataType
{
	Bool,
	Int,
	UInt,
	Float,
	Double,
	String,
	FilePath,
	FolderPath
};

//----------------------------------------------------------------------------------------
enum class IGenericAccessDataValue::IntDisplayMode
{
	Binary,
	Octal,
	Decimal,
	Hexadecimal
};

//----------------------------------------------------------------------------------------
enum class IGenericAccessDataValue::FloatDisplayMode
{
	Fixed,
	Scientific
};

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(bool value)
{
	return SetValueBool(value);
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(int value)
{
	return SetValueInt(value);
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(unsigned int value)
{
	return SetValueUInt(value);
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(float value)
{
	return SetValueFloat(value);
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(double value)
{
	return SetValueDouble(value);
}

//----------------------------------------------------------------------------------------
bool IGenericAccessDataValue::SetValue(const std::wstring& value)
{
	return SetValueString(value);
}
