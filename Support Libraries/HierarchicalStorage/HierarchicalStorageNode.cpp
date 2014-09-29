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
void HierarchicalStorageNode::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
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
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChild(const std::wstring& aname)
{
	HierarchicalStorageNode* child = new HierarchicalStorageNode(aname);
	child->SetParent(this);
	children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return CreateChild(nameMarshaller.MarshalTo());
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
std::list<IHierarchicalStorageNode*> HierarchicalStorageNode::GetChildList() const
{
	std::list<IHierarchicalStorageNode*> childList;
	for(size_t i = 0; i < children.size(); ++i)
	{
		childList.push_back(children[i]);
	}
	return childList;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::GetChildListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageNode*>>& marshaller) const
{
	marshaller.MarshalFrom(GetChildList());
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsChildPresent(const std::wstring& name) const
{
	for(ChildList::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		HierarchicalStorageNode* childNode = *i;
		if(childNode->GetName() == name)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsChildPresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const
{
	return IsChildPresent(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode* HierarchicalStorageNode::GetChild(const std::wstring& name, const IHierarchicalStorageNode* searchAfterChildNode) const
{
	bool foundSearchStartNode = (searchAfterChildNode == 0);
	for(ChildList::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		HierarchicalStorageNode* childNode = *i;
		if(foundSearchStartNode && (childNode->GetName() == name))
		{
			return childNode;
		}
		foundSearchStartNode |= (childNode == searchAfterChildNode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode* HierarchicalStorageNode::GetChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller, const IHierarchicalStorageNode* searchAfterChildNode) const
{
	return GetChild(nameMarshaller.MarshalTo(), searchAfterChildNode);
}

//----------------------------------------------------------------------------------------
//Attribute functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsAttributePresent(const std::wstring& name) const
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
IHierarchicalStorageAttribute* HierarchicalStorageNode::GetAttribute(const std::wstring& name) const
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
IHierarchicalStorageAttribute& HierarchicalStorageNode::CreateAttribute(const std::wstring& name)
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
bool HierarchicalStorageNode::IsAttributePresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const
{
	return IsAttributePresent(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute* HierarchicalStorageNode::GetAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const
{
	return GetAttribute(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute& HierarchicalStorageNode::CreateAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return CreateAttribute(nameMarshaller.MarshalTo());
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
std::list<IHierarchicalStorageAttribute*> HierarchicalStorageNode::GetAttributeList() const
{
	std::list<IHierarchicalStorageAttribute*> attributeList;
	for(size_t i = 0; i < attributes.size(); ++i)
	{
		attributeList.push_back(attributes[i].second);
	}
	return attributeList;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::GetAttributeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageAttribute*>>& marshaller) const
{
	marshaller.MarshalFrom(GetAttributeList());
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
void HierarchicalStorageNode::GetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetBinaryDataBufferName());
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetBinaryDataBufferName(marshaller.MarshalTo());
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
