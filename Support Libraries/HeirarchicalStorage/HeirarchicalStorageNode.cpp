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
//Content functions
//----------------------------------------------------------------------------------------
bool HeirarchicalStorageNode::IsEmpty() const
{
	return (children.empty() && attributes.empty() && !binaryDataPresent && (dataStream.Size() == 0));
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
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChild()
{
	HeirarchicalStorageNode* child = new HeirarchicalStorageNode();
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChild(const std::wstring& aname)
{
	HeirarchicalStorageNode* child = new HeirarchicalStorageNode(aname);
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChildInternal(const wchar_t* aname)
{
	return CreateChild(aname);
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::DeleteChild(IHeirarchicalStorageNode& node)
{
	ChildList::iterator childListIterator = children.begin();
	while(childListIterator != children.end())
	{
		if(*childListIterator == &node)
		{
			children.erase(childListIterator);
			return;
		}
		++childListIterator;
	}
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageNode** HeirarchicalStorageNode::GetChildListInternal(unsigned int& childCount)
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
bool HeirarchicalStorageNode::IsAttributePresent(const std::wstring& name) const
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
IHeirarchicalStorageAttribute* HeirarchicalStorageNode::GetAttribute(const std::wstring& name) const
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
IHeirarchicalStorageAttribute& HeirarchicalStorageNode::CreateAttribute(const std::wstring& name)
{
	IHeirarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		HeirarchicalStorageAttribute* newAttribute = new HeirarchicalStorageAttribute(name);
		attribute = newAttribute;
		attributes.push_back(AttributeListEntry(name, newAttribute));
	}
	return *attribute;
}

//----------------------------------------------------------------------------------------
bool HeirarchicalStorageNode::IsAttributePresentInternal(const wchar_t* name) const
{
	return IsAttributePresent(name);
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute* HeirarchicalStorageNode::GetAttributeInternal(const wchar_t* name) const
{
	return GetAttribute(name);
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute& HeirarchicalStorageNode::CreateAttributeInternal(const wchar_t* name)
{
	return CreateAttribute(name);
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::DeleteAttribute(IHeirarchicalStorageAttribute& attribute)
{
	AttributeList::iterator attributeIterator = attributes.begin();
	while(attributeIterator != attributes.end())
	{
		if(attributeIterator->second == &attribute)
		{
			attributes.erase(attributeIterator);
			return;
		}
	}
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute** HeirarchicalStorageNode::GetAttributeListInternal(unsigned int& attributeCount)
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
//Common data functions
//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::ClearData()
{
	binaryDataPresent = false;
	inlineBinaryData = false;
	binaryDataName.clear();
	dataStream.Resize(0);
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
std::wstring HeirarchicalStorageNode::GetBinaryDataBufferName() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataBufferName(const std::wstring& aname)
{
	binaryDataName = aname;
}

//----------------------------------------------------------------------------------------
const wchar_t* HeirarchicalStorageNode::GetBinaryDataBufferNameInternal() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataBufferNameInternal(const wchar_t* aname)
{
	binaryDataName = aname;
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
