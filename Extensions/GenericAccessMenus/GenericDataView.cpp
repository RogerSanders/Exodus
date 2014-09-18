#include "GenericDataView.h"
#include "WindowsControls/WindowsControls.pkg"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"
#include <sstream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericDataView::GenericDataView(IUIManager& auiManager, GenericDataViewPresenter& apresenter, IGenericAccess& amodel, const IGenericAccessPage* apage)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), page(apage)
{
	SetWindowSettings(page->GetName(), 0, 0, 500, 300);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT GenericDataView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		return msgWM_PAINT(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_SETFOCUS:
		return msgWM_SETFOCUS(hwnd, wparam, lparam);
	case WM_KILLFOCUS:
		return msgWM_KILLFOCUS(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the default font for this window
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	valueFont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the DataGrid child control
	hwndDataList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)DATALISTCONTROL, GetAssemblyHandle(), NULL);

	//Set the default font for the child controls
	SendMessage(hwndDataList, WM_SETFONT, (WPARAM)valueFont, TRUE);

	//Insert our columns into the device DataGrid control
	WC_DataGrid::Grid_InsertColumn nameColumn(L"Name", NAMECOLUMNID);
	WC_DataGrid::Grid_InsertColumn valueColumn(L"Value", VALUECOLUMNID, true);
	valueColumn.sizeMode = WC_DataGrid::COLUMNSIZEMODE_PROPORTIONAL;
	SendMessage(hwndDataList, WC_DataGrid::GRID_INSERTCOLUMN, 0, (LPARAM)&nameColumn);
	SendMessage(hwndDataList, WC_DataGrid::GRID_INSERTCOLUMN, 0, (LPARAM)&valueColumn);

	//Check if the root node contains any child groups
	const IGenericAccessGroup* rootNode = page->GetContentRoot();
	bool rootNodeContainsChildGroups = false;
	std::list<IGenericAccessGroupEntry*> rootNodeEntries = rootNode->GetEntries();
	for(std::list<IGenericAccessGroupEntry*>::const_iterator i = rootNodeEntries.begin(); i != rootNodeEntries.end(); ++i)
	{
		IGenericAccessGroup::GroupEntryType groupEntryType = (*i)->GetGroupEntryType();
		rootNodeContainsChildGroups |= (groupEntryType == IGenericAccessGroup::GROUPENTRYTYPE_GROUP) || (groupEntryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION);
	}

	//Set the initial state of the data grid
	unsigned int initialIndentLevel = (rootNodeContainsChildGroups)? 1: 0;
	unsigned int currentRow = 0;
	std::list<ExpandStateInfo> preservedExpandState;
	std::list<ParentCollectionInfo> parentCollectionInfo;
	for(std::list<IGenericAccessGroupEntry*>::const_iterator i = rootNodeEntries.begin(); i != rootNodeEntries.end(); ++i)
	{
		PopulateDataGrid(*i, currentRow, initialIndentLevel, false, false, false, 0, preservedExpandState, parentCollectionInfo);
	}

	ShowWindow(hwndDataList, SW_SHOWNORMAL);
	UpdateWindow(hwndDataList);
	SetFocus(hwndDataList);

	SetTimer(hwnd, 1, 50, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Update the contents of the data grid
	unsigned int currentRow = 0;
	while(currentRow < (unsigned int)rowInfo.size())
	{
		UpdateDataGrid(currentRow, true);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Dimensions and relative positions of the controls
	unsigned int dataListControlLeftOffset = 0;
	unsigned int dataListControlRightOffset = dataListControlLeftOffset;
	unsigned int dataListControlTopOffset = 0;
	unsigned int dataListControlBottomOffset = 0;

	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	unsigned int controlWidth = rect.right;
	unsigned int controlHeight = rect.bottom;

	//Resize the HexEdit control to match
	MoveWindow(hwndDataList, dataListControlLeftOffset, dataListControlTopOffset, controlWidth - dataListControlLeftOffset - dataListControlRightOffset, controlHeight - dataListControlTopOffset - dataListControlBottomOffset, TRUE);

	//##TODO## Autosize the columns to fit to the available space

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Fill the background of the control with the dialog background colour
	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, hbrush);

	SelectObject(hdc, hbrushOld);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);

	return DefWindowProc(hwnd, WM_PAINT, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == DATALISTCONTROL)
	{
		if(HIWORD(wparam) == WC_DataGrid::GRID_CELLEDIT)
		{
			WC_DataGrid::Grid_CellEditEvent* cellEditEventInfo = (WC_DataGrid::Grid_CellEditEvent*)lparam;
			if((cellEditEventInfo->targetColumnID == VALUECOLUMNID) && (cellEditEventInfo->targetRowNo < rowInfo.size()) && LockTargetRowEntry(cellEditEventInfo->targetRowNo))
			{
				const GridRowInfo& targetRowInfo = rowInfo[cellEditEventInfo->targetRowNo];
				if((targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA) || (targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_SINGLESELECTIONLIST))
				{
					//Obtain the data ID and data context for the edited data value, and
					//determine the actual value to apply as the new value for this data
					//item.
					unsigned int dataID;
					const IGenericAccess::DataContext* dataContext;
					std::wstring newData = cellEditEventInfo->newData;
					if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA)
					{
						const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(targetRowInfo.entry);
						dataID = dataEntry->GetDataID();
						dataContext = dataEntry->GetDataContext();
					}
					else
					{
						const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(targetRowInfo.entry);
						dataID = selectionListEntry->GetDataID();
						dataContext = selectionListEntry->GetDataContext();

						//Attempt to map the new data value for this selection list item
						//from the display name to the associated backing data value.
						std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> selectionList = selectionListEntry->GetSelectionList();
						std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator selectionListIterator = selectionList.begin();
						bool convertedValue = false;
						while(!convertedValue && (selectionListIterator != selectionList.end()))
						{
							if(newData == selectionListIterator->first->GetValueString())
							{
								newData = selectionListIterator->second->GetValueString();
								convertedValue = true;
								continue;
							}
							++selectionListIterator;
						}
					}

					//Write the edited data to the device
					model.WriteGenericData(dataID, dataContext, newData);

					//Now that the edited data has been written to the device, read it
					//back again and send it back to the edited cell. We do this because
					//the cell doesn't change itself when an edit occurs, it merely
					//requests the edit to be processed. Additionally, the device may
					//restrict or alter the input value, meaning the final value will be
					//different to what the user entered. Sending the final value back
					//immediately here ensures that the control will immediately change to
					//reflect the final value, and won't flicker briefly back to the
					//original value before the edit occurred.
					WC_DataGrid::Grid_UpdateCellText updateCellTextParams;
					updateCellTextParams.columnID = cellEditEventInfo->targetColumnID;
					updateCellTextParams.rowNo = cellEditEventInfo->targetRowNo;
					if(model.ReadGenericData(dataID, dataContext, updateCellTextParams.newText))
					{
						//If this entry has a selection list, map the new value back from
						//a literal value to its corresponding selection list entry.
						if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_SINGLESELECTIONLIST)
						{
							const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(targetRowInfo.entry);
							std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> selectionList = selectionListEntry->GetSelectionList();
							std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator selectionListIterator = selectionList.begin();
							bool convertedValue = false;
							while(!convertedValue && (selectionListIterator != selectionList.end()))
							{
								if(updateCellTextParams.newText == selectionListIterator->second->GetValueString())
								{
									updateCellTextParams.newText = selectionListIterator->first->GetValueString();
									convertedValue = true;
									continue;
								}
								++selectionListIterator;
							}
						}

						//Update the data value for this cell
						SendMessage(hwndDataList, WC_DataGrid::GRID_UPDATECELLTEXT, 0, (LPARAM)&updateCellTextParams);
					}
				}

				//Unlock the target row entry now that we're finished accessing its data
				UnlockTargetRowEntry(cellEditEventInfo->targetRowNo);
			}
		}
		else if(HIWORD(wparam) == WC_DataGrid::GRID_CELLBUTTONCLICK)
		{
			WC_DataGrid::Grid_CellButtonClickEvent* cellButtonClickEventInfo = (WC_DataGrid::Grid_CellButtonClickEvent*)lparam;
			if((cellButtonClickEventInfo->targetColumnID == VALUECOLUMNID) && (cellButtonClickEventInfo->targetRowNo < rowInfo.size()) && LockTargetRowEntry(cellButtonClickEventInfo->targetRowNo))
			{
				const GridRowInfo& targetRowInfo = rowInfo[cellButtonClickEventInfo->targetRowNo];
				if(targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_COMMAND)
				{
					//Execute the specified command on the device
					const IGenericAccessGroupCommandEntry* commandEntry = static_cast<const IGenericAccessGroupCommandEntry*>(targetRowInfo.entry);
					model.ExecuteGenericCommand(commandEntry->GetCommandID(), commandEntry->GetDataContext());
				}
				else if(targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_DATA)
				{
					//Open a file or folder browse dialog
					const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(targetRowInfo.entry);
					const IGenericAccessDataInfo* dataInfo = model.GetGenericDataInfo(dataEntry->GetDataID());
					IGenericAccessDataValue::DataType dataType = dataInfo->GetType();
					std::wstring dataValueAsString;
					if(model.ReadGenericData(dataEntry->GetDataID(), dataEntry->GetDataContext(), dataValueAsString))
					{
						if(dataType == IGenericAccessDataValue::DATATYPE_FILEPATH)
						{
							if(presenter.SelectFile(dataValueAsString, !dataInfo->GetFilePathCreatingTarget(), dataInfo->GetFilePathExtensionFilter(), dataInfo->GetFilePathDefaultExtension(), dataInfo->GetFilePathAllowScanningIntoArchives()))
							{
								model.WriteGenericData(dataEntry->GetDataID(), dataEntry->GetDataContext(), dataValueAsString);
							}
						}
						else if(dataType == IGenericAccessDataValue::DATATYPE_FOLDERPATH)
						{
							if(presenter.SelectFolder(dataValueAsString, !dataInfo->GetFolderPathCreatingTarget()))
							{
								model.WriteGenericData(dataEntry->GetDataID(), dataEntry->GetDataContext(), dataValueAsString);
							}
						}
					}
				}

				//Unlock the target row entry now that we're finished accessing its data
				UnlockTargetRowEntry(cellButtonClickEventInfo->targetRowNo);
			}
		}
		else if(HIWORD(wparam) == WC_DataGrid::GRID_SELECTIONEVENT)
		{
			WC_DataGrid::Grid_SelectionEvent* info = (WC_DataGrid::Grid_SelectionEvent*)lparam;

			//If the CTRL key was pressed when a row selection was made, toggle the locked
			//state of the data item referenced by the target row.
			//##TODO## Instead of having this ctrl+click locking behaviour, add a padlock
			//icon on the left side of the value column for controls that support locking,
			//which is defined as a custom action button that when clicked, toggles the
			//locked state of the register, and changes the padlock icon between open and
			//closed. There are padlock characters defined in the unicode standard we can
			//use. This will make the behaviour more obvious, allow the user to easily see
			//which registers support locking, and free up text colour changes to
			//represent other information, like the highlight concept we want to introduce
			//for changed register values for the M68000.
			if(info->rowSelected && info->keyPressedCtrl)
			{
				//Toggle the locked state of the target data item
				if((info->selectedRowNo < rowInfo.size()) && LockTargetRowEntry(info->selectedRowNo))
				{
					const GridRowInfo& targetRowInfo = rowInfo[info->selectedRowNo];
					if((targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_DATA) || (targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_SINGLESELECTIONLIST))
					{
						unsigned int dataID;
						const IGenericAccess::DataContext* dataContext;
						if(targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_DATA)
						{
							const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(targetRowInfo.entry);
							dataID = dataEntry->GetDataID();
							dataContext = dataEntry->GetDataContext();
						}
						else
						{
							const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(targetRowInfo.entry);
							dataID = selectionListEntry->GetDataID();
							dataContext = selectionListEntry->GetDataContext();
						}
						bool newLockedState = !model.GetGenericDataLocked(dataID, dataContext);
						model.SetGenericDataLocked(dataID, dataContext, newLockedState);
					}

					//Unlock the target row entry now that we're finished accessing its
					//data
					UnlockTargetRowEntry(info->selectedRowNo);
				}

				//Since we're handling this selection event ourselves, and we don't want a
				//grid selection to occur in response, instruct the data grid to ignore
				//the selection.
				info->ignoreSelectionEvent = true;
			}

			//Don't allow read-only cells to enter edit mode
			if(info->rowSelected && info->columnSelected)
			{
				//Don't allow group summary or read only data cells to be edited
				if((info->selectedRowNo < rowInfo.size()) && LockTargetRowEntry(info->selectedRowNo))
				{
					const GridRowInfo& targetRowInfo = rowInfo[info->selectedRowNo];
					if((targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_GROUP) || (targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_COLLECTION))
					{
						info->ignoreSelectionEvent = true;
					}
					else if((targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_DATA) || (targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_SINGLESELECTIONLIST))
					{
						unsigned int dataID;
						if(targetRowInfo.entryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_DATA)
						{
							const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(targetRowInfo.entry);
							dataID = dataEntry->GetDataID();
						}
						else
						{
							const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(targetRowInfo.entry);
							dataID = selectionListEntry->GetDataID();
						}
						const IGenericAccessDataInfo* dataInfo = model.GetGenericDataInfo(dataID);
						if(dataInfo != 0)
						{
							if(dataInfo->GetReadOnly())
							{
								info->ignoreSelectionEvent = true;
							}
						}
					}

					//Unlock the target row entry now that we're finished accessing its
					//data
					UnlockTargetRowEntry(info->selectedRowNo);
				}
			}
		}
		else if(HIWORD(wparam) == WC_DataGrid::GRID_TREEENTRYEXPANDEVENT)
		{
			WC_DataGrid::Grid_TreeEntryExpandEvent* info = (WC_DataGrid::Grid_TreeEntryExpandEvent*)lparam;

			//Toggle the expand state of the target group
			if((info->targetColumnID == NAMECOLUMNID) && (info->targetRowNo < rowInfo.size()) && LockTargetRowEntry(info->targetRowNo))
			{
				GridRowInfo* targetRowInfo = &rowInfo[info->targetRowNo];
				if(info->expand != targetRowInfo->expandState)
				{
					//Record the new expand state, adding or removing the child elements
					//of this group from the tree as required.
					if(!info->expand)
					{
						std::list<ExpandStateInfo> preservedExpandStateBuffer;
						DepopulateDataGrid(info->targetRowNo, true, preservedExpandStateBuffer);
						targetRowInfo = &rowInfo[info->targetRowNo];
						targetRowInfo->expandState = info->expand;
						targetRowInfo->preservedExpandState = preservedExpandStateBuffer;
					}
					else
					{
						targetRowInfo->expandState = info->expand;
						unsigned int currentRow = info->targetRowNo;
						std::list<ParentCollectionInfo> parentCollectionInfo = targetRowInfo->parentCollectionInfo;
						std::list<ExpandStateInfo> preservedExpandStateBuffer = targetRowInfo->preservedExpandState;
						PopulateDataGrid(targetRowInfo->entry, currentRow, targetRowInfo->nestingLevel, true, !preservedExpandStateBuffer.empty(), true, 0, preservedExpandStateBuffer, parentCollectionInfo);
						targetRowInfo = &rowInfo[info->targetRowNo];
						targetRowInfo->preservedExpandState = preservedExpandStateBuffer;
					}

					//Update the expand state of this group
					SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&(const WC_DataGrid::Grid_SetCellInfo&)WC_DataGrid::Grid_SetCellInfo(NAMECOLUMNID, info->targetRowNo, WC_DataGrid::CELLCONTROLTYPE_TREEENTRY, targetRowInfo->nestingLevel, true, targetRowInfo->expandState));
				}

				//Unlock the target row entry now that we're finished accessing its data
				UnlockTargetRowEntry(info->targetRowNo);
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_SETFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetFocus(hwndDataList);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT GenericDataView::msgWM_KILLFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(hwndDataList, WM_KILLFOCUS, NULL, 0);
	return 0;
}

