#ifndef __IHIERARCHICALSTORAGENODE_H__
#define __IHIERARCHICALSTORAGENODE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
#include <list>
#include <vector>
class IHierarchicalStorageAttribute;

class IHierarchicalStorageNode
{
public:
	//Constructors
	virtual ~IHierarchicalStorageNode() = 0 {}

	//Name functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname) = 0;

	//Parent functions
	virtual IHierarchicalStorageNode& GetParent() const = 0;

	//Content functions
	virtual bool IsEmpty() const = 0;

	//Child functions
	virtual IHierarchicalStorageNode& CreateChild() = 0;
	virtual IHierarchicalStorageNode& CreateChild(const MarshalSupport::Marshal::In<std::wstring>& aname) = 0;
	template<class T> IHierarchicalStorageNode& CreateChild(const std::wstring& aname, const T& adata);
	template<class T> IHierarchicalStorageNode& CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length);
	template<class T> IHierarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> IHierarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData = true);
	virtual void DeleteChild(IHierarchicalStorageNode& node) = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageNode*>> GetChildList() const = 0;
	virtual bool IsChildPresent(const MarshalSupport::Marshal::In<std::wstring>& name) const = 0;
	virtual IHierarchicalStorageNode* GetChild(const MarshalSupport::Marshal::In<std::wstring>& name, const IHierarchicalStorageNode* searchAfterChildNode = 0) const = 0;

	//Attribute functions
	virtual bool IsAttributePresent(const MarshalSupport::Marshal::In<std::wstring>& name) const = 0;
	virtual IHierarchicalStorageAttribute* GetAttribute(const MarshalSupport::Marshal::In<std::wstring>& name) const = 0;
	virtual IHierarchicalStorageAttribute& CreateAttribute(const MarshalSupport::Marshal::In<std::wstring>& name) = 0;
	template<class T> IHierarchicalStorageNode& CreateAttribute(const std::wstring& name, const T& value);
	template<class T> IHierarchicalStorageNode& CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length);
	template<class T> bool ExtractAttribute(const std::wstring& name, T& target);
	template<class T> bool ExtractAttributeHex(const std::wstring& name, T& target);
	virtual void DeleteAttribute(IHierarchicalStorageAttribute& attribute) = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<IHierarchicalStorageAttribute*>> GetAttributeList() const = 0;

	//Common data functions
	virtual void ClearData() = 0;

	//Data read functions
	inline std::wstring GetData() const;
	template<class T> T ExtractData();
	template<class T> T ExtractHexData();
	template<class T> IHierarchicalStorageNode& ExtractData(T& target);
	template<class T> IHierarchicalStorageNode& ExtractHexData(T& target);

	//Data write functions
	template<class T> IHierarchicalStorageNode& SetData(const T& adata);
	template<class T> IHierarchicalStorageNode& InsertData(const T& adata);
	template<class T> IHierarchicalStorageNode& InsertHexData(const T& adata, unsigned int length);

	//Binary data functions
	virtual bool GetBinaryDataPresent() const = 0;
	virtual void SetBinaryDataPresent(bool state) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetBinaryDataBufferName() const = 0;
	virtual void SetBinaryDataBufferName(const MarshalSupport::Marshal::In<std::wstring>& aname) = 0;
	virtual Stream::IStream& GetBinaryDataBufferStream() = 0;
	virtual bool GetInlineBinaryDataEnabled() const = 0;
	virtual void SetInlineBinaryDataEnabled(bool state) = 0;

	//Binary data read functions
	template<class T> T ExtractBinaryData();
	template<class T> IHierarchicalStorageNode& ExtractBinaryData(T& target);

	//Binary data write functions
	template<class T> IHierarchicalStorageNode& InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> IHierarchicalStorageNode& InsertBinaryData(const T* buffer, size_t entries, const std::wstring& bufferName, bool ainlineBinaryData = true);

protected:
	//Stream functions
	virtual void ResetInternalStreamPosition() const = 0;
	virtual Stream::IStream& GetInternalStream() const = 0;
};

#include "IHierarchicalStorageNode.inl"
#endif
