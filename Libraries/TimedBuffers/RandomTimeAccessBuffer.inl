#include "Stream/Stream.pkg"
#include "Debug/Debug.pkg"

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::TimesliceEntry
{
	TimesliceType timesliceLength;
	bool committed;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::WriteEntry
{
	WriteEntry()
	{}
	WriteEntry(const DataType& defaultValue)
	:newValue(defaultValue)
	{}
	WriteEntry(unsigned int awriteAddress, TimesliceType awriteTime, const DataType& anewValue, const Timeslice& acurrentTimeslice)
	:writeAddress(awriteAddress), writeTime(awriteTime), newValue(anewValue), currentTimeslice(acurrentTimeslice)
	{}

	unsigned int writeAddress;
	TimesliceType writeTime;
	DataType newValue;
	Timeslice currentTimeslice;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::WriteInfo
{
	WriteInfo()
	{}
	WriteInfo(const DataType& defaultValue)
	:newValue(defaultValue)
	{}

	bool exists;
	unsigned int writeAddress;
	TimesliceType writeTime;
	DataType newValue;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::TimesliceSaveEntry
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
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::WriteSaveEntry
{
	WriteSaveEntry(unsigned int awriteAddress, TimesliceType awriteTime, const DataType& aoldValue, unsigned int acurrentTimeslice)
	:writeAddress(awriteAddress), writeTime(awriteTime), oldValue(aoldValue), currentTimeslice(acurrentTimeslice)
	{}
	unsigned int writeAddress;
	TimesliceType writeTime;
	DataType oldValue;
	unsigned int currentTimeslice;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::AccessTarget
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
	AccessTarget& AccessCommitted(TimesliceType aaccessTime)
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
	AccessTarget& AccessTime(TimesliceType aaccessTime)
	{
		accessTarget = ACCESSTARGET_TIME;
		accessTime = aaccessTime;
		return *this;
	}
	AccessTarget& AccessTimeslice(TimesliceType aaccessTime, const Timeslice& aaccessTimeslice)
	{
		accessTarget = ACCESSTARGET_TIMESLICE;
		accessTime = aaccessTime;
		accessTimeslice = aaccessTimeslice;
		return *this;
	}

	Target accessTarget;
	TimesliceType accessTime;
	Timeslice accessTimeslice;
};

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> struct RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceSession
{
	AdvanceSession()
	:initialized(false)
	{}

	void Reset()
	{
		initialized = false;
	}

	bool initialized;
	TimesliceType timeRemovedDuringSession;
	TimesliceType initialTimeOffset;
	TimesliceType nextWriteTime;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessBuffer<DataType, TimesliceType>::RandomTimeAccessBuffer()
:latestMemoryBufferExists(false)
{}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessBuffer<DataType, TimesliceType>::RandomTimeAccessBuffer(const DataType& adefaultValue)
:defaultValue(adefaultValue)
{}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessBuffer<DataType, TimesliceType>::RandomTimeAccessBuffer(unsigned int size, bool akeepLatestCopy)
:latestMemoryBufferExists(akeepLatestCopy)
{
	memory.resize(size);
	if(latestMemoryBufferExists)
	{
		latestMemory.resize(size);
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> RandomTimeAccessBuffer<DataType, TimesliceType>::RandomTimeAccessBuffer(unsigned int size, bool akeepLatestCopy, const DataType& adefaultValue)
:defaultValue(adefaultValue), latestMemoryBufferExists(akeepLatestCopy)
{
	memory.resize(size, defaultValue);
	if(latestMemoryBufferExists)
	{
		latestMemory.resize(size, defaultValue);
	}
}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> unsigned int RandomTimeAccessBuffer<DataType, TimesliceType>::Size() const
{
	return (unsigned int)memory.size();
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Resize(unsigned int size, bool akeepLatestCopy)
{
	boost::mutex::scoped_lock lock(accessLock);
	latestMemoryBufferExists = akeepLatestCopy;
	memory.resize(size, defaultValue);
	if(latestMemoryBufferExists)
	{
		latestMemory.resize(size);
	}
	else
	{
		latestMemory.clear();
	}
}

//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessBuffer<DataType, TimesliceType>::Read(unsigned int address, const AccessTarget& accessTarget) const
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
	ReleaseAssert(false);
	return DataType(defaultValue);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget)
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
	ReleaseAssert(false);
}

//----------------------------------------------------------------------------------------
//This function starts at the end of the buffered writes, and works its way back to the
//beginning, stopping when it finds a write to the target address. We do this to optimize
//ad-hoc reads of a buffer when there are a large number of reads and writes occurring to
//a small number of addresses.
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessBuffer<DataType, TimesliceType>::Read(unsigned int address, TimesliceType readTime) const
{
	boost::mutex::scoped_lock lock(accessLock);

	//Search for written values in the current timeslice
	std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice))
	{
		if((i->writeAddress == address) && (i->writeTime <= readTime))
		{
			return i->newValue;
		}
		++i;
	}
	//Search for written values in previous timeslices
	while(i != writeList.rend())
	{
		if(i->writeAddress == address)
		{
			return i->newValue;
		}
		++i;
	}

	//Default to the committed value
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Write(unsigned int address, TimesliceType writeTime, const DataType& data)
{
	boost::mutex::scoped_lock lock(accessLock);

	WriteEntry entry(address, writeTime, data, latestTimeslice);

	//Find the correct location in the list to insert the new write entry. The writeList
	//must be sorted from earliest to latest write by time.
	bool updateLatestBufferContents = true;
	std::list<WriteEntry>::reverse_iterator i = writeList.rbegin();
	while((i != writeList.rend()) && (i->currentTimeslice == latestTimeslice) && (i->writeTime > writeTime))
	{
		if(latestMemoryBufferExists && (i->writeAddress == address))
		{
			updateLatestBufferContents = false;
		}
		++i;
	}
	writeList.insert(i.base(), entry);

	//If we're holding a cached copy of the latest memory state, update it.
	if(latestMemoryBufferExists && updateLatestBufferContents)
	{
		latestMemory[address] = data;
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType& RandomTimeAccessBuffer<DataType, TimesliceType>::ReferenceCommitted(unsigned int address)
{
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessBuffer<DataType, TimesliceType>::ReadCommitted(unsigned int address) const
{
	return memory[address];
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessBuffer<DataType, TimesliceType>::ReadCommitted(unsigned int address, TimesliceType readTime) const
{
	boost::mutex::scoped_lock lock(accessLock);
	TimesliceType currentTimeBase = 0;

	//Default to the committed value
	DataType foundValue = memory[address];

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
		//If the write occurred to the same address as our target, set it as the new
		//value for our target address at the time of the read.
		if(i->writeAddress == address)
		{
			foundValue = i->newValue;
		}
		++i;
	}

	return foundValue;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::WriteCommitted(unsigned int address, const DataType& data)
{
	//##NOTE## We don't update the latest memory buffer state here, since it would be very
	//costly in performance to do so, and the premise of this function is kind of flawed
	//to begin with. I strongly recommend removing this function entirely. We shouldn't
	//need it anywhere, and the only place it is actually called right now is the old VDP
	//core, and incorrectly too from what I can tell. Calling WriteLatest is the correct
	//thing to do in this kind of case, not WriteCommitted.
	memory[address] = data;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> DataType RandomTimeAccessBuffer<DataType, TimesliceType>::ReadLatest(unsigned int address) const
{
	//If we don't have a cached copy of the latest memory state saved, determine the
	//latest value for the target memory address by iterating through the uncommitted
	//write list.
	if(!latestMemoryBufferExists)
	{
		boost::mutex::scoped_lock lock(accessLock);

		//Search for written values in any timeslice
		std::list<WriteEntry>::const_reverse_iterator i = writeList.rbegin();
		while(i != writeList.rend())
		{
			if(i->writeAddress == address)
			{
				return i->newValue;
			}
			++i;
		}

		//Default to the committed value
		return memory[address];
	}

	//Return the cached latest value for this memory address
	return latestMemory[address];
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::WriteLatest(unsigned int address, const DataType& data)
{
	boost::mutex::scoped_lock lock(accessLock);

	//Erase any write entries to this address in any timeslice. We do this to prevent
	//uncommitted writes from overwriting this change. This write function should make
	//the new value visible from all access functions.
	std::list<WriteEntry>::iterator i = writeList.begin();
	while(i != writeList.end())
	{
		if(i->writeAddress == address)
		{
			writeList.erase(i++);
		}
		else
		{
			++i;
		}
	}

	//Write the new value directly to the committed state
	memory[address] = data;

	//If we're holding a cached copy of the latest memory state, update it.
	if(latestMemoryBufferExists)
	{
		latestMemory[address] = data;
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::GetLatestBufferCopy(std::vector<DataType>& buffer) const
{
	if(!latestMemoryBufferExists)
	{
		boost::mutex::scoped_lock lock(accessLock);

		//Resize the target buffer to match the size of the source buffer, and populate
		//with the committed memory state.
		buffer.assign(memory.begin(), memory.end());

		//Commit each buffered write entry to the target buffer
		for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
		{
			buffer[i->writeAddress] = i->newValue;
		}
	}
	else
	{
		//Resize the target buffer to match the size of the source buffer, and populate
		//with the latest memory state.
		buffer.assign(latestMemory.begin(), latestMemory.end());
	}
}

//----------------------------------------------------------------------------------------
//Time management functions
//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Initialize()
{
	boost::mutex::scoped_lock lock(accessLock);

	//Initialize buffers
	for(unsigned int i = 0; i < memory.size(); ++i)
	{
		memory[i] = defaultValue;
	}
	if(latestMemoryBufferExists)
	{
		for(unsigned int i = 0; i < latestMemory.size(); ++i)
		{
			latestMemory[i] = defaultValue;
		}
	}
	writeList.clear();
	timesliceList.clear();
	currentTimeOffset = 0;
	latestTimeslice = timesliceList.end();
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> typename RandomTimeAccessBuffer<DataType, TimesliceType>::Timeslice RandomTimeAccessBuffer<DataType, TimesliceType>::GetLatestTimeslice()
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::AdvancePastTimeslice(const Timeslice& targetTimeslice)
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
		memory[i->writeAddress] = i->newValue;
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceToTimeslice(const Timeslice& targetTimeslice)
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
		memory[i->writeAddress] = i->newValue;
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice)
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
		memory[i->writeAddress] = i->newValue;
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
template<class DataType, class TimesliceType> bool RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceByStep(const Timeslice& targetTimeslice)
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
			memory[i->writeAddress] = i->newValue;
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice& targetTimeslice)
{
	//Ensure this session has been initialized
	//##TODO## Make a separate function which creates and initializes a session, and
	//remove this check here.
	if(!advanceSession.initialized)
	{
		boost::mutex::scoped_lock lock(accessLock);

		//Initialize the base time settings for this session
		advanceSession.timeRemovedDuringSession = 0;
		advanceSession.initialTimeOffset = currentTimeOffset;

		//Get the next write time, relative to the start of this session.
		advanceSession.nextWriteTime = (advanceSession.timeRemovedDuringSession + GetNextWriteTimeNoLock(targetTimeslice)) - advanceSession.initialTimeOffset;

		//Mark the session as initialized
		advanceSession.initialized = true;
	}

	//Check if we're going to reach a write in this step. We perform this conditional test
	//here before the inner loop so that we can avoid any locks for updates where we don't
	//reach a write.
	if(currentProgress >= advanceSession.nextWriteTime)
	{
		//Since a write needs to be processed, obtain a lock, and loop around until there
		//are no writes left within the update step.
		boost::mutex::scoped_lock lock(accessLock);
		bool done = false;
		while(!done && (currentProgress >= advanceSession.nextWriteTime))
		{
			TimesliceType step = (((currentProgress - advanceSession.timeRemovedDuringSession) + advanceSession.initialTimeOffset) - currentTimeOffset);
			TimesliceType currentTimeBase = 0;

			//Commit buffered writes which we have passed in this step
			std::list<TimesliceEntry>::iterator currentTimeslice = timesliceList.begin();
			std::list<WriteEntry>::iterator i = writeList.begin();
			bool foundNextWrite = false;
			bool reachedEndOfTargetTimeslice = false;
			while((i != writeList.end()) && !foundNextWrite && !reachedEndOfTargetTimeslice)
			{
				//Advance through the timeslice list until we find the timeslice matching
				//the next buffered write, or we pass the end of this time step.
				while((currentTimeslice != i->currentTimeslice)
					&& (currentTimeslice != targetTimeslice)
					&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step))
				{
					currentTimeBase += currentTimeslice->timesliceLength;
					++currentTimeslice;
				}
				//If the next buffered write is outside the time step, terminate
				//processing of buffered writes.
				if((currentTimeslice != i->currentTimeslice))
				{
					reachedEndOfTargetTimeslice = true;
					continue;
				}
				if(((currentTimeBase + i->writeTime) - currentTimeOffset) > step)
				{
					//We capture the next write time here, so we have it to perform the
					//next step in this session.
					advanceSession.nextWriteTime = (advanceSession.timeRemovedDuringSession + currentTimeBase + i->writeTime) - advanceSession.initialTimeOffset;
					foundNextWrite = true;
					continue;
				}
				//If the next buffered write has been passed during this update, commit
				//it, and advance to the next write.
				memory[i->writeAddress] = i->newValue;
				++i;
			}
			//Check for any remaining timeslices which have expired
			while((currentTimeslice != targetTimeslice)
				&& (((currentTimeBase + currentTimeslice->timesliceLength) - currentTimeOffset) <= step))
			{
				currentTimeBase += currentTimeslice->timesliceLength;
				++currentTimeslice;
			}
			//Set the amount of the current timeslice which has been stepped through as
			//the time offset for the next step operation.
			currentTimeOffset = (currentTimeOffset + step) - currentTimeBase;

			//If we didn't find the next write after the current step, set the next write
			//time to be the end of the target timeslice, and explicitly break out of the
			//advance loop.
			if(!foundNextWrite)
			{
				TimesliceType timeTillEndOfTargetTimeslice = 0;
				std::list<TimesliceEntry>::iterator tempTimeslice = currentTimeslice;
				while(tempTimeslice != targetTimeslice)
				{
					timeTillEndOfTargetTimeslice += tempTimeslice->timesliceLength;
					++tempTimeslice;
				}
				timeTillEndOfTargetTimeslice += tempTimeslice->timesliceLength;
				advanceSession.nextWriteTime = (advanceSession.timeRemovedDuringSession + currentTimeBase + timeTillEndOfTargetTimeslice) - advanceSession.initialTimeOffset;
				done = true;
			}

			//Erase buffered writes which have been committed, and timeslices which have
			//expired.
			writeList.erase(writeList.begin(), i);
			timesliceList.erase(timesliceList.begin(), currentTimeslice);

			//If we've just removed some timeslices as a result of this step, advance the
			//base address of the session.
			advanceSession.timeRemovedDuringSession += currentTimeBase;
		}
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> TimesliceType RandomTimeAccessBuffer<DataType, TimesliceType>::GetNextWriteTimeNoLock(const Timeslice& targetTimeslice) const
{
	bool foundWrite = false;
	TimesliceType nextWriteTime = 0;
	TimesliceType currentTimeBase = 0;

	//Search the write list for the next buffered write inside this time step
	std::list<TimesliceEntry>::const_iterator currentTimeslice = timesliceList.begin();
	std::list<WriteEntry>::const_iterator i = writeList.begin();
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
template<class DataType, class TimesliceType> TimesliceType RandomTimeAccessBuffer<DataType, TimesliceType>::GetNextWriteTime(const Timeslice& targetTimeslice) const
{
	boost::mutex::scoped_lock lock(accessLock);
	return GetNextWriteTimeNoLock(targetTimeslice);
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> typename RandomTimeAccessBuffer<DataType, TimesliceType>::WriteInfo RandomTimeAccessBuffer<DataType, TimesliceType>::GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
{
	boost::mutex::scoped_lock lock(accessLock);

	TimesliceType currentTimeBase = 0;
	unsigned int currentIndex = 0;
	WriteInfo writeInfo(defaultValue);
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
				writeInfo.writeAddress = i->writeAddress;
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Commit()
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
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::Rollback()
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

	//If we're caching the latest memory state, rebuild the buffer contents.
	if(latestMemoryBufferExists)
	{
		//Populate the latest memory buffer with the committed memory state
		latestMemory.assign(memory.begin(), memory.end());

		//Commit each buffered write entry to the latest memory buffer
		for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
		{
			latestMemory[i->writeAddress] = i->newValue;
		}
	}
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> void RandomTimeAccessBuffer<DataType, TimesliceType>::AddTimeslice(TimesliceType timeslice)
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
template<class DataType, class TimesliceType> bool RandomTimeAccessBuffer<DataType, TimesliceType>::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	std::list<WriteSaveEntry> writeSaveList;

	//Load the current time offset
	node.ExtractAttribute(L"CurrentTimeOffset", currentTimeOffset);

	//Read saved data from the XML tree
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"TimesliceList")
		{
			if(!LoadTimesliceEntries(*(*i), timesliceSaveList))
			{
				return false;
			}
		}
		else if((*i)->GetName() == L"WriteList")
		{
			if(!LoadWriteEntries(*(*i), writeSaveList))
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

	//Recalculate the latest timeslice
	if(timesliceList.empty())
	{
		latestTimeslice = timesliceList.end();
	}
	else
	{
		latestTimeslice = (++timesliceList.rbegin()).base();
	}

	//Load memory buffer
	std::vector<unsigned char> savedMemory;
	node.ExtractBinaryData(savedMemory);
	Stream::Buffer saveBuffer;
	if(!saveBuffer.WriteData(savedMemory))
	{
		return false;
	}
	saveBuffer.SetStreamPos(0);
	Stream::ViewBinary bufferView(saveBuffer);
	for(unsigned int i = 0; i < memory.size(); ++i)
	{
		bufferView >> memory[i];
	}

	//Load write list, and rebuild memory buffer
	writeList.clear();
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
			writeList.push_front(writeEntry);
		}
	}

	//If we're caching the latest memory state, rebuild the buffer contents.
	if(latestMemoryBufferExists)
	{
		//Populate the latest memory buffer with the committed memory state
		latestMemory.assign(memory.begin(), memory.end());

		//Commit each buffered write entry to the latest memory buffer
		for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
		{
			latestMemory[i->writeAddress] = i->newValue;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class DataType, class TimesliceType> bool RandomTimeAccessBuffer<DataType, TimesliceType>::LoadTimesliceEntries(IHeirarchicalStorageNode& node, std::list<TimesliceSaveEntry>& timesliceSaveList)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Timeslice")
		{
			IHeirarchicalStorageAttribute* timesliceID = (*i)->GetAttribute(L"TimesliceID");
			IHeirarchicalStorageAttribute* timesliceLength = (*i)->GetAttribute(L"TimesliceLength");
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
template<class DataType, class TimesliceType> bool RandomTimeAccessBuffer<DataType, TimesliceType>::LoadWriteEntries(IHeirarchicalStorageNode& node, std::list<WriteSaveEntry>& writeSaveList)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Write")
		{
			IHeirarchicalStorageAttribute* writeAddress = (*i)->GetAttribute(L"WriteAddress");
			IHeirarchicalStorageAttribute* writeTime = (*i)->GetAttribute(L"WriteTime");
			IHeirarchicalStorageAttribute* oldValue = (*i)->GetAttribute(L"OldValue");
			IHeirarchicalStorageAttribute* timesliceID = (*i)->GetAttribute(L"TimesliceID");
			if((writeAddress != 0) && (writeTime != 0) && (oldValue != 0) && (timesliceID != 0))
			{
				//Extract the write entry from the XML stream
				DataType oldValueData(defaultValue);
				oldValue->ExtractValue(oldValueData);
				WriteSaveEntry entry(writeAddress->ExtractValue<unsigned int>(), writeTime->ExtractValue<TimesliceType>(), oldValueData, timesliceID->ExtractValue<unsigned int>());

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
template<class DataType, class TimesliceType> bool RandomTimeAccessBuffer<DataType, TimesliceType>::GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName, bool inlineData) const
{
	std::list<TimesliceSaveEntry> timesliceSaveList;
	std::vector<DataType> saveMemory;

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
	for(std::list<TimesliceEntry>::const_iterator i = timesliceList.begin(); i != timesliceList.end(); ++i)
	{
		timesliceSaveList.push_back(TimesliceSaveEntry(i, id));
		IHeirarchicalStorageNode& timesliceEntry = timesliceListState.CreateChild(L"Timeslice");
		timesliceEntry.CreateAttribute(L"TimesliceID", id);
		timesliceEntry.CreateAttribute(L"TimesliceLength", i->timesliceLength);
		++id;
	}

	//Save the writeList state
	IHeirarchicalStorageNode& writeListState = node.CreateChild(L"WriteList");
	std::list<TimesliceSaveEntry>::iterator currentTimeslice = timesliceSaveList.begin();
	for(std::list<WriteEntry>::const_iterator i = writeList.begin(); i != writeList.end(); ++i)
	{
		IHeirarchicalStorageNode& writeEntry = writeListState.CreateChild(L"Write");
		while(currentTimeslice->timeslice != i->currentTimeslice)
		{
			++currentTimeslice;
		}
		writeEntry.CreateAttribute(L"TimesliceID", currentTimeslice->id);
		writeEntry.CreateAttribute(L"WriteAddress", i->writeAddress);
		writeEntry.CreateAttribute(L"WriteTime", i->writeTime);
		writeEntry.CreateAttribute(L"OldValue", saveMemory[i->writeAddress]);
		saveMemory[i->writeAddress] = i->newValue;
	}

	//Add the memory buffer to the XML tree
	node.InsertBinaryData(saveMemory, bufferName, inlineData);

	return true;
}
