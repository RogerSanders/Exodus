#ifndef __CUSTOMMARSHALOBJECT_H__
#define __CUSTOMMARSHALOBJECT_H__
#include <string>
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif
#include <iostream>
#include <sstream>

class CustomMarshalObject : MarshalSupport::IMarshallingObject
{
public:
	//Constructors
	CustomMarshalObject(int someData)
	{
		data1 = someData;
		data2 = data1 + someData;
		data3 = data2 + someData;
		std::wstringstream stream;
		stream << data1 + data2 + data3;
		data4 = stream.str();
	}
	virtual ~CustomMarshalObject()
	{ }
	CustomMarshalObject(const CustomMarshalObject& source)
	:data1(source.data1), data2(source.data2), data3(source.data3), data4(source.data4)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObject(CustomMarshalObject&& source)
	:data1(source.data1), data2(source.data2), data3(source.data3), data4(std::move(source.data4))
	{ }
#endif
	CustomMarshalObject(MarshalSupport::marshal_object_tag, const CustomMarshalObject& source)
	{
		source.MarshalObjectData(data1, data2, data3, data4);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObject(MarshalSupport::marshal_object_tag, CustomMarshalObject&& source)
	{
		source.MarshalObjectData(data1, data2, data3, data4);
	}
#endif

	CustomMarshalObject& operator=(const CustomMarshalObject& source)
	{
		data1 = source.data1;
		data2 = source.data2;
		data3 = source.data3;
		data4 = source.data4;
		return *this;
	}

	bool operator<(const CustomMarshalObject& target) const
	{
		if(*this == target)
		{
			return false;
		}
		else if(data1 != target.data1)
		{
			return (data1 < target.data1);
		}
		else if(data2 != target.data2)
		{
			return (data2 < target.data2);
		}
		else if(data3 != target.data3)
		{
			return (data3 < target.data3);
		}
		return (data4 < target.data4);
	}
	bool operator==(const CustomMarshalObject& target) const
	{
		return (data1 == target.data1) && (data2 == target.data2) && (data3 == target.data3) && (data4 == target.data4);
	}

private:
	//Marshalling methods
	virtual void MarshalObjectData(unsigned int& data1Target, unsigned int& data2Target, unsigned int& data3Target, const Marshal::Out<std::wstring>& data4Target) const
	{
		data1Target = data1;
		data2Target = data2;
		data3Target = data3;
		data4Target = data4;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void MarshalObjectData(unsigned int& data1Target, unsigned int& data2Target, unsigned int& data3Target, const Marshal::Out<std::wstring>& data4Target)
	{
		data1Target = data1;
		data2Target = data2;
		data3Target = data3;
		data4Target = std::move(data4);
	}
#endif

public:
	unsigned int data1;
	unsigned int data2;
	unsigned int data3;
	std::wstring data4;
};

class CustomMarshalObjectWithDefaultConstructor :public CustomMarshalObject
{
public:
	//Constructors
	CustomMarshalObjectWithDefaultConstructor(int someData = 42)
		:CustomMarshalObject(someData)
	{ }
	CustomMarshalObjectWithDefaultConstructor(const CustomMarshalObjectWithDefaultConstructor& source)
		:CustomMarshalObject(source)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObjectWithDefaultConstructor(CustomMarshalObjectWithDefaultConstructor&& source)
		:CustomMarshalObject(std::move(source))
	{ }
#endif
	CustomMarshalObjectWithDefaultConstructor(MarshalSupport::marshal_object_tag, const CustomMarshalObjectWithDefaultConstructor& source)
		:CustomMarshalObject(MarshalSupport::marshal_object_tag(), source)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObjectWithDefaultConstructor(MarshalSupport::marshal_object_tag, CustomMarshalObjectWithDefaultConstructor&& source)
		:CustomMarshalObject(MarshalSupport::marshal_object_tag(), std::move(source))
	{ }
#endif

	CustomMarshalObjectWithDefaultConstructor& operator=(const CustomMarshalObjectWithDefaultConstructor& source)
	{
		CustomMarshalObject::operator=(source);
		return *this;
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
namespace std {
template <>
struct hash<CustomMarshalObject>
{
	std::size_t operator()(const CustomMarshalObject& k) const
	{
		return hash<unsigned int>()(k.data1) ^ hash<unsigned int>()(k.data2) ^ hash<unsigned int>()(k.data3) ^ hash<std::wstring>()(k.data4);
	}
};
}
#endif

#endif
