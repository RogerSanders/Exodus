#include "PathFunctions.h"
#include "WindowFunctions.h"
#include <commctrl.h>
#include <shlobj.h>
#include <sstream>

//----------------------------------------------------------------------------------------
//Path handling functions
//----------------------------------------------------------------------------------------
std::wstring PathGetFileName(const std::wstring& path)
{
	const std::wstring pathSeparators = L"\\/";
	std::wstring::size_type lastPathSeparatorPos = path.find_last_of(pathSeparators);
	std::wstring fileName = path;
	if(lastPathSeparatorPos != std::wstring::npos)
	{
		fileName = path.substr(lastPathSeparatorPos + 1);
	}
	return fileName;
}

//----------------------------------------------------------------------------------------
std::wstring PathGetFileExtension(const std::wstring& path)
{
	const std::wstring pathSeparators = L"\\/";
	const std::wstring extensionSeparators = L".";
	std::wstring::size_type lastPathSeparatorPos = path.find_last_of(pathSeparators);
	std::wstring::size_type lastExtensionSeparatorPos = path.find_last_of(extensionSeparators);
	std::wstring fileExtension = L"";
	if((lastExtensionSeparatorPos != std::wstring::npos) && ((lastPathSeparatorPos == std::wstring::npos) || (lastPathSeparatorPos < lastExtensionSeparatorPos)))
	{
		fileExtension = path.substr(lastExtensionSeparatorPos + 1);
	}
	return fileExtension;
}

//----------------------------------------------------------------------------------------
std::wstring PathGetDirectory(const std::wstring& path)
{
	const std::wstring directorySeparators = L"\\/";
	std::wstring::size_type lastDirectorySeparatorPos = path.find_last_of(directorySeparators);
	std::wstring fileDirectory = L"";
	if(lastDirectorySeparatorPos != std::wstring::npos)
	{
		fileDirectory = path.substr(0, lastDirectorySeparatorPos + 1);
	}
	return fileDirectory;
}

//----------------------------------------------------------------------------------------
bool PathIsRelativePath(const std::wstring& path)
{
	bool absolutePath = false;
	if(path.size() >= 2)
	{
		absolutePath |= ((path[0] == L'\\') && (path[1] == L'\\'));
		absolutePath |= (path[1] == L':');
	}
	return !absolutePath;
}

//----------------------------------------------------------------------------------------
std::wstring PathCombinePaths(const std::wstring& pathHead, const std::wstring& pathTail)
{
	//##FIX## We have to do what PathCanonicalize does after performing this operation, so
	//that a relative path can be combined with a base path, and the properly combined
	//result of the two, taking into account relative path elements, is returned by this
	//function.
	std::wstring combinedPath = pathHead;
	if(!pathHead.empty())
	{
		wchar_t lastCharInHead = *pathHead.rbegin();
		if((lastCharInHead != L'\\') && (lastCharInHead != L'/'))
		{
			combinedPath += L'\\';
		}
	}
	combinedPath += pathTail;
	return combinedPath;
}

//----------------------------------------------------------------------------------------
bool PathStartsWithBasePath(const std::wstring& basePath, const std::wstring path)
{
	return (path.compare(0, basePath.length(), basePath) == 0);
}

