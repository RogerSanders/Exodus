#include "XMLTree.h"
#include <vector>
#include <sstream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
XMLTree::XMLTree()
:allowSeparateBinaryData(true)
{}

//----------------------------------------------------------------------------------------
void XMLTree::Initialize()
{
	root.Initialize();
}

//----------------------------------------------------------------------------------------
//Save/Load functions
//----------------------------------------------------------------------------------------
bool XMLTree::SaveTree(Stream::IStream& target)
{
	return SaveEntity(root, target, L"");
}

//----------------------------------------------------------------------------------------
bool XMLTree::LoadTree(Stream::IStream& source)
{
	//Load the contents of the source stream into a buffer of unicode characters
	std::wstring buffer;
	Stream::ViewText view(source);
	if(!view.ReadTextString(buffer, false))
	{
		return false;
	}
	if(buffer.empty())
	{
		return false;
	}

	//Create a new expat XML parser
	XML_Parser parser = XML_ParserCreate(NULL);
	if(parser == 0)
	{
		return false;
	}

	//Set our callback handlers for the parser
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, LoadStartElement, LoadEndElement);
	XML_SetCharacterDataHandler(parser, LoadData);

	//Parse the XML data
	currentElement = 0;
	if(XML_Parse(parser, (char*)&buffer[0], (int)(buffer.size() * sizeof(wchar_t)), 1) != XML_STATUS_OK)
	{
		//If XML parsing failed, set the error string, and return false.
		std::wstringstream errorStream;
		errorStream << XML_ErrorString(XML_GetErrorCode(parser)) << L" at line " << XML_GetCurrentLineNumber(parser);
		errorString = errorStream.str();
		XML_ParserFree(parser);
		return false;
	}

	//Free the XML parser
	XML_ParserFree(parser);

	return true;
}

//----------------------------------------------------------------------------------------
bool XMLTree::SaveEntity(XMLEntity& entity, Stream::IStream& stream, const std::wstring& indentPrefix) const
{
	Stream::ViewText streamView(stream);

	//Open key
	streamView << indentPrefix << L'<' << entity.GetName();

	//Write attributes
	for(XMLEntity::AttributeList::const_iterator i = entity.attributes.begin(); i != entity.attributes.end(); ++i)
	{
		std::wstring name = i->second.GetName();
		std::wstring value = i->second.GetValue();
		std::wstring finalValue;
		for(unsigned int i = 0; i < value.length(); ++i)
		{
			if(IsCharacterReserved(value[i]))
			{
				finalValue += GetNumericCharacterReference(value[i]);
			}
			else
			{
				finalValue.push_back(value[i]);
			}
		}
		streamView << L' ' << name << L"=\"" << finalValue << L"\"";
	}
	if(entity.binaryDataPresent)
	{
		streamView << L" BinaryDataPresent=\"1\"";
		if(GetSeparateBinaryDataEnabled() && !entity.inlineBinaryData)
		{
			streamView << L" SeparateBinaryData=\"1\"";
		}
	}

	if(entity.children.empty() && !entity.binaryDataPresent && entity.GetData().empty())
	{
		//If this entity contains no children and no data, shortcut the rest of the save
		//process and use an empty element tag.
		streamView << L" />";
	}
	else
	{
		//If we need to output a full open/close key pair, close the opening value, and
		//continue with the save process.
		streamView << L'>';

		//Write data
		if(!entity.binaryDataPresent)
		{
			std::wstring data = entity.GetData();
			std::wstring finalData;
			for(unsigned int i = 0; i < data.length(); ++i)
			{
				if(IsCharacterReserved(data[i]))
				{
					finalData += GetNumericCharacterReference(data[i]);
				}
				else
				{
					finalData.push_back(data[i]);
				}
			}
			streamView << finalData;
		}
		else
		{
			//Check whether the binary data should be saved in a separate file, or saved
			//within the XML tree in inline form.
			if(GetSeparateBinaryDataEnabled() && !entity.inlineBinaryData)
			{
				//Output the name of the separate binary storage buffer
				streamView << entity.binaryDataName;
			}
			else
			{
				//Save binary data in the XML structure
				std::vector<unsigned char> buffer;
				entity.ExtractBinaryData(buffer);
				for(unsigned int i = 0; i < buffer.size(); ++i)
				{
					streamView << Stream::Hex(2) << buffer[i];
				}
			}
		}

		//Write child elements
		if(!entity.children.empty())
		{
			streamView << L"\n";
			for(XMLEntity::EntityList::iterator i = entity.children.begin(); i != entity.children.end(); ++i)
			{
				SaveEntity(*i, stream, indentPrefix + L'\t');
			}
			streamView << indentPrefix;
		}

		//Close key
		streamView << L"</" << entity.GetName() << L'>';
	}

	//Move to a new line in preparation for the next entity
	streamView << L"\n";
	return true;
}

