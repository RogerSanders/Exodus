#ifndef __XMLATTRIBUTE_H__
#define __XMLATTRIBUTE_H__
#include "Stream/Stream.pkg"
#include <string>

class XMLAttribute
{
public:
	//Constructors
	inline XMLAttribute();
	explicit inline XMLAttribute(const std::wstring& aname);
	template<class T> inline XMLAttribute(const std::wstring& aname, const T& adata);
	template<class T> inline XMLAttribute(const std::wstring& aname, const T& adata, unsigned int length);

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Value read functions
	inline std::wstring GetValue() const;
	template<class T> T ExtractValue();
	template<class T> T ExtractHexValue();
	template<class T> void ExtractValue(T& target);
	template<class T> void ExtractHexValue(T& target);

	//Value write functions
	template<class T> void SetValue(const T& adata);
	template<class T> void InsertValue(const T& adata);
	template<class T> void InsertHexValue(const T& adata, unsigned int length);

private:
	std::wstring name;
	mutable Stream::Buffer value;
};

#include "XMLAttribute.inl"
#endif
