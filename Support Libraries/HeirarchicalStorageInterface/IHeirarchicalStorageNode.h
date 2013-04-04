#ifndef __IHEIRARCHICALSTORAGENODE_H__
#define __IHEIRARCHICALSTORAGENODE_H__
#include "StreamInterface/StreamInterface.pkg"
#include <string>
#include <list>
#include <vector>
class IHeirarchicalStorageAttribute;

class IHeirarchicalStorageNode
{
public:
	//Constructors
	virtual ~IHeirarchicalStorageNode() = 0 {}

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

	//Parent functions
	virtual IHeirarchicalStorageNode& GetParent() const = 0;

	//Content functions
	virtual bool IsEmpty() const = 0;

	//Child functions
	virtual IHeirarchicalStorageNode& CreateChild() = 0;
	inline IHeirarchicalStorageNode& CreateChild(const std::wstring& aname);
	template<class T> IHeirarchicalStorageNode& CreateChild(const std::wstring& aname, const T& adata);
	template<class T> IHeirarchicalStorageNode& CreateChildHex(const std::wstring& aname, const T& adata, unsigned int length);
	template<class T> IHeirarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> IHeirarchicalStorageNode& CreateChildBinary(const std::wstring& aname, const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData = true);
	virtual void DeleteChild(IHeirarchicalStorageNode& node) = 0;
	inline std::list<IHeirarchicalStorageNode*> GetChildList();

	//Attribute functions
	inline bool IsAttributePresent(const std::wstring& name) const;
	inline IHeirarchicalStorageAttribute* GetAttribute(const std::wstring& name) const;
	inline IHeirarchicalStorageAttribute& CreateAttribute(const std::wstring& name);
	template<class T> IHeirarchicalStorageNode& CreateAttribute(const std::wstring& name, const T& value);
	template<class T> IHeirarchicalStorageNode& CreateAttributeHex(const std::wstring& name, const T& value, unsigned int length);
	template<class T> bool ExtractAttribute(const std::wstring& name, T& target);
	template<class T> bool ExtractAttributeHex(const std::wstring& name, T& target);
	virtual void DeleteAttribute(IHeirarchicalStorageAttribute& attribute) = 0;
	inline std::list<IHeirarchicalStorageAttribute*> GetAttributeList();

	//Common data functions
	virtual void ClearData() = 0;

	//Data read functions
	inline std::wstring GetData() const;
	template<class T> T ExtractData();
	template<class T> T ExtractHexData();
	template<class T> IHeirarchicalStorageNode& ExtractData(T& target);
	template<class T> IHeirarchicalStorageNode& ExtractHexData(T& target);

	//Data write functions
	template<class T> IHeirarchicalStorageNode& SetData(const T& adata);
	template<class T> IHeirarchicalStorageNode& InsertData(const T& adata);
	template<class T> IHeirarchicalStorageNode& InsertHexData(const T& adata, unsigned int length);

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
	template<class T> IHeirarchicalStorageNode& ExtractBinaryData(T& target);
	template<class T> IHeirarchicalStorageNode& ExtractBinaryData(std::vector<T>& target);
	template<> inline IHeirarchicalStorageNode& ExtractBinaryData(std::vector<bool>& target);
	template<> inline IHeirarchicalStorageNode& ExtractBinaryData(std::vector<unsigned char>& target);

	//Binary data write functions
	template<class T> IHeirarchicalStorageNode& InsertBinaryData(const T& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<class T> IHeirarchicalStorageNode& InsertBinaryData(const std::vector<T>& adata, const std::wstring& bufferName, bool ainlineBinaryData = true);
	template<> inline IHeirarchicalStorageNode& InsertBinaryData(const std::vector<unsigned char>& adata, const std::wstring& bufferName, bool ainlineBinaryData);
	template<class T> IHeirarchicalStorageNode& InsertBinaryData(const T* buffer, unsigned int entries, const std::wstring& bufferName, bool ainlineBinaryData = true);

protected:
	//Name functions
	virtual const wchar_t* GetNameInternal() const = 0;
	virtual void SetNameInternal(const wchar_t* aname) = 0;

	//Stream functions
	virtual void ResetInternalStreamPosition() const = 0;
	virtual Stream::IStream& GetInternalStream() const = 0;

	//Child functions
	virtual IHeirarchicalStorageNode& CreateChildInternal(const wchar_t* aname) = 0;
	virtual IHeirarchicalStorageNode** GetChildListInternal(unsigned int& childCount) = 0;

	//Attribute functions
	virtual IHeirarchicalStorageAttribute** GetAttributeListInternal(unsigned int& attributeCount) = 0;
	virtual bool IsAttributePresentInternal(const wchar_t* name) const = 0;
	virtual IHeirarchicalStorageAttribute* GetAttributeInternal(const wchar_t* name) const = 0;
	virtual IHeirarchicalStorageAttribute& CreateAttributeInternal(const wchar_t* name) = 0;

	//Binary data functions
	virtual const wchar_t* GetBinaryDataBufferNameInternal() const = 0;
	virtual void SetBinaryDataBufferNameInternal(const wchar_t* aname) = 0;
};

#include "IHeirarchicalStorageNode.inl"
#endif
