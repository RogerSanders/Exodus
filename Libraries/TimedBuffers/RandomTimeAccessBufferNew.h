#ifndef __RANDOMTIMEACCESSBUFFERNEW_H__
#define __RANDOMTIMEACCESSBUFFERNEW_H__
#include <list>
#include <vector>
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include <boost/thread/mutex.hpp>

//template<class T> class RandomTimeAccessBufferNew
//{
//public:
//	//Structures
//	struct TimesliceEntry;
//	struct WriteEntry;
//	struct WriteInfo;
//	struct TimesliceSaveEntry;
//	struct WriteSaveEntry;
//	struct AccessTarget;
//	typedef typename std::list<TimesliceEntry>::iterator Timeslice;
//
//	//Constructors
//	inline RandomTimeAccessBufferNew();
//	inline RandomTimeAccessBufferNew(unsigned int size);
//	inline RandomTimeAccessBufferNew(unsigned int size, const T& adefaultValue);
//
//	//Common functions: Overlapped access permissible
//	inline T Read(unsigned int address, const AccessTarget& accessTarget) const;
//	inline void Write(unsigned int address, const T& data, const AccessTarget& accessTarget);
//	inline unsigned int Size() const;
//
//	//Active functions: One active function at a time, overlapped with others
//	T Read(unsigned int address, double readTime) const;	//Reads writeList, committed
//	void Write(unsigned int address, double writeTime, const T& data);	//Adds writeList
//	void AddTimeslice(double timeslice);	//Adds timesliceList
//	T ReadLatest(unsigned int address) const;	//Reads writeList, committed
//	void WriteLatest(unsigned int address, const T& data);	//Writes writeList, Writes committed
//	void GetLatestBufferCopy(std::vector<T>& buffer) const;	//Reads writeList, committed
//	void Commit();	//Writes writeList
//	void Rollback();	//Writes writeList
//	Timeslice GetLatestTimeslice() const;	//Reads timesliceList
//
//	//Committed functions: One committed function at a time, overlapped with others
//	inline T ReadCommitted(unsigned int address) const;	//Reads committed
//	T ReadCommitted(unsigned int address, double readTime) const;	//Reads committed, writeList
//	inline void WriteCommitted(unsigned int address, const T& data);	//Writes committed
//	void AdvancePastTimeslice(const Timeslice& targetTimeslice);	//Writes writeList, committed
//	void AdvanceToTimeslice(const Timeslice& targetTimeslice);	//Writes writeList, committed
//	void AdvanceByTime(double step, const Timeslice& targetTimeslice);	//Writes writeList, committed
//	bool AdvanceByStep(const Timeslice& targetTimeslice);	//Writes writeList, committed
//	double GetNextWriteTime(const Timeslice& targetTimeslice);	//Reads writeList
//	WriteInfo GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice);	//Reads writeList
//
//	//Control functions: Require global exclusive access
//	void Initialize();
//	void Flush();
//	void Resize(unsigned int size);
//	void LoadState(XMLEntity* entity);
//	std::list<TimesliceSaveEntry> LoadTimesliceEntries(XMLEntity* entity);
//	std::list<WriteSaveEntry> LoadWriteEntries(XMLEntity* entity);
//	void GetState(XMLEntity* entity, const std::wstring& bufferName, bool inlineData = false) const;
//
//private:
//	std::list<TimesliceEntry> timesliceList;
//	Timeslice latestTimeslice;
//	std::list<WriteEntry> writeList;
//	std::vector<T> memory;
//	T defaultValue;
//	double currentTimeOffset;
//	//##TODO## Create an iterator member which stores a reference to the first non-committed
//	//entry in the writelist, for use with the advance functions
//};

#include "ThinList.pkg"

