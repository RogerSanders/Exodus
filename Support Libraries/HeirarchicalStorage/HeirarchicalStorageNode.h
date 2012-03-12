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

	//Parent functions
	virtual IHeirarchicalStorageNode& GetParent() const;

	//Child functions
	using IHeirarchicalStorageNode::GetChildList;
	virtual IHeirarchicalStorageNode& CreateChild();
	virtual IHeirarchicalStorageNode& CreateChild(const wchar_t* aname);

	//Attribute functions
	using IHeirarchicalStorageNode::GetAttributeList;
	using IHeirarchicalStorageNode::IsAttributePresent;
	using IHeirarchicalStorageNode::GetAttribute;
	using IHeirarchicalStorageNode::CreateAttribute;
	virtual bool IsAttributePresent(const wchar_t* name) const;
	virtual IHeirarchicalStorageAttribute* GetAttribute(const wchar_t* name) const;
	virtual IHeirarchicalStorageAttribute& CreateAttribute(const wchar_t* name);

	//Binary data functions
	virtual bool GetBinaryDataPresent() const;
	virtual void SetBinaryDataPresent(bool state);
	virtual Stream::IStream& GetBinaryDataBufferStream();
	virtual bool GetInlineBinaryDataEnabled() const;
	virtual void SetInlineBinaryDataEnabled(bool state);
	void AddBinaryDataEntitiesToList(std::list<HeirarchicalStorageNode*>& binaryEntityList);

protected:
	//Name functions
	virtual const wchar_t* GetNameInternal() const;
	virtual void SetNameInternal(const wchar_t* aname);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual Stream::IStream& GetInternalStream() const;

	//Child functions
	virtual IHeirarchicalStorageNode** GetChildList(unsigned int& childCount);

	//Attribute functions
	virtual IHeirarchicalStorageAttribute** GetAttributeList(unsigned int& attributeCount);

	//Binary data functions
	virtual void SetBinaryDataBufferName(const wchar_t* aname);
	virtual const wchar_t* GetBinaryDataBufferNameInternal() const;

private:
	//Parent functions
	void SetParent(HeirarchicalStorageNode* aparent);

private:
	typedef std::vector<HeirarchicalStorageNode*> ChildList;
	typedef std::pair<std::wstring, HeirarchicalStorageAttribute*> AttributeListEntry;
	//Note that this is a vector rather than a map, so that we can preserve the explicit
	//ordering of attributes.
	typedef std::vector<AttributeListEntry> AttributeList;

	std::wstring name;
	HeirarchicalStorageNode* parent;
	ChildList children;
	AttributeList attributes;
	bool binaryDataPresent;
	bool inlineBinaryData;
	std::wstring binaryDataName;
	mutable Stream::Buffer dataStream;
	std::vector<HeirarchicalStorageAttribute*> attributeArrayCached;
};

#endif
