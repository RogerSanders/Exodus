//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum ExodusInterface::ViewOperationType
{
	VIEWOPERATIONTYPE_OPEN,
	VIEWOPERATIONTYPE_CLOSE,
	VIEWOPERATIONTYPE_DELETE,
	VIEWOPERATIONTYPE_ACTIVATE,
	VIEWOPERATIONTYPE_SHOW,
	VIEWOPERATIONTYPE_HIDE
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ExodusInterface::NewMenuItem
{
	NewMenuItem(HMENU aparentMenu, unsigned int amenuID, IMenuSelectableOption& amenuItem)
	:parentMenu(aparentMenu), menuID(amenuID), menuItem(amenuItem)
	{}

	HMENU parentMenu;
	unsigned int menuID;
	IMenuSelectableOption& menuItem;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::ViewInfo
{
	ViewInfo()
	:waitCount(0), viewCurrentlyOpen(false), ignoreForLayoutPurposes(false)
	{}

	bool viewCurrentlyOpen;
	bool openHidden;
	bool ignoreForLayoutPurposes;
	int waitCount;
	IViewModel* viewModel;
	ViewModelNotifier* notifier;
	boost::condition viewClosed;
	boost::condition viewOpened;
	boost::condition viewInfoSafeToDelete;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::Region2D
{
	Region2D()
	{}
	Region2D(long aposx, long aposy, long asizex, long asizey)
	:posx(aposx), posy(aposy), sizex(asizex), sizey(asizey)
	{}

	long posx;
	long posy;
	long sizex;
	long sizey;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SavestateCellWindowState
{
	SavestateCellWindowState()
	:hbitmap(NULL), hwnd(NULL), savestatePresent(false), savestateSlotSelected(false), debugStatePresent(false), screenshotPresent(false), initializedBitmap(false)
	{}

	HWND hwnd;
	unsigned int slotNo;
	std::wstring slotName;
	std::wstring date;
	std::wstring time;
	std::wstring timestamp;
	bool savestatePresent;
	bool savestateSlotSelected;
	bool debugStatePresent;

	Image originalImage;
	Image resizedImage;
	bool screenshotPresent;
	bool initializedBitmap;
	HBITMAP hbitmap;
	unsigned int bitmapWidth;
	unsigned int bitmapHeight;
	unsigned int bitmapXPos;
	unsigned int bitmapYPos;
	unsigned int infoRectangleHeight;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::ViewOperation
{
	ViewOperation(ViewOperationType atype, IViewModel* aviewModel)
	:type(atype), viewModel(aviewModel)
	{}

	ViewOperationType type;
	IViewModel* viewModel;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::WorkspaceViewEntryDetails
{
	WorkspaceViewEntryDetails(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, bool aownerIsSystem, const std::wstring& adeviceInstanceName, IViewModel* aviewModel = 0)
	:viewModelGroupName(aviewModelGroupName), viewModelName(aviewModelName), ownerIsSystem(aownerIsSystem), deviceInstanceName(adeviceInstanceName), viewModel(aviewModel)
	{}

	bool ownerIsSystem;
	std::wstring deviceInstanceName;
	std::wstring viewModelName;
	std::wstring viewModelGroupName;
	IViewModel* viewModel;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::MapConnectorDialogParams
{
	MapConnectorDialogParams()
	:selectionMade(false), system(0)
	{}

	bool selectionMade;
	std::list<ConnectorInfo> connectorList;
	ConnectorInfo selectedConnector;
	ISystemExternal* system;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SelectCompressedFileDialogParams
{
	SelectCompressedFileDialogParams()
	:selectionMade(false)
	{}

	bool selectionMade;
	unsigned int selectedEntryID;
	std::list<SelectCompressedFileDialogParamsFileEntry> fileList;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SelectCompressedFileDialogParamsFileEntry
{
	std::wstring fileName;
	unsigned int entryID;
};