//----------------------------------------------------------------------------------------
std::vector<std::wstring> SplitPathIntoComponents(const std::wstring& path)
{
	const std::wstring directorySeparators = L"\\/";
	std::vector<std::wstring> pathElements;
	std::wstring::size_type currentPos = 0;
	while(currentPos != std::wstring::npos)
	{
		//If this appears to be a UNC path, and we're looking at the first path element,
		//adjust the search position to combine the UNC path prefix, host, and volume name
		//together into a single path element.
		//##FIX## A UNC path or normal path is only valid if a trailing backslash appears
		//after the target volume specifier. We should therefore return the trailing
		//backslash here, otherwise this component isn't really referencing a directory at
		//all.
		//##FIX## The above functions that retrieve file and directory components don't
		//take the path prefix into account, and may return bad information.
		std::wstring::size_type searchPos = currentPos;
		if((searchPos == 0) && (PathStartsWithBasePath(L"\\\\", path) || PathStartsWithBasePath(L"//", path)))
		{
			std::wstring::size_type hostNameSeparatorPos = path.find_first_of(directorySeparators, currentPos + 2);
			if(hostNameSeparatorPos != std::wstring::npos)
			{
				std::wstring::size_type volumeNameSeparatorPos = path.find_first_of(directorySeparators, hostNameSeparatorPos);
				searchPos = volumeNameSeparatorPos;
			}
		}

		//Extract the next path element
		std::wstring::size_type separatorPos = path.find_first_of(directorySeparators, searchPos);
		std::wstring::size_type pathElementEndPos = (separatorPos != std::wstring::npos)? separatorPos - currentPos: std::wstring::npos;
		std::wstring pathElement = path.substr(currentPos, pathElementEndPos);

		//If the next path element isn't empty, add it to the list of path elements.
		if(!pathElement.empty())
		{
			pathElements.push_back(pathElement);
		}

		//Advance the current search position past this path element
		currentPos = (separatorPos != std::wstring::npos)? (separatorPos + 1): std::wstring::npos;
	}
	return pathElements;
}

//----------------------------------------------------------------------------------------
bool PathBuildRelativePathToTarget(const std::wstring& sourcePath, const std::wstring& targetPath, bool targetIsFile, std::wstring& relativePath)
{
	//Break the source and target paths into their components
	std::vector<std::wstring> sourcePathComponents = SplitPathIntoComponents(sourcePath);
	std::vector<std::wstring> targetPathComponents = SplitPathIntoComponents(targetPath);

	//If the target path references a file, extract the filename, and strip it from the
	//list of target path components.
	std::wstring targetFileName;
	if(targetIsFile && !targetPathComponents.empty())
	{
		targetFileName = *targetPathComponents.rbegin();
		targetPathComponents.pop_back();
	}

	//Count the number of base components that the source and target paths have in common
	unsigned int matchingComponentCount = 0;
	while((matchingComponentCount < (unsigned int)sourcePathComponents.size())
	   && (matchingComponentCount < (unsigned int)targetPathComponents.size())
	   && (sourcePathComponents[matchingComponentCount] == targetPathComponents[matchingComponentCount]))
	{
		++matchingComponentCount;
	}

	//If the source and target paths don't have any common base components, return false.
	if(matchingComponentCount == 0)
	{
		return false;
	}

	//Clear the supplied relative path string
	relativePath.clear();

	//Add components to the relative path string to move up from the source directory back
	//to the first common base directory
	unsigned int sourcePathComponentsToLeave = (unsigned int)sourcePathComponents.size() - matchingComponentCount;
	for(unsigned int i = 0; i < sourcePathComponentsToLeave; ++i)
	{
		relativePath = PathCombinePaths(relativePath, L"..");
	}

	//Add components from the relative path string to move from the first common base
	//directory down into the target directory
	for(unsigned int i = matchingComponentCount; i < (unsigned int)targetPathComponents.size(); ++i)
	{
		relativePath = PathCombinePaths(relativePath, targetPathComponents[i]);
	}

	//If the target path references a file, add the filename to the relative path string.
	if(targetIsFile)
	{
		relativePath = PathCombinePaths(relativePath, targetFileName);
	}

	return true;
}

//----------------------------------------------------------------------------------------
std::wstring PathGetCurrentWorkingDirectory()
{
	//Attempt to obtain the current working directory
	std::wstring currentDirectory = L"";
	bool pathRead = false;
	bool operationAborted = false;
	while(!pathRead && !operationAborted)
	{
		//Attempt to determine the required buffer size to read in the current directory
		DWORD requiredBufferSize = GetCurrentDirectory(0, NULL);
		if(requiredBufferSize <= 0)
		{
			operationAborted = true;
			continue;
		}

		//Create a buffer to hold the retrieved path
		TCHAR* pathBuffer = new TCHAR[requiredBufferSize];

		//Attempt to retrieve the current directory
		DWORD getCurrentDirectoryReturn = GetCurrentDirectory(requiredBufferSize, pathBuffer);
		if(getCurrentDirectoryReturn == 0)
		{
			delete[] pathBuffer;
			operationAborted = true;
			continue;
		}

		//In the unlikely event that the current directory has been changed and grown in
		//length since we obtained the required buffer size, delete the buffer we created
		//and try again.
		if(getCurrentDirectoryReturn > requiredBufferSize)
		{
			delete[] pathBuffer;
			continue;
		}

		//Retrieve the current directory from the buffer, delete the buffer, and flag that
		//we've managed to retrieve the current directory.
		currentDirectory = pathBuffer;
		delete[] pathBuffer;
		pathRead = true;
	}

	//Return the retrieved directory if the operation succeeded, or an empty string if it
	//failed.
	return currentDirectory;
}

