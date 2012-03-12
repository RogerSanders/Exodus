#include "SN76489.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void SN76489::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void SN76489::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void SN76489::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	menuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
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
