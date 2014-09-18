#ifndef __MARSHALSUPPORT_MARSHALRET_H__
#define __MARSHALSUPPORT_MARSHALRET_H__
#include "InteropPreprocessorMacros.h"
#include "STLObjectSource.h"
namespace InteropSupport {
namespace Marshal {

//Note that logically this type should be a private member of the Ret class, but if it was
//defined in this way, its total scope would be limited to the class definition itself,
//and as such the compiler would be fully aware of the inheritance hierarchy of the type,
//and could theoretically optimize away virtual function calls, with knowledge that they
//couldn't be overridden by any other type. We therefore define the type separate from the
//class body here, to ensure that the compiler cannot know that there are no derived
//types. We wrap the type definition into an unnamed namespace to ensure its not visible
//to users of this library.
namespace {
template <class T>
class RetData
{
public:
	//Constructors
	RetData(const T& source)
	:_sourceObject(source), _source(_sourceObject), _refCount(1)
	{}
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	RetData(T&& source)
	:_sourceObject(std::move(source)), _source(_sourceObject), _refCount(1)
	{}
#endif

	//Reference count functions
	virtual void IncrementRefCount()
	{
		//Note that this method doesn't strictly need to be virtual, but it is better to
		//make it virtual so that the owning Ret object doesn't have to know anything
		//about the layout of this object. This would allow us to make changes to the
		//structure of this type without breaking compatibility with existing compiled
		//code.
		++_refCount;
	}
	virtual void DecrementRefCount()
	{
		//Note that this method must be virtual, so that the assembly which allocated
		//this object will be the one in which the deletion is performed.
		--_refCount;
		if(_refCount <= 0)
		{
			delete this;
		}
	}

	//Marshal functions
	virtual const ISTLObjectSource<T>& GetMarshaller() const
	{
		//Note that this method needs to be virtual, so that the calling method doesn't
		//know the real type of the marshaller object, and therefore can't optimize away
		//any virtual function boundaries. Note that we could store a reference to the
		//marshaller object instead and make this method non-virtual, but it is better to
		//make it virtual so that the caller doesn't have to know anything about the
		//layout of this object, as per the notes in IncrementRefCount.
		return _source;
	}

private:
	//Disable copying and moving
	RetData(const RetData& source);
	RetData& operator=(const RetData& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	RetData(RetData&& source);
	RetData& operator=(RetData&& source);
#endif

private:
	//Note that these data members MUST be listed in this order. We need the
	//_sourceObject member to be initialized before the _source member within the
	//constructor initializer list.
	unsigned int _refCount;
	T _sourceObject;
	STLObjectSource<T> _source;
};
} //Close unnamed namespace

template <class T>
class Ret
{
public:
	//Constructors
	Ret(const T& source)
	:_data(new RetData(source))
	{ }
	Ret(const Ret& source)
	:_data(source._data)
	{
		_data.IncrementRefCount();
	}
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	Ret(T&& source)
	:_data(new RetData(std::move(source)))
	{ }
	Ret(Ret&& source)
	:_data(source._data)
	{
		source._data = 0;
	}
#endif
	~Ret()
	{
		if(_data != 0)
		{
			_data.DecrementRefCount();
		}
	}

	//Marshal functions
	T Get() const
	{
		T targetObject;
		Get(targetObject);
		return targetObject;
	}
	void Get(T& targetObject) const
	{
		_data->GetMarshaller()->MarshalTo(targetObject);
	}

	//Implicit conversion
	operator T() const
	{
		return Get();
	}

private:
	RetData* _data;
};

} //Close namespace Marshal
} //Close namespace InteropSupport
#endif
