#ifndef __CREATESTLCONTAINERITEMARRAY_H__
#define __CREATESTLCONTAINERITEMARRAY_H__
#include "MarshalPreprocessorMacros.h"
namespace MarshalSupport {
namespace Internal {

template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArrayEntryCount, const ElementType* elementPointer);

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "CreateSTLContainerItemArray.inl"
#endif
