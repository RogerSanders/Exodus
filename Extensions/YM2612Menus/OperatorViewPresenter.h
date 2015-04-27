#ifndef __OPERATORVIEWPRESENTER_H__
#define __OPERATORVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class OperatorViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	OperatorViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel, unsigned int achannelNo = 0, unsigned int aoperatorNo = 0);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Operator functions
	void SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo);

private:
	YM2612Menus& owner;
	const IDevice& modelInstanceKey;
	IYM2612& model;
	unsigned int channelNo;
	unsigned int operatorNo;
};

#endif
