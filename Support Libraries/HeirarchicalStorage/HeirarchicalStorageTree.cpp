#include "HeirarchicalStorageTree.h"
#include <sstream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HeirarchicalStorageTree::HeirarchicalStorageTree()
:allowSeparateBinaryData(true), storageMode(STORAGEMODE_XML)
{
	root = new HeirarchicalStorageNode();
}

//----------------------------------------------------------------------------------------
HeirarchicalStorageTree::~HeirarchicalStorageTree()
{
	delete root;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageTree::Initialize()
{
	root->Initialize();
}

//----------------------------------------------------------------------------------------
//Save/Load functions
//----------------------------------------------------------------------------------------
bool HeirarchicalStorageTree::SaveTree(Stream::IStream& target)
{
	return SaveNode(*root, target, L"");
}

//----------------------------------------------------------------------------------------
bool HeirarchicalStorageTree::LoadTree(Stream::IStream& source)
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
	currentNodeDuringLoad = 0;
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
bool HeirarchicalStorageTree::SaveNode(IHeirarchicalStorageNode& node, Stream::IStream& stream, const std::wstring& indentPrefix) const
{
	Stream::ViewText streamView(stream);

	//Open key
	streamView << indentPrefix << L'<' << node.GetName();

	//Write attributes
	std::list<IHeirarchicalStorageAttribute*> attributeList = node.GetAttributeList();
	for(std::list<IHeirarchicalStorageAttribute*>::const_iterator i = attributeList.begin(); i != attributeList.end(); ++i)
	{
		std::wstring name = (*i)->GetName();
		std::wstring value = (*i)->GetValue();
		std::wstring finalValue;
		for(unsigned int valuePos = 0; valuePos < value.length(); ++valuePos)
		{
			if(IsCharacterReserved(value[valuePos]))
			{
				finalValue += GetNumericCharacterReference(value[valuePos]);
			}
			else
			{
				finalValue.push_back(value[valuePos]);
			}
		}
		streamView << L' ' << name << L"=\"" << finalValue << L"\"";
	}
	if(node.GetBinaryDataPresent())
	{
		streamView << L" BinaryDataPresent=\"1\"";
		if(GetSeparateBinaryDataEnabled() && !node.GetInlineBinaryDataEnabled())
		{
			streamView << L" SeparateBinaryData=\"1\"";
		}
	}

	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	if(childList.empty() && !node.GetBinaryDataPresent() && node.GetData().empty())
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
		if(!node.GetBinaryDataPresent())
		{
			std::wstring data = node.GetData();
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
			if(GetSeparateBinaryDataEnabled() && !node.GetInlineBinaryDataEnabled())
			{
				//Output the name of the separate binary storage buffer
				streamView << node.GetBinaryDataBufferName();
			}
			else
			{
				//Save binary data in the XML structure
				std::vector<unsigned char> buffer;
				node.ExtractBinaryData(buffer);
				for(unsigned int i = 0; i < buffer.size(); ++i)
				{
					streamView << Stream::Hex(2) << buffer[i];
				}
			}
		}

		//Write child elements
		if(!childList.empty())
		{
			streamView << L"\n";
			for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
			{
				//##TODO## Add error handling
				SaveNode(*(*i), stream, indentPrefix + L'\t');
			}
			streamView << indentPrefix;
		}

		//Close key
		streamView << L"</" << node.GetName() << L'>';
	}

	//Move to a new line in preparation for the next entity
	streamView << L"\n";
	return true;
}

//----------------------------------------------------------------------------------------
void XMLCALL HeirarchicalStorageTree::LoadStartElement(void *userData, const XML_Char *aname, const XML_Char **aatts)
{
	HeirarchicalStorageTree* tree = (HeirarchicalStorageTree*)userData;
	HeirarchicalStorageNode* node = 0;
	if(tree->currentNodeDuringLoad == 0)
	{
		node = tree->root;
		tree->currentNodeDuringLoad = node;
	}
	else
	{
		node = (HeirarchicalStorageNode*)(&tree->currentNodeDuringLoad->CreateChild());
		tree->currentNodeDuringLoad = node;
	}

	node->SetName(aname);
	while(*aatts != 0)
	{
		std::wstring name;
		std::wstring value;
		name = (*(aatts++));
		if(*aatts != 0)
		{
			value = (*(aatts++));
			node->CreateAttribute(name, value);
		}
	}
}

