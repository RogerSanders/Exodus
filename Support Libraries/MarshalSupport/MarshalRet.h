#ifndef __MARSHALSUPPORT_MARSHALRET_H__
#define __MARSHALSUPPORT_MARSHALRET_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalSource.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#endif
namespace MarshalSupport {
namespace Marshal {

//Forward declarations
template <class T>
class In;
template <class T>
class InOut;

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about the presence of virtual functions without a virtual destructor.
//The class is internal, and is never derived from. We only use virtual functions to
//create code barriers between assemblies, so a virtual destructor is not required.
#pragma warning(disable:4265)
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif

//----------------------------------------------------------------------------------------
//Note that logically this type should be a private member of our Ret class, but if it was
//defined in this way, its total visibility would be limited to this class definition, and
//as such the compiler would be fully aware that no types could be derived from it,
//theoretically allowing the compiler to optimize away virtual function calls, with
//knowledge that they couldn't be overridden by any other type. We could define this class
//as a protected member instead, to ensure that the compiler cannot know that there are no
//derived types, while still keeping it private to users of the Ret class. To make
//specialization of the Ret class easier however, we define it as an external type here.
namespace Internal {
template <class T>
class RetData
{
public:
	//Constructors
	inline RetData(const T& source)
	:_sourceObject(source), _source(_sourceObject), _refCount(1)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline RetData(T&& source)
	:_sourceObject(std::move(source)), _source(_sourceObject), _refCount(1)
	{ }
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
		//Note that this method must be virtual, so that the assembly which allocated this
		//object will be the one in which the deletion is performed.
		--_refCount;
		if(_refCount <= 0)
		{
			delete this;
		}
	}

	//Marshal functions
	virtual const IMarshalSource<T>& GetMarshaller() const
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
	RetData(const RetData& source) MARSHALSUPPORT_DELETEMETHOD;
	RetData& operator=(const RetData& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	RetData(RetData&& source) = delete;
	RetData& operator=(RetData&& source) = delete;
#endif

private:
	//Note that these data members MUST be listed in this order. We need the _sourceObject
	//member to be initialized before the _source member within the constructor
	//initializer list.
	T _sourceObject;
	MarshalSource<T> _source;
	unsigned int _refCount;
};
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template <class T>
class Ret
{
public:
	//Friend declarations
	template <class T>
	friend class In;
	template <class T>
	friend class InOut;

public:
	//Constructors
	inline Ret(const T& source)
	:_data(new Internal::RetData<T>(source))
	{ }
	inline Ret(const In<T>& source)
	:_data(new Internal::RetData<T>(source))
	{ }
	inline Ret(const InOut<T>& source)
	:_data(new Internal::RetData<T>(source))
	{ }
	inline Ret(const Ret& source)
	:_data(source._data)
	{
		_data->IncrementRefCount();
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline Ret(T&& source)
	:_data(new Internal::RetData<T>(std::move(source)))
	{ }
	inline Ret(Ret&& source)
	:_data(source._data)
	{
		source._data = 0;
	}
#endif
	inline ~Ret()
	{
		if(_data != 0)
		{
			_data->DecrementRefCount();
		}
	}

	//Marshal functions
	inline T Get() const
	{
		return _data->GetMarshaller().MarshalTo();
	}
	inline void Get(T& targetObject) const
	{
		_data->GetMarshaller().MarshalTo(targetObject);
	}
	inline T GetWithoutMove() const
	{
		return _data->GetMarshaller().MarshalToWithoutMove();
	}
	inline void GetWithoutMove(T& targetObject) const
	{
		_data->GetMarshaller().MarshalToWithoutMove(targetObject);
	}

	//Implicit conversion
	inline operator T() const
	{
		return _data->GetMarshaller().MarshalToWithoutMove();
	}

private:
	Internal::RetData<T>* _data;
};

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

} //Close namespace Marshal
} //Close namespace MarshalSupport
#include "MarshalRetStringOperators.inl"
#endif
