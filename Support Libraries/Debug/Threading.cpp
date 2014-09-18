#include "Threading.h"
#include "DataConversion/DataConversion.pkg"
#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------
//The following code comes from Microsoft. It uses a special custom exception which is
//recognized by Visual Studio and some other debuggers to set the thread name in the
//debugger. See the following MSDN article for further information:
//http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

//----------------------------------------------------------------------------------------
//Threading functions
//----------------------------------------------------------------------------------------
void SetCallingThreadName(const std::wstring& threadName)
{
	std::string threadNameConverted = WStringToString(threadName);
	if(!threadNameConverted.empty())
	{
		SetThreadName((DWORD)-1, &threadNameConverted[0]);
	}
}
