//----------------------------------------------------------------------------------------
//Message numbers
//----------------------------------------------------------------------------------------
enum class WC_HexEdit::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	SetWindowSize,
	SetWindowPos,
	SetWindowAddressOffset,
	SetWindowAddressWidth,
	UpdateWindowData,
	GetWindowSize,
	GetWindowPos,
	SetMarkingInfo
};

//----------------------------------------------------------------------------------------
enum class WC_HexEdit::WindowNotifications :unsigned int
{
	ReadData = 100,
	WriteData,
	NewWindowPos,
	ReadDataBlock,
	WriteDataBlock,
	UpdateDataMarking
};

//----------------------------------------------------------------------------------------
//Context menu numbers
//----------------------------------------------------------------------------------------
enum class WC_HexEdit::MenuItem :unsigned int
{
	Copy = 0x100,
	Paste,
	Mark,
	Unmark,
	ColumnWidth,
	ColumnWidth1,
	ColumnWidth2,
	ColumnWidth4,
	ColumnWidth8,
	ColumnWidth16,
	EnableText,
	SaveToFile,
	LoadFromFile
};

//----------------------------------------------------------------------------------------
//Message parameter structures
//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_UpdateWindowData
{
	Hex_UpdateWindowData()
	:newBufferData(0), newMarkBufferData(0)
	{}
	unsigned int newBufferSize;
	unsigned char* newBufferData;
	unsigned char* newMarkBufferData;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_DataMarkingInfo
{
	bool markingEnabled;
	std::wstring markName;
	std::wstring unmarkName;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_ReadDataInfo
{
	unsigned int offset;
	unsigned char data;
	bool processed;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_WriteDataInfo
{
	unsigned int offset;
	unsigned char data;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_NewWindowPosInfo
{
	unsigned int windowPos;
	unsigned int windowSize;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_ReadDataBlockInfo
{
	unsigned int offset;
	unsigned int size;
	unsigned char* buffer;
	bool processed;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_WriteDataBlockInfo
{
	unsigned int offset;
	unsigned int size;
	unsigned char* buffer;
	bool processed;
};

//----------------------------------------------------------------------------------------
struct WC_HexEdit::Hex_UpdateDataMarkingState
{
	unsigned int offset;
	unsigned int size;
	bool state;
};
