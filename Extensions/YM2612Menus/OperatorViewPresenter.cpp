#include "OperatorViewPresenter.h"
#include "OperatorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
OperatorViewPresenter::OperatorViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model, unsigned int channelNo, unsigned int operatorNo)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{
	_channelNo = channelNo;
	_operatorNo = operatorNo;
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
	return new OperatorView(uiManager, *this, _model, _channelNo, _operatorNo);
}

//----------------------------------------------------------------------------------------
void OperatorViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------
//Operator functions
//----------------------------------------------------------------------------------------
void OperatorViewPresenter::SetTargetOperator(unsigned int channelNo, unsigned int operatorNo)
{
	_channelNo = channelNo;
	_operatorNo = operatorNo;
	IView* viewBase = GetOpenView();
	if (viewBase != 0)
	{
		OperatorView* view = dynamic_cast<OperatorView*>(viewBase);
		if (view != 0)
		{
			view->SetTargetOperator(_channelNo, _operatorNo);
		}
	}
}
