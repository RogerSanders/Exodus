#ifndef __IHEIRARCHICALSTORAGEATTRIBUTE_H__
#define __IHEIRARCHICALSTORAGEATTRIBUTE_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>

class IHeirarchicalStorageAttribute
{
public:
	//Constructors
	virtual ~IHeirarchicalStorageAttribute() = 0 {}

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Value read functions
	//##FIX## Why do we reset the stream position when extracting data, but not when
	//inserting data?
	inline std::wstring GetValue() const;
	template<class T> T ExtractValue();
	template<class T> T ExtractHexValue();
	template<class T> void ExtractValue(T& target);
	template<class T> void ExtractHexValue(T& target);

	//Value write functions
	template<class T> void SetValue(const T& adata);
	template<class T> void InsertValue(const T& adata);
	template<class T> void InsertHexValue(const T& adata, unsigned int length);

protected:
	//Name functions
	virtual const wchar_t* GetNameInternal() const = 0;
	virtual void SetNameInternal(const wchar_t* aname) = 0;

	//Stream functions
	virtual void ResetInternalStreamPosition() const = 0;
	virtual void EmptyInternalStream() = 0;
	virtual Stream::IStream& GetInternalStream() const = 0;
};

#include "IHeirarchicalStorageAttribute.inl"
#endif