//----------------------------------------------------------------------------------------
void PathSetCurrentWorkingDirectory(const std::wstring& path)
{
	//Attempt to set the current working directory, looping on failure up to a maximum
	//number of retries.
	unsigned int maxRetryCount = 10;
	unsigned int retryCount = 0;
	bool done = false;
	while(!done && (retryCount < maxRetryCount))
	{
		//Attempt to set the current directory
		BOOL setCurrentDirectoryReturn = SetCurrentDirectory(path.c_str());
		if(setCurrentDirectoryReturn == 0)
		{
			++retryCount;
			continue;
		}

		//Flag that the current directory was set successfully
		done = true;
	}
}

//----------------------------------------------------------------------------------------
//File and directory handling functions
//----------------------------------------------------------------------------------------
std::list<FileSelectionType> ParseSelectionTypeString(const std::wstring& selectionTypeString)
{
	//Extract all the type elements from the selection type string
	const std::wstring entrySeparators = L";";
	std::list<std::wstring> typeElementStrings;
	std::wstring::size_type typeStringCurrentPos = 0;
	while(typeStringCurrentPos != std::wstring::npos)
	{
		//Extract the next complete entry string inside the selection type string
		std::wstring::size_type separatorPos = selectionTypeString.find_first_of(entrySeparators, typeStringCurrentPos);
		std::wstring::size_type typeElementEndPos = (separatorPos != std::wstring::npos)? separatorPos - typeStringCurrentPos: std::wstring::npos;
		std::wstring typeElement = selectionTypeString.substr(typeStringCurrentPos, typeElementEndPos);
		typeElementStrings.push_back(typeElement);
		typeStringCurrentPos = (separatorPos != std::wstring::npos)? (separatorPos + 1): std::wstring::npos;
	}

	//Decode the extacted type element strings, and build a FileSelectionType structure
	//for each one.
	std::list<FileSelectionType> selectionTypes;
	for(std::list<std::wstring>::const_iterator i = typeElementStrings.begin(); i != typeElementStrings.end(); ++i)
	{
		//Extract the next type element, and create a new selection type structure for
		//this type element.
		std::wstring typeElement = *i;
		FileSelectionType selectionType;

		//Decode the type element string, and load the decoded data into the selection
		//type structure.
		const std::wstring extensionSeparators = L"|";
		bool firstTypeElementEntry = true;
		std::wstring::size_type typeElementCurrentPos = 0;
		while(typeElementCurrentPos != std::wstring::npos)
		{
			//Extract the next entry from the type element
			std::wstring::size_type typeElementSeparatorPos = typeElement.find_first_of(extensionSeparators, typeElementCurrentPos);
			std::wstring::size_type typeExtensionEndPos = (typeElementSeparatorPos != std::wstring::npos)? typeElementSeparatorPos - typeElementCurrentPos: std::wstring::npos;
			std::wstring typeElementEntry = typeElement.substr(typeElementCurrentPos, typeExtensionEndPos);

			//If this is the first entry in the type element string, add it as the type
			//name, otherwise add it to the list of type extensions.
			if(firstTypeElementEntry)
			{
				selectionType.typeName = typeElementEntry;
				firstTypeElementEntry = false;
			}
			else
			{
				selectionType.extensionFilters.push_back(typeElementEntry);
			}

			//Advance to the next entry in the type element string
			typeElementCurrentPos = (typeElementSeparatorPos != std::wstring::npos)? (typeElementSeparatorPos + 1): std::wstring::npos;
		}

		//Add this selection type structure to the list of selection types
		selectionTypes.push_back(selectionType);
	}

	//Return the list of decoded selection types
	return selectionTypes;
}

