#include "ReadWriteLock.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ReadWriteLock::ReadWriteLock()
:writeLocked(false)
{}

//----------------------------------------------------------------------------------------
//Lock functions
//----------------------------------------------------------------------------------------
void ReadWriteLock::ObtainReadLock()
{
	//Obtain the current thread ID
	DWORD currentThreadID = GetCurrentThreadId();

	//Wait until no write lock is present or pending from another thread
	boost::mutex::scoped_lock lock(accessMutex);
	while((writeLocked && (writeLockThreadID != currentThreadID)) || !writeLockPendingThreadIDs.empty())
	{
		writeLockReleased.wait(lock);
	}

	//Increment the read lock count for this thread
	++readLockCount[currentThreadID];
}

//----------------------------------------------------------------------------------------
bool ReadWriteLock::TryObtainReadLock()
{
	//Obtain the current thread ID
	DWORD currentThreadID = GetCurrentThreadId();

	//If a write lock is present or pending from another thread, return false.
	boost::mutex::scoped_lock lock(accessMutex);
	if((writeLocked && (writeLockThreadID != currentThreadID)) || !writeLockPendingThreadIDs.empty())
	{
		return false;
	}

	//Increment the read lock count for this thread, and return true.
	++readLockCount[currentThreadID];
	return true;
}

//----------------------------------------------------------------------------------------
void ReadWriteLock::ReleaseReadLock()
{
	//Obtain the current thread ID
	DWORD currentThreadID = GetCurrentThreadId();

	//Decrement the read lock count
	boost::mutex::scoped_lock lock(accessMutex);
	int newReadLockCount = --readLockCount[currentThreadID];

	//If we've just released the last read lock for this thread, remove this thread from
	//the list of threads with active read locks.
	if(newReadLockCount == 0)
	{
		//Remove the read lock entry for this thread
		readLockCount.erase(currentThreadID);

		//If there's at least one thread waiting for a write lock, notify a single waiting
		//thread that a write lock may now be available.
		if(!writeLockPendingThreadIDs.empty())
		{
			writeLockAvailable.notify_one();
		}
	}
}

//----------------------------------------------------------------------------------------
void ReadWriteLock::ObtainWriteLock()
{
	//Obtain the current thread ID
	DWORD currentThreadID = GetCurrentThreadId();

	//If the calling thread already has a write lock, increment the write lock count and
	//abort any further processing.
	boost::mutex::scoped_lock lock(accessMutex);
	if(writeLocked && (writeLockThreadID == currentThreadID))
	{
		++writeLockCount;
		return;
	}

	//If a write lock isn't currently available for the calling thread, wait for it to
	//become available.
	if(!IsNewWriteLockAvailableForThread(currentThreadID))
	{
		//Add this thread to the list of threads waiting for a write lock
		writeLockPendingThreadIDs.insert(currentThreadID);

		//Wait for a write lock to become available for this thread
		do
		{
			writeLockAvailable.wait(lock);
		}
		while(!IsNewWriteLockAvailableForThread(currentThreadID));

		//Remove this thread from the list of threads waiting for a write lock
		writeLockPendingThreadIDs.erase(currentThreadID);
	}

	//Assign an exclusive write lock to this thread
	writeLockThreadID = currentThreadID;
	writeLockCount = 1;
	writeLocked = true;
}

//----------------------------------------------------------------------------------------
bool ReadWriteLock::TryObtainWriteLock()
{
	//Obtain the current thread ID
	DWORD currentThreadID = GetCurrentThreadId();

	//If the calling thread already has a write lock, increment the write lock count and
	//abort any further processing.
	boost::mutex::scoped_lock lock(accessMutex);
	if(writeLocked && (writeLockThreadID == currentThreadID))
	{
		++writeLockCount;
		return true;
	}

	//If a write lock isn't currently available for the calling thread, return false.
	if(!IsNewWriteLockAvailableForThread(currentThreadID))
	{
		return false;
	}

	//Assign an exclusive write lock to this thread, and return true.
	writeLockThreadID = currentThreadID;
	writeLockCount = 1;
	writeLocked = true;
	return true;
}

//----------------------------------------------------------------------------------------
bool ReadWriteLock::IsNewWriteLockAvailableForThread(DWORD threadID)
{
	//If no write lock is currently active, return true.
	if(!writeLocked)
	{
		return true;
	}

	//If any read locks are currently owned by other threads that are not blocked waiting
	//for a write lock, return false.
	for(std::map<DWORD, unsigned int>::const_iterator i = readLockCount.begin(); i != readLockCount.end(); ++i)
	{
		DWORD readLockThreadID = i->first;
		if((readLockThreadID != threadID) && (writeLockPendingThreadIDs.find(readLockThreadID) == writeLockPendingThreadIDs.end()))
		{
			return false;
		}
	}

	//Since a write lock should be available for the target thread, return true.
	return true;
}

//----------------------------------------------------------------------------------------
void ReadWriteLock::ReleaseWriteLock()
{
	//Decrement the write lock count
	boost::mutex::scoped_lock lock(accessMutex);
	--writeLockCount;

	//If the write lock count has reached zero, release the write lock.
	if(writeLockCount == 0)
	{
		//Release the write lock
		writeLocked = false;

		//If any other threads are waiting for a write lock, notify a single waiting
		//thread that a write lock is now available, otherwise notify any threads waiting
		//for a read lock that the write lock has been released.
		if(!writeLockPendingThreadIDs.empty())
		{
			writeLockAvailable.notify_one();
		}
		else
		{
			writeLockReleased.notify_all();
		}
	}
}
