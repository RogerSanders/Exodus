#include "SN76489.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void SN76489::CreateMenuHandlers()
{
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
}

//----------------------------------------------------------------------------------------
void SN76489::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void SN76489::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void SN76489::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void SN76489::LockRegister(LockedRegisterKey key, const boost::function<void()>& function)
{
	lockedRegisterIndex.insert(key);
	lockedRegisters.insert(LockedRegisterListEntry(key, function));
}

//----------------------------------------------------------------------------------------
void SN76489::UnlockRegister(LockedRegisterKey key)
{
	lockedRegisterIndex.erase(key);
	lockedRegisters.erase(key);
}

//----------------------------------------------------------------------------------------
bool SN76489::IsRegisterLocked(LockedRegisterKey key)
{
	return (lockedRegisterIndex.find(key) != lockedRegisterIndex.end());
}
