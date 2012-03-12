#include "YM2612.h"
#ifndef __YM2612_OPERATORVIEWMODEL_H__
#define __YM2612_OPERATORVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class YM2612::OperatorViewModel :public ViewModelBase
{
public:
	//Constructors
	OperatorViewModel(const std::wstring& amenuHandlerName, int aviewModelID, YM2612* adevice, unsigned int achannelNo = 0, unsigned int aoperatorNo = 0);

	//Operator functions
	void SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	YM2612* device;
	unsigned int channelNo;
	unsigned int operatorNo;
};

#endif
