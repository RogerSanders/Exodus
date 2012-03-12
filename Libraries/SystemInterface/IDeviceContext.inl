//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IDeviceContext::KeyCode
{
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

	//Characters
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
	KEYCODE_Z
};

//----------------------------------------------------------------------------------------
//System message functions
//----------------------------------------------------------------------------------------
std::wstring IDeviceContext::GetCapturePath() const
{
	return GetCapturePathInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IDeviceContext::GetModuleDisplayName() const
{
	return GetModuleDisplayNameInternal();
}
