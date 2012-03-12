#include "TraceView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::TraceView::TraceView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Trace");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_TRACE));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::TraceView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(device->debugMutex);

	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_ENABLED, (device->GetTraceEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_DISASSEMBLE, (device->traceDisassemble)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_COVERAGE_ENABLED, (device->GetTraceCoverageEnabled())? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_LENGTH, device->GetTraceLength());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_TRACE_START, device->GetAddressBusCharWidth(), device->GetTraceCoverageStart());
	if(device->GetTraceCoverageEnd() == 0)
	{
		device->SetTraceCoverageEnd((1 << device->GetAddressBusWidth()) - 1);
	}
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_TRACE_END, device->GetAddressBusCharWidth(), device->GetTraceCoverageEnd());
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_COVERAGE_SIZE, device->GetTraceCoverageLogSize());

	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	boost::mutex::scoped_lock lock(device->debugMutex);

	if(currentControlFocus != IDC_PROCESSOR_TRACE_START)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_TRACE_START, device->GetAddressBusCharWidth(), device->GetTraceCoverageStart());
	if(currentControlFocus != IDC_PROCESSOR_TRACE_END)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_TRACE_END, device->GetAddressBusCharWidth(), device->GetTraceCoverageEnd());
	if(currentControlFocus != IDC_PROCESSOR_TRACE_COVERAGE_SIZE)	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_COVERAGE_SIZE, device->GetTraceCoverageLogSize());

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_RESETCONTENT, 0, NULL);
	unsigned int width = device->GetAddressBusCharWidth();
	for(Processor::Trace::const_iterator i = device->trace.begin(); i != device->trace.end(); ++i)
	{
		std::wstringstream buffer;
		buffer << std::setw(width) << std::setfill(L'0') << std::hex << std::uppercase << i->address << L'\t' << i->disassembly;
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_ADDSTRING, 0, (LPARAM)buffer.str().c_str());
	}
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if(HIWORD(wparam) == EN_KILLFOCUS)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			boost::mutex::scoped_lock lock(device->debugMutex);
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_TRACE_LENGTH:
				device->SetTraceLength(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_TRACE_START:
				device->SetTraceCoverageStart(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_TRACE_END:
				device->SetTraceCoverageEnd(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_TRACE_ENABLED:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->SetTraceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_DISASSEMBLE:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->traceDisassemble = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_CLEAR:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->ClearTraceLog();
			break;}
		case IDC_PROCESSOR_TRACE_COVERAGE_ENABLED:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->SetTraceCoverageEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_COVERAGE_CLEAR:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->ClearTraceCoverageLog();
			break;}
		case IDC_PROCESSOR_TRACE_COVERAGE_SAVE:{
			//Get filename
			TCHAR fileName[MAX_PATH];
			OPENFILENAME openFileParams;
			ZeroMemory(&openFileParams, sizeof(openFileParams));
			openFileParams.lStructSize = sizeof(openFileParams);
			openFileParams.hwndOwner = hwnd;
			openFileParams.lpstrFile = fileName;

			openFileParams.lpstrFile[0] = '\0';
			openFileParams.nMaxFile = sizeof(fileName);
			openFileParams.lpstrFilter = L"Text files (*.txt)\0*.txt\0All (*.*)\0*.*\0";
			openFileParams.lpstrDefExt = L"txt";
			openFileParams.nFilterIndex = 1;
			openFileParams.lpstrFileTitle = NULL;
			openFileParams.nMaxFileTitle = 0;
			openFileParams.lpstrInitialDir = NULL;
			openFileParams.Flags = 0;

			if(GetSaveFileName(&openFileParams) != 0)
			{
				boost::mutex::scoped_lock lock(device->debugMutex);

				Stream::File logFile;
				logFile.Open(openFileParams.lpstrFile, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE);
				if(logFile.IsOpen())
				{
					Stream::ViewText logFileView(logFile);
					std::set<unsigned int> traceCoverageLog;
					traceCoverageLog = device->GetTraceCoverageLog();
					for(std::set<unsigned int>::const_iterator i = traceCoverageLog.begin(); i != traceCoverageLog.end(); ++i)
					{
						logFileView << "0x" << std::hex << std::uppercase << *i << '\n';
					}

					//##DEBUG##
					std::map<unsigned int, DisassemblyAddressInfo*> disassemblyCodeSorted;
					std::map<unsigned int, DisassemblyAddressInfo*> disassemblyOffsetSorted;
					for(std::set<DisassemblyAddressInfo*>::const_iterator i = device->disassemblyAddressInfoSet.begin(); i != device->disassemblyAddressInfoSet.end(); ++i)
					{
						DisassemblyAddressInfo* entry = *i;
						if(entry->dataType == DISASSEMBLYDATATYPE_CODE)
						{
							disassemblyCodeSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(entry->baseMemoryAddress, entry));
						}
						else if((entry->dataType == DISASSEMBLYDATATYPE_OFFSETCODE) && !entry->conflictsWithKnownCode)
						{
							disassemblyOffsetSorted.insert(std::pair<unsigned int, DisassemblyAddressInfo*>(entry->baseMemoryAddress, entry));
						}
					}
					//logFileView << "Data:\n";
					//for(std::set<DisassemblyAddressInfo*>::const_iterator i = device->disassemblyAddressInfoSet.begin(); i != device->disassemblyAddressInfoSet.end(); ++i)
					//{
					//	DisassemblyAddressInfo* entry = *i;
					//	if((entry->dataType == DISASSEMBLYDATATYPE_DATA) && !entry->conflictsWithKnownCode)
					//	{
					//		logFileView << "0x" << std::hex << std::uppercase << entry->baseMemoryAddress << '\t' << entry->memoryBlockSize << '\n';
					//	}
					//}
					logFileView << "Disassembly:\n";
					for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyCodeSorted.begin(); i != disassemblyCodeSorted.end(); ++i)
					{
						logFileView << "0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << '\n';
					}
					logFileView << "Offset:\n";
					for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyOffsetSorted.begin(); i != disassemblyOffsetSorted.end(); ++i)
					{
						logFileView << "0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << '\t' << "0x" << std::hex << std::uppercase << i->second->memoryBlockSize << '\t' << i->second->relativeOffset << '\t' << "0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << '\n';
					}

					//##TODO## Try and guess array bounds
					//##TODO## Write a python script to perform this analysis, and hook it
					//into our program. This will allow user-defined code analysis rules,
					//where the rules can be
					//Rules for array detection:
					//-If we have two or more offset values sharing the same base address
					//that are located within 0x40 bytes from the base, or we have a
					//single offset directly at the base, consider that we've found a
					//possible offset array, and start intelligent prediction of the array
					//bounds.
					//-For intelligent array bounds prediction, try and guess the stride
					//between values. It's possible we have an array of structures here,
					//where there is an offset, then an unknown number of values between
					//them. For stride prediction, we need at least four values decoded in
					//the array. If we don't have this, consider the array to be tightly
					//packed. If we have four values, look at the three stride values
					//between the offsets. If they have a common base factor, and the
					//alignment appears correct, consider that to be the stride between
					//the values of the array.
					//-Try and guess the end of the array. For this, calculate the highest
					//and lowest observed value in the array. For an offset array,
					//consider the lowest observed value to be a likely array end.
					//Calculate an average difference between each successive value as
					//"(highestValue - lowestValue) / (highestIndex - lowestIndex)". Try
					//and step past the end of the array. If we hit a known code or offset
					//location, stop. For each entry we try and step past, if its value is
					//less than averageDifference*2, and for an offset array, if the value
					//is less than the first calculated offset end point, consider it to
					//be a part of the array.
					logFileView << "//####################################################################\n";
					logFileView << "#include <idc.idc>\n";
					logFileView << "static main()\n";
					logFileView << "{\n";
					for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyCodeSorted.begin(); i != disassemblyCodeSorted.end(); ++i)
					{
						logFileView << "MakeCode(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ");\n";
					}
					for(std::map<unsigned int, DisassemblyAddressInfo*>::const_iterator i = disassemblyOffsetSorted.begin(); i != disassemblyOffsetSorted.end(); ++i)
					{
						if(!i->second->relativeOffset)
						{
							if(i->second->memoryBlockSize == 4)
							{
								logFileView << "MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", FF_DWRD, 4, 0);\n";
								logFileView << "OpOff(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", -1, 0x0);\n";
							}
						}
						else
						{
							if(i->second->memoryBlockSize == 2)
							{
								logFileView << "MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", FF_WORD, 2, 0);\n";
								logFileView << "OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", -1, REF_OFF32, 0xFFFFFFFF, 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << ", 0);\n";
							}
							else if(i->second->memoryBlockSize == 4)
							{
								logFileView << "MakeData(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", FF_DWRD, 4, 0);\n";
								logFileView << "OpOffEx(0x" << std::hex << std::uppercase << i->second->baseMemoryAddress << ", -1, REF_OFF32, 0xFFFFFFFF, 0x" << std::hex << std::uppercase << i->second->relativeOffsetBaseAddress << ", 0);\n";
							}
						}
					}
					logFileView << "}\n";
				}
				logFile.Close();
			}
			break;}
		}
	}

	return TRUE;
}