//----------------------------------------------------------------------------------------
//Data update methods
//----------------------------------------------------------------------------------------
void GenericDataView::PopulateDataGrid(const IGenericAccessGroupEntry* entry, unsigned int& currentRow, unsigned int indentLevel, bool addChildrenOnly, bool usePreservedExpandState, bool preservedExpandState, unsigned int preservedExpandStateCollectionEntryCount, std::list<ExpandStateInfo>& preservedExpandStateBuffer, const std::list<ParentCollectionInfo>& parentCollectionInfo, const std::wstring& collectionEntryKey, bool useNameOverride, const std::wstring& nameOverride)
{
	//Retrieve the type of the target entry
	IGenericAccessGroupEntry::GroupEntryType entryType = entry->GetGroupEntryType();

	//Add the target entry to the data grid if required
	if(!addChildrenOnly)
	{
		//Explicitly insert this row into the data grid
		WC_DataGrid::Grid_InsertRows insertRowsInfo;
		insertRowsInfo.targetRowNo = currentRow;
		insertRowsInfo.rowCount = 1;
		SendMessage(hwndDataList, WC_DataGrid::GRID_INSERTROWS, 0, (LPARAM)&insertRowsInfo);

		//Obtain information on this entry, and setup the value cell correctly for this
		//entry type.
		bool entryHasChildren = false;
		bool childrenExpandedByDefault = false;
		unsigned int childEntryCount = 0;
		std::wstring name;
		if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_GROUP)
		{
			const IGenericAccessGroup* groupEntry = static_cast<const IGenericAccessGroup*>(entry);
			name = groupEntry->GetName();
			childEntryCount = groupEntry->GetEntryCount();
			entryHasChildren = (childEntryCount > 0);
			if(usePreservedExpandState)
			{
				childrenExpandedByDefault = preservedExpandState;
			}
			else
			{
				childrenExpandedByDefault = entryHasChildren && groupEntry->GetOpenByDefault();
			}

			//Set a gray colour for the summary text on a group
			WC_DataGrid::Grid_SetCellColor setCellColorParams;
			setCellColorParams.targetRowNo = currentRow;
			setCellColorParams.targetColumnID = VALUECOLUMNID;
			setCellColorParams.textColorDefined = true;
			setCellColorParams.colorTextFront = WinColor(96, 96, 96);
			setCellColorParams.colorTextBack = WinColor(255, 255, 255);
			SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLCOLOR, 0, (LPARAM)&setCellColorParams);
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION)
		{
			const IGenericAccessGroupCollectionEntry* collectionEntry = static_cast<const IGenericAccessGroupCollectionEntry*>(entry);
			name = collectionEntry->GetName();
			childEntryCount = collectionEntry->GetEntryCount();
			entryHasChildren = true;
			if(usePreservedExpandState)
			{
				childrenExpandedByDefault = preservedExpandState;
			}
			else
			{
				childrenExpandedByDefault = collectionEntry->GetOpenByDefault();
			}

			//Set a gray colour for the summary text on a collection
			WC_DataGrid::Grid_SetCellColor setCellColorParams;
			setCellColorParams.targetRowNo = currentRow;
			setCellColorParams.targetColumnID = VALUECOLUMNID;
			setCellColorParams.textColorDefined = true;
			setCellColorParams.colorTextFront = WinColor(96, 96, 96);
			setCellColorParams.colorTextBack = WinColor(255, 255, 255);
			SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLCOLOR, 0, (LPARAM)&setCellColorParams);
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_COMMAND)
		{
			//const IGenericAccessGroupCommandEntry* commandEntry = static_cast<const IGenericAccessGroupCommandEntry*>(entry);
			name = L"";

			//Set the correct control type for the value column
			WC_DataGrid::Grid_SetCellInfo setCellInfo(VALUECOLUMNID, currentRow, WC_DataGrid::CELLCONTROLTYPE_BUTTON);
			SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&setCellInfo);
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA)
		{
			const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(entry);
			const IGenericAccessDataInfo* dataInfo = model.GetGenericDataInfo(dataEntry->GetDataID());
			name = dataEntry->GetName();

			//If this data entry supports locking, add it to the lock state cache map.
			if(dataInfo->GetLockingSupported())
			{
				cachedDataLockedState[dataEntry->GetDataID()][dataEntry->GetDataContext()] = false;
			}

			//Set the correct control type for the value column
			if(dataInfo->GetType() == IGenericAccessDataValue::DATATYPE_BOOL)
			{
				SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&(const WC_DataGrid::Grid_SetCellInfo&)WC_DataGrid::Grid_SetCellInfo(VALUECOLUMNID, currentRow, WC_DataGrid::CELLCONTROLTYPE_CHECKBOX));
			}
			else if((dataInfo->GetType() == IGenericAccessDataValue::DATATYPE_FILEPATH) || (dataInfo->GetType() == IGenericAccessDataValue::DATATYPE_FOLDERPATH))
			{
				WC_DataGrid::Grid_SetCellInfo setCellInfo(VALUECOLUMNID, currentRow, WC_DataGrid::CELLCONTROLTYPE_TEXTBOXWITHBUTTON);
				setCellInfo.customButtonText = L"\u2026";
				setCellInfo.ellipsisMode = WC_DataGrid::TEXTELLIPSISMODE_PATH;
				SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&setCellInfo);
			}
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_SINGLESELECTIONLIST)
		{
			const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(entry);
			const IGenericAccessDataInfo* dataInfo = model.GetGenericDataInfo(selectionListEntry ->GetDataID());
			name = selectionListEntry ->GetName();

			//If this data entry supports locking, add it to the lock state cache map.
			if(dataInfo->GetLockingSupported())
			{
				cachedDataLockedState[selectionListEntry ->GetDataID()][selectionListEntry ->GetDataContext()] = false;
			}

			//Set the correct control type for the value column
			WC_DataGrid::Grid_SetCellInfo setCellInfo(VALUECOLUMNID, currentRow, WC_DataGrid::CELLCONTROLTYPE_COMBOBOX);
			setCellInfo.pickFromSelectionListOnly = !selectionListEntry->GetAllowNewItemEntry();
			std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> selectionList = selectionListEntry->GetSelectionList();
			setCellInfo.selectionList.resize(selectionList.size());
			size_t selectionListIndex = 0;
			for(std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator selectionListIterator = selectionList.begin(); selectionListIterator != selectionList.end(); ++selectionListIterator)
			{
				setCellInfo.selectionList[selectionListIndex++] = selectionListIterator->first->GetValueString();
			}
			SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&setCellInfo);
		}

		//Set the control type for the name column
		SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLINFO, 0, (LPARAM)&(const WC_DataGrid::Grid_SetCellInfo&)WC_DataGrid::Grid_SetCellInfo(NAMECOLUMNID, currentRow, WC_DataGrid::CELLCONTROLTYPE_TREEENTRY, indentLevel, entryHasChildren, childrenExpandedByDefault));

		//Update the name cell data for this child entry
		WC_DataGrid::Grid_UpdateCellText updateCellTextInfo;
		updateCellTextInfo.columnID = NAMECOLUMNID;
		updateCellTextInfo.newText = (useNameOverride)? nameOverride: name;
		updateCellTextInfo.rowNo = currentRow;
		SendMessage(hwndDataList, WC_DataGrid::GRID_UPDATECELLTEXT, 0, (LPARAM)&updateCellTextInfo);

		//Add this entry into our state array
		GridRowInfo entryRowInfo;
		entryRowInfo.entry = entry;
		entryRowInfo.entryType = entryType;
		entryRowInfo.nestingLevel = indentLevel;
		entryRowInfo.childEntryCount = childEntryCount;
		entryRowInfo.expandState = childrenExpandedByDefault;
		entryRowInfo.lastCollectionModifiedToken = 0;
		entryRowInfo.collectionEntryKey = collectionEntryKey;
		entryRowInfo.parentCollectionInfo = parentCollectionInfo;
		rowInfo.insert(rowInfo.begin() + currentRow, entryRowInfo);
	}

	//Advance the current grid row number
	unsigned int targetRowNo = currentRow;
	++currentRow;

	//If this child entry is a group or collection with children that needs to be expanded
	//by default, recurse into it to update each of its children.
	if(rowInfo[targetRowNo].expandState)
	{
		if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_GROUP)
		{
			const IGenericAccessGroup* groupEntry = static_cast<const IGenericAccessGroup*>(entry);
			std::list<IGenericAccessGroupEntry*> nodeEntries = groupEntry->GetEntries();
			for(std::list<IGenericAccessGroupEntry*>::const_iterator i = nodeEntries.begin(); i != nodeEntries.end(); ++i)
			{
				bool childExpandState = false;
				unsigned int childCollectionEntryCount = 0;
				if(usePreservedExpandState)
				{
					IGenericAccessGroupEntry::GroupEntryType childEntryType = (*i)->GetGroupEntryType();
					if((childEntryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_GROUP) || (childEntryType == IGenericAccessGroupEntry::GROUPENTRYTYPE_COLLECTION))
					{
						const ExpandStateInfo& childExpandStateInfo = preservedExpandStateBuffer.front();
						childExpandState = childExpandStateInfo.expanded;
						childCollectionEntryCount = childExpandStateInfo.collectionEntryCount;
						preservedExpandStateBuffer.pop_front();
					}
				}
				PopulateDataGrid(*i, currentRow, indentLevel + 1, false, usePreservedExpandState, childExpandState, childCollectionEntryCount, preservedExpandStateBuffer, parentCollectionInfo);
			}
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION)
		{
			//Build a map of of preserved expand state for the previous collection entries
			//based on the saved collection key values, if required.
			std::map<std::wstring, ExpandStateInfo> expandStateInfoForCollectionEntries;
			if(usePreservedExpandState)
			{
				//##FIX## This won't work correctly with child groups or collection
				//entries, because the children of each entry in our group will appear
				//inline following each entry. If our child expand data was physically
				//nested under our expand data, rather than just appearing sequentially
				//after it, we could avoid this issue.
				for(unsigned int i = 0; i < preservedExpandStateCollectionEntryCount; ++i)
				{
					const ExpandStateInfo& childExpandStateInfo = preservedExpandStateBuffer.front();
					expandStateInfoForCollectionEntries[childExpandStateInfo.collectionKeyAsString] = childExpandStateInfo;
					preservedExpandStateBuffer.pop_front();
				}
			}

			//Populate the child entries for this collection
			const IGenericAccessGroupCollectionEntry* collectionEntry = static_cast<const IGenericAccessGroupCollectionEntry*>(entry);
			collectionEntry->ObtainReadLock();
			std::list<IGenericAccessGroupCollectionEntry::CollectionEntry> collectionEntries = collectionEntry->GetCollectionEntries();
			for(std::list<IGenericAccessGroupCollectionEntry::CollectionEntry>::const_iterator collectionIterator = collectionEntries.begin(); collectionIterator != collectionEntries.end(); ++collectionIterator)
			{
				const IGenericAccessGroupCollectionEntry::CollectionEntry& collectionChild = *collectionIterator;
				bool childExpandStatePresent = false;
				bool childExpandState = false;
				unsigned int childCollectionEntryCount = 0;
				if(usePreservedExpandState)
				{
					//##FIX## We're only supposed to extract expand state info for group
					//or collection children
					std::map<std::wstring, ExpandStateInfo>::const_iterator collectionEntryExpandStateIterator = expandStateInfoForCollectionEntries.find(collectionChild.key->GetValueString());
					if(collectionEntryExpandStateIterator != expandStateInfoForCollectionEntries.end())
					{
						const ExpandStateInfo& childExpandStateInfo = collectionEntryExpandStateIterator->second;
						childExpandStatePresent = true;
						childExpandState = childExpandStateInfo.expanded;
						childCollectionEntryCount = childExpandStateInfo.collectionEntryCount;
					}
				}

				std::wstring childCollectionEntryKey = collectionChild.key->GetValueString();
				std::wstring childCollectionEntryKeyText = L"[" + childCollectionEntryKey + L"]";
				std::list<ParentCollectionInfo> parentCollectionInfoForChild = parentCollectionInfo;
				parentCollectionInfoForChild.push_back(ParentCollectionInfo(collectionEntry, childCollectionEntryKey, collectionChild.value));
				PopulateDataGrid(collectionChild.value, currentRow, indentLevel + 1, false, childExpandStatePresent, childExpandState, childCollectionEntryCount, preservedExpandStateBuffer, parentCollectionInfoForChild, childCollectionEntryKey, true, childCollectionEntryKeyText);
			}
			//Note that we could not keep a reference to the target row info from earlier,
			//as after a single call to PopulateDataGrid, the rowInfo structure and all
			//existing references are potentially invalidated, due to a change in the
			//array contents.
			GridRowInfo& targetRowInfo = rowInfo[targetRowNo];
			targetRowInfo.childEntryCount = (unsigned int)collectionEntries.size();
			targetRowInfo.lastCollectionModifiedToken = collectionEntry->GetLastModifiedToken();
			collectionEntry->ReleaseReadLock();
		}
	}
}

