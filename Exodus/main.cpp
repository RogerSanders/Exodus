#include "WindowsSupport/WindowsSupport.pkg"
#include "Debug/Debug.pkg"
#include "ExodusInterface.h"
#include "SystemInterface/SystemInterface.pkg"
#include "SystemInfo.h"
#include "resource.h"
#include <stdlib.h>
#include <time.h>

//##TODO## Fix the static module linking process. We're not actively using it anymore, but
//we want to retain support for it. Some users may have cases where it is useful to be
//able to compile versions of our emulation platform which are pre-assembled to emulate a
//particular system.

//If we're statically linking to any devices, include the interface functions for those
//devices here.
#ifndef EX_DLLINTERFACE
#include "../Devices/315-5313/interface.h"
#include "../Devices/M68000/interface.h"
#include "../Devices/MD1600IO/interface.h"
#include "../Devices/Memory/interface.h"
#include "../Devices/Z80/interface.h"
#include "../Devices/YM2612/interface.h"
#include "../Devices/SN76489/interface.h"
#endif


//----------------------------------------------------------------------------------------
//WinMain function
//----------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Register our minidump exception handler
	//##TODO## Consider putting a build number into the filename
	//##TODO## Consider making a text crash report file too, which reports on memory
	//usage, build version, platform info, and other such details.
	//##TODO## Consider adding a custom crash screen, which will inform the user of the
	//error in a nicer way and give them some instructions to send a crash report to us.
	RegisterMinidumpExceptionHandler(L"Exodus", L"Crash Reports", false);

	//Seed the "rand()" random number generator using the current system time
	srand((unsigned int)time(NULL));

	//If we're statically linking to any devices, bind them to the system here.
#ifndef EX_DLLINTERFACE
	systemObject.RegisterDevice(GetA04000Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA06000Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA11100Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetA10000Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetM68000Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetRAMLibrary(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetSharedRAMLibrary(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetTimedRAMLibrary(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetROMLibrary(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetSN76489Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetVDPLibrary(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetYM2612Library(), (AssemblyHandle)GetModuleHandle(NULL));
	systemObject.RegisterDevice(GetZ80Library(), (AssemblyHandle)GetModuleHandle(NULL));
#endif

	//Create the main interface object
	ExodusInterface exodusInterface;

	//Load the system assembly
	ExodusInterface::PluginInfo systemPluginInfo;
	if(!exodusInterface.LoadAssemblyInfo(L"System.dll", systemPluginInfo) || (systemPluginInfo.GetSystemEntry == 0))
	{
		return 1;
	}

	//Retrieve information on the system plugin from the system assembly
	SystemInfo systemInfo;
	if(!systemPluginInfo.GetSystemEntry(0, systemInfo))
	{
		return 2;
	}

	//Construct the system object
	ISystemInfo::AllocatorPointer systemAllocator = systemInfo.GetAllocator();
	ISystemInfo::DestructorPointer systemDestructor = systemInfo.GetDestructor();
	ISystemGUIInterface* systemObject = systemAllocator(exodusInterface);

	//Bind the main interface to the system
	exodusInterface.BindToSystem(systemObject);

	//Create the main interface window
	HWND hwnd = exodusInterface.CreateMainInterface(hInstance);
	if(hwnd == NULL)
	{
		return 3;
	}

	//Load the keyboard accelerator table
	HACCEL acceleratorTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));
	if(acceleratorTable == NULL)
	{
		return 4;
	}

	//Begin the message loop
	MSG msg;
	bool done = false;
	while(!done)
	{
		BOOL getMessageReturn = GetMessage(&msg, NULL, 0, 0);
		if(getMessageReturn == 0)
		{
			done = true;
		}
		else if(getMessageReturn != -1)
		{
			if(TranslateAccelerator(hwnd, acceleratorTable, &msg) == 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	//Destroy the keyboard accelerator table
	DestroyAcceleratorTable(acceleratorTable);

	//Unbind the system object from the interface
	exodusInterface.UnbindFromSystem();

	//Destroy the system object
	systemDestructor(systemObject);

	return 0;
}
