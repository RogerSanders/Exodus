#include "OperatorViewModel.h"
#include "OperatorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::OperatorViewModel::OperatorViewModel(const std::wstring& amenuHandlerName, int aviewModelID, YM2612* adevice, unsigned int achannelNo, unsigned int aoperatorNo)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{
	channelNo = achannelNo;
	operatorNo = aoperatorNo;
}

//----------------------------------------------------------------------------------------
//Operator functions
//----------------------------------------------------------------------------------------
void YM2612::OperatorViewModel::SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo)
{
	channelNo = achannelNo;
	operatorNo = aoperatorNo;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		OperatorView* view = dynamic_cast<OperatorView*>(viewBase);
		if(view != 0)
		{
			view->SetTargetOperator(channelNo, operatorNo);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* YM2612::OperatorViewModel::CreateView()
{
	return new OperatorView(device, channelNo, operatorNo);
}

//----------------------------------------------------------------------------------------
void YM2612::OperatorViewModel::DeleteView(IView* aview)
{
	delete aview;
}
