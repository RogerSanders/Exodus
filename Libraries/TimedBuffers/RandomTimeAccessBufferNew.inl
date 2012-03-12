//#include "Stream/Stream.pkg"
//#include "Stream/ViewBinary.h"
//
////----------------------------------------------------------------------------------------
////Structures
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::TimesliceEntry
//{
//	TimesliceEntry()
//	:notDeleted(true)
//	{}
//
//	double timesliceLength;
//	bool committed;
//	bool notDeleted;
//};
//
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::WriteEntry
//{
//	WriteEntry()
//	:notDeleted(true)
//	{}
//	WriteEntry(const T& defaultValue)
//	:notDeleted(true), newValue(defaultValue)
//	{}
//	WriteEntry(unsigned int awriteAddress, double awriteTime, const T& anewValue, const Timeslice& acurrentTimeslice)
//	:notDeleted(true), writeAddress(awriteAddress), writeTime(awriteTime), newValue(anewValue), currentTimeslice(acurrentTimeslice)
//	{}
//
//	unsigned int writeAddress;
//	double writeTime;
//	T newValue;
//	Timeslice currentTimeslice;
//	bool notDeleted;
//};
//
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::WriteInfo
//{
//	WriteInfo()
//	{}
//	WriteInfo(const T& defaultValue)
//	:newValue(defaultValue)
//	{}
//
//	bool exists;
//	unsigned int writeAddress;
//	double writeTime;
//	T newValue;
//};
//
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::TimesliceSaveEntry
//{
//	TimesliceSaveEntry(typename const std::list<TimesliceEntry>::const_iterator& atimeslice, unsigned int aid)
//	:timeslice(atimeslice), id(aid)
//	{}
//	TimesliceSaveEntry(unsigned int aid, double atimesliceLength)
//	:id(aid), timesliceLength(atimesliceLength)
//	{}
//	typename std::list<TimesliceEntry>::const_iterator timeslice;
//	Timeslice timesliceLoad;
//	unsigned int id;
//	double timesliceLength;
//};
//
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::WriteSaveEntry
//{
//	WriteSaveEntry(unsigned int awriteAddress, double awriteTime, T aoldValue, unsigned int acurrentTimeslice)
//	:writeAddress(awriteAddress), writeTime(awriteTime), oldValue(aoldValue), currentTimeslice(acurrentTimeslice)
//	{}
//	unsigned int writeAddress;
//	double writeTime;
//	T oldValue;
//	unsigned int currentTimeslice;
//};
//
////----------------------------------------------------------------------------------------
//template<class T> struct RandomTimeAccessBufferNew<T>::AccessTarget
//{
//	enum Target
//	{
//		ACCESSTARGET_COMMITTED,
//		ACCESSTARGET_COMMITTED_TIME,
//		ACCESSTARGET_LATEST,
//		ACCESSTARGET_TIME,
//		ACCESSTARGET_TIMESLICE
//	};
//
//	AccessTarget()
//	{}
//	AccessTarget& AccessCommitted()
//	{
//		accessTarget = ACCESSTARGET_COMMITTED;
//		return *this;
//	}
//	AccessTarget& AccessCommitted(double aaccessTime)
//	{
//		accessTarget = ACCESSTARGET_COMMITTED_TIME;
//		accessTime = aaccessTime;
//		return *this;
//	}
//	AccessTarget& AccessLatest()
//	{
//		accessTarget = ACCESSTARGET_LATEST;
//		return *this;
//	}
//	AccessTarget& AccessTime(double aaccessTime)
//	{
//		accessTarget = ACCESSTARGET_TIME;
//		accessTime = aaccessTime;
//		return *this;
//	}
//	AccessTarget& AccessTimeslice(double aaccessTime, const Timeslice& aaccessTimeslice)
//	{
//		accessTarget = ACCESSTARGET_TIMESLICE;
//		accessTime = aaccessTime;
//		accessTimeslice = aaccessTimeslice;
//		return *this;
//	}
//
//	Target accessTarget;
//	double accessTime;
//	Timeslice accessTimeslice;
//};
//
////----------------------------------------------------------------------------------------
////Constructors
////----------------------------------------------------------------------------------------
//template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew()
//{}
//
////----------------------------------------------------------------------------------------
//template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew(unsigned int size)
//{
//	memory.resize(size);
//}
//
////----------------------------------------------------------------------------------------
//template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew(unsigned int size, const T& adefaultValue)
//:defaultValue(adefaultValue)
//{
//	memory.resize(size, defaultValue);
//}
//
////----------------------------------------------------------------------------------------
////Common functions: Overlapped access permissible
////----------------------------------------------------------------------------------------
//template<class T> T RandomTimeAccessBufferNew<T>::Read(unsigned int address, const AccessTarget& accessTarget) const
//{
//	switch(accessTarget.accessTarget)
//	{
//	case accessTarget.ACCESSTARGET_COMMITTED:
//		return ReadCommitted(address);
//	case accessTarget.ACCESSTARGET_COMMITTED_TIME:
//		return ReadCommitted(address, accessTarget.accessTime);
//	case accessTarget.ACCESSTARGET_LATEST:
//		return ReadLatest(address);
//	case accessTarget.ACCESSTARGET_TIME:
//		return Read(address, accessTarget.accessTime);
//	}
//	//##TODO## Raise an assert here
//	return T(defaultValue);
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Write(unsigned int address, const T& data, const AccessTarget& accessTarget)
//{
//	switch(accessTarget.accessTarget)
//	{
//	case accessTarget.ACCESSTARGET_COMMITTED:
//		WriteCommitted(address, data);
//		return;
//	case accessTarget.ACCESSTARGET_LATEST:
//		WriteLatest(address, data);
//		return;
//	case accessTarget.ACCESSTARGET_TIME:
//		Write(address, accessTarget.accessTime, data);
//		return;
//	}
//	//##TODO## Raise an assert here
//}
//
////----------------------------------------------------------------------------------------
//template<class T> unsigned int RandomTimeAccessBufferNew<T>::Size() const
//{
//	return (unsigned int)memory.size();
//}
//
////----------------------------------------------------------------------------------------
////Active functions: One active function at a time, overlapped with others
////----------------------------------------------------------------------------------------
////This function starts at the end of the buffered writes, and works its way back to the
////beginning, stopping when it finds a write to the target address. We do this to optimize
////ad-hoc reads of a buffer when there are a large number of reads and writes occurring to
////a small number of addresses.
////----------------------------------------------------------------------------------------
//template<class T> T RandomTimeAccessBufferNew<T>::Read(unsigned int address, double readTime) const
//{
//	//Search for written values in the current timeslice
//	std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
//	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice))
//	{
//		if(i->notDeleted && (i->writeAddress == address) && (i->writeTime <= readTime))
//		{
//			return i->newValue;
//		}
//		++i;
//	}
//	//Search for written values in previous timeslices
//	while(i != writeList.rend())
//	{
//		if(i->notDeleted && (i->writeAddress == address))
//		{
//			return i->newValue;
//		}
//		++i;
//	}
//
//	//Default to the committed value
//	return memory[address];
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Write(unsigned int address, double writeTime, const T& data)
//{
//	WriteEntry entry(address, writeTime, data, latestTimeslice);
//
//	//Find the correct location in the list to insert the new write entry. The writeList
//	//must be sorted from earliest to latest write by time.
//	std::list<WriteEntry>::reverse_iterator i = writeList.rbegin();
//	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice) && (i->writeTime > writeTime))
//	{
//		++i;
//	}
//	writeList.insert(i.base(), entry);
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::AddTimeslice(double timeslice)
//{
//	//Add the new timeslice entry to the list
//	TimesliceEntry entry;
//	entry.timesliceLength = timeslice;
//	entry.committed = false;
//	timesliceList.push_back(entry);
//
//	//Select the new timeslice entry as the latest timeslice
//	latestTimeslice = (++timesliceList.rbegin()).base();
//}
//
////----------------------------------------------------------------------------------------
//template<class T> T RandomTimeAccessBufferNew<T>::ReadLatest(unsigned int address) const
//{
//	//Search for written values in any timeslice
//	std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
//	while(i != writeList.rend())
//	{
//		if(i->notDeleted && (i->writeAddress == address))
//		{
//			return i->newValue;
//		}
//		++i;
//	}
//
//	//Default to the committed value
//	return memory[address];
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::WriteLatest(unsigned int address, const T& data)
//{
//	//Erase any write entries to this address in any timeslice. We do this to prevent
//	//uncommitted writes from overwriting this change. This write function should make
//	//the new value visible from all access functions.
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	while(i != writeList.end())
//	{
//		if(i->writeAddress == address)
//		{
//			i->notDeleted = false;
//		}
//		++i;
//	}
//
//	//Write the new value directly to the committed state
//	memory[address] = data;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::GetLatestBufferCopy(std::vector<T>& buffer) const
//{
//	//Resize the target buffer to match the size of the source buffer, and populate with
//	//the committed memory state.
//	buffer.assign(memory.begin(), memory.end());
//
//	//Commit each buffered write entry to the target buffer
//	for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
//	{
//		if(i->notDeleted)
//		{
//			buffer[i->writeAddress] = i->newValue;
//		}
//	}
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Commit()
//{
//	//Flag all timeslices as committed
//	std::list<TimesliceEntry>::reverse_iterator i = timesliceList.rbegin();
//	while((i != timesliceList.rend()) && !i->committed)
//	{
//		i->committed = true;
//		++i;
//	}
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Rollback()
//{
//	//Erase non-committed memory writes
//	std::list<WriteEntry>::reverse_iterator i = writeList.rbegin();
//	while((i != writeList.rend()) && !i->currentTimeslice->committed)
//	{
//		i->notDeleted = false;
//		++i;
//	}
//
//	//Erase non-committed timeslice entries
//	std::list<TimesliceEntry>::reverse_iterator j = timesliceList.rbegin();
//	while((j != timesliceList.rend()) && (!j->committed))
//	{
//		j->notDeleted = false;
//		++j;
//	}
//
//	//Recalculate the latest timeslice
//	if(!timesliceList.empty())
//	{
//		latestTimeslice = (++timesliceList.rbegin()).base();
//	}
//	else
//	{
//		latestTimeslice = timesliceList.end();
//	}
//}
//
////----------------------------------------------------------------------------------------
//template<class T> typename RandomTimeAccessBufferNew<T>::Timeslice RandomTimeAccessBufferNew<T>::GetLatestTimeslice() const
//{
//	return latestTimeslice;
//}
//
////----------------------------------------------------------------------------------------
////Committed functions: One committed function at a time, overlapped with others
////----------------------------------------------------------------------------------------
//template<class T> T RandomTimeAccessBufferNew<T>::ReadCommitted(unsigned int address) const
//{
//	return memory[address];
//}
//
////----------------------------------------------------------------------------------------
//template<class T> T RandomTimeAccessBufferNew<T>::ReadCommitted(unsigned int address, double readTime) const
//{
//	double currentTimeBase = 0;
//
//	//Default to the committed value
//	T foundValue = memory[address];
//
//	//Search for any buffered writes before the read time
//	std::list<TimesliceEntry>::const_iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::const_iterator i = writeList.begin();
//	bool done = false;
//	while((i != writeList.end()) && !done)
//	{
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice)
//			&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= readTime)
//			|| !currentTimeslice->notDeleted)
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//			}
//			++currentTimeslice;
//		}
//		//If the next buffered write is outside the time step, terminate processing of
//		//buffered writes.
//		if((currentTimeslice != i->currentTimeslice) || (((currentTimeBase + i->writeTime) - currentTimeOffset) > readTime))
//		{
//			done = true;
//			continue;
//		}
//		//If the write occurred to the same address as our target, set it as the new
//		//value for our target address at the time of the read.
//		if((i->writeAddress == address) && i->notDeleted)
//		{
//			foundValue = i->newValue;
//		}
//		++i;
//	}
//
//	return foundValue;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::WriteCommitted(unsigned int address, const T& data)
//{
//	memory[address] = data;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::AdvancePastTimeslice(const Timeslice& targetTimeslice)
//{
//	//Commit buffered writes which we have passed in this step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	bool done = false;
//	while((i != writeList.end()) && !done)
//	{
//		if(!i->notDeleted)
//		{
//			++i;
//			continue;
//		}
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice) && (currentTimeslice != targetTimeslice))
//		{
//			currentTimeslice->notDeleted = false;
//			++currentTimeslice;
//		}
//		//If the next buffered write is outside the time step, terminate processing of
//		//buffered writes.
//		if(currentTimeslice != i->currentTimeslice)
//		{
//			done = true;
//			continue;
//		}
//		memory[i->writeAddress] = i->newValue;
//		i->notDeleted = false;
//		++i;
//	}
//	//Check for any remaining timeslices which have expired
//	while(currentTimeslice != targetTimeslice)
//	{
//		currentTimeslice->notDeleted = false;
//		++currentTimeslice;
//	}
//	//Set our current time offset to the end of the target timeslice
//	currentTimeOffset = targetTimeslice->timesliceLength;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::AdvanceToTimeslice(const Timeslice& targetTimeslice)
//{
//	//Commit buffered writes which we have passed in this step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	bool done = false;
//	while((i != writeList.end()) && !done)
//	{
//		if(!i->notDeleted)
//		{
//			++i;
//			continue;
//		}
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice) && (currentTimeslice != targetTimeslice))
//		{
//			currentTimeslice->notDeleted = false;
//			++currentTimeslice;
//		}
//		//If the next buffered write is outside the time step, terminate processing of
//		//buffered writes.
//		if((currentTimeslice != i->currentTimeslice) || (currentTimeslice == targetTimeslice))
//		{
//			done = true;
//			continue;
//		}
//		memory[i->writeAddress] = i->newValue;
//		i->notDeleted = false;
//		++i;
//	}
//	//Check for any remaining timeslices which have expired
//	while(currentTimeslice != targetTimeslice)
//	{
//		currentTimeslice->notDeleted = false;
//		++currentTimeslice;
//	}
//	//Set our current time offset to the start of the target timeslice
//	currentTimeOffset = 0;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::AdvanceByTime(double step, const Timeslice& targetTimeslice)
//{
//	double currentTimeBase = 0;
//
//	//Commit buffered writes which we have passed in this step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	bool done = false;
//	while((i != writeList.end()) && !done)
//	{
//		if(!i->notDeleted)
//		{
//			++i;
//			continue;
//		}
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice)
//			&& (currentTimeslice != targetTimeslice)
//			&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step)
//			|| !currentTimeslice->notDeleted)
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//				currentTimeslice->notDeleted = false;
//			}
//			++currentTimeslice;
//		}
//		//If the next buffered write is outside the time step, terminate processing of
//		//buffered writes.
//		if((currentTimeslice != i->currentTimeslice) || (((currentTimeBase + i->writeTime) - currentTimeOffset) > step))
//		{
//			done = true;
//			continue;
//		}
//		memory[i->writeAddress] = i->newValue;
//		i->notDeleted = false;
//		++i;
//	}
//	//Check for any remaining timeslices which have expired
//	while((currentTimeslice != targetTimeslice)
//		&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step))
//	{
//		if(currentTimeslice->notDeleted)
//		{
//			currentTimeBase += currentTimeslice->timesliceLength;
//			currentTimeslice->notDeleted = false;
//		}
//		++currentTimeslice;
//	}
//	//Set the amount of the current timeslice which has been stepped through as the time
//	//offset for the next step operation.
//	currentTimeOffset = (currentTimeOffset + step) - currentTimeBase;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> bool RandomTimeAccessBufferNew<T>::AdvanceByStep(const Timeslice& targetTimeslice)
//{
//	double currentTimeBase = 0;
//	double writeTime = targetTimeslice->timesliceLength;
//	bool foundWrite = false;
//
//	//Commit buffered writes which we have passed in this step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	while((i != writeList.end()) && !foundWrite)
//	{
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice)
//			&& (currentTimeslice != targetTimeslice))
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//				currentTimeslice->notDeleted = false;
//			}
//			++currentTimeslice;
//		}
//		//If the next buffered write is within the time step, save the write time and
//		//commit the data.
//		if((currentTimeslice == i->currentTimeslice) && i->notDeleted)
//		{
//			foundWrite = true;
//			writeTime = i->writeTime;
//			memory[i->writeAddress] = i->newValue;
//			i->notDeleted = false;
//		}
//		++i;
//	}
//	//Check for any remaining timeslices which have expired.
//	if(!foundWrite)
//	{
//		while(currentTimeslice != targetTimeslice)
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//				currentTimeslice->notDeleted = false;
//			}
//			++currentTimeslice;
//		}
//	}
//
//	//Set the amount of the current timeslice which has been stepped through as the time
//	//offset for the next step operation.
//	currentTimeOffset = writeTime;
//
//	return foundWrite;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> double RandomTimeAccessBufferNew<T>::GetNextWriteTime(const Timeslice& targetTimeslice)
//{
//	bool foundWrite = false;
//	double nextWriteTime = 0;
//	double currentTimeBase = 0;
//
//	//Search the write list for the next buffered write inside this time step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	while((i != writeList.end()) && !foundWrite)
//	{
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice)
//			&& (currentTimeslice != targetTimeslice))
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//			}
//			++currentTimeslice;
//		}
//		//If the next buffered write is within the time step, save the write time.
//		if((currentTimeslice == i->currentTimeslice) && i->notDeleted)
//		{
//			foundWrite = true;
//			nextWriteTime = ((currentTimeBase + i->writeTime) - currentTimeOffset);
//		}
//		++i;
//	}
//	//If no write is pending within the time step, output the end of the target timeslice
//	//as the next write time.
//	if(!foundWrite)
//	{
//		while(currentTimeslice != targetTimeslice)
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//			}
//			++currentTimeslice;
//		}
//		nextWriteTime = ((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset);
//	}
//
//	return nextWriteTime;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> typename RandomTimeAccessBufferNew<T>::WriteInfo RandomTimeAccessBufferNew<T>::GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
//{
//	double currentTimeBase = 0;
//	unsigned int currentIndex = 0;
//	WriteInfo writeInfo(defaultValue);
//	writeInfo.exists = false;
//
//	//Search the write list for the next buffered write inside this time step
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	bool done = false;
//	while((i != writeList.end()) && !done)
//	{
//		if(!i->notDeleted)
//		{
//			++i;
//			continue;
//		}
//		//Advance through the timeslice list until we find the timeslice matching the
//		//next buffered write, or we pass the end of this time step.
//		while((currentTimeslice != i->currentTimeslice)
//			&& (currentTimeslice != targetTimeslice))
//		{
//			if(currentTimeslice->notDeleted)
//			{
//				currentTimeBase += currentTimeslice->timesliceLength;
//			}
//			++currentTimeslice;
//		}
//		//If the next buffered write is within the time step, output its data
//		if(currentTimeslice == i->currentTimeslice)
//		{
//			if(currentIndex == index)
//			{
//				writeInfo.exists = true;
//				writeInfo.writeAddress = i->writeAddress;
//				writeInfo.newValue = i->newValue;
//				writeInfo.writeTime = ((currentTimeBase + i->writeTime) - currentTimeOffset);
//			}
//			else
//			{
//				++currentIndex;
//				continue;
//			}
//		}
//		done = true;
//	}
//
//	return writeInfo;
//}
//
////----------------------------------------------------------------------------------------
////Control functions: Require global exclusive access
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Initialize()
//{
//	//Initialize buffers
//	for(unsigned int i = 0; i < memory.size(); ++i)
//	{
//		memory[i] = defaultValue;
//	}
//	writeList.clear();
//	timesliceList.clear();
//	currentTimeOffset = 0;
//	latestTimeslice = timesliceList.end();
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Flush()
//{
//	std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
//	while((currentTimeslice != timesliceList.end()) && !currentTimeslice->notDeleted)
//	{
//		++currentTimeslice;
//	}
//
//	std::list<WriteEntry>::iterator i = writeList.begin();
//	while((i != writeList.end()) && !i->notDeleted)
//	{
//		++i;
//	}
//
//	//Erase buffered writes which have been committed, and timeslices which have expired.
//	writeList.erase(writeList.begin(), i);
//	timesliceList.erase(timesliceList.begin(), currentTimeslice);
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::Resize(unsigned int size)
//{
//	memory.resize(size, defaultValue);
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::LoadState(XMLEntity* entity)
//{
//	std::list<TimesliceSaveEntry> timesliceSaveList;
//	std::list<WriteSaveEntry> writeSaveList;
//
//	//Load the current time offset
//	entity->ExtractAttribute(L"CurrentTimeOffset", currentTimeOffset);
//
//	//Read saved data from the XML tree
//	for(XMLEntity::EntityList::iterator i = entity->children.begin(); i != entity->children.end(); ++i)
//	{
//		if(i->GetName() == L"TimesliceList")
//		{
//			timesliceSaveList = LoadTimesliceEntries(&(*i));
//		}
//		else if(i->GetName() == L"WriteList")
//		{
//			writeSaveList = LoadWriteEntries(&(*i));
//		}
//	}
//
//	//Load timeslice list
//	timesliceList.clear();
//	for(std::list<TimesliceSaveEntry>::iterator i = timesliceSaveList.begin(); i != timesliceSaveList.end(); ++i)
//	{
//		TimesliceEntry timesliceEntry;
//		timesliceEntry.timesliceLength = i->timesliceLength;
//		timesliceList.push_back(timesliceEntry);
//		i->timesliceLoad = (++timesliceList.rbegin()).base();
//	}
//
//	//Calculate the latest timeslice
//	if(timesliceList.empty())
//	{
//		latestTimeslice = timesliceList.end();
//	}
//	else
//	{
//		latestTimeslice = (++timesliceList.rbegin()).base();
//	}
//
//	//Load memory buffer
//	std::vector<unsigned char> savedMemory;
//	entity->ExtractBinaryData(savedMemory);
//	Buffer saveBuffer(savedMemory.size());
//	for(unsigned int i = 0; i < savedMemory.size(); ++i)
//	{
//		saveBuffer.WriteBinary(savedMemory[i]);
//	}
//	Stream::ViewBinary bufferView(saveBuffer);
//	for(unsigned int i = 0; i < memory.size(); ++i)
//	{
//		bufferView >> memory[i];
//	}
//
//	//Load write list, and rebuild memory buffer
//	writeList.clear();
//	for(std::list<WriteSaveEntry>::reverse_iterator i = writeSaveList.rbegin(); i != writeSaveList.rend(); ++i)
//	{
//		WriteEntry writeEntry(defaultValue);
//		writeEntry.writeAddress = i->writeAddress;
//		writeEntry.writeTime = i->writeTime;
//		std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
//		while((currentTimeslice != timesliceSaveList.end()) && (currentTimeslice->id != i->currentTimeslice))
//		{
//			++currentTimeslice;
//		}
//		if(currentTimeslice != timesliceSaveList.end())
//		{
//			writeEntry.currentTimeslice = currentTimeslice->timesliceLoad;
//			writeEntry.newValue = memory[writeEntry.writeAddress];
//			memory[writeEntry.writeAddress] = i->oldValue;
//			writeList.push_front(writeEntry);
//		}
//	}
//}
//
////----------------------------------------------------------------------------------------
//template<class T> std::list<typename RandomTimeAccessBufferNew<T>::TimesliceSaveEntry> RandomTimeAccessBufferNew<T>::LoadTimesliceEntries(XMLEntity* entity)
//{
//	std::list<TimesliceSaveEntry> timesliceSaveList;
//
//	for(XMLEntity::EntityList::iterator i = entity->children.begin(); i != entity->children.end(); ++i)
//	{
//		if(i->GetName() == L"Timeslice")
//		{
//			XMLAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
//			XMLAttribute* timesliceLength = i->GetAttribute(L"TimesliceLength");
//			if((timesliceID != 0) && (timesliceLength != 0))
//			{
//				TimesliceSaveEntry entry(timesliceID->ExtractValue<unsigned int>(), timesliceLength->ExtractValue<double>());
//
//				//Find the correct location in the list to insert the timeslice entry. The
//				//timeslice list must be sorted from earliest to latest by id.
//				std::list<TimesliceSaveEntry>::reverse_iterator j = timesliceSaveList.rbegin();
//				while((j != timesliceSaveList.rend()) && (j->id > entry.id))
//				{
//					++j;
//				}
//				timesliceSaveList.insert(j.base(), entry);
//			}
//		}
//	}
//
//	return timesliceSaveList;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> std::list<typename RandomTimeAccessBufferNew<T>::WriteSaveEntry> RandomTimeAccessBufferNew<T>::LoadWriteEntries(XMLEntity* entity)
//{
//	std::list<WriteSaveEntry> writeSaveList;
//
//	for(XMLEntity::EntityList::iterator i = entity->children.begin(); i != entity->children.end(); ++i)
//	{
//		if(i->GetName() == L"Write")
//		{
//			XMLAttribute* writeAddress = i->GetAttribute(L"WriteAddress");
//			XMLAttribute* writeTime = i->GetAttribute(L"WriteTime");
//			XMLAttribute* oldValue = i->GetAttribute(L"OldValue");
//			XMLAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
//			if((writeAddress != 0) && (writeTime != 0) && (oldValue != 0) && (timesliceID != 0))
//			{
//				//Extract the write entry from the XML stream
//				T oldValueData(defaultValue);
//				oldValue->ExtractValue(oldValueData);
//				WriteSaveEntry entry(writeAddress->ExtractValue<unsigned int>(), writeTime->ExtractValue<double>(), oldValueData, timesliceID->ExtractValue<unsigned int>());
//
//				//Find the correct location in the list to insert the write entry. The
//				//write list must be sorted from earliest to latest.
//				std::list<WriteSaveEntry>::reverse_iterator j = writeSaveList.rbegin();
//				while((j != writeSaveList.rend()) && ((j->currentTimeslice > entry.currentTimeslice) || ((j->currentTimeslice == entry.currentTimeslice) && (j->writeTime > entry.writeTime))))
//				{
//					++j;
//				}
//				writeSaveList.insert(j.base(), entry);
//			}
//		}
//	}
//
//	return writeSaveList;
//}
//
////----------------------------------------------------------------------------------------
//template<class T> void RandomTimeAccessBufferNew<T>::GetState(XMLEntity* entity, const std::wstring& bufferName, bool inlineData) const
//{
//	std::list<TimesliceSaveEntry> timesliceSaveList;
//	std::vector<T> saveMemory;
//
//	//Save the current time offset
//	entity->CreateAttribute(L"CurrentTimeOffset", currentTimeOffset);
//
//	//Take a copy of the current memory buffer. We need to copy the contents rather than
//	//directly dumping the current memory buffer, because we change the direction of the
//	//write history when we save it. Rather than saving the committed state of the
//	//memory, we save the latest volatile state of the memory, and each writeEntry
//	//contains the previous data that was in the memory before that write occurred,
//	//rather than the new value which is being written. We do this to allow easy
//	//modification of the saved data, without having to worry about uncommitted writes
//	//overwriting changes.
//	saveMemory.insert(saveMemory.begin(), memory.begin(), memory.end());
//
//	//Build numeric ID's to identify each timeslice, and save the timeslice list state
//	XMLEntity timesliceListState(L"TimesliceList");
//	unsigned int id = 0;
//	for(std::list<TimesliceEntry>::const_iterator i = timesliceList.begin(); i != timesliceList.end(); ++i)
//	{
//		if(i->notDeleted)
//		{
//			timesliceSaveList.push_back(TimesliceSaveEntry(i, id));
//			XMLEntity timesliceEntry(L"Timeslice");
//			timesliceEntry.AddAttribute(XMLAttribute(L"TimesliceID", id));
//			timesliceEntry.AddAttribute(XMLAttribute(L"TimesliceLength", i->timesliceLength));
//			timesliceListState.AddChild(timesliceEntry);
//			++id;
//		}
//	}
//	entity->AddChild(timesliceListState);
//
//	//Save the writeList state
//	XMLEntity writeListState(L"WriteList");
//	std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
//	for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
//	{
//		if(i->notDeleted)
//		{
//			XMLEntity writeEntry(L"Write");
//			while(currentTimeslice->timeslice != i->currentTimeslice)
//			{
//				++currentTimeslice;
//			}
//			writeEntry.AddAttribute(XMLAttribute(L"TimesliceID", currentTimeslice->id));
//			writeEntry.AddAttribute(XMLAttribute(L"WriteAddress", i->writeAddress));
//			writeEntry.AddAttribute(XMLAttribute(L"WriteTime", i->writeTime));
//			writeEntry.AddAttribute(XMLAttribute(L"OldValue", saveMemory[i->writeAddress]));
//			writeListState.AddChild(writeEntry);
//			saveMemory[i->writeAddress] = i->newValue;
//		}
//	}
//	entity->AddChild(writeListState);
//
//	//Add the memory buffer to the XML tree
//	entity->InsertBinaryData(saveMemory, bufferName, inlineData);
//}




