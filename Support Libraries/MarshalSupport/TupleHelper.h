#ifndef __TUPLEHELPER_H__
#define __TUPLEHELPER_H__
#include "INestedMarshallerBase.h"
#include <tuple>
namespace MarshalSupport {
namespace Internal {

template<size_t TupleElementNo, class... Args>
class TupleHelper
{
public:
	static inline void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void EnsureSTLContainerKeySizesMatch(INestedMarshallerBase* const nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element);
	static inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element);
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element);
};

template<class... Args>
class TupleHelper<0, Args...>
{
public:
	static inline void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void EnsureSTLContainerKeySizesMatch(INestedMarshallerBase* const nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
	static inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element);
	static inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element);
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element);
};

} // Close namespace Internal
} // Close namespace MarshalSupport
#endif
