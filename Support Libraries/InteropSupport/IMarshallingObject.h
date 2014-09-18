#ifndef __IMARSHALLINGOBJECT_H__
#define __IMARSHALLINGOBJECT_H__
namespace InteropSupport {

class IMarshallingObject
{
public:
	//Constructors
	virtual ~IMarshallingObject() = 0 {}

public:
	//Marshalling methods
	virtual void MarshalFromSource(const IMarshallingObject& source) = 0;
};

} //Close namespace InteropSupport
#endif
