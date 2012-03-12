#ifndef __XMLENTITY_H__
#define __XMLENTITY_H__
#include <string>
#include <list>
#include "Stream/Stream.pkg"
#include "XMLAttribute.h"
class XMLTree;

class XMLEntity
{
public:
	//Constructors
	inline XMLEntity();
	explicit inline XMLEntity(const std::wstring& aname);
	template<class T> inline XMLEntity(const std::wstring& aname, const T& adata);
	template<class T> inline XMLEntity(const std::wstring& aname, const T& adata, unsigned int length);
	void Initialize();

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Parent and child functions
	inline XMLEntity* GetParent() const;
	inline void AddChild(const XMLEntity& child);
	void AddBinaryDataEntitiesToList(std::list<XMLEntity*>& binaryEntityList);

	//Attribute functions
	XMLAttribute* GetAttribute(const std::wstring& name);
	void AddAttribute(const XMLAttribute& attribute);
	template<class T> XMLEntity& CreateAttribute(const std::wstring& name, const T& value);
	template<class T> XMLEntity& CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length);
	template<class T> bool ExtractAttribute(const std::wstring& name, T& target);
	template<class T> bool ExtractAttributeHex(const std::wstring& name, T& target);

	//Data read functions
	inline std::wstring GetData() const;
	template<class T> T ExtractData();
	template<class T> T ExtractHexData();
	template<class T> void ExtractData(T& target);
	template<class T> void ExtractHexData(T& target);

	//Data write functions
	template<class T> void SetData(const T& adata);
	template<class T> void InsertData(const T& adata);
	template<class T> void InsertHexData(const T& adata, unsigned int length);

	//Binary data functions
	inline std::wstring GetBinaryDataBufferName() const;
	inline Stream::Buffer& GetBinaryDataBufferStream();
	inline bool GetInlineBinaryDataEnabled() const;
	inline void SetInlineBinaryDataEnabled(bool state);

	//Binary data read functions
	template<class T> T ExtractBinaryData();
	template<class T> void ExtractBinaryData(T& target);
	template<class T> void ExtractBinaryData(std::vector<T>& target);

	//Binary data write functions
	template<class T> void InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> void InsertBinaryData(const std::vector<T>& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<> void InsertBinaryData(const std::vector<unsigned char>& adata, const std::wstring& bufferName, bool ainlineBinaryData);
	template<class T> void InsertBinaryData(T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData = true);

private:
	//Parent and child functions
	inline void SetParent(XMLEntity* aparent);

public:
	typedef std::list<XMLEntity> EntityList;
	EntityList children;

private:
	friend class XMLTree;
	typedef std::pair<std::wstring, XMLAttribute> AttributeListEntry;
	typedef std::list<AttributeListEntry> AttributeList;

	std::wstring name;
	XMLEntity* parent;

	AttributeList attributes;
	bool binaryDataPresent;
	bool inlineBinaryData;
	std::wstring binaryDataName;
	mutable Stream::Buffer dataStream;
};

#include "XMLEntity.inl"
#endif
