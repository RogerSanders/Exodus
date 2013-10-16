#ifndef __MEMORYREAD_H__
#define __MEMORYREAD_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"

//##TODO## Introduce a new interface for classes derived from this class, which
//provides a high-performance alternative to the Read/Write Transparent functions
//which specifically provide memory access for the debugger. The functions should
//work with arrays of data rather than individual values. In our TimedRAM class, we
//will implement the functions to work on the committed state rather than building
//information on the latest state. Actually, I'm not sure this will work, because
//isn't the "committed" state for a TimedRAM buffer defined as the baseline which
//has been advanced to, which is governed by our render thread?
class MemoryRead :public Device
{
public:
	//Constructors
	MemoryRead(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~MemoryRead();

	//Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);

	//Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const = 0;
	unsigned int GetMemoryEntryCount() const;
	void SetMemoryEntryCount(unsigned int amemoryEntryCount);

	//Window functions
	void CreateMenuHandlers();
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher);

protected:
	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsAddressLocked(unsigned int location) const;

private:
	//View and menu classes
	class DebugMenuHandler;
	class MemoryEditorViewModel;
	class MemoryViewerOldViewModel;
	class MemoryEditorOldViewModel;
	class MemoryEditorView;
	class MemoryViewerOldView;
	class MemoryEditorOldView;
	friend class MemoryEditorViewModel;
	friend class MemoryViewerOldViewModel;
	friend class MemoryEditorOldViewModel;
	friend class MemoryEditorView;
	friend class MemoryViewerOldView;
	friend class MemoryEditorOldView;

private:
	//Menu handling
	DebugMenuHandler* menuHandler;

	//Memory size
	unsigned int memoryEntryCount;
};

#endif