#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::TimesliceEntry
{
	TimesliceEntry()
	:notDeleted(true)
	{}

	double timesliceLength;
	bool committed;
	bool notDeleted;
};

//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::WriteEntry
{
	WriteEntry()
	:notDeleted(true)
	{}
	WriteEntry(const T& defaultValue)
	:notDeleted(true), newValue(defaultValue)
	{}
	WriteEntry(unsigned int awriteAddress, double awriteTime, const T& anewValue, const Timeslice& acurrentTimeslice)
	:notDeleted(true), writeAddress(awriteAddress), writeTime(awriteTime), newValue(anewValue), currentTimeslice(acurrentTimeslice)
	{}

	unsigned int writeAddress;
	double writeTime;
	T newValue;
	Timeslice currentTimeslice;
	bool notDeleted;
};

//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::WriteInfo
{
	WriteInfo()
	{}
	WriteInfo(const T& defaultValue)
	:newValue(defaultValue)
	{}

	bool exists;
	unsigned int writeAddress;
	double writeTime;
	T newValue;
};

//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::TimesliceSaveEntry
{
	TimesliceSaveEntry(typename TimesliceList::Element* atimeslice, unsigned int aid)
	:timeslice(atimeslice), id(aid)
	{}
	TimesliceSaveEntry(unsigned int aid, double atimesliceLength)
	:id(aid), timesliceLength(atimesliceLength)
	{}
	typename TimesliceList::Element* timeslice;
	Timeslice timesliceLoad;
	unsigned int id;
	double timesliceLength;
};

