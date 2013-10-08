#ifndef __HEIRARCHICALSTORAGENODE_H__
#define __HEIRARCHICALSTORAGENODE_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "HeirarchicalStorageAttribute.h"
#include "Stream/Stream.pkg"
#include <vector>
#include <map>
#include <string>

class HeirarchicalStorageNode :public IHeirarchicalStorageNode
{
public:
	//Constructors
	HeirarchicalStorageNode();
	explicit HeirarchicalStorageNode(const std::wstring& aname);
	~HeirarchicalStorageNode();
	void Initialize();

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Parent functions
	virtual IHeirarchicalStorageNode& GetParent() const;

	//Content functions
	virtual bool IsEmpty() const;

	//Child functions
	using IHeirarchicalStorageNode::CreateChild;
	virtual IHeirarchicalStorageNode& CreateChild();
	IHeirarchicalStorageNode& CreateChild(const std::wstring& aname);
	virtual void DeleteChild(IHeirarchicalStorageNode& node);
	std::list<IHeirarchicalStorageNode*> GetChildList();

	//Attribute functions
	using IHeirarchicalStorageNode::CreateAttribute;
	bool IsAttributePresent(const std::wstring& name) const;
	IHeirarchicalStorageAttribute* GetAttribute(const std::wstring& name) const;
	IHeirarchicalStorageAttribute& CreateAttribute(const std::wstring& name);
	virtual void DeleteAttribute(IHeirarchicalStorageAttribute& attribute);
	std::list<IHeirarchicalStorageAttribute*> GetAttributeList();

	//Binary data functions
	virtual bool GetBinaryDataPresent() const;
	virtual void SetBinaryDataPresent(bool state);
	inline std::wstring GetBinaryDataBufferName() const;
	inline void SetBinaryDataBufferName(const std::wstring& aname);
	virtual Stream::IStream& GetBinaryDataBufferStream();
	virtual bool GetInlineBinaryDataEnabled() const;
	virtual void SetInlineBinaryDataEnabled(bool state);
	void AddBinaryDataEntitiesToList(std::list<IHeirarchicalStorageNode*>& binaryEntityList);

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual Stream::IStream& GetInternalStream() const;

	//Child functions
	virtual IHeirarchicalStorageNode& CreateChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void GetChildListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageNode*>>& marshaller);

	//Attribute functions
	virtual bool IsAttributePresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const;
	virtual IHeirarchicalStorageAttribute* GetAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const;
	virtual IHeirarchicalStorageAttribute& CreateAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void GetAttributeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageAttribute*>>& marshaller);

	//Common data functions
	virtual void ClearData();

	//Binary data functions
	virtual void GetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

private:
	//Parent functions
	void SetParent(HeirarchicalStorageNode* aparent);

private:
	//Typedefs
	typedef std::vector<HeirarchicalStorageNode*> ChildList;
	typedef std::pair<std::wstring, HeirarchicalStorageAttribute*> AttributeListEntry;
	//Note that this is a vector rather than a map, so that we can preserve the explicit
	//ordering of attributes.
	typedef std::vector<AttributeListEntry> AttributeList;

private:
	std::wstring name;
	HeirarchicalStorageNode* parent;
	ChildList children;
	AttributeList attributes;
	bool binaryDataPresent;
	bool inlineBinaryData;
	std::wstring binaryDataName;
	mutable Stream::Buffer dataStream;
};

#include "HeirarchicalStorageNode.inl"
#endif
