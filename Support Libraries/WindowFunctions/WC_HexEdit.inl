//----------------------------------------------------------------------------------------
//Message numbers
//----------------------------------------------------------------------------------------
enum WC_HexEdit::WindowMessages
{
	HEX_WMBASE = WM_USER + 0x100,
	HEX_SETWINDOWSIZE,
	HEX_SETWINDOWPOS,
	HEX_SETWINDOWADDRESSOFFSET,
	HEX_SETWINDOWADDRESSWIDTH,
	HEX_UPDATEWINDOWDATA,
	HEX_GETWINDOWSIZE,
	HEX_GETWINDOWPOS,
	HEX_SETMARKINGINFO
};

//----------------------------------------------------------------------------------------
enum WC_HexEdit::WindowNotifications
{
	HEX_READDATA = 100,
	HEX_WRITEDATA,
	HEX_NEWWINDOWPOS,
	HEX_READDATABLOCK,
	HEX_WRITEDATABLOCK,
	HEX_UPDATEDATAMARKING
};

//----------------------------------------------------------------------------------------
//Context menu numbers
//----------------------------------------------------------------------------------------
enum WC_HexEdit::MenuItem
{
	MNU_COPY = 0x100,
	MNU_PASTE,
	MNU_MARK,
	MNU_UNMARK,
	MNU_COLUMNWIDTH,
	MNU_COLUMNWIDTH_1,
	MNU_COLUMNWIDTH_2,
	MNU_COLUMNWIDTH_4,
	MNU_COLUMNWIDTH_8,
	MNU_COLUMNWIDTH_16,
	MNU_ENABLETEXT,
	MNU_SAVETOFILE,
	MNU_LOADFROMFILE
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

struct WC_HexEdit::Hex_DataMarkingInfo
{
	bool markingEnabled;
	std::wstring markName;
	std::wstring unmarkName;
};

struct WC_HexEdit::Hex_ReadDataInfo
{
	unsigned int offset;
	unsigned char data;
	bool processed;
};

struct WC_HexEdit::Hex_WriteDataInfo
{
	unsigned int offset;
	unsigned char data;
};

struct WC_HexEdit::Hex_NewWindowPosInfo
{
	unsigned int windowPos;
	unsigned int windowSize;
};

struct WC_HexEdit::Hex_ReadDataBlockInfo
{
	unsigned int offset;
	unsigned int size;
	unsigned char* buffer;
	bool processed;
};

struct WC_HexEdit::Hex_WriteDataBlockInfo
{
	unsigned int offset;
	unsigned int size;
	unsigned char* buffer;
	bool processed;
};

struct WC_HexEdit::Hex_UpdateDataMarkingState
{
	unsigned int offset;
	unsigned int size;
	bool state;
};
