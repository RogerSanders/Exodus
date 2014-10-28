#ifndef __MARSHALSOURCE_H__
#define __MARSHALSOURCE_H__
#include "IMarshalSource.h"
#include "MarshalPreprocessorMacros.h"
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
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about using "this" pointer in initializer list. Our usage here is safe,
//and guaranteed by the standard. See section 12.6.2 [class.base.init] paragraph 7 of the
//standard for clarification.
#pragma warning(disable:4355)
//Disable warning about our use of type traits causing conditional expressions to be
//constant. The code behaves as intended, and the compiler is free to optimize away the
//dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class MarshalSourceInternal :public IMarshalSource<ContainerType, false, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	MarshalSourceInternal(const ContainerType& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSourceInternal(ContainerType&& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(true)
	{ }
#endif

protected:
	//Marshalling methods
	virtual void RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const*& keyMarshallerArray, bool allowMove) const
	{
		//Retrieve the byte size of the innermost element to be contained in our flat
		//arrays
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);

		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, _sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, &_sourceObject);
		size_t* elementSizeArrayTemp = new size_t[elementSizeArraySize];
		elementSizeArray = elementSizeArrayTemp;

		//Allocate key marshallers for any keyed containers in the source object
		if(Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			Internal::ISTLObjectKeyMarshallerBase** keyMarshallerArrayTemp = new Internal::ISTLObjectKeyMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(keyMarshallerArrayTemp, 0, (ContainerType*)0);
			keyMarshallerArray = keyMarshallerArrayTemp;
		}

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		if(_sourceDataMovable && allowMove)
		{
			Internal::DecomposeSTLContainer(itemArray, elementSizeArrayTemp, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, std::move(const_cast<ContainerType&>(_sourceObject)));
		}
		else
#endif
		{
			Internal::DecomposeSTLContainer(itemArray, elementSizeArrayTemp, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, _sourceObject);
		}
	}
	virtual void FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const* keyMarshallerArray) const
	{
		//Release the allocated data arrays
		Internal::DeleteSTLContainerItemArray(itemArray, &_sourceObject);
		delete[] elementSizeArray;
		if(Internal::nested_container_has_keys<ContainerType>::value)
		{
			Internal::DeleteSTLContainerKeyMarshallers(const_cast<Internal::ISTLObjectKeyMarshallerBase**>(keyMarshallerArray), 0, (ContainerType*)0);
			delete[] keyMarshallerArray;
		}
	}

