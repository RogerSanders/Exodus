#include "CrashReport.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "Stream/Stream.pkg"
#include "ZIP/ZIP.pkg"
#include <Dbghelp.h>

//----------------------------------------------------------------------------------------
std::wstring GetMinidumpPath()
{
	return GetEnvironmentVariableString(L"DEBUG_MINIDUMP_PATH");
}

//----------------------------------------------------------------------------------------
std::wstring GetMinidumpName()
{
	return GetEnvironmentVariableString(L"DEBUG_MINIDUMP_NAME");
}

//----------------------------------------------------------------------------------------
bool GetLargeMinidumpFlag()
{
	return !GetEnvironmentVariableString(L"DEBUG_MINIDUMP_LARGE").empty();
}

//----------------------------------------------------------------------------------------
void SetMinidumpPath(const std::wstring& minidumpPath)
{
	SetEnvironmentVariableString(L"DEBUG_MINIDUMP_PATH", minidumpPath);
}

//----------------------------------------------------------------------------------------
void SetMinidumpName(const std::wstring& minidumpName)
{
	SetEnvironmentVariableString(L"DEBUG_MINIDUMP_NAME", minidumpName);
}

//----------------------------------------------------------------------------------------
void SetLargeMinidumpFlag(bool largeMinidump)
{
	SetEnvironmentVariableString(L"DEBUG_MINIDUMP_LARGE", largeMinidump? L"1": L"");
}

//----------------------------------------------------------------------------------------
//Minidump functions
//----------------------------------------------------------------------------------------
bool GenerateMinidump(_EXCEPTION_POINTERS* exceptionPointers, MINIDUMP_TYPE minidumpType, const std::wstring& minidumpFolder, const std::wstring& minidumpFileName, bool compress)
{
	//Append the minidump extension to the file name
	std::wstring minidumpFilePath = PathCombinePaths(minidumpFolder, minidumpFileName + L".dmp");

	//Set the attributes for the minidump file. If we're intending to compress the
	//minidump, we mark the file as temporary to increase efficiency.
	DWORD fileAttributes = FILE_ATTRIBUTE_NORMAL;
	if(compress)
	{
		fileAttributes |= FILE_ATTRIBUTE_TEMPORARY;
	}

	//Create the empty minidump file
	HANDLE fileHandle = CreateFile(minidumpFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, fileAttributes, NULL);
	if((fileHandle == NULL) || (fileHandle == INVALID_HANDLE_VALUE))
	{
		return false;
	}

	//Set the exception pointer information
	MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
	exceptionInformation.ThreadId = GetCurrentThreadId();
	exceptionInformation.ExceptionPointers = exceptionPointers;
	exceptionInformation.ClientPointers = FALSE;
	MINIDUMP_EXCEPTION_INFORMATION* exceptionParam = (exceptionPointers != 0)? &exceptionInformation: 0;

	//Write the minidump to the file
	if(!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), fileHandle, minidumpType, exceptionParam, NULL, NULL))
	{
		CloseHandle(fileHandle);
		return false;
	}

	//Get the total size of the minidump file
	DWORD fileSize = GetFileSize(fileHandle, NULL);
	if(fileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	//If compression was requested, and the filesize is smaller than 5MB, proceed with
	//file compression for the minidump.
	const unsigned int maxSizeForCompression = (5*1024*1024);
	if(compress && (fileSize <= maxSizeForCompression))
	{
		//Seek to the start of the minidump file
		SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN);

		//Copy the mindump file into our buffer
		Stream::Buffer fileBuffer(fileSize);
		DWORD bytesRead;
		BOOL readFileReturn = ReadFile(fileHandle, fileBuffer.GetRawBuffer(), fileSize, &bytesRead, NULL);
		if(readFileReturn == 0)
		{
			CloseHandle(fileHandle);
			return false;
		}

		//Compress the minidump buffer
		ZIPArchive archive;
		ZIPFileEntry entry;
		entry.SetFileName(minidumpFileName + L".dmp");
		if(!entry.Compress(fileBuffer))
		{
			CloseHandle(fileHandle);
			return false;
		}
		archive.AddFileEntry(entry);

		//Generate the zip file name
		std::wstring minidumpZipFilePath = PathCombinePaths(minidumpFolder, minidumpFileName + L".zip");

		//Create the zip file
		Stream::File target;
		if(!target.Open(minidumpZipFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			CloseHandle(fileHandle);
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			CloseHandle(fileHandle);
			return false;
		}

		//Since we've succeeded in creating the zip file, delete the uncompressed minidump
		CloseHandle(fileHandle);
		DeleteFile(minidumpFilePath.c_str());
	}
	else
	{
		//If we're not compressing the minidump, close the minidump file.
		CloseHandle(fileHandle);
	}

	return true;
}

