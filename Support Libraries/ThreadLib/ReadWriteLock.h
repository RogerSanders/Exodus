#ifndef __READWRITELOCK_H__
#define __READWRITELOCK_H__
#include <WindowsSupport/WindowsSupport.pkg>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <map>
#include <set>

//Notes:
//-Locks are re-entrant. The same calling thread can take multiple write locks for example
// without deadlocking itself.
//-Read locks can be taken by multiple threads at any one given time
//-Write locks are exclusive, and can only be held by one thread at any given time, and
// no read locks are allowed at the same time as a write lock, unless those read locks are
// also held by the thread with the write lock, or are held by other threads waiting for a
// write lock.
//-A single thread can hold both a read and write lock at the same time, however, if a
// read lock is held, and a write lock is then obtained, the locked item is permitted to
// change while the write lock is being obtained. This means any state previously observed
// on the target item is potentially invalidated at the time a write lock is obtained,
// regardless of whether a read lock was already held. If this was not the case, two
// threads could deadlock each other if they both obtained read locks, then both requested
// write locks.
//-Write lock requests will not be blocked forever by constant overlapping requests for
// read locks from other threads, as write lock attempts take priority, and attempts to
// obtain read locks are blocked while any requests for write locks are pending.
class ReadWriteLock
{
public:
	//Constructors
	ReadWriteLock();

	//Lock functions
	void ObtainReadLock();
	bool TryObtainReadLock();
	void ReleaseReadLock();
	void ObtainWriteLock();
	bool TryObtainWriteLock();
	void ReleaseWriteLock();

private:
	//Lock functions
	bool IsNewWriteLockAvailableForThread(DWORD threadID);

private:
	//Thread synchronization
	boost::mutex accessMutex;
	boost::condition writeLockReleased;
	boost::condition writeLockAvailable;

	//Read lock info
	std::map<DWORD, unsigned int> readLockCount;

	//Write lock info
	bool writeLocked;
	DWORD writeLockThreadID;
	unsigned int writeLockCount;
	std::set<DWORD> writeLockPendingThreadIDs;
};

#endif
