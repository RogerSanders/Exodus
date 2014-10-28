#ifndef __CUSTOMMARSHALOBJECT_H__
#define __CUSTOMMARSHALOBJECT_H__
#include <string>
#include "MarshalSupport/MarshalSupport.pkg"
using namespace MarshalSupport;
#include <iostream>
#include <sstream>

class CustomMarshalObject
{
public:
	//Constructors
	CustomMarshalObject(int someData)
	{
		//std::wcout << "CustomMarshalObject(int someData)\n";
		sourceAddress = someData;
		targetAddress = sourceAddress + someData;
		returnAddress = targetAddress + someData;
		std::wstringstream stream;
		stream << sourceAddress + targetAddress + returnAddress;
		disassembly = stream.str();
	}
	virtual ~CustomMarshalObject()
	{
		//std::wcout << "virtual ~CustomMarshalObject()\n";
	}
	CustomMarshalObject(const CustomMarshalObject& source)
		:sourceAddress(source.sourceAddress), targetAddress(source.targetAddress), returnAddress(source.returnAddress), disassembly(source.disassembly)
	{
		//std::wcout << "CustomMarshalObject(const CustomMarshalObject& source)\n";
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObject(CustomMarshalObject&& source)
		:sourceAddress(std::move(source.sourceAddress)), targetAddress(std::move(source.targetAddress)), returnAddress(std::move(source.returnAddress)), disassembly(std::move(source.disassembly))
	{
		//std::wcout << "CustomMarshalObject(CustomMarshalObject&& source)\n";
	}
#endif
	CustomMarshalObject(marshal_object_t, const CustomMarshalObject& source)
	{
		//std::wcout << "CustomMarshalObject(IMarshallingObject*, const CustomMarshalObject& source)\n";
		source.MarshalSet_sourceAddress(sourceAddress);
		source.MarshalSet_targetAddress(targetAddress);
		source.MarshalSet_returnAddress(returnAddress);
		source.MarshalSet_disassembly(disassembly);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObject(marshal_object_t, CustomMarshalObject&& source)
	{
		//std::wcout << "CustomMarshalObject(IMarshallingObject*, CustomMarshalObject&& source)\n";
		source.MarshalSet_sourceAddress(sourceAddress);
		source.MarshalSet_targetAddress(targetAddress);
		source.MarshalSet_returnAddress(returnAddress);
		source.MarshalSet_disassembly(disassembly);
	}
#endif

	bool operator<(const CustomMarshalObject& target) const
	{
		if(*this == target)
		{
			return false;
		}
		else if(sourceAddress != target.sourceAddress)
		{
			return (sourceAddress < target.sourceAddress);
		}
		else if(targetAddress != target.targetAddress)
		{
			return (targetAddress < target.targetAddress);
		}
		else if(returnAddress != target.returnAddress)
		{
			return (returnAddress < target.returnAddress);
		}
		return (disassembly < target.disassembly);
	}
	bool operator==(const CustomMarshalObject& target) const
	{
		return (sourceAddress == target.sourceAddress) && (targetAddress == target.targetAddress) && (returnAddress == target.returnAddress) && (disassembly == target.disassembly);
	}

private:
	//Marshalling methods
	virtual void MarshalSet_sourceAddress(unsigned int& marshaller) const { marshaller = sourceAddress; }
	virtual void MarshalSet_targetAddress(unsigned int& marshaller) const { marshaller = targetAddress; }
	virtual void MarshalSet_returnAddress(unsigned int& marshaller) const { marshaller = returnAddress; }
	virtual void MarshalSet_disassembly(const Marshal::Out<std::wstring>& marshaller) const { marshaller = disassembly; }

public:
	unsigned int sourceAddress;
	unsigned int targetAddress;
	unsigned int returnAddress;
	std::wstring disassembly;
};

class CustomMarshalObjectWithDefaultConstructor :public CustomMarshalObject
{
public:
	//Constructors
	CustomMarshalObjectWithDefaultConstructor(int someData = 42)
		:CustomMarshalObject(someData)
	{
		//std::wcout << "CustomMarshalObjectWithDefaultConstructor()\n";
	}
	CustomMarshalObjectWithDefaultConstructor(const CustomMarshalObjectWithDefaultConstructor& source)
		:CustomMarshalObject(source)
	{
		//std::wcout << "CustomMarshalObjectWithDefaultConstructor(const CustomMarshalObjectWithDefaultConstructor& source)\n";
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObjectWithDefaultConstructor(CustomMarshalObjectWithDefaultConstructor&& source)
		:CustomMarshalObject(std::move(source))
	{
		//std::wcout << "CustomMarshalObjectWithDefaultConstructor(CustomMarshalObjectWithDefaultConstructor&& source)\n";
	}
#endif
	CustomMarshalObjectWithDefaultConstructor(marshal_object_t, const CustomMarshalObjectWithDefaultConstructor& source)
		:CustomMarshalObject(marshal_object_t(), source)
	{
		//std::wcout << "CustomMarshalObjectWithDefaultConstructor(IMarshallingObject*, const CustomMarshalObjectWithDefaultConstructor& source)\n";
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	CustomMarshalObjectWithDefaultConstructor(marshal_object_t, CustomMarshalObjectWithDefaultConstructor&& source)
		:CustomMarshalObject(marshal_object_t(), std::move(source))
	{
		//std::wcout << "CustomMarshalObjectWithDefaultConstructor(IMarshallingObject*, CustomMarshalObjectWithDefaultConstructor&& source)\n";
	}
#endif
};

#endif
