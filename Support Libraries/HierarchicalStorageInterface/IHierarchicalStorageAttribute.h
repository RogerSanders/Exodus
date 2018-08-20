#ifndef __IHIERARCHICALSTORAGEATTRIBUTE_H__
#define __IHIERARCHICALSTORAGEATTRIBUTE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
using namespace MarshalSupport::Operators;

class IHierarchicalStorageAttribute
{
public:
	// Constructors
	virtual ~IHierarchicalStorageAttribute() = 0 { }

	// Name functions
	virtual Marshal::Ret<std::wstring> GetName() const = 0;
	virtual void SetName(const Marshal::In<std::wstring>& name) = 0;

	// Value read functions
	//##FIX## Why do we reset the stream position when extracting data, but not when
	// inserting data?
	inline std::wstring GetValue() const;
	template<class T> T ExtractValue();
	template<class T> T ExtractHexValue();
	template<class T> void ExtractValue(T& target);
	template<class T> void ExtractHexValue(T& target);

	// Value write functions
	template<class T> void SetValue(const T& data);
	template<class T> void InsertValue(const T& data);
	template<class T> void InsertHexValue(const T& data, unsigned int length);

protected:
	// Stream functions
	virtual void ResetInternalStreamPosition() const = 0;
	virtual void EmptyInternalStream() = 0;
	virtual Stream::IStream& GetInternalStream() const = 0;
};

#include "IHierarchicalStorageAttribute.inl"
#endif
