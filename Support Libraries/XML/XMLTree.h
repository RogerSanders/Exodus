#ifndef __XMLTREE_H__
#define __XMLTREE_H__
#define XML_STATIC
#define XML_UNICODE_WCHAR_T
#include <expat.h>
#include <string>
#include "XMLEntity.h"
#include "StreamInterface/StreamInterface.pkg"

//##TODO## Phase out this library, and implement the HeirarchicalStorageInterface library
//everywhere.
class XMLTree
{
public:
	//Constructors
	XMLTree();
	void Initialize();

	//Save/Load functions
	bool SaveTree(Stream::IStream& target);
	bool LoadTree(Stream::IStream& source);

	//Binary data functions
	bool GetSeparateBinaryDataEnabled() const;
	void SetSeparateBinaryDataEnabled(bool state);
	std::list<XMLEntity*> GetBinaryDataEntityList();

	//Error handling functions
	std::wstring GetErrorString() const;

private:
	//Save/Load functions
	bool SaveEntity(XMLEntity& entity, Stream::IStream& stream, const std::wstring& indentPrefix) const;
	static void XMLCALL LoadStartElement(void *userData, const XML_Char *aname, const XML_Char **aatts);
	static void XMLCALL LoadEndElement(void *userData, const XML_Char *aname);
	static void XMLCALL LoadData(void *userData, const XML_Char *s, int len);

	//Reserved character substitution functions
	bool IsCharacterReserved(wchar_t character) const;
	std::wstring GetNumericCharacterReference(wchar_t character) const;

public:
	XMLEntity root;

private:
	XMLEntity* currentElement;
	mutable std::wstring errorString;
	bool allowSeparateBinaryData;
};

#endif