//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::WriteSaveEntry
{
	WriteSaveEntry(unsigned int awriteAddress, double awriteTime, const T& aoldValue, unsigned int acurrentTimeslice)
	:writeAddress(awriteAddress), writeTime(awriteTime), oldValue(aoldValue), currentTimeslice(acurrentTimeslice)
	{}
	unsigned int writeAddress;
	double writeTime;
	T oldValue;
	unsigned int currentTimeslice;
};

//----------------------------------------------------------------------------------------
template<class T> struct RandomTimeAccessBufferNew<T>::AccessTarget
{
	enum Target
	{
		ACCESSTARGET_COMMITTED,
		ACCESSTARGET_COMMITTED_TIME,
		ACCESSTARGET_LATEST,
		ACCESSTARGET_TIME,
		ACCESSTARGET_TIMESLICE
	};

	AccessTarget()
	{}
	AccessTarget& AccessCommitted()
	{
		accessTarget = ACCESSTARGET_COMMITTED;
		return *this;
	}
	AccessTarget& AccessCommitted(double aaccessTime)
	{
		accessTarget = ACCESSTARGET_COMMITTED_TIME;
		accessTime = aaccessTime;
		return *this;
	}
	AccessTarget& AccessLatest()
	{
		accessTarget = ACCESSTARGET_LATEST;
		return *this;
	}
	AccessTarget& AccessTime(double aaccessTime)
	{
		accessTarget = ACCESSTARGET_TIME;
		accessTime = aaccessTime;
		return *this;
	}
	AccessTarget& AccessTimeslice(double aaccessTime, const Timeslice& aaccessTimeslice)
	{
		accessTarget = ACCESSTARGET_TIMESLICE;
		accessTime = aaccessTime;
		accessTimeslice = aaccessTimeslice;
		return *this;
	}

	Target accessTarget;
	double accessTime;
	Timeslice accessTimeslice;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew()
{}

//----------------------------------------------------------------------------------------
template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew(unsigned int size)
{
	memory.resize(size);
}

//----------------------------------------------------------------------------------------
template<class T> RandomTimeAccessBufferNew<T>::RandomTimeAccessBufferNew(unsigned int size, const T& adefaultValue)
:defaultValue(adefaultValue)
{
	memory.resize(size, defaultValue);
}

//----------------------------------------------------------------------------------------
//Common functions: Overlapped access permissible
//----------------------------------------------------------------------------------------
template<class T> T RandomTimeAccessBufferNew<T>::Read(unsigned int address, const AccessTarget& accessTarget) const
{
	switch(accessTarget.accessTarget)
	{
	case accessTarget.ACCESSTARGET_COMMITTED:
		return ReadCommitted(address);
	case accessTarget.ACCESSTARGET_COMMITTED_TIME:
		return ReadCommitted(address, accessTarget.accessTime);
	case accessTarget.ACCESSTARGET_LATEST:
		return ReadLatest(address);
	case accessTarget.ACCESSTARGET_TIME:
		return Read(address, accessTarget.accessTime);
	}
	//##TODO## Raise an assert here
	return T(defaultValue);
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Write(unsigned int address, const T& data, const AccessTarget& accessTarget)
{
	switch(accessTarget.accessTarget)
	{
	case accessTarget.ACCESSTARGET_COMMITTED:
		WriteCommitted(address, data);
		return;
	case accessTarget.ACCESSTARGET_LATEST:
		WriteLatest(address, data);
		return;
	case accessTarget.ACCESSTARGET_TIME:
		Write(address, accessTarget.accessTime, data);
		return;
	}
	//##TODO## Raise an assert here
}

//----------------------------------------------------------------------------------------
template<class T> unsigned int RandomTimeAccessBufferNew<T>::Size() const
{
	return (unsigned int)memory.size();
}

//----------------------------------------------------------------------------------------
//Active functions: One active function at a time, overlapped with others
//----------------------------------------------------------------------------------------
//This function starts at the end of the buffered writes, and works its way back to the
//beginning, stopping when it finds a write to the target address. We do this to optimize
//ad-hoc reads of a buffer when there are a large number of reads and writes occurring to
//a small number of addresses.
//----------------------------------------------------------------------------------------
template<class T> T RandomTimeAccessBufferNew<T>::Read(unsigned int address, double readTime) const
{
	//Search for written values in the current timeslice
	WriteList::Element* i = writeList.GetLastEntry();
	while((i != 0) && (i->object.currentTimeslice == latestTimeslice))
	{
		if(i->object.notDeleted && (i->object.writeAddress == address) && (i->object.writeTime <= readTime))
		{
			return i->object.newValue;
		}
		i = i->previous;
	}
	//Search for written values in previous timeslices
	while(i != 0)
	{
		if(i->object.notDeleted && (i->object.writeAddress == address))
		{
			return i->object.newValue;
		}
		i = i->previous;
	}

	//Default to the committed value
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Write(unsigned int address, double writeTime, const T& data)
{
	WriteEntry entry(address, writeTime, data, latestTimeslice);

	//Find the correct location in the list to insert the new write entry. The writeList
	//must be sorted from earliest to latest write by time.
	WriteList::Element* i = writeList.GetLastEntry();
	while((i != 0) && (i->object.currentTimeslice == latestTimeslice) && (i->object.writeTime > writeTime))
	{
		i = i->previous;
	}

	if(i != 0)
	{
		writeList.InsertAfter(i, entry);
	}
	else
	{
		writeList.PushToFront(entry);
	}
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::AddTimeslice(double timeslice)
{
	//Add the new timeslice entry to the list
	TimesliceEntry entry;
	entry.timesliceLength = timeslice;
	entry.committed = false;
	timesliceList.PushToBack(entry);

	//Select the new timeslice entry as the latest timeslice
	latestTimeslice = timesliceList.GetLastEntry();
}

//----------------------------------------------------------------------------------------
template<class T> T RandomTimeAccessBufferNew<T>::ReadLatest(unsigned int address) const
{
	boost::mutex::scoped_lock lock(accessLock);

	//Search for written values in any timeslice
	WriteList::Element* i = writeList.GetLastEntry();
	while(i != 0)
	{
		if(i->object.notDeleted && (i->object.writeAddress == address))
		{
			return i->object.newValue;
		}
		i = i->previous;
	}

	//Default to the committed value
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::WriteLatest(unsigned int address, const T& data)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Erase any write entries to this address in any timeslice. We do this to prevent
	//uncommitted writes from overwriting this change. This write function should make
	//the new value visible from all access functions.
	WriteList::Element* i = writeList.GetFirstEntry();
	while(i != 0)
	{
		if(i->object.writeAddress == address)
		{
			i->object.notDeleted = false;
		}
		i = i->next;
	}

	//Write the new value directly to the committed state
	memory[address] = data;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::GetLatestBufferCopy(std::vector<T>& buffer) const
{
	boost::mutex::scoped_lock lock(accessLock);

	//Resize the target buffer to match the size of the source buffer, and populate with
	//the committed memory state.
	buffer.assign(memory.begin(), memory.end());

	//Commit each buffered write entry to the target buffer
	for(WriteList::Element* i = writeList.GetFirstEntry(); i != 0; i = i->next)
	{
		if(i->object.notDeleted)
		{
			buffer[i->object.writeAddress] = i->object.newValue;
		}
	}
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Commit()
{
	//Flag all timeslices as committed
	TimesliceList::Element* i = timesliceList.GetLastEntry();
	while((i != 0) && !i->object.committed)
	{
		i->object.committed = true;
		i = i->previous;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Rollback()
{
	//Erase non-committed memory writes
	WriteList::Element* i = writeList.GetLastEntry();
	while((i != 0) && !i->object.currentTimeslice->object.committed)
	{
		i->object.notDeleted = false;
		i = i->previous;
	}

	//Erase non-committed timeslice entries
	TimesliceList::Element* j = timesliceList.GetLastEntry();
	while((j != 0) && !j->object.committed)
	{
		j->object.committed = true;
		j->object.notDeleted = false;
		j = j->previous;
	}

	//Recalculate the latest timeslice
	latestTimeslice = timesliceList.GetLastEntry();
}

//----------------------------------------------------------------------------------------
template<class T> typename RandomTimeAccessBufferNew<T>::Timeslice RandomTimeAccessBufferNew<T>::GetLatestTimeslice() const
{
	return latestTimeslice;
}

//----------------------------------------------------------------------------------------
//Committed functions: One committed function at a time, overlapped with others
//----------------------------------------------------------------------------------------
template<class T> T RandomTimeAccessBufferNew<T>::ReadCommitted(unsigned int address) const
{
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class T> T RandomTimeAccessBufferNew<T>::ReadCommitted(unsigned int address, double readTime) const
{
	double currentTimeBase = 0;

	//Default to the committed value
	T foundValue = memory[address];

	//Search for any buffered writes before the read time
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	bool done = false;
	while((i != 0) && !done)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice)
			&& ((((currentTimeBase + currentTimeslice->object.timesliceLength) - currentTimeOffset) <= readTime)
			|| !currentTimeslice->object.notDeleted))
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
			}
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->object.currentTimeslice) || (((currentTimeBase + i->object.writeTime) - currentTimeOffset) > readTime))
		{
			done = true;
			continue;
		}
		//If the write occurred to the same address as our target, set it as the new
		//value for our target address at the time of the read.
		if((i->object.writeAddress == address) && i->object.notDeleted)
		{
			foundValue = i->object.newValue;
		}
		i = i->next;
	}

	return foundValue;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::WriteCommitted(unsigned int address, const T& data)
{
	memory[address] = data;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::AdvancePastTimeslice(const Timeslice& targetTimeslice)
{
	//Commit buffered writes which we have passed in this step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	bool done = false;
	while((i != 0) && !done)
	{
		if(!i->object.notDeleted)
		{
			i = i->next;
			continue;
		}
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice) && (currentTimeslice != targetTimeslice))
		{
			currentTimeslice->object.notDeleted = false;
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if(currentTimeslice != i->object.currentTimeslice)
		{
			done = true;
			continue;
		}
		memory[i->object.writeAddress] = i->object.newValue;
		i->object.notDeleted = false;
		i = i->next;
	}
	//Check for any remaining timeslices which have expired
	while(currentTimeslice != targetTimeslice)
	{
		currentTimeslice->object.notDeleted = false;
		currentTimeslice = currentTimeslice->next;
	}
	//Set our current time offset to the end of the target timeslice
	currentTimeOffset = targetTimeslice->object.timesliceLength;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::AdvanceToTimeslice(const Timeslice& targetTimeslice)
{
	//Commit buffered writes which we have passed in this step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	bool done = false;
	while((i != 0) && !done)
	{
		if(!i->object.notDeleted)
		{
			i = i->next;
			continue;
		}
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice) && (currentTimeslice != targetTimeslice))
		{
			currentTimeslice->object.notDeleted = false;
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->object.currentTimeslice) || (currentTimeslice == targetTimeslice))
		{
			done = true;
			continue;
		}
		memory[i->object.writeAddress] = i->object.newValue;
		i->object.notDeleted = false;
		i = i->next;
	}
	//Check for any remaining timeslices which have expired
	while(currentTimeslice != targetTimeslice)
	{
		currentTimeslice->object.notDeleted = false;
		currentTimeslice = currentTimeslice->next;
	}
	//Set our current time offset to the start of the target timeslice
	currentTimeOffset = 0;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::AdvanceByTime(double step, const Timeslice& targetTimeslice)
{
	double currentTimeBase = 0;

	//Commit buffered writes which we have passed in this step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	bool done = false;
	while((i != 0) && !done)
	{
		if(!i->object.notDeleted)
		{
			i = i->next;
			continue;
		}
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice)
			&& (currentTimeslice != targetTimeslice)
			&& ((((currentTimeBase + currentTimeslice->object.timesliceLength) - currentTimeOffset) <= step)
			|| !currentTimeslice->object.notDeleted))
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
				currentTimeslice->object.notDeleted = false;
			}
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is outside the time step, terminate processing of
		//buffered writes.
		if((currentTimeslice != i->object.currentTimeslice) || (((currentTimeBase + i->object.writeTime) - currentTimeOffset) > step))
		{
			done = true;
			continue;
		}
		memory[i->object.writeAddress] = i->object.newValue;
		i->object.notDeleted = false;
		i = i->next;
	}
	//Check for any remaining timeslices which have expired
	while((currentTimeslice != targetTimeslice)
		&& (((currentTimeBase + currentTimeslice->object.timesliceLength) - currentTimeOffset) <= step))
	{
		if(currentTimeslice->object.notDeleted)
		{
			currentTimeBase += currentTimeslice->object.timesliceLength;
			currentTimeslice->object.notDeleted = false;
		}
		currentTimeslice = currentTimeslice->next;
	}
	//Set the amount of the current timeslice which has been stepped through as the time
	//offset for the next step operation.
	currentTimeOffset = (currentTimeOffset + step) - currentTimeBase;
}

