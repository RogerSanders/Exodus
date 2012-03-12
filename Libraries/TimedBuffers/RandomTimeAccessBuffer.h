/*--------------------------------------------------------------------------------------*\
Things to do:
-Separate the functions into three categories:
1. Active functions which operate on the current timeslice. These have locks embedded to
allow concurrent access, and must be fully thread-safe.
2. Committed functions which operate on the committed data. These have no locks at all,
and can only be accessed by one thread at a time. All active functions are guaranteed not
to interfere with committed functions however.
3. Control functions which perform large and important operations. These functions have
no locks, but cannot be used at the same time as either committed or active functions.
-Based on the separation of functions proposed above, I would further propose that no
locks be used in this container at all. Rather, it becomes the responsibility of the
owner of the object to ensure that two threads don't attempt simultaneous access which
would violate these access rules. In all real-world cases so far, concurrent access
through the active functions is impossible anyway, since we always have an access lock
before this container can be accessed, making the embedded lock redundant. It's also
worth noting that we cannot fully enforce the access requirements proposed above using
an internal locking mechanism, with the restricted use of access locks which has been
mandated. Since we cannot truly make the container thread safe through internal means, we
should hand the responsibility over to the caller, and give them the rules under which
access must occur. This is consistent with how any other basic container would operate
anyway.
\*--------------------------------------------------------------------------------------*/

#ifndef __RANDOMTIMEACCESSBUFFER_H__
#define __RANDOMTIMEACCESSBUFFER_H__
#include <list>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"

//Any object can be stored, saved, or loaded from this container, provided it meets the
//following requirements:
//-It is copy constructible
//-It is assignable
//-It is streamable into and from Stream::ViewBinary and Stream::ViewText, either natively
//or through overloaded stream operators.

//##TODO## Finish implementing the optional cached copy of the latest buffer state
//##TODO## Consider making this class 64-bit compliant by using size_t for the address and
//size arguments. In fact, I would definitely do this, since it should cost us nothing
//internally in terms of performance.
//##TODO## Re-evaluate the locking on this class, and compare with RandomTimeAccessBufferNew.
//Note that we've had one idea about making the lock on read/write optional. In most
//cases, this buffer is internal to a device, and that device itself has locks to prevent
//concurrent access. If the buffer is being used in this kind of scenario, by instructing
//this container to skip the lock, we could get a performance boost.
template<class DataType, class TimesliceType> class RandomTimeAccessBuffer
{
public:
	//Structures
	struct TimesliceEntry;
	struct WriteEntry;
	struct WriteInfo;
	struct TimesliceSaveEntry;
	struct WriteSaveEntry;
	struct AccessTarget;
	struct AdvanceSession;

	//Typedefs
	typedef typename std::list<TimesliceEntry>::iterator Timeslice;

	//Constructors
	inline RandomTimeAccessBuffer();
	inline RandomTimeAccessBuffer(const DataType& adefaultValue);
	inline RandomTimeAccessBuffer(unsigned int size, bool akeepLatestCopy);
	inline RandomTimeAccessBuffer(unsigned int size, bool akeepLatestCopy, const DataType& adefaultValue);

	//Size functions
	inline unsigned int Size() const;
	void Resize(unsigned int size, bool akeepLatestCopy = false);

	//Access functions
	inline DataType Read(unsigned int address, const AccessTarget& accessTarget) const;
	inline void Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget);
	DataType Read(unsigned int address, TimesliceType readTime) const;
	void Write(unsigned int address, TimesliceType writeTime, const DataType& data);
	inline DataType& ReferenceCommitted(unsigned int address);
	inline DataType ReadCommitted(unsigned int address) const;
	DataType ReadCommitted(unsigned int address, TimesliceType readTime) const;
	//##TODO## Consider removing this function
	//##NOTE## I now strongly recommend removing this function. Not only does it
	//effectively not work, since the newly written value can be overwritten at any moment
	//from a buffered write, making it useless for debugger changes, it also doesn't work
	//with the latest memory state buffer. Implementing support for this function call
	//with the latest memory state buffer would make this function perform quite slowly
	//too, which is actually its only advantage over the WriteLatest function.
	inline void WriteCommitted(unsigned int address, const DataType& data);
	DataType ReadLatest(unsigned int address) const;
	void WriteLatest(unsigned int address, const DataType& data);
	void GetLatestBufferCopy(std::vector<DataType>& buffer) const;

	//Time management functions
	void Initialize();
	Timeslice GetLatestTimeslice();
	void AdvancePastTimeslice(const Timeslice& targetTimeslice);
	void AdvanceToTimeslice(const Timeslice& targetTimeslice);
	void AdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice);
	bool AdvanceByStep(const Timeslice& targetTimeslice);
	void AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice& targetTimeslice);
	TimesliceType GetNextWriteTime(const Timeslice& targetTimeslice) const;
	WriteInfo GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice);
	void Commit();
	void Rollback();
	void AddTimeslice(TimesliceType timeslice);

	//Savestate functions
	bool LoadState(IHeirarchicalStorageNode& node);
	bool LoadTimesliceEntries(IHeirarchicalStorageNode& node, std::list<TimesliceSaveEntry>& timesliceSaveList);
	bool LoadWriteEntries(IHeirarchicalStorageNode& node, std::list<WriteSaveEntry>& writeSaveList);
	bool GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName, bool inlineData = false) const;

private:
	//Time management functions
	TimesliceType GetNextWriteTimeNoLock(const Timeslice& targetTimeslice) const;

private:
	mutable boost::mutex accessLock;
	std::list<TimesliceEntry> timesliceList;
	Timeslice latestTimeslice;
	std::list<WriteEntry> writeList;
	std::vector<DataType> memory;
	bool latestMemoryBufferExists;
	std::vector<DataType> latestMemory;
	DataType defaultValue;
	TimesliceType currentTimeOffset;
};

#include "RandomTimeAccessBuffer.inl"
#endif
