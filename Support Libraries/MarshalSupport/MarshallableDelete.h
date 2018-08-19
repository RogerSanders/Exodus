#ifndef __MARSHALLABLEDELETE_H__
#define __MARSHALLABLEDELETE_H__
#include "MarshalPreprocessorMacros.h"
namespace MarshalSupport {

//Disable warning about the presence of virtual functions without a virtual destructor. The class is internal, and is
//never derived from. We only use virtual functions to create code barriers between assemblies, so a virtual destructor
//is not required.
#ifdef _MSC_VER
#pragma warning(push)
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

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class T>
class MarshallableDelete
{
public:
	//Constructors
	inline MarshallableDelete()
	:_data(new MarshallableDeleteData())
	{ }
	inline MarshallableDelete(const MarshallableDelete& source)
	:_data(source._data)
	{
		_data->IncrementRefCount();
	}
	inline ~MarshallableDelete()
	{
		_data->DecrementRefCount();
	}

	//Function operator
	inline void operator()(T* data)
	{
		_data->PerformDelete(data);
	}

	//Note that we make our nested object protected rather than private here, so that the compiler can't assume no types
	//are derived from it, otherwise a clever compiler could theoretically optimize away the virtual function calls.
protected:
	//Nested types
	class MarshallableDeleteData
	{
	public:
		MarshallableDeleteData()
		:_refCount(1)
		{ }

		//Reference count methods
		virtual void MARSHALSUPPORT_CALLINGCONVENTION IncrementRefCount()
		{
			//Note that this method doesn't strictly need to be virtual, but it is better to make it virtual so that the
			//owning object doesn't have to know anything about the layout of this object. This would allow us to make
			//changes to the structure of this type without breaking compatibility with existing compiled code.
			++_refCount;
		}
		virtual void MARSHALSUPPORT_CALLINGCONVENTION DecrementRefCount()
		{
			//Note that this method must be virtual, so that the assembly which allocated this object will be the one in
			//which the deletion is performed.
			--_refCount;
			if (_refCount <= 0)
			{
				delete this;
			}
		}

		//Delete methods
		virtual void MARSHALSUPPORT_CALLINGCONVENTION PerformDelete(T* data)
		{
			delete data;
		}

	private:
		unsigned int _refCount;
	};

private:
	MarshallableDeleteData* _data;
};

//----------------------------------------------------------------------------------------
template<class T>
class MarshallableDelete<T[]>
{
public:
	//Constructors
	inline MarshallableDelete()
	:_data(new MarshallableDeleteData())
	{ }
	inline MarshallableDelete(const MarshallableDelete& source)
	:_data(source._data)
	{
		_data->IncrementRefCount();
	}
	inline ~MarshallableDelete()
	{
		_data->DecrementRefCount();
	}

	//Function operator
	inline void operator()(T* data)
	{
		_data->PerformDelete(data);
	}

	//Note that we make our nested object protected rather than private here, so that the compiler can't assume no types
	//are derived from it, otherwise a clever compiler could theoretically optimize away the virtual function calls.
protected:
	//Nested types
	class MarshallableDeleteData
	{
	public:
		MarshallableDeleteData()
		:_refCount(1)
		{ }

		//Reference count methods
		virtual void MARSHALSUPPORT_CALLINGCONVENTION IncrementRefCount()
		{
			//Note that this method doesn't strictly need to be virtual, but it is better to make it virtual so that the
			//owning object doesn't have to know anything about the layout of this object. This would allow us to make
			//changes to the structure of this type without breaking compatibility with existing compiled code.
			++_refCount;
		}
		virtual void MARSHALSUPPORT_CALLINGCONVENTION DecrementRefCount()
		{
			//Note that this method must be virtual, so that the assembly which allocated this object will be the one in
			//which the deletion is performed.
			--_refCount;
			if (_refCount <= 0)
			{
				delete this;
			}
		}

		//Delete methods
		virtual void MARSHALSUPPORT_CALLINGCONVENTION PerformDelete(T* data)
		{
			delete[] data;
		}

	private:
		unsigned int _refCount;
	};

private:
	MarshallableDeleteData* _data;
};
#endif

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

} //Close namespace MarshalSupport
#endif