//----------------------------------------------------------------------------------------
template<class T> bool RandomTimeAccessBufferNew<T>::AdvanceByStep(const Timeslice& targetTimeslice)
{
	double currentTimeBase = 0;
	double writeTime = targetTimeslice->object.timesliceLength;
	bool foundWrite = false;

	//Commit buffered writes which we have passed in this step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	while((i != 0) && !foundWrite)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
				currentTimeslice->object.notDeleted = false;
			}
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is within the time step, save the write time and
		//commit the data.
		if((currentTimeslice == i->object.currentTimeslice) && i->object.notDeleted)
		{
			foundWrite = true;
			writeTime = i->object.writeTime;
			memory[i->object.writeAddress] = i->object.newValue;
			i->object.notDeleted = false;
		}
		i = i->next;
	}
	//Check for any remaining timeslices which have expired.
	if(!foundWrite)
	{
		while(currentTimeslice != targetTimeslice)
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
				currentTimeslice->object.notDeleted = false;
			}
			currentTimeslice = currentTimeslice->next;
		}
	}

	//Set the amount of the current timeslice which has been stepped through as the time
	//offset for the next step operation.
	currentTimeOffset = writeTime;

	return foundWrite;
}

//----------------------------------------------------------------------------------------
template<class T> double RandomTimeAccessBufferNew<T>::GetNextWriteTime(const Timeslice& targetTimeslice)
{
	bool foundWrite = false;
	double nextWriteTime = 0;
	double currentTimeBase = 0;

	//Search the write list for the next buffered write inside this time step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	while((i != 0) && !foundWrite)
	{
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
			}
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is within the time step, save the write time.
		if((currentTimeslice == i->object.currentTimeslice) && i->object.notDeleted)
		{
			foundWrite = true;
			nextWriteTime = ((currentTimeBase + i->object.writeTime) - currentTimeOffset);
		}
		i = i->next;
	}
	//If no write is pending within the time step, output the end of the target timeslice
	//as the next write time.
	if(!foundWrite)
	{
		while(currentTimeslice != targetTimeslice)
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
			}
			currentTimeslice = currentTimeslice->next;
		}
		nextWriteTime = ((currentTimeBase + currentTimeslice->object.timesliceLength) - currentTimeOffset);
	}

	return nextWriteTime;
}