//----------------------------------------------------------------------------------------
void XMLCALL HeirarchicalStorageTree::LoadEndElement(void *userData, const XML_Char *aname)
{
	HeirarchicalStorageTree* tree = (HeirarchicalStorageTree*)userData;
	if(tree->currentNodeDuringLoad != 0)
	{
		tree->currentNodeDuringLoad = &tree->currentNodeDuringLoad->GetParent();
	}
}

//----------------------------------------------------------------------------------------
void XMLCALL HeirarchicalStorageTree::LoadData(void *userData, const XML_Char *s, int len)
{
	HeirarchicalStorageTree* tree = (HeirarchicalStorageTree*)userData;

	//Append all printable characters to the data stream
	std::wstring data;
	if(!tree->currentNodeDuringLoad->GetBinaryDataPresent())
	{
		data = tree->currentNodeDuringLoad->GetData();
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
	if(tree->currentNodeDuringLoad->IsAttributePresent(L"BinaryDataPresent"))
	{
		tree->currentNodeDuringLoad->SetBinaryDataPresent(true);
		if(tree->currentNodeDuringLoad->IsAttributePresent(L"SeparateBinaryData"))
		{
			tree->currentNodeDuringLoad->SetInlineBinaryDataEnabled(false);
			//Load the name of the separate binary storage buffer
			tree->currentNodeDuringLoad->SetBinaryDataBufferName(tree->currentNodeDuringLoad->GetBinaryDataBufferName() + data);
		}
		else
		{
			tree->currentNodeDuringLoad->SetInlineBinaryDataEnabled(true);
			//Load inline binary data from the XML structure
			size_t charPos = 0;
			while((data.length() - charPos) >= 2)
			{
				std::wstringstream dataStream;
				dataStream << data[charPos + 0] << data[charPos + 1];
				unsigned int byte;
				dataStream >> std::hex >> byte;

				Stream::ViewBinary bufferView(tree->currentNodeDuringLoad->GetBinaryDataBufferStream());
				bufferView << (unsigned char)byte;

				charPos	+= 2;
			}
		}
	}
	else
	{
		tree->currentNodeDuringLoad->SetData(data);
	}
}

//----------------------------------------------------------------------------------------
//Storage mode functions
//----------------------------------------------------------------------------------------
HeirarchicalStorageTree::StorageMode HeirarchicalStorageTree::GetStorageMode() const
{
	return storageMode;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageTree::SetStorageMode(StorageMode astorageMode)
{
	storageMode = astorageMode;
}

//----------------------------------------------------------------------------------------
bool HeirarchicalStorageTree::GetSeparateBinaryDataEnabled() const
{
	return allowSeparateBinaryData;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageTree::SetSeparateBinaryDataEnabled(bool state)
{
	allowSeparateBinaryData = state;
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageTree::GetRootNode() const
{
	return *root;
}

//----------------------------------------------------------------------------------------
//Error handling functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageTree::GetErrorStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetErrorString());
}

//----------------------------------------------------------------------------------------
//Node access functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageTree::GetBinaryDataNodeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageNode*>>& marshaller)
{
	marshaller.MarshalFrom(GetBinaryDataNodeList());
}

//----------------------------------------------------------------------------------------
//Reserved character substitution functions
//----------------------------------------------------------------------------------------
bool HeirarchicalStorageTree::IsCharacterReserved(wchar_t character) const
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
std::wstring HeirarchicalStorageTree::GetNumericCharacterReference(wchar_t character) const
{
	std::wstringstream substitution;
	substitution << L"&#" << (unsigned int)character << L';';
	return substitution.str();
}
