#include "GetLastNestedContainerElementType.h"
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArrayEntryCount, const ElementType* elementPointer)
{
	//Note that we allocate a raw byte array here of the required size, rather than
	//allocating an array of ElementType objects, in order to avoid the need for
	//ElementType to provide a default constructor. We use placement new during the
	//marshalling process to invoke our marshalling constructor for the copied objects,
	//which allows ElementType to contain reference types or other values which cannot be
	//initialized in a default constructor. Note that the C++ standard explicitly
	//guarantees that an array of char or unsigned char will have an alignment compatible
	//with any object type that could fit in the allocated memory block. See section 5.3.4
	//paragraph 10 of the C++ standard. Since C++ also guarantees that array elements have
	//to be contiguous with zero padding between elements, it is safe and well defined to
	//allocate our memory in this manner.
	return (void*)new unsigned char[itemArrayEntryCount * sizeof(typename get_last_nested_container_element_type<ElementType>::type)];
}

} //Close namespace Internal
} //Close namespace MarshalSupport
