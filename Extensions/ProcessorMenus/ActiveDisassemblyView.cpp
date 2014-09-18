#include "ActiveDisassemblyView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ActiveDisassemblyView::ActiveDisassemblyView(IUIManager& auiManager, ActiveDisassemblyViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_ACTIVEDISASSEMBLY));
	SetDialogViewType();
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
	initializedDialog = true;

	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ENABLED, model.ActiveDisassemblyEnabled()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE, model.GetActiveDisassemblyAnalyzeCode()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA, model.GetActiveDisassemblyAnalyzeData()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS, model.GetActiveDisassemblyAnalyzeCodeOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS, model.GetActiveDisassemblyAnalyzeDataOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS, model.GetActiveDisassemblyAnalyzePredictedArrays()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES, model.GetActiveDisassemblyAnalyzePredictedJumpTables()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE, model.GetActiveDisassemblyExploreCodePaths()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION, model.GetActiveDisassemblyPerformLabelSubstitution()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS, model.GetActiveDisassemblyDetectOffsets()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES, model.GetActiveDisassemblyDetectJumpTables()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA, model.GetActiveDisassemblyDetectData()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS, model.GetActiveDisassemblyDetectDataArrays()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA, model.GetActiveDisassemblyDetectTextData()? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_START, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyStartLocation());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_END, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyEndLocation());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyAnalysisStartLocation());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyAnalysisEndLocation());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE)  UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE, model.GetActiveDisassemblyOffsetArrayIncreaseTolerance());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT)  UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT, model.GetActiveDisassemblyMinimumArrayEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyOffsetArrayDistanceTolerance());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE, model.GetAddressBusCharWidth(), model.GetActiveDisassemblyJumpTableDistanceTolerance());

	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE, model.GetActiveDisassemblyRecordedItemCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE, model.GetActiveDisassemblyAnalysisCodeEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS, model.GetActiveDisassemblyAnalysisOffsetEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA, model.GetActiveDisassemblyAnalysisDataEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS, model.GetActiveDisassemblyAnalysisLabelEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE, model.GetActiveDisassemblyAnalysisPredictedCodeEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS, model.GetActiveDisassemblyAnalysisPredictedOffsetEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA, model.GetActiveDisassemblyAnalysisPredictedDataEntryCount());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS, model.GetActiveDisassemblyAnalysisPredictedLabelEntryCount());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ActiveDisassemblyView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_START:{
				unsigned int newActiveDisassemblyStartLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				unsigned int currentActiveDisassemblyStartLocation = model.GetActiveDisassemblyStartLocation();
				if(newActiveDisassemblyStartLocation != currentActiveDisassemblyStartLocation)
				{
					if(model.GetActiveDisassemblyAnalysisStartLocation() == currentActiveDisassemblyStartLocation)
					{
						model.SetActiveDisassemblyAnalysisStartLocation(newActiveDisassemblyStartLocation);
					}
					model.SetActiveDisassemblyStartLocation(newActiveDisassemblyStartLocation);
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_END:{
				unsigned int newActiveDisassemblyEndLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				unsigned int currentActiveDisassemblyEndLocation = model.GetActiveDisassemblyEndLocation();
				if(newActiveDisassemblyEndLocation != currentActiveDisassemblyEndLocation)
				{
					if(model.GetActiveDisassemblyAnalysisEndLocation() == currentActiveDisassemblyEndLocation)
					{
						model.SetActiveDisassemblyAnalysisEndLocation(newActiveDisassemblyEndLocation);
					}
					model.SetActiveDisassemblyEndLocation(newActiveDisassemblyEndLocation);
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START:
				model.SetActiveDisassemblyAnalysisStartLocation(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END:
				model.SetActiveDisassemblyAnalysisEndLocation(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT:
				model.SetActiveDisassemblyMinimumArrayEntryCount(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE:
				model.SetActiveDisassemblyOffsetArrayIncreaseTolerance(GetDlgItemDouble(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE:
				model.SetActiveDisassemblyOffsetArrayDistanceTolerance(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE:
				model.SetActiveDisassemblyJumpTableDistanceTolerance(GetDlgItemHex(hwnd, LOWORD(wparam)));
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
				model.EnableActiveDisassembly();
			}
			else
			{
				model.DisableActiveDisassembly();
			}
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_CLEAR:
			model.ClearActiveDisassembly();
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE:
			model.SetActiveDisassemblyAnalyzeCode(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA:
			model.SetActiveDisassemblyAnalyzeData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS:
			model.SetActiveDisassemblyAnalyzeCodeOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS:
			model.SetActiveDisassemblyAnalyzeDataOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS:
			model.SetActiveDisassemblyAnalyzePredictedArrays(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES:
			model.SetActiveDisassemblyAnalyzePredictedJumpTables(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE:
			model.SetActiveDisassemblyExploreCodePaths(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION:
			model.SetActiveDisassemblyPerformLabelSubstitution(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS:
			model.SetActiveDisassemblyDetectOffsets(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES:
			model.SetActiveDisassemblyDetectJumpTables(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA:
			model.SetActiveDisassemblyDetectData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS:
			model.SetActiveDisassemblyDetectDataArrays(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA:
			model.SetActiveDisassemblyDetectTextData(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZE:
			model.PerformActiveDisassemblyAnalysis();
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_CLEARANALYSIS:
			model.ClearActiveDisassemblyAnalysis();
			break;

		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOASM:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Assembly files|asm", L"asm", L"", L"", selectedFilePath))
			{
				model.ActiveDisassemblyExportAnalysisToASMFile(selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOTEXT:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Text files|txt", L"txt", L"", L"", selectedFilePath))
			{
				model.ActiveDisassemblyExportAnalysisToTextFile(selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOIDC:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"IDC script files|idc", L"idc", L"", L"", selectedFilePath))
			{
				model.ActiveDisassemblyExportAnalysisToIDCFile(selectedFilePath);
			}
			break;}
		}
	}

	return TRUE;
}
