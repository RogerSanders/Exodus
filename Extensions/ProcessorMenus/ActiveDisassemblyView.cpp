#include "ActiveDisassemblyView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ActiveDisassemblyView::ActiveDisassemblyView(IUIManager& uiManager, ActiveDisassemblyViewPresenter& presenter, IProcessor& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_ACTIVEDISASSEMBLY));
	SetDockableViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 200, NULL);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	_initializedDialog = true;

	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ENABLED, _model.ActiveDisassemblyEnabled()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE, _model.GetActiveDisassemblyAnalyzeCode()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA, _model.GetActiveDisassemblyAnalyzeData()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS, _model.GetActiveDisassemblyAnalyzeCodeOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS, _model.GetActiveDisassemblyAnalyzeDataOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS, _model.GetActiveDisassemblyAnalyzePredictedArrays()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES, _model.GetActiveDisassemblyAnalyzePredictedJumpTables()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE, _model.GetActiveDisassemblyExploreCodePaths()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION, _model.GetActiveDisassemblyPerformLabelSubstitution()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS, _model.GetActiveDisassemblyDetectOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES, _model.GetActiveDisassemblyDetectJumpTables()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA, _model.GetActiveDisassemblyDetectData()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS, _model.GetActiveDisassemblyDetectDataArrays()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA, _model.GetActiveDisassemblyDetectTextData()? BST_CHECKED: BST_UNCHECKED);
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_START, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyStartLocation());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_END, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyEndLocation());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyAnalysisStartLocation());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyAnalysisEndLocation());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE)  UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE, _model.GetActiveDisassemblyOffsetArrayIncreaseTolerance());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT)  UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT, _model.GetActiveDisassemblyMinimumArrayEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyOffsetArrayDistanceTolerance());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE, _model.GetAddressBusCharWidth(), _model.GetActiveDisassemblyJumpTableDistanceTolerance());

	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE, _model.GetActiveDisassemblyRecordedItemCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE, _model.GetActiveDisassemblyAnalysisCodeEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS, _model.GetActiveDisassemblyAnalysisOffsetEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA, _model.GetActiveDisassemblyAnalysisDataEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS, _model.GetActiveDisassemblyAnalysisLabelEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE, _model.GetActiveDisassemblyAnalysisPredictedCodeEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS, _model.GetActiveDisassemblyAnalysisPredictedOffsetEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA, _model.GetActiveDisassemblyAnalysisPredictedDataEntryCount());
	if(_currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS, _model.GetActiveDisassemblyAnalysisPredictedLabelEntryCount());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && _initializedDialog)
	{
		_previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		_currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && _initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != _previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_START:{
				unsigned int newActiveDisassemblyStartLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				unsigned int currentActiveDisassemblyStartLocation = _model.GetActiveDisassemblyStartLocation();
				if(newActiveDisassemblyStartLocation != currentActiveDisassemblyStartLocation)
				{
					if(_model.GetActiveDisassemblyAnalysisStartLocation() == currentActiveDisassemblyStartLocation)
					{
						_model.SetActiveDisassemblyAnalysisStartLocation(newActiveDisassemblyStartLocation);
					}
					_model.SetActiveDisassemblyStartLocation(newActiveDisassemblyStartLocation);
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_END:{
				unsigned int newActiveDisassemblyEndLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				unsigned int currentActiveDisassemblyEndLocation = _model.GetActiveDisassemblyEndLocation();
				if(newActiveDisassemblyEndLocation != currentActiveDisassemblyEndLocation)
				{
					if(_model.GetActiveDisassemblyAnalysisEndLocation() == currentActiveDisassemblyEndLocation)
					{
						_model.SetActiveDisassemblyAnalysisEndLocation(newActiveDisassemblyEndLocation);
					}
					_model.SetActiveDisassemblyEndLocation(newActiveDisassemblyEndLocation);
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START:
				_model.SetActiveDisassemblyAnalysisStartLocation(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END:
				_model.SetActiveDisassemblyAnalysisEndLocation(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT:
				_model.SetActiveDisassemblyMinimumArrayEntryCount(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE:
				_model.SetActiveDisassemblyOffsetArrayIncreaseTolerance(GetDlgItemDouble(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE:
				_model.SetActiveDisassemblyOffsetArrayDistanceTolerance(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE:
				_model.SetActiveDisassemblyJumpTableDistanceTolerance(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ENABLED:
			if(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED)
			{
				_model.EnableActiveDisassembly();
			}
			else
			{
				_model.DisableActiveDisassembly();
			}
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_CLEAR:
			_model.ClearActiveDisassembly();
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE:
			_model.SetActiveDisassemblyAnalyzeCode(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA:
			_model.SetActiveDisassemblyAnalyzeData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS:
			_model.SetActiveDisassemblyAnalyzeCodeOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS:
			_model.SetActiveDisassemblyAnalyzeDataOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS:
			_model.SetActiveDisassemblyAnalyzePredictedArrays(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES:
			_model.SetActiveDisassemblyAnalyzePredictedJumpTables(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE:
			_model.SetActiveDisassemblyExploreCodePaths(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION:
			_model.SetActiveDisassemblyPerformLabelSubstitution(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS:
			_model.SetActiveDisassemblyDetectOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES:
			_model.SetActiveDisassemblyDetectJumpTables(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA:
			_model.SetActiveDisassemblyDetectData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS:
			_model.SetActiveDisassemblyDetectDataArrays(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA:
			_model.SetActiveDisassemblyDetectTextData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZE:
			_model.PerformActiveDisassemblyAnalysis();
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_CLEARANALYSIS:
			_model.ClearActiveDisassemblyAnalysis();
			break;

		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOASM:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Assembly files|asm", L"asm", L"", L"", selectedFilePath))
			{
				_model.ActiveDisassemblyExportAnalysisToASMFile(selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOTEXT:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Text files|txt", L"txt", L"", L"", selectedFilePath))
			{
				_model.ActiveDisassemblyExportAnalysisToTextFile(selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOIDC:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"IDC script files|idc", L"idc", L"", L"", selectedFilePath))
			{
				_model.ActiveDisassemblyExportAnalysisToIDCFile(selectedFilePath);
			}
			break;}
		}
	}

	return TRUE;
}
