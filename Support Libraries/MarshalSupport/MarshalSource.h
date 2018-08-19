#ifndef __MARSHALSOURCE_H__
#define __MARSHALSOURCE_H__
#include "IMarshalSource.h"
#include "MarshalPreprocessorMacros.h"
#include "IsOnlyMovable.h"
#include "GetLastNestedContainerElementType.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "NestedContainerHasKeys.h"
#include "CalculateDecomposedSTLContainerSize.h"
#include "CalculateSTLContainerNestingDepth.h"
#include "GetLastNestedContainerElementType.h"
#include "CreateSTLContainerKeyMarshallers.h"
#include "DeleteSTLContainerKeyMarshallers.h"
#include "CreateSTLContainerItemArray.h"
#include "DeleteSTLContainerItemArray.h"
#include "DecomposeSTLContainer.h"
#include "NestedMarshaller.h"
#include "NoBoundObjectTag.h"
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about using "this" pointer in initializer list. Our usage here is safe, and guaranteed by the
//standard. See section 12.6.2 [class.base.init] paragraph 7 of the standard for clarification.
#pragma warning(disable:4355)
//Disable warning about our use of type traits causing conditional expressions to be constant. The code behaves as
//intended, and the compiler is free to optimize away the dead branch.
#pragma warning(disable:4127)
//Disable warning about classes with virtual functions without a virtual destructor. We use protected inlined
//destructors where appropriate to prevent destruction through base class pointers.
#pragma warning(disable:4265)
#endif

