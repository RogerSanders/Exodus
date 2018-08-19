#include "HierarchicalStorageNode.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
HierarchicalStorageNode::HierarchicalStorageNode()
:_parent(0), _binaryDataPresent(false), _inlineBinaryData(false), _dataStream(Stream::IStream::TextEncoding::UTF16, Stream::IStream::NewLineEncoding::Unix, Stream::IStream::ByteOrder::BigEndian, 0)
{}

//----------------------------------------------------------------------------------------
HierarchicalStorageNode::HierarchicalStorageNode(const std::wstring& name)
:_name(name), _parent(0), _binaryDataPresent(false), _inlineBinaryData(false), _dataStream(Stream::IStream::TextEncoding::UTF16, Stream::IStream::NewLineEncoding::Unix, Stream::IStream::ByteOrder::BigEndian, 0)
{}

//----------------------------------------------------------------------------------------
HierarchicalStorageNode::~HierarchicalStorageNode()
{
	Initialize();
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::Initialize()
{
	for(ChildList::iterator i = _children.begin(); i != _children.end(); ++i)
	{
		delete *i;
	}
	for(AttributeList::iterator i = _attributes.begin(); i != _attributes.end(); ++i)
	{
		delete i->second;
	}
	_children.clear();
	_attributes.clear();
	_binaryDataName.clear();
	_binaryDataPresent = false;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> HierarchicalStorageNode::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::GetParent() const
{
	return *_parent;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetParent(HierarchicalStorageNode* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------
//Content functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsEmpty() const
{
	return (_children.empty() && _attributes.empty() && !_binaryDataPresent && (_dataStream.Size() == 0));
}

//----------------------------------------------------------------------------------------
//Stream functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::ResetInternalStreamPosition() const
{
	_dataStream.SetStreamPos(0);
}

//----------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageNode::GetInternalStream() const
{
	return _dataStream;
}

//----------------------------------------------------------------------------------------
//Child functions
//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChild()
{
	HierarchicalStorageNode* child = new HierarchicalStorageNode();
	child->SetParent(this);
	_children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageNode& HierarchicalStorageNode::CreateChild(const Marshal::In<std::wstring>& name)
{
	HierarchicalStorageNode* child = new HierarchicalStorageNode(name);
	child->SetParent(this);
	_children.push_back(child);
	return *child;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::DeleteChild(IHierarchicalStorageNode& node)
{
	ChildList::iterator childListIterator = _children.begin();
	while(childListIterator != _children.end())
	{
		if(*childListIterator == &node)
		{
			_children.erase(childListIterator);
			return;
		}
		++childListIterator;
	}
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::list<IHierarchicalStorageNode*>> HierarchicalStorageNode::GetChildList() const
{
	std::list<IHierarchicalStorageNode*> childList;
	for(size_t i = 0; i < _children.size(); ++i)
	{
		childList.push_back(_children[i]);
	}
	return childList;
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::IsChildPresent(const Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(ChildList::const_iterator i = _children.begin(); i != _children.end(); ++i)
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
IHierarchicalStorageNode* HierarchicalStorageNode::GetChild(const Marshal::In<std::wstring>& name, const IHierarchicalStorageNode* searchAfterChildNode) const
{
	std::wstring nameResolved = name.Get();
	bool foundSearchStartNode = (searchAfterChildNode == 0);
	for(ChildList::const_iterator i = _children.begin(); i != _children.end(); ++i)
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
bool HierarchicalStorageNode::IsAttributePresent(const Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(AttributeList::const_iterator i = _attributes.begin(); i != _attributes.end(); ++i)
	{
		if(i->first == nameResolved)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute* HierarchicalStorageNode::GetAttribute(const Marshal::In<std::wstring>& name) const
{
	std::wstring nameResolved = name.Get();
	for(AttributeList::const_iterator i = _attributes.begin(); i != _attributes.end(); ++i)
	{
		if(i->first == nameResolved)
		{
			return i->second;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IHierarchicalStorageAttribute& HierarchicalStorageNode::CreateAttribute(const Marshal::In<std::wstring>& name)
{
	IHierarchicalStorageAttribute* attribute = GetAttribute(name);
	if(attribute == 0)
	{
		HierarchicalStorageAttribute* newAttribute = new HierarchicalStorageAttribute(name);
		attribute = newAttribute;
		_attributes.push_back(AttributeListEntry(name, newAttribute));
	}
	return *attribute;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::DeleteAttribute(IHierarchicalStorageAttribute& attribute)
{
	AttributeList::iterator attributeIterator = _attributes.begin();
	while(attributeIterator != _attributes.end())
	{
		if(attributeIterator->second == &attribute)
		{
			_attributes.erase(attributeIterator);
			return;
		}
	}
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::list<IHierarchicalStorageAttribute*>> HierarchicalStorageNode::GetAttributeList() const
{
	std::list<IHierarchicalStorageAttribute*> attributeList;
	for(size_t i = 0; i < _attributes.size(); ++i)
	{
		attributeList.push_back(_attributes[i].second);
	}
	return attributeList;
}

//----------------------------------------------------------------------------------------
//Common data functions
//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::ClearData()
{
	_binaryDataPresent = false;
	_inlineBinaryData = false;
	_binaryDataName.clear();
	_dataStream.Resize(0);
}

//----------------------------------------------------------------------------------------
//Binary data functions
//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::GetBinaryDataPresent() const
{
	return _binaryDataPresent;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataPresent(bool state)
{
	_binaryDataPresent = state;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> HierarchicalStorageNode::GetBinaryDataBufferName() const
{
	return _binaryDataName.c_str();
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetBinaryDataBufferName(const Marshal::In<std::wstring>& name)
{
	_binaryDataName = name;
}

//----------------------------------------------------------------------------------------
Stream::IStream& HierarchicalStorageNode::GetBinaryDataBufferStream()
{
	return _dataStream;
}

//----------------------------------------------------------------------------------------
bool HierarchicalStorageNode::GetInlineBinaryDataEnabled() const
{
	return _inlineBinaryData;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::SetInlineBinaryDataEnabled(bool state)
{
	_inlineBinaryData = state;
}

//----------------------------------------------------------------------------------------
void HierarchicalStorageNode::AddBinaryDataEntitiesToList(std::list<IHierarchicalStorageNode*>& binaryEntityList)
{
	if(_binaryDataPresent && !_inlineBinaryData)
	{
		binaryEntityList.push_back(this);
	}
	for(ChildList::iterator i = _children.begin(); i != _children.end(); ++i)
	{
		(*i)->AddBinaryDataEntitiesToList(binaryEntityList);
	}
}
