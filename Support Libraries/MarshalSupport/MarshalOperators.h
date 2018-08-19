#ifndef __MARSHALSUPPORT_MARSHALOPERATORS_H__
#define __MARSHALSUPPORT_MARSHALOPERATORS_H__
#include "MarshalPreprocessorMacros.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include "MarshalIn.h"
#include "MarshalOut.h"
#include "MarshalInOut.h"
#include "MarshalRet.h"
namespace MarshalSupport {
namespace Operators {
namespace Marshal {

// Define alias templates for our marshal helpers. We do this to provide a "clean" namespace where we guarantee that only
// these symbols are present, and nothing else will be imported through a "using namespace" operation other than these
// types. The intended usage is for code to include a line within header files of the following form:
// using namespace MarshalSupport::Operators;
// The marshalling operators can then be referenced on method declarations in the form "Marshal::In", "Marshal::Out", and
// so forth.

template<class ContainerType, bool IsOnlyMovable = MarshalSupport::Internal::is_only_movable<typename MarshalSupport::Internal::get_last_nested_container_element_type<ContainerType>::type>::value, bool IsAssignable = MarshalSupport::Internal::is_assignable<ContainerType>::value, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
using In = MarshalSupport::Marshal::In<ContainerType, IsOnlyMovable, IsAssignable, IsLastElement, IsThisOrNextElementLastElement>;

template<class ContainerType, bool IsOnlyMovable = MarshalSupport::Internal::is_only_movable<typename MarshalSupport::Internal::get_last_nested_container_element_type<ContainerType>::type>::value, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
using Out = MarshalSupport::Marshal::Out<ContainerType, IsOnlyMovable, IsLastElement, IsThisOrNextElementLastElement>;

template<class ContainerType, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
using InOut = MarshalSupport::Marshal::InOut<ContainerType, IsLastElement, IsThisOrNextElementLastElement>;

template<class ContainerType, bool IsOnlyMovable = MarshalSupport::Internal::is_only_movable<typename MarshalSupport::Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
using Ret = MarshalSupport::Marshal::Ret<ContainerType, IsOnlyMovable>;

} // Close namespace Marshal
} // Close namespace Operators
} // Close namespace MarshalSupport
#endif
#endif
