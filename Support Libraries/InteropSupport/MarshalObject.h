#ifndef __MARSHALOBJECT_H__
#define __MARSHALOBJECT_H__
namespace InteropSupport {

template<bool DerivedFromIMarshallingObject>
inline bool MarshalObject(const void* sourceObject, void* targetObject);
template<>
inline bool MarshalObject<true>(const void* sourceObject, void* targetObject);

} //Close namespace InteropSupport
#include "MarshalObject.inl"
#endif
