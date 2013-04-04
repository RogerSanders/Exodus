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

	//Content functions
	virtual bool IsEmpty() const;

	//Child functions
	using IHeirarchicalStorageNode::CreateChild;
	virtual IHeirarchicalStorageNode& CreateChild();
	IHeirarchicalStorageNode& CreateChild(const std::wstring& aname);
	virtual void DeleteChild(IHeirarchicalStorageNode& node);

	//Attribute functions
	using IHeirarchicalStorageNode::CreateAttribute;
	bool IsAttributePresent(const std::wstring& name) const;
	IHeirarchicalStorageAttribute* GetAttribute(const std::wstring& name) const;
	IHeirarchicalStorageAttribute& CreateAttribute(const std::wstring& name);
	virtual void DeleteAttribute(IHeirarchicalStorageAttribute& attribute);

	//Binary data functions
	virtual bool GetBinaryDataPresent() const;
	virtual void SetBinaryDataPresent(bool state);
	std::wstring GetBinaryDataBufferName() const;
	void SetBinaryDataBufferName(const std::wstring& aname);
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
	virtual IHeirarchicalStorageNode& CreateChildInternal(const wchar_t* aname);
	virtual IHeirarchicalStorageNode** GetChildListInternal(unsigned int& childCount);

	//Attribute functions
	virtual IHeirarchicalStorageAttribute** GetAttributeListInternal(unsigned int& attributeCount);
	virtual bool IsAttributePresentInternal(const wchar_t* name) const;
	virtual IHeirarchicalStorageAttribute* GetAttributeInternal(const wchar_t* name) const;
	virtual IHeirarchicalStorageAttribute& CreateAttributeInternal(const wchar_t* name);

	//Common data functions
	virtual void ClearData();

	//Binary data functions
	virtual const wchar_t* GetBinaryDataBufferNameInternal() const;
	virtual void SetBinaryDataBufferNameInternal(const wchar_t* aname);

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