private:
	//Disable copying and moving
	MarshalSourceInternal(const MarshalSourceInternal& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSourceInternal& operator=(const MarshalSourceInternal& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSourceInternal(MarshalSourceInternal&& source) = delete;
	MarshalSourceInternal& operator=(MarshalSourceInternal&& source) = delete;
#endif

private:
	const ContainerType& _sourceObject;
	bool _sourceDataMovable;
};
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class MarshalSource
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalSource<ContainerType, false, false> :public Internal::MarshalSourceInternal<ContainerType, false>
{
public:
	//Constructors
	MarshalSource(const ContainerType& asourceObject)
	:Internal::MarshalSourceInternal<ContainerType, false>(asourceObject)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(ContainerType&& asourceObject)
	:Internal::MarshalSourceInternal<ContainerType, false>(std::move(asourceObject))
	{ }
#endif

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif
};

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalSource<ContainerType, false, true> :public Internal::MarshalSourceInternal<ContainerType, true>
{
public:
	//Constructors
	MarshalSource(const ContainerType& asourceObject)
	:Internal::MarshalSourceInternal<ContainerType, true>(asourceObject)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(ContainerType&& asourceObject)
	:Internal::MarshalSourceInternal<ContainerType, true>(std::move(asourceObject))
	{ }
#endif

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif
};

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalSource<ContainerType, true, true> :public IMarshalSource<ContainerType, true, true>
{
public:
	//Constructors
	MarshalSource(const ContainerType& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(ContainerType&& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(true)
	{ }
#endif

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ContainerType*& sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type);
		sourceData = &_sourceObject;
		return _sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif

private:
	const ContainerType& _sourceObject;
	bool _sourceDataMovable;
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalSource<std::vector<ElementType, Alloc>, false, true> :public IMarshalSource<std::vector<ElementType, Alloc>, false, true>
{
public:
	//Constructors
	MarshalSource(const std::vector<ElementType, Alloc>& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(false)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(std::vector<ElementType, Alloc>&& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(true)
	{ }
#endif

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		sourceData = _sourceObject.data();
#else
		sourceData = _sourceObject.empty() ? 0 : &_sourceObject[0];
#endif
		sourceDataLength = _sourceObject.size();
		return _sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif

private:
	const std::vector<ElementType, Alloc>& _sourceObject;
	bool _sourceDataMovable;
};

//----------------------------------------------------------------------------------------
template<class Alloc>
class MarshalSource<std::vector<bool, Alloc>, false, true> :public IMarshalSource<std::vector<bool, Alloc>, false, true>
{
public:
	//Constructors
	MarshalSource(const std::vector<bool, Alloc>& sourceObject)
	:_sourceObject(sourceObject)
	{ }

protected:
	//Marshalling methods
	virtual void RetrieveData(const bool*& sourceData, size_t& sourceDataLength) const
	{
		//Since the C++ standard defines a rather poor specialization of std::vector for
		//the bool type which bitpacks values, and therefore can't provide a reference to
		//its underlying data members like the vector container normally can, we need to
		//repack the elements of this boolean vector here into a flat array so that we can
		//marshal it.
		size_t sourceObjectLength = _sourceObject.size();
		bool* itemArray = new bool[sourceObjectLength];
		for(size_t i = 0; i < sourceObjectLength; ++i)
		{
			itemArray[i] = _sourceObject[i];
		}

		//Return the raw vector data to the caller
		sourceData = itemArray;
		sourceDataLength = sourceObjectLength;
	}
	virtual void FreeDataArrays(const bool* sourceData) const
	{
		delete[] sourceData;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif

private:
	const std::vector<bool, Alloc>& _sourceObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalSource<std::array<ElementType, ArraySize>, false, true> :public IMarshalSource<std::array<ElementType, ArraySize>, false, true>
{
public:
	//Constructors
	MarshalSource(const std::array<ElementType, ArraySize>& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(false)
	{ }
	MarshalSource(std::array<ElementType, ArraySize>&& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(true)
	{ }

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		sourceData = _sourceObject.data();
		return _sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) = delete;
	MarshalSource& operator=(const MarshalSource& source) = delete;
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;

private:
	const std::array<ElementType, ArraySize>& _sourceObject;
	bool _sourceDataMovable;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true> :public IMarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true>
{
public:
	//Constructors
	MarshalSource(const std::basic_string<ElementType, traits, Alloc>& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(false)
	{ }
	MarshalSource(const ElementType* sourceObject)
	:_tempString(sourceObject), _sourceObject(_tempString), _sourceDataMovable(true)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(std::basic_string<ElementType, traits, Alloc>&& sourceObject)
	:_sourceObject(sourceObject), _sourceDataMovable(true)
	{ }
#endif

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const
	{
		elementByteSize = sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type);
		sourceData = _sourceObject.data();
		sourceDataLength = _sourceObject.size();
		return _sourceDataMovable;
	}

private:
	//Disable copying and moving
	MarshalSource(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalSource& operator=(const MarshalSource& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalSource(MarshalSource&& source) = delete;
	MarshalSource& operator=(MarshalSource&& source) = delete;
#endif

private:
	std::basic_string<ElementType, traits, Alloc> _tempString;
	const std::basic_string<ElementType, traits, Alloc>& _sourceObject;
	bool _sourceDataMovable;
};

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace MarshalSupport
#endif
