#include "HeirarchicalStorageNode.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HeirarchicalStorageNode::HeirarchicalStorageNode()
:parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, Stream::IStream::NEWLINEENCODING_UNIX, Stream::IStream::BYTEORDER_BIGENDIAN, 0)
{}

//----------------------------------------------------------------------------------------
HeirarchicalStorageNode::HeirarchicalStorageNode(const std::wstring& aname)
:name(aname), parent(0), binaryDataPresent(false), inlineBinaryData(false), dataStream(Stream::IStream::TEXTENCODING_UTF16, Stream::IStream::NEWLINEENCODING_UNIX, Stream::IStream::BYTEORDER_BIGENDIAN, 0)
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
void HeirarchicalStorageNode::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
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
IHeirarchicalStorageNode& HeirarchicalStorageNode::CreateChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return CreateChild(nameMarshaller.MarshalTo());
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
std::list<IHeirarchicalStorageNode*> HeirarchicalStorageNode::GetChildList()
{
	std::list<IHeirarchicalStorageNode*> childList;
	for(size_t i = 0; i < children.size(); ++i)
	{
		childList.push_back(children[i]);
	}
	return childList;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::GetChildListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageNode*>>& marshaller)
{
	marshaller.MarshalFrom(GetChildList());
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
bool HeirarchicalStorageNode::IsAttributePresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const
{
	return IsAttributePresent(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute* HeirarchicalStorageNode::GetAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const
{
	return GetAttribute(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IHeirarchicalStorageAttribute& HeirarchicalStorageNode::CreateAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return CreateAttribute(nameMarshaller.MarshalTo());
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
std::list<IHeirarchicalStorageAttribute*> HeirarchicalStorageNode::GetAttributeList()
{
	std::list<IHeirarchicalStorageAttribute*> attributeList;
	for(size_t i = 0; i < attributes.size(); ++i)
	{
		attributeList.push_back(attributes[i].second);
	}
	return attributeList;
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::GetAttributeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageAttribute*>>& marshaller)
{
	marshaller.MarshalFrom(GetAttributeList());
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
void HeirarchicalStorageNode::GetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetBinaryDataBufferName());
}

//----------------------------------------------------------------------------------------
void HeirarchicalStorageNode::SetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetBinaryDataBufferName(marshaller.MarshalTo());
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
void HeirarchicalStorageNode::AddBinaryDataEntitiesToList(std::list<IHeirarchicalStorageNode*>& binaryEntityList)
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