//----------------------------------------------------------------------------------------
template<class T> typename RandomTimeAccessBufferNew<T>::WriteInfo RandomTimeAccessBufferNew<T>::GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
{
	double currentTimeBase = 0;
	unsigned int currentIndex = 0;
	WriteInfo writeInfo(defaultValue);
	writeInfo.exists = false;

	//Search the write list for the next buffered write inside this time step
	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	WriteList::Element* i = writeList.GetFirstEntry();
	bool done = false;
	while((i != 0) && !done)
	{
		if(!i->object.notDeleted)
		{
			i = i->next;
			continue;
		}
		//Advance through the timeslice list until we find the timeslice matching the
		//next buffered write, or we pass the end of this time step.
		while((currentTimeslice != i->object.currentTimeslice)
			&& (currentTimeslice != targetTimeslice))
		{
			if(currentTimeslice->object.notDeleted)
			{
				currentTimeBase += currentTimeslice->object.timesliceLength;
			}
			currentTimeslice = currentTimeslice->next;
		}
		//If the next buffered write is within the time step, output its data
		if(currentTimeslice == i->object.currentTimeslice)
		{
			if(currentIndex == index)
			{
				writeInfo.exists = true;
				writeInfo.writeAddress = i->object.writeAddress;
				writeInfo.newValue = i->object.newValue;
				writeInfo.writeTime = ((currentTimeBase + i->object.writeTime) - currentTimeOffset);
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
template<class T> void RandomTimeAccessBufferNew<T>::Flush(const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);

	TimesliceList::Element* currentTimeslice = timesliceList.GetFirstEntry();
	while((currentTimeslice != targetTimeslice) && !currentTimeslice->object.notDeleted)
	{
		currentTimeslice = currentTimeslice->next;
	}

	WriteList::Element* i = writeList.GetFirstEntry();
	while((i != 0) && (i->object.currentTimeslice != targetTimeslice) && !i->object.notDeleted)
	{
		i = i->next;
	}

	//Erase buffered writes which have been committed, and timeslices which have expired.
	if(i != 0)
	{
		writeList.EraseFromFrontToElement(i);
	}
	if(currentTimeslice != 0)
	{
		timesliceList.EraseFromFrontToElement(currentTimeslice);
	}
}

//----------------------------------------------------------------------------------------
//Control functions: Require global exclusive access
//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Initialize()
{
	//Initialize buffers
	for(unsigned int i = 0; i < memory.size(); ++i)
	{
		memory[i] = defaultValue;
	}
	writeList.Clear();
	timesliceList.Clear();
	currentTimeOffset = 0;
	latestTimeslice = timesliceList.GetLastEntry();
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::Resize(unsigned int size)
{
	memory.resize(size, defaultValue);
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	std::list<WriteSaveEntry> writeSaveList;

	//Load the current time offset
	node.ExtractAttribute(L"CurrentTimeOffset", currentTimeOffset);

	//Read saved data from the XML tree
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"TimesliceList")
		{
			timesliceSaveList = LoadTimesliceEntries(&(*i));
		}
		else if(i->GetName() == L"WriteList")
		{
			writeSaveList = LoadWriteEntries(&(*i));
		}
	}

	//Load timeslice list
	timesliceList.Clear();
	for(std::list<TimesliceSaveEntry>::iterator i = timesliceSaveList.begin(); i != timesliceSaveList.end(); ++i)
	{
		TimesliceEntry timesliceEntry;
		timesliceEntry.timesliceLength = i->timesliceLength;
		timesliceList.PushToBack(timesliceEntry);
		i->timesliceLoad = timesliceList.GetLastEntry();
	}

	//Calculate the latest timeslice
	latestTimeslice = timesliceList.GetLastEntry();

	//Load memory buffer
	std::vector<unsigned char> savedMemory;
	node.ExtractBinaryData(savedMemory);
	Stream::Buffer saveBuffer(savedMemory.size());
	for(unsigned int i = 0; i < savedMemory.size(); ++i)
	{
		saveBuffer.WriteBinary(savedMemory[i]);
	}
	Stream::ViewBinary bufferView(saveBuffer);
	for(unsigned int i = 0; i < memory.size(); ++i)
	{
		bufferView >> memory[i];
	}

	//Load write list, and rebuild memory buffer
	writeList.Clear();
	for(std::list<WriteSaveEntry>::reverse_iterator i = writeSaveList.rbegin(); i != writeSaveList.rend(); ++i)
	{
		WriteEntry writeEntry(defaultValue);
		writeEntry.writeAddress = i->writeAddress;
		writeEntry.writeTime = i->writeTime;
		std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
		while((currentTimeslice != timesliceSaveList.end()) && (currentTimeslice->id != i->currentTimeslice))
		{
			++currentTimeslice;
		}
		if(currentTimeslice != timesliceSaveList.end())
		{
			writeEntry.currentTimeslice = currentTimeslice->timesliceLoad;
			writeEntry.newValue = memory[writeEntry.writeAddress];
			memory[writeEntry.writeAddress] = i->oldValue;
			writeList.PushToFront(writeEntry);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class T> std::list<typename RandomTimeAccessBufferNew<T>::TimesliceSaveEntry> RandomTimeAccessBufferNew<T>::LoadTimesliceEntries(IHeirarchicalStorageNode& node)
{
	std::list<TimesliceSaveEntry> timesliceSaveList;

	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"Timeslice")
		{
			IHeirarchicalStorageAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
			IHeirarchicalStorageAttribute* timesliceLength = i->GetAttribute(L"TimesliceLength");
			if((timesliceID != 0) && (timesliceLength != 0))
			{
				TimesliceSaveEntry entry(timesliceID->ExtractValue<unsigned int>(), timesliceLength->ExtractValue<double>());

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

	return timesliceSaveList;
}

//----------------------------------------------------------------------------------------
template<class T> std::list<typename RandomTimeAccessBufferNew<T>::WriteSaveEntry> RandomTimeAccessBufferNew<T>::LoadWriteEntries(IHeirarchicalStorageNode& node)
{
	std::list<WriteSaveEntry> writeSaveList;

	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if(i->GetName() == L"Write")
		{
			IHeirarchicalStorageAttribute* writeAddress = i->GetAttribute(L"WriteAddress");
			IHeirarchicalStorageAttribute* writeTime = i->GetAttribute(L"WriteTime");
			IHeirarchicalStorageAttribute* oldValue = i->GetAttribute(L"OldValue");
			IHeirarchicalStorageAttribute* timesliceID = i->GetAttribute(L"TimesliceID");
			if((writeAddress != 0) && (writeTime != 0) && (oldValue != 0) && (timesliceID != 0))
			{
				//Extract the write entry from the XML stream
				T oldValueData(defaultValue);
				oldValue->ExtractValue(oldValueData);
				WriteSaveEntry entry(writeAddress->ExtractValue<unsigned int>(), writeTime->ExtractValue<double>(), oldValueData, timesliceID->ExtractValue<unsigned int>());

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

	return writeSaveList;
}

//----------------------------------------------------------------------------------------
template<class T> void RandomTimeAccessBufferNew<T>::GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName, bool inlineData) const
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	std::vector<T> saveMemory;

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
	saveMemory.insert(saveMemory.begin(), memory.begin(), memory.end());

	//Build numeric ID's to identify each timeslice, and save the timeslice list state
	IHeirarchicalStorageNode& timesliceListState = node.CreateChild(L"TimesliceList");
	unsigned int id = 0;
	for(TimesliceList::Element* i = timesliceList.GetFirstEntry(); i != 0; i = i->next)
	{
		if(i->object.notDeleted)
		{
			timesliceSaveList.push_back(TimesliceSaveEntry(i, id));
			IHeirarchicalStorageNode timesliceEntry(L"Timeslice");
			timesliceEntry.AddAttribute(IHeirarchicalStorageAttribute(L"TimesliceID", id));
			timesliceEntry.AddAttribute(IHeirarchicalStorageAttribute(L"TimesliceLength", i->object.timesliceLength));
			timesliceListState.AddChild(timesliceEntry);
			++id;
		}
	}

	//Save the writeList state
	IHeirarchicalStorageNode& writeListState = node.CreateChild(L"WriteList");
	std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
	for(WriteList::Element* i = writeList.GetFirstEntry(); i != 0; i = i->next)
	{
		if(i->object.notDeleted)
		{
			IHeirarchicalStorageNode writeEntry(L"Write");
			while(currentTimeslice->timeslice != i->object.currentTimeslice)
			{
				++currentTimeslice;
			}
			writeEntry.AddAttribute(IHeirarchicalStorageAttribute(L"TimesliceID", currentTimeslice->id));
			writeEntry.AddAttribute(IHeirarchicalStorageAttribute(L"WriteAddress", i->object.writeAddress));
			writeEntry.AddAttribute(IHeirarchicalStorageAttribute(L"WriteTime", i->object.writeTime));
			writeEntry.AddAttribute(IHeirarchicalStorageAttribute(L"OldValue", saveMemory[i->object.writeAddress]));
			writeListState.AddChild(writeEntry);
			saveMemory[i->object.writeAddress] = i->object.newValue;
		}
	}

	//Add the memory buffer to the XML tree
	node.InsertBinaryData(saveMemory, bufferName, inlineData);
}
