//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum ISystemDeviceInterface::KeyCode
{
	KEYCODE_NONE,

	//Control keys
	KEYCODE_ESCAPE,
	KEYCODE_TAB,
	KEYCODE_ENTER,
	KEYCODE_SPACE,
	KEYCODE_BACKSPACE,
	KEYCODE_INSERT,
	KEYCODE_DELETE,
	KEYCODE_PAGEUP,
	KEYCODE_PAGEDOWN,
	KEYCODE_HOME,
	KEYCODE_END,
	KEYCODE_UP,
	KEYCODE_DOWN,
	KEYCODE_LEFT,
	KEYCODE_RIGHT,
	KEYCODE_PRINTSCREEN,
	KEYCODE_PAUSE,
	KEYCODE_NUMLOCK,
	KEYCODE_CAPSLOCK,
	KEYCODE_SCROLLLOCK,
	KEYCODE_LEFTWINDOWS,
	KEYCODE_RIGHTWINDOWS,
	KEYCODE_MENU,

	//Modifier keys
	KEYCODE_CTRL,
	KEYCODE_ALT,
	KEYCODE_SHIFT,

	//Function keys
	KEYCODE_F1,
	KEYCODE_F2,
	KEYCODE_F3,
	KEYCODE_F4,
	KEYCODE_F5,
	KEYCODE_F6,
	KEYCODE_F7,
	KEYCODE_F8,
	KEYCODE_F9,
	KEYCODE_F10,
	KEYCODE_F11,
	KEYCODE_F12,

	//Numbers
	KEYCODE_0,
	KEYCODE_1,
	KEYCODE_2,
	KEYCODE_3,
	KEYCODE_4,
	KEYCODE_5,
	KEYCODE_6,
	KEYCODE_7,
	KEYCODE_8,
	KEYCODE_9,

	//Letters
	KEYCODE_A,
	KEYCODE_B,
	KEYCODE_C,
	KEYCODE_D,
	KEYCODE_E,
	KEYCODE_F,
	KEYCODE_G,
	KEYCODE_H,
	KEYCODE_I,
	KEYCODE_J,
	KEYCODE_K,
	KEYCODE_L,
	KEYCODE_M,
	KEYCODE_N,
	KEYCODE_O,
	KEYCODE_P,
	KEYCODE_Q,
	KEYCODE_R,
	KEYCODE_S,
	KEYCODE_T,
	KEYCODE_U,
	KEYCODE_V,
	KEYCODE_W,
	KEYCODE_X,
	KEYCODE_Y,
	KEYCODE_Z,

	//Symbol keys
	KEYCODE_OEM1,
	KEYCODE_OEMPLUS,
	KEYCODE_OEMCOMMA,
	KEYCODE_OEMMINUS,
	KEYCODE_OEMPERIOD,
	KEYCODE_OEM2,
	KEYCODE_OEM3,
	KEYCODE_OEM4,
	KEYCODE_OEM5,
	KEYCODE_OEM6,
	KEYCODE_OEM7,
	KEYCODE_OEM8,
	KEYCODE_OEMAX,
	KEYCODE_OEM102,

	//Numpad keys
	KEYCODE_NUMPAD0,
	KEYCODE_NUMPAD1,
	KEYCODE_NUMPAD2,
	KEYCODE_NUMPAD3,
	KEYCODE_NUMPAD4,
	KEYCODE_NUMPAD5,
	KEYCODE_NUMPAD6,
	KEYCODE_NUMPAD7,
	KEYCODE_NUMPAD8,
	KEYCODE_NUMPAD9,
	KEYCODE_NUMPADMULTIPLY,
	KEYCODE_NUMPADDIVIDE,
	KEYCODE_NUMPADSUBTRACT,
	KEYCODE_NUMPADADD,
	KEYCODE_NUMPADDECIMAL,

	//Joystick buttons
	KEYCODE_JOYSTICK00BUTTON00,
	KEYCODE_JOYSTICK15BUTTON31 = (ISystemDeviceInterface::KEYCODE_JOYSTICK00BUTTON00 + ((16*32) - 1)),

	//Joystick axes as buttons
	KEYCODE_JOYSTICK00AXIS0PLUS,
	KEYCODE_JOYSTICK00AXIS0MINUS,
	KEYCODE_JOYSTICK15AXIS5PLUS = (ISystemDeviceInterface::KEYCODE_JOYSTICK00AXIS0PLUS + ((16*6*2) - 2)),
	KEYCODE_JOYSTICK15AXIS5MINUS = (ISystemDeviceInterface::KEYCODE_JOYSTICK00AXIS0MINUS + ((16*6*2) - 2)),

	KEYCODE_ENDOFLIST
};

//----------------------------------------------------------------------------------------
enum ISystemDeviceInterface::AxisCode
{
	AXISCODE_NONE,

	//Joystick axes
	AXISCODE_JOYSTICK00AXIS0,
	AXISCODE_JOYSTICK15AXIS6 = (ISystemDeviceInterface::AXISCODE_JOYSTICK00AXIS0 + ((16*6) - 1)),

	AXISCODE_ENDOFLIST
};

//----------------------------------------------------------------------------------------
enum ISystemDeviceInterface::ScrollCode
{
	SCROLLCODE_NONE,

	//Mouse input
	SCROLLCODE_MOUSEX,
	SCROLLCODE_MOUSEY,
	SCROLLCODE_MOUSEZ,

	SCROLLCODE_ENDOFLIST
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
