//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class ISystemDeviceInterface::KeyCode
{
	None,

	//Control keys
	Escape,
	Tab,
	Enter,
	Space,
	Backspace,
	Insert,
	Delete,
	PageUp,
	PageDown,
	Home,
	End,
	Up,
	Down,
	Left,
	Right,
	PrintScreen,
	Pause,
	NumLock,
	CapsLock,
	ScrollLock,
	LeftWindows,
	RightWindows,
	Menu,

	//Modifier keys
	Ctrl,
	Alt,
	Shift,

	//Function keys
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	//Numbers
	Number0,
	Number1,
	Number2,
	Number3,
	Number4,
	Number5,
	Number6,
	Number7,
	Number8,
	Number9,

	//Letters
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	//Symbol keys
	OEM1,
	OEMPLUS,
	OEMCOMMA,
	OEMMINUS,
	OEMPERIOD,
	OEM2,
	OEM3,
	OEM4,
	OEM5,
	OEM6,
	OEM7,
	OEM8,
	OEMAX,
	OEM102,

	//Numpad keys
	NUMPAD0,
	NUMPAD1,
	NUMPAD2,
	NUMPAD3,
	NUMPAD4,
	NUMPAD5,
	NUMPAD6,
	NUMPAD7,
	NUMPAD8,
	NUMPAD9,
	NUMPADMULTIPLY,
	NUMPADDIVIDE,
	NUMPADSUBTRACT,
	NUMPADADD,
	NUMPADDECIMAL,

	//Joystick buttons
	JOYSTICK00BUTTON00,
	JOYSTICK15BUTTON31 = (ISystemDeviceInterface::KeyCode::JOYSTICK00BUTTON00 + ((16*32) - 1)),

	//Joystick axes as buttons
	JOYSTICK00AXIS0PLUS,
	JOYSTICK00AXIS0MINUS,
	JOYSTICK15AXIS5PLUS = (ISystemDeviceInterface::KeyCode::JOYSTICK00AXIS0PLUS + ((16*6*2) - 2)),
	JOYSTICK15AXIS5MINUS = (ISystemDeviceInterface::KeyCode::JOYSTICK00AXIS0MINUS + ((16*6*2) - 2)),

	EndOfList
};

//----------------------------------------------------------------------------------------
enum class ISystemDeviceInterface::AxisCode
{
	None,

	//Joystick axes
	Joystick00Axis0,
	Joystick15Axis6 = (ISystemDeviceInterface::AxisCode::Joystick00Axis0 + ((16*6) - 1)),

	EndOfList
};

//----------------------------------------------------------------------------------------
enum class ISystemDeviceInterface::ScrollCode
{
	None,

	//Mouse input
	MouseX,
	MouseY,
	MouseZ,

	EndOfList
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISystemDeviceInterface::ThisISystemDeviceInterfaceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Path functions
//----------------------------------------------------------------------------------------
std::wstring ISystemDeviceInterface::GetCapturePath() const
{
	std::wstring result;
	GetCapturePathInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