//----------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class MarshalSourceInternal :public IMarshalSource<ContainerType, false, IsThisOrNextElementLastElement, IsOnlyMovable>
{
public:
	//Constructors
	MarshalSourceInternal(const ContainerType& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSourceInternal(ContainerType&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
#endif
	MarshalSourceInternal(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray, bool allowMove) const
	{
		//Retrieve the byte size of the innermost element to be contained in our flat arrays
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);

		//Calculate the required size of our flat arrays in order to hold a decomposed version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, *__sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, __sourceObject);
		size_t* elementSizeArrayTemp = new size_t[elementSizeArraySize];
		elementSizeArray = elementSizeArrayTemp;

		//Allocate key marshallers for any keyed containers in the _source object
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			Internal::INestedMarshallerBase** nestedMarshallerArrayTemp = new Internal::INestedMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(nestedMarshallerArrayTemp, 0, (ContainerType*)0);
			nestedMarshallerArray = nestedMarshallerArrayTemp;
		}

		//Decompose the container hierarchy into a flat array of the innermost elements, with a corresponding array of
		//sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t nestedMarshallerArrayIndex = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		if (__sourceDataMovable && allowMove)
		{
			Internal::DecomposeSTLContainer(itemArray, elementSizeArrayTemp, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, std::move(const_cast<ContainerType&>(*__sourceObject)));
		}
		else
#endif
		{
			Internal::DecomposeSTLContainer(itemArray, elementSizeArrayTemp, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, *__sourceObject);
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const
	{
		//Release the allocated data arrays
		Internal::DeleteSTLContainerItemArray(itemArray, __sourceObject);
		delete[] elementSizeArray;
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			Internal::DeleteSTLContainerKeyMarshallers(const_cast<Internal::INestedMarshallerBase**>(nestedMarshallerArray), 0, (ContainerType*)0);
			delete[] nestedMarshallerArray;
		}
	}

private:
	//Disable copying and moving
	MarshalSourceInternal(const MarshalSourceInternal& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal& operator=(const MarshalSourceInternal& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSourceInternal(MarshalSourceInternal&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal& operator=(MarshalSourceInternal&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	const ContainerType* __sourceObject;
	bool __sourceDataMovable;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsThisOrNextElementLastElement>
class MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, true> :public IMarshalSource<ContainerType, false, IsThisOrNextElementLastElement, true>
{
public:
	//Constructors
	MarshalSourceInternal(ContainerType&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSourceInternal(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray) const
	{
		//Retrieve the byte size of the innermost element to be contained in our flat arrays
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);

		//Calculate the required size of our flat arrays in order to hold a decomposed version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, *__sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, __sourceObject);
		size_t* elementSizeArrayTemp = new size_t[elementSizeArraySize];
		elementSizeArray = elementSizeArrayTemp;

		//Allocate key marshallers for any keyed containers in the _source object
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			Internal::INestedMarshallerBase** nestedMarshallerArrayTemp = new Internal::INestedMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(nestedMarshallerArrayTemp, 0, (ContainerType*)0);
			nestedMarshallerArray = nestedMarshallerArrayTemp;
		}

		//Decompose the container hierarchy into a flat array of the innermost elements, with a corresponding array of
		//sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t nestedMarshallerArrayIndex = 0;
		Internal::DecomposeSTLContainer(itemArray, elementSizeArrayTemp, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, std::move(*__sourceObject));
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const
	{
		//Release the allocated data arrays
		Internal::DeleteSTLContainerItemArray(itemArray, __sourceObject);
		delete[] elementSizeArray;
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			Internal::DeleteSTLContainerKeyMarshallers(const_cast<Internal::INestedMarshallerBase**>(nestedMarshallerArray), 0, (ContainerType*)0);
			delete[] nestedMarshallerArray;
		}
	}

private:
	//Disable copying and moving
	MarshalSourceInternal(const MarshalSourceInternal& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal& operator=(const MarshalSourceInternal& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal(MarshalSourceInternal&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal& operator=(MarshalSourceInternal&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	ContainerType* __sourceObject;
};
#endif
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class MarshalSource
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsThisOrNextElementLastElement>
class MarshalSource<ContainerType, false, IsThisOrNextElementLastElement, false> :public Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, false>
{
public:
	//Constructors
	MarshalSource(const ContainerType& a_sourceObject)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, false>(a_sourceObject)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(ContainerType&& a_sourceObject)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, false>(std::move(a_sourceObject))
	{ }
#endif
	MarshalSource(no_bound_object_tag* tag)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, false>(tag)
	{ }

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsThisOrNextElementLastElement>
class MarshalSource<ContainerType, false, IsThisOrNextElementLastElement, true> :public Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, true>
{
public:
	//Constructors
	MarshalSource(const ContainerType& a_sourceObject)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, true>(a_sourceObject)
	{ }
	MarshalSource(ContainerType&& a_sourceObject)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, true>(std::move(a_sourceObject))
	{ }
	MarshalSource(no_bound_object_tag* tag)
	:Internal::MarshalSourceInternal<ContainerType, IsThisOrNextElementLastElement, true>(tag)
	{ }

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
};
#endif

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalSource<ContainerType, true, true, false> :public IMarshalSource<ContainerType, true, true, false>
{
public:
	//Constructors
	MarshalSource(const ContainerType& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(ContainerType&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
#endif
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ContainerType*& _sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);
		_sourceData = __sourceObject;
		return __sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	const ContainerType* __sourceObject;
	bool __sourceDataMovable;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalSource<ContainerType, true, true, true> :public IMarshalSource<ContainerType, true, true, true>
{
public:
	//Constructors
	MarshalSource(ContainerType&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, ContainerType*& _sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);
		_sourceData = __sourceObject;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	ContainerType* __sourceObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalSource<std::vector<ElementType, Alloc>, false, true, false> :public IMarshalSource<std::vector<ElementType, Alloc>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::vector<ElementType, Alloc>& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(std::vector<ElementType, Alloc>&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
#endif
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& _sourceData, size_t& _sourceDataLength) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		_sourceData = __sourceObject->data();
#else
		_sourceData = __sourceObject->empty() ? 0 : &(*__sourceObject)[0];
#endif
		_sourceDataLength = __sourceObject->size();
		return __sourceDataMovable;
	}

	//Size methods
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const
	{
		return static_cast<size_t>(__sourceObject->size());
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const
	{
		return static_cast<size_t>(__sourceObject->capacity());
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	const std::vector<ElementType, Alloc>* __sourceObject;
	bool __sourceDataMovable;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalSource<std::vector<ElementType, Alloc>, false, true, true> :public IMarshalSource<std::vector<ElementType, Alloc>, false, true, true>
{
public:
	//Constructors
	MarshalSource(std::vector<ElementType, Alloc>&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, ElementType*& _sourceData, size_t& _sourceDataLength) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		_sourceData = __sourceObject->data();
		_sourceDataLength = __sourceObject->size();
	}

	//Size methods
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const
	{
		return static_cast<size_t>(__sourceObject->size());
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const
	{
		return static_cast<size_t>(__sourceObject->capacity());
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::vector<ElementType, Alloc>* __sourceObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class Alloc>
class MarshalSource<std::vector<bool, Alloc>, false, true, false> :public IMarshalSource<std::vector<bool, Alloc>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::vector<bool, Alloc>& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(const bool*& _sourceData, size_t& _sourceDataLength) const
	{
		//Since the C++ standard defines a rather poor specialization of std::vector for the bool type which is intended
		//to allow multiple boolean values to be packed into shared integer values values, and therefore can't provide a
		//reference to its underlying data members like the vector container normally can, we need to repack the
		//elements of this boolean vector here into a flat array so that we can marshal it.
		size_t _sourceObjectLength = __sourceObject->size();
		bool* itemArray = new bool[_sourceObjectLength];
		for (size_t i = 0; i < _sourceObjectLength; ++i)
		{
			itemArray[i] = (*__sourceObject)[(typename std::vector<bool, Alloc>::size_type)i];
		}

		//Return the raw vector data to the caller
		_sourceData = itemArray;
		_sourceDataLength = _sourceObjectLength;
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(const bool* _sourceData) const
	{
		delete[] _sourceData;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	const std::vector<bool, Alloc>* __sourceObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalSource<std::array<ElementType, ArraySize>, false, true, false> :public IMarshalSource<std::array<ElementType, ArraySize>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::array<ElementType, ArraySize>& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
	MarshalSource(std::array<ElementType, ArraySize>&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& _sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		_sourceData = __sourceObject->data();
		return __sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	const std::array<ElementType, ArraySize>* __sourceObject;
	bool __sourceDataMovable;
};

//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalSource<std::array<ElementType, ArraySize>, false, true, true> :public IMarshalSource<std::array<ElementType, ArraySize>, false, true, true>
{
public:
	//Constructors
	MarshalSource(std::array<ElementType, ArraySize>&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, ElementType*& _sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		_sourceData = __sourceObject->data();
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::array<ElementType, ArraySize>* __sourceObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true, false> :public IMarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::basic_string<ElementType, traits, Alloc>& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
	MarshalSource(const ElementType* _sourceObject)
	:_tempString(_sourceObject), __sourceObject(&_tempString), __sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(std::basic_string<ElementType, traits, Alloc>&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
#endif
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& _sourceData, size_t& _sourceDataLength) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		_sourceData = __sourceObject->data();
		_sourceDataLength = __sourceObject->size();
		return __sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::basic_string<ElementType, traits, Alloc> _tempString;
	const std::basic_string<ElementType, traits, Alloc>* __sourceObject;
	bool __sourceDataMovable;
};

//----------------------------------------------------------------------------------------
template<class T1, class T2>
class MarshalSource<std::pair<T1, T2>, false, true, false> :public IMarshalSource<std::pair<T1, T2>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::pair<T1, T2>& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(std::pair<T1, T2>&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
#endif
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual Marshal::Ret<T1, false> MARSHALSUPPORT_CALLINGCONVENTION RetrieveFirstElement(bool allowMove) const
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		if (__sourceDataMovable && allowMove)
		{
			return std::move(const_cast<T1&>(__sourceObject->first));
		}
#endif
		return __sourceObject->first;
	}
	virtual Marshal::Ret<T2, false> MARSHALSUPPORT_CALLINGCONVENTION RetrieveSecondElement(bool allowMove) const
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		if (__sourceDataMovable && allowMove)
		{
			return std::move(const_cast<T2&>(__sourceObject->second));
		}
#endif
		return __sourceObject->second;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	const std::pair<T1, T2>* __sourceObject;
	bool __sourceDataMovable;
};

//----------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class T1, class T2>
class MarshalSource<std::pair<T1, T2>, false, true, true> :public IMarshalSource<std::pair<T1, T2>, false, true, true>
{
public:
	//Constructors
	MarshalSource(std::pair<T1, T2>&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual Marshal::Ret<T1, true> MARSHALSUPPORT_CALLINGCONVENTION RetrieveFirstElement() const
	{
		return std::move(__sourceObject->first);
	}
	virtual Marshal::Ret<T2, true> MARSHALSUPPORT_CALLINGCONVENTION RetrieveSecondElement() const
	{
		return std::move(__sourceObject->second);
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::pair<T1, T2>* __sourceObject;
};
#endif

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------
template<>
class MarshalSource<std::tuple<>, false, true, false> :public IMarshalSource<std::tuple<>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::tuple<>& _sourceObject)
	{ }
	MarshalSource(std::tuple<>&& _sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
};

//----------------------------------------------------------------------------------------
template<class... Args>
class MarshalSource<std::tuple<Args...>, false, true, false> :public IMarshalSource<std::tuple<Args...>, false, true, false>
{
public:
	//Constructors
	MarshalSource(const std::tuple<Args...>& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(false)
	{ }
	MarshalSource(std::tuple<Args...>&& _sourceObject)
	:__sourceObject(&_sourceObject), __sourceDataMovable(true)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], bool allowMove) const
	{
		if (__sourceDataMovable && allowMove)
		{
			CreateElementMarshallersHelper<true, std::tuple_size<std::tuple<Args...>>::value-1, Args...>::CreateElementMarshallers(elementMarshallers, const_cast<std::tuple<Args...>&>(*__sourceObject));
		}
		else
		{
			CreateElementMarshallersHelper<false, std::tuple_size<std::tuple<Args...>>::value-1, Args...>::CreateElementMarshallers(elementMarshallers, *__sourceObject);
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION DeleteElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const
	{
		for (size_t i = 0; i < std::tuple_size<std::tuple<Args...>>::value; ++i)
		{
			delete elementMarshallers[i];
		}
	}

private:
	//Marshalling methods
	template<bool performMove, size_t TupleElementNo, class... Args>
	class CreateElementMarshallersHelper { };
	template<size_t TupleElementNo, class... Args>
	class CreateElementMarshallersHelper<true, TupleElementNo, Args...>
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[TupleElementNo] = (new Internal::NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<TupleElementNo>(std::move(_sourceObject)));
			CreateElementMarshallersHelper<true, TupleElementNo-1, Args...>::CreateElementMarshallers(elementMarshallers, _sourceObject);
		}
	};
	template<class... Args>
	class CreateElementMarshallersHelper<true, 0, Args...>
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[0] = (new Internal::NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<0>(std::move(_sourceObject)));
		}
	};
	template<size_t TupleElementNo, class... Args>
	class CreateElementMarshallersHelper<false, TupleElementNo, Args...>
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], const std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[TupleElementNo] = (new Internal::NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<TupleElementNo>(_sourceObject));
			CreateElementMarshallersHelper<false, TupleElementNo-1, Args...>::CreateElementMarshallers(elementMarshallers, _sourceObject);
		}
	};
	template<class... Args>
	class CreateElementMarshallersHelper<false, 0, Args...>
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], const std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[0] = (new Internal::NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<0>(_sourceObject));
		}
	};

	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	const std::tuple<Args...>* __sourceObject;
	bool __sourceDataMovable;
};

//----------------------------------------------------------------------------------------
template<class... Args>
class MarshalSource<std::tuple<Args...>, false, true, true> :public IMarshalSource<std::tuple<Args...>, false, true, true>
{
public:
	//Constructors
	MarshalSource(std::tuple<Args...>&& _sourceObject)
	:__sourceObject(&_sourceObject)
	{ }
	MarshalSource(no_bound_object_tag*)
	{ }

protected:
	//Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const
	{
		CreateElementMarshallersHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::CreateElementMarshallers(elementMarshallers, *__sourceObject);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION DeleteElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const
	{
		for (size_t i = 0; i < std::tuple_size<std::tuple<Args...>>::value; ++i)
		{
			delete elementMarshallers[i];
		}
	}

private:
	//Marshalling methods
	template<size_t TupleElementNo, class... Args>
	class CreateElementMarshallersHelper
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[TupleElementNo] = (new Internal::NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<TupleElementNo>(std::move(_sourceObject)));
			CreateElementMarshallersHelper<TupleElementNo-1, Args...>::CreateElementMarshallers(elementMarshallers, _sourceObject);
		}
	};
	template<class... Args>
	class CreateElementMarshallersHelper<0, Args...>
	{
	public:
		static void CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], std::tuple<Args...>& _sourceObject)
		{
			elementMarshallers[0] = (new Internal::NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>())->AddKeyWithReturn(std::get<0>(std::move(_sourceObject)));
		}
	};

	//Disable copying and moving
	MarshalSource(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(MarshalSource&& _source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::tuple<Args...>* __sourceObject;
};
#endif

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace MarshalSupport
#endif
