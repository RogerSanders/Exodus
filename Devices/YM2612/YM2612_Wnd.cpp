#include "YM2612.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void YM2612::CreateMenuHandlers()
{
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
}

//----------------------------------------------------------------------------------------
void YM2612::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void YM2612::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void YM2612::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void YM2612::OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo)
{
	menuHandler->OpenOperatorView(achannelNo, aoperatorNo);
}

//----------------------------------------------------------------------------------------
//Register locking
//----------------------------------------------------------------------------------------
void YM2612::LockRegister(LockedRegisterKey key, const boost::function<void()>& function)
{
	lockedRegisterIndex.insert(key);
	lockedRegisters.insert(LockedRegisterListEntry(key, function));
}

//----------------------------------------------------------------------------------------
void YM2612::UnlockRegister(LockedRegisterKey key)
{
	lockedRegisterIndex.erase(key);
	lockedRegisters.erase(key);
}

//----------------------------------------------------------------------------------------
bool YM2612::IsRegisterLocked(LockedRegisterKey key)
{
	return (lockedRegisterIndex.find(key) != lockedRegisterIndex.end());
}

//----------------------------------------------------------------------------------------
bool YM2612::GetKeyStateLocked(unsigned int channelNo, unsigned int operatorNo)
{
	return keyStateLocking[channelNo][operatorNo];
}

//----------------------------------------------------------------------------------------
void YM2612::SetKeyStateLocked(unsigned int channelNo, unsigned int operatorNo, bool state)
{
	keyStateLocking[channelNo][operatorNo] = state;
}

//----------------------------------------------------------------------------------------
bool YM2612::GetTimerParamLocked(TimerParam param)
{
	switch(param)
	{
	case TIMERPARAMA_LOAD:
		return timerAStateLocking.load;
	case TIMERPARAMA_ENABLE:
		return timerAStateLocking.enable;
	case TIMERPARAMA_OVERFLOW:
		return timerAStateLocking.overflow;
	case TIMERPARAMA_RATE:
		return timerAStateLocking.rate;
	case TIMERPARAMA_COUNTER:
		return timerAStateLocking.counter;
	case TIMERPARAMB_LOAD:
		return timerBStateLocking.load;
	case TIMERPARAMB_ENABLE:
		return timerBStateLocking.enable;
	case TIMERPARAMB_OVERFLOW:
		return timerBStateLocking.overflow;
	case TIMERPARAMB_RATE:
		return timerBStateLocking.rate;
	case TIMERPARAMB_COUNTER:
		return timerBStateLocking.counter;
	}
	return false;
}

//----------------------------------------------------------------------------------------
void YM2612::SetTimerParamLocked(TimerParam param, bool state)
{
	switch(param)
	{
	case TIMERPARAMA_LOAD:
		timerAStateLocking.load = state;
		break;
	case TIMERPARAMA_ENABLE:
		timerAStateLocking.enable = state;
		break;
	case TIMERPARAMA_OVERFLOW:
		timerAStateLocking.overflow = state;
		break;
	case TIMERPARAMA_RATE:
		timerAStateLocking.rate = state;
		break;
	case TIMERPARAMA_COUNTER:
		timerAStateLocking.counter = state;
		break;
	case TIMERPARAMB_LOAD:
		timerBStateLocking.load = state;
		break;
	case TIMERPARAMB_ENABLE:
		timerBStateLocking.enable = state;
		break;
	case TIMERPARAMB_OVERFLOW:
		timerBStateLocking.overflow = state;
		break;
	case TIMERPARAMB_RATE:
		timerBStateLocking.rate = state;
		break;
	case TIMERPARAMB_COUNTER:
		timerBStateLocking.counter = state;
		break;
	}
}
