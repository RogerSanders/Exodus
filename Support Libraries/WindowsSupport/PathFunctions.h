#ifndef __PATHFUNCTIONS_H__
#define __PATHFUNCTIONS_H__
#include <string>
#include <vector>
#include <list>
#include "WindowFunctions.h"

//Path handling functions
//##TODO## Consider case sensitivity issues. Right now, all comparisons are case
//sensitive.
//##TODO## Fix the PathGetDirectory and PathGetFileName functions to handle prefixes
//correctly
std::wstring PathGetFileName(const std::wstring& path);
std::wstring PathGetFileExtension(const std::wstring& path);
std::wstring PathGetDirectory(const std::wstring& path);
bool PathIsRelativePath(const std::wstring& path);
std::wstring PathCombinePaths(const std::wstring& pathHead, const std::wstring& pathTail);
std::vector<std::wstring> SplitPathIntoComponents(const std::wstring& path);
bool PathStartsWithBasePath(const std::wstring& basePath, const std::wstring path);
bool PathBuildRelativePathToTarget(const std::wstring& sourcePath, const std::wstring& targetPath, bool targetIsFile, std::wstring& relativePath);
std::wstring PathGetCurrentWorkingDirectory();
void PathSetCurrentWorkingDirectory(const std::wstring& path);

//File and directory handling functions
struct FileSelectionType
{
	std::wstring typeName;
	std::list<std::wstring> extensionFilters;
};
std::list<FileSelectionType> ParseSelectionTypeString(const std::wstring& selectionTypeString);
bool SelectExistingFile(HWND parentWindow, const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath);
bool SelectExistingFile(HWND parentWindow, const std::list<FileSelectionType>& selectionTypes, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath);
bool SelectExistingDirectory(HWND parentWindow, const std::wstring& initialDirectory, std::wstring& selectedDirectoryPath);
bool SelectNewFile(HWND parentWindow, const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath);
bool SelectNewFile(HWND parentWindow, const std::list<FileSelectionType>& selectionTypes, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath);
bool CreateDirectory(const std::wstring& path, bool createIntermediateDirectories);

#endif
