//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessValue<DataType, TimesliceType>::TimesliceEntry
{
	TimesliceType timesliceLength;
	bool committed;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessValue<DataType, TimesliceType>::WriteEntry
{
	WriteEntry()
	{}
	WriteEntry(const DataType& defaultValue)
	:newValue(defaultValue)
	{}
	WriteEntry(TimesliceType awriteTime, const DataType& anewValue, const Timeslice& acurrentTimeslice)
	:writeTime(awriteTime), newValue(anewValue), currentTimeslice(acurrentTimeslice)
	{}

	TimesliceType writeTime;
	DataType newValue;
	Timeslice currentTimeslice;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessValue<DataType, TimesliceType>::WriteInfo
{
	WriteInfo()
	{}
	WriteInfo(const DataType& defaultValue)
	:newValue(defaultValue)
	{}

	bool exists;
	TimesliceType writeTime;
	DataType newValue;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessValue<DataType, TimesliceType>::TimesliceSaveEntry
{
	TimesliceSaveEntry(typename const std::list<TimesliceEntry>::const_iterator& atimeslice, unsigned int aid)
	:timeslice(atimeslice), id(aid)
	{}
	TimesliceSaveEntry(unsigned int aid, TimesliceType atimesliceLength)
	:id(aid), timesliceLength(atimesliceLength)
	{}
	typename std::list<TimesliceEntry>::const_iterator timeslice;
	Timeslice timesliceLoad;
	unsigned int id;
	TimesliceType timesliceLength;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessValue<DataType, TimesliceType>::WriteSaveEntry
{
	WriteSaveEntry(TimesliceType awriteTime, const DataType& aoldValue, unsigned int acurrentTimeslice)
	:writeTime(awriteTime), oldValue(aoldValue), currentTimeslice(acurrentTimeslice)
	{}
	TimesliceType writeTime;
	DataType oldValue;
	unsigned int currentTimeslice;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessValue<DataType, TimesliceType>::RandomTimeAccessValue()
{}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessValue<DataType, TimesliceType>::RandomTimeAccessValue(const DataType& defaultValue)
:value(defaultValue)
{}

//----------------------------------------------------------------------------------------
//Dereference operators
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> const DataType& RandomTimeAccessValue<DataType, TimesliceType>::operator*() const
{
	return value;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType& RandomTimeAccessValue<DataType, TimesliceType>::operator*()
{
	return value;
}

//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessValue<DataType, TimesliceType>::Read(TimesliceType readTime) const
{
	boost::mutex::scoped_lock lock(accessLock);

	//Search for written values in the current timeslice
	std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice))
	{
		if(i->writeTime <= readTime)
		{
			return i->newValue;
		}
		++i;
	}

	if(i != writeList.rend())
	{
		//Return the latest write from a previous timeslice
		return i->newValue;
	}
	else
	{
		//Default to the committed value
		return value;
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::Write(TimesliceType writeTime, const DataType& data)
{
	boost::mutex::scoped_lock lock(accessLock);
	WriteEntry entry(writeTime, data, latestTimeslice);

	//Find the correct location in the list to insert the new write entry. The writeList
	//must be sorted from earliest to latest write by time.
	std::list<WriteEntry>::reverse_iterator i = writeList.rbegin();
	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice) && (i->writeTime > writeTime))
	{
		++i;
	}
	writeList.insert(i.base(), entry);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessValue<DataType, TimesliceType>::ReadCommitted() const
{
	return value;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessValue<DataType, TimesliceType>::ReadCommitted(TimesliceType readTime) const
{
	boost::mutex::scoped_lock lock(accessLock);
	TimesliceType currentTimeBase = 0;

	//Default to the committed value
	DataType foundValue = value;

	//Search for any buffered writes before the read time
	std::list<TimesliceEntry>::const_iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::const_iterator i = writeList.begin();
	bool done = false;
	while((i != writeList.end()) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice)
			&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= readTime))
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->currentTimeslice) || (((currentTimeBase + i->writeTime) - currentTimeOffset) > readTime))
		{
			done = true;
			continue;
		}
		//Set the found write as the value at the time of the read, and advance to the
		//next buffered write.
		foundValue = i->newValue;
		++i;
	}

	return foundValue;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::WriteCommitted(const DataType& data)
{
	value = data;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessValue<DataType, TimesliceType>::ReadLatest() const
{
	boost::mutex::scoped_lock lock(accessLock);

	//Search for written values in any timeslice
	std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
	if(i != writeList.rend())
	{
		//Return the latest write from any timeslice
		return i->newValue;
	}
	else
	{
		//Default to the committed value
		return value;
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::WriteLatest(const DataType& data)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Erase any write entries to this address in any timeslice. We do this to prevent
	//uncommitted writes from overwriting this change. This write function should make
	//the new value visible from all access functions.
	writeList.clear();

	//Write the new value directly to the committed state
	value = data;
}

//----------------------------------------------------------------------------------------
//Time management functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::Initialize()
{
	value = DataType();

	//Initialize buffers
	writeList.clear();
	timesliceList.clear();
	currentTimeOffset = 0;
	latestTimeslice = timesliceList.end();
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::DoesLatestTimesliceExist() const
{
	boost::mutex::scoped_lock lock(accessLock);
	return !timesliceList.empty();
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> typename RandomTimeAccessValue<DataType, TimesliceType>::Timeslice RandomTimeAccessValue<DataType, TimesliceType>::GetLatestTimeslice()
{
	boost::mutex::scoped_lock lock(accessLock);

	if(timesliceList.empty())
	{
		return timesliceList.end();
	}
	else
	{
		return (++timesliceList.rbegin()).base();
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::AdvancePastTimeslice(const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Commit buffered writes which we have passed in this step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	bool done = false;
	while((i != writeList.end()) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice) && (currentTimeslice != targetTimeslice))
		{
			++currentTimeslice;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if(currentTimeslice != i->currentTimeslice)
		{
			done = true;
			continue;
		}
		value = i->newValue;
		++i;
	}
	//Check for any remaining timeslices which have expired
	while(currentTimeslice != targetTimeslice)
	{
		++currentTimeslice;
	}
	//Set our current time offset to the end of the target timeslice
	currentTimeOffset = targetTimeslice->timesliceLength;

	//Erase buffered writes which have been committed, and timeslices which have expired.
	writeList.erase(writeList.begin(), i);
	timesliceList.erase(timesliceList.begin(), targetTimeslice);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::AdvanceToTimeslice(const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Commit buffered writes which we have passed in this step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	bool done = false;
	while((i != writeList.end()) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice) && (currentTimeslice != targetTimeslice))
		{
			++currentTimeslice;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->currentTimeslice) || (currentTimeslice == targetTimeslice))
		{
			done = true;
			continue;
		}
		value = i->newValue;
		++i;
	}
	//Check for any remaining timeslices which have expired
	while(currentTimeslice != targetTimeslice)
	{
		++currentTimeslice;
	}
	//Set our current time offset to the start of the target timeslice
	currentTimeOffset = 0;

	//Erase buffered writes which have been committed, and timeslices which have expired.
	writeList.erase(writeList.begin(), i);
	timesliceList.erase(timesliceList.begin(), targetTimeslice);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::AdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);
	TimesliceType currentTimeBase = 0;

	//Commit buffered writes which we have passed in this step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	bool done = false;
	while((i != writeList.end()) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice)
			&& (currentTimeslice != targetTimeslice)
			&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step))
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->currentTimeslice) || (((currentTimeBase + i->writeTime) - currentTimeOffset) > step))
		{
			done = true;
			continue;
		}
		value = i->newValue;
		++i;
	}
	//Check for any remaining timeslices which have expired
	while((currentTimeslice != targetTimeslice)
		&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step))
	{
		currentTimeBase += currentTimeslice->timesliceLength;
		++currentTimeslice;
	}
	//Set the amount of the current timeslice which has been stepped through as the time
	//offset for the next step operation.
	currentTimeOffset = (currentTimeOffset + step) - currentTimeBase;

	//Erase buffered writes which have been committed, and timeslices which have expired.
	writeList.erase(writeList.begin(), i);
	timesliceList.erase(timesliceList.begin(), currentTimeslice);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::AdvanceByStep(const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);
	TimesliceType currentTimeBase = 0;
	TimesliceType writeTime = targetTimeslice->timesliceLength;
	bool foundWrite = false;

	//Commit buffered writes which we have passed in this step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	if(!writeList.empty())
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		//If the next buffered write is within the time step, save the write time and
		//commit the data.
		if(currentTimeslice == i->currentTimeslice)
		{
			foundWrite = true;
			writeTime = i->writeTime;
			value = i->newValue;
			++i;
		}
	}
	//Check for any remaining timeslices which have expired.
	if(!foundWrite)
	{
		while(currentTimeslice != targetTimeslice)
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
	}

	//Set the amount of the current timeslice which has been stepped through as the time
	//offset for the next step operation.
	currentTimeOffset = writeTime;

	//Erase buffered writes which have been committed, and timeslices which have expired.
	writeList.erase(writeList.begin(), i);
	timesliceList.erase(timesliceList.begin(), currentTimeslice);

	return foundWrite;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimesliceType RandomTimeAccessValue<DataType, TimesliceType>::GetNextWriteTime(const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);
	bool foundWrite = false;
	TimesliceType nextWriteTime = 0;
	TimesliceType currentTimeBase = 0;

	//Search the write list for the next buffered write inside this time step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	if(!writeList.empty())
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		//If the next buffered write is within the time step, save the write time.
		if(currentTimeslice == i->currentTimeslice)
		{
			foundWrite = true;
			nextWriteTime = ((currentTimeBase + i->writeTime) - currentTimeOffset);
		}
	}
	//If no write is pending within the time step, output the end of the target timeslice
	//as the next write time.
	if(!foundWrite)
	{
		while(currentTimeslice != targetTimeslice)
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		nextWriteTime = ((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset);
	}

	return nextWriteTime;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> typename RandomTimeAccessValue<DataType, TimesliceType>::WriteInfo RandomTimeAccessValue<DataType, TimesliceType>::GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);
	TimesliceType currentTimeBase = 0;
	unsigned int currentIndex = 0;
	WriteInfo writeInfo;
	writeInfo.exists = false;

	//Search the write list for the next buffered write inside this time step
	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::iterator i = writeList.begin();
	bool done = false;
	while((i != writeList.end()) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			currentTimeBase += currentTimeslice->timesliceLength;
			++currentTimeslice;
		}
		//If the next buffered write is within the time step, output its data
		if(currentTimeslice == i->currentTimeslice)
		{
			if(currentIndex == index)
			{
				writeInfo.exists = true;
				writeInfo.newValue = i->newValue;
				writeInfo.writeTime = ((currentTimeBase + i->writeTime) - currentTimeOffset);
			}
			else
			{
				++currentIndex;
				continue;
			}
		}
		done = true;
	}

	return writeInfo;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::Commit()
{
	boost::mutex::scoped_lock lock(accessLock);

	//Flag all timeslices as committed
	std::list<TimesliceEntry>::reverse_iterator i = timesliceList.rbegin();
	while((i != timesliceList.rend()) && (!i->committed))
	{
		i->committed = true;
		++i;
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::Rollback()
{
	boost::mutex::scoped_lock lock(accessLock);

	//Erase non-committed memory writes
	std::list<WriteEntry>::reverse_iterator i = writeList.rbegin();
	while((i != writeList.rend()) && (!i->currentTimeslice->committed))
	{
		++i;
	}
	writeList.erase(i.base(), writeList.end());

	//Erase non-committed timeslice entries
	std::list<TimesliceEntry>::reverse_iterator j = timesliceList.rbegin();
	while((j != timesliceList.rend()) && (!j->committed))
	{
		++j;
	}
	timesliceList.erase(j.base(), timesliceList.end());

	//Recalculate the latest timeslice
	if(timesliceList.empty())
	{
		latestTimeslice = timesliceList.end();
	}
	else
	{
		latestTimeslice = (++timesliceList.rbegin()).base();
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessValue<DataType, TimesliceType>::AddTimeslice(TimesliceType timeslice)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Add the new timeslice entry to the list
	TimesliceEntry entry;
	entry.timesliceLength = timeslice;
	entry.committed = false;
	timesliceList.push_back(entry);

	//Select the new timeslice entry as the latest timeslice
	latestTimeslice = (++timesliceList.rbegin()).base();
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::LoadState(IHierarchicalStorageNode& node)
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	std::list<WriteSaveEntry> writeSaveList;

	//Load the current time offset
	node.ExtractAttribute(L"CurrentTimeOffset", currentTimeOffset);

	//Read saved data from the XML tree
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"TimesliceList")
		{
			if(!LoadTimesliceEntries(&(*i), timesliceSaveList))
			{
				return false;
			}
		}
		else if(i->GetName() == L"WriteList")
		{
			if(!LoadWriteEntries(&(*i), writeSaveList))
			{
				return false;
			}
		}
	}

	//Load timeslice list
	timesliceList.clear();
	for(std::list<TimesliceSaveEntry>::iterator i = timesliceSaveList.begin(); i != timesliceSaveList.end(); ++i)
	{
		TimesliceEntry timesliceEntry;
		timesliceEntry.timesliceLength = i->timesliceLength;
		timesliceList.push_back(timesliceEntry);
		i->timesliceLoad = (++timesliceList.rbegin()).base();
	}
	latestTimeslice = GetLatestTimeslice();

	//Load memory buffer
	IHierarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if(valueAttribute != 0)
	{
		valueAttribute->ExtractValue(value);
	}

	//Load write list, and rebuild memory buffer
	writeList.clear();
	for(std::list<WriteSaveEntry>::reverse_iterator i = writeSaveList.rbegin(); i != writeSaveList.rend(); ++i)
	{
		WriteEntry writeEntry(value);
		writeEntry.writeTime = i->writeTime;
		std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
		while((currentTimeslice != timesliceSaveList.end()) && (currentTimeslice->id != i->currentTimeslice))
		{
			++currentTimeslice;
		}
		if(currentTimeslice != timesliceSaveList.end())
		{
			writeEntry.currentTimeslice = currentTimeslice->timesliceLoad;
			writeEntry.newValue = value;
			value = i->oldValue;
			writeList.push_front(writeEntry);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::LoadTimesliceEntries(IHierarchicalStorageNode& node, std::list<TimesliceSaveEntry>& timesliceSaveList)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"Timeslice")
		{
			IHierarchicalStorageAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
			IHierarchicalStorageAttribute* timesliceLength = i->GetAttribute(L"TimesliceLength");
			if((timesliceID != 0) && (timesliceLength != 0))
			{
				TimesliceSaveEntry entry(timesliceID->ExtractValue<unsigned int>(), timesliceLength->ExtractValue<TimesliceType>());

				//Find the correct location in the list to insert the timeslice entry. The
				//timeslice list must be sorted from earliest to latest by id.
				std::list<TimesliceSaveEntry>::reverse_iterator j = timesliceSaveList.rbegin();
				while((j != timesliceSaveList.rend()) && (j->id > entry.id))
				{
					++j;
				}
				timesliceSaveList.insert(j.base(), entry);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::LoadWriteEntries(IHierarchicalStorageNode& node, std::list<WriteSaveEntry>& writeSaveList)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"Write")
		{
			IHierarchicalStorageAttribute* writeTime = i->GetAttribute(L"WriteTime");
			IHierarchicalStorageAttribute* oldValue = i->GetAttribute(L"OldValue");
			IHierarchicalStorageAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
			if((writeTime != 0) && (oldValue != 0) && (timesliceID != 0))
			{
				//Extract the write entry from the XML stream
				WriteSaveEntry entry(writeTime->ExtractValue<TimesliceType>(), value, timesliceID->ExtractValue<unsigned int>());
				oldValue->ExtractValue(entry.oldValue);

				//Find the correct location in the list to insert the write entry. The
				//write list must be sorted from earliest to latest.
				std::list<WriteSaveEntry>::reverse_iterator j = writeSaveList.rbegin();
				while((j != writeSaveList.rend()) && ((j->currentTimeslice > entry.currentTimeslice) || ((j->currentTimeslice == entry.currentTimeslice) && (j->writeTime > entry.writeTime))))
				{
					++j;
				}
				writeSaveList.insert(j.base(), entry);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessValue<DataType, TimesliceType>::SaveState(IHierarchicalStorageNode& node) const
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	DataType saveValue(value);

	//Save the current time offset
	node.CreateAttribute(L"CurrentTimeOffset", currentTimeOffset);

	//Take a copy of the current memory buffer. We need to copy the contents rather than
	//directly dumping the current memory buffer, because we change the direction of the
	//write history when we save it. Rather than saving the committed state of the
	//memory, we save the latest volatile state of the memory, and each writeEntry
	//contains the previous data that was in the memory before that write occurred,
	//rather than the new value which is being written. We do this to allow easy
	//modification of the saved data, without having to worry about uncommitted writes
	//overwriting changes.
	saveValue = value;

	//Build numeric ID's to identify each timeslice, and save the timeslice list state
	IHierarchicalStorageNode& timesliceListState = node.CreateChild(L"TimesliceList");
	unsigned int id = 0;
	for(std::list<TimesliceEntry>::const_iterator i = timesliceList.begin(); i != timesliceList.end(); ++i)
	{
		timesliceSaveList.push_back(TimesliceSaveEntry(i, id));
		IHierarchicalStorageNode timesliceEntry(L"Timeslice");
		timesliceEntry.AddAttribute(IHierarchicalStorageAttribute(L"TimesliceID", id));
		timesliceEntry.AddAttribute(IHierarchicalStorageAttribute(L"TimesliceLength", i->timesliceLength));
		timesliceListState.AddChild(timesliceEntry);
		++id;
	}

	//Save the writeList state
	IHierarchicalStorageNode& writeListState = node.CreateChild(L"WriteList");
	std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
	for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
	{
		IHierarchicalStorageNode writeEntry(L"Write");
		while(currentTimeslice->timeslice != i->currentTimeslice)
		{
			++currentTimeslice;
		}
		writeEntry.AddAttribute(IHierarchicalStorageAttribute(L"TimesliceID", currentTimeslice->id));
		writeEntry.AddAttribute(IHierarchicalStorageAttribute(L"WriteTime", i->writeTime));
		writeEntry.AddAttribute(IHierarchicalStorageAttribute(L"OldValue", saveValue));
		writeListState.AddChild(writeEntry);
		saveValue = i->newValue;
	}

	//Save the latest value
	node.CreateAttribute(L"Value", saveValue);

	return true;
}
