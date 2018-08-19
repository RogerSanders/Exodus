#ifndef __CRASHREPORT_H__
#define __CRASHREPORT_H__
#include <string>

//Minidump functions
bool RegisterMinidumpExceptionHandler(const std::wstring& minidumpName, const std::wstring& minidumpPath, bool largeMinidump);

#endif