//----------------------------------------------------------------------------------------
void GenericDataView::DepopulateDataGrid(unsigned int targetRowNo, bool removeChildrenOnly, std::list<ExpandStateInfo>& preservedExpandState)
{
	unsigned int rowsToRemove = 0;
	DepopulateDataGrid(0, targetRowNo, rowsToRemove, removeChildrenOnly, preservedExpandState);
}

//----------------------------------------------------------------------------------------
void GenericDataView::DepopulateDataGrid(unsigned int recursionDepth, unsigned int& targetRowNo, unsigned int& rowsToRemove, bool removeChildrenOnly, std::list<ExpandStateInfo>& preservedExpandState)
{
	//If this is the lowest entry in the recursion stack, initialize the preserved expand
	//state list.
	if(recursionDepth == 0)
	{
		preservedExpandState.clear();
	}

	//Add this row to the set of rows to remove if required
	const GridRowInfo& entryRowInfo = rowInfo[targetRowNo];
	if((recursionDepth != 0) || !removeChildrenOnly)
	{
		//Increment the count of rows to remove
		++rowsToRemove;

		//If this entry can contain child entries, save its current expand state to the
		//preserved expand state list.
		if((entryRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_GROUP) || (entryRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION))
		{
			ExpandStateInfo expandStateInfo;
			expandStateInfo.expanded = entryRowInfo.expandState;
			expandStateInfo.collectionEntryCount = entryRowInfo.childEntryCount;
			expandStateInfo.collectionKeyAsString = entryRowInfo.collectionEntryKey;
			preservedExpandState.push_back(expandStateInfo);
		}
	}

	//Calculate the first row number of the removed items if this is the lowest entry in
	//the recursion stack
	unsigned int firstRowNo = targetRowNo + (removeChildrenOnly? 1: 0);

	//Advance to the next row
	++targetRowNo;

	//If this row contained visible children, add each child row to the set of rows to
	//remove.
	if(entryRowInfo.expandState)
	{
		for(unsigned int i = 0; i < entryRowInfo.childEntryCount; ++i)
		{
			DepopulateDataGrid(recursionDepth + 1, targetRowNo, rowsToRemove, false, preservedExpandState);
		}
	}

	//If this is the lowest entry in the recursion stack, remove the target set of rows.
	if(recursionDepth == 0)
	{
		//Delete the rows from the data grid
		WC_DataGrid::Grid_DeleteRows deleteRowsInfo;
		deleteRowsInfo.targetRowNo = firstRowNo;
		deleteRowsInfo.rowCount = rowsToRemove;
		SendMessage(hwndDataList, WC_DataGrid::GRID_DELETEROWS, 0, (LPARAM)&deleteRowsInfo);

		//Remove the rows from our state array
		rowInfo.erase(rowInfo.begin() + firstRowNo, rowInfo.begin() + firstRowNo + rowsToRemove);
	}
}

