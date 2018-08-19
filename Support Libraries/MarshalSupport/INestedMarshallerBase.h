#ifndef __INESTEDMARSHALLERBASE_H__
#define __INESTEDMARSHALLERBASE_H__
namespace MarshalSupport {
namespace Internal {

class INestedMarshallerBase
{
public:
	// Constructors
	inline virtual ~INestedMarshallerBase() = 0;
};
INestedMarshallerBase::~INestedMarshallerBase() { }

} // Close namespace Internal
} // Close namespace MarshalSupport
#endif
