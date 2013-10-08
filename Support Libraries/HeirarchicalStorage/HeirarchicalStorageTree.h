#ifndef __HEIRARCHICALSTORAGETREE_H__
#define __HEIRARCHICALSTORAGETREE_H__
#define XML_STATIC
#define XML_UNICODE_WCHAR_T
#include <expat.h>
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "HeirarchicalStorageNode.h"
#include <vector>

class HeirarchicalStorageTree :public IHeirarchicalStorageTree
{
public:
	//Constructors
	HeirarchicalStorageTree();
	~HeirarchicalStorageTree();
	void Initialize();

	//Save/Load functions
	//##TODO## Take the storage mode into account when saving and loading
	virtual bool SaveTree(Stream::IStream& target);
	virtual bool LoadTree(Stream::IStream& source);

	//Storage mode functions
	virtual StorageMode GetStorageMode() const;
	virtual void SetStorageMode(StorageMode astorageMode);
	virtual bool GetSeparateBinaryDataEnabled() const;
	virtual void SetSeparateBinaryDataEnabled(bool state);

	//Error handling functions
	inline std::wstring GetErrorString() const;

	//Node access functions
	virtual IHeirarchicalStorageNode& GetRootNode() const;
	inline std::list<IHeirarchicalStorageNode*> GetBinaryDataNodeList();

protected:
	//Error handling functions
	virtual void GetErrorStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Node access functions
	virtual void GetBinaryDataNodeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHeirarchicalStorageNode*>>& marshaller);

private:
	//Save/Load functions
	bool SaveNode(IHeirarchicalStorageNode& node, Stream::IStream& stream, const std::wstring& indentPrefix) const;
	static void XMLCALL LoadStartElement(void *userData, const XML_Char *aname, const XML_Char **aatts);
	static void XMLCALL LoadEndElement(void *userData, const XML_Char *aname);
	static void XMLCALL LoadData(void *userData, const XML_Char *s, int len);

	//Reserved character substitution functions
	bool IsCharacterReserved(wchar_t character) const;
	std::wstring GetNumericCharacterReference(wchar_t character) const;

private:
	StorageMode storageMode;
	HeirarchicalStorageNode* root;
	IHeirarchicalStorageNode* currentNodeDuringLoad;
	mutable std::wstring errorString;
	bool allowSeparateBinaryData;
};

#include "HeirarchicalStorageTree.inl"
#endif
