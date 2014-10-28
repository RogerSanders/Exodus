#ifndef __DELETESTLCONTAINERITEMARRAY_H__
#define __DELETESTLCONTAINERITEMARRAY_H__
#include "MarshalPreprocessorMacros.h"
namespace MarshalSupport {
namespace Internal {

template<class ElementType>
inline void DeleteSTLContainerItemArray(void* itemArray, const ElementType* elementPointer);

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "DeleteSTLContainerItemArray.inl"
#endif
