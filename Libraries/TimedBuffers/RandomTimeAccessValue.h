#ifndef __RANDOMTIMEACCESSVALUE_H__
#define __RANDOMTIMEACCESSVALUE_H__
#include <boost/thread/mutex.hpp>
#include <list>

//Any object can be stored, saved, or loaded from this container, provided it meets the
//following requirements:
//-It is copy constructible
//-It is assignable
//-It is streamable into and from Stream::ViewText, either natively or through overloaded
//stream operators.

template<class DataType, class TimesliceType> class RandomTimeAccessValue
{
public:
	//Structures
	struct TimesliceEntry;
	struct WriteEntry;
	struct WriteInfo;
	struct TimesliceSaveEntry;
	struct WriteSaveEntry;

	//Typedefs
	typedef typename std::list<TimesliceEntry>::iterator Timeslice;

	//Constructors
	RandomTimeAccessValue();
	RandomTimeAccessValue(const DataType& default);

	//Dereference operators
	const DataType& operator*() const;
	DataType& operator*();

	//Access functions
	DataType Read(TimesliceType readTime) const;
	void Write(TimesliceType writeTime, const DataType& data);
	DataType ReadCommitted() const;
	DataType ReadCommitted(TimesliceType readTime) const;
	void WriteCommitted(const DataType& data);
	DataType ReadLatest() const;
	void WriteLatest(const DataType& data);

	//Time management functions
	void Initialize();
	bool DoesLatestTimesliceExist() const;
	Timeslice GetLatestTimeslice();
	void AdvancePastTimeslice(const Timeslice& targetTimeslice);
	void AdvanceToTimeslice(const Timeslice& targetTimeslice);
	void AdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice);
	bool AdvanceByStep(const Timeslice& targetTimeslice);
	TimesliceType GetNextWriteTime(const Timeslice& targetTimeslice);
	WriteInfo GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice);
	void Commit();
	void Rollback();
	void AddTimeslice(TimesliceType timeslice);

	//Savestate functions
	bool LoadState(IHeirarchicalStorageNode& node);
	bool LoadTimesliceEntries(IHeirarchicalStorageNode& node, std::list<TimesliceSaveEntry>& timesliceSaveList);
	bool LoadWriteEntries(IHeirarchicalStorageNode& node, std::list<WriteSaveEntry>& writeSaveList);
	bool GetState(IHeirarchicalStorageNode& node) const;

private:
	mutable boost::mutex accessLock;
	std::list<TimesliceEntry> timesliceList;
	Timeslice latestTimeslice;
	std::list<WriteEntry> writeList;
	DataType value;
	TimesliceType currentTimeOffset;
};

#include "RandomTimeAccessValue.inl"
#endif
