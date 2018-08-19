#ifndef __OPERATORVIEWPRESENTER_H__
#define __OPERATORVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class OperatorViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	OperatorViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model, unsigned int channelNo = 0, unsigned int operatorNo = 0);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	//Operator functions
	void SetTargetOperator(unsigned int channelNo, unsigned int operatorNo);

private:
	YM2612Menus& _owner;
	const IDevice& _modelInstanceKey;
	IYM2612& _model;
	unsigned int _channelNo;
	unsigned int _operatorNo;
};

#endif
