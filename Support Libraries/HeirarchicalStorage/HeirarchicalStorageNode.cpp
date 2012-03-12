#include "HeirarchicalStorageNode.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HeirarchicalStorageNode::HeirarchicalStorageNode()
:parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{}

//----------------------------------------------------------------------------------------
HeirarchicalStorageNode::HeirarchicalStorageNode(const std::wstring& aname)
:name(aname), parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, 0)
{}

//----------------------------------------------------------------------------------------
HeirarchicalStorageNode::~HeirarchicalStorageNode()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::Initialize()
{
	for(ChildList::iterator i = children.begin(); i != children.end(); ++i)
	{
		delete *i;
	}
	for(AttributeList::iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		delete i->second;
	}
	children.clear();
	attributes.clear();
	binaryDataName.clear();
	binaryDataPresent = false;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageNode::GetParent() const
{
	return *parent;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetParent(HeirarchicalStorageNode* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChild()
{
	HeirarchicalStorageNode* child = new HeirarchicalStorageNode();
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChild(const wchar_t* aname)
{
	HeirarchicalStorageNode* child = new HeirarchicalStorageNode(aname);
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
bool HeirarchicalStorageNode::IsAttributePresent(const wchar_t* name) const
{
	for(AttributeList::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		if(i->first == name)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute* HeirarchicalStorageNode::GetAttribute(const wchar_t* name) const
{
	for(AttributeList::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		if(i->first == name)
		{
			return i->second;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute& HeirarchicalStorageNode::CreateAttribute(const wchar_t* name)
{
	HeirarchicalStorageAttribute* attribute = new HeirarchicalStorageAttribute(name);
	attributes.push_back(AttributeListEntry(name, attribute));
	return *attribute;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
bool HeirarchicalStorageNode::GetBinaryDataPresent() const
{
	return binaryDataPresent;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataPresent(bool state)
{
	binaryDataPresent = state;
}

//----------------------------------------------------------------------------------------
Stream::IStream& HeirarchicalStorageNode::GetBinaryDataBufferStream()
{
	return dataStream;
}

//----------------------------------------------------------------------------------------
bool HeirarchicalStorageNode::GetInlineBinaryDataEnabled() const
{
	return inlineBinaryData;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetInlineBinaryDataEnabled(bool state)
{
	inlineBinaryData = state;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
const wchar_t* HeirarchicalStorageNode::GetNameInternal() const
{
	return name.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetNameInternal(const wchar_t* aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Stream functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::ResetInternalStreamPosition() const
{
	dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
Stream::IStream& HeirarchicalStorageNode::GetInternalStream() const
{
	return dataStream;
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode** HeirarchicalStorageNode::GetChildList(unsigned int& childCount)
{
	childCount = (unsigned int)children.size();
	if(childCount > 0)
	{
		return (IHeirarchicalStorageNode**)&children[0];
	}
	return (IHeirarchicalStorageNode**)0;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute** HeirarchicalStorageNode::GetAttributeList(unsigned int& attributeCount)
{
	attributeArrayCached.clear();
	attributeArrayCached.reserve(attributes.size());
	for(AttributeList::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		attributeArrayCached.push_back(i->second);
	}
	attributeCount = (unsigned int)attributeArrayCached.size();
	if(attributeCount > 0)
	{
		return (IHeirarchicalStorageAttribute**)&attributeArrayCached[0];
	}
	return (IHeirarchicalStorageAttribute**)0;
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataBufferName(const wchar_t* aname)
{
	binaryDataName = aname;
}

//----------------------------------------------------------------------------------------
const wchar_t* HeirarchicalStorageNode::GetBinaryDataBufferNameInternal() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::AddBinaryDataEntitiesToList(std::list<HeirarchicalStorageNode*>& binaryEntityList)
{
	if(binaryDataPresent && !inlineBinaryData)
	{
		binaryEntityList.push_back(this);
	}
	for(ChildList::iterator i = children.begin(); i != children.end(); ++i)
	{
		(*i)->AddBinaryDataEntitiesToList(binaryEntityList);
	}
}
