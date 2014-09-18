#include "OperatorViewPresenter.h"
#include "OperatorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
OperatorViewPresenter::OperatorViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel, unsigned int achannelNo, unsigned int aoperatorNo)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{
	channelNo = achannelNo;
	operatorNo = aoperatorNo;
}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring OperatorViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Operator";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* OperatorViewPresenter::CreateView(IUIManager& uiManager)
{
	return new OperatorView(uiManager, *this, model, channelNo, operatorNo);
}

//----------------------------------------------------------------------------------------
void OperatorViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Operator functions
//----------------------------------------------------------------------------------------
void OperatorViewPresenter::SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo)
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
