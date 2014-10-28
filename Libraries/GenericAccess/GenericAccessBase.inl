//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class B> GenericAccessBase<B>::~GenericAccessBase()
{
	//Delete all pages from the page list
	for(size_t i = 0; i < genericPageList.size(); ++i)
	{
		delete genericPageList[i];
	}

	//Delete all entries from the data list
	for(std::map<unsigned int, const IGenericAccessDataInfo*>::iterator i = genericDataList.begin(); i != genericDataList.end(); ++i)
	{
		delete i->second;
	}
	genericDataList.clear();

	//Delete all entries from the command list
	for(std::map<unsigned int, const IGenericAccessCommandInfo*>::iterator i = genericCommandList.begin(); i != genericCommandList.end(); ++i)
	{
		delete i->second;
	}
	genericCommandList.clear();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
template<class B> unsigned int GenericAccessBase<B>::GetIGenericAccessVersion() const
{
	return ThisIGenericAccessVersion();
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
template<class B> MarshalSupport::Marshal::Ret<std::set<unsigned int>> GenericAccessBase<B>::GetGenericDataIDList() const
{
	std::set<unsigned int> dataIDSet;
	for(std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator i = genericDataList.begin(); i != genericDataList.end(); ++i)
	{
		dataIDSet.insert(i->first);
	}
	return dataIDSet;
}

//----------------------------------------------------------------------------------------
template<class B> const IGenericAccessDataInfo* GenericAccessBase<B>::GetGenericDataInfo(unsigned int dataID) const
{
	//Attempt to retrieve the target data info from the list
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator == genericDataList.end())
	{
		return 0;
	}
	const IGenericAccessDataInfo* targetDataInfo = dataListIterator->second;

	//Return the info for the specified data item to the caller
	return targetDataInfo;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::AddGenericDataInfo(const IGenericAccessDataInfo* dataInfo)
{
	//Retrieve the ID number for this data entry
	unsigned int dataID = dataInfo->GetID();

	//Ensure a data entry with the specified ID hasn't already been defined
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator != genericDataList.end())
	{
		return false;
	}

	//Add this data entry to the generic data list
	genericDataList[dataID] = dataInfo;
	return true;
}

//----------------------------------------------------------------------------------------
//Command info functions
//----------------------------------------------------------------------------------------
template<class B> MarshalSupport::Marshal::Ret<std::set<unsigned int>> GenericAccessBase<B>::GetGenericCommandIDList() const
{
	std::set<unsigned int> commandIDSet;
	for(std::map<unsigned int, const IGenericAccessCommandInfo*>::const_iterator i = genericCommandList.begin(); i != genericCommandList.end(); ++i)
	{
		commandIDSet.insert(i->first);
	}
	return commandIDSet;
}

//----------------------------------------------------------------------------------------
template<class B> const IGenericAccessCommandInfo* GenericAccessBase<B>::GetGenericCommandInfo(unsigned int commandID) const
{
	//Attempt to retrieve the target command info from the list
	std::map<unsigned int, const IGenericAccessCommandInfo*>::const_iterator commandListIterator = genericCommandList.find(commandID);
	if(commandListIterator == genericCommandList.end())
	{
		return 0;
	}
	const IGenericAccessCommandInfo* targetCommandInfo = commandListIterator->second;

	//Return the info for the specified command item to the caller
	return targetCommandInfo;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::AddGenericCommandInfo(const IGenericAccessCommandInfo* commandInfo)
{
	//Retrieve the ID number for this command entry
	unsigned int commandID = commandInfo->GetID();

	//Ensure a command entry with the specified ID hasn't already been defined
	std::map<unsigned int, const IGenericAccessCommandInfo*>::const_iterator commandListIterator = genericCommandList.find(commandID);
	if(commandListIterator != genericCommandList.end())
	{
		return false;
	}

	//Add this command entry to the generic command list
	genericCommandList[commandID] = commandInfo;
	return true;
}

//----------------------------------------------------------------------------------------
//Page info functions
//----------------------------------------------------------------------------------------
template<class B> unsigned int GenericAccessBase<B>::GetGenericAccessPageCount() const
{
	return (unsigned int)genericPageList.size();
}

//----------------------------------------------------------------------------------------
template<class B> const IGenericAccessPage* GenericAccessBase<B>::GetGenericAccessPage(unsigned int pageNo) const
{
	//Ensure the requested page exists
	if((size_t)pageNo >= genericPageList.size())
	{
		return 0;
	}

	//Return the requested page to the caller
	return genericPageList[pageNo];
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::AddGenericAccessPage(const IGenericAccessPage* page)
{
	genericPageList.push_back(page);
	return true;
}

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::ReadGenericData(unsigned int dataID, const typename B::DataContext* dataContext, const MarshalSupport::Marshal::Out<std::wstring>& dataValue) const
{
	//Attempt to retrieve the target data info from the list
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator == genericDataList.end())
	{
		return false;
	}
	const IGenericAccessDataInfo* targetDataInfo = dataListIterator->second;

	//Attempt to read the target value as a string
	switch(targetDataInfo->GetType())
	{
	case IGenericAccessDataValue::DataType::Bool:{
		GenericAccessDataValueBool genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::Int:{
		GenericAccessDataValueInt genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::UInt:{
		GenericAccessDataValueUInt genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::Float:{
		GenericAccessDataValueFloat genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::Double:{
		GenericAccessDataValueDouble genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::String:{
		GenericAccessDataValueString genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::FilePath:{
		GenericAccessDataValueFilePath genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	case IGenericAccessDataValue::DataType::FolderPath:{
		GenericAccessDataValueFolderPath genericDataValue;
		if(!ReadGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		dataValue = genericDataValue.GetValueString();
		break;}
	default:
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::WriteGenericData(unsigned int dataID, const typename B::DataContext* dataContext, const MarshalSupport::Marshal::In<std::wstring>& dataValue)
{
	//Attempt to retrieve the target data info from the list
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator == genericDataList.end())
	{
		return false;
	}
	const IGenericAccessDataInfo* targetDataInfo = dataListIterator->second;

	//Attempt to write the target value
	switch(targetDataInfo->GetType())
	{
	case IGenericAccessDataValue::DataType::Bool:{
		GenericAccessDataValueBool genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::Int:{
		GenericAccessDataValueInt genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::UInt:{
		GenericAccessDataValueUInt genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::Float:{
		GenericAccessDataValueFloat genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::Double:{
		GenericAccessDataValueDouble genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::String:{
		GenericAccessDataValueString genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::FilePath:{
		GenericAccessDataValueFilePath genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	case IGenericAccessDataValue::DataType::FolderPath:{
		GenericAccessDataValueFolderPath genericDataValue;
		ApplyGenericDataValueDisplaySettings(dataID, genericDataValue);
		if(!genericDataValue.SetValueString(dataValue))
		{
			return false;
		}
		if(!WriteGenericData(dataID, dataContext, genericDataValue))
		{
			return false;
		}
		break;}
	default:
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::ApplyGenericDataValueLimitSettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const
{
	//Attempt to retrieve the target data info from the list
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator == genericDataList.end())
	{
		return false;
	}
	const IGenericAccessDataInfo* targetDataInfo = dataListIterator->second;

	//Load any specified data limits for the target data into the data value
	switch(dataValue.GetType())
	{
	case IGenericAccessDataValue::DataType::Bool:{
		//IGenericGenericDataValueBool& dataValueTyped = (IGenericGenericDataValueBool&)dataValue;
		break;}
	case IGenericAccessDataValue::DataType::Int:{
		IGenericAccessDataValueInt& dataValueTyped = (IGenericAccessDataValueInt&)dataValue;
		dataValueTyped.SetMinValue(targetDataInfo->GetIntMinValue());
		dataValueTyped.SetMaxValue(targetDataInfo->GetIntMaxValue());
		break;}
	case IGenericAccessDataValue::DataType::UInt:{
		IGenericAccessDataValueUInt& dataValueTyped = (IGenericAccessDataValueUInt&)dataValue;
		dataValueTyped.SetMinValue(targetDataInfo->GetUIntMinValue());
		dataValueTyped.SetMaxValue(targetDataInfo->GetUIntMaxValue());
		break;}
	case IGenericAccessDataValue::DataType::Float:{
		IGenericAccessDataValueFloat& dataValueTyped = (IGenericAccessDataValueFloat&)dataValue;
		dataValueTyped.SetMinValue(targetDataInfo->GetFloatMinValue());
		dataValueTyped.SetMaxValue(targetDataInfo->GetFloatMaxValue());
		break;}
	case IGenericAccessDataValue::DataType::Double:{
		IGenericAccessDataValueDouble& dataValueTyped = (IGenericAccessDataValueDouble&)dataValue;
		dataValueTyped.SetMinValue(targetDataInfo->GetDoubleMinValue());
		dataValueTyped.SetMaxValue(targetDataInfo->GetDoubleMaxValue());
		break;}
	case IGenericAccessDataValue::DataType::String:{
		IGenericAccessDataValueString& dataValueTyped = (IGenericAccessDataValueString&)dataValue;
		dataValueTyped.SetMaxLength(targetDataInfo->GetStringMaxLength());
		break;}
	case IGenericAccessDataValue::DataType::FilePath:{
		IGenericAccessDataValueFilePath& dataValueTyped = (IGenericAccessDataValueFilePath&)dataValue;
		dataValueTyped.SetMaxLength(targetDataInfo->GetStringMaxLength());
		break;}
	case IGenericAccessDataValue::DataType::FolderPath:{
		IGenericAccessDataValueFolderPath& dataValueTyped = (IGenericAccessDataValueFolderPath&)dataValue;
		dataValueTyped.SetMaxLength(targetDataInfo->GetStringMaxLength());
		break;}
	default:
		return false;
	}

	//Now that we've set the correct limits for this data value, apply those limits to the
	//currently stored data.
	dataValue.ApplyLimitSettingsToCurrentValue();

	return true;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::ApplyGenericDataValueDisplaySettings(unsigned int dataID, IGenericAccessDataValue& dataValue) const
{
	//Attempt to retrieve the target data info from the list
	std::map<unsigned int, const IGenericAccessDataInfo*>::const_iterator dataListIterator = genericDataList.find(dataID);
	if(dataListIterator == genericDataList.end())
	{
		return false;
	}
	const IGenericAccessDataInfo* dataInfo = dataListIterator->second;

	//Load any specified display settings for the target data into the data value
	switch(dataValue.GetType())
	{
	case IGenericAccessDataValue::DataType::Bool:{
		//IGenericAccessDataValueBool& dataValueTyped = (IGenericAccessDataValueBool&)dataValue;
		break;}
	case IGenericAccessDataValue::DataType::Int:{
		IGenericAccessDataValueInt& dataValueTyped = (IGenericAccessDataValueInt&)dataValue;
		dataValueTyped.SetDisplayMode(dataInfo->GetIntDisplayMode());
		unsigned int displayChars = dataInfo->GetMinWholeNumberChars();
		if(displayChars == 0)
		{
			displayChars = dataValueTyped.CalculateDisplayChars(dataInfo->GetIntDisplayMode(), dataInfo->GetIntMinValue(), dataInfo->GetIntMaxValue());
		}
		dataValueTyped.SetMinChars(displayChars);
		break;}
	case IGenericAccessDataValue::DataType::UInt:{
		IGenericAccessDataValueUInt& dataValueTyped = (IGenericAccessDataValueUInt&)dataValue;
		dataValueTyped.SetDisplayMode(dataInfo->GetIntDisplayMode());
		unsigned int displayChars = dataInfo->GetMinWholeNumberChars();
		if(displayChars == 0)
		{
			displayChars = dataValueTyped.CalculateDisplayChars(dataInfo->GetIntDisplayMode(), dataInfo->GetUIntMinValue(), dataInfo->GetUIntMaxValue());
		}
		dataValueTyped.SetMinChars(displayChars);
		break;}
	case IGenericAccessDataValue::DataType::Float:{
		IGenericAccessDataValueFloat& dataValueTyped = (IGenericAccessDataValueFloat&)dataValue;
		dataValueTyped.SetDisplayMode(dataInfo->GetFloatDisplayMode());
		dataValueTyped.SetMinWholeNumberChars(dataInfo->GetMinWholeNumberChars());
		dataValueTyped.SetMinFractionalNumberChars(dataInfo->GetMinFractionalNumberChars());
		break;}
	case IGenericAccessDataValue::DataType::Double:{
		IGenericAccessDataValueDouble& dataValueTyped = (IGenericAccessDataValueDouble&)dataValue;
		dataValueTyped.SetDisplayMode(dataInfo->GetFloatDisplayMode());
		dataValueTyped.SetMinWholeNumberChars(dataInfo->GetMinWholeNumberChars());
		dataValueTyped.SetMinFractionalNumberChars(dataInfo->GetMinFractionalNumberChars());
		break;}
	case IGenericAccessDataValue::DataType::String:{
		//IGenericAccessDataValueString& dataValueTyped = (IGenericAccessDataValueString&)dataValue;
		break;}
	case IGenericAccessDataValue::DataType::FilePath:{
		//IGenericAccessDataValueFilePath& dataValueTyped = (IGenericAccessDataValueFilePath&)dataValue;
		break;}
	case IGenericAccessDataValue::DataType::FolderPath:{
		//IGenericAccessDataValueFolderPath& dataValueTyped = (IGenericAccessDataValueFolderPath&)dataValue;
		break;}
	default:
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Data locking functions
//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::GetGenericDataLocked(unsigned int dataID, const typename B::DataContext* dataContext) const
{
	return false;
}

//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::SetGenericDataLocked(unsigned int dataID, const typename B::DataContext* dataContext, bool state)
{
	return false;
}

//----------------------------------------------------------------------------------------
//Command execution functions
//----------------------------------------------------------------------------------------
template<class B> bool GenericAccessBase<B>::ExecuteGenericCommand(unsigned int commandID, const typename B::DataContext* dataContext)
{
	return false;
}
