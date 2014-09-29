#ifndef __IHIERARCHICALSTORAGETREE_H__
#define __IHIERARCHICALSTORAGETREE_H__
#include "StreamInterface/StreamInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include <string>
#include <list>
class IHierarchicalStorageNode;

class IHierarchicalStorageTree
{
public:
	//Enumerations
	enum class StorageMode;

	//Constructors
	virtual ~IHierarchicalStorageTree() = 0 {}

	//Save/Load functions
	virtual bool SaveTree(Stream::IStream& target) = 0;
	virtual bool LoadTree(Stream::IStream& source) = 0;

	//Storage mode functions
	virtual StorageMode GetStorageMode() const = 0;
	virtual void SetStorageMode(StorageMode astorageMode) = 0;
	virtual bool GetSeparateBinaryDataEnabled() const = 0;
	virtual void SetSeparateBinaryDataEnabled(bool state) = 0;

	//Error handling functions
	inline std::wstring GetErrorString() const;

	//Node access functions
	virtual IHierarchicalStorageNode& GetRootNode() const = 0;
	inline std::list<IHierarchicalStorageNode*> GetBinaryDataNodeList();

protected:
	//Error handling functions
	virtual void GetErrorStringInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Node access functions
	virtual void GetBinaryDataNodeListInternal(const InteropSupport::ISTLObjectTarget<std::list<IHierarchicalStorageNode*>>& marshaller) = 0;
};

#include "IHierarchicalStorageTree.inl"
#endif