//----------------------------------------------------------------------------------------
std::wstring BuildCommonFileDialogTypeFilterString(const std::list<FileSelectionType>& typeFilters)
{
	//Build a filter string using the appropriate format for the GetOpenFileName and
	//GetSaveFileName functions
	std::wstringstream filterStringStream;
	for(std::list<FileSelectionType>::const_iterator i = typeFilters.begin(); i != typeFilters.end(); ++i)
	{
		//Build a string for the extension filters
		std::wstringstream extensionFilterStringStream;
		bool firstExtensionFilter = true;
		for(std::list<std::wstring>::const_iterator extensionFiltersIterator = i->extensionFilters.begin(); extensionFiltersIterator != i->extensionFilters.end(); ++extensionFiltersIterator)
		{
			if(!firstExtensionFilter)
			{
				extensionFilterStringStream << L";";
			}
			extensionFilterStringStream << L"*." << *extensionFiltersIterator;
			firstExtensionFilter = false;
		}
		std::wstring extensionFilterString = extensionFilterStringStream.str();

		//Write this filter name to the filter string
		filterStringStream << i->typeName << L" (" << extensionFilterString << L")";
		filterStringStream.put(L'\0');

		//Write the extension filters for this entry to the filter string
		filterStringStream << extensionFilterString;
		filterStringStream.put(L'\0');
	}
	filterStringStream.put(L'\0');

	//Return the filter string
	std::wstring filterString = filterStringStream.str();
	return filterString;
}

//----------------------------------------------------------------------------------------
bool SelectExistingFile(HWND parentWindow, const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath)
{
	//Break the supplied selection type string down into individual type entries
	std::list<FileSelectionType> selectionTypes = ParseSelectionTypeString(selectionTypeString);

	//Perform the file selection operation
	return SelectExistingFile(parentWindow, selectionTypes, defaultExtension, initialFilePath, initialDirectory, selectedFilePath);
}

//----------------------------------------------------------------------------------------
bool SelectExistingFile(HWND parentWindow, const std::list<FileSelectionType>& selectionTypes, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath)
{
	//Build a modified selection type list, adding the "all" selection type, so the user
	//can easily select alternate files not covered by the supplied type filter strings.
	std::list<FileSelectionType> fullSelectionTypes = selectionTypes;
	FileSelectionType allType;
	allType.typeName = L"All";
	allType.extensionFilters.push_back(L"*");
	fullSelectionTypes.push_back(allType);

	//Build a filter string for the specified type filters which is compatible with the
	//open file dialog
	std::wstring filterString = BuildCommonFileDialogTypeFilterString(fullSelectionTypes);

	//Build a new buffer for the target filename, with some default initialization to add
	//some safety against buffer overflows.
	TCHAR fileNameBuffer[MAX_PATH+1];
	fileNameBuffer[MAX_PATH] = L'\0';
	fileNameBuffer[0] = L'\0';

	//Select the initial directory path and file path to use in order to populate the
	//initially selected content of the dialog.
	std::wstring initialDirectoryToUse = initialDirectory;
	if(!initialFilePath.empty())
	{
		if(PathGetFileName(initialFilePath).empty())
		{
			//If an initial file path has been specified, but there isn't any actual
			//filename component specified in the path, and no initial directory path has
			//been specified, use the directory path supplied in the initial file path as
			//the initial directory path. If an initial directory path has been specified,
			//ignore the invalid initial file path. We do this because if a path is passed
			//to the dialog as the initial file path, and there is no file name component,
			//the dialog will fail with the FNERR_INVALIDFILENAME error code.
			if(initialDirectoryToUse.empty())
			{
				initialDirectoryToUse = PathGetDirectory(initialFilePath);
			}
		}
		else
		{
			//If the supplied initial file path appears to be valid, use it to populate
			//the initial file path of the dialog.
			unsigned int pathCharIndex = 0;
			while((pathCharIndex < (unsigned int)initialFilePath.size()) && (pathCharIndex < MAX_PATH))
			{
				fileNameBuffer[pathCharIndex] = initialFilePath[pathCharIndex];
				++pathCharIndex;
			}
			fileNameBuffer[pathCharIndex] = L'\0';
		}
	}

	//Build the OPENFILENAME structure for the open file dialog
	OPENFILENAME openFileParams;
	ZeroMemory(&openFileParams, sizeof(openFileParams));
	openFileParams.lStructSize = sizeof(openFileParams);
	openFileParams.hwndOwner = parentWindow;
	openFileParams.lpstrFile = fileNameBuffer;
	openFileParams.nMaxFile = (sizeof(fileNameBuffer) / sizeof(fileNameBuffer[0]))-1;
	openFileParams.lpstrFilter = &filterString[0];
	openFileParams.lpstrDefExt = &defaultExtension[0];
	openFileParams.nFilterIndex = 1;
	openFileParams.lpstrFileTitle = NULL;
	openFileParams.nMaxFileTitle = 0;
	openFileParams.lpstrInitialDir = NULL;
	if(!initialDirectoryToUse.empty())
	{
		openFileParams.lpstrInitialDir = &initialDirectoryToUse[0];
	}
	openFileParams.Flags = OFN_HIDEREADONLY | OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//Save the current working directory before launching the open file dialog. For some
	//reason, no option is given to retain the directory when using the GetOpenFileName
	//dialog. The OFN_NOCHANGEDIR flag only works with GetSaveFileName.
	std::wstring currentWorkingDirectory = PathGetCurrentWorkingDirectory();

	//Attempt to select a file using the open file dialog
	BOOL openDialogReturn = GetOpenFileName(&openFileParams);
	if(openDialogReturn == 0)
	{
		//If the dialog failed with an actual error, rather than the user just canceling
		//the operation, display that error to the user.
		DWORD dialogErrorCode = CommDlgExtendedError();
		if(dialogErrorCode != 0)
		{
			std::wstring title = L"Error selecting file!";
			std::wstringstream textStream;
			textStream << L"GetOpenFileName failed with error code " << dialogErrorCode;
			std::wstring text = textStream.str();
			MessageBox(parentWindow, text.c_str(), title.c_str(), MB_ICONEXCLAMATION);
		}

		//Since the selection process has failed, return false.
		return false;
	}

	//Restore the current working directory after the file selection
	PathSetCurrentWorkingDirectory(currentWorkingDirectory);

	//Return the selected file path to the user
	selectedFilePath = openFileParams.lpstrFile;
	return true;
}

