//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum MDBusArbiter::CELineID
{
	CELINE_RW = 1,
	CELINE_UDS,
	CELINE_LDS,
	CELINE_OE0,
	CELINE_CE0,
	CELINE_ROM,
	CELINE_ASEL,
	CELINE_FDC,
	CELINE_FDWR,
	CELINE_TIME,
	CELINE_IO,
	CELINE_EOE,
	CELINE_NOE,
	CELINE_ZRAM,
	CELINE_SOUND
};

//----------------------------------------------------------------------------------------
enum MDBusArbiter::LineID
{
	LINE_CARTIN = 1,   //I
	LINE_VPA,          //O
	LINE_INTAK,        //O
	LINE_BR,           //IO
	LINE_BG,           //I
	LINE_ZBR,          //O
	LINE_ZBAK,         //I
	LINE_ZRES          //O
};

//----------------------------------------------------------------------------------------
enum MDBusArbiter::MemoryInterface
{
	MEMORYINTERFACE_INTERRUPT_ACKNOWLEDGE_CYCLE = 1,
	MEMORYINTERFACE_M68K_TO_Z80_MEMORYWINDOW = 2,
	MEMORYINTERFACE_Z80_TO_VDP_MEMORYWINDOW = 3,
	MEMORYINTERFACE_Z80_TO_M68K_MEMORYWINDOW = 4,
	MEMORYINTERFACE_Z80_WINDOW_BANKSWITCH = 5,
	MEMORYINTERFACE_MEMORYMODE = 6,
	MEMORYINTERFACE_Z80_BUSREQ = 7,
	MEMORYINTERFACE_Z80_RESET = 8
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MDBusArbiter::LineAccess
{
	LineAccess(unsigned int alineLD, const Data& astate, double aaccessTime)
	:lineID(alineLD), state(astate), accessTime(aaccessTime)
	{}

	unsigned int lineID;
	Data state;
	double accessTime;
};