/*
This container is designed to be, effectively, an advanced, special purpose thread-safe
linked list, which is implemented without the use of any locking mechanisms or
synchronization primitives whatsoever. This should allow maximum performance without
compromising stability. It is one of the fundamental containers deployed in this system
to allow high performance shared access to data across threads.

In order to achieve the goal of no locks, there are some restrictions on how this
container can be used. Some sets of functions can be used simultaneously. Some sets
of functions 

We may need to make a small compromise here in order to make this container stable. It
may be necessary to add a lock to the Flush() function, as well as to the ReadLatest(),
WriteLatest(), and GetLatestBufferCopy() functions. The "Latest" functions are the only
functions which cross the boundary between the committed and uncommitted state. This
overlapped access might be safe for adding new entries to the buffer, depending on how
we do things, but I don't see any way to make it safe or reliable for removing entries
from the buffer. To this end, a lock spanning the "Latest" functions, and the Flush
function, should make it safe. Note that this will also allow us to use the "Latest"
functions across any number of threads simultaneously, although technically we already
could.
*/

template<class T> class RandomTimeAccessBufferNew
{
public:
	//Structures
	struct TimesliceEntry;
	struct WriteEntry;
	struct WriteInfo;
	struct TimesliceSaveEntry;
	struct WriteSaveEntry;
	struct AccessTarget;
	typedef typename ThinListDouble<TimesliceEntry>* Timeslice;

	//Constructors
	inline RandomTimeAccessBufferNew();
	inline RandomTimeAccessBufferNew(unsigned int size);
	inline RandomTimeAccessBufferNew(unsigned int size, const T& adefaultValue);

	//Common functions: Overlapped access permissible
	inline T Read(unsigned int address, const AccessTarget& accessTarget) const;
	inline void Write(unsigned int address, T data, const AccessTarget& accessTarget);
	inline unsigned int Size() const;

	//Active functions: One active function at a time, overlapped with others
	T Read(unsigned int address, double readTime) const;	//Reads writeList, committed
	void Write(unsigned int address, double writeTime, const T& data);	//Adds writeList
	void AddTimeslice(double timeslice);	//Adds timesliceList
	void Commit();	//Writes timesliceList
	void Rollback();	//Writes timesliceList, writeList
	Timeslice GetLatestTimeslice() const;	//Reads timesliceList

	//Latest functions: Locked - Overlapped with all
	T ReadLatest(unsigned int address) const;	//Reads writeList, committed
	void WriteLatest(unsigned int address, const T& data);	//Writes writeList, Writes committed
	void GetLatestBufferCopy(std::vector<T>& buffer) const;	//Reads writeList, committed

	//Committed functions: One committed function at a time, overlapped with others
	inline T ReadCommitted(unsigned int address) const;	//Reads committed
	T ReadCommitted(unsigned int address, double readTime) const;	//Reads committed, writeList
	inline void WriteCommitted(unsigned int address, const T& data);	//Writes committed
	void AdvancePastTimeslice(const Timeslice& targetTimeslice);	//Writes writeList, committed
	void AdvanceToTimeslice(const Timeslice& targetTimeslice);	//Writes writeList, committed
	void AdvanceByTime(double step, const Timeslice& targetTimeslice);	//Writes writeList, committed
	bool AdvanceByStep(const Timeslice& targetTimeslice);	//Writes writeList, committed
	double GetNextWriteTime(const Timeslice& targetTimeslice);	//Reads writeList
	WriteInfo GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice);	//Reads writeList
	void Flush(const Timeslice& targetTimeslice); //Locked - Deletes writeList, timesliceList

	//Control functions: Require global exclusive access
	void Initialize();
	void Resize(unsigned int size);
	void LoadState(IHeirarchicalStorageNode& node);
	void GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName, bool inlineData = false) const;

private:
	std::list<TimesliceSaveEntry> LoadTimesliceEntries(IHeirarchicalStorageNode* entity);
	std::list<WriteSaveEntry> LoadWriteEntries(IHeirarchicalStorageNode* entity);

private:
	typedef ThinListDoubleContainer<TimesliceEntry> TimesliceList;
	typedef ThinListDoubleContainer<WriteEntry> WriteList;

	TimesliceList timesliceList;
	WriteList writeList;

	Timeslice latestTimeslice;
	std::vector<T> memory;
	T defaultValue;
	double currentTimeOffset;
	//##TODO## Create an iterator member which stores a reference to the first non-committed
	//entry in the writelist, for use with the advance functions

	mutable boost::mutex accessLock;
};

#include "RandomTimeAccessBufferNew.inl"
#endif