//----------------------------------------------------------------------------------------
bool SelectNewFile(HWND parentWindow, const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath)
{
	//Break the supplied selection type string down into individual type entries
	std::list<FileSelectionType> selectionTypes = ParseSelectionTypeString(selectionTypeString);

	//Perform the file selection operation
	return SelectNewFile(parentWindow, selectionTypes, defaultExtension, initialFilePath, initialDirectory, selectedFilePath);
}

//----------------------------------------------------------------------------------------
bool SelectNewFile(HWND parentWindow, const std::list<FileSelectionType>& selectionTypes, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath)
{
	//Build a modified selection type list, adding the "all" selection type, so the user
	//can easily select alternate files not covered by the supplied type filter strings.
	std::list<FileSelectionType> fullSelectionTypes = selectionTypes;
	FileSelectionType allType;
	allType.typeName = L"All";
	allType.extensionFilters.push_back(L"*");
	fullSelectionTypes.push_back(allType);

	//Build a filter string for the specified type filters which is compatible with the
	//open file dialog
	std::wstring filterString = BuildCommonFileDialogTypeFilterString(fullSelectionTypes);

	//Build a new buffer for the target filename, with some default initialization to add
	//some safety against buffer overflows.
	TCHAR fileNameBuffer[MAX_PATH+1];
	fileNameBuffer[MAX_PATH] = L'\0';
	fileNameBuffer[0] = L'\0';

	//Use the initial file path to populate the initially selected content of the dialog
	unsigned int pathCharIndex = 0;
	while((pathCharIndex < (unsigned int)initialFilePath.size()) && (pathCharIndex < MAX_PATH))
	{
		fileNameBuffer[pathCharIndex] = initialFilePath[pathCharIndex];
		++pathCharIndex;
	}
	fileNameBuffer[pathCharIndex] = L'\0';

	//Build the OPENFILENAME structure for the save file dialog
	OPENFILENAME openFileParams;
	ZeroMemory(&openFileParams, sizeof(openFileParams));
	openFileParams.lStructSize = sizeof(openFileParams);
	openFileParams.hwndOwner = parentWindow;
	openFileParams.lpstrFile = fileNameBuffer;
	openFileParams.nMaxFile = (sizeof(fileNameBuffer) / sizeof(fileNameBuffer[0]))-1;
	openFileParams.lpstrFilter = &filterString[0];
	openFileParams.lpstrDefExt = &defaultExtension[0];
	openFileParams.nFilterIndex = 1;
	openFileParams.lpstrFileTitle = NULL;
	openFileParams.nMaxFileTitle = 0;
	openFileParams.lpstrInitialDir = NULL;
	if(!initialDirectory.empty())
	{
		openFileParams.lpstrInitialDir = &initialDirectory[0];
	}
	//Note that the OFN_HIDEREADONLY flag alters the dialog template slightly, even
	//though the read only checkbox is never displayed either way. We include the flag
	//in the save dialog to keep the look consistent with the open dialog, where we do
	//want to include this flag.
	openFileParams.Flags = OFN_HIDEREADONLY | OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	//Attempt to select a file using the save file dialog
	BOOL saveDialogReturn = GetSaveFileName(&openFileParams);
	if(saveDialogReturn == 0)
	{
		//If the dialog failed with an actual error, rather than the user just canceling
		//the operation, display that error to the user.
		DWORD dialogErrorCode = CommDlgExtendedError();
		if(dialogErrorCode != 0)
		{
			std::wstring title = L"Error selecting file!";
			std::wstringstream textStream;
			textStream << L"GetSaveFileName failed with error code " << dialogErrorCode;
			std::wstring text = textStream.str();
			MessageBox(parentWindow, text.c_str(), title.c_str(), MB_ICONEXCLAMATION);
		}

		//Since the selection process has failed, return false.
		return false;
	}

	//Return the selected file path to the user
	selectedFilePath = openFileParams.lpstrFile;
	return true;
}

