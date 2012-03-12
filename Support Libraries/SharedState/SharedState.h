/*
Things to do:
-Completely phase out this class. We should be using associated window data rather than
the SharedState class.
*/
#ifndef __SHAREDSTATE_H__
#define __SHAREDSTATE_H__
#include <map>
#include <boost/thread/mutex.hpp>

template<class Key, class Data> class SharedState
{
public:
	//Entry creation and deletion
	void AddEntry(Key key, Data data);
	void DeleteEntry(Key key);

	//Entry retrieval
	Data* GetEntry(Key key);
	bool GetEntry(Key key, Data*& dataReference) const;
	bool GetEntry(Key key, Data& dataReference) const;

	//Entry modification
	bool SetEntry(Key key, const Data& dataReference);

private:
	typedef std::map<Key, Data> Index;
	typedef std::pair<Key, Data> IndexEntry;
	mutable boost::mutex accessMutex;
	Index index;
};

#include "SharedState.inl"
#endif
