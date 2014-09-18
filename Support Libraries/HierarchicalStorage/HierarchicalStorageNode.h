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
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Parent functions
	virtual IHierarchicalStorageNode& GetParent() const;

	//Content functions
	virtual bool IsEmpty() const;

	//Child functions
	using IHierarchicalStorageNode::CreateChild;
	virtual IHierarchicalStorageNode& CreateChild();
	IHierarchicalStorageNode& CreateChild(const std::wstring& aname);
	virtual void DeleteChild(IHierarchicalStorageNode& node);
	std::list<IHierarchicalStorageNode*> GetChildList() const;
	bool IsChildPresent(const std::wstring& name) const;
	IHierarchicalStorageNode* GetChild(const std::wstring& name, const IHierarchicalStorageNode* searchAfterChildNode = 0) const;

	//Attribute functions
	using IHierarchicalStorageNode::CreateAttribute;
	bool IsAttributePresent(const std::wstring& name) const;
	IHierarchicalStorageAttribute* GetAttribute(const std::wstring& name) const;
	IHierarchicalStorageAttribute& CreateAttribute(const std::wstring& name);
	virtual void DeleteAttribute(IHierarchicalStorageAttribute& attribute);
	std::list<IHierarchicalStorageAttribute*> GetAttributeList() const;

	//Binary data functions
	virtual bool GetBinaryDataPresent() const;
	virtual void SetBinaryDataPresent(bool state);
	inline std::wstring GetBinaryDataBufferName() const;
	inline void SetBinaryDataBufferName(const std::wstring& aname);
	virtual Stream::IStream& GetBinaryDataBufferStream();
	virtual bool GetInlineBinaryDataEnabled() const;
	virtual void SetInlineBinaryDataEnabled(bool state);
	void AddBinaryDataEntitiesToList(std::list<IHierarchicalStorageNode*>& binaryEntityList);

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual Stream::IStream& GetInternalStream() const;

	//Child functions
	virtual IHierarchicalStorageNode& CreateChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void GetChildListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageNode*>>& marshaller) const;
	virtual bool IsChildPresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const;
	virtual IHierarchicalStorageNode* GetChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller, const IHierarchicalStorageNode* searchAfterChildNode) const;

	//Attribute functions
	virtual bool IsAttributePresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const;
	virtual IHierarchicalStorageAttribute* GetAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const;
	virtual IHierarchicalStorageAttribute& CreateAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void GetAttributeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageAttribute*>>& marshaller) const;

	//Common data functions
	virtual void ClearData();

	//Binary data functions
	virtual void GetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

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

#include "HierarchicalStorageNode.inl"
#endif
