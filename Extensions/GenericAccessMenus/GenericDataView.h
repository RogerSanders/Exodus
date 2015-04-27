#ifndef __GENERICDATAVIEW_H__
#define __GENERICDATAVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "GenericDataViewPresenter.h"
#include <vector>
#include <list>

class GenericDataView :public ViewBase
{
public:
	//Constructors
	GenericDataView(IUIManager& auiManager, GenericDataViewPresenter& apresenter, IGenericAccess& amodel, const IGenericAccessPage* apage);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Constants
	static const long long DATALISTCONTROL = 100;
	static const unsigned int NAMECOLUMNID = 1;
	static const unsigned int VALUECOLUMNID = 2;

	//Structures
	struct ExpandStateInfo
	{
		unsigned int collectionEntryCount;
		std::wstring collectionKeyAsString;
		bool expanded;
	};
	struct ParentCollectionInfo
	{
		ParentCollectionInfo(const IGenericAccessGroupCollectionEntry* acollection, const std::wstring& achildKey, const IGenericAccessGroupEntry* achildEntry)
		:collection(acollection), childKey(achildKey), childEntry(achildEntry)
		{}

		const IGenericAccessGroupCollectionEntry* collection;
		std::wstring childKey;
		const IGenericAccessGroupEntry* childEntry;
	};
	struct GridRowInfo
	{
		const IGenericAccessGroupEntry* entry;
		IGenericAccessGroup::GroupEntryType entryType;
		bool expandState;
		unsigned int childEntryCount;
		unsigned int nestingLevel;
		unsigned int lastCollectionModifiedToken;
		std::wstring collectionEntryKey;
		std::list<ExpandStateInfo> preservedExpandState;
		std::list<ParentCollectionInfo> parentCollectionInfo;
	};
	struct CachedState
	{
		bool supportsLocking;
		bool lockedState;
		bool supportsHighlighting;
		bool highlightState;
	};

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Data update methods
	void PopulateDataGrid(const IGenericAccessGroupEntry* entry, unsigned int& currentRow, unsigned int indentLevel, bool addChildrenOnly, bool usePreservedExpandState, bool preservedExpandState, unsigned int preservedExpandStateCollectionEntryCount, std::list<ExpandStateInfo>& preservedExpandStateBuffer, const std::list<ParentCollectionInfo>& parentCollectionInfo, const std::wstring& collectionEntryKey = L"", bool useNameOverride = false, const std::wstring& nameOverride = L"");
	void DepopulateDataGrid(unsigned int targetRowNo, bool removeChildrenOnly, std::list<ExpandStateInfo>& preservedExpandStateBuffer);
	void DepopulateDataGrid(unsigned int recursionDepth, unsigned int& targetRowNo, unsigned int& rowsToRemove, bool removeChildrenOnly, std::list<ExpandStateInfo>& preservedExpandStateBuffer);
	void UpdateDataGrid(unsigned int& currentRow, bool parentLockSucceeded);
	std::wstring BuildGroupSummaryText(const IGenericAccessGroup* currentNode);

	//Collection locking methods
	bool LockTargetRowEntry(unsigned int rowNo);
	void UnlockTargetRowEntry(unsigned int rowNo);

private:
	const IGenericAccessPage* page;
	GenericDataViewPresenter& presenter;
	IGenericAccess& model;
	std::map<unsigned int, std::map<const IGenericAccess::DataContext*, CachedState>> cachedStateMap;
	std::vector<GridRowInfo> rowInfo;
	HWND hwndDataList;
	HFONT valueFont;
	HINSTANCE assemblyHandle;
};

#endif