//----------------------------------------------------------------------------------------
LONG WINAPI MinidumpExceptionHandler(_EXCEPTION_POINTERS* exceptionPointers)
{
	//Get the minidump properties
	std::wstring minidumpPath = GetMinidumpPath();
	std::wstring minidumpName = GetMinidumpName();
	bool largeMinidump = GetLargeMinidumpFlag();

	//Generate a timestamp for the file name
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	std::wstringstream dateStream;
	dateStream << std::setfill(L'0') << std::setw(4) << systemTime.wYear << L'-' << std::setw(2) << systemTime.wMonth << L'-' << std::setw(2) << systemTime.wDay;
	std::wstring date = dateStream.str();
	std::wstringstream timeStream;
	timeStream << std::setfill(L'0') << std::setw(2) << systemTime.wHour << L'.' << std::setw(2) << systemTime.wMinute << L'.' << std::setw(2) << systemTime.wSecond;
	std::wstring time = timeStream.str();
	std::wstring timestamp = date + L'(' + time + L')';

	//Generate the full name of the minidump file, without the extension.
	std::wstring minidumpFileName = timestamp + L' ' + minidumpName;

	//Make sure the target directory exists
	CreateDirectory(minidumpPath, true);

	//Select what data to include in the minidump
	int minidumpType = MiniDumpNormal; //Start off with a normal minidump
	minidumpType |= MiniDumpWithThreadInfo; //Save some general thread info such as processor affinity, execution times, etc
	minidumpType |= MiniDumpWithProcessThreadData; //Save the process and thread environment block data
	minidumpType |= MiniDumpWithFullMemoryInfo; //Save complete info on the virtual memory layout
	minidumpType |= MiniDumpWithHandleData; //Save info about all current win32 handles
	minidumpType |= MiniDumpWithUnloadedModules; //Include info about recently unloaded modules
	minidumpType |= MiniDumpWithIndirectlyReferencedMemory; //Include snippets of data from the heap and other areas of memory which are referenced by pointers in registers or on the stack
	if(largeMinidump)
	{
		minidumpType |= MiniDumpWithDataSegs; //Include writeable sections of loaded modules. This preserves static variables and arrays. Can be quite large if big static arrays are allocated.
		minidumpType |= MiniDumpWithPrivateReadWriteMemory; //Include all private read/write pages of memory. This will not preserve static variables and arrays, but should fully preserve the heap (unless part of it has been marked as read-only). Can be quite large if a lot of data has been allocated off the heap.
	}

	//Build the filename for the minidump
	std::wstring minidumpFileNameFinal = minidumpFileName;
	if(largeMinidump)
	{
		minidumpFileNameFinal += L" - Large";
	}
	else
	{
		minidumpFileNameFinal += L" - Small";
	}

	//Write the minidump file
	GenerateMinidump(exceptionPointers, (MINIDUMP_TYPE)minidumpType, minidumpPath, minidumpFileNameFinal, true);

	//Continue with the normal exception handling process
	return EXCEPTION_CONTINUE_SEARCH;
}

//----------------------------------------------------------------------------------------
bool RegisterMinidumpExceptionHandler(const std::wstring& aminidumpName, const std::wstring& aminidumpPath, bool alargeMinidump)
{
	//Make sure the target directory is a fully qualified path
	std::wstring minidumpPathFull = aminidumpPath;
	if(PathIsRelativePath(minidumpPathFull))
	{
		//Get the current working directory
		std::wstring currentDirectory = PathGetCurrentWorkingDirectory();

		//Resolve the relative path, starting from the current working directory.
		minidumpPathFull = PathCombinePaths(currentDirectory, minidumpPathFull);
	}

	//Save the target filename, path, and large minidump flag.
	SetMinidumpName(aminidumpName);
	SetMinidumpPath(minidumpPathFull);
	SetLargeMinidumpFlag(alargeMinidump);

	//Register our exception handler
	SetUnhandledExceptionFilter(MinidumpExceptionHandler);

	return true;
}
