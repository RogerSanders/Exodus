#ifndef __ISTLOBJECTKEYMARSHALLERBASE_H__
#define __ISTLOBJECTKEYMARSHALLERBASE_H__
namespace MarshalSupport {
namespace Internal {

class ISTLObjectKeyMarshallerBase
{
public:
	//Constructors
	inline virtual ~ISTLObjectKeyMarshallerBase() = 0;
};
ISTLObjectKeyMarshallerBase::~ISTLObjectKeyMarshallerBase() { }

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