//----------------------------------------------------------------------------------------
int CALLBACK SetSHBrowseForFolderInitialDir(HWND hwnd, UINT umsg, LPARAM lparam, LPARAM lpData)
{
	if(umsg == BFFM_INITIALIZED)
	{
		//When the dialog is initialized, try and set the initial selection on the tree.
		LPTSTR pathName = (LPTSTR)lpData;
		LPTSTR filePart;
		TCHAR fullPathName[MAX_PATH+1];
		fullPathName[MAX_PATH] = L'\0';
		fullPathName[0] = L'\0';
		DWORD getFullPathNameReturn = GetFullPathName(pathName, MAX_PATH, &fullPathName[0], &filePart);
		if(getFullPathNameReturn != 0)
		{
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)&fullPathName[0]);
		}
	}
	else if(umsg == BFFM_SELCHANGED)
	{
		//Note that on Windows 7 and up, the BFFM_SETSELECTION message doesn't always
		//work. Sometimes the tree isn't fully loaded by this point, and the folder can't
		//be scrolled to because it hasn't been loaded into the tree control yet.
		//Microsoft originally acknowledged the bug, but they appear to have now abandoned
		//it and won't fix it. The only two known workarounds are to either fire off extra
		//BFFM_SETSELECTION messages on a delay, or do what we do below, which is to
		//manually locate the tree control in the dialog, retrieve its current selection,
		//and force it to ensure the selected item is visible. Although this solution is
		//bound to internal details, it's preferable to the timer method, since it will
		//definitely make the dialog always work correctly regardless of how long it takes
		//to load, and it won't interfere with input from the user, which a timer method
		//could, since it has no way of detecting when the selection has been set.
		const int subPaneDialogItemID = 0x0;
		const int treeDialogItemID = 0x64;
		HWND subPaneHandle = GetDlgItem(hwnd, subPaneDialogItemID);
		if(subPaneHandle != 0)
		{
			HWND treeHandle = GetDlgItem(subPaneHandle, treeDialogItemID);
			if(treeHandle != 0)
			{
				HTREEITEM selectedItemHandle = (HTREEITEM)SendMessage(treeHandle, TVM_GETNEXTITEM, TVGN_CARET, 0);
				SendMessage(treeHandle, TVM_ENSUREVISIBLE, 0, (LPARAM)selectedItemHandle);
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
bool SelectExistingDirectory(HWND parentWindow, const std::wstring& initialDirectory, std::wstring& selectedDirectoryPath)
{
	TCHAR folderDataBuffer[MAX_PATH+1];
	folderDataBuffer[MAX_PATH] = L'\0';
	folderDataBuffer[0] = L'\0';

	//Build the BROWSEINFO structure
	std::wstring messageText = L"Please select the target folder";
	BROWSEINFO info;
	info.hwndOwner = parentWindow;
	info.pidlRoot = NULL;
	info.pszDisplayName = folderDataBuffer;
	info.lpszTitle = &messageText[0];
	info.ulFlags = BIF_USENEWUI;
	info.lpfn = SetSHBrowseForFolderInitialDir;
	info.lParam = (LPARAM)&initialDirectory[0];

	//Attempt to obtain a folder selection from the user
	LPITEMIDLIST itemIDList;
	itemIDList = SHBrowseForFolder(&info);
	if(itemIDList == NULL)
	{
		return false;
	}

	//Obtain a path from the folder selection results
	if(SHGetPathFromIDList(itemIDList, &folderDataBuffer[0]) != TRUE)
	{
		CoTaskMemFree(itemIDList);
		return false;
	}

	//Return the selected path to the user
	selectedDirectoryPath = folderDataBuffer;
	CoTaskMemFree(itemIDList);
	return true;
}

//----------------------------------------------------------------------------------------
bool CreateDirectoryInternal(const std::wstring& path)
{
	//Obtain file attributes for the element referenced by the target path
	DWORD fileAttributes = GetFileAttributes(path.c_str());

	//If the file attributes couldn't be retrieved, assume the folder doesn't exist and
	//create it, otherwise verify that the target path references a folder.
	if(fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		//Attempt to create the target folder, and return false if it fails.
		BOOL createDirectoryReturn = CreateDirectory(path.c_str(), NULL);
		if(createDirectoryReturn == 0)
		{
			return false;
		}
	}
	else
	{
		//If this path element references a file rather than a folder, return false.
		if((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool CreateDirectory(const std::wstring& path, bool createIntermediateDirectories)
{
	//If we haven't been requested to create intermediate directories, attempt to directly
	//create the target folder, and return the result of the operation.
	if(!createIntermediateDirectories)
	{
		return CreateDirectoryInternal(path);
	}

	//Split the target path into its components
	std::vector<std::wstring> pathComponents = SplitPathIntoComponents(path);

	//Iterate through each path component and ensure a directory exists at each level of
	//the path
	std::wstring currentPath;
	for(unsigned int i = 0; i < (unsigned int)pathComponents.size(); ++i)
	{
		//Add the next path component to the current path string
		currentPath = PathCombinePaths(currentPath, pathComponents[i]);

		//Attempt to create the directory referenced by this level in the path string, and
		//return false if the operation fails.
		if(!CreateDirectoryInternal(currentPath))
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool CreateDirectoryAndAllIntermediateDirectories(const std::wstring& path)
{
	//Split the target path into its components
	std::vector<std::wstring> pathComponents = SplitPathIntoComponents(path);

	//Iterate through each path component and ensure a directory exists at each level of
	//the path
	std::wstring currentPath;
	for(unsigned int i = 0; i < (unsigned int)pathComponents.size(); ++i)
	{
		//Add the next path component to the current path string
		currentPath = PathCombinePaths(currentPath, pathComponents[i]);

		//Obtain file attributes for the element referenced by the target path
		DWORD fileAttributes = GetFileAttributes(currentPath.c_str());

		//If the file attributes couldn't be retrieved, assume the folder doesn't exist
		//and create it, otherwise verify that the target path references a folder.
		if(fileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			//Attempt to create the target folder, and return false if it fails.
			BOOL createDirectoryReturn = CreateDirectory(currentPath.c_str(), NULL);
			if(createDirectoryReturn == 0)
			{
				return false;
			}
		}
		else
		{
			//If this path element references a file rather than a folder, return false.
			if((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				return false;
			}
		}
	}

	return true;
}
