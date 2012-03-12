#ifndef __IVIEW_H__
#define __IVIEW_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
class IViewModel;

class IView
{
public:
	//Constructors
	virtual ~IView() = 0 {}

	//View management functions
	virtual bool OpenView(void* aparentWindow, IViewModel* aviewModel, int xpos, int ypos) = 0;
	virtual void CloseView() = 0;
	virtual void ShowView() = 0;
	virtual void HideView() = 0;
	virtual void ActivateView() = 0;

	//Title functions
	inline std::wstring GetViewTitle() const;

	//Position and size functions
	virtual bool CanResizeView() const = 0;
	virtual void GetViewPosition(int& xpos, int& ypos) const = 0;
	virtual void GetViewSize(int& sizex, int& sizey) const = 0;
	virtual void SetViewPosition(int xpos, int ypos) = 0;
	virtual void SetViewSize(int sizex, int sizey) = 0;

	//State functions
	virtual bool SaveViewLayoutState(IHeirarchicalStorageNode& node) const = 0;
	virtual bool LoadViewLayoutState(IHeirarchicalStorageNode& node) = 0;
	virtual bool DoesWindowHandleMatchView(void* awindowHandle) const = 0;

protected:
	//Title functions
	virtual const wchar_t* GetViewTitleInternal() const = 0;
};

#include "IView.inl"
#endif