//----------------------------------------------------------------------------------------
void GenericDataView::UpdateDataGrid(unsigned int& currentRow, bool parentLockSucceeded)
{
	//Attempt to obtain a lock on the target value
	unsigned int originalRowNo = currentRow;
	bool lockSucceeded = parentLockSucceeded && LockTargetRowEntry(currentRow);

	//If we successfully obtained a lock, update the data value for this row.
	if(lockSucceeded)
	{
		//Retrieve info on the target row
		GridRowInfo& targetRowInfo = rowInfo[currentRow];

		//Obtain the value for this child entry
		std::wstring value;
		if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_GROUP)
		{
			if(!targetRowInfo.expandState)
			{
				const IGenericAccessGroup* groupEntry = static_cast<const IGenericAccessGroup*>(targetRowInfo.entry);
				value = BuildGroupSummaryText(groupEntry);
			}
		}
		else if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION)
		{
			const IGenericAccessGroupCollectionEntry* collectionEntry = static_cast<const IGenericAccessGroupCollectionEntry*>(targetRowInfo.entry);
			IntToString(collectionEntry->GetEntryCount(), value);
		}
		else if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_COMMAND)
		{
			const IGenericAccessGroupCommandEntry* commandEntry = static_cast<const IGenericAccessGroupCommandEntry*>(targetRowInfo.entry);
			value = commandEntry->GetName();
		}
		else if((targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA) || (targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_SINGLESELECTIONLIST))
		{
			unsigned int dataID;
			const IGenericAccess::DataContext* dataContext;
			if(targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA)
			{
				const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(targetRowInfo.entry);
				dataID = dataEntry->GetDataID();
				dataContext = dataEntry->GetDataContext();
				model.ReadGenericData(dataID, dataContext, value);
			}
			else
			{
				const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(targetRowInfo.entry);
				dataID = selectionListEntry->GetDataID();
				dataContext = selectionListEntry->GetDataContext();
				model.ReadGenericData(dataID, dataContext, value);

				std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> selectionList = selectionListEntry->GetSelectionList();
				std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator selectionListIterator = selectionList.begin();
				bool convertedValue = false;
				while(!convertedValue && (selectionListIterator != selectionList.end()))
				{
					if(value == selectionListIterator->second->GetValueString())
					{
						value = selectionListIterator->first->GetValueString();
						convertedValue = true;
						continue;
					}
					++selectionListIterator;
				}
			}

			std::map<unsigned int, std::map<const IGenericAccess::DataContext*, bool>>::iterator cachedDataLockedStateIterator = cachedDataLockedState.find(dataID);
			if(cachedDataLockedStateIterator != cachedDataLockedState.end())
			{
				std::map<const IGenericAccess::DataContext*, bool>::iterator cachedDataLockedStateIteratorForDataValue = cachedDataLockedStateIterator->second.find(dataContext);
				if(cachedDataLockedStateIteratorForDataValue != cachedDataLockedStateIterator->second.end())
				{
					//Update the locked state of the data cell for this data value
					bool newLockedState = model.GetGenericDataLocked(dataID, dataContext);
					if(cachedDataLockedStateIteratorForDataValue->second != newLockedState)
					{
						cachedDataLockedStateIteratorForDataValue->second = newLockedState;
						WC_DataGrid::Grid_SetCellColor setCellColorParams;
						setCellColorParams.targetRowNo = currentRow;
						setCellColorParams.targetColumnID = VALUECOLUMNID;
						if(newLockedState)
						{
							setCellColorParams.backgroundColorDefined = true;
							setCellColorParams.colorBackground = WinColor(255, 127, 127);
							setCellColorParams.textColorDefined = true;
							setCellColorParams.colorTextFront = WinColor(0, 0, 0);
							setCellColorParams.colorTextBack = setCellColorParams.colorBackground;
						}
						SendMessage(hwndDataList, WC_DataGrid::GRID_SETCELLCOLOR, 0, (LPARAM)&setCellColorParams);
					}
				}
			}
		}

		//Update the value cell data for this child entry
		WC_DataGrid::Grid_UpdateCellText updateCellTextInfo;
		updateCellTextInfo.columnID = VALUECOLUMNID;
		updateCellTextInfo.newText = value;
		updateCellTextInfo.rowNo = currentRow;
		SendMessage(hwndDataList, WC_DataGrid::GRID_UPDATECELLTEXT, 0, (LPARAM)&updateCellTextInfo);

		//If this entry is an expanded collection entry, refresh the child item list if it
		//has been modified.
		if((targetRowInfo.entryType == IGenericAccessGroup::GROUPENTRYTYPE_COLLECTION) && targetRowInfo.expandState)
		{
			const IGenericAccessGroupCollectionEntry* collectionEntry = static_cast<const IGenericAccessGroupCollectionEntry*>(targetRowInfo.entry);
			if(targetRowInfo.lastCollectionModifiedToken != collectionEntry->GetLastModifiedToken())
			{
				unsigned int originalCollectionEntryCount = targetRowInfo.childEntryCount;
				std::list<ParentCollectionInfo> parentCollectionInfo = targetRowInfo.parentCollectionInfo;
				std::list<ExpandStateInfo> preservedExpandStateBuffer;
				unsigned int nestingLevel = targetRowInfo.nestingLevel;
				//Note that after the call to DepopulateDataGrid below, our targetRowInfo
				//reference is invalidated, due to potential reallocation of the rowInfo
				//array.
				DepopulateDataGrid(currentRow, true, preservedExpandStateBuffer);
				//##FIX## Make a simple entry wrapper to this recursive method
				unsigned int currentRowTemp = currentRow;
				PopulateDataGrid(collectionEntry, currentRowTemp, nestingLevel, true, true, true, originalCollectionEntryCount, preservedExpandStateBuffer, parentCollectionInfo);
			}
		}
	}

	//Advance the current grid row number
	++currentRow;

	//If this child entry is a currently expanded group or collection, recurse into it to
	//update each of its children.
	if(rowInfo[originalRowNo].expandState)
	{
		for(unsigned int childNo = 0; childNo < rowInfo[originalRowNo].childEntryCount; ++childNo)
		{
			UpdateDataGrid(currentRow, lockSucceeded);
		}
	}

	//If we successfully obtained a lock, release the lock now.
	if(lockSucceeded)
	{
		UnlockTargetRowEntry(originalRowNo);
	}
}

