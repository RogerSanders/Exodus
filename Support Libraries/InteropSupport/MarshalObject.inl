#include "IMarshallingObject.h"
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<bool DerivedFromIMarshallingObject>
inline bool MarshalObject(const void* sourceObject, void* targetObject)
{
	return DerivedFromIMarshallingObject;
}

//----------------------------------------------------------------------------------------
template<>
inline bool MarshalObject<true>(const void* sourceObject, void* targetObject)
{
	const IMarshallingObject& sourceObjectAsIMarshallingObject = *static_cast<const IMarshallingObject*>(sourceObject);
	IMarshallingObject& targetObjectAsIMarshallingObject = *static_cast<IMarshallingObject*>(targetObject);
	targetObjectAsIMarshallingObject.MarshalFromSource(sourceObjectAsIMarshallingObject);
	return true;
}

} //Close namespace InteropSupport
