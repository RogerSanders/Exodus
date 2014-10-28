#ifndef __HIERARCHICALSTORAGENODE_H__
#define __HIERARCHICALSTORAGENODE_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "HierarchicalStorageAttribute.h"
#include "Stream/Stream.pkg"
#include <vector>
#include <map>
#include <string>

class HierarchicalStorageNode :public IHierarchicalStorageNode
{
public:
	//Constructors
	HierarchicalStorageNode();
	explicit HierarchicalStorageNode(const std::wstring& aname);
	~HierarchicalStorageNode();
	void Initialize();

	//Name functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname);

	//Parent functions
	virtual IHierarchicalStorageNode& GetParent() const;

	//Content functions
	virtual bool IsEmpty() const;

	//Child functions
	virtual IHierarchicalStorageNode& CreateChild();
	virtual IHierarchicalStorageNode& CreateChild(const MarshalSupport::Marshal::In<std::wstring>& aname);
	virtual void DeleteChild(IHierarchicalStorageNode& node);
	virtual MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageNode*>> GetChildList() const;
	virtual bool IsChildPresent(const MarshalSupport::Marshal::In<std::wstring>& name) const;
	virtual IHierarchicalStorageNode* GetChild(const MarshalSupport::Marshal::In<std::wstring>& name, const IHierarchicalStorageNode* searchAfterChildNode = 0) const;

	//Attribute functions
	using IHierarchicalStorageNode::CreateAttribute;
	virtual bool IsAttributePresent(const MarshalSupport::Marshal::In<std::wstring>& name) const;
	virtual IHierarchicalStorageAttribute* GetAttribute(const MarshalSupport::Marshal::In<std::wstring>& name) const;
	virtual IHierarchicalStorageAttribute& CreateAttribute(const MarshalSupport::Marshal::In<std::wstring>& name);
	virtual void DeleteAttribute(IHierarchicalStorageAttribute& attribute);
	virtual MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageAttribute*>> GetAttributeList() const;

	//Binary data functions
	virtual bool GetBinaryDataPresent() const;
	virtual void SetBinaryDataPresent(bool state);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetBinaryDataBufferName() const;
	virtual void SetBinaryDataBufferName(const MarshalSupport::Marshal::In<std::wstring>& aname);
	virtual Stream::IStream& GetBinaryDataBufferStream();
	virtual bool GetInlineBinaryDataEnabled() const;
	virtual void SetInlineBinaryDataEnabled(bool state);
	void AddBinaryDataEntitiesToList(std::list<IHierarchicalStorageNode*>& binaryEntityList);

protected:
	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual Stream::IStream& GetInternalStream() const;

	//Common data functions
	virtual void ClearData();

private:
	//Parent functions
	void SetParent(HierarchicalStorageNode* aparent);

private:
	//Typedefs
	typedef std::vector<HierarchicalStorageNode*> ChildList;
	typedef std::pair<std::wstring, HierarchicalStorageAttribute*> AttributeListEntry;
	//Note that this is a vector rather than a map, so that we can preserve the explicit
	//ordering of attributes.
	typedef std::vector<AttributeListEntry> AttributeList;

private:
	std::wstring name;
	HierarchicalStorageNode* parent;
	ChildList children;
	AttributeList attributes;
	bool binaryDataPresent;
	bool inlineBinaryData;
	std::wstring binaryDataName;
	mutable Stream::Buffer dataStream;
};

#endif
