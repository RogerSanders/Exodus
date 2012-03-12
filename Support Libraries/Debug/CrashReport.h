#ifndef __CRASHREPORT_H__
#define __CRASHREPORT_H__
#include <string>

//Minidump functions
bool RegisterMinidumpExceptionHandler(const std::wstring& aminidumpName, const std::wstring& aminidumpPath, bool alargeMinidump);

#endif
