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
struct ExodusInterface::MapConnectorDialogParams
{
	MapConnectorDialogParams()
	:selectionMade(false), system(0)
	{}

	bool selectionMade;
	std::list<ConnectorInfo> connectorList;
	ConnectorInfo selectedConnector;
	ISystemGUIInterface* system;
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

//----------------------------------------------------------------------------------------
struct ExodusInterface::RegisteredDeviceInfo
{
	std::wstring assemblyPath;
	DeviceInfo info;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::RegisteredExtensionInfo
{
	std::wstring assemblyPath;
	ExtensionInfo info;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::PluginInfo
{
	AssemblyHandle assemblyHandle;
	unsigned int interfaceVersion;
	bool (*GetDeviceEntry)(unsigned int entryNo, IDeviceInfo& entry);
	bool (*GetExtensionEntry)(unsigned int entryNo, IExtensionInfo& entry);
	bool (*GetSystemEntry)(unsigned int entryNo, ISystemInfo& entry);
};
