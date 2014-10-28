#include "HierarchicalStorageNode.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HierarchicalStorageNode::HierarchicalStorageNode()
:parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TextEncoding::UTF16, Stream::IStream::NewLineEncoding::Unix, Stream::IStream::ByteOrder::BigEndian, 0)
{}

//----------------------------------------------------------------------------------------
HierarchicalStorageNode::HierarchicalStorageNode(const std::wstring& aname)
:name(aname), parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TextEncoding::UTF16, Stream::IStream::NewLineEncoding::Unix, Stream::IStream::ByteOrder::BigEndian, 0)
{}

//----------------------------------------------------------------------------------------
HierarchicalStorageNode::~HierarchicalStorageNode()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::Initialize()
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
MarshalSupport::Marshal::Ret<std::wstring> HierarchicalStorageNode::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetName(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::GetParent() const
{
	return *parent;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetParent(HierarchicalStorageNode* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Content functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsEmpty() const
{
	return (children.empty() && attributes.empty() && !binaryDataPresent && (dataStream.Size() == 0));
}

//----------------------------------------------------------------------------------------
//Stream functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::ResetInternalStreamPosition() const
{
	dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageNode::GetInternalStream() const
{
	return dataStream;
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChild()
{
	HierarchicalStorageNode* child = new HierarchicalStorageNode();
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChild(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	HierarchicalStorageNode* child = new HierarchicalStorageNode(aname);
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::DeleteChild(IHierarchicalStorageNode& node)
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
MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageNode*>> HierarchicalStorageNode::GetChildList() const
{
	std::list<IHierarchicalStorageNode*> childList;
	for(size_t i = 0; i < children.size(); ++i)
	{
		childList.push_back(children[i]);
	}
	return childList;
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsChildPresent(const MarshalSupport::Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(ChildList::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		HierarchicalStorageNode* childNode = *i;
		if(childNode->GetName() == nameResolved)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode* HierarchicalStorageNode::GetChild(const MarshalSupport::Marshal::In<std::wstring>& name, const IHierarchicalStorageNode* searchAfterChildNode) const
{
	std::wstring nameResolved = name.Get();
	bool foundSearchStartNode = (searchAfterChildNode == 0);
	for(ChildList::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		HierarchicalStorageNode* childNode = *i;
		if(foundSearchStartNode && (childNode->GetName() == nameResolved))
		{
			return childNode;
		}
		foundSearchStartNode |= (childNode == searchAfterChildNode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsAttributePresent(const MarshalSupport::Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(AttributeList::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		if(i->first == nameResolved)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute* HierarchicalStorageNode::GetAttribute(const MarshalSupport::Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(AttributeList::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
	{
		if(i->first == nameResolved)
		{
			return i->second;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute& HierarchicalStorageNode::CreateAttribute(const MarshalSupport::Marshal::In<std::wstring>& name)
{
	IHierarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		HierarchicalStorageAttribute* newAttribute = new HierarchicalStorageAttribute(name);
		attribute = newAttribute;
		attributes.push_back(AttributeListEntry(name, newAttribute));
	}
	return *attribute;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::DeleteAttribute(IHierarchicalStorageAttribute& attribute)
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
MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageAttribute*>> HierarchicalStorageNode::GetAttributeList() const
{
	std::list<IHierarchicalStorageAttribute*> attributeList;
	for(size_t i = 0; i < attributes.size(); ++i)
	{
		attributeList.push_back(attributes[i].second);
	}
	return attributeList;
}

//----------------------------------------------------------------------------------------
//Common data functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::ClearData()
{
	binaryDataPresent = false;
	inlineBinaryData = false;
	binaryDataName.clear();
	dataStream.Resize(0);
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::GetBinaryDataPresent() const
{
	return binaryDataPresent;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataPresent(bool state)
{
	binaryDataPresent = state;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> HierarchicalStorageNode::GetBinaryDataBufferName() const
{
	return binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataBufferName(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	binaryDataName = aname;
}

//----------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageNode::GetBinaryDataBufferStream()
{
	return dataStream;
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::GetInlineBinaryDataEnabled() const
{
	return inlineBinaryData;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetInlineBinaryDataEnabled(bool state)
{
	inlineBinaryData = state;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::AddBinaryDataEntitiesToList(std::list<IHierarchicalStorageNode*>& binaryEntityList)
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