//----------------------------------------------------------------------------------------
std::wstring GenericDataView::BuildGroupSummaryText(const IGenericAccessGroup* group)
{
	std::wstringstream summaryText;
	bool summaryTextEmpty = true;
	std::list<IGenericAccessGroupEntry*> groupEntries = group->GetEntries();
	for(std::list<IGenericAccessGroupEntry*>::const_iterator i = groupEntries.begin(); i != groupEntries.end(); ++i)
	{
		const IGenericAccessGroupEntry* entry = *i;
		IGenericAccessGroup::GroupEntryType entryType = entry->GetGroupEntryType();
		if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_DATA)
		{
			const IGenericAccessGroupDataEntry* dataEntry = static_cast<const IGenericAccessGroupDataEntry*>(entry);
			std::wstring dataValue;
			if(model.ReadGenericData(dataEntry->GetDataID(), dataEntry->GetDataContext(), dataValue))
			{
				if(!summaryTextEmpty)
				{
					summaryText << L", ";
				}
				summaryText << dataEntry->GetName() << L"=\"" << dataValue << L'\"';
				summaryTextEmpty = false;
			}
		}
		else if(entryType == IGenericAccessGroup::GROUPENTRYTYPE_SINGLESELECTIONLIST)
		{
			const IGenericAccessGroupSingleSelectionList* selectionListEntry = static_cast<const IGenericAccessGroupSingleSelectionList*>(entry);
			std::wstring dataValue;
			if(model.ReadGenericData(selectionListEntry->GetDataID(), selectionListEntry->GetDataContext(), dataValue))
			{
				if(!summaryTextEmpty)
				{
					summaryText << L", ";
				}
				summaryText << selectionListEntry->GetName() << L"=\"" << dataValue << L'\"';
				summaryTextEmpty = false;
			}
		}
	}
	return summaryText.str();
}

