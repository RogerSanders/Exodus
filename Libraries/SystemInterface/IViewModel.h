#ifndef __IVIEWMODEL_H__
#define __IVIEWMODEL_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "IViewModelNotifier.h"
#include <string>
class IView;

class IViewModel
{
public:
	//Constructors
	virtual ~IViewModel() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIViewModelVersion();
	virtual unsigned int GetIViewModelVersion() const = 0;

	//View management functions
	virtual bool OpenView(IViewModelNotifier* anotifier, void* aparentWindow, int xpos, int ypos) = 0;
	virtual void CloseView() = 0;
	virtual void ShowView() = 0;
	virtual void HideView() = 0;
	virtual void ActivateView() = 0;
	virtual void NotifyViewClosed(IView* aview) = 0;

	//View owner functions
	virtual bool IsViewOwnerDevice() const = 0;
	virtual bool IsViewOwnerSystem() const = 0;
	inline std::wstring GetViewOwnerDeviceInstanceName() const;
	inline void SetViewOwnerAsDevice(const std::wstring& deviceInstanceName, unsigned int moduleID);
	virtual void SetViewOwnerAsSystem() = 0;
	virtual unsigned int GetViewOwnerModuleID() const = 0;

	//Title functions
	inline std::wstring GetViewTitle() const;

	//Position and size functions
	virtual bool CanResizeView() const = 0;
	virtual void GetViewPosition(int& xpos, int& ypos) const = 0;
	virtual void GetViewSize(int& sizex, int& sizey) const = 0;
	virtual void SetViewPosition(int xpos, int ypos) = 0;
	virtual void SetViewSize(int sizex, int sizey) = 0;

	//State functions
	virtual int GetViewID() const = 0;
	inline std::wstring GetViewModelGroupName() const;
	inline std::wstring GetViewModelName() const;
	virtual bool SaveViewLayoutState(IHeirarchicalStorageNode& node) const = 0;
	virtual bool LoadViewLayoutState(IHeirarchicalStorageNode& node) = 0;
	virtual bool DoesWindowHandleMatchView(void* awindowHandle) const = 0;

protected:
	//View owner functions
	virtual void SetViewOwnerAsDeviceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& deviceInstanceNameMarshaller, unsigned int moduleID) = 0;
	virtual void GetViewOwnerDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Title functions
	virtual void GetViewTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//State functions
	virtual void GetViewModelGroupNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetViewModelNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IViewModel.inl"
#endif
