//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class MDBusArbiter::CELineID
{
	RW = 1,
	UDS,
	LDS,
	OE0,
	CE0,
	BootROM,
	ROM,
	ASEL,
	FDC,
	FDWR,
	TIME,
	IO,
	EOE,
	NOE,
	ZRAM,
	SOUND
};

//----------------------------------------------------------------------------------------
enum class MDBusArbiter::LineID
{
	CART = 1, //I
	VPA,      //O
	INTAK,    //O
	BR,       //IO
	BG,       //I
	ZBR,      //O
	ZBAK,     //I
	ZRES,     //O
	VRES,     //O
	HALT,     //O
	SRES,     //O
	WRES,     //I

	//These pseudo-lines are used to control settings for this device
	ActivateTMSS,
	ActivateBootROM
};

//----------------------------------------------------------------------------------------
enum class MDBusArbiter::MemoryInterface
{
	InterruptAcknowledgeCycle = 1,
	M68kToZ80MemoryWindow = 2,
	Z80ToVDPMemoryWindow = 3,
	Z80ToM68kMemoryWindow = 4,
	Z80WindowBankswitch = 5,
	MemoryMode = 6,
	Z80BusReq = 7,
	Z80Reset = 8,
	TMSS = 9,
	TMSSBootROMSwitch = 10
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MDBusArbiter::LineAccess
{
	LineAccess(LineID alineLD, const Data& astate, double aaccessTime)
	:lineID(alineLD), state(astate), accessTime(aaccessTime)
	{}

	LineID lineID;
	Data state;
	double accessTime;
};