//----------------------------------------------------------------------------------------
//Collection locking methods
//----------------------------------------------------------------------------------------
bool GenericDataView::LockTargetRowEntry(unsigned int rowNo)
{
	//##TODO## Introduce some kind of lock buffer so that we only obtain and release locks
	//when required. Right now, we perform the parent validation check for each child,
	//which is quite inefficient, since we'll already have a lock in most cases.
	const GridRowInfo& targetRowInfo = rowInfo[rowNo];
	for(std::list<ParentCollectionInfo>::const_iterator i = targetRowInfo.parentCollectionInfo.begin(); i != targetRowInfo.parentCollectionInfo.end(); ++i)
	{
		const ParentCollectionInfo& collectionInfo = *i;
		collectionInfo.collection->ObtainReadLock();
		if(collectionInfo.collection->GetCollectionEntry(collectionInfo.childKey) != collectionInfo.childEntry)
		{
			collectionInfo.collection->ReleaseReadLock();
			std::list<ParentCollectionInfo>::const_reverse_iterator unlockIterator(i);
			while(unlockIterator != targetRowInfo.parentCollectionInfo.rend())
			{
				unlockIterator->collection->ReleaseReadLock();
				++unlockIterator;
			}
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
void GenericDataView::UnlockTargetRowEntry(unsigned int rowNo)
{
	const GridRowInfo& targetRowInfo = rowInfo[rowNo];
	for(std::list<ParentCollectionInfo>::const_reverse_iterator i = targetRowInfo.parentCollectionInfo.rbegin(); i != targetRowInfo.parentCollectionInfo.rend(); ++i)
	{
		const ParentCollectionInfo& collectionInfo = *i;
		collectionInfo.collection->ReleaseReadLock();
	}
}
