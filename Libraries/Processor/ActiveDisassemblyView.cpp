#include "ActiveDisassemblyView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::ActiveDisassemblyView::ActiveDisassemblyView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Active Disassembly");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_ACTIVEDISASSEMBLY));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::ActiveDisassemblyView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
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
INT_PTR Processor::ActiveDisassemblyView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(device->activeDisassemblyEndLocation == 0)
	{
		device->activeDisassemblyEndLocation = (1 << (device->GetAddressBusWidth() - 1));
	}
	if(device->activeDisassemblyAnalysisEndLocation == 0)
	{
		device->activeDisassemblyAnalysisEndLocation = (1 << (device->GetAddressBusWidth() - 1));
	}

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ActiveDisassemblyView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ActiveDisassemblyView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	boost::mutex::scoped_lock lock(device->debugMutex);

	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ENABLED, device->activeDisassemblyEnabled? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE, device->activeDisassemblyAnalyzeCode? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA, device->activeDisassemblyAnalyzeData? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS, device->activeDisassemblyAnalyzeCodeOffsets? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS, device->activeDisassemblyAnalyzeDataOffsets? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS, device->activeDisassemblyAnalyzePredictedArrays? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES, device->activeDisassemblyAnalyzePredictedJumpTables? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE, device->activeDisassemblyExploreCodePaths? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION, device->activeDisassemblyPerformLabelSubstitution? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS, device->activeDisassemblyDetectOffsets? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES, device->activeDisassemblyDetectJumpTables? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA, device->activeDisassemblyDetectData? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS, device->activeDisassemblyDetectDataArrays? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA, device->activeDisassemblyDetectTextData? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_START, device->GetAddressBusCharWidth(), device->activeDisassemblyStartLocation);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_END, device->GetAddressBusCharWidth(), device->activeDisassemblyEndLocation);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START, device->GetAddressBusCharWidth(), device->activeDisassemblyAnalysisStartLocation);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END)    UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END, device->GetAddressBusCharWidth(), device->activeDisassemblyAnalysisEndLocation);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE)  UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE, device->activeDisassemblyOffsetArrayIncreaseTolerance);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT)  UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT, device->activeDisassemblyMinimumArrayEntryCount);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE, device->GetAddressBusCharWidth(), device->activeDisassemblyOffsetArrayDistanceTolerance);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE)  UpdateDlgItemHex(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE, device->GetAddressBusCharWidth(), device->activeDisassemblyJumpTableDistanceTolerance);

	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_SIZE, (unsigned int)device->activeDisassemblyAddressInfoSet.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDCODE, (unsigned int)device->activeDisassemblyAnalysis->disassemblyCodeSorted.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDOFFSETS, (unsigned int)device->activeDisassemblyAnalysis->disassemblyOffsetSorted.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDDATA, (unsigned int)device->activeDisassemblyAnalysis->disassemblyDataSorted.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OBSERVEDLABELTARGETS, device->activeDisassemblyAnalysis->labelSettings.observedLabelCount);
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDCODE, (unsigned int)device->activeDisassemblyAnalysis->predictedCodeEntries.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDOFFSETS, (unsigned int)device->activeDisassemblyAnalysis->predictedOffsetEntries.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDDATA, (unsigned int)device->activeDisassemblyAnalysis->predictedDataEntries.size());
	if(currentControlFocus != IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS)   UpdateDlgItemBin(hwnd, IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTEDLABELTARGETS, device->activeDisassemblyAnalysis->labelSettings.detectedLabelCount);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ActiveDisassemblyView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			boost::mutex::scoped_lock lock(device->debugMutex);
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_START:{
				unsigned int newActiveDisassemblyStartLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				if(newActiveDisassemblyStartLocation != device->activeDisassemblyStartLocation)
				{
					bool updateAnalysisLocation = (device->activeDisassemblyAnalysisStartLocation == device->activeDisassemblyStartLocation);
					if(device->ActiveDisassemblyEnabled())
					{
						device->DisableActiveDisassembly();
						device->EnableActiveDisassembly(newActiveDisassemblyStartLocation, device->activeDisassemblyEndLocation);
					}
					else
					{
						device->activeDisassemblyStartLocation = newActiveDisassemblyStartLocation;
					}
					if(updateAnalysisLocation)
					{
						device->activeDisassemblyAnalysisStartLocation = newActiveDisassemblyStartLocation;
					}
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_END:{
				unsigned int newActiveDisassemblyEndLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				if(newActiveDisassemblyEndLocation != device->activeDisassemblyEndLocation)
				{
					bool updateAnalysisLocation = (device->activeDisassemblyAnalysisEndLocation == device->activeDisassemblyEndLocation);
					if(device->ActiveDisassemblyEnabled())
					{
						device->DisableActiveDisassembly();
						device->EnableActiveDisassembly(device->activeDisassemblyStartLocation, newActiveDisassemblyEndLocation);
					}
					else
					{
						device->activeDisassemblyEndLocation = newActiveDisassemblyEndLocation;
					}
					if(updateAnalysisLocation)
					{
						device->activeDisassemblyAnalysisEndLocation = newActiveDisassemblyEndLocation;
					}
				}
				break;}
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_START:
				device->activeDisassemblyAnalysisStartLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_END:
				device->activeDisassemblyAnalysisEndLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_MINIMUMARRAYENTRYCOUNT:
				device->activeDisassemblyMinimumArrayEntryCount = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYINCREASETOLERANCE:
				device->activeDisassemblyOffsetArrayIncreaseTolerance = GetDlgItemDouble(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_OFFSETARRAYTOLERANCE:
				device->activeDisassemblyOffsetArrayDistanceTolerance = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_JUMPTABLETOLERANCE:
				device->activeDisassemblyJumpTableDistanceTolerance = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ENABLED:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			if(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED)
			{
				device->EnableActiveDisassembly(device->activeDisassemblyStartLocation, device->activeDisassemblyEndLocation);
			}
			else
			{
				device->DisableActiveDisassembly();
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_CLEAR:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->ClearActiveDisassembly();
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODE:
			device->activeDisassemblyAnalyzeCode = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATA:
			device->activeDisassemblyAnalyzeData = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZECODEOFFSETS:
			device->activeDisassemblyAnalyzeCodeOffsets = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEDATAOFFSETS:
			device->activeDisassemblyAnalyzeDataOffsets = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEARRAYS:
			device->activeDisassemblyAnalyzePredictedArrays = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZEJUMPTABLES:
			device->activeDisassemblyAnalyzePredictedJumpTables = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTCODE:
			device->activeDisassemblyExploreCodePaths = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_PERFORMLABELSUBSTITUTION:
			device->activeDisassemblyPerformLabelSubstitution = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTOFFSETARRAYS:
			device->activeDisassemblyDetectOffsets = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTJUMPTABLES:
			device->activeDisassemblyDetectJumpTables = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATA:
			device->activeDisassemblyDetectData = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTDATAARRAYS:
			device->activeDisassemblyDetectDataArrays = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_DETECTTEXTDATA:
			device->activeDisassemblyDetectTextData = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_ANALYZE:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->activeDisassemblyAnalysis->Initialize();
			device->PerformActiveDisassemblyAnalysis(device->activeDisassemblyAnalysisStartLocation, device->activeDisassemblyAnalysisEndLocation, *device->activeDisassemblyAnalysis);
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_CLEARANALYSIS:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->activeDisassemblyAnalysis->Initialize();
			break;}

		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOASM:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Assembly files|asm", L"asm", L"", L"", selectedFilePath))
			{
				boost::mutex::scoped_lock lock(device->debugMutex);
				device->ActiveDisassemblyExportAnalysisToASMFile(*device->activeDisassemblyAnalysis, selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOTEXT:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"Text files|txt", L"txt", L"", L"", selectedFilePath))
			{
				boost::mutex::scoped_lock lock(device->debugMutex);
				device->ActiveDisassemblyExportAnalysisToTextFile(*device->activeDisassemblyAnalysis, selectedFilePath);
			}
			break;}
		case IDC_PROCESSOR_ACTIVEDISASSEMBLY_ANALYSIS_EXPORTTOIDC:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"IDC script files|idc", L"idc", L"", L"", selectedFilePath))
			{
				boost::mutex::scoped_lock lock(device->debugMutex);
				device->ActiveDisassemblyExportAnalysisToIDCFile(*device->activeDisassemblyAnalysis, selectedFilePath);
			}
			break;}
		}
	}

	return TRUE;
}
