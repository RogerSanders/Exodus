#ifndef __IHIERARCHICALSTORAGENODE_H__
#define __IHIERARCHICALSTORAGENODE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
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
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Parent functions
	virtual IHierarchicalStorageNode& GetParent() const = 0;

	//Content functions
	virtual bool IsEmpty() const = 0;

	//Child functions
	virtual IHierarchicalStorageNode& CreateChild() = 0;
	inline IHierarchicalStorageNode& CreateChild(const std::wstring& aname);
	template<class T> IHierarchicalStorageNode& CreateChild(const std::wstring& aname, const T& adata);
	template<class T> IHierarchicalStorageNode& CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length);
	template<class T> IHierarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> IHierarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData = true);
	virtual void DeleteChild(IHierarchicalStorageNode& node) = 0;
	inline std::list<IHierarchicalStorageNode*> GetChildList();

	//Attribute functions
	inline bool IsAttributePresent(const std::wstring& name) const;
	inline IHierarchicalStorageAttribute* GetAttribute(const std::wstring& name) const;
	inline IHierarchicalStorageAttribute& CreateAttribute(const std::wstring& name);
	template<class T> IHierarchicalStorageNode& CreateAttribute(const std::wstring& name, const T& value);
	template<class T> IHierarchicalStorageNode& CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length);
	template<class T> bool ExtractAttribute(const std::wstring& name, T& target);
	template<class T> bool ExtractAttributeHex(const std::wstring& name, T& target);
	virtual void DeleteAttribute(IHierarchicalStorageAttribute& attribute) = 0;
	inline std::list<IHierarchicalStorageAttribute*> GetAttributeList();

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
	inline std::wstring GetBinaryDataBufferName() const;
	inline void SetBinaryDataBufferName(const std::wstring& aname);
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
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;

	//Stream functions
	virtual void ResetInternalStreamPosition() const = 0;
	virtual Stream::IStream& GetInternalStream() const = 0;

	//Child functions
	virtual IHierarchicalStorageNode& CreateChildInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual void GetChildListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageNode*>>& marshaller) = 0;

	//Attribute functions
	virtual bool IsAttributePresentInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const = 0;
	virtual IHierarchicalStorageAttribute* GetAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) const = 0;
	virtual IHierarchicalStorageAttribute& CreateAttributeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual void GetAttributeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageAttribute*>>& marshaller) = 0;

	//Binary data functions
	virtual void GetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetBinaryDataBufferNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "IHierarchicalStorageNode.inl"
#endif