//----------------------------------------------------------------------------------------
void XMLCALL XMLTree::LoadStartElement(void *userData, const XML_Char *aname, const XML_Char **aatts)
{
	XMLTree* tree = (XMLTree*)userData;
	XMLEntity* element = 0;
	if(tree->currentElement == 0)
	{
		element = &tree->root;
		tree->currentElement = &tree->root;
	}
	else
	{
		tree->currentElement->AddChild(XMLEntity());
		element = &tree->currentElement->children.back();
		tree->currentElement = element;
	}

	element->SetName(aname);
	while(*aatts != 0)
	{
		std::wstring name;
		std::wstring value;
		name = (*(aatts++));
		if(*aatts != 0)
		{
			value = (*(aatts++));
			element->AddAttribute(XMLAttribute(name, value));
		}
	}
}

//----------------------------------------------------------------------------------------
void XMLCALL XMLTree::LoadEndElement(void *userData, const XML_Char *aname)
{
	XMLTree* tree = (XMLTree*)userData;
	if(tree->currentElement != 0)
	{
		tree->currentElement = tree->currentElement->GetParent();
	}
}

//----------------------------------------------------------------------------------------
void XMLCALL XMLTree::LoadData(void *userData, const XML_Char *s, int len)
{
	XMLTree* tree = (XMLTree*)userData;

	//Append all printable characters to the data stream
	std::wstring data;
	if(!tree->currentElement->binaryDataPresent)
	{
		data = tree->currentElement->GetData();
	}
	for(int i = 0; i < len; ++i)
	{
		//Exclude all characters that are not printable, and exclude all whitespace
		//characters with the exception of space.
		if((iswprint(*(s + i)) != 0) && ((iswspace(*(s + i)) == 0) || (*(s + i) == L' ')))
		{
			data.push_back(*(s + i));
		}
	}
	if(tree->currentElement->GetAttribute(L"BinaryDataPresent") != 0)
	{
		tree->currentElement->binaryDataPresent = true;
		if(tree->currentElement->GetAttribute(L"SeparateBinaryData") != 0)
		{
			tree->currentElement->inlineBinaryData = false;
			//Load the name of the separate binary storage buffer
			tree->currentElement->binaryDataName += data;
		}
		else
		{
			tree->currentElement->inlineBinaryData = true;
			//Load inline binary data from the XML structure
			size_t charPos = 0;
			while((data.length() - charPos) >= 2)
			{
				std::wstringstream dataStream;
				dataStream << data[charPos + 0] << data[charPos + 1];
				unsigned int byte;
				dataStream >> std::hex >> byte;

				Stream::ViewBinary bufferView(tree->currentElement->dataStream);
				bufferView << (unsigned char)byte;

				charPos	+= 2;
			}
		}
	}
	else
	{
		tree->currentElement->SetData(data);
	}
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
bool XMLTree::GetSeparateBinaryDataEnabled() const
{
	return allowSeparateBinaryData;
}

//----------------------------------------------------------------------------------------
void XMLTree::SetSeparateBinaryDataEnabled(bool state)
{
	allowSeparateBinaryData = state;
}

//----------------------------------------------------------------------------------------
std::list<XMLEntity*> XMLTree::GetBinaryDataEntityList()
{
	std::list<XMLEntity*> binaryEntityList;
	root.AddBinaryDataEntitiesToList(binaryEntityList);
	return binaryEntityList;
}

//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
std::wstring XMLTree::GetErrorString() const
{
	return errorString;
}

//----------------------------------------------------------------------------------------
//Reserved character substitution functions
//----------------------------------------------------------------------------------------
bool XMLTree::IsCharacterReserved(wchar_t character) const
{
	switch(character)
	{
	case L'\t':
	case L'\n':
	case L'\"':
	case L'\'':
	case L'>':
	case L'<':
	case L'&':
	case L'%':
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
std::wstring XMLTree::GetNumericCharacterReference(wchar_t character) const
{
	std::wstringstream substitution;
	substitution << L"&#" << (unsigned int)character << L';';
	return substitution.str();
}
