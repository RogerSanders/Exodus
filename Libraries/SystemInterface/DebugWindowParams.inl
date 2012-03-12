//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams()
:system(0), baseDevice(0), currentControlFocus(0), initializedDialog(false), structName(L"DebugWindowParams")
{}

//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams(System* asystem)
:system(asystem), baseDevice(0), currentControlFocus(0), initializedDialog(false), structName(L"DebugWindowParams")
{}

//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams(IDevice* adevice)
:system(0), baseDevice(adevice), currentControlFocus(0), initializedDialog(false), structName(L"DebugWindowParams")
{}

//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams(const std::wstring& astructName)
:system(0), baseDevice(0), currentControlFocus(0), initializedDialog(false), structName(astructName)
{}

//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams(const std::wstring& astructName, System* asystem)
:system(asystem), baseDevice(0), currentControlFocus(0), initializedDialog(false), structName(astructName)
{}

//----------------------------------------------------------------------------------------
DebugWindowParams::DebugWindowParams(const std::wstring& astructName, IDevice* adevice)
:system(0), baseDevice(adevice), currentControlFocus(0), initializedDialog(false), structName(astructName)
{}
