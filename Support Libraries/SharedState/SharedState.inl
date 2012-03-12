//----------------------------------------------------------------------------------------
//Entry creation and deletion
//----------------------------------------------------------------------------------------
template<class Key, class Data> void SharedState<Key, Data>::AddEntry(Key key, Data data)
{
	boost::mutex::scoped_lock lock(accessMutex);
	index.insert(IndexEntry(key, data));
}

//----------------------------------------------------------------------------------------
template<class Key, class Data> void SharedState<Key, Data>::DeleteEntry(Key key)
{
	boost::mutex::scoped_lock lock(accessMutex);
	index.erase(key);
}

//----------------------------------------------------------------------------------------
//Entry retrieval
//----------------------------------------------------------------------------------------
template<class Key, class Data> Data* SharedState<Key, Data>::GetEntry(Key key)
{
	boost::mutex::scoped_lock lock(accessMutex);
	Index::iterator i = index.find(key);
	if(i != index.end())
	{
		return &(i->second);
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------------
template<class Key, class Data> bool SharedState<Key, Data>::GetEntry(Key key, Data*& dataReference) const
{
	boost::mutex::scoped_lock lock(accessMutex);
	Index::const_iterator i = index.find(key);
	if(i != index.end())
	{
		dataReference = &(i->second);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class Key, class Data> bool SharedState<Key, Data>::GetEntry(Key key, Data& dataReference) const
{
	boost::mutex::scoped_lock lock(accessMutex);
	Index::const_iterator i = index.find(key);
	if(i != index.end())
	{
		dataReference = i->second;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Entry modification
//----------------------------------------------------------------------------------------
template<class Key, class Data> bool SharedState<Key, Data>::SetEntry(Key key, const Data& dataReference)
{
	boost::mutex::scoped_lock lock(accessMutex);
	Index::iterator i = index.find(key);
	if(i != index.end())
	{
		i->second = dataReference;
		return true;
	}
	return false;
}
