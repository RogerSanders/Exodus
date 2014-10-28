#ifndef __IMARSHALLINGOBJECT_H__
#define __IMARSHALLINGOBJECT_H__
namespace MarshalSupport {

class IMarshallingObject
{
public:
	//Constructors
	inline virtual ~IMarshallingObject() = 0;
};
IMarshallingObject::~IMarshallingObject() { }

} //Close namespace MarshalSupport
#endif
