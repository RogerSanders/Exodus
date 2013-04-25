#include "System.h"
#include "HeirarchicalStorage/HeirarchicalStorage.pkg"
#include "Stream/Stream.pkg"
#include "ZIP/ZIP.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Image/Image.pkg"
#include <time.h>
#include <boost/bind.hpp>
#include <sstream>
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int System::GetISystemExtensionInterfaceVersion() const
{
	return ThisISystemExtensionInterfaceVersion();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void System::BindToGUIExtensionInterface(IGUIExtensionInterface* aguiExtensionInterface)
{
	guiExtensionInterface = aguiExtensionInterface;
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool System::LoadState(const std::wstring& filePath, FileType fileType, bool debuggerState)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Open the target file
	FileStreamReference sourceStreamReference(*guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the file could not be opened!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}
	Stream::IStream& source = *sourceStreamReference;

	HeirarchicalStorageTree tree;
	if(fileType == FILETYPE_ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the zip file structure could not be decoded!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Load XML tree from file
		ZIPFileEntry* entry = archive.GetFileEntry(L"save.xml");
		if(entry == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the save.xml file could not be found within the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because there was an error decompressing the save.xml file from the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Load external binary data into the XML tree
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			ZIPFileEntry* entry = archive.GetFileEntry(binaryFileName);
			if(entry == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found within the zip archive!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry->Decompress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because there was an error decompressing the binary data file " + binaryFileName + L" from the zip archive!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		//Determine the text format for the target file
		source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		source.ProcessByteOrderMark();

		//Attempt to load the XML tree from the file
		if(!tree.LoadTree(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Load external binary data into the XML tree
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = binaryFileName;

			//If the file path contains a relative path to the target, resolve the relative
			//file path using the directory containing the module file as a base.
			if(PathIsRelativePath(binaryFilePath))
			{
				binaryFilePath = PathCombinePaths(fileDir, binaryFilePath);
			}

			//Open the target file
			FileStreamReference binaryFileStreamReference(*guiExtensionInterface);
			if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			Stream::IStream& binaryFile = *binaryFileStreamReference;

			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);

			unsigned int bufferSize = (unsigned int)binaryFile.Size();
			unsigned char* buffer = new unsigned char[bufferSize];
			if(!binaryFile.ReadData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!binaryData.WriteData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			delete[] buffer;
		}
	}

	//Validate the root node
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"State")
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the root node in the XML tree wasn't of type \"State\"!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}

	//Restore system state from XML data
	ModuleRelationshipMap relationshipMap;
	std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring elementName = (*i)->GetName();

		//Load the device node
		if(elementName == L"Device")
		{
			//Extract the mandatory attributes
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"Name");
			IHeirarchicalStorageAttribute* moduleIDAttribute = (*i)->GetAttribute(L"ModuleID");
			if((nameAttribute != 0) && (moduleIDAttribute != 0))
			{
				std::wstring deviceName = nameAttribute->GetValue();
				unsigned int savedModuleID = moduleIDAttribute->ExtractValue<unsigned int>();

				//Attempt to locate a matching loaded device
				bool foundDevice = false;
				IDevice* device = 0;
				ModuleRelationshipMap::const_iterator relationshipMapIterator = relationshipMap.find(savedModuleID);
				if(relationshipMapIterator != relationshipMap.end())
				{
					const ModuleRelationship& moduleRelationship = relationshipMapIterator->second;
					if(moduleRelationship.foundMatch)
					{
						device = GetDevice(moduleRelationship.loadedModuleID, deviceName);
						if(device != 0)
						{
							foundDevice = true;
						}
					}
				}

				//If we found a matching device, load the state for this device.
				if(foundDevice)
				{
					if(debuggerState)
					{
						device->LoadDebuggerState(*(*i));
					}
					else
					{
						//Note that we negate the output line state here, and re-assert it
						//after loading the state data. This is technically unnecessary
						//when loading complete system states, but is very important when
						//loading partial system states.
						device->NegateCurrentOutputLineState();
						device->LoadState(*(*i));
						device->AssertCurrentOutputLineState();
					}
				}

				//If a matching loaded device couldn't be located, log an error.
				if(!foundDevice)
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"While loading state data from file " + filePath + L" state data was found for device " + deviceName + L" , which could not be located in the system. The state data for this device will be ignored, and the state will continue to load, but note that the system may not run as expected."));
				}
			}
		}
		//Load the ModuleRelationships node
		else if(elementName == L"ModuleRelationships")
		{
			if(!LoadModuleRelationshipsNode(*(*i), relationshipMap))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + filePath + L" because the ModuleRelationships node could not be loaded!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized element: " + elementName + L" when loading state from file " + filePath + L"."));
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Loaded state from file " + filePath));

	//Restore running state
	if(running)
	{
		RunSystem();
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool System::SaveState(const std::wstring& filePath, FileType fileType, bool debuggerState)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Create the new savestate XML tree
	HeirarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"State");

	//Fill in general information about the savestate
	IHeirarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
	Timestamp timestamp = GetTimestamp();
	stateInfo.CreateAttribute(L"CreationDate", timestamp.GetDate());
	stateInfo.CreateAttribute(L"CreationTime", timestamp.GetTime());
	stateInfo.CreateAttribute(L"DebuggerState", debuggerState);
	Image screenshot;
	bool screenshotPresent = false;
	std::wstring screenshotFilename = L"screenshot.png";
	if(!debuggerState)
	{
		for(DeviceArray::const_iterator i = devices.begin(); i != devices.end(); ++i)
		{
			screenshotPresent |= (*i)->GetTargetDevice()->GetScreenshot(screenshot);
		}
		if(screenshotPresent)
		{
			stateInfo.CreateAttribute(L"Screenshot", screenshotFilename);
		}
	}

	//Save the ModuleRelationships node
	IHeirarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	SaveModuleRelationshipsNode(moduleRelationshipsNode);

	//Save the system state to the XML tree
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		IHeirarchicalStorageNode& node = tree.GetRootNode().CreateChild(L"Device");
		node.CreateAttribute(L"Name", (*i).device->GetDeviceInstanceName());
		node.CreateAttribute(L"ModuleID").SetValue((*i).moduleID);
		if(debuggerState)
		{
			(*i).device->SaveDebuggerState(node);
		}
		else
		{
			(*i).device->SaveState(node);
		}
	}

	if(fileType == FILETYPE_ZIP)
	{
		//Save the XML tree to a unicode buffer
		Stream::Buffer buffer(Stream::IStream::TEXTENCODING_UTF8, 0);
		buffer.InsertByteOrderMark();
		if(!tree.SaveTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		ZIPArchive archive;
		ZIPFileEntry entry;
		entry.SetFileName(L"save.xml");
		buffer.SetStreamPos(0);
		if(!entry.Compress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the save.xml file!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		archive.AddFileEntry(entry);

		//Save external binary data to separate files
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			ZIPFileEntry entry;
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			entry.SetFileName(binaryFileName);
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry.Compress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the " + binaryFileName + L" file!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			archive.AddFileEntry(entry);
		}

		//Add the screenshot file
		if(screenshotPresent)
		{
			ZIPFileEntry entry;
			entry.SetFileName(screenshotFilename);
			Stream::Buffer screenshotFile(0);
			if(!screenshot.SavePNGImage(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the screenshot file with a file name of " + screenshotFilename + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			screenshotFile.SetStreamPos(0);
			if(!entry.Compress(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the " + screenshotFilename + L" file!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			archive.AddFileEntry(entry);
		}

		//Create the target file
		Stream::File target;
		if(!target.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the zip structure to the file!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		//Save XML tree to the target file
		Stream::File file(Stream::IStream::TEXTENCODING_UTF8);
		if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		file.InsertByteOrderMark();
		if(!tree.SaveTree(file))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Save external binary data to separate files
		std::wstring fileName = PathGetFileName(filePath);
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = fileName + L" - " + (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = PathCombinePaths(fileDir, binaryFileName);
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the binary data file " + binaryFileName + L" at the full path of " + binaryFilePath + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}

			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			while(!binaryData.IsAtEnd())
			{
				unsigned char temp;
				if(!binaryData.ReadData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error reading the source data from memory to save to the binary data file " + binaryFileName + L"!"));
					if(running)
					{
						RunSystem();
					}
					return false;
				}
				if(!binaryFile.WriteData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error writing to the binary data file " + binaryFileName + L"!"));
					if(running)
					{
						RunSystem();
					}
					return false;
				}
			}
		}

		//Save the screenshot file
		if(screenshotPresent)
		{
			std::wstring screenshotFilenameFull = fileName + L" - " + screenshotFilename;
			std::wstring screenshotFilePath = PathCombinePaths(fileDir, screenshotFilenameFull);
			Stream::File screenshotFile;
			if(!screenshotFile.Open(screenshotFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the screenshot file with a file name of " + screenshotFilenameFull + L" with a full path of " + screenshotFilePath + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!screenshot.SavePNGImage(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the screenshot to the " + screenshotFilenameFull + L" file!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Saved state to file " + filePath));

	//Restore running state
	if(running)
	{
		RunSystem();
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadPersistentStateForModule(const std::wstring& filePath, unsigned int moduleID, FileType fileType, bool returnSuccessOnNoFilePresent)
{
	//Open the target file
	FileStreamReference sourceStreamReference(*guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		if(returnSuccessOnNoFilePresent)
		{
			return true;
		}
		else
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the file could not be opened!"));
			return false;
		}
	}
	Stream::IStream& source = *sourceStreamReference;

	HeirarchicalStorageTree tree;
	if(fileType == FILETYPE_ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the zip file structure could not be decoded!"));
			return false;
		}

		//Load XML tree from file
		ZIPFileEntry* entry = archive.GetFileEntry(L"save.xml");
		if(entry == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the save.xml file could not be found within the zip archive!"));
			return false;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error decompressing the save.xml file from the zip archive!"));
			return false;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Load external binary data into the XML tree
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			ZIPFileEntry* entry = archive.GetFileEntry(binaryFileName);
			if(entry == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found within the zip archive!"));
				return false;
			}
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry->Decompress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error decompressing the binary data file " + binaryFileName + L" from the zip archive!"));
				return false;
			}
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		//Determine the text format for the target file
		source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		source.ProcessByteOrderMark();

		//Attempt to load the XML tree from the file
		if(!tree.LoadTree(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Load external binary data into the XML tree
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = binaryFileName;

			//If the file path contains a relative path to the target, resolve the relative
			//file path using the directory containing the module file as a base.
			if(PathIsRelativePath(binaryFilePath))
			{
				binaryFilePath = PathCombinePaths(fileDir, binaryFilePath);
			}

			//Open the target file
			FileStreamReference binaryFileStreamReference(*guiExtensionInterface);
			if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
				return false;
			}
			Stream::IStream& binaryFile = *binaryFileStreamReference;

			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);

			unsigned int bufferSize = (unsigned int)binaryFile.Size();
			unsigned char* buffer = new unsigned char[bufferSize];
			if(!binaryFile.ReadData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
				return false;
			}
			if(!binaryData.WriteData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
				return false;
			}
			delete[] buffer;
		}
	}

	//Validate the root node
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"PersistentState")
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load persistent state from file " + filePath + L" because the root node in the XML tree wasn't of type \"PersistentState\"!"));
		return false;
	}

	//Restore persistent system state from XML data
	std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring elementName = (*i)->GetName();

		//Load the device node
		if(elementName == L"Device")
		{
			//Extract the mandatory attributes
			IHeirarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"Name");
			if(nameAttribute != 0)
			{
				std::wstring deviceName = nameAttribute->GetValue();

				//Attempt to locate a matching loaded device
				bool foundDevice = false;
				IDevice* device = GetDevice(moduleID, deviceName);
				if(device != 0)
				{
					foundDevice = true;
				}

				//If we found a matching device, load the state for this device.
				if(foundDevice)
				{
					//Note that we negate the output line state here, and re-assert it
					//after loading the state data. This is technically unnecessary
					//when loading complete system states, but is very important when
					//loading partial system states.
					device->NegateCurrentOutputLineState();
					device->LoadState(*(*i));
					device->AssertCurrentOutputLineState();
				}

				//If a matching loaded device couldn't be located, log an error.
				if(!foundDevice)
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"While loading persistent state data from file " + filePath + L" state data was found for device " + deviceName + L" , which could not be located in the system. The state data for this device will be ignored, and the state will continue to load, but note that the system may not run as expected."));
				}
			}
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized element: " + elementName + L" when loading persistent state from file " + filePath + L"."));
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Loaded persistent state from file " + filePath));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::SavePersistentStateForModule(const std::wstring& filePath, unsigned int moduleID, FileType fileType, bool generateNoFileIfNoContentPresent)
{
	//Create the new savestate XML tree
	HeirarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"PersistentState");

	//Save the system state to the XML tree
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(i->moduleID == moduleID)
		{
			IHeirarchicalStorageNode& deviceNode = tree.GetRootNode().CreateChild(L"Device");
			(*i).device->SavePersistentState(deviceNode);
			if(deviceNode.IsEmpty())
			{
				tree.GetRootNode().DeleteChild(deviceNode);
				continue;
			}
			deviceNode.CreateAttribute(L"Name", (*i).device->GetDeviceInstanceName());
		}
	}

	//If we've been requested not to generate a file if no content is present, and no
	//devices saved any persistent state, abort any further processing, and return true.
	if(generateNoFileIfNoContentPresent && tree.GetRootNode().IsEmpty())
	{
		return true;
	}

	//Fill in general information about the savestate
	IHeirarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
	Timestamp timestamp = GetTimestamp();
	stateInfo.CreateAttribute(L"CreationDate", timestamp.GetDate());
	stateInfo.CreateAttribute(L"CreationTime", timestamp.GetTime());

	if(fileType == FILETYPE_ZIP)
	{
		//Save the XML tree to a unicode buffer
		Stream::Buffer buffer(Stream::IStream::TEXTENCODING_UTF8, 0);
		buffer.InsertByteOrderMark();
		if(!tree.SaveTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		ZIPArchive archive;
		ZIPFileEntry entry;
		entry.SetFileName(L"save.xml");
		buffer.SetStreamPos(0);
		if(!entry.Compress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error compressing the save.xml file!"));
			return false;
		}
		archive.AddFileEntry(entry);

		//Save external binary data to separate files
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			ZIPFileEntry entry;
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			entry.SetFileName(binaryFileName);
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry.Compress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error compressing the " + binaryFileName + L" file!"));
				return false;
			}
			archive.AddFileEntry(entry);
		}

		//Create the target file
		Stream::File target;
		if(!target.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the zip structure to the file!"));
			return false;
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		//Save XML tree to the target file
		Stream::File file(Stream::IStream::TEXTENCODING_UTF8);
		if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			return false;
		}
		file.InsertByteOrderMark();
		if(!tree.SaveTree(file))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Save external binary data to separate files
		std::wstring fileName = PathGetFileName(filePath);
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = fileName + L" - " + (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = PathCombinePaths(fileDir, binaryFileName);
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the binary data file " + binaryFileName + L" at the full path of " + binaryFilePath + L"!"));
				return false;
			}

			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			while(!binaryData.IsAtEnd())
			{
				unsigned char temp;
				if(!binaryData.ReadData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error reading the source data from memory to save to the binary data file " + binaryFileName + L"!"));
					return false;
				}
				if(!binaryFile.WriteData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error writing to the binary data file " + binaryFileName + L"!"));
					return false;
				}
			}
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Saved persistent state to file " + filePath));

	return true;
}

//----------------------------------------------------------------------------------------
System::StateInfo System::GetStateInfo(const std::wstring& filePath, FileType fileType) const
{
	StateInfo stateInfo;
	stateInfo.valid = false;

	//Open the target file
	FileStreamReference sourceStreamReference(*guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		return stateInfo;
	}
	Stream::IStream& source = *sourceStreamReference;

	HeirarchicalStorageTree tree;
	if(fileType == FILETYPE_ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			return stateInfo;
		}

		//Load XML tree from file
		ZIPFileEntry* entry = archive.GetFileEntry(L"save.xml");
		if(entry == 0)
		{
			return stateInfo;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			return stateInfo;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			return stateInfo;
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		source.ProcessByteOrderMark();
		if(!tree.LoadTree(source))
		{
			return stateInfo;
		}
	}

	//Load savestate info from XML data
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() == L"State")
	{
		stateInfo.valid = true;
		std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
		bool foundStateInfo = false;
		std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin();
		while(!foundStateInfo && (i != childList.end()))
		{
			if((*i)->GetName() == L"Info")
			{
				(*i)->ExtractAttribute(L"CreationDate", stateInfo.creationDate);
				(*i)->ExtractAttribute(L"CreationTime", stateInfo.creationTime);
				(*i)->ExtractAttribute(L"Comments", stateInfo.comments);
				(*i)->ExtractAttribute(L"DebuggerState", stateInfo.debuggerState);
				if((*i)->ExtractAttribute(L"Screenshot", stateInfo.screenshotFilename))
				{
					stateInfo.screenshotPresent = true;
				}
				foundStateInfo = true;
				continue;
			}
			++i;
		}
	}

	return stateInfo;
}

//----------------------------------------------------------------------------------------
bool System::LoadSavedRelationshipMap(IHeirarchicalStorageNode& node, SavedRelationshipMap& relationshipMap) const
{
	//Validate the name of the node
	if(node.GetName() != L"ModuleRelationships")
	{
		return false;
	}

	//Load the saved module relationship data
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Module")
		{
			//Extract the mandatory attributes
			IHeirarchicalStorageAttribute* moduleIDAttribute = (*i)->GetAttribute(L"ModuleID");
			IHeirarchicalStorageAttribute* systemClassNameAttribute = (*i)->GetAttribute(L"SystemClassName");
			IHeirarchicalStorageAttribute* moduleClassNameAttribute = (*i)->GetAttribute(L"ModuleClassName");
			IHeirarchicalStorageAttribute* moduleInstanceNameAttribute = (*i)->GetAttribute(L"ModuleInstanceName");
			if((moduleIDAttribute != 0) && (systemClassNameAttribute != 0) && (moduleClassNameAttribute != 0) && (moduleInstanceNameAttribute != 0))
			{
				//Extract info on this module
				SavedRelationshipModule savedData;
				savedData.moduleID = moduleIDAttribute->ExtractValue<unsigned int>();
				savedData.systemClassName = systemClassNameAttribute->GetValue();
				savedData.className = moduleClassNameAttribute->GetValue();
				savedData.instanceName = moduleInstanceNameAttribute->GetValue();
				IHeirarchicalStorageAttribute* filePathAttribute = (*i)->GetAttribute(L"FilePath");
				if(filePathAttribute != 0)
				{
					savedData.filePath = filePathAttribute->GetValue();
				}

				//Extract any connector info
				std::list<IHeirarchicalStorageNode*> moduleChildList = (*i)->GetChildList();
				for(std::list<IHeirarchicalStorageNode*>::const_iterator moduleChildIterator = moduleChildList.begin(); moduleChildIterator != moduleChildList.end(); ++moduleChildIterator)
				{
					if((*moduleChildIterator)->GetName() == L"ExportConnector")
					{
						//Extract the mandatory attributes
						IHeirarchicalStorageAttribute* connectorClassNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorClassName");
						IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceName");
						if((connectorClassNameAttribute != 0) && (connectorInstanceNameAttribute != 0))
						{
							SavedRelationshipExportConnector savedConnectorData;
							savedConnectorData.className = connectorClassNameAttribute->GetValue();
							savedConnectorData.instanceName = connectorInstanceNameAttribute->GetValue();
							savedData.exportedConnectors.push_back(savedConnectorData);
						}
					}
					else if((*moduleChildIterator)->GetName() == L"ImportConnector")
					{
						//Extract the mandatory attributes
						IHeirarchicalStorageAttribute* connectorModuleIDAttribute = (*moduleChildIterator)->GetAttribute(L"ExportingModuleID");
						IHeirarchicalStorageAttribute* connectorClassNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorClassName");
						IHeirarchicalStorageAttribute* connectorInstanceNameLocalAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceNameLocal");
						IHeirarchicalStorageAttribute* connectorInstanceNameRemoteAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceNameRemote");
						if((connectorModuleIDAttribute != 0) && (connectorClassNameAttribute != 0) && (connectorInstanceNameLocalAttribute != 0) && (connectorInstanceNameRemoteAttribute != 0))
						{
							SavedRelationshipImportConnector savedConnectorData;
							savedConnectorData.moduleID = connectorModuleIDAttribute->ExtractValue<unsigned int>();
							savedConnectorData.className = connectorClassNameAttribute->GetValue();
							savedConnectorData.instanceNameLocal = connectorInstanceNameLocalAttribute->GetValue();
							savedConnectorData.instanceNameRemote = connectorInstanceNameRemoteAttribute->GetValue();
							savedData.importedConnectors.push_back(savedConnectorData);
						}
					}
				}

				//Add info on this module to our list of modules
				relationshipMap.insert(SavedRelationshipMapEntry(savedData.moduleID, savedData));
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleRelationshipsNode(IHeirarchicalStorageNode& node, ModuleRelationshipMap& relationshipMap) const
{
	//Load the saved module relationship data
	SavedRelationshipMap savedRelationshipData;
	if(!LoadSavedRelationshipMap(node, savedRelationshipData))
	{
		return false;
	}

	//Build a copy of the connectorDetailsMap, keyed on the ID of the importing module.
	ConnectorInfoMapOnImportingModuleID connectorInfoOnImportingModuleID;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		const ConnectorInfoInternal& connectorDetails = i->second;
		if(connectorDetails.connectorUsed)
		{
			ConnectorInfo connectorInfo;
			connectorInfo.SetConnectorID(connectorDetails.connectorID);
			connectorInfo.SetExportingModuleID(connectorDetails.exportingModuleID);
			connectorInfo.SetConnectorClassName(connectorDetails.connectorClassName);
			connectorInfo.SetExportingModuleConnectorInstanceName(connectorDetails.exportingModuleConnectorInstanceName);
			connectorInfo.SetSystemClassName(connectorDetails.systemClassName);
			connectorInfo.SetIsConnectorUsed(connectorDetails.connectorUsed);
			connectorInfo.SetImportingModuleID(connectorDetails.importingModuleID);
			connectorInfo.SetImportingModuleConnectorInstanceName(connectorDetails.importingModuleConnectorInstanceName);
			connectorInfoOnImportingModuleID[i->second.importingModuleID].push_back(connectorInfo);
		}
	}

	//Examine each module referenced in the saved data, and try and find matching modules
	//in the loaded module list. Store information on module associations in the
	//relationshipMap.
	relationshipMap.clear();
	for(SavedRelationshipMap::const_iterator i = savedRelationshipData.begin(); i != savedRelationshipData.end(); ++i)
	{
		//Look for a loaded module which matches the saved module data
		bool foundMatchingModule = false;
		const SavedRelationshipModule& savedModuleInfo = i->second;
		LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
		while(!foundMatchingModule && (loadedModuleIterator != loadedModuleInfoList.end()))
		{
			//Check if these modules match
			const LoadedModuleInfoInternal& loadedModuleInfo = *loadedModuleIterator;
			if(DoesLoadedModuleMatchSavedModule(savedRelationshipData, savedModuleInfo, loadedModuleInfo, connectorInfoOnImportingModuleID))
			{
				//If these modules match, store the association between the modules, and
				//move on to the next module.
				ModuleRelationship moduleRelationship;
				moduleRelationship.foundMatch = true;
				moduleRelationship.savedModuleID = savedModuleInfo.moduleID;
				moduleRelationship.savedModuleClassName = savedModuleInfo.className;
				moduleRelationship.savedModuleInstanceName = savedModuleInfo.instanceName;
				moduleRelationship.loadedModuleID = loadedModuleInfo.moduleID;
				relationshipMap.insert(ModuleRelationshipEntry(moduleRelationship.savedModuleID, moduleRelationship));
				foundMatchingModule = true;
				continue;
			}
			++loadedModuleIterator;
		}

		//If no matching module was found, store information about the failed match,
		//record that not all modules were found, and move on to the next module.
		if(!foundMatchingModule)
		{
			ModuleRelationship moduleRelationship;
			moduleRelationship.foundMatch = false;
			moduleRelationship.savedModuleID = savedModuleInfo.moduleID;
			moduleRelationship.savedModuleClassName = savedModuleInfo.className;
			moduleRelationship.savedModuleInstanceName = savedModuleInfo.instanceName;
			relationshipMap.insert(ModuleRelationshipEntry(moduleRelationship.savedModuleID, moduleRelationship));
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsNode(IHeirarchicalStorageNode& relationshipsNode, bool saveFilePathInfo, const std::wstring& relativePathBase) const
{
	for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
	{
		IHeirarchicalStorageNode& moduleNode = relationshipsNode.CreateChild(L"Module");
		moduleNode.CreateAttribute(L"ModuleID").SetValue(i->moduleID);
		moduleNode.CreateAttribute(L"SystemClassName", i->systemClassName);
		moduleNode.CreateAttribute(L"ModuleClassName", i->className);
		moduleNode.CreateAttribute(L"ModuleInstanceName", i->instanceName);
		if(saveFilePathInfo)
		{
			//Convert the file directory to a relative path from the base directory, if
			//the target directory is the same or contained within the target path.
			std::wstring fileDir = PathGetDirectory(i->filePath);
			if(PathStartsWithBasePath(relativePathBase, fileDir))
			{
				std::wstring relativePath;
				if(PathBuildRelativePathToTarget(relativePathBase, fileDir, false, relativePath))
				{
					fileDir = relativePath;
				}
			}

			std::wstring filePath = PathCombinePaths(fileDir, PathGetFileName(i->filePath));
			moduleNode.CreateAttribute(L"FilePath", filePath);
		}
		SaveModuleRelationshipsExportConnectors(moduleNode, i->moduleID);
		SaveModuleRelationshipsImportConnectors(moduleNode, i->moduleID);
	}
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsExportConnectors(IHeirarchicalStorageNode& moduleNode, unsigned int moduleID) const
{
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		if(i->second.exportingModuleID == moduleID)
		{
			IHeirarchicalStorageNode& exportNode = moduleNode.CreateChild(L"ExportConnector");
			exportNode.CreateAttribute(L"ConnectorClassName", i->second.connectorClassName);
			exportNode.CreateAttribute(L"ConnectorInstanceName", i->second.exportingModuleConnectorInstanceName);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsImportConnectors(IHeirarchicalStorageNode& moduleNode, unsigned int moduleID) const
{
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		if(i->second.connectorUsed && (i->second.importingModuleID == moduleID))
		{
			IHeirarchicalStorageNode& exportNode = moduleNode.CreateChild(L"ImportConnector");
			exportNode.CreateAttribute(L"ExportingModuleID").SetValue(i->second.exportingModuleID);
			exportNode.CreateAttribute(L"ConnectorClassName", i->second.connectorClassName);
			exportNode.CreateAttribute(L"ConnectorInstanceNameLocal", i->second.importingModuleConnectorInstanceName);
			exportNode.CreateAttribute(L"ConnectorInstanceNameRemote", i->second.exportingModuleConnectorInstanceName);
		}
	}
}

//----------------------------------------------------------------------------------------
bool System::DoesLoadedModuleMatchSavedModule(const SavedRelationshipMap& savedRelationshipData, const SavedRelationshipModule& savedModuleInfo, const LoadedModuleInfoInternal& loadedModuleInfo, const ConnectorInfoMapOnImportingModuleID& connectorDetailsOnImportingModuleID) const
{
	//Validate that the class and instance names of these modules match
	if((loadedModuleInfo.systemClassName != savedModuleInfo.systemClassName) || (loadedModuleInfo.className != savedModuleInfo.className) || (loadedModuleInfo.instanceName != savedModuleInfo.instanceName))
	{
		return false;
	}

	//Validate that both modules import the same connectors from the same modules
	bool allConnectorsFound = true;
	ConnectorInfoMapOnImportingModuleID::const_iterator connectorDetailsIterator = connectorDetailsOnImportingModuleID.find(loadedModuleInfo.moduleID);
	if(connectorDetailsIterator != connectorDetailsOnImportingModuleID.end())
	{
		//Look for connectors which are being imported in the saved module data, and
		//correlate them with connectors which are being imported in the loaded module
		//data.
		const std::list<ConnectorInfo>& connectorList = connectorDetailsIterator->second;
		for(std::list<SavedRelationshipImportConnector>::const_iterator i = savedModuleInfo.importedConnectors.begin(); i != savedModuleInfo.importedConnectors.end(); ++i)
		{
			//For this saved imported connector, iterate through all the imported
			//connectors in the loaded module, looking for a match.
			bool foundMatchingConnector = false;
			std::list<ConnectorInfo>::const_iterator connectorListIterator = connectorList.begin();
			while(!foundMatchingConnector && (connectorListIterator != connectorList.end()))
			{
				//Check if the connector class and instance names match
				const ConnectorInfo& connectorInfo = *connectorListIterator;
				if((connectorInfo.GetConnectorClassName() == i->className) && (connectorInfo.GetExportingModuleConnectorInstanceName() == i->instanceNameRemote) && (connectorInfo.GetImportingModuleConnectorInstanceName() == i->instanceNameLocal))
				{
					//Find the saved info on the module which exports this connector
					SavedRelationshipMap::const_iterator savedRelationshipDataForExportingModuleIterator = savedRelationshipData.find(i->moduleID);
					if(savedRelationshipDataForExportingModuleIterator != savedRelationshipData.end())
					{
						const SavedRelationshipModule& savedModuleInfoForExportingModule = savedRelationshipDataForExportingModuleIterator->second;

						//Find the loaded info on the module which exports the matching
						//connector
						bool foundModuleEntry = false;
						LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
						while(!foundModuleEntry && (loadedModuleIterator != loadedModuleInfoList.end()))
						{
							if(loadedModuleIterator->moduleID == connectorInfo.GetExportingModuleID())
							{
								foundModuleEntry = true;
								continue;
							}
							++loadedModuleIterator;
						}

						//Recursively validate that the exporting module, and all modules
						//it is dependent on, match their corresponding saved module info.
						if(foundModuleEntry)
						{
							const LoadedModuleInfoInternal& loadedModuleInfoForExportingModule = *loadedModuleIterator;
							foundMatchingConnector = DoesLoadedModuleMatchSavedModule(savedRelationshipData, savedModuleInfoForExportingModule, loadedModuleInfoForExportingModule, connectorDetailsOnImportingModuleID);
						}
					}
				}
				++connectorListIterator;
			}
			allConnectorsFound &= foundMatchingConnector;
		}
	}

	//If all the imported connectors were found and matched to the loaded module info,
	//the modules match.
	return allConnectorsFound;
}

//----------------------------------------------------------------------------------------
//Logging functions
//----------------------------------------------------------------------------------------
void System::WriteLogEvent(const ILogEntry& entry) const
{
	boost::mutex::scoped_lock lock(eventLogMutex);
	LogEntryInternal logEntryInternal;
	logEntryInternal.text = entry.GetText();
	logEntryInternal.source = entry.GetSource();
	logEntryInternal.eventLevel = entry.GetEventLevel();
	logEntryInternal.eventLevelString = entry.GetEventLevelString();
	logEntryInternal.eventTimeString = entry.GetTimeString();
	log.push_front(logEntryInternal);
	if(log.size() > eventLogSize)
	{
		log.pop_back();
	}
}

//----------------------------------------------------------------------------------------
void System::ClearEventLog()
{
	boost::mutex::scoped_lock lock(eventLogMutex);
	log.clear();
}

//----------------------------------------------------------------------------------------
void System::SaveEventLog(const std::wstring& fileName)
{
	Stream::File target;
	if(target.Open(fileName, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		boost::mutex::scoped_lock lock(eventLogMutex);
		target.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
		target.InsertByteOrderMark();
		Stream::ViewText targetView(target);
		for(std::list<LogEntryInternal>::const_iterator i = log.begin(); i != log.end(); ++i)
		{
			targetView << i->eventTimeString << L'\t' << i->eventLevelString << L'\t' << i->source << L'\t' << i->text << L'\n';
		}
	}
}

//----------------------------------------------------------------------------------------
unsigned int System::GetEventLogSize() const
{
	boost::mutex::scoped_lock lock(eventLogMutex);
	return eventLogSize;
}

//----------------------------------------------------------------------------------------
void System::SetEventLogSize(unsigned int alogSize)
{
	boost::mutex::scoped_lock lock(eventLogMutex);
	eventLogSize = alogSize;
	while(log.size() > eventLogSize)
	{
		log.pop_back();
	}
}

//----------------------------------------------------------------------------------------
//Path functions
//----------------------------------------------------------------------------------------
std::wstring System::GetCapturePath() const
{
	return capturePath;
}

//----------------------------------------------------------------------------------------
void System::SetCapturePath(const std::wstring& apath)
{
	capturePath = apath;
}

//----------------------------------------------------------------------------------------
//Loaded entity functions
//----------------------------------------------------------------------------------------
IDevice* System::GetDevice(unsigned int moduleID, const std::wstring& deviceName) const
{
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if((i->moduleID == moduleID) && (i->name == deviceName))
		{
			return i->device;
		}
	}
	for(ImportedDeviceInfoList::const_iterator i = importedDeviceInfoList.begin(); i != importedDeviceInfoList.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleDeviceInstanceName == deviceName))
		{
			return i->device;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IExtension* System::GetExtension(unsigned int moduleID, const std::wstring& extensionName) const
{
	for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
	{
		if((i->moduleID == moduleID) && (i->name == extensionName))
		{
			return i->extension;
		}
	}
	for(ImportedExtensionInfoList::const_iterator i = importedExtensionInfoList.begin(); i != importedExtensionInfoList.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleExtensionInstanceName == extensionName))
		{
			return i->extension;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IExtension* System::GetGlobalExtension(const std::wstring& extensionName) const
{
	LoadedGlobalExtensionInfoList::const_iterator globalExtensionIterator = globalExtensionInfoList.find(extensionName);
	if(globalExtensionIterator != globalExtensionInfoList.end())
	{
		return globalExtensionIterator->second.extension;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
BusInterface* System::GetBusInterface(unsigned int moduleID, const std::wstring& busInterfaceName) const
{
	for(BusInterfaceList::const_iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		if((i->moduleID == moduleID) && (i->name == busInterfaceName))
		{
			return i->busInterface;
		}
	}
	for(ImportedBusInterfaceList::const_iterator i = importedBusInterfaces.begin(); i != importedBusInterfaces.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleBusInterfaceName == busInterfaceName))
		{
			return i->busInterface;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
ClockSource* System::GetClockSource(unsigned int moduleID, const std::wstring& clockSourceName) const
{
	for(ClockSourceList::const_iterator i = clockSources.begin(); i != clockSources.end(); ++i)
	{
		if((i->moduleID == moduleID) && (i->name == clockSourceName))
		{
			return i->clockSource;
		}
	}
	for(ImportedClockSourceList::const_iterator i = importedClockSources.begin(); i != importedClockSources.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleClockSourceName == clockSourceName))
		{
			return i->clockSource;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int System::GetSystemLineID(unsigned int moduleID, const std::wstring& systemLineName) const
{
	for(SystemLineMap::const_iterator i = systemLines.begin(); i != systemLines.end(); ++i)
	{
		if((i->second.moduleID == moduleID) && (i->second.lineName == systemLineName))
		{
			return i->second.lineID;
		}
	}
	for(ImportedSystemLineList::const_iterator i = importedSystemLines.begin(); i != importedSystemLines.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleSystemLineName == systemLineName))
		{
			return i->systemLineID;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
unsigned int System::GetSystemSettingID(unsigned int moduleID, const std::wstring& systemSettingName) const
{
	ModuleSystemSettingMap::const_iterator moduleSettingsIterator = moduleSettings.find(moduleID);
	if(moduleSettingsIterator != moduleSettings.end())
	{
		for(SystemSettingsIDList::const_iterator i = moduleSettingsIterator->second.begin(); i != moduleSettingsIterator->second.end(); ++i)
		{
			//Retrieve info for this system setting
			SystemSettingsMap::const_iterator systemSettingsIterator = systemSettings.find(*i);
			if(systemSettingsIterator == systemSettings.end())
			{
				continue;
			}
			const SystemSettingInfo& systemSettingInfo = systemSettingsIterator->second;

			//If this is the target system setting, return the setting ID number.
			if(systemSettingInfo.name == systemSettingName)
			{
				return systemSettingInfo.systemSettingID;
			}
		}
	}
	for(ImportedSystemSettingList::const_iterator i = importedSystemSettings.begin(); i != importedSystemSettings.end(); ++i)
	{
		if((i->importingModuleID == moduleID) && (i->importingModuleSystemSettingName == systemSettingName))
		{
			return i->systemSettingID;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//System interface functions
//----------------------------------------------------------------------------------------
bool System::ValidateSystem()
{
	bool result = true;
	for(LoadedDeviceInfoList::iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(!i->device->ValidateDevice())
		{
			std::wstring message = L"ValidateDevice failed for ";
			message += i->name;
			message += L"!";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", message));
			result = false;
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
void System::Initialize()
{
	initialize = true;
}

//----------------------------------------------------------------------------------------
void System::InitializeAllDevices()
{
	//Negate the current output line state for all devices we are about to initialize. We
	//need to do this, as the Initialize routine may alter the internal state of the
	//device in such a way that line state changes which have been conditionally asserted
	//for a future point in time should no longer be triggered. The Initialize routine is
	//not allowed to interact with external devices however, so it cannot revoke any
	//pending line state changes that have already been asserted. By explicitly negating
	//all output line state here, we ensure that there are no pending line state changes
	//asserted before we initialize the devices.
	executionManager.NegateCurrentOutputLineState();

	//Initialize the devices
	executionManager.Initialize();

	//Load the persistent state for all loaded modules
	if(enablePersistentState)
	{
		for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
		{
			const LoadedModuleInfoInternal& moduleInfo = *i;
			std::wstring persistentModuleFileName = moduleInfo.className + L".zip";
			std::wstring persistentModuleFilePath = PathCombinePaths(guiExtensionInterface->GetGlobalPreferencePathPersistentState(), persistentModuleFileName);
			if(!LoadPersistentStateForModule(persistentModuleFilePath, moduleInfo.moduleID, FILETYPE_ZIP, true))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Failed to load persistent state from file \"" + persistentModuleFilePath + L"\" when initializing module with name " + moduleInfo.displayName + L"."));
			}
		}
	}

	//Assert the specified unmapped line state for any unmapped lines in the system that
	//have an unmapped line state specified
	for(UnmappedLineStateList::const_iterator i = unmappedLineStateList.begin(); i != unmappedLineStateList.end(); ++i)
	{
		//Retrieve information on the module that contains the target device
		unsigned int deviceModuleID = i->targetDevice->GetDeviceModuleID();
		bool foundDeviceModuleEntry = false;
		LoadedModuleInfoList::const_iterator deviceLoadedModuleIterator = loadedModuleInfoList.begin();
		while(!foundDeviceModuleEntry && (deviceLoadedModuleIterator != loadedModuleInfoList.end()))
		{
			if(deviceLoadedModuleIterator->moduleID == deviceModuleID)
			{
				foundDeviceModuleEntry = true;
				continue;
			}
			++deviceLoadedModuleIterator;
		}
		if(!foundDeviceModuleEntry)
		{
			continue;
		}
		const LoadedModuleInfoInternal& deviceModuleInfo = *deviceLoadedModuleIterator;

		//If the module which contains this device has not been validated, skip any
		//further processing for this device.
		if(!deviceModuleInfo.moduleValidated)
		{
			continue;
		}

		//Check if at least one mapping exists to the target line on the device with the
		//unmapped line state setting
		bool foundMappingToLine = false;
		BusInterfaceList::const_iterator busInterfaceIterator = busInterfaces.begin();
		while(!foundMappingToLine && (busInterfaceIterator != busInterfaces.end()))
		{
			foundMappingToLine |= busInterfaceIterator->busInterface->IsDeviceLineMappedTo(i->targetDevice, i->lineNo);
			++busInterfaceIterator;
		}

		//If no mapping could be found to the target line, set the target line to the
		//specified unmapped line state.
		if(!foundMappingToLine)
		{
			i->targetDevice->TransparentSetLineState(i->lineNo, i->lineData);
		}
	}

	//Re-assert the current output line state for all devices we just initialized. This is
	//required, as the initialization routine for a device may change its internal state
	//in a way that would affect its asserted output line state, but the Initialize method
	//is not allowed to interact with external devices. A call to the
	//AssertCurrentOutputLineState method ensures that the correct external line state can
	//now be asserted for all initialized devices.
	executionManager.AssertCurrentOutputLineState();

	//Re-assert the current line state for all system lines. This is required, as devices
	//reset their input line state as a result of a call to the Initialize method.
	for(SystemLineMap::const_iterator i = systemLines.begin(); i != systemLines.end(); ++i)
	{
		SetSystemLineState(i->first, Data(i->second.lineWidth, i->second.currentValue));
	}
}

//----------------------------------------------------------------------------------------
void System::RunSystem()
{
	if(!SystemRunning())
	{
		//Log the event
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"System entered the running state"));

		stopSystem = false;
		systemStopped = false;
		boost::thread workerThread(boost::bind(boost::mem_fn(&System::ExecuteThread), this));
	}
}

//----------------------------------------------------------------------------------------
void System::ExecuteThread()
{
	//Boost the system thread priority. This thread is always on the critical path. We
	//don't want secondary threads from various devices in the system preventing this
	//thread running.
	SetCurrentThreadPriority(THREADPRIORITY_HIGH);

	//Start active device threads
	executionManager.BeginExecution();

	//Commit the current state of each device. We perform this task here to ensure that
	//manual changes made through the debug interface while the system was idle are not
	//lost in the event of a rollback.
	executionManager.Commit();

	//Main system loop
	double accumulatedExecutionTime = 0;
	PerformanceTimer timer;
	while(!stopSystem)
	{
		//Initialize all devices if it has been requested
		if(initialize)
		{
			//Stop active device threads
			executionManager.SuspendExecution();

			//Initialize the devices
			InitializeAllDevices();

			//Start active device threads
			executionManager.BeginExecution();

			//Commit changes from initialization
			executionManager.Commit();

			//Clear the initialize flag
			initialize = false;
		}

		//##TODO## Make it possible to configure the maximum timeslice size
		//##TODO## Build in heuristics to calculate the optimal maximum timeslice size
		double systemStepTime = ExecuteSystemStep(20000000.0);
		accumulatedExecutionTime += systemStepTime;

		//##DEBUG##
		//Note that this kills performance
//		std::wcout << std::setprecision(16) << "System Step: " << systemStepTime << '\t' << accumulatedExecutionTime << '\n';

		//If we're running too fast (*chuckle*), delay execution until we get back in
		//sync.
		if(accumulatedExecutionTime >= 20000000.0)
//		if(accumulatedExecutionTime >= 1000000000.0)
		{
			timer.Sync(accumulatedExecutionTime, enableThrottling);
			accumulatedExecutionTime = 0;
		}
	}

	//Stop active device threads
	executionManager.SuspendExecution();

	SignalSystemStopped();
}

//----------------------------------------------------------------------------------------
void System::ExecuteSystemStepManual(double targetTime)
{
	//Start active device threads
	executionManager.BeginExecution();

	//Commit the current state of each device. We perform this task here to ensure that
	//manual changes made through the debug interface while the system was idle are not
	//lost in the event of a rollback.
	executionManager.Commit();

	//Advance the system until we reach the target time
	double totalSystemExecutionTime = 0;
	while(totalSystemExecutionTime < targetTime)
	{
		double timeRemainingToTarget = targetTime - totalSystemExecutionTime;
		totalSystemExecutionTime += ExecuteSystemStep(timeRemainingToTarget);
	}

	//Stop active device threads
	executionManager.SuspendExecution();
}

//----------------------------------------------------------------------------------------
double System::ExecuteSystemStep(double maximumTimeslice)
{
	//Determine the maximum length of time all devices can run unsynchronized before the
	//next timing point
	DeviceContext* nextDeviceStep = 0;
	unsigned int nextDeviceStepContext = 0;
	double timeslice = executionManager.GetNextTimingPoint(maximumTimeslice, nextDeviceStep, nextDeviceStepContext);

	bool callbackStep = false;
	void (*callbackFunction)(void*) = 0;
	void* callbackParams = 0;
	do
	{
		rollback = false;

		//Notify upcoming timeslice
		executionManager.NotifyUpcomingTimeslice(timeslice);

		//Send any buffered input events
		SendStoredInputEvents();

		//##DEBUG##
//		std::wcout << "Timeslice\t" << timeslice << '\n';

		//Notify before execute called
		executionManager.NotifyBeforeExecuteCalled();

		//Execute next timeslice
		executionManager.ExecuteTimeslice(timeslice);

		//Notify after execute called
		executionManager.NotifyAfterExecuteCalled();

		//##TODO## Introduce the ability to "suspend" execution of a worker thread, until
		//all other non-suspended worker threads have completed execution. At this point,
		//any worker threads which are still suspended are notified all other devices have
		//finished, and they can perform whatever tasks they need to in order to finalize
		//the current timeslice.
		//-In order to implement this, I recommend we introduce a new member function into
		//the IDevice interface, called NotifyAllDevicesFinishedDuringSuspend() or
		//something, which the device needs to implement if it uses suspend states.
		//-I recommend another function in IDevice like UsesExecuteSuspend() or the like,
		//which can be used to determine if a given device may issue a suspend operation
		//at any time. This could be useful for optimizing the implementation.
		//-I recommend we introduce a new class, called an ExecutionManager or the like,
		//which has its own thread. The purpose of this class is to act as an intermediary
		//between the system and the DeviceContext instances themselves, forwarding new
		//timeslices on to the devices, and in this case, managing the issue of
		//determining when all devices are finished executing in particular. In order to
		//support suspension, we need the system to simply be able to wait for "all
		//devices to finish", while the actual execution manager will keep track of how
		//many devices are executing, how many are finished, and how many are suspended.
		//When no devices are left executing, any suspended devices will be resumed, then
		//when all devices are finished, the execution will be flagged as complete.

		//Roll back or commit changes
		if(rollback)
		{
			//##DEBUG##
			std::wcout << "Rollback\t" << std::setprecision(16) << rollbackTimeslice << '\n';
			executionManager.Rollback();

			//##DEBUG##
			if(rollbackTimeslice < 0)
			{
				std::wcout << "Device returned invalid rollback timeslice:\t" << rollbackTimeslice << '\n';
				system("pause");
			}

			timeslice = rollbackTimeslice;
			nextDeviceStep = (DeviceContext*)rollbackDevice;
			nextDeviceStepContext = rollbackContext;
			callbackStep = useRollbackFunction;
			if(callbackStep)
			{
				callbackFunction = rollbackFunction;
				callbackParams = rollbackParams;
			}
			continue;
		}
	}
	while(rollback && (rollbackTimeslice > 0));

	//If we are currently sitting on a timing point for a device, step through it.
	if(nextDeviceStep != 0)
	{
		//Notify upcoming timeslice
		executionManager.NotifyUpcomingTimeslice(0.0);

		//Notify before execute called
		executionManager.NotifyBeforeExecuteCalled();

		if(!callbackStep)
		{
			nextDeviceStep->ExecuteStep(nextDeviceStepContext);
		}
		else
		{
			callbackFunction(callbackParams);
		}

		//Notify after execute called
		executionManager.NotifyAfterExecuteCalled();
	}

	//Commit all changes
	executionManager.Commit();

	//Clear all input events which have been successfully processed
	ClearSentStoredInputEvents();

	return timeslice;
}

//----------------------------------------------------------------------------------------
void System::ExecuteDeviceStep(DeviceContext* device)
{
	//Initialize all devices if it has been requested
	if(initialize)
	{
		//Initialize the devices
		InitializeAllDevices();

		//Clear the initialize flag
		initialize = false;
	}

	//Start active device threads
	executionManager.BeginExecution();

	//Commit the current state of each device. We perform this task here to ensure that
	//manual changes made through the debug interface while the system was idle, and the
	//initialize step above, are not lost in the event of a rollback.
	executionManager.Commit();

	//Notify upcoming timeslice
	executionManager.NotifyUpcomingTimeslice(0.0);

	//Notify before execute called
	executionManager.NotifyBeforeExecuteCalled();

	//##FIX## This execution model is not only unusual, it now also causes a deadlock, due
	//to the missed NotifyAfterExecuteStepFinishedTimeslice() event. It could also cause
	//logic problems for timeslice execution devices, which receive a notification of an
	//upcoming timeslice, but never receive the timeslice itself. I think perhaps it's
	//time to unify device stepping with our normal execution model, and just make a
	//device step execute a system step for a time of 0.0, then step through the device
	//as a timing point. Actually, looking at our system implementation, it has the exact
	//same problems and deadlock case. We need to review our execution model here.
	//Step through the target device
	rollback = false;
	double timeslice = device->ExecuteStep();

	//Notify after execute called
	executionManager.NotifyAfterExecuteCalled();

	if(rollback)
	{
		//If the device we're trying to step through is sitting on a timing point, commit
		//now, since we've just advanced it through its timing point in lock step. We need
		//to perform this operation here, as if we try and step through a device sitting
		//on a timing point using the loop method below, only the device will advance, and
		//ExecuteSystemStep will correctly return 0. This will leave us in an infinite
		//loop.
		executionManager.Commit();
	}
	else
	{
		//If the device we're trying to step through is not sitting on a timing point,
		//roll back the execution, and pass the execution time to ExecuteSystemStep to
		//advance the entire system by the target amount.
		executionManager.Rollback();

		double totalTimeExecuted = 0;
		while(totalTimeExecuted < timeslice)
		{
			totalTimeExecuted += ExecuteSystemStep(timeslice - totalTimeExecuted);
		}
	}

	//Stop active device threads
	executionManager.SuspendExecution();
}

//----------------------------------------------------------------------------------------
void System::StopSystem()
{
	boost::mutex::scoped_lock lock(controlMutex);
	if(!systemStopped)
	{
		stopSystem = true;
		notifySystemStopped.wait(lock);
		stopSystem = false;

		//Log the event
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"System halted"));
	}
}

//----------------------------------------------------------------------------------------
void System::FlagStopSystem()
{
	stopSystem = true;
}

//----------------------------------------------------------------------------------------
bool System::GetThrottlingState() const
{
	return enableThrottling;
}

//----------------------------------------------------------------------------------------
void System::SetThrottlingState(bool state)
{
	enableThrottling = state;
}

//----------------------------------------------------------------------------------------
bool System::GetRunWhenProgramModuleLoadedState() const
{
	return runWhenProgramModuleLoaded;
}

//----------------------------------------------------------------------------------------
void System::SetRunWhenProgramModuleLoadedState(bool state)
{
	runWhenProgramModuleLoaded = state;
}

//----------------------------------------------------------------------------------------
bool System::GetEnablePersistentState() const
{
	return enablePersistentState;
}

//----------------------------------------------------------------------------------------
void System::SetEnablePersistentState(bool state)
{
	enablePersistentState = state;
}

//----------------------------------------------------------------------------------------
bool System::SystemRunning() const
{
	return !systemStopped;
}

//----------------------------------------------------------------------------------------
void System::SignalSystemStopped()
{
	boost::mutex::scoped_lock lock(controlMutex);
	systemStopped = true;
	notifySystemStopped.notify_all();
}

//----------------------------------------------------------------------------------------
bool System::IsSystemRollbackFlagged() const
{
	return rollback;
}

//----------------------------------------------------------------------------------------
double System::SystemRollbackTime() const
{
	return rollbackTimeslice;
}

//----------------------------------------------------------------------------------------
void System::SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*), void* callbackParams)
{
	//##DEBUG##
	std::wstringstream message;
	message << L"Rollback flagged:\t" << atriggerDevice->GetTargetDevice()->GetDeviceInstanceName() << '\t';
	if(arollbackDevice != 0)
	{
		message << arollbackDevice->GetTargetDevice()->GetDeviceInstanceName() << '\t';
	}
	message << std::setprecision(16) << timeslice << '\n';
	std::wcout << message.str();

	boost::mutex::scoped_lock lock(accessMutex);
	if(!rollback || (timeslice < rollbackTimeslice))
	{
		rollback = true;
		rollbackContext = accessContext;
		rollbackDevice = arollbackDevice;

		//If the device which triggered the rollback uses the step execution method, we
		//trigger the rollback using the reported current timeslice progress of the device
		//rather than the actual time at which the access occurred which triggered the
		//rollback. We need to do this, otherwise devices which have multiple external
		//access events within a single indivisible operation can trigger a rollback on
		//a later access, which may occur with an access time past the start of the actual
		//operation itself, which will then create a rollback target that we can't
		//actually advance up to without triggering the rollback again. See EX-62 for more
		//info.
		if((arollbackDevice != 0) && (arollbackDevice->GetTargetDevice()->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP))
		{
			rollbackTimeslice = arollbackDevice->GetCurrentTimesliceProgress();
		}
		else
		{
			rollbackTimeslice = timeslice;
		}

		useRollbackFunction = false;
		if(callbackFunction != 0)
		{
			useRollbackFunction = true;
			rollbackFunction = *callbackFunction;
			rollbackParams = callbackParams;
		}
	}
}

//----------------------------------------------------------------------------------------
//Device registration
//----------------------------------------------------------------------------------------
bool System::RegisterDevice(const DeviceInfo& entry, IDevice::AssemblyHandle assemblyHandle)
{
	//Make sure a valid device class name has been supplied
	if(entry.GetDeviceClassName().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device. No device class name was supplied."));
		return false;
	}

	//Make sure a valid device implementation name has been supplied
	if(entry.GetDeviceImplementationName().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device. No device implementation name was supplied."));
		return false;
	}

	//Make sure an allocator function has been supplied
	if(entry.GetAllocator() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device " + entry.GetDeviceImplementationName() + L". No allocator function was supplied."));
		return false;
	}

	//Make sure a destructor function has been supplied
	if(entry.GetDestructor() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device " + entry.GetDeviceImplementationName() + L". No destructor function was supplied."));
		return false;
	}

	//Check if we already have a device with the same name registered
	DeviceLibraryList::iterator existingEntry = deviceLibrary.find(entry.GetDeviceImplementationName());
	if(existingEntry != deviceLibrary.end())
	{
		if(existingEntry->second.versionNo >= entry.GetDeviceVersionNo())
		{
			//If we already have a newer version of this device registered, log the event, and
			//return true.
			std::wstringstream message;
			message << L"Ignored device " << entry.GetDeviceImplementationName() << L" with version number " << entry.GetDeviceVersionNo() << L" because another version of this device has already been registered with a version number of " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));
			return true;
		}
		else
		{
			//Log the fact we just overrode an existing device registration
			std::wstringstream message;
			message << L"Device " << entry.GetDeviceImplementationName() << L" with version number " << entry.GetDeviceVersionNo() << L" overrode the existing registration for this device with version number " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));

			//If the existing device registration is for an older version, remove the old
			//device registration. This new registration with a higher version number
			//supersedes the old one.
			UnregisterDevice(entry.GetDeviceImplementationName());
		}
	}

	//Register the device
	DeviceLibraryEntry listEntry;
	listEntry.Allocator = entry.GetAllocator();
	listEntry.Destructor = entry.GetDestructor();
	listEntry.className = entry.GetDeviceClassName();
	listEntry.implementationName = entry.GetDeviceImplementationName();
	listEntry.versionNo = entry.GetDeviceVersionNo();
	listEntry.copyright = entry.GetDeviceCopyright();
	listEntry.comments = entry.GetDeviceComments();
	listEntry.assemblyHandle = assemblyHandle;
	deviceLibrary.insert(DeviceLibraryListEntry(entry.GetDeviceImplementationName(), listEntry));

	//Log the device registration
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Successfully registered device " + entry.GetDeviceImplementationName() + L"."));

	return true;
}

//----------------------------------------------------------------------------------------
void System::UnregisterDevice(const std::wstring deviceName)
{
	DeviceLibraryList::iterator i = deviceLibrary.find(deviceName);
	if(i != deviceLibrary.end())
	{
		deviceLibrary.erase(i);
	}
}

//----------------------------------------------------------------------------------------
//Extension registration
//----------------------------------------------------------------------------------------
bool System::RegisterExtension(const ExtensionInfo& entry, IExtension::AssemblyHandle assemblyHandle)
{
	//Make sure a valid extension class name has been supplied
	if(entry.GetExtensionClassName().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering extension. No extension class name was supplied."));
		return false;
	}

	//Make sure a valid extension implementation name has been supplied
	if(entry.GetExtensionImplementationName().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering extension. No extension implementation name was supplied."));
		return false;
	}

	//Make sure an allocator function has been supplied
	if(entry.GetAllocator() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering extension " + entry.GetExtensionImplementationName() + L". No allocator function was supplied."));
		return false;
	}

	//Make sure a destructor function has been supplied
	if(entry.GetDestructor() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering extension " + entry.GetExtensionImplementationName() + L". No destructor function was supplied."));
		return false;
	}

	//Check if we already have an extension with the same name registered
	ExtensionLibraryList::iterator existingEntry = extensionLibrary.find(entry.GetExtensionImplementationName());
	if(existingEntry != extensionLibrary.end())
	{
		if(existingEntry->second.versionNo >= entry.GetExtensionVersionNo())
		{
			//If we already have a newer version of this extension registered, log the
			//event, and return true.
			std::wstringstream message;
			message << L"Ignored extension " << entry.GetExtensionImplementationName() << L" with version number " << entry.GetExtensionVersionNo() << L" because another version of this extension has already been registered with a version number of " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));
			return true;
		}
		else
		{
			//Log the fact we just overrode an existing extension registration
			std::wstringstream message;
			message << L"Extension " << entry.GetExtensionImplementationName() << L" with version number " << entry.GetExtensionVersionNo() << L" overrode the existing registration for this extension with version number " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));

			//If the existing extension registration is for an older version, remove the
			//old extension registration. This new registration with a higher version
			//number supersedes the old one.
			UnregisterExtension(entry.GetExtensionImplementationName());
		}
	}

	//Register the extension
	ExtensionLibraryEntry listEntry;
	listEntry.Allocator = entry.GetAllocator();
	listEntry.Destructor = entry.GetDestructor();
	listEntry.className = entry.GetExtensionClassName();
	listEntry.implementationName = entry.GetExtensionImplementationName();
	listEntry.versionNo = entry.GetExtensionVersionNo();
	listEntry.copyright = entry.GetExtensionCopyright();
	listEntry.comments = entry.GetExtensionComments();
	listEntry.assemblyHandle = assemblyHandle;
	extensionLibrary.insert(ExtensionLibraryListEntry(entry.GetExtensionImplementationName(), listEntry));

	//Log the extension registration
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Successfully registered extension " + entry.GetExtensionImplementationName() + L"."));

	return true;
}

//----------------------------------------------------------------------------------------
void System::UnregisterExtension(const std::wstring extensionName)
{
	ExtensionLibraryList::iterator i = extensionLibrary.find(extensionName);
	if(i != extensionLibrary.end())
	{
		extensionLibrary.erase(i);
	}
}

//----------------------------------------------------------------------------------------
//Device creation and deletion
//----------------------------------------------------------------------------------------
bool System::AddDevice(unsigned int moduleID, IDevice* device, DeviceContext* deviceContext)
{
	LoadedDeviceInfo deviceInfo;
	deviceInfo.moduleID = moduleID;
	deviceInfo.device = device;
	deviceInfo.deviceContext = deviceContext;
	deviceInfo.name = device->GetDeviceInstanceName();
	loadedDeviceInfoList.push_back(deviceInfo);

	devices.push_back(deviceContext);
	executionManager.AddDevice(deviceContext);

	return true;
}

//----------------------------------------------------------------------------------------
IDevice* System::CreateDevice(const std::wstring& deviceName, const std::wstring& instanceName, unsigned int moduleID) const
{
	IDevice* device = 0;
	DeviceLibraryList::const_iterator i = deviceLibrary.find(deviceName);
	if(i != deviceLibrary.end())
	{
		device = i->second.Allocator(deviceName.c_str(), instanceName.c_str(), moduleID);
		device->SetAssemblyHandle(i->second.assemblyHandle);
	}
	return device;
}

//----------------------------------------------------------------------------------------
void System::DestroyDevice(const std::wstring& deviceName, IDevice* device) const
{
	//##TODO## Log an error if a device cannot be located in the device library
	DeviceLibraryList::const_iterator i = deviceLibrary.find(deviceName);
	if(i != deviceLibrary.end())
	{
		i->second.Destructor(device);
	}
}

//----------------------------------------------------------------------------------------
void System::UnloadDevice(IDevice* adevice)
{
	//Remove all references to the device from our input mappings
	UnmapAllKeyCodeMappingsForDevice(adevice);

	//Remove all registered input targets for this device from our input registration list
	InputRegistrationList::iterator inputRegistrationListIterator = inputRegistrationList.begin();
	while(inputRegistrationListIterator != inputRegistrationList.end())
	{
		InputRegistrationList::iterator currentElement = inputRegistrationListIterator;
		++inputRegistrationListIterator;
		if(currentElement->targetDevice == adevice)
		{
			inputRegistrationList.erase(currentElement);
		}
	}

	//Remove all bus references to the device
	for(BusInterfaceList::iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		i->busInterface->RemoveAllReferencesToDevice(adevice);
	}

	//Remove any unmapped line state entries which reference the device
	UnmappedLineStateList::iterator unmappedLineStateEntry = unmappedLineStateList.begin();
	while(unmappedLineStateEntry != unmappedLineStateList.end())
	{
		UnmappedLineStateList::iterator currentElement = unmappedLineStateEntry;
		++unmappedLineStateEntry;
		if(currentElement->targetDevice == adevice)
		{
			//Delete this unmapped line state entry
			unmappedLineStateList.erase(currentElement);
		}
	}

	//Remove the device itself from the system
	executionManager.RemoveDevice((DeviceContext*)adevice->GetDeviceContext());
	RemoveDeviceFromDeviceList(devices, adevice);

	//Destroy the device
	DestroyDevice(adevice->GetDeviceImplementationName(), adevice);
}

//----------------------------------------------------------------------------------------
void System::RemoveDeviceFromDeviceList(DeviceArray& deviceList, IDevice* adevice) const
{
	IDeviceContext* deviceContext = adevice->GetDeviceContext();
	bool done = false;
	DeviceArray::iterator i = deviceList.begin();
	while(!done && (i != deviceList.end()))
	{
		if(*i == deviceContext)
		{
			deviceList.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}
}

//----------------------------------------------------------------------------------------
//Extension creation and deletion
//----------------------------------------------------------------------------------------
IExtension* System::CreateGlobalExtension(const std::wstring& extensionName) const
{
	return CreateExtension(extensionName, extensionName, 0);
}

//----------------------------------------------------------------------------------------
IExtension* System::CreateExtension(const std::wstring& extensionName, const std::wstring& instanceName, unsigned int moduleID) const
{
	IExtension* extension = 0;
	ExtensionLibraryList::const_iterator i = extensionLibrary.find(extensionName);
	if(i != extensionLibrary.end())
	{
		extension = i->second.Allocator(extensionName.c_str(), instanceName.c_str(), moduleID);
		extension->SetAssemblyHandle(i->second.assemblyHandle);
	}
	return extension;
}

//----------------------------------------------------------------------------------------
bool System::AddGlobalExtension(unsigned int moduleID, IExtension* extension)
{
	//If this global extension has already been referenced by another module, add the
	//additional module reference to the extension.
	LoadedGlobalExtensionInfoList::iterator loadedGlobalExtensionInfoIterator = globalExtensionInfoList.find(extension->GetExtensionClassName());
	if(loadedGlobalExtensionInfoIterator != globalExtensionInfoList.end())
	{
		loadedGlobalExtensionInfoIterator->second.moduleIDs.insert(moduleID);
		return true;
	}

	//If this global extension has not been previously referenced by any module, record
	//information on the extension, and add the specified module as a referencing module.
	LoadedGlobalExtensionInfo extensionInfo;
	extensionInfo.extension = extension;
	extensionInfo.name = extension->GetExtensionClassName();
	extensionInfo.moduleIDs.insert(moduleID);
	globalExtensionInfoList.insert(LoadedGlobalExtensionInfoListEntry(extensionInfo.name, extensionInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::AddExtension(unsigned int moduleID, IExtension* extension)
{
	LoadedExtensionInfo extensionInfo;
	extensionInfo.moduleID = moduleID;
	extensionInfo.extension = extension;
	extensionInfo.name = extension->GetExtensionInstanceName();
	loadedExtensionInfoList.push_back(extensionInfo);

	return true;
}

//----------------------------------------------------------------------------------------
void System::DestroyExtension(const std::wstring& extensionName, IExtension* extension) const
{
	ExtensionLibraryList::const_iterator i = extensionLibrary.find(extensionName);
	if(i != extensionLibrary.end())
	{
		i->second.Destructor(extension);
	}
}

//----------------------------------------------------------------------------------------
void System::UnloadExtension(IExtension* aextension)
{
	//Destroy the extension
	DestroyExtension(aextension->GetExtensionClassName(), aextension);
}

//----------------------------------------------------------------------------------------
//Module loading and unloading
//----------------------------------------------------------------------------------------
bool System::GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const
{
	bool foundModule = false;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	while(!foundModule && (loadedModuleIterator != loadedModuleInfoList.end()))
	{
		if(loadedModuleIterator->moduleID == moduleID)
		{
			foundModule = true;
			moduleDisplayName = loadedModuleIterator->displayName;
		}
		++loadedModuleIterator;
	}
	return foundModule;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleInstanceName(unsigned int moduleID, std::wstring& moduleInstanceName) const
{
	bool foundModule = false;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	while(!foundModule && (loadedModuleIterator != loadedModuleInfoList.end()))
	{
		if(loadedModuleIterator->moduleID == moduleID)
		{
			foundModule = true;
			moduleInstanceName = loadedModuleIterator->instanceName;
		}
		++loadedModuleIterator;
	}
	return foundModule;
}

//----------------------------------------------------------------------------------------
void System::LoadModuleSynchronous(const std::wstring& filePath, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher)
{
	loadSystemComplete = false;
	loadSystemProgress = 0;
	loadSystemAbort = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&System::LoadModule), this, filePath, boost::ref(connectorMappings), boost::ref(aviewModelLauncher)));
}

//----------------------------------------------------------------------------------------
void System::LoadModuleSynchronousAbort()
{
	loadSystemAbort = true;
}

//----------------------------------------------------------------------------------------
float System::LoadModuleSynchronousProgress() const
{
	return loadSystemProgress;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleSynchronousComplete() const
{
	return loadSystemComplete;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleSynchronousResult() const
{
	return loadSystemResult;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleSynchronousAborted() const
{
	return loadSystemAbort;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule(const std::wstring& filePath, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher)
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Initialize the system load status
	loadSystemAbort = false;
	loadSystemResult = true;

	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Load the module file
	LoadedModuleInfoList addedModules;
	std::list<ViewModelOpenRequest> viewModelOpenRequests;
	std::list<InputRegistration> inputRegistrationRequests;
	std::list<SystemStateChange> systemSettingsChangeRequests;
	if(!LoadModuleInternal(filePath, connectorMappings, viewModelOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModules))
	{
		//If there's an error loading the module, log the failure, and return false.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + filePath + L"!"));
		for(LoadedModuleInfoList::const_iterator addedModuleIterator = addedModules.begin(); addedModuleIterator != addedModules.end(); ++addedModuleIterator)
		{
			UnloadModuleInternal(addedModuleIterator->moduleID);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
	}

	//Build a set of all loaded module ID's, and detect if we just loaded a program
	//module.
	std::set<unsigned int> addedModuleIDs;
	bool programModuleLoaded = false;
	for(LoadedModuleInfoList::const_iterator i = addedModules.begin(); i != addedModules.end(); ++i)
	{
		programModuleLoaded |= i->programModule;
		addedModuleIDs.insert(i->moduleID);
	}

	//Bind all CE line mappings specified in all BusInterface objects to their referenced
	//devices.
	for(BusInterfaceList::iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		if(!i->busInterface->BindCELineMappings())
		{
			//If there's an error binding the ce line mappings, log the failure, and
			//return false.
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindCELineMappings failed for BusInterface " + i->name + L" when loading module from file " + filePath + L"!"));
			for(std::set<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
			{
				UnloadModuleInternal(*addedModuleIDsIterator);
			}
			if(!ValidateSystem())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
			}
			loadSystemComplete = true;
			loadSystemResult = false;
			return false;
		}
	}

	//Explicitly initialize all devices we just loaded
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(addedModuleIDs.find(i->moduleID) != addedModuleIDs.end())
		{
			i->deviceContext->Initialize();
		}
	}

	//Ensure the initial clock states are set for all devices
	for(ClockSourceList::const_iterator i = clockSources.begin(); i != clockSources.end(); ++i)
	{
		i->clockSource->PublishEffectiveClockFrequency();
	}

	//Perform any additional construction tasks the devices within the system require
	if(!ValidateSystem())
	{
		//If there's an error building the system, log the failure, and return false.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"System validation failed after loading module from file " + filePath + L"!"));
		for(std::set<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
		{
			UnloadModuleInternal(*addedModuleIDsIterator);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
	}

	//Flag that all loaded modules have passed validation. After this point, we want to
	//call functions on a device which affect the state of other devices when unloading
	//them from the system.
	for(LoadedModuleInfoList::iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
	{
		std::set<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.find(i->moduleID);
		if(addedModuleIDsIterator != addedModuleIDs.end())
		{
			i->moduleValidated = true;
		}
	}

	//Synchronize the asserted line state for all devices in the system, now that this
	//module has been loaded and all added devices have been fully initialized and
	//validated.
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		i->device->AssertCurrentOutputLineState();
	}

	//Assert the specified unmapped line state for any unmapped lines in the system that
	//have an unmapped line state specified
	for(UnmappedLineStateList::const_iterator i = unmappedLineStateList.begin(); i != unmappedLineStateList.end(); ++i)
	{
		//Check if at least one mapping exists to the target line on the device with the
		//unmapped line state setting
		bool foundMappingToLine = false;
		BusInterfaceList::const_iterator busInterfaceIterator = busInterfaces.begin();
		while(!foundMappingToLine && (busInterfaceIterator != busInterfaces.end()))
		{
			foundMappingToLine |= busInterfaceIterator->busInterface->IsDeviceLineMappedTo(i->targetDevice, i->lineNo);
			++busInterfaceIterator;
		}

		//If no mapping could be found to the target line, set the target line to the
		//specified unmapped line state.
		if(!foundMappingToLine)
		{
			i->targetDevice->TransparentSetLineState(i->lineNo, i->lineData);
		}
	}

	//Re-assert the current line state for all system lines. We need to do this here so
	//that any newly loaded devices which map to system lines receive the current output
	//line state for those lines.
	for(SystemLineMap::const_iterator i = systemLines.begin(); i != systemLines.end(); ++i)
	{
		SetSystemLineState(i->first, Data(i->second.lineWidth, i->second.currentValue));
	}

	//Bind new system options to the system option menu, and apply the default settings
	//for any system settings in the set of loaded modules.
	for(std::set<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
	{
		ModuleSystemSettingMap::const_iterator moduleSettingsIterator = moduleSettings.find(*addedModuleIDsIterator);
		if(moduleSettingsIterator != moduleSettings.end())
		{
			for(SystemSettingsIDList::const_iterator settingsIDIterator = moduleSettingsIterator->second.begin(); settingsIDIterator != moduleSettingsIterator->second.end(); ++settingsIDIterator)
			{
				//Retrieve info for this system setting
				SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(*settingsIDIterator);
				if(systemSettingsIterator == systemSettings.end())
				{
					continue;
				}
				SystemSettingInfo& systemSettingInfo = systemSettingsIterator->second;

				//Add this setting to the menu handler
				if(systemSettingInfo.toggleSetting)
				{
					//If this is a toggle menu item, add this setting to the menu handler
					unsigned int newMenuItemID = systemOptionMenuHandler->AddSystemSettingMenuItem(systemSettingInfo.systemSettingID, 0);
					systemSettingInfo.menuItemID = newMenuItemID;
				}
				else
				{
					//If this isn't a toggle menu item, add each option under this setting
					//to the menu handler.
					for(unsigned int i = 0; i < (unsigned int)systemSettingInfo.options.size(); ++i)
					{
						unsigned int newMenuItemID = systemOptionMenuHandler->AddSystemSettingMenuItem(systemSettingInfo.systemSettingID, i);
						systemSettingInfo.options[i].menuItemID = newMenuItemID;
					}
				}

				//Apply the default option for this setting
				if(systemSettingInfo.defaultOption < (unsigned int)systemSettingInfo.options.size())
				{
					//Replace the current option selection
					if(systemSettingInfo.toggleSetting)
					{
						systemSettingInfo.selectedOption = systemSettingInfo.defaultOption;
						if(systemSettingInfo.menuItemEntry != 0)
						{
							systemSettingInfo.menuItemEntry->SetCheckedState((systemSettingInfo.selectedOption == systemSettingInfo.onOption));
						}
					}
					else
					{
						if(systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry != 0)
						{
							systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry->SetCheckedState(false);
						}
						systemSettingInfo.selectedOption = systemSettingInfo.defaultOption;
						if(systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry != 0)
						{
							systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry->SetCheckedState(true);
						}
					}

					//Apply the new option selection
					SystemSettingOption& settingOption = systemSettingInfo.options[systemSettingInfo.selectedOption];
					for(std::list<SystemStateChange>::const_iterator i = settingOption.stateChanges.begin(); i != settingOption.stateChanges.end(); ++i)
					{
						//Apply this system state change
						if(!ApplySystemStateChange(*i))
						{
							WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Failed to apply system setting change for element with name \"" + i->targetElementName + L"\" in system option \"" + settingOption.name + L"\" on system setting \"" + systemSettingInfo.name + L"\" when loading module from file " + filePath + L"!"));
						}
					}
				}
			}
		}
	}

	//Apply any system settings changes specified in the loaded module
	for(std::list<SystemStateChange>::const_iterator i = systemSettingsChangeRequests.begin(); i != systemSettingsChangeRequests.end(); ++i)
	{
		//Apply this system state change
		if(!ApplySystemStateChange(*i))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Failed to apply system setting change operating on element with name \"" + i->targetElementName + L"\" when loading module from file " + filePath + L"!"));
		}
	}

	//Map any remaining unmapped controller inputs from the set of loaded devices that
	//have their preferred input keys available.
	for(std::list<InputRegistration>::const_iterator i = inputRegistrationRequests.begin(); i != inputRegistrationRequests.end(); ++i)
	{
		const InputRegistration& defaultInputRegistration = *i;

		//Attempt to map this input to the preferred system key code if one has been
		//specified
		if(defaultInputRegistration.preferredSystemKeyCodeSpecified)
		{
			//If this device input has already been mapped, skip it.
			if(IsDeviceKeyCodeMapped(defaultInputRegistration.targetDevice, defaultInputRegistration.deviceKeyCode))
			{
				continue;
			}

			//Map this input if the desired key is available
			if(!IsKeyCodeMapped(defaultInputRegistration.systemKeyCode))
			{
				if(!MapKeyCode(defaultInputRegistration.targetDevice, defaultInputRegistration.deviceKeyCode, defaultInputRegistration.systemKeyCode))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"MapKeyCode failed on default input registration for " + defaultInputRegistration.targetDevice->GetDeviceInstanceName() + L" with device key code " + defaultInputRegistration.targetDevice->GetKeyCodeName(defaultInputRegistration.deviceKeyCode) + L" and system key code " + GetKeyCodeName(defaultInputRegistration.systemKeyCode) + L"!"));
				}
			}
		}
	}

	//Save all input registration requests from the list of loaded modules into the list
	//of active input registrations.
	inputRegistrationList.splice(inputRegistrationList.end(), inputRegistrationRequests);

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Loaded module from file " + filePath + L"."));

	//Process any view model open requests we encountered during the system load
	LoadModule_ProcessViewModelQueue(viewModelOpenRequests, aviewModelLauncher);

	//Restore running state, or trigger the system to run automatically if we just loaded
	//a program module and the user has instructed the system to run automatically when
	//program modules are loaded.
	if(running || (runWhenProgramModuleLoaded && programModuleLoaded))
	{
		RunSystem();
	}

	//Flag that the load system operation is complete
	loadSystemComplete = true;

	//Return the system load result
	return loadSystemResult;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleInternal(const std::wstring& filePath, const ConnectorMappingList& connectorMappings, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, LoadedModuleInfoList& addedModules)
{
	//Update the name of the currently loading module
	std::wstring fileName = PathGetFileName(filePath);
	PushLoadModuleCurrentModuleName(fileName);

	//Open the target file
	FileStreamReference sourceStreamReference(*guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not open module file " + filePath + L"!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	Stream::IStream& source = *sourceStreamReference;

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
	source.ProcessByteOrderMark();

	//Load the XML structure from the file
	HeirarchicalStorageTree tree;
	if(!tree.LoadTree(source))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error loading XML structure from module file " + filePath + L"! The xml error string is as follows: " + tree.GetErrorString()));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();

	//Load external binary data into the XML tree
	std::list<IHeirarchicalStorageNode*> binaryList;
	binaryList = tree.GetBinaryDataNodeList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
	{
		std::wstring binaryFileName = (*i)->GetBinaryDataBufferName();
		std::wstring binaryFilePath = binaryFileName;

		//If the file path contains a relative path to the target, resolve the relative
		//file path using the directory containing the module file as a base.
		std::wstring fileDir = PathGetDirectory(filePath);
		if(PathIsRelativePath(binaryFilePath))
		{
			binaryFilePath = PathCombinePaths(fileDir, binaryFilePath);
		}

		//Open the target file
		FileStreamReference binaryFileStreamReference(*guiExtensionInterface);
		if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + filePath + L" because a binary data file could not be found in the target path " + binaryFilePath + L"!"));
			PopLoadModuleCurrentModuleName();
			return false;
		}
		Stream::IStream& dataStream = *binaryFileStreamReference;

		//Obtain a reference to the binary data stream within this XML element
		Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
		binaryData.SetStreamPos(0);

		//Load the external binary data into the binary data stream for this XML element
		unsigned int bufferSize = (unsigned int)dataStream.Size();
		unsigned char* buffer = new unsigned char[bufferSize];
		if(!dataStream.ReadData(buffer, bufferSize))
		{
			delete[] buffer;
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + filePath + L" because there was an error reading binary data from file " + binaryFilePath + L"!"));
			PopLoadModuleCurrentModuleName();
			return false;
		}
		if(!binaryData.WriteData(buffer, bufferSize))
		{
			delete[] buffer;
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + filePath + L" because there was an error saving binary data read from file " + binaryFilePath + L"!"));
			PopLoadModuleCurrentModuleName();
			return false;
		}
		delete[] buffer;
	}

	//If this is a system module, defer processing of this module to the LoadSystem
	//method, otherwise verify that this file is marked as a module.
	if(rootNode.GetName() == L"System")
	{
		bool result = LoadSystem(filePath, rootNode, viewModelOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModules);
		PopLoadModuleCurrentModuleName();
		return result;
	}
	else if(rootNode.GetName() != L"Module")
	{
		//Neither a system nor a module root node was found. Abort any further processing.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error loading module file " + filePath + L"! The root node was not of type System or Module!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//Create a new module info structure for this module
	LoadedModuleInfoInternal moduleInfo;
	moduleInfo.moduleID = GenerateFreeModuleID();
	moduleInfo.filePath = filePath;

	//Extract mandatory module metadata
	IHeirarchicalStorageAttribute* systemClassNameAttribute = rootNode.GetAttribute(L"SystemClassName");
	IHeirarchicalStorageAttribute* moduleClassNameAttribute = rootNode.GetAttribute(L"ModuleClassName");
	IHeirarchicalStorageAttribute* moduleInstanceNameAttribute = rootNode.GetAttribute(L"ModuleInstanceName");
	if((systemClassNameAttribute == 0) || (moduleClassNameAttribute == 0) || (moduleInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SystemClassName, ModuleClassName, or ModuleInstanceName attribute on root node!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	moduleInfo.systemClassName = systemClassNameAttribute->GetValue();
	moduleInfo.className = moduleClassNameAttribute->GetValue();
	moduleInfo.instanceName = moduleInstanceNameAttribute->GetValue();

	//Load any additional optional metadata
	IHeirarchicalStorageAttribute* programModuleAttribute = rootNode.GetAttribute(L"ProgramModule");
	if(programModuleAttribute != 0)
	{
		moduleInfo.programModule = programModuleAttribute->ExtractValue<bool>();
	}
	IHeirarchicalStorageAttribute* manufacturerCodeAttribute = rootNode.GetAttribute(L"ManufacturerCode");
	if(manufacturerCodeAttribute != 0)
	{
		moduleInfo.manufacturerCode = manufacturerCodeAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* moduleDisplayNameAttribute = rootNode.GetAttribute(L"ModuleDisplayName");
	if(moduleDisplayNameAttribute != 0)
	{
		moduleInfo.displayName = moduleDisplayNameAttribute->GetValue();
	}

	//Check for existing modules with the same instance name, and ensure a unique instance
	//name is generated if a conflict is found.
	unsigned int instanceNameCurrentPostfixNumber = 0;
	LoadedModuleInfoList::const_iterator instanceNameModuleIterator = loadedModuleInfoList.begin();
	std::wstring initialModuleInstanceName = moduleInfo.instanceName;
	while(instanceNameModuleIterator != loadedModuleInfoList.end())
	{
		//If the instance name is the same, try and make a new unique instance name for
		//our module.
		if(instanceNameModuleIterator->instanceName == moduleInfo.instanceName)
		{
			//Determine which postfix number to use
			unsigned int numericPostfix = ++instanceNameCurrentPostfixNumber;

			//Convert the numeric postfix to a string
			std::wstringstream numericPostfixToString;
			numericPostfixToString << numericPostfix;

			//Rebuild our instance name using the postfix
			moduleInfo.instanceName = initialModuleInstanceName + L" [" + numericPostfixToString.str() + L"]";

			//Restart the loop
			instanceNameModuleIterator = loadedModuleInfoList.begin();
			continue;
		}
		++instanceNameModuleIterator;
	}

	//Generate the display name for the module if one hasn't been specified
	if(moduleInfo.displayName.empty())
	{
		moduleInfo.displayName = moduleInfo.instanceName;
	}

	//Check for existing modules with the same display name, and ensure a unique display
	//name is generated if a conflict is found.
	unsigned int displayNameCurrentPostfixNumber = 0;
	LoadedModuleInfoList::const_iterator displayNameModuleIterator = loadedModuleInfoList.begin();
	std::wstring initialModuleDisplayName = moduleInfo.displayName;
	while(displayNameModuleIterator != loadedModuleInfoList.end())
	{
		//If the display name is the same, try and make a new unique display name for our
		//module.
		if(displayNameModuleIterator->displayName == moduleInfo.displayName)
		{
			//Determine which postfix number to use
			unsigned int numericPostfix = ++displayNameCurrentPostfixNumber;

			//Convert the numeric postfix to a string
			std::wstringstream numericPostfixToString;
			numericPostfixToString << numericPostfix;

			//Rebuild our display name using the postfix
			moduleInfo.displayName = initialModuleDisplayName + L" [" + numericPostfixToString.str() + L"]";

			//Restart the loop
			displayNameModuleIterator = loadedModuleInfoList.begin();
			continue;
		}
		++displayNameModuleIterator;
	}

	//Load the elements from the root node one by one
	bool loadedWithoutErrors = true;
	NameToIDMap connectorNameToIDMap;
	NameToIDMap lineGroupNameToIDMap;
	unsigned int entriesProcessed = 0;
	std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
	unsigned int entryCount = (unsigned int)childList.size();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
	{
		loadSystemProgress = ((float)++entriesProcessed / (float)entryCount);

		std::wstring elementName = (*i)->GetName();
		if(elementName == L"Device")
		{
			loadedWithoutErrors &= LoadModule_Device(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.SetDependentDevice")
		{
			loadedWithoutErrors &= LoadModule_Device_SetDependentDevice(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.ReferenceDevice")
		{
			loadedWithoutErrors &= LoadModule_Device_ReferenceDevice(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.ReferenceExtension")
		{
			loadedWithoutErrors &= LoadModule_Device_ReferenceExtension(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.ReferenceBus")
		{
			loadedWithoutErrors &= LoadModule_Device_ReferenceBus(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.ReferenceClockSource")
		{
			loadedWithoutErrors &= LoadModule_Device_ReferenceClockSource(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Device.RegisterInput")
		{
			loadedWithoutErrors &= LoadModule_Device_RegisterInput(*(*i), moduleInfo.moduleID, inputRegistrationRequests);
		}
		else if(elementName == L"GlobalExtension")
		{
			loadedWithoutErrors &= LoadModule_GlobalExtension(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Extension")
		{
			loadedWithoutErrors &= LoadModule_Extension(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Extension.ReferenceDevice")
		{
			loadedWithoutErrors &= LoadModule_Extension_ReferenceDevice(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Extension.ReferenceExtension")
		{
			loadedWithoutErrors &= LoadModule_Extension_ReferenceExtension(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Extension.ReferenceBus")
		{
			loadedWithoutErrors &= LoadModule_Extension_ReferenceBus(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"Extension.ReferenceClockSource")
		{
			loadedWithoutErrors &= LoadModule_Extension_ReferenceClockSource(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface")
		{
			loadedWithoutErrors &= LoadModule_BusInterface(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.DefineLineGroup")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_DefineLineGroup(*(*i), moduleInfo.moduleID, lineGroupNameToIDMap);
		}
		else if(elementName == L"BusInterface.DefineCELineMemory")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_DefineCELineMemory(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.DefineCELinePort")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_DefineCELinePort(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapCELineInputMemory")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapCELineInputMemory(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapCELineInputPort")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapCELineInputPort(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapCELineOutputMemory")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapCELineOutputMemory(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapCELineOutputPort")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapCELineOutputPort(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapDevice")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapDevice(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapPort")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapPort(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.MapLine")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapLine(*(*i), moduleInfo.moduleID, lineGroupNameToIDMap);
		}
		else if(elementName == L"BusInterface.MapClockSource")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_MapClockSource(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"BusInterface.UnmappedLineState")
		{
			loadedWithoutErrors &= LoadModule_BusInterface_UnmappedLineState(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"ClockSource")
		{
			loadedWithoutErrors &= LoadModule_ClockSource(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"ClockSource.SetInputClockSource")
		{
			loadedWithoutErrors &= LoadModule_ClockSource_SetInputClockSource(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"System.OpenViewModel")
		{
			//Note that we don't process view model open requests immediately when they
			//are encountered. These requests are only processed once we're sure the
			//module has loaded successfully. This removes the need to worry about locked
			//threads waiting for views to open preventing us from doing cleanup of this
			//module, in the case that the load fails.
			loadedWithoutErrors &= LoadModule_System_OpenViewModel(*(*i), moduleInfo.moduleID, viewModelOpenRequests);
		}
		else if(elementName == L"System.ExportConnector")
		{
			loadedWithoutErrors &= LoadModule_System_ExportConnector(*(*i), moduleInfo.moduleID, moduleInfo.systemClassName, connectorNameToIDMap);
		}
		else if(elementName == L"System.ExportDevice")
		{
			loadedWithoutErrors &= LoadModule_System_ExportDevice(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ExportExtension")
		{
			loadedWithoutErrors &= LoadModule_System_ExportExtension(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ExportBusInterface")
		{
			loadedWithoutErrors &= LoadModule_System_ExportBusInterface(*(*i), moduleInfo.moduleID, connectorNameToIDMap, lineGroupNameToIDMap);
		}
		else if(elementName == L"System.ExportClockSource")
		{
			loadedWithoutErrors &= LoadModule_System_ExportClockSource(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ExportSystemLine")
		{
			loadedWithoutErrors &= LoadModule_System_ExportSystemLine(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ExportSystemSetting")
		{
			loadedWithoutErrors &= LoadModule_System_ExportSystemSetting(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportConnector")
		{
			//Note that we actually need to explicitly instruct the system on every
			//connector to import, specifying a target connector. This is required, in
			//order to allow the user to specify connections between modules before
			//performing a load.
			loadedWithoutErrors &= LoadModule_System_ImportConnector(*(*i), moduleInfo.moduleID, moduleInfo.systemClassName, connectorMappings, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportDevice")
		{
			loadedWithoutErrors &= LoadModule_System_ImportDevice(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportExtension")
		{
			loadedWithoutErrors &= LoadModule_System_ImportExtension(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportBusInterface")
		{
			loadedWithoutErrors &= LoadModule_System_ImportBusInterface(*(*i), moduleInfo.moduleID, connectorNameToIDMap, lineGroupNameToIDMap);
		}
		else if(elementName == L"System.ImportClockSource")
		{
			loadedWithoutErrors &= LoadModule_System_ImportClockSource(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportSystemLine")
		{
			loadedWithoutErrors &= LoadModule_System_ImportSystemLine(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.ImportSystemSetting")
		{
			loadedWithoutErrors &= LoadModule_System_ImportSystemSetting(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else if(elementName == L"System.DefineEmbeddedROM")
		{
			loadedWithoutErrors &= LoadModule_System_DefineEmbeddedROM(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"System.DefineSystemLine")
		{
			loadedWithoutErrors &= LoadModule_System_DefineSystemLine(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"System.MapSystemLine")
		{
			loadedWithoutErrors &= LoadModule_System_MapSystemLine(*(*i), moduleInfo.moduleID);
		}
		else if(elementName == L"System.Setting")
		{
			loadedWithoutErrors &= LoadModule_System_Setting(*(*i), moduleInfo.moduleID, fileName);
		}
		else if(elementName == L"System.SelectSettingOption")
		{
			SystemStateChange systemStateChange;
			if(LoadModule_System_SelectSettingOption(*(*i), moduleInfo.moduleID, systemStateChange))
			{
				systemSettingsChangeRequests.push_back(systemStateChange);
			}
			else
			{
				loadedWithoutErrors = false;
			}
		}
		else if(elementName == L"System.SetClockFrequency")
		{
			SystemStateChange systemStateChange;
			if(LoadModule_System_SetClockFrequency(*(*i), moduleInfo.moduleID, systemStateChange))
			{
				systemSettingsChangeRequests.push_back(systemStateChange);
			}
			else
			{
				loadedWithoutErrors = false;
			}
		}
		else if(elementName == L"System.SetLineState")
		{
			SystemStateChange systemStateChange;
			if(LoadModule_System_SetLineState(*(*i), moduleInfo.moduleID, systemStateChange))
			{
				systemSettingsChangeRequests.push_back(systemStateChange);
			}
			else
			{
				loadedWithoutErrors = false;
			}
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized element: " + elementName + L" when loading module file " + filePath + L"."));
		}
	}

	//Add the info for this module to the list of loaded modules
	addedModules.push_back(moduleInfo);
	loadedModuleInfoList.push_back(moduleInfo);

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading module from file " + filePath + L"."));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//If any errors occurred while loading the module file, log the error and return
	//false.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Errors occurred while loading module from file " + filePath + L"."));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//Update the currently loading module name stack
	PopLoadModuleCurrentModuleName();

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_Device_Settings(IHeirarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID and device name
	IHeirarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if((moduleIDAttribute == 0) || (deviceInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ModuleID or DeviceInstanceName attribute for Device.Settings!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for Device.Settings!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name " + deviceName + L" for Device.Settings!"));
		return false;
	}

	//Load the device settings from this node
	device->LoadSettingsState(node);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_Device_MapInput(IHeirarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHeirarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* systemKeyCodeAttribute = node.GetAttribute(L"SystemKeyCode");
	IHeirarchicalStorageAttribute* deviceKeyCodeAttribute = node.GetAttribute(L"DeviceKeyCode");
	if((moduleIDAttribute == 0) || (deviceInstanceNameAttribute == 0) || (systemKeyCodeAttribute == 0) || (deviceKeyCodeAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ModuleID, DeviceInstanceName, SystemKeyCode or DeviceKeyCode attribute for Device.MapInput!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring systemKeyCodeString = systemKeyCodeAttribute->GetValue();
	std::wstring deviceKeyCodeString = deviceKeyCodeAttribute->GetValue();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for Device.MapInput!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name " + deviceName + L" for Device.MapInput!"));
		return false;
	}

	//Translate the device key code
	unsigned int deviceKeyCode = device->GetKeyCodeID(deviceKeyCodeString);
	if(deviceKeyCode == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device key code with name " + deviceKeyCodeString + L" on device " + deviceName + L" for Device.MapInput!"));
		return false;
	}

	//Translate the system key code
	IDeviceContext::KeyCode systemKeyCode = GetKeyCodeID(systemKeyCodeString);
	if(systemKeyCode == IDeviceContext::KEYCODE_NONE)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate system key code with name " + systemKeyCodeString + L" for Device.MapInput!"));
		return false;
	}

	//Add the key code mapping to the system
	if(!MapKeyCode(device, deviceKeyCode, systemKeyCode))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"MapKeyCode failed on Device.MapInput key for " + deviceName + L" with device key code " + deviceKeyCodeString + L" and system key code " + systemKeyCodeString + L" for Device.MapInput!"));
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_System_LoadEmbeddedROMData(const std::wstring& fileDir, IHeirarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHeirarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* interfaceNumberAttribute = node.GetAttribute(L"InterfaceNumber");
	IHeirarchicalStorageAttribute* filePathAttribute = node.GetAttribute(L"FilePath");
	if((moduleIDAttribute == 0) || (deviceInstanceNameAttribute == 0) || (interfaceNumberAttribute == 0) || (filePathAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ModuleID, DeviceInstanceName, InterfaceNumber or FilePath attribute for System.LoadEmbeddedROMData!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	unsigned int interfaceNumber = interfaceNumberAttribute->ExtractValue<unsigned int>();
	std::wstring filePath = filePathAttribute->GetValue();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for System.LoadEmbeddedROMData!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name " + deviceName + L" for System.LoadEmbeddedROMData!"));
		return false;
	}

	//Locate the defined ROM info for this embedded ROM
	EmbeddedROMInfo* embeddedROMInfoEntry = 0;
	for(EmbeddedROMList::iterator romInfoIterator = embeddedROMInfo.begin(); romInfoIterator != embeddedROMInfo.end(); ++romInfoIterator)
	{
		if(romInfoIterator->targetDevice == device)
		{
			embeddedROMInfoEntry = &(*romInfoIterator);
		}
	}
	if(embeddedROMInfoEntry == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate matching embedded ROM definition for device with name " << deviceName << L" and interface number " << interfaceNumber << L" for System.LoadEmbeddedROMData!";
		WriteLogEvent(logEntry);
		return false;
	}

	//If the file path contains a relative path to the target, resolve the relative file
	//path using the directory containing the system file as a base.
	if(PathIsRelativePath(filePath))
	{
		filePath = PathCombinePaths(fileDir, filePath);
	}

	//Save the new file path as the selected file path for this embedded ROM file
	embeddedROMInfoEntry->filePath = filePath;

	//Open the target file
	FileStreamReference fileStreamReference(*guiExtensionInterface);
	if(!fileStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to open target file with path \"" + filePath + L"\" when attempting to load embedded ROM data for device with name " + deviceName + L" for System.LoadEmbeddedROMData!"));
		return false;
	}
	Stream::IStream& file = *fileStreamReference;

	//Load the data from the target file into this embedded ROM
	for(unsigned int i = 0; (i < file.Size()) && (i < embeddedROMInfoEntry->romRegionSize); ++i)
	{
		unsigned char byte;
		if(file.ReadData(byte))
		{
			device->TransparentWriteInterface(interfaceNumber, i, Data(8, byte), 0, 0);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_System_SelectSettingOption(IHeirarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap, SystemStateChange& stateChange)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHeirarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	if(moduleIDAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ModuleID attribute for System.SelectSettingOption!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for System.SelectSettingOption!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Defer the remainder of this load operation to the module loader method
	return LoadModule_System_SelectSettingOption(node, moduleID, stateChange);
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem(const std::wstring& filePath, IHeirarchicalStorageNode& rootNode, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, LoadedModuleInfoList& addedModules)
{
	//Extract the module relationships data from the file
	bool moduleRelationshipsLoaded = false;
	SavedRelationshipMap savedRelationshipData;
	std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		//Load the ModuleRelationships node
		if((*i)->GetName() == L"ModuleRelationships")
		{
			moduleRelationshipsLoaded = LoadSavedRelationshipMap(*(*i), savedRelationshipData);
		}
	}

	//Validate that we managed to retrieve the module relationships data from the file
	if(!moduleRelationshipsLoaded)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load system from file " + filePath + L" because the ModuleRelationships node could not be loaded!"));
		return false;
	}

	//Attempt to load each module referenced in the saved module relationship data
	std::wstring fileDir = PathGetDirectory(filePath);
	bool modulesLoadedWithoutErrors = true;
	std::map<unsigned int, unsigned int> savedModuleIDToLoadedModuleIDMap;
	for(SavedRelationshipMap::const_iterator i = savedRelationshipData.begin(); !loadSystemAbort && modulesLoadedWithoutErrors && (i != savedRelationshipData.end()); ++i)
	{
		const SavedRelationshipModule& savedModuleInfo = i->second;

		//Attempt to find available connectors to import that match the imported
		//connectors in the saved system
		ConnectorMappingList targetModuleConnectorMappings;
		for(std::list<SavedRelationshipImportConnector>::const_iterator importedConnector = savedModuleInfo.importedConnectors.begin(); importedConnector != savedModuleInfo.importedConnectors.end(); ++importedConnector)
		{
			//Retrieve the loaded module ID of the target module
			std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(importedConnector->moduleID);
			if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
			{
				continue;
			}
			unsigned int loadedModuleID = loadedModuleIDIterator->second;

			//Attempt to find a matching available connector
			bool foundMatchingConnector = false;
			ConnectorDetailsMap::const_iterator connectorMapIterator = connectorDetailsMap.begin();
			while(!foundMatchingConnector && (connectorMapIterator != connectorDetailsMap.end()))
			{
				const ConnectorInfoInternal& connectorDetails = connectorMapIterator->second;
				if((connectorDetails.exportingModuleID == loadedModuleID) && !connectorDetails.connectorUsed && (connectorDetails.exportingModuleConnectorInstanceName == importedConnector->instanceNameRemote))
				{
					foundMatchingConnector = true;
					continue;
				}
				++connectorMapIterator;
			}
			if(!foundMatchingConnector)
			{
				continue;
			}

			//Create a connector mapping for this connector
			const ConnectorInfoInternal& connectorDetails = connectorMapIterator->second;
			ConnectorMapping connectorMapping;
			connectorMapping.connectorID = connectorDetails.connectorID;
			connectorMapping.importingModuleConnectorInstanceName = importedConnector->instanceNameLocal;
			targetModuleConnectorMappings.push_back(connectorMapping);
		}

		//If the system file contains a relative path to this module, resolve the relative
		//file path using the directory containing the system file as a base.
		std::wstring savedModuleFileDir = PathGetDirectory(savedModuleInfo.filePath);
		if(PathIsRelativePath(savedModuleFileDir))
		{
			savedModuleFileDir = PathCombinePaths(fileDir, savedModuleFileDir);
		}

		//Attempt to load this module
		std::wstring savedModuleFilePath = PathCombinePaths(savedModuleFileDir, PathGetFileName(savedModuleInfo.filePath));
		modulesLoadedWithoutErrors &= LoadModuleInternal(savedModuleFilePath, targetModuleConnectorMappings, viewModelOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModules);

		//Save the mapping between the saved module ID and the loaded module ID for the
		//module we just loaded.
		//##FIX## If we referenced another system file, this module ID will be
		//meaningless. But then again, we don't really want to support referencing a
		//system file from within a system file. Add some kind of verification during
		//module loading to prevent this. Once we've done that, this code below will be
		//acceptable.
		LoadedModuleInfoList::const_reverse_iterator addedModuleIterator = addedModules.rbegin();
		if(addedModuleIterator != addedModules.rend())
		{
			savedModuleIDToLoadedModuleIDMap.insert(std::pair<unsigned int, unsigned int>(savedModuleInfo.moduleID, addedModuleIterator->moduleID));
		}
	}

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading system from file " + filePath + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error and return
	//false.
	if(!modulesLoadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"One or more modules failed to load successfully when loading system from file " + filePath + L"."));
		return false;
	}

	//Load the elements from the root node one by one
	bool loadedWithoutErrors = true;
	NameToIDMap connectorNameToIDMap;
	NameToIDMap lineGroupNameToIDMap;
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
	{
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"ModuleRelationships")
		{
			//We've already processed the ModuleRelationships node above, so we skip it
			//here.
		}
		else if(elementName == L"Info")
		{
			//We expect an Info node to exist, but we're not interested in its contents at
			//this stage, so we skip it here.
		}
		else if(elementName == L"Device.Settings")
		{
			loadedWithoutErrors &= LoadSystem_Device_Settings(*(*i), savedModuleIDToLoadedModuleIDMap);
		}
		else if(elementName == L"Device.MapInput")
		{
			loadedWithoutErrors &= LoadSystem_Device_MapInput(*(*i), savedModuleIDToLoadedModuleIDMap);
		}
		else if(elementName == L"System.LoadEmbeddedROMData")
		{
			loadedWithoutErrors &= LoadSystem_System_LoadEmbeddedROMData(fileDir, *(*i), savedModuleIDToLoadedModuleIDMap);
		}
		else if(elementName == L"System.SelectSettingOption")
		{
			SystemStateChange systemStateChange;
			if(LoadSystem_System_SelectSettingOption(*(*i), savedModuleIDToLoadedModuleIDMap, systemStateChange))
			{
				systemSettingsChangeRequests.push_back(systemStateChange);
			}
			else
			{
				loadedWithoutErrors = false;
			}
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized element: " + elementName + L" when loading system file " + filePath + L"."));
		}
	}

	//If the system load was aborted, log the event, and flag the module to unload.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading system from file " + filePath + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error, and flag the
	//module to unload.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Errors occurred while loading system from file " + filePath + L"."));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::SaveSystem(const std::wstring& filePath)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Create the new system XML tree
	HeirarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"System");

	//Fill in general information about the file
	IHeirarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
	Timestamp timestamp = GetTimestamp();
	stateInfo.CreateAttribute(L"CreationDate", timestamp.GetDate());
	stateInfo.CreateAttribute(L"CreationTime", timestamp.GetTime());

	//Save the ModuleRelationships node
	std::wstring fileDir = PathGetDirectory(filePath);
	IHeirarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	SaveModuleRelationshipsNode(moduleRelationshipsNode, true, fileDir);

	//Save device settings to the system file
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		IHeirarchicalStorageNode& node = tree.GetRootNode().CreateChild(L"Device.Settings");
		node.CreateAttribute(L"DeviceInstanceName", (*i).device->GetDeviceInstanceName());
		node.CreateAttribute(L"ModuleID").SetValue((*i).moduleID);
		(*i).device->SaveSettingsState(node);
	}

	//Save mapped key inputs to the system file
	{
		boost::mutex::scoped_lock lock(inputMutex);
		for(InputKeyMap::const_iterator i = inputKeyMap.begin(); i != inputKeyMap.end(); ++i)
		{
			const InputMapEntry& inputMapEntry = i->second;

			//Find the module ID for the target device
			unsigned int moduleID = 0;
			bool foundModuleID = false;
			LoadedDeviceInfoList::const_iterator deviceInfoListIterator = loadedDeviceInfoList.begin();
			while(!foundModuleID && (deviceInfoListIterator != loadedDeviceInfoList.end()))
			{
				if(deviceInfoListIterator->device == inputMapEntry.targetDevice)
				{
					moduleID = deviceInfoListIterator->moduleID;
					foundModuleID = true;
					continue;
				}
				++deviceInfoListIterator;
			}
			if(!foundModuleID)
			{
				continue;
			}

			//Create the input registration node for this input key map entry
			IHeirarchicalStorageNode& registerInputNode = tree.GetRootNode().CreateChild(L"Device.MapInput");
			registerInputNode.CreateAttribute(L"ModuleID", moduleID);
			registerInputNode.CreateAttribute(L"DeviceInstanceName", inputMapEntry.targetDevice->GetDeviceInstanceName());
			registerInputNode.CreateAttribute(L"SystemKeyCode", GetKeyCodeName(inputMapEntry.keyCode));
			registerInputNode.CreateAttribute(L"DeviceKeyCode", inputMapEntry.targetDevice->GetKeyCodeName(inputMapEntry.targetDeviceKeyCode));
		}
	}

	//Save current system setting selections to the system file
	for(SystemSettingsMap::const_iterator i = systemSettings.begin(); i != systemSettings.end(); ++i)
	{
		const SystemSettingInfo& systemSettingInfo = i->second;
		if(systemSettingInfo.selectedOption < (unsigned int)systemSettingInfo.options.size())
		{
			//Create the system option selection node for this setting
			IHeirarchicalStorageNode& selectSettingOptionNode = tree.GetRootNode().CreateChild(L"System.SelectSettingOption");
			selectSettingOptionNode.CreateAttribute(L"ModuleID", systemSettingInfo.moduleID);
			selectSettingOptionNode.CreateAttribute(L"SettingName", systemSettingInfo.name);
			selectSettingOptionNode.CreateAttribute(L"OptionName", systemSettingInfo.options[systemSettingInfo.selectedOption].name);
		}
	}

	//Save embedded ROM file selections to the system file
	for(EmbeddedROMList::const_iterator i = embeddedROMInfo.begin(); i != embeddedROMInfo.end(); ++i)
	{
		//If no file path has been specified for this embedded ROM, skip it.
		if(i->filePath.empty())
		{
			continue;
		}

		//If the target file is contained within the same directory or a subdirectory of
		//the location where the system file is being saved, convert the path to the ROM
		//file into a relative path.
		std::wstring filePath = i->filePath;
		if(PathStartsWithBasePath(fileDir, filePath))
		{
			std::wstring relativePath;
			if(PathBuildRelativePathToTarget(fileDir, filePath, true, relativePath))
			{
				filePath = relativePath;
			}
		}

		//Create the embedded ROM selection node for this setting
		IHeirarchicalStorageNode& loadROMDataNode = tree.GetRootNode().CreateChild(L"System.LoadEmbeddedROMData");
		loadROMDataNode.CreateAttribute(L"ModuleID", i->moduleID);
		loadROMDataNode.CreateAttribute(L"DeviceInstanceName", i->targetDevice->GetDeviceInstanceName());
		loadROMDataNode.CreateAttribute(L"InterfaceNumber", i->interfaceNumber);
		loadROMDataNode.CreateAttribute(L"FilePath", filePath);
	}

	//Save XML tree to the target file
	Stream::File file(Stream::IStream::TEXTENCODING_UTF8);
	if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save system to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}
	file.InsertByteOrderMark();
	if(!tree.SaveTree(file))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save system to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
		if(running)
		{
			RunSystem();
		}
		return false;
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Saved system to file " + filePath));

	//Restore running state
	if(running)
	{
		RunSystem();
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool System::UnloadModule(unsigned int moduleID)
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Unload the module
	UnloadModuleInternal(moduleID);

	//Attempt to validate the system now that the module has been unloaded
	bool result = ValidateSystem();

	//Release the lock on debugMutex before we call RunSystem below
	lock.unlock();

	//Restore running state
	if(running && result)
	{
		RunSystem();
	}

	return result;
}

//----------------------------------------------------------------------------------------
void System::UnloadModuleInternal(unsigned int moduleID)
{
	//Retrieve information on the target module
	bool foundModuleEntry = false;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	while(!foundModuleEntry && (loadedModuleIterator != loadedModuleInfoList.end()))
	{
		if(loadedModuleIterator->moduleID == moduleID)
		{
			foundModuleEntry = true;
			continue;
		}
		++loadedModuleIterator;
	}
	if(!foundModuleEntry)
	{
		return;
	}
	const LoadedModuleInfoInternal& moduleInfo = *loadedModuleIterator;

	//Update the name stack of the currently unloading module
	std::wstring fileName = PathGetFileName(moduleInfo.filePath);
	PushUnloadModuleCurrentModuleName(fileName);

	//Build a list of any modules which import connectors exported by this module. These
	//modules are dependent on our module, and must be unloaded first.
	std::list<unsigned int> dependentModulesToUnload;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		const ConnectorInfoInternal& connectorDetails = i->second;
		if((connectorDetails.exportingModuleID == moduleID) && connectorDetails.connectorUsed)
		{
			dependentModulesToUnload.push_back(connectorDetails.importingModuleID);
		}
	}

	//Unload any modules which are dependent on this module
	for(std::list<unsigned int>::const_iterator i = dependentModulesToUnload.begin(); i != dependentModulesToUnload.end(); ++i)
	{
		//##TODO## Consider checking if the module ID is still valid. It's possible that
		//one dependent device in our list will unload another dependent device before we
		//get to it. This shouldn't be harmful, but isn't great design.
		UnloadModuleInternal(*i);
	}

	//If this module passed validation, perform additional unloading tasks which require
	//participation from the loaded elements within this module.
	if(moduleInfo.moduleValidated)
	{
		//Save any persistent state for the loaded devices within this module
		if(enablePersistentState)
		{
			std::wstring persistentModuleFileName = moduleInfo.className + L".zip";
			std::wstring persistentModuleFilePath = PathCombinePaths(guiExtensionInterface->GetGlobalPreferencePathPersistentState(), persistentModuleFileName);
			if(!SavePersistentStateForModule(persistentModuleFilePath, moduleID, FILETYPE_ZIP, true))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Failed to save persistent state data for module with name " + moduleInfo.displayName + L" to file \"" + persistentModuleFilePath + L"\"."));
			}
		}

		//Negate any active output lines that are being asserted by devices in this
		//module. This will allow other devices in the system that are not being unloaded
		//with this module to correctly restore their input line state. We only perform
		//this operation for validated modules, since modules that have not yet been
		//validated have not yet asserted any output line state, and devices within that
		//module may not yet have been initialized.
		for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
		{
			if(i->moduleID == moduleID)
			{
				i->device->NegateCurrentOutputLineState();
			}
		}
	}

	//Free any connectors this module was importing
	for(ConnectorDetailsMap::iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		ConnectorInfoInternal& connectorDetails = i->second;
		if(connectorDetails.connectorUsed && (connectorDetails.importingModuleID == moduleID))
		{
			connectorDetails.connectorUsed = false;
			connectorDetails.importingModuleID = 0;
		}
	}

	//Remove any connectors this module was exporting
	ConnectorDetailsMap::iterator nextConnectorDetailsMapEntry = connectorDetailsMap.begin();
	while(nextConnectorDetailsMapEntry != connectorDetailsMap.end())
	{
		ConnectorDetailsMap::iterator currentElement = nextConnectorDetailsMapEntry;
		++nextConnectorDetailsMapEntry;
		if(currentElement->second.exportingModuleID == moduleID)
		{
			connectorDetailsMap.erase(currentElement);
		}
	}

	//Remove any clock sources which belong to the module
	ClockSourceList::iterator nextclockSourceEntry = clockSources.begin();
	while(nextclockSourceEntry != clockSources.end())
	{
		ClockSourceList::iterator currentElement = nextclockSourceEntry;
		++nextclockSourceEntry;
		if(currentElement->moduleID == moduleID)
		{
			//Remove any references to this clock source, IE, through
			//ReferenceClockSource.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->clockSource);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(currentElement->clockSource);
			}

			//If this clock source receives an input from another clock source, remove
			//this clock source as a dependent clock source of the parent.
			ClockSource* inputClockSource = currentElement->clockSource->GetInputClockSource();
			if(inputClockSource != 0)
			{
				inputClockSource->RemoveDependentClockSource(currentElement->clockSource);
			}

			//Remove any input references other clock sources may have on this clock
			//source
			currentElement->clockSource->RemoveAllDependentClockSources();

			//Delete the clock source
			delete currentElement->clockSource;
			clockSources.erase(currentElement);
		}
	}

	//Remove any bus interfaces which belong to the module
	BusInterfaceList::iterator nextBusInterfaceEntry = busInterfaces.begin();
	while(nextBusInterfaceEntry != busInterfaces.end())
	{
		BusInterfaceList::iterator currentElement = nextBusInterfaceEntry;
		++nextBusInterfaceEntry;
		if(currentElement->moduleID == moduleID)
		{
			//Remove any references to this bus, IE, through ReferenceBus.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->busInterface);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(currentElement->busInterface);
			}

			//Delete the bus interface
			delete currentElement->busInterface;
			busInterfaces.erase(currentElement);
		}
	}

	//Remove any devices which belong to the module
	LoadedDeviceInfoList::iterator nextDeviceEntry = loadedDeviceInfoList.begin();
	while(nextDeviceEntry != loadedDeviceInfoList.end())
	{
		LoadedDeviceInfoList::iterator currentElement = nextDeviceEntry;
		++nextDeviceEntry;
		if(currentElement->moduleID == moduleID)
		{
			//Remove any references to this device, IE, through ReferenceDevice or
			//SetDependentDevice.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->device);
				i->deviceContext->RemoveDeviceDependency(currentElement->deviceContext);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(currentElement->device);
			}

			//Delete the device
			UnloadDevice(currentElement->device);
			loadedDeviceInfoList.erase(currentElement);
		}
	}

	//Remove any extensions which belong to the module
	LoadedExtensionInfoList::iterator nextExtensionEntry = loadedExtensionInfoList.begin();
	while(nextExtensionEntry != loadedExtensionInfoList.end())
	{
		LoadedExtensionInfoList::iterator currentElement = nextExtensionEntry;
		++nextExtensionEntry;
		if(currentElement->moduleID == moduleID)
		{
			//Remove any references to this extension, IE, through ReferenceExtension.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->extension);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(currentElement->extension);
			}

			//Delete the extension
			UnloadExtension(currentElement->extension);
			loadedExtensionInfoList.erase(currentElement);
		}
	}

	//Remove references to any global extensions which are used by this module
	LoadedGlobalExtensionInfoList::iterator nextGlobalExtensionEntry = globalExtensionInfoList.begin();
	while(nextGlobalExtensionEntry != globalExtensionInfoList.end())
	{
		LoadedGlobalExtensionInfoList::iterator currentElement = nextGlobalExtensionEntry;
		++nextGlobalExtensionEntry;

		//Remove this module from the list of referencing modules for this global
		//extension
		currentElement->second.moduleIDs.erase(moduleID);

		//If the global extension is no longer referenced by any modules, remove it.
		if(currentElement->second.moduleIDs.empty())
		{
			//Remove any references to this extension, IE, through ReferenceExtension.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->second.extension);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(currentElement->second.extension);
			}

			//Delete the extension
			UnloadExtension(currentElement->second.extension);
			globalExtensionInfoList.erase(currentElement);
		}
	}

	//Remove any clock source import entries which belong to the module
	ImportedClockSourceList::iterator nextImportedClockSourceEntry = importedClockSources.begin();
	while(nextImportedClockSourceEntry != importedClockSources.end())
	{
		ImportedClockSourceList::iterator currentElement = nextImportedClockSourceEntry;
		++nextImportedClockSourceEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedClockSources.erase(currentElement);
		}
	}

	//Remove any system option import entries which belong to the module
	ImportedSystemSettingList::iterator nextImportedSystemSettingEntry = importedSystemSettings.begin();
	while(nextImportedSystemSettingEntry != importedSystemSettings.end())
	{
		ImportedSystemSettingList::iterator currentElement = nextImportedSystemSettingEntry;
		++nextImportedSystemSettingEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedSystemSettings.erase(currentElement);
		}
	}

	//Remove any system line import entries which belong to the module
	ImportedSystemLineList::iterator nextImportedSystemLineEntry = importedSystemLines.begin();
	while(nextImportedSystemLineEntry != importedSystemLines.end())
	{
		ImportedSystemLineList::iterator currentElement = nextImportedSystemLineEntry;
		++nextImportedSystemLineEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedSystemLines.erase(currentElement);
		}
	}

	//Remove any bus interface import entries which belong to this module
	ImportedBusInterfaceList::iterator nextImportedBusInterfaceEntry = importedBusInterfaces.begin();
	while(nextImportedBusInterfaceEntry != importedBusInterfaces.end())
	{
		ImportedBusInterfaceList::iterator currentElement = nextImportedBusInterfaceEntry;
		++nextImportedBusInterfaceEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedBusInterfaces.erase(currentElement);
		}
	}

	//Remove any extension import entries which belong to this module
	ImportedExtensionInfoList::iterator nextImportedExtensionEntry = importedExtensionInfoList.begin();
	while(nextImportedExtensionEntry != importedExtensionInfoList.end())
	{
		ImportedExtensionInfoList::iterator currentElement = nextImportedExtensionEntry;
		++nextImportedExtensionEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedExtensionInfoList.erase(currentElement);
		}
	}

	//Remove any device import entries which belong to the module
	ImportedDeviceInfoList::iterator nextImportedDeviceEntry = importedDeviceInfoList.begin();
	while(nextImportedDeviceEntry != importedDeviceInfoList.end())
	{
		ImportedDeviceInfoList::iterator currentElement = nextImportedDeviceEntry;
		++nextImportedDeviceEntry;
		if(currentElement->importingModuleID == moduleID)
		{
			importedDeviceInfoList.erase(currentElement);
		}
	}

	//Remove any system lines defined by this module
	SystemLineMap::iterator nextSystemLineEntry = systemLines.begin();
	while(nextSystemLineEntry != systemLines.end())
	{
		SystemLineMap::iterator currentElement = nextSystemLineEntry;
		++nextSystemLineEntry;
		if(currentElement->second.moduleID == moduleID)
		{
			systemLines.erase(currentElement);
		}
	}

	//Remove any system settings defined by this module
	ModuleSystemSettingMap::iterator moduleSettingsEntry = moduleSettings.find(moduleID);
	if(moduleSettingsEntry != moduleSettings.end())
	{
		for(SystemSettingsIDList::const_iterator i = moduleSettingsEntry->second.begin(); i != moduleSettingsEntry->second.end(); ++i)
		{
			systemSettings.erase(*i);
		}
		moduleSettings.erase(moduleSettingsEntry);
	}

	//Remove any embedded ROM entries defined by this module
	EmbeddedROMList::iterator nextEmbeddedROMInfoEntry = embeddedROMInfo.begin();
	while(nextEmbeddedROMInfoEntry != embeddedROMInfo.end())
	{
		EmbeddedROMList::iterator currentElement = nextEmbeddedROMInfoEntry;
		++nextEmbeddedROMInfoEntry;
		if(currentElement->moduleID == moduleID)
		{
			embeddedROMInfo.erase(currentElement);
		}
	}

	//Remove this module from the list of loaded modules
	LoadedModuleInfoList::iterator nextModuleEntry = loadedModuleInfoList.begin();
	while(nextModuleEntry != loadedModuleInfoList.end())
	{
		LoadedModuleInfoList::iterator currentElement = nextModuleEntry;
		++nextModuleEntry;
		if(currentElement->moduleID == moduleID)
		{
			loadedModuleInfoList.erase(currentElement);
		}
	}

	//Assert the specified unmapped line state for any unmapped lines in the system that
	//have an unmapped line state specified
	for(UnmappedLineStateList::const_iterator i = unmappedLineStateList.begin(); i != unmappedLineStateList.end(); ++i)
	{
		//Retrieve information on the module that contains the target device
		unsigned int deviceModuleID = i->targetDevice->GetDeviceModuleID();
		bool foundDeviceModuleEntry = false;
		LoadedModuleInfoList::const_iterator deviceLoadedModuleIterator = loadedModuleInfoList.begin();
		while(!foundDeviceModuleEntry && (deviceLoadedModuleIterator != loadedModuleInfoList.end()))
		{
			if(deviceLoadedModuleIterator->moduleID == deviceModuleID)
			{
				foundDeviceModuleEntry = true;
				continue;
			}
			++deviceLoadedModuleIterator;
		}
		if(!foundDeviceModuleEntry)
		{
			continue;
		}
		const LoadedModuleInfoInternal& deviceModuleInfo = *deviceLoadedModuleIterator;

		//If the module which contains this device has not been validated, skip any
		//further processing for this device.
		if(!deviceModuleInfo.moduleValidated)
		{
			continue;
		}

		//Check if at least one mapping exists to the target line on the device with the
		//unmapped line state setting
		bool foundMappingToLine = false;
		BusInterfaceList::const_iterator busInterfaceIterator = busInterfaces.begin();
		while(!foundMappingToLine && (busInterfaceIterator != busInterfaces.end()))
		{
			foundMappingToLine |= busInterfaceIterator->busInterface->IsDeviceLineMappedTo(i->targetDevice, i->lineNo);
			++busInterfaceIterator;
		}

		//If no mapping could be found to the target line, set the target line to the
		//specified unmapped line state.
		if(!foundMappingToLine)
		{
			i->targetDevice->TransparentSetLineState(i->lineNo, i->lineData);
		}
	}

	//Update the name stack of the currently unloading module
	PopUnloadModuleCurrentModuleName();
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeModuleID() const
{
	bool moduleIDFree;
	unsigned int moduleID;
	do
	{
		moduleID = nextFreeModuleID++;
		moduleIDFree = true;
		for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
		{
			if(i->moduleID == moduleID)
			{
				moduleIDFree = false;
			}
		}
	} while (!moduleIDFree);
	return moduleID;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeConnectorID() const
{
	bool connectorIDFree;
	unsigned int connectorID;
	do
	{
		connectorID = nextFreeConnectorID++;
		connectorIDFree = true;
		connectorIDFree = (connectorDetailsMap.find(connectorID) == connectorDetailsMap.end());
	} while (!connectorIDFree);
	return connectorID;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeLineGroupID() const
{
	bool lineGroupIDFree;
	unsigned int lineGroupID;
	do
	{
		lineGroupID = nextFreeLineGroupID++;
		lineGroupIDFree = true;
		lineGroupIDFree = (connectorDetailsMap.find(lineGroupID) == connectorDetailsMap.end());
	} while (!lineGroupIDFree);
	return lineGroupID;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeSystemLineID() const
{
	bool systemLineIDFree;
	unsigned int systemLineID;
	do
	{
		systemLineID = nextFreeSystemLineID++;
		systemLineIDFree = true;
		systemLineIDFree = (systemLines.find(systemLineID) == systemLines.end());
	} while (!systemLineIDFree);
	return systemLineID;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeSystemSettingID() const
{
	bool systemSettingIDFree;
	unsigned int systemSettingID;
	do
	{
		systemSettingID = nextFreeSystemSettingID++;
		systemSettingIDFree = true;
		systemSettingIDFree = (systemSettings.find(systemSettingID) == systemSettings.end());
	} while (!systemSettingIDFree);
	return systemSettingID;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device class and instance names
	IHeirarchicalStorageAttribute* deviceNameAttribute = node.GetAttribute(L"DeviceName");
	IHeirarchicalStorageAttribute* instanceNameAttribute = node.GetAttribute(L"InstanceName");
	if((deviceNameAttribute == 0) || (instanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceName or InstanceName attribute for Device entry!"));
		return false;
	}
	std::wstring deviceName = deviceNameAttribute->GetValue();
	std::wstring instanceName = instanceNameAttribute->GetValue();

	//Create the new device object
	IDevice* device = CreateDevice(deviceName, instanceName, moduleID);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"CreateDevice failed for " + instanceName + L"!"));
		return false;
	}

	//Create and bind the device context
	DeviceContext* deviceContext = new DeviceContext(device);
	if(!device->BindToDeviceContext(deviceContext))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindToDeviceContext failed for  " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Provide an interface into the system to the device
	deviceContext->SetSystemInterface(this);

	//Construct the device object
	if(!device->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Construct failed for " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Call BuildDevice() to perform any other required post-creation initialzation for the
	//device.
	if(!device->BuildDevice())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BuildDevice failed for " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Add the device object to the system
	if(!AddDevice(moduleID, device, deviceContext))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"AddDevice failed for " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_SetDependentDevice(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device names
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	if((deviceInstanceNameAttribute == 0) || (targetInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName or TargetInstanceName attribute for Device.SetDependentDevice!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring targetName = targetInstanceNameAttribute->GetValue();

	//Retrieve the specified devices from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	IDevice* target = GetDevice(moduleID, targetName);
	if((device == 0) || (target == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate source device with name " + deviceName + L" or target device with name " + targetName + L" for Device.SetDependentDevice!"));
		return false;
	}

	//Set the target device as a dependent device
	DeviceContext* deviceContext = (DeviceContext*)device->GetDeviceContext();
	deviceContext->AddDeviceDependency((DeviceContext*)target->GetDeviceContext());

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceDevice(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device names, and reference name.
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (targetInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName, TargetInstanceName, or ReferenceName attribute for Device.ReferenceDevice!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring targetName = targetInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified devices from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	IDevice* target = GetDevice(moduleID, targetName);
	if((device == 0) || (target == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate source device with name " + deviceName + L" or target device with name " + targetName + L" for Device.ReferenceDevice!"));
		return false;
	}

	//Add the specified device reference to the device
	if(!device->AddReference(referenceName, target))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceDevice failed for reference from " + deviceName + L" to " + targetName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceExtension(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the GlobalExtensionName or ExtensionInstanceName attributes
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* globalExtensionNameAttribute = node.GetAttribute(L"GlobalExtensionName");
	if((extensionInstanceNameAttribute == 0) && (globalExtensionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Either the ExtensionInstanceName or the GlobalExtensionName attribute must be specified for Device.ReferenceExtension!"));
		return false;
	}
	else if((extensionInstanceNameAttribute != 0) && (globalExtensionNameAttribute != 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The ExtensionInstanceName and GlobalExtensionName attributes cannot be used together for Device.ReferenceExtension!"));
		return false;
	}

	//Determine if a global extension is being referenced, and retrieve the name of the
	//extension.
	bool globalExtension = false;
	std::wstring extensionName;
	if(globalExtensionNameAttribute != 0)
	{
		globalExtension = true;
		extensionName = globalExtensionNameAttribute->GetValue();
	}
	else
	{
		extensionName = extensionInstanceNameAttribute->GetValue();
	}

	//Retrieve the referenced extension
	IExtension* extension = 0;
	if(globalExtension)
	{
		extension = GetGlobalExtension(extensionName);
		if(extension == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
			logEntry << L"Could not locate global extension with name " << extensionName << L" for Device.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}
	else
	{
		extension = GetExtension(moduleID, extensionName);
		if(extension == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
			logEntry << L"Could not locate extension with name " << extensionName << L" in module " << moduleID << L" for Device.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}

	//Load the device name and reference name.
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName or ReferenceName attribute for Device.ReferenceExtension!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceExtension!"));
		return false;
	}

	//Add the specified extension reference to the device
	if(!device->AddReference(referenceName, extension))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceExtension failed for reference from " + deviceName + L" to extension " + extensionName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceBus(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for Device.ReferenceBus!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for Device.ReferenceBus!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name and reference name.
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName or ReferenceName attribute for Device.ReferenceBus!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceBus!"));
		return false;
	}

	//Add the specified bus reference to the device
	if(!device->AddReference(referenceName, busInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceBus failed for reference from " + deviceName + L" to bus " + busInterfaceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name, device name, and reference name.
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName, ClockSourceName, or ReferenceName attribute for Device.ReferenceClockSource!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceClockSource!"));
		return false;
	}

	//Retrieve the specified clock source from the system
	IClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate clock source with name " + clockSourceName + L" for Device.ReferenceClockSource!"));
		return false;
	}

	//Add the specified clock source reference to the device
	if(!device->AddReference(referenceName, clockSource))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceClockSource failed for reference from " + deviceName + L" to " + clockSourceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_RegisterInput(IHeirarchicalStorageNode& node, unsigned int moduleID, std::list<InputRegistration>& inputRegistrationRequests)
{
	//Load the device name, system key code, and target key code.
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* deviceKeyCodeAttribute = node.GetAttribute(L"DeviceKeyCode");
	if((deviceInstanceNameAttribute == 0) || (deviceKeyCodeAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName or DeviceKeyCode attribute for Device.RegisterInput!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring deviceKeyCodeString = deviceKeyCodeAttribute->GetValue();

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name " + deviceName + L" for Device.RegisterInput!"));
		return false;
	}

	//Translate the device key code
	unsigned int deviceKeyCode = device->GetKeyCodeID(deviceKeyCodeString);
	if(deviceKeyCode == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device key code with name " + deviceKeyCodeString + L" on device " + deviceName + L" for Device.RegisterInput!"));
		return false;
	}

	//Retrieve the preferred system key code attribute if one has been specified
	bool preferredSystemKeyCodeSpecified = false;
	IDeviceContext::KeyCode systemKeyCode = IDeviceContext::KEYCODE_NONE;
	IHeirarchicalStorageAttribute* systemKeyCodeAttribute = node.GetAttribute(L"PreferredSystemKeyCode");
	if(systemKeyCodeAttribute != 0)
	{
		//Extract the system key code string
		std::wstring systemKeyCodeString = systemKeyCodeAttribute->GetValue();

		//Translate the system key code
		systemKeyCode = GetKeyCodeID(systemKeyCodeString);
		if(systemKeyCode == IDeviceContext::KEYCODE_NONE)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate system key code with name " + systemKeyCodeString + L" for Device.RegisterInput!"));
			return false;
		}

		//Flag that a preferred system key code has been specified
		preferredSystemKeyCodeSpecified = true;
	}

	//Add this input registration request to the list of input registration requests
	InputRegistration inputRegistrationRequest;
	inputRegistrationRequest.moduleID = moduleID;
	inputRegistrationRequest.targetDevice = device;
	inputRegistrationRequest.deviceKeyCode = deviceKeyCode;
	inputRegistrationRequest.preferredSystemKeyCodeSpecified = preferredSystemKeyCodeSpecified;
	inputRegistrationRequest.systemKeyCode = systemKeyCode;
	inputRegistrationRequests.push_back(inputRegistrationRequest);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_GlobalExtension(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the extension class name
	IHeirarchicalStorageAttribute* extensionNameAttribute = node.GetAttribute(L"ExtensionName");
	if(extensionNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionName attribute for GlobalExtension entry!"));
		return false;
	}
	std::wstring extensionName = extensionNameAttribute->GetValue();

	//Attempt to retrieve an existing instance of this global extension
	IExtension* extension = 0;
	extension = GetGlobalExtension(extensionName);

	//If an existing instance of this global extension was not found, create it.
	if(extension == 0)
	{
		//Create the new extension object
		extension = CreateGlobalExtension(extensionName);
		if(extension == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"CreateGlobalExtension failed for " + extensionName + L"!"));
			return false;
		}

		//Bind to the system interface
		if(!extension->BindToSystemInterface(systemExtensionInterface))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindToSystemInterface failed for  " + extensionName + L"!"));
			DestroyExtension(extensionName, extension);
			return false;
		}

		//Bind to the GUI interface
		if(!extension->BindToGUIInterface(guiExtensionInterface))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindToGUIInterface failed for  " + extensionName + L"!"));
			DestroyExtension(extensionName, extension);
			return false;
		}

		//Construct the extension object
		if(!extension->Construct(node))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Construct failed for " + extensionName + L"!"));
			DestroyExtension(extensionName, extension);
			return false;
		}

		//Call BuildExtension() to perform any other required post-creation initialzation
		//for the extension.
		if(!extension->BuildExtension())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BuildExtension failed for " + extensionName + L"!"));
			DestroyExtension(extensionName, extension);
			return false;
		}
	}

	//Add a reference to this global extension object from this module
	if(!AddGlobalExtension(moduleID, extension))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"AddGlobalExtension failed for " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the extension class and instance names
	IHeirarchicalStorageAttribute* extensionNameAttribute = node.GetAttribute(L"ExtensionName");
	IHeirarchicalStorageAttribute* instanceNameAttribute = node.GetAttribute(L"InstanceName");
	if((extensionNameAttribute == 0) || (instanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionName or InstanceName attribute for Extension entry!"));
		return false;
	}
	std::wstring extensionName = extensionNameAttribute->GetValue();
	std::wstring instanceName = instanceNameAttribute->GetValue();

	//Create the new extension object
	IExtension* extension = CreateExtension(extensionName, instanceName, moduleID);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"CreateExtension failed for " + instanceName + L"!"));
		return false;
	}

	//Bind to the system interface
	if(!extension->BindToSystemInterface(systemExtensionInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindToSystemInterface failed for  " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Bind to the GUI interface
	if(!extension->BindToGUIInterface(guiExtensionInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindToGUIInterface failed for  " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Construct the extension object
	if(!extension->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Construct failed for " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Call BuildExtension() to perform any other required post-creation initialzation for
	//the extension.
	if(!extension->BuildExtension())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BuildExtension failed for " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Add the extension object to the system
	if(!AddExtension(moduleID, extension))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"AddExtension failed for " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceDevice(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the ExtensionInstanceName attribute
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for Extension.ReferenceDevice!"));
		return false;
	}
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for Extension.ReferenceDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name and reference name.
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceDevice!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension object from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceDevice!"));
		return false;
	}

	//Add the specified device reference to the extension
	if(!extension->AddReference(referenceName, device))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Extension.ReferenceDevice failed for reference from " + extensionName + L" to device " + deviceInstanceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceExtension(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the GlobalExtensionName or ExtensionInstanceName attributes
	IHeirarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	IHeirarchicalStorageAttribute* targetGlobalExtensionNameAttribute = node.GetAttribute(L"TargetGlobalExtensionName");
	if((targetInstanceNameAttribute == 0) && (targetGlobalExtensionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Either the TargetInstanceName or the TargetGlobalExtensionName attribute must be specified for Extension.ReferenceExtension!"));
		return false;
	}
	else if((targetInstanceNameAttribute != 0) && (targetGlobalExtensionNameAttribute != 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The TargetInstanceName and TargetGlobalExtensionName attributes cannot be used together for Extension.ReferenceExtension!"));
		return false;
	}

	//Determine if a global extension is being referenced, and retrieve the name of the
	//extension.
	bool globalExtensionTarget = false;
	std::wstring targetExtensionName;
	if(targetGlobalExtensionNameAttribute != 0)
	{
		globalExtensionTarget = true;
		targetExtensionName = targetGlobalExtensionNameAttribute->GetValue();
	}
	else
	{
		targetExtensionName = targetInstanceNameAttribute->GetValue();
	}

	//Retrieve the referenced extension
	IExtension* target = 0;
	if(globalExtensionTarget)
	{
		target = GetGlobalExtension(targetExtensionName);
		if(target == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
			logEntry << L"Could not locate target global extension with name " << targetExtensionName << L" for Extension.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}
	else
	{
		target = GetExtension(moduleID, targetExtensionName);
		if(target == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
			logEntry << L"Could not locate target extension with name " << targetExtensionName << L" in module " << moduleID << L" for Extension.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}

	//Load the ExtensionInstanceName and ReferenceName attributes
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceExtension!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extensions from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate source extension with name " + extensionName + L" for Extension.ReferenceExtension!"));
		return false;
	}

	//Add the specified extension reference to the extension
	if(!extension->AddReference(referenceName, target))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Extension.ReferenceExtension failed for reference from " + extensionName + L" to " + targetExtensionName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceBus(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for Extension.ReferenceBus!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for Extension.ReferenceBus!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the extension name and reference name.
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceBus!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension object from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceBus!"));
		return false;
	}

	//Add the specified bus reference to the device
	if(!extension->AddReference(referenceName, busInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Extension.ReferenceBus failed for reference from " + extensionName + L" to bus " + busInterfaceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name, device name, and reference name.
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHeirarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ExtensionInstanceName, ClockSourceName, or ReferenceName attribute for Extension.ReferenceClockSource!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceClockSource!"));
		return false;
	}

	//Retrieve the specified clock source from the system
	IClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate clock source with name " + clockSourceName + L" for Extension.ReferenceClockSource!"));
		return false;
	}

	//Add the specified clock source reference to the device
	if(!extension->AddReference(referenceName, clockSource))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Extension.ReferenceClockSource failed for reference from " + extensionName + L" to " + clockSourceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the bus name
	IHeirarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Name attribute for BusInterface!"));
		return false;
	}
	std::wstring busInterfaceName = nameAttribute->GetValue();

	//Ensure a bus object with the specified name doesn't already exist in the system
	for(BusInterfaceList::const_iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		const LoadedBusInfo& busInfo = *i;
		if((busInfo.moduleID == moduleID) && (busInfo.name == busInterfaceName))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface with name of " + busInterfaceName + L" already exists!"));
			return false;
		}
	}

	//Create and construct a new bus interface object
	BusInterface* busInterface = new BusInterface();
	if(!busInterface->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Construct failed for BusInterface " + busInterfaceName + L"!"));
		delete busInterface;
		return false;
	}

	//Add the bus interface object to the system
	LoadedBusInfo busInfo;
	busInfo.busInterface = busInterface;
	busInfo.moduleID = moduleID;
	busInfo.name = busInterfaceName;
	busInterfaces.push_back(busInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_DefineLineGroup(IHeirarchicalStorageNode& node, unsigned int moduleID, NameToIDMap& lineGroupNameToIDMap)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineLineGroup!"));
		return false;
	}

	//Extract the LineGroupName attribute
	IHeirarchicalStorageAttribute* lineGroupNameAttribute = node.GetAttribute(L"LineGroupName");
	if(lineGroupNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing LineGroupName attribute for BusInterface.DefineLineGroup!"));
		return false;
	}
	std::wstring lineGroupName = lineGroupNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.DefineLineGroup!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Add details of this line group to the list of line groups which have been defined
	LineGroupDetails lineGroupDetails;
	lineGroupDetails.lineGroupID = GenerateFreeLineGroupID();
	lineGroupDetails.busInterface = busInterface;
	lineGroupDetails.lineGroupName = lineGroupName;
	lineGroupDetailsMap.insert(LineGroupDetailsMapEntry(lineGroupDetails.lineGroupID, lineGroupDetails));

	//Add a mapping for this line group name back to the line group ID
	lineGroupNameToIDMap.insert(NameToIDMapEntry(lineGroupName, lineGroupDetails.lineGroupID));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_DefineCELineMemory(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineCELineMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.DefineCELineMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->DefineCELineMemory(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.DefineCELineMemory failed on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_DefineCELinePort(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineCELinePort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.DefineCELinePort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->DefineCELinePort(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.DefineCELinePort failed on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineInputMemory(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineInputMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineInputMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineInputMemory!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineInputMemory!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineInputMemory(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapCELineInputMemory failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineInputPort(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineInputPort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineInputPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineInputPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineInputPort!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineInputPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapCELineInputPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineOutputMemory(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineOutputMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineOutputMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineOutputMemory!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineOutputMemory!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineOutputMemory(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapCELineOutputMemory failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineOutputPort(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	std::wstring busInterfaceName;
	unsigned int busInterfaceModuleID;
	if((busInterfaceNameAttribute != 0))
	{
		//Save the target bus interface name and module ID
		busInterfaceName = busInterfaceNameAttribute->GetValue();
		busInterfaceModuleID = moduleID;
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineOutputPort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineOutputPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineOutputPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineOutputPort!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineOutputPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapCELineOutputPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapDevice(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName for BusInterface.MapDevice!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapDevice!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapDevice!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapDevice(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapDevice failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapPort(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapPort!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapPort!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapLine(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& lineGroupNameToIDMap)
{
	//Attempt to extract all possible attributes defining the source and target of the
	//line mapping.
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHeirarchicalStorageAttribute* sourceDeviceNameAttribute = node.GetAttribute(L"SourceDeviceInstanceName");
	IHeirarchicalStorageAttribute* sourceLineGroupNameAttribute = node.GetAttribute(L"SourceLineGroupName");
	IHeirarchicalStorageAttribute* targetDeviceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHeirarchicalStorageAttribute* targetLineGroupNameAttribute = node.GetAttribute(L"TargetLineGroupName");
	if((sourceDeviceNameAttribute != 0) && (targetDeviceNameAttribute != 0) && (busInterfaceNameAttribute != 0))
	{
		//Load the bus, source device, and target device names.
		std::wstring sourceDeviceName = sourceDeviceNameAttribute->GetValue();
		std::wstring targetDeviceName = targetDeviceNameAttribute->GetValue();
		std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

		//Retrieve the bus, source device, and target device objects from the system.
		BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
		IDevice* sourceDevice = GetDevice(moduleID, sourceDeviceName);
		IDevice* targetDevice = GetDevice(moduleID, targetDeviceName);
		if((busInterface == 0) || (sourceDevice == 0) || (targetDevice == 0))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate source device with name \"" + sourceDeviceName + L"\" or target device with name \"" + targetDeviceName + L"\" or bus interface with name \"" + busInterfaceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Add the bus interface mapping
		if(!busInterface->MapLine(sourceDevice, targetDevice, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapLine failed for mapping from \"" + sourceDeviceName + L"\" to \"" + targetDeviceName + L"\" on bus \"" + busInterfaceName + L"\"!"));
			return false;
		}
	}
	else if((sourceDeviceNameAttribute != 0) && (targetLineGroupNameAttribute != 0))
	{
		//Load the source device and target line group names
		std::wstring sourceDeviceName = sourceDeviceNameAttribute->GetValue();
		std::wstring targetLineGroupName = targetLineGroupNameAttribute->GetValue();

		//Retrieve the source device
		IDevice* sourceDevice = GetDevice(moduleID, sourceDeviceName);
		if(sourceDevice == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate source device with name \"" + sourceDeviceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Find the ID for the referenced line group
		NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(targetLineGroupName);
		if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate line group with name \"" + targetLineGroupName + L"\" for BusInterface.MapLine!"));
			return false;
		}
		unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

		//Retrieve the target line group info
		LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
		if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not retrieve line group details for line group with name \"" + targetLineGroupName + L"\" in BusInterface.MapLine!"));
			return false;
		}
		const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

		//Retrieve the referenced bus interface
		BusInterface* busInterface = lineGroupDetails.busInterface;

		//Add the bus interface mapping
		if(!busInterface->MapLine(sourceDevice, lineGroupID, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapLine failed for mapping from device \"" + sourceDeviceName + L"\" to line group \"" + targetLineGroupName + L"\"!"));
			return false;
		}
	}
	else if((sourceLineGroupNameAttribute != 0) && (targetDeviceNameAttribute != 0))
	{
		//Load the source line group and target device names
		std::wstring sourceLineGroupName = sourceLineGroupNameAttribute->GetValue();
		std::wstring targetDeviceName = targetDeviceNameAttribute->GetValue();

		//Retrieve the target device
		IDevice* targetDevice = GetDevice(moduleID, targetDeviceName);
		if(targetDevice == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name \"" + targetDeviceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Find the ID for the referenced line group
		NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(sourceLineGroupName);
		if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate line group with name \"" + sourceLineGroupName + L"\" for BusInterface.MapLine!"));
			return false;
		}
		unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

		//Retrieve the target line group info
		LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
		if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not retrieve line group details for line group with name \"" + sourceLineGroupName + L"\" in BusInterface.MapLine!"));
			return false;
		}
		const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

		//Retrieve the referenced bus interface
		BusInterface* busInterface = lineGroupDetails.busInterface;

		//Add the bus interface mapping
		if(!busInterface->MapLine(lineGroupID, targetDevice, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapLine failed for mapping from line group \"" + sourceLineGroupName + L"\" to device \"" + targetDeviceName + L"\"!"));
			return false;
		}
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No valid combination of source and target mappings exists for BusInterface.MapLine!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Extract the ClockSourceName attribute
	IHeirarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	if(clockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ClockSourceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();

	//Retrieve the referenced clock source
	ClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate clock source with name " << clockSourceName << L" in module " << moduleID << L" for BusInterface.MapClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapClockSource!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapClockSource(clockSource, device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BusInterface.MapClockSource failed when mapping clock source with name " + clockSourceName + L" to device with name " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	//Add a reference to the target bus interface to the clock source
	clockSource->AddReference(busInterface);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_UnmappedLineState(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract all required attributes
	IHeirarchicalStorageAttribute* deviceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* targetLineAttribute = node.GetAttribute(L"TargetLine");
	IHeirarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((deviceNameAttribute == 0) || (targetLineAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName, TargetLine, or Value attribute for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Load the device name and target line name attributes
	std::wstring deviceName = deviceNameAttribute->GetValue();
	std::wstring targetLineName = targetLineAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name \"" + deviceName + L"\" for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Obtain the line ID for the target line on the device
	unsigned int targetLineID = device->GetLineID(targetLineName);
	if(targetLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate line with name \"" + targetLineName + L"\" on device with name \"" + deviceName + L"\" for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Obtain the bit count for the target line
	unsigned int lineBitCount = device->GetLineWidth(targetLineID);
	if(lineBitCount == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Target line with name \"" + targetLineName + L"\" on device with name \"" + deviceName + L"\" returned a bit count of 0 for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Extract the unmapped line value for the target line
	unsigned int unmappedValue = valueAttribute->ExtractHexValue<unsigned int>();

	//Add the UnmappedLineStateInfo object to the system
	UnmappedLineStateInfo unmappedLineInfo(moduleID, device, targetLineID, Data(lineBitCount, unmappedValue));
	unmappedLineStateList.push_back(unmappedLineInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_ClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name
	IHeirarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Name attribute for ClockSource!"));
		return false;
	}
	std::wstring clockSourceName = nameAttribute->GetValue();

	//Ensure a clock source object with the specified name doesn't already exist in the
	//system
	for(ClockSourceList::const_iterator i = clockSources.begin(); i != clockSources.end(); ++i)
	{
		const LoadedClockSourceInfo& clockSourceInfo = *i;
		if((clockSourceInfo.moduleID == moduleID) && (clockSourceInfo.name == clockSourceName))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"ClockSource with name of " + clockSourceName + L" already exists!"));
			return false;
		}
	}

	//Create and construct a new clock source object
	ClockSource* clockSource = new ClockSource();
	if(!clockSource->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Construct failed for ClockSource " + clockSourceName + L"!"));
		delete clockSource;
		return false;
	}

	//Add the clock source object to the system
	LoadedClockSourceInfo clockSourceInfo;
	clockSourceInfo.clockSource = clockSource;
	clockSourceInfo.moduleID = moduleID;
	clockSourceInfo.name = clockSourceName;
	clockSources.push_back(clockSourceInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_ClockSource_SetInputClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the source clock source name
	IHeirarchicalStorageAttribute* inputClockSourceNameAttribute = node.GetAttribute(L"InputClockSourceName");
	if(inputClockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing InputClockSourceName attribute for ClockSource.SetInputClockSource!"));
		return false;
	}
	std::wstring inputClockSourceName = inputClockSourceNameAttribute->GetValue();

	//Load the target clock source name
	IHeirarchicalStorageAttribute* targetClockSourceNameAttribute = node.GetAttribute(L"TargetClockSourceName");
	if(targetClockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing TargetClockSourceName attribute for ClockSource.SetInputClockSource!"));
		return false;
	}
	std::wstring targetClockSourceName = targetClockSourceNameAttribute->GetValue();

	//Retrieve the referenced input clock source object
	ClockSource* inputClockSource = GetClockSource(moduleID, inputClockSourceName);
	if(inputClockSource == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate clock source with name " << inputClockSourceName << L" in module " << moduleID << L" for ClockSource.SetInputClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the referenced target clock source object
	ClockSource* targetClockSource = GetClockSource(moduleID, targetClockSourceName);
	if(targetClockSource == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate clock source with name " << targetClockSourceName << L" in module " << moduleID << L" for ClockSource.SetInputClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Attempt to add the target clock source as a dependent clock source of the input
	return inputClockSource->AddDependentClockSource(targetClockSource);
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_OpenViewModel(IHeirarchicalStorageNode& node, unsigned int moduleID, std::list<ViewModelOpenRequest>& viewModelOpenRequests)
{
	//Extract the Owner, MenuHandlerName, and ViewID attributes
	IHeirarchicalStorageAttribute* ownerAttribute = node.GetAttribute(L"Owner");
	IHeirarchicalStorageAttribute* viewModelGroupAttribute = node.GetAttribute(L"ViewModelGroupName");
	IHeirarchicalStorageAttribute* viewModelNameAttribute = node.GetAttribute(L"ViewModelName");
	if((ownerAttribute == 0) || (viewModelGroupAttribute == 0) || (viewModelNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Owner, ViewModelGroupName, or ViewModelName attribute for System.OpenViewModel!"));
		return false;
	}
	std::wstring owner = ownerAttribute->GetValue();
	std::wstring viewModelGroupName = viewModelGroupAttribute->GetValue();
	std::wstring viewModelName = viewModelNameAttribute->GetValue();

	if(owner == L"System")
	{
		//Add the open request to the queue
		ViewModelOpenRequest openRequest;
		openRequest.ownerSystem = true;
		openRequest.viewModelGroupName = viewModelGroupName;
		openRequest.viewModelName = viewModelName;
		viewModelOpenRequests.push_back(openRequest);
	}
	else if(owner == L"Device")
	{
		//Extract the device instance name attribute
		IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
		if(deviceInstanceNameAttribute == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing DeviceInstanceName attribute for System.OpenViewModel!"));
			return false;
		}
		std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();

		//Add the open request to the queue
		ViewModelOpenRequest openRequest;
		openRequest.ownerSystem = false;
		openRequest.moduleID = moduleID;
		openRequest.viewModelGroupName = viewModelGroupName;
		openRequest.viewModelName = viewModelName;
		openRequest.deviceInstanceName = deviceInstanceName;
		viewModelOpenRequests.push_back(openRequest);
	}
	else
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportConnector(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, NameToIDMap& connectorNameToIDMap)
{
	//Extract the BusInterfaceName, ConnectorClassName, and ConnectorInstanceName attributes
	IHeirarchicalStorageAttribute* connectorClassNameAttribute = node.GetAttribute(L"ConnectorClassName");
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	if((connectorClassNameAttribute == 0) || (connectorInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorClassName or ConnectorInstanceName attribute for System.ExportConnector!"));
		return false;
	}
	std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();

	//Add details of this connector to the list of connectors which have been defined
	ConnectorInfoInternal connectorDetails;
	connectorDetails.connectorID = GenerateFreeConnectorID();
	connectorDetails.connectorClassName = connectorClassName;
	connectorDetails.exportingModuleConnectorInstanceName = connectorInstanceName;
	connectorDetails.systemClassName = systemClassName;
	connectorDetails.connectorUsed = false;
	connectorDetails.exportingModuleID = moduleID;
	connectorDetailsMap.insert(ConnectorDetailsMapEntry(connectorDetails.connectorID, connectorDetails));
	connectorNameToIDMap.insert(NameToIDMapEntry(connectorInstanceName, connectorDetails.connectorID));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportDevice(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, DeviceInstanceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (deviceInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ExportDevice!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for System.ExportDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportDevice!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportDevice!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedDeviceInfo exportedDeviceInfo;
	exportedDeviceInfo.device = device;
	//##TODO## Find a way to remove this cast. It may be safe, but we should be able to
	//avoid it.
	exportedDeviceInfo.deviceContext = (DeviceContext*)device->GetDeviceContext();
	exportedDeviceInfo.exportingModuleDeviceInstanceName = deviceInstanceName;
	exportedDeviceInfo.importName = importName;
	connectorDetails.exportedDeviceInfo.insert(std::pair<std::wstring, ExportedDeviceInfo>(exportedDeviceInfo.importName, exportedDeviceInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportExtension(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ExtensionInstanceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (extensionInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, ExtensionInstanceName, or ImportName attribute for System.ExportExtension!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced extension
	IExtension* extension = GetExtension(moduleID, extensionInstanceName);
	if(extension == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate extension with name " << extensionInstanceName << L" in module " << moduleID << L" for System.ExportExtension!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportExtension!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportExtension!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedExtensionInfo exportedExtensionInfo;
	exportedExtensionInfo.extension = extension;
	exportedExtensionInfo.exportingModuleExtensionInstanceName = extensionInstanceName;
	exportedExtensionInfo.importName = importName;
	connectorDetails.exportedExtensionInfo.insert(std::pair<std::wstring, ExportedExtensionInfo>(exportedExtensionInfo.importName, exportedExtensionInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, const NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ExportBusInterface!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate bus interface with name \"" << busInterfaceName << L"\" in module " << moduleID << L" for System.ExportBusInterface!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name \"" + connectorInstanceName + L"\" for System.ExportBusInterface!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name \"" + connectorInstanceName + L"\" for System.ExportBusInterface!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Build the info about this exported bus interface
	ExportedBusInfo exportedBusInfo;
	exportedBusInfo.busInterface = busInterface;
	exportedBusInfo.exportingModuleBusInterfaceName = busInterfaceName;
	exportedBusInfo.importName = importName;

	//Process any child elements of this node
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHeirarchicalStorageNode& childNode = *(*i);
		std::wstring elementName = childNode.GetName();
		if(elementName == L"ExportLineGroup")
		{
			//Extract the LineGroupName and ImportName attributes
			IHeirarchicalStorageAttribute* childLineGroupNameAttribute = childNode.GetAttribute(L"LineGroupName");
			IHeirarchicalStorageAttribute* childImportNameAttribute = childNode.GetAttribute(L"ImportName");
			if((childLineGroupNameAttribute == 0) || (childImportNameAttribute == 0))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing LineGroupName or ImportName attribute for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			std::wstring lineGroupName = childLineGroupNameAttribute->GetValue();
			std::wstring importName = childImportNameAttribute->GetValue();

			//Find the ID for the referenced line group
			NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(lineGroupName);
			if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate line group with name \"" + lineGroupName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

			//Retrieve the target line group info
			LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
			if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not retrieve line group details for line group with name \"" + lineGroupName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

			//Verify that the specified line group belongs to the BusInterface object
			//being exported.
			if(lineGroupDetails.busInterface != busInterface)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The specified line group with name \"" + lineGroupName + L"\" does not belong to the specified bus interface with name \"" + busInterfaceName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}

			//Build info about this exported line group, and add it to the info about this
			//exported bus interface.
			ExportedLineGroupInfo exportedLineGroupInfo;
			exportedLineGroupInfo.lineGroupID = lineGroupID;
			exportedLineGroupInfo.importName = importName;
			exportedLineGroupInfo.localName = lineGroupDetails.lineGroupName;
			exportedBusInfo.exportedLineGroups.push_back(exportedLineGroupInfo);
		}
		else
		{
			//If we've encountered an unrecognized child element, log an error, and return
			//false.
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Unrecognized child element: \"" + elementName + L"\" encountered when processing System.ExportBusInterface!"));
			return false;
		}
	}

	//Add details of this exported object to the connector details
	connectorDetails.exportedBusInfo.insert(std::pair<std::wstring, ExportedBusInfo>(exportedBusInfo.importName, exportedBusInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ClockSourceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ExportClockSource!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced clock source
	ClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate clock source with name " << clockSourceName << L" in module " << moduleID << L" for System.ExportClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportClockSource!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportClockSource!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedClockSourceInfo exportedClockSourceInfo;
	exportedClockSourceInfo.clockSource = clockSource;
	exportedClockSourceInfo.exportingModuleClockSourceName = clockSourceName;
	exportedClockSourceInfo.importName = importName;
	connectorDetails.exportedClockSourceInfo.insert(std::pair<std::wstring, ExportedClockSourceInfo>(exportedClockSourceInfo.importName, exportedClockSourceInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportSystemLine(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemLineName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemLineNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, SystemLineName, or ImportName attribute for System.ExportSystemLine!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the ID number for the referenced system line
	unsigned int systemLineID = GetSystemLineID(moduleID, systemLineName);
	if(systemLineID == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate system line with name " << systemLineName << L" in module " << moduleID << L" for System.ExportSystemLine!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportSystemLine!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportSystemLine!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedSystemLineInfo exportedSystemLineInfo;
	exportedSystemLineInfo.systemLineID = systemLineID;
	exportedSystemLineInfo.exportingModuleSystemLineName = systemLineName;
	exportedSystemLineInfo.importName = importName;
	connectorDetails.exportedSystemLineInfo.insert(std::pair<std::wstring, ExportedSystemLineInfo>(exportedSystemLineInfo.importName, exportedSystemLineInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportSystemSetting(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemSettingName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* systemSettingNameAttribute = node.GetAttribute(L"SystemSettingName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemSettingNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, SystemSettingName, or ImportName attribute for System.ExportSystemSetting!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemSettingName = systemSettingNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the ID number for the referenced system setting
	unsigned int systemSettingID = GetSystemSettingID(moduleID, systemSettingName);
	if(systemSettingID == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate system setting with name " << systemSettingName << L" in module " << moduleID << L" for System.ExportSystemSetting!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportSystemSetting!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportSystemSetting!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedSystemSettingInfo exportedSystemSettingInfo;
	exportedSystemSettingInfo.systemSettingID = systemSettingID;
	exportedSystemSettingInfo.exportingModuleSystemSettingName = systemSettingName;
	exportedSystemSettingInfo.importName = importName;
	connectorDetails.exportedSystemSettingInfo.insert(std::pair<std::wstring, ExportedSystemSettingInfo>(exportedSystemSettingInfo.importName, exportedSystemSettingInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportConnector(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, const ConnectorMappingList& connectorMappings, NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorClassName and ConnectorInstanceName attributes
	IHeirarchicalStorageAttribute* connectorClassNameAttribute = node.GetAttribute(L"ConnectorClassName");
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	if((connectorClassNameAttribute == 0) || (connectorInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorClassName or ConnectorInstanceName attribute for System.ImportConnector!"));
		return false;
	}
	std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();

	//Locate a corresponding connector mapping for this connector
	bool connectorImportSucceeded = false;
	bool foundConnectorMapping = false;
	ConnectorMappingList::const_iterator connectorMappingIterator = connectorMappings.begin();
	while(!foundConnectorMapping && (connectorMappingIterator != connectorMappings.end()))
	{
		//If we've found a matching connector mapping, process it.
		const ConnectorMapping& connectorMapping = *connectorMappingIterator;
		if(connectorMapping.importingModuleConnectorInstanceName == connectorInstanceName)
		{
			//Locate the corresponding connector that is being referenced
			foundConnectorMapping = true;
			ConnectorDetailsMap::iterator connectorDetailsMapIterator = connectorDetailsMap.find(connectorMapping.connectorID);
			if(connectorDetailsMapIterator != connectorDetailsMap.end())
			{
				//If the connector is not currently in use, import it.
				ConnectorInfoInternal& connectorDetails = connectorDetailsMapIterator->second;
				if((connectorDetails.systemClassName != systemClassName) || (connectorDetails.connectorClassName != connectorClassName))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"A connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the connector class name or system class name didn't match, while processing System.ImportConnector!"));
				}
				else if(connectorDetails.connectorUsed)
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"A connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the connector was already in use, while processing System.ImportConnector!"));
				}
				else
				{
					connectorDetails.connectorUsed = true;
					connectorDetails.importingModuleID = moduleID;
					connectorDetails.importingModuleConnectorInstanceName = connectorInstanceName;
					connectorNameToIDMap.insert(NameToIDMapEntry(connectorInstanceName, connectorDetails.connectorID));
					connectorImportSucceeded = true;
				}
			}
		}
		++connectorMappingIterator;
	}

	//Report any errors while importing the connector
	if(!foundConnectorMapping)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\" while processing System.ImportConnector!"));
	}
	if(foundConnectorMapping && !connectorImportSucceeded)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"A connector import mapping was found for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the target connector was already in use when processing System.ImportConnector!"));
	}

	//Return the result of the connector import operation
	return foundConnectorMapping && connectorImportSucceeded;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportDevice(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, DeviceInstanceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (deviceInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ImportDevice!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported device
	std::map<std::wstring, ExportedDeviceInfo>::const_iterator exportedDeviceInfoIterator = connectorDetails.exportedDeviceInfo.find(importName);
	if(exportedDeviceInfoIterator == connectorDetails.exportedDeviceInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported device with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
		return false;
	}
	const ExportedDeviceInfo& exportedDeviceInfo = exportedDeviceInfoIterator->second;

	//Record details of this imported object
	ImportedDeviceInfo importedDeviceInfo;
	importedDeviceInfo.device = exportedDeviceInfo.device;
	importedDeviceInfo.deviceContext = exportedDeviceInfo.deviceContext;
	importedDeviceInfo.exportingModuleDeviceInstanceName = exportedDeviceInfo.exportingModuleDeviceInstanceName;
	importedDeviceInfo.importName = exportedDeviceInfo.importName;
	importedDeviceInfo.importingModuleID = moduleID;
	importedDeviceInfo.importingModuleDeviceInstanceName = deviceInstanceName;
	importedDeviceInfo.connectorID = connectorID;
	importedDeviceInfoList.push_back(importedDeviceInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportExtension(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ExtensionInstanceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (extensionInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, ExtensionInstanceName, or ImportName attribute for System.ImportExtension!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported extension
	std::map<std::wstring, ExportedExtensionInfo>::const_iterator exportedExtensionInfoIterator = connectorDetails.exportedExtensionInfo.find(importName);
	if(exportedExtensionInfoIterator == connectorDetails.exportedExtensionInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported extension with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
		return false;
	}
	const ExportedExtensionInfo& exportedExtensionInfo = exportedExtensionInfoIterator->second;

	//Record details of this imported object
	ImportedExtensionInfo importedExtensionInfo;
	importedExtensionInfo.extension = exportedExtensionInfo.extension;
	importedExtensionInfo.exportingModuleExtensionInstanceName = exportedExtensionInfo.exportingModuleExtensionInstanceName;
	importedExtensionInfo.importName = exportedExtensionInfo.importName;
	importedExtensionInfo.importingModuleID = moduleID;
	importedExtensionInfo.importingModuleExtensionInstanceName = extensionInstanceName;
	importedExtensionInfo.connectorID = connectorID;
	importedExtensionInfoList.push_back(importedExtensionInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ImportBusInterface!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported bus interface
	std::map<std::wstring, ExportedBusInfo>::const_iterator exportedBusInfoIterator = connectorDetails.exportedBusInfo.find(importName);
	if(exportedBusInfoIterator == connectorDetails.exportedBusInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported bus interface with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
		return false;
	}
	const ExportedBusInfo& exportedBusInfo = exportedBusInfoIterator->second;

	//Build the info about this imported bus interface
	ImportedBusInfo importedBusInfo;
	importedBusInfo.busInterface = exportedBusInfo.busInterface;
	importedBusInfo.exportingModuleBusInterfaceName = exportedBusInfo.exportingModuleBusInterfaceName;
	importedBusInfo.importName = exportedBusInfo.importName;
	importedBusInfo.importingModuleID = moduleID;
	importedBusInfo.importingModuleBusInterfaceName = busInterfaceName;
	importedBusInfo.connectorID = connectorID;

	//Process any child elements of this node
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHeirarchicalStorageNode& childNode = *(*i);
		std::wstring elementName = childNode.GetName();
		if(elementName == L"ImportLineGroup")
		{
			//Extract the LineGroupName and ImportName attributes
			IHeirarchicalStorageAttribute* childLineGroupNameAttribute = childNode.GetAttribute(L"LineGroupName");
			IHeirarchicalStorageAttribute* childImportNameAttribute = childNode.GetAttribute(L"ImportName");
			if((childLineGroupNameAttribute == 0) || (childImportNameAttribute == 0))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing LineGroupName or ImportName attribute for ImportLineGroup on System.ImportBusInterface!"));
				return false;
			}
			std::wstring lineGroupName = childLineGroupNameAttribute->GetValue();
			std::wstring importName = childImportNameAttribute->GetValue();

			//Find the definition of the referenced exported line group
			bool foundReferencedLineGroup = false;
			unsigned int lineGroupID = 0;
			std::list<ExportedLineGroupInfo>::const_iterator exportedLineGroupIterator = exportedBusInfo.exportedLineGroups.begin();
			while(!foundReferencedLineGroup && (exportedLineGroupIterator != exportedBusInfo.exportedLineGroups.end()))
			{
				const ExportedLineGroupInfo& exportedLineGroupInfo = *exportedLineGroupIterator;
				if(exportedLineGroupInfo.importName == importName)
				{
					foundReferencedLineGroup = true;
					lineGroupID = exportedLineGroupInfo.lineGroupID;
					continue;
				}
				++exportedLineGroupIterator;
			}

			//If the target line group could not be located, log an error, and return
			//false.
			if(!foundReferencedLineGroup)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate exported line group with name " + lineGroupName + L" on exported bus interface with name " + busInterfaceName + L" for ImportLineGroup on System.ImportBusInterface!"));
				return false;
			}

			//Build info about this imported line group, and add it to the info about this
			//imported bus interface.
			ImportedLineGroupInfo importedLineGroupInfo;
			importedLineGroupInfo.lineGroupID = lineGroupID;
			importedLineGroupInfo.localName = lineGroupName;
			importedLineGroupInfo.importName = importName;
			importedBusInfo.importedLineGroups.push_back(importedLineGroupInfo);

			//Add a mapping for this line group name back to the line group ID
			lineGroupNameToIDMap.insert(NameToIDMapEntry(lineGroupName, lineGroupID));
		}
		else
		{
			//If we've encountered an unrecognized child element, log an error, and return
			//false.
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Unrecognized child element: " + elementName + L" encountered when processing System.ImportBusInterface!"));
			return false;
		}
	}

	//Store the details of this imported bus interface
	importedBusInterfaces.push_back(importedBusInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ClockSourceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ImportClockSource!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported clock source
	std::map<std::wstring, ExportedClockSourceInfo>::const_iterator exportedClockSourceInfoIterator = connectorDetails.exportedClockSourceInfo.find(importName);
	if(exportedClockSourceInfoIterator == connectorDetails.exportedClockSourceInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported clock source with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
		return false;
	}
	const ExportedClockSourceInfo& exportedClockSourceInfo = exportedClockSourceInfoIterator->second;

	//Record details of this imported object
	ImportedClockSourceInfo importedClockSourceInfo;
	importedClockSourceInfo.clockSource = exportedClockSourceInfo.clockSource;
	importedClockSourceInfo.exportingModuleClockSourceName = exportedClockSourceInfo.exportingModuleClockSourceName;
	importedClockSourceInfo.importName = exportedClockSourceInfo.importName;
	importedClockSourceInfo.importingModuleID = moduleID;
	importedClockSourceInfo.importingModuleClockSourceName = clockSourceName;
	importedClockSourceInfo.connectorID = connectorID;
	importedClockSources.push_back(importedClockSourceInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportSystemLine(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemLineName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemLineNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, SystemLineName, or ImportName attribute for System.ImportSystemLine!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported system line
	std::map<std::wstring, ExportedSystemLineInfo>::const_iterator exportedSystemLineInfoIterator = connectorDetails.exportedSystemLineInfo.find(importName);
	if(exportedSystemLineInfoIterator == connectorDetails.exportedSystemLineInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported system line with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
		return false;
	}
	const ExportedSystemLineInfo& exportedSystemLineInfo = exportedSystemLineInfoIterator->second;

	//Record details of this imported object
	ImportedSystemLineInfo importedSystemLineInfo;
	importedSystemLineInfo.systemLineID = exportedSystemLineInfo.systemLineID;
	importedSystemLineInfo.exportingModuleSystemLineName = exportedSystemLineInfo.exportingModuleSystemLineName;
	importedSystemLineInfo.importName = exportedSystemLineInfo.importName;
	importedSystemLineInfo.importingModuleID = moduleID;
	importedSystemLineInfo.importingModuleSystemLineName = systemLineName;
	importedSystemLineInfo.connectorID = connectorID;
	importedSystemLines.push_back(importedSystemLineInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportSystemSetting(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemOptionName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* systemSettingNameAttribute = node.GetAttribute(L"SystemSettingName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemSettingNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing ConnectorInstanceName, SystemSettingName, or ImportName attribute for System.ImportSystemSetting!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemSettingName = systemSettingNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported system line
	std::map<std::wstring, ExportedSystemSettingInfo>::const_iterator exportedSystemSettingInfoIterator = connectorDetails.exportedSystemSettingInfo.find(importName);
	if(exportedSystemSettingInfoIterator == connectorDetails.exportedSystemSettingInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not find exported system setting with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
		return false;
	}
	const ExportedSystemSettingInfo& exportedSystemSettingInfo = exportedSystemSettingInfoIterator->second;

	//Record details of this imported object
	ImportedSystemSettingInfo importedSystemSettingInfo;
	importedSystemSettingInfo.systemSettingID = exportedSystemSettingInfo.systemSettingID;
	importedSystemSettingInfo.exportingModuleSystemSettingName = exportedSystemSettingInfo.exportingModuleSystemSettingName;
	importedSystemSettingInfo.importName = exportedSystemSettingInfo.importName;
	importedSystemSettingInfo.importingModuleID = moduleID;
	importedSystemSettingInfo.importingModuleSystemSettingName = systemSettingName;
	importedSystemSettingInfo.connectorID = connectorID;
	importedSystemSettings.push_back(importedSystemSettingInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_DefineEmbeddedROM(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract all the required attributes
	IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHeirarchicalStorageAttribute* interfaceNumberAttribute = node.GetAttribute(L"InterfaceNumber");
	IHeirarchicalStorageAttribute* romregionSizeAttribute = node.GetAttribute(L"ROMRegionSize");
	IHeirarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	if((deviceInstanceNameAttribute == 0) || (interfaceNumberAttribute == 0) || (romregionSizeAttribute == 0) || (displayNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing TargetDeviceInstanceName, InterfaceNumber, ROMRegionSize or DisplayName attribute for System.DefineEmbeddedROM!"));
		return false;
	}
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	unsigned int interfaceNumber = interfaceNumberAttribute->ExtractValue<unsigned int>();
	unsigned int romRegionSize = romregionSizeAttribute->ExtractHexValue<unsigned int>();
	std::wstring displayName = displayNameAttribute->GetValue();

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for System.DefineEmbeddedROM!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Record details of this embedded ROM
	EmbeddedROMInfo embeddedROM;
	embeddedROM.moduleID = moduleID;
	embeddedROM.targetDevice = device;
	embeddedROM.interfaceNumber = interfaceNumber;
	embeddedROM.romRegionSize = romRegionSize;
	embeddedROM.displayName = displayName;
	embeddedROMInfo.push_back(embeddedROM);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_DefineSystemLine(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the Name and Width attributes
	IHeirarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	IHeirarchicalStorageAttribute* widthAttribute = node.GetAttribute(L"Width");
	if((nameAttribute == 0) || (widthAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Name or Width attribute for System.DefineSystemLine!"));
		return false;
	}
	std::wstring lineName = nameAttribute->GetValue();
	unsigned int lineWidth = widthAttribute->ExtractValue<unsigned int>();

	//Validate the supplied line width
	if(lineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The system line with name \"" + lineName + L"\" was declared with a specified width of 0 for System.DefineSystemLine!"));
		return false;
	}

	//Record details of this system line
	SystemLineInfo systemLine;
	systemLine.moduleID = moduleID;
	systemLine.lineName = lineName;
	systemLine.lineID = GenerateFreeSystemLineID();
	systemLine.lineWidth = lineWidth;
	systemLine.currentValue = 0;
	systemLines.insert(SystemLineMapEntry(systemLine.lineID, systemLine));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_MapSystemLine(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the Name and Width attributes
	IHeirarchicalStorageAttribute* sourceSystemLineNameAttribute = node.GetAttribute(L"SourceSystemLineName");
	IHeirarchicalStorageAttribute* targetDeviceInstanceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHeirarchicalStorageAttribute* targetLineAttribute = node.GetAttribute(L"TargetLine");
	if((sourceSystemLineNameAttribute == 0) || (targetDeviceInstanceNameAttribute == 0) || (targetLineAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SourceSystemLineName, TargetDeviceInstanceName, or TargetLine attribute for System.MapSystemLine!"));
		return false;
	}
	std::wstring systemLineName = sourceSystemLineNameAttribute->GetValue();
	std::wstring targetDeviceName = targetDeviceInstanceNameAttribute->GetValue();
	std::wstring targetLineName = targetLineAttribute->GetValue();

	//Extract the LineMapping attribute if one has been specified
	bool lineMappingSpecified = false;
	std::wstring lineMapping;
	IHeirarchicalStorageAttribute* lineMappingAttribute = node.GetAttribute(L"LineMapping");
	if(lineMappingAttribute != 0)
	{
		lineMappingSpecified = true;
		lineMapping = lineMappingAttribute->GetValue();
	}

	//Retrieve the target device object from the system
	IDevice* targetDevice = GetDevice(moduleID, targetDeviceName);
	if(targetDevice == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate target device with name \"" + targetDeviceName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the ID number for the system line
	unsigned int systemLineID = GetSystemLineID(moduleID, systemLineName);
	if(systemLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to locate system line with name \"" + systemLineName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the width of the system line
	unsigned int systemLineWidth = GetSystemLineWidth(systemLineID);
	if(systemLineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The system line with name \"" + systemLineName + L"\" returned a line width of 0 for System.MapSystemLine!"));
		return false;
	}

	//Obtain the ID number for the target line
	unsigned int targetLineID = targetDevice->GetLineID(targetLineName);
	if(targetLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to locate line with name \"" + targetLineName + L"\" on device with name \"" + targetDeviceName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the width of the target line
	unsigned int targetLineWidth = targetDevice->GetLineWidth(targetLineID);
	if(targetLineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The line with name \"" + targetLineName + L"\" on device with name \"" + targetDeviceName + L"\" returned a line width of 0 for System.MapSystemLine!"));
		return false;
	}

	//Extract any line bitmasks that have been specified
	unsigned int lineMaskAND = 0xFFFFFFFF;
	IHeirarchicalStorageAttribute* lineMaskANDAttribute = node.GetAttribute(L"ANDMask");
	if(lineMaskANDAttribute != 0)
	{
		lineMaskAND = lineMaskANDAttribute->ExtractHexValue<unsigned int>();
	}
	unsigned int lineMaskOR = 0;
	IHeirarchicalStorageAttribute* lineMaskORAttribute = node.GetAttribute(L"ORMask");
	if(lineMaskORAttribute != 0)
	{
		lineMaskOR = lineMaskORAttribute->ExtractHexValue<unsigned int>();
	}
	unsigned int lineMaskXOR = 0;
	IHeirarchicalStorageAttribute* lineMaskXORAttribute = node.GetAttribute(L"XORMask");
	if(lineMaskXORAttribute != 0)
	{
		lineMaskXOR = lineMaskXORAttribute->ExtractHexValue<unsigned int>();
	}

	//Record details of this system line
	SystemLineMapping systemLineMapping;
	systemLineMapping.moduleID = moduleID;
	systemLineMapping.targetDevice = targetDevice;
	systemLineMapping.systemLineID = systemLineID;
	systemLineMapping.systemLineBitCount = systemLineWidth;
	systemLineMapping.targetLine = targetLineID;
	systemLineMapping.targetLineBitCount = targetLineWidth;
	systemLineMapping.lineMaskAND = lineMaskAND;
	systemLineMapping.lineMaskOR = lineMaskOR;
	systemLineMapping.lineMaskXOR = lineMaskXOR;
	systemLineMapping.remapLines = lineMappingSpecified;
	systemLineMappings.push_back(systemLineMapping);
	SystemLineMapping& systemLineMappingAdded = *systemLineMappings.rbegin();

	//Attempt to build a line remap table if requested. Note that we do this after the
	//line has already been added to the list of system lines, so that we don't have to
	//copy the line remap table over to a new object when adding the line info to the
	//system line list.
	if(!systemLineMappingAdded.lineRemapTable.SetDataMapping(lineMapping, systemLineWidth))
	{
		//If we fail to successfully construct the line remap table, remove this system
		//line from the list of system lines, log an error, and return false.
		systemLineMappings.pop_back();
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load the system line with name \"" + systemLineName + L"\" because an error occurred attempting to construct a DataRemapTable from the LineMapping attribute for System.MapSystemLine!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_Setting(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName)
{
	//Extract the Name attribute
	IHeirarchicalStorageAttribute* settingNameAttribute = node.GetAttribute(L"Name");
	if(settingNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Name attribute for System.Setting!"));
		return false;
	}
	std::wstring settingName = settingNameAttribute->GetValue();

	//Build the display name
	std::wstring displayName = settingName;
	IHeirarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Populate the system setting object with this setting info
	SystemSettingInfo setting;
	setting.moduleID = moduleID;
	setting.name = settingName;
	setting.displayName = displayName;

	//Extract any optional attributes
	setting.toggleSetting = false;
	setting.toggleSettingAutoRevert = false;
	setting.toggleSettingAutoRevertTime = 0;
	IHeirarchicalStorageAttribute* toggleSettingAttribute = node.GetAttribute(L"ToggleSetting");
	IHeirarchicalStorageAttribute* toggleSettingAutoRevertAttribute = node.GetAttribute(L"ToggleSettingAutoRevert");
	IHeirarchicalStorageAttribute* toggleSettingAutoRevertTimeAttribute = node.GetAttribute(L"ToggleSettingAutoRevertTime");
	if(toggleSettingAttribute != 0)
	{
		setting.toggleSetting = toggleSettingAttribute->ExtractValue<bool>();
		if((toggleSettingAutoRevertAttribute != 0) && (toggleSettingAutoRevertTimeAttribute != 0))
		{
			setting.toggleSettingAutoRevert = toggleSettingAutoRevertAttribute->ExtractValue<bool>();
			setting.toggleSettingAutoRevertTime = toggleSettingAutoRevertTimeAttribute->ExtractValue<double>();
		}
	}

	//Load the child elements from this setting node
	unsigned int toggleSettingOnOptionIndex;
	bool toggleSettingOnOptionIndexDefined = false;
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		bool loadedSettingOptionSuccessfully = false;
		SystemSettingOption settingOption;
		bool defaultOption = false;
		bool toggleSettingOnOption = false;
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"System.Setting.Option")
		{
			loadedSettingOptionSuccessfully = LoadModule_System_Setting_Option(*(*i), moduleID, fileName, settingOption, defaultOption, toggleSettingOnOption);
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting node under module file " + fileName + L"."));
			continue;
		}

		//If this option defines the on state for a toggle setting, latch this option
		//index.
		if(toggleSettingOnOption)
		{
			toggleSettingOnOptionIndex = (unsigned int)setting.options.size();
			toggleSettingOnOptionIndexDefined = true;
		}

		//If we managed to load the setting option node successfully, record its
		//information within this system setting, otherwise log an error and return false.
		if(loadedSettingOptionSuccessfully)
		{
			//Store the index for this new option as the default option for this setting
			//if it was flagged as the default option
			if(defaultOption)
			{
				setting.defaultOption = (unsigned int)setting.options.size();
			}

			//Store this option within this system setting
			setting.options.push_back(settingOption);
		}
		else
		{
			//Log an error if we failed to load an element correctly, and return false.
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting node under module file " + fileName + L"."));
			return false;
		}
	}

	//If this setting is a toggle setting, and there are more or less than 2 options
	//defined for this setting, log an error, and return false.
	unsigned int optionCount = (unsigned int)setting.options.size();
	if(setting.toggleSetting && (optionCount != 2))
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
		logEntry << L"A total of " << optionCount << L" options were found for a toggle setting when loading System.Setting node under module file " << fileName << L". Only 2 options can be defined for a toggle setting.";
		WriteLogEvent(logEntry);
		return false;
	}

	//If this setting is a toggle setting, and no option was defined as the on state for
	//the toggle setting, log an error, and return false.
	if(setting.toggleSetting && !toggleSettingOnOptionIndexDefined)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"No options set the ToggleSettingOnOption attribute for toggle setting with name : " + setting.name + L" when loading System.Setting node under module file " + fileName + L"."));
		return false;
	}

	//If this setting is a toggle setting, set the option index numbers for the on and off
	//options.
	if(setting.toggleSetting)
	{
		setting.onOption = toggleSettingOnOptionIndex;
		setting.offOption = (toggleSettingOnOptionIndex != 0)? 0: 1;
	}

	//Generate an ID number for this system setting
	setting.systemSettingID = GenerateFreeSystemSettingID();

	//Add this system setting to the set of defined system settings
	systemSettings.insert(SystemSettingsMapEntry(setting.systemSettingID, setting));
	moduleSettings[moduleID].push_back(setting.systemSettingID);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_Setting_Option(IHeirarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName, SystemSettingOption& option, bool& defaultOption, bool& toggleSettingOnOption)
{
	//Extract the Name attribute
	IHeirarchicalStorageAttribute* optionNameAttribute = node.GetAttribute(L"Name");
	if(optionNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing Name attribute for System.Setting.Option!"));
		return false;
	}
	std::wstring optionName = optionNameAttribute->GetValue();

	//Build the display name
	std::wstring displayName = optionName;
	IHeirarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Extract the Default attribute
	defaultOption = false;
	IHeirarchicalStorageAttribute* defaultAttribute = node.GetAttribute(L"Default");
	if(defaultAttribute != 0)
	{
		defaultOption = defaultAttribute->ExtractValue<bool>();
	}

	//Extract the ToggleSettingOnOption attribute
	toggleSettingOnOption = false;
	IHeirarchicalStorageAttribute* toggleSettingOnOptionAttribute = node.GetAttribute(L"ToggleSettingOnOption");
	if(toggleSettingOnOptionAttribute != 0)
	{
		toggleSettingOnOption = toggleSettingOnOptionAttribute->ExtractValue<bool>();
	}

	//Populate the system setting option object with this option info
	option.name = optionName;
	option.displayName = displayName;

	//Load the child elements from this option node
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		bool loadedSettingChangeSuccessfully = false;
		SystemStateChange settingChange;
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"System.SetClockFrequency")
		{
			loadedSettingChangeSuccessfully = LoadModule_System_SetClockFrequency(*(*i), moduleID, settingChange);
		}
		else if(elementName == L"System.SetLineState")
		{
			loadedSettingChangeSuccessfully = LoadModule_System_SetLineState(*(*i), moduleID, settingChange);
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting.Option node under module file " + fileName + L"."));
			continue;
		}

		//If we managed to load the settings change node successfully, record its
		//information within this system option, otherwise log an error and return false.
		if(loadedSettingChangeSuccessfully)
		{
			//Store this settings change within this option
			option.stateChanges.push_back(settingChange);
		}
		else
		{
			//Log an error if we failed to load an element correctly, and return false.
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting.Option node under module file " + fileName + L"."));
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SelectSettingOption(IHeirarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the SettingName and OptionName attributes
	IHeirarchicalStorageAttribute* settingNameAttribute = node.GetAttribute(L"SettingName");
	IHeirarchicalStorageAttribute* optionNameAttribute = node.GetAttribute(L"OptionName");
	if((settingNameAttribute == 0) || (optionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SettingName or OptionName attribute for System.SelectSettingOption!"));
		return false;
	}
	std::wstring settingName = settingNameAttribute->GetValue();
	std::wstring optionName = optionNameAttribute->GetValue();

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SYSTEMSTATECHANGETYPE_SETSYSTEMOPTION;
	stateChange.targetElementName = settingName;
	stateChange.setSystemOptionValue = optionName;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SetClockFrequency(IHeirarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the TargetClockName, ClockType, and Value attributes
	IHeirarchicalStorageAttribute* targetClockNameAttribute = node.GetAttribute(L"TargetClockName");
	IHeirarchicalStorageAttribute* clockTypeAttribute = node.GetAttribute(L"ClockType");
	IHeirarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((targetClockNameAttribute == 0) || (clockTypeAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing TargetClockName, ClockType, or Value attribute for System.SetClockFrequency!"));
		return false;
	}
	std::wstring targetClockName = targetClockNameAttribute->GetValue();
	std::wstring clockTypeString = clockTypeAttribute->GetValue();
	double clockRate = valueAttribute->ExtractValue<double>();

	//Decode the clock type string
	ClockSource::ClockType clockType;
	if(!ClockSource::DecodeClockTypeString(clockTypeString, clockType))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to decode the clock type string \"" + clockTypeString + L"\" for the target clock with name \"" + targetClockName + L"\" for System.SetClockFrequency!"));
		return false;
	}

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SYSTEMSTATECHANGETYPE_SETCLOCKFREQUENCY;
	stateChange.targetElementName = targetClockName;
	stateChange.setClockRateClockType = clockType;
	stateChange.setClockRateValue = clockRate;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SetLineState(IHeirarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the SystemLineName and Value attributes
	IHeirarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHeirarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((systemLineNameAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SystemLineName or Value attribute for System.SetClockFrequency!"));
		return false;
	}
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	unsigned int lineValue = valueAttribute->ExtractHexValue<unsigned int>();

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SYSTEMSTATECHANGETYPE_SETSYSTEMLINESTATE;
	stateChange.targetElementName = systemLineName;
	stateChange.setLineStateValue = lineValue;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_ProcessViewModelQueue(const std::list<ViewModelOpenRequest>& viewModelOpenRequests, IViewModelLauncher& aviewModelLauncher)
{
	//Process each view model open request
	bool result = true;
	for(std::list<ViewModelOpenRequest>::const_iterator i = viewModelOpenRequests.begin(); i != viewModelOpenRequests.end(); ++i)
	{
		const ViewModelOpenRequest& request = *i;
		if(request.ownerSystem)
		{
			//Open the view
			debugMenuHandler->OpenViewModel(request.viewModelGroupName, request.viewModelName, aviewModelLauncher);
		}
		else
		{
			//Retrieve the device object from the system
			IDevice* device = GetDevice(request.moduleID, request.deviceInstanceName);
			if(device == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + request.deviceInstanceName + L" for System.OpenViewModel!"));
				result = false;
			}

			//Open the view
			device->OpenViewModel(request.viewModelGroupName, request.viewModelName, aviewModelLauncher);
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
void System::UnloadAllModulesSynchronous()
{
	clearSystemComplete = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&System::UnloadAllModules), this));
}

//----------------------------------------------------------------------------------------
bool System::UnloadAllModulesSynchronousComplete() const
{
	return clearSystemComplete;
}

//----------------------------------------------------------------------------------------
void System::UnloadAllModules()
{
	boost::mutex::scoped_lock lock(debugMutex);

	//Stop the system if it is currently running
	StopSystem();

	//Unload each loaded module
	while(!loadedModuleInfoList.empty())
	{
		LoadedModuleInfoInternal moduleInfo = *loadedModuleInfoList.begin();
		UnloadModuleInternal(moduleInfo.moduleID);
	}

	//Flag that the operation is complete
	clearSystemComplete = true;
}

//----------------------------------------------------------------------------------------
bool System::ReadModuleConnectorInfo(const std::wstring& filePath, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const
{
	//Open the target file
	FileStreamReference streamReference(*guiExtensionInterface);
	if(!streamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not open module file " + filePath + L"!"));
		return false;
	}
	Stream::IStream& source = *streamReference;

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
	source.ProcessByteOrderMark();

	//Load the XML structure from the file
	HeirarchicalStorageTree tree;
	if(!tree.LoadTree(source))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error loading XML structure from module file " + filePath + L"! The xml error string is as follows: " + tree.GetErrorString()));
		return false;
	}
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();

	//If this is a system module, no connector info exists. In this case, return true with
	//no data in the connector import and export lists, and a blank system class name.
	if(rootNode.GetName() == L"System")
	{
		return true;
	}

	//Extract the system class name
	IHeirarchicalStorageAttribute* systemClassNameAttribute = rootNode.GetAttribute(L"SystemClassName");
	if(systemClassNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SystemClassName on root node!"));
		return false;
	}
	systemClassName = systemClassNameAttribute->GetValue();

	//Extract connector definitions
	std::list<IHeirarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
	{
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"System.ExportConnector")
		{
			IHeirarchicalStorageAttribute* connectorClassNameAttribute = (*i)->GetAttribute(L"ConnectorClassName");
			IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = (*i)->GetAttribute(L"ConnectorInstanceName");
			if((connectorClassNameAttribute != 0) && (connectorInstanceNameAttribute != 0))
			{
				std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
				std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
				ConnectorDefinitionExport connectorDefinition;
				connectorDefinition.className = connectorClassName;
				connectorDefinition.instanceName = connectorInstanceName;
				connectorsExported.push_back(connectorDefinition);
			}
		}
		else if(elementName == L"System.ImportConnector")
		{
			IHeirarchicalStorageAttribute* connectorClassNameAttribute = (*i)->GetAttribute(L"ConnectorClassName");
			IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = (*i)->GetAttribute(L"ConnectorInstanceName");
			if((connectorClassNameAttribute != 0) && (connectorInstanceNameAttribute != 0))
			{
				std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
				std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
				ConnectorDefinitionImport connectorDefinition;
				connectorDefinition.className = connectorClassName;
				connectorDefinition.instanceName = connectorInstanceName;
				connectorsImported.push_back(connectorDefinition);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
std::wstring System::LoadModuleSynchronousCurrentModuleName() const
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	std::wstring moduleName;
	if(!loadSystemCurrentModuleNameStack.empty())
	{
		moduleName = *loadSystemCurrentModuleNameStack.rbegin();
	}
	return moduleName;
}

//----------------------------------------------------------------------------------------
void System::PushLoadModuleCurrentModuleName(const std::wstring& moduleName)
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	loadSystemCurrentModuleNameStack.push_back(moduleName);
}

//----------------------------------------------------------------------------------------
void System::PopLoadModuleCurrentModuleName()
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	loadSystemCurrentModuleNameStack.pop_back();
}

//----------------------------------------------------------------------------------------
std::wstring System::UnloadModuleSynchronousCurrentModuleName() const
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	std::wstring moduleName;
	if(!unloadSystemCurrentModuleNameStack.empty())
	{
		moduleName = *unloadSystemCurrentModuleNameStack.rbegin();
	}
	return moduleName;
}

//----------------------------------------------------------------------------------------
void System::PushUnloadModuleCurrentModuleName(const std::wstring& moduleName)
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	unloadSystemCurrentModuleNameStack.push_back(moduleName);
}

//----------------------------------------------------------------------------------------
void System::PopUnloadModuleCurrentModuleName()
{
	boost::mutex::scoped_lock lock(moduleNameMutex);
	unloadSystemCurrentModuleNameStack.pop_back();
}

//----------------------------------------------------------------------------------------
//Loaded module info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> System::GetLoadedModuleIDs() const
{
	std::list<unsigned int> idList;
	for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
	{
		idList.push_back(i->moduleID);
	}
	return idList;
}

//----------------------------------------------------------------------------------------
void System::GetLoadedModuleIDsInternal(unsigned int itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const
{
	//Ensure that the supplied array is big enough to hold all the items
	requiredSize = (unsigned int)loadedModuleInfoList.size();
	if(requiredSize > arraySize)
	{
		itemsRetrieved = false;
		return;
	}

	//Write all the items to the array
	unsigned int arrayIndex = 0;
	for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
	{
		itemArray[arrayIndex++] = i->moduleID;
	}
	itemsRetrieved = true;
}

//----------------------------------------------------------------------------------------
bool System::GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const
{
	bool foundEntry = false;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	while(!foundEntry && (loadedModuleIterator != loadedModuleInfoList.end()))
	{
		if(loadedModuleIterator->moduleID == moduleID)
		{
			moduleInfo.SetModuleID(loadedModuleIterator->moduleID);
			moduleInfo.SetModuleFilePath(loadedModuleIterator->filePath);
			moduleInfo.SetIsProgramModule(loadedModuleIterator->programModule);
			moduleInfo.SetSystemClassName(loadedModuleIterator->systemClassName);
			moduleInfo.SetClassName(loadedModuleIterator->className);
			moduleInfo.SetInstanceName(loadedModuleIterator->instanceName);
			moduleInfo.SetDisplayName(loadedModuleIterator->displayName);
			moduleInfo.SetProductionYear(loadedModuleIterator->productionYear);
			moduleInfo.SetManufacturerCode(loadedModuleIterator->manufacturerCode);
			moduleInfo.SetManufacturerDisplayName(loadedModuleIterator->manufacturerDisplayName);
			foundEntry = true;
			continue;
		}
		++loadedModuleIterator;
	}
	return foundEntry;
}

//----------------------------------------------------------------------------------------
//Connector info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> System::GetConnectorIDs() const
{
	std::list<unsigned int> idList;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		idList.push_back(i->second.connectorID);
	}
	return idList;
}

//----------------------------------------------------------------------------------------
void System::GetConnectorIDsInternal(unsigned int itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const
{
	//Ensure that the supplied array is big enough to hold all the items
	requiredSize = (unsigned int)connectorDetailsMap.size();
	if(requiredSize > arraySize)
	{
		itemsRetrieved = false;
		return;
	}

	//Write all the items to the array
	unsigned int arrayIndex = 0;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		itemArray[arrayIndex++] = i->second.connectorID;
	}
	itemsRetrieved = true;
}

//----------------------------------------------------------------------------------------
bool System::GetConnectorInfo(unsigned int connectorID, IConnectorInfo& connectorInfo) const
{
	bool foundEntry = false;
	ConnectorDetailsMap::const_iterator connectorDetailsIterator = connectorDetailsMap.begin();
	while(!foundEntry && (connectorDetailsIterator != connectorDetailsMap.end()))
	{
		if(connectorDetailsIterator->second.connectorID == connectorID)
		{
			const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;
			connectorInfo.SetConnectorID(connectorDetails.connectorID);
			connectorInfo.SetExportingModuleID(connectorDetails.exportingModuleID);
			connectorInfo.SetConnectorClassName(connectorDetails.connectorClassName);
			connectorInfo.SetExportingModuleConnectorInstanceName(connectorDetails.exportingModuleConnectorInstanceName);
			connectorInfo.SetSystemClassName(connectorDetails.systemClassName);
			connectorInfo.SetIsConnectorUsed(connectorDetails.connectorUsed);
			connectorInfo.SetImportingModuleID(connectorDetails.importingModuleID);
			connectorInfo.SetImportingModuleConnectorInstanceName(connectorDetails.importingModuleConnectorInstanceName);
			foundEntry = true;
			continue;
		}
		++connectorDetailsIterator;
	}
	return foundEntry;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
IDeviceContext::KeyCode System::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	//Control keys
	if(keyCodeName == L"Esc")             return IDeviceContext::KEYCODE_ESCAPE;
	else if(keyCodeName == L"Tab")        return IDeviceContext::KEYCODE_TAB;
	else if(keyCodeName == L"Enter")      return IDeviceContext::KEYCODE_ENTER;
	else if(keyCodeName == L"Space")      return IDeviceContext::KEYCODE_SPACE;
	else if(keyCodeName == L"Backspace")  return IDeviceContext::KEYCODE_BACKSPACE;
	else if(keyCodeName == L"Insert")     return IDeviceContext::KEYCODE_INSERT;
	else if(keyCodeName == L"Delete")     return IDeviceContext::KEYCODE_DELETE;
	else if(keyCodeName == L"PgUp")       return IDeviceContext::KEYCODE_PAGEUP;
	else if(keyCodeName == L"PgDn")       return IDeviceContext::KEYCODE_PAGEDOWN;
	else if(keyCodeName == L"Home")       return IDeviceContext::KEYCODE_HOME;
	else if(keyCodeName == L"End")        return IDeviceContext::KEYCODE_END;
	else if(keyCodeName == L"Up")         return IDeviceContext::KEYCODE_UP;
	else if(keyCodeName == L"Down")       return IDeviceContext::KEYCODE_DOWN;
	else if(keyCodeName == L"Left")       return IDeviceContext::KEYCODE_LEFT;
	else if(keyCodeName == L"Right")      return IDeviceContext::KEYCODE_RIGHT;
	else if(keyCodeName == L"Print")      return IDeviceContext::KEYCODE_PRINTSCREEN;
	else if(keyCodeName == L"Pause")      return IDeviceContext::KEYCODE_PAUSE;
	else if(keyCodeName == L"NumLock")    return IDeviceContext::KEYCODE_NUMLOCK;
	else if(keyCodeName == L"CapsLock")   return IDeviceContext::KEYCODE_CAPSLOCK;
	else if(keyCodeName == L"ScrollLock") return IDeviceContext::KEYCODE_SCROLLLOCK;
	else if(keyCodeName == L"LeftWin")    return IDeviceContext::KEYCODE_LEFTWINDOWS;
	else if(keyCodeName == L"RightWin")   return IDeviceContext::KEYCODE_RIGHTWINDOWS;
	else if(keyCodeName == L"Menu")       return IDeviceContext::KEYCODE_MENU;
	//Modifier keys
	else if(keyCodeName == L"Ctrl")       return IDeviceContext::KEYCODE_CTRL;
	else if(keyCodeName == L"Alt")        return IDeviceContext::KEYCODE_ALT;
	else if(keyCodeName == L"Shift")      return IDeviceContext::KEYCODE_SHIFT;
	//Function keys
	else if(keyCodeName == L"F1")         return IDeviceContext::KEYCODE_F1;
	else if(keyCodeName == L"F2")         return IDeviceContext::KEYCODE_F2;
	else if(keyCodeName == L"F3")         return IDeviceContext::KEYCODE_F3;
	else if(keyCodeName == L"F4")         return IDeviceContext::KEYCODE_F4;
	else if(keyCodeName == L"F5")         return IDeviceContext::KEYCODE_F5;
	else if(keyCodeName == L"F6")         return IDeviceContext::KEYCODE_F6;
	else if(keyCodeName == L"F7")         return IDeviceContext::KEYCODE_F7;
	else if(keyCodeName == L"F8")         return IDeviceContext::KEYCODE_F8;
	else if(keyCodeName == L"F9")         return IDeviceContext::KEYCODE_F9;
	else if(keyCodeName == L"F10")        return IDeviceContext::KEYCODE_F10;
	else if(keyCodeName == L"F11")        return IDeviceContext::KEYCODE_F11;
	else if(keyCodeName == L"F12")        return IDeviceContext::KEYCODE_F12;
	//Numbers
	else if(keyCodeName == L"0")          return IDeviceContext::KEYCODE_0;
	else if(keyCodeName == L"1")          return IDeviceContext::KEYCODE_1;
	else if(keyCodeName == L"2")          return IDeviceContext::KEYCODE_2;
	else if(keyCodeName == L"3")          return IDeviceContext::KEYCODE_3;
	else if(keyCodeName == L"4")          return IDeviceContext::KEYCODE_4;
	else if(keyCodeName == L"5")          return IDeviceContext::KEYCODE_5;
	else if(keyCodeName == L"6")          return IDeviceContext::KEYCODE_6;
	else if(keyCodeName == L"7")          return IDeviceContext::KEYCODE_7;
	else if(keyCodeName == L"8")          return IDeviceContext::KEYCODE_8;
	else if(keyCodeName == L"9")          return IDeviceContext::KEYCODE_9;
	//Letters
	else if(keyCodeName == L"A")          return IDeviceContext::KEYCODE_A;
	else if(keyCodeName == L"B")          return IDeviceContext::KEYCODE_B;
	else if(keyCodeName == L"C")          return IDeviceContext::KEYCODE_C;
	else if(keyCodeName == L"D")          return IDeviceContext::KEYCODE_D;
	else if(keyCodeName == L"E")          return IDeviceContext::KEYCODE_E;
	else if(keyCodeName == L"F")          return IDeviceContext::KEYCODE_F;
	else if(keyCodeName == L"G")          return IDeviceContext::KEYCODE_G;
	else if(keyCodeName == L"H")          return IDeviceContext::KEYCODE_H;
	else if(keyCodeName == L"I")          return IDeviceContext::KEYCODE_I;
	else if(keyCodeName == L"J")          return IDeviceContext::KEYCODE_J;
	else if(keyCodeName == L"K")          return IDeviceContext::KEYCODE_K;
	else if(keyCodeName == L"L")          return IDeviceContext::KEYCODE_L;
	else if(keyCodeName == L"M")          return IDeviceContext::KEYCODE_M;
	else if(keyCodeName == L"N")          return IDeviceContext::KEYCODE_N;
	else if(keyCodeName == L"O")          return IDeviceContext::KEYCODE_O;
	else if(keyCodeName == L"P")          return IDeviceContext::KEYCODE_P;
	else if(keyCodeName == L"Q")          return IDeviceContext::KEYCODE_Q;
	else if(keyCodeName == L"R")          return IDeviceContext::KEYCODE_R;
	else if(keyCodeName == L"S")          return IDeviceContext::KEYCODE_S;
	else if(keyCodeName == L"T")          return IDeviceContext::KEYCODE_T;
	else if(keyCodeName == L"U")          return IDeviceContext::KEYCODE_U;
	else if(keyCodeName == L"V")          return IDeviceContext::KEYCODE_V;
	else if(keyCodeName == L"W")          return IDeviceContext::KEYCODE_W;
	else if(keyCodeName == L"X")          return IDeviceContext::KEYCODE_X;
	else if(keyCodeName == L"Y")          return IDeviceContext::KEYCODE_Y;
	else if(keyCodeName == L"Z")          return IDeviceContext::KEYCODE_Z;
	//Symbol keys
	else if(keyCodeName == L"OEM1")       return IDeviceContext::KEYCODE_OEM1;
	else if(keyCodeName == L"OEMPlus")    return IDeviceContext::KEYCODE_OEMPLUS;
	else if(keyCodeName == L"OEMComma")   return IDeviceContext::KEYCODE_OEMCOMMA;
	else if(keyCodeName == L"OEMMinus")   return IDeviceContext::KEYCODE_OEMMINUS;
	else if(keyCodeName == L"OEMPeriod")  return IDeviceContext::KEYCODE_OEMPERIOD;
	else if(keyCodeName == L"OEM2")       return IDeviceContext::KEYCODE_OEM2;
	else if(keyCodeName == L"OEM3")       return IDeviceContext::KEYCODE_OEM3;
	else if(keyCodeName == L"OEM4")       return IDeviceContext::KEYCODE_OEM4;
	else if(keyCodeName == L"OEM5")       return IDeviceContext::KEYCODE_OEM5;
	else if(keyCodeName == L"OEM6")       return IDeviceContext::KEYCODE_OEM6;
	else if(keyCodeName == L"OEM7")       return IDeviceContext::KEYCODE_OEM7;
	else if(keyCodeName == L"OEM8")       return IDeviceContext::KEYCODE_OEM8;
	else if(keyCodeName == L"OEMAX")      return IDeviceContext::KEYCODE_OEMAX;
	else if(keyCodeName == L"OEM102")     return IDeviceContext::KEYCODE_OEM102;
	//Numpad keys
	else if(keyCodeName == L"Numpad0")    return IDeviceContext::KEYCODE_NUMPAD0;
	else if(keyCodeName == L"Numpad1")    return IDeviceContext::KEYCODE_NUMPAD1;
	else if(keyCodeName == L"Numpad2")    return IDeviceContext::KEYCODE_NUMPAD2;
	else if(keyCodeName == L"Numpad3")    return IDeviceContext::KEYCODE_NUMPAD3;
	else if(keyCodeName == L"Numpad4")    return IDeviceContext::KEYCODE_NUMPAD4;
	else if(keyCodeName == L"Numpad5")    return IDeviceContext::KEYCODE_NUMPAD5;
	else if(keyCodeName == L"Numpad6")    return IDeviceContext::KEYCODE_NUMPAD6;
	else if(keyCodeName == L"Numpad7")    return IDeviceContext::KEYCODE_NUMPAD7;
	else if(keyCodeName == L"Numpad8")    return IDeviceContext::KEYCODE_NUMPAD8;
	else if(keyCodeName == L"Numpad9")    return IDeviceContext::KEYCODE_NUMPAD9;
	else if(keyCodeName == L"Numpad*")    return IDeviceContext::KEYCODE_NUMPADMULTIPLY;
	else if(keyCodeName == L"Numpad/")    return IDeviceContext::KEYCODE_NUMPADDIVIDE;
	else if(keyCodeName == L"Numpad-")    return IDeviceContext::KEYCODE_NUMPADSUBTRACT;
	else if(keyCodeName == L"Numpad+")    return IDeviceContext::KEYCODE_NUMPADADD;
	else if(keyCodeName == L"Numpad.")    return IDeviceContext::KEYCODE_NUMPADDECIMAL;

	return IDeviceContext::KEYCODE_NONE;
}

//----------------------------------------------------------------------------------------
std::wstring System::GetKeyCodeName(IDeviceContext::KeyCode keyCode) const
{
	switch(keyCode)
	{
	//Control keys
	case IDeviceContext::KEYCODE_ESCAPE:            return L"Esc";
	case IDeviceContext::KEYCODE_TAB:               return L"Tab";
	case IDeviceContext::KEYCODE_ENTER:             return L"Enter";
	case IDeviceContext::KEYCODE_SPACE:             return L"Space";
	case IDeviceContext::KEYCODE_BACKSPACE:         return L"Backspace";
	case IDeviceContext::KEYCODE_INSERT:            return L"Insert";
	case IDeviceContext::KEYCODE_DELETE:            return L"Delete";
	case IDeviceContext::KEYCODE_PAGEUP:            return L"PgUp";
	case IDeviceContext::KEYCODE_PAGEDOWN:          return L"PgDn";
	case IDeviceContext::KEYCODE_HOME:              return L"Home";
	case IDeviceContext::KEYCODE_END:               return L"End";
	case IDeviceContext::KEYCODE_UP:                return L"Up";
	case IDeviceContext::KEYCODE_DOWN:              return L"Down";
	case IDeviceContext::KEYCODE_LEFT:              return L"Left";
	case IDeviceContext::KEYCODE_RIGHT:             return L"Right";
	case IDeviceContext::KEYCODE_PRINTSCREEN:       return L"Print";
	case IDeviceContext::KEYCODE_PAUSE:             return L"Pause";
	case IDeviceContext::KEYCODE_NUMLOCK:           return L"NumLock";
	case IDeviceContext::KEYCODE_CAPSLOCK:          return L"CapsLock";
	case IDeviceContext::KEYCODE_SCROLLLOCK:        return L"ScrollLock";
	case IDeviceContext::KEYCODE_LEFTWINDOWS:       return L"LeftWin";
	case IDeviceContext::KEYCODE_RIGHTWINDOWS:      return L"RightWin";
	case IDeviceContext::KEYCODE_MENU:              return L"Menu";
	//Modifier keys
	case IDeviceContext::KEYCODE_CTRL:              return L"Ctrl";
	case IDeviceContext::KEYCODE_ALT:               return L"Alt";
	case IDeviceContext::KEYCODE_SHIFT:             return L"Shift";
	//Function keys
	case IDeviceContext::KEYCODE_F1:                return L"F1";
	case IDeviceContext::KEYCODE_F2:                return L"F2";
	case IDeviceContext::KEYCODE_F3:                return L"F3";
	case IDeviceContext::KEYCODE_F4:                return L"F4";
	case IDeviceContext::KEYCODE_F5:                return L"F5";
	case IDeviceContext::KEYCODE_F6:                return L"F6";
	case IDeviceContext::KEYCODE_F7:                return L"F7";
	case IDeviceContext::KEYCODE_F8:                return L"F8";
	case IDeviceContext::KEYCODE_F9:                return L"F9";
	case IDeviceContext::KEYCODE_F10:               return L"F10";
	case IDeviceContext::KEYCODE_F11:               return L"F11";
	case IDeviceContext::KEYCODE_F12:               return L"F12";
	//Numbers
	case IDeviceContext::KEYCODE_0:                 return L"0";
	case IDeviceContext::KEYCODE_1:                 return L"1";
	case IDeviceContext::KEYCODE_2:                 return L"2";
	case IDeviceContext::KEYCODE_3:                 return L"3";
	case IDeviceContext::KEYCODE_4:                 return L"4";
	case IDeviceContext::KEYCODE_5:                 return L"5";
	case IDeviceContext::KEYCODE_6:                 return L"6";
	case IDeviceContext::KEYCODE_7:                 return L"7";
	case IDeviceContext::KEYCODE_8:                 return L"8";
	case IDeviceContext::KEYCODE_9:                 return L"9";
	//Letters
	case IDeviceContext::KEYCODE_A:                 return L"A";
	case IDeviceContext::KEYCODE_B:                 return L"B";
	case IDeviceContext::KEYCODE_C:                 return L"C";
	case IDeviceContext::KEYCODE_D:                 return L"D";
	case IDeviceContext::KEYCODE_E:                 return L"E";
	case IDeviceContext::KEYCODE_F:                 return L"F";
	case IDeviceContext::KEYCODE_G:                 return L"G";
	case IDeviceContext::KEYCODE_H:                 return L"H";
	case IDeviceContext::KEYCODE_I:                 return L"I";
	case IDeviceContext::KEYCODE_J:                 return L"J";
	case IDeviceContext::KEYCODE_K:                 return L"K";
	case IDeviceContext::KEYCODE_L:                 return L"L";
	case IDeviceContext::KEYCODE_M:                 return L"M";
	case IDeviceContext::KEYCODE_N:                 return L"N";
	case IDeviceContext::KEYCODE_O:                 return L"O";
	case IDeviceContext::KEYCODE_P:                 return L"P";
	case IDeviceContext::KEYCODE_Q:                 return L"Q";
	case IDeviceContext::KEYCODE_R:                 return L"R";
	case IDeviceContext::KEYCODE_S:                 return L"S";
	case IDeviceContext::KEYCODE_T:                 return L"T";
	case IDeviceContext::KEYCODE_U:                 return L"U";
	case IDeviceContext::KEYCODE_V:                 return L"V";
	case IDeviceContext::KEYCODE_W:                 return L"W";
	case IDeviceContext::KEYCODE_X:                 return L"X";
	case IDeviceContext::KEYCODE_Y:                 return L"Y";
	case IDeviceContext::KEYCODE_Z:                 return L"Z";
	//Symbol keys
	case IDeviceContext::KEYCODE_OEM1:              return L"OEM1";
	case IDeviceContext::KEYCODE_OEMPLUS:           return L"OEMPlus";
	case IDeviceContext::KEYCODE_OEMCOMMA:          return L"OEMComma";
	case IDeviceContext::KEYCODE_OEMMINUS:          return L"OEMMinus";
	case IDeviceContext::KEYCODE_OEMPERIOD:         return L"OEMPeriod";
	case IDeviceContext::KEYCODE_OEM2:              return L"OEM2";
	case IDeviceContext::KEYCODE_OEM3:              return L"OEM3";
	case IDeviceContext::KEYCODE_OEM4:              return L"OEM4";
	case IDeviceContext::KEYCODE_OEM5:              return L"OEM5";
	case IDeviceContext::KEYCODE_OEM6:              return L"OEM6";
	case IDeviceContext::KEYCODE_OEM7:              return L"OEM7";
	case IDeviceContext::KEYCODE_OEM8:              return L"OEM8";
	case IDeviceContext::KEYCODE_OEMAX:             return L"OEMAX";
	case IDeviceContext::KEYCODE_OEM102:            return L"OEM102";
	//Numpad keys
	case IDeviceContext::KEYCODE_NUMPAD0:           return L"Numpad0";
	case IDeviceContext::KEYCODE_NUMPAD1:           return L"Numpad1";
	case IDeviceContext::KEYCODE_NUMPAD2:           return L"Numpad2";
	case IDeviceContext::KEYCODE_NUMPAD3:           return L"Numpad3";
	case IDeviceContext::KEYCODE_NUMPAD4:           return L"Numpad4";
	case IDeviceContext::KEYCODE_NUMPAD5:           return L"Numpad5";
	case IDeviceContext::KEYCODE_NUMPAD6:           return L"Numpad6";
	case IDeviceContext::KEYCODE_NUMPAD7:           return L"Numpad7";
	case IDeviceContext::KEYCODE_NUMPAD8:           return L"Numpad8";
	case IDeviceContext::KEYCODE_NUMPAD9:           return L"Numpad9";
	case IDeviceContext::KEYCODE_NUMPADMULTIPLY:    return L"Numpad*";
	case IDeviceContext::KEYCODE_NUMPADDIVIDE:      return L"Numpad/";
	case IDeviceContext::KEYCODE_NUMPADSUBTRACT:    return L"Numpad-";
	case IDeviceContext::KEYCODE_NUMPADADD:         return L"Numpad+";
	case IDeviceContext::KEYCODE_NUMPADDECIMAL:     return L"Numpad.";
	}
	return L"None";
}

//----------------------------------------------------------------------------------------
bool System::IsKeyCodeMapped(IDeviceContext::KeyCode keyCode) const
{
	boost::mutex::scoped_lock lock(inputMutex);
	return (inputKeyMap.find(keyCode) != inputKeyMap.end());
}

//----------------------------------------------------------------------------------------
bool System::IsDeviceKeyCodeMapped(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const
{
	boost::mutex::scoped_lock lock(inputMutex);
	bool foundMapping = false;
	InputKeyMap::const_iterator inputKeyMapIterator = inputKeyMap.begin();
	while(!foundMapping&& (inputKeyMapIterator != inputKeyMap.end()))
	{
		const InputMapEntry& inputMapEntry = inputKeyMapIterator->second;
		if((inputMapEntry.targetDevice == targetDevice) && (inputMapEntry.targetDeviceKeyCode == targetDeviceKeyCode))
		{
			foundMapping = true;
		}
		++inputKeyMapIterator;
	}
	return foundMapping;
}

//----------------------------------------------------------------------------------------
IDeviceContext::KeyCode System::GetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const
{
	boost::mutex::scoped_lock lock(inputMutex);
	InputKeyMap::const_iterator inputKeyMapIterator = inputKeyMap.begin();
	while(inputKeyMapIterator != inputKeyMap.end())
	{
		const InputMapEntry& inputMapEntry = inputKeyMapIterator->second;
		if((inputMapEntry.targetDevice == targetDevice) && (inputMapEntry.targetDeviceKeyCode == targetDeviceKeyCode))
		{
			return inputMapEntry.keyCode;
		}
		++inputKeyMapIterator;
	}
	return IDeviceContext::KEYCODE_NONE;
}

//----------------------------------------------------------------------------------------
bool System::MapKeyCode(IDevice* targetDevice, unsigned int targetDeviceKeyCode, IDeviceContext::KeyCode keyCode)
{
	if(!IsKeyCodeMapped(keyCode))
	{
		boost::mutex::scoped_lock lock(inputMutex);
		InputMapEntry mapEntry;
		mapEntry.keyCode = keyCode;
		mapEntry.targetDevice = targetDevice;
		mapEntry.targetDeviceKeyCode = targetDeviceKeyCode;
		inputKeyMap.insert(InputKeyMapEntry(mapEntry.keyCode, mapEntry));
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
void System::UnmapKeyCode(IDeviceContext::KeyCode keyCode)
{
	boost::mutex::scoped_lock lock(inputMutex);
	inputKeyMap.erase(keyCode);
}

//----------------------------------------------------------------------------------------
void System::UnmapKeyCode(IDevice* targetDevice, unsigned int targetDeviceKeyCode)
{
	boost::mutex::scoped_lock lock(inputMutex);
	InputKeyMap::iterator inputKeyMapIterator = inputKeyMap.begin();
	while(inputKeyMapIterator != inputKeyMap.end())
	{
		const InputMapEntry& inputMapEntry = inputKeyMapIterator->second;
		if((inputMapEntry.targetDevice == targetDevice) && (inputMapEntry.targetDeviceKeyCode == targetDeviceKeyCode))
		{
			inputKeyMap.erase(inputKeyMapIterator);
			return;
		}
		++inputKeyMapIterator;
	}
}

//----------------------------------------------------------------------------------------
void System::ClearKeyCodeMap()
{
	boost::mutex::scoped_lock lock(inputMutex);
	inputKeyMap.clear();
}

//----------------------------------------------------------------------------------------
void System::HandleInputKeyDown(IDeviceContext::KeyCode keyCode)
{
	boost::mutex::scoped_lock lock(inputMutex);
	if(!systemStopped)
	{
		InputKeyMap::const_iterator i = inputKeyMap.find(keyCode);
		if(i != inputKeyMap.end())
		{
			InputEventEntry entry;
			entry.sent = false;
			entry.inputEvent = INPUTEVENT_KEYDOWN;
			entry.keyCode = keyCode;
			inputEvents.push_back(entry);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::HandleInputKeyUp(IDeviceContext::KeyCode keyCode)
{
	boost::mutex::scoped_lock lock(inputMutex);
	if(!systemStopped)
	{
		InputKeyMap::const_iterator i = inputKeyMap.find(keyCode);
		if(i != inputKeyMap.end())
		{
			InputEventEntry entry;
			entry.sent = false;
			entry.inputEvent = INPUTEVENT_KEYUP;
			entry.keyCode = keyCode;
			inputEvents.push_back(entry);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::SendStoredInputEvents()
{
	boost::mutex::scoped_lock lock(inputMutex);
	for(std::list<InputEventEntry>::iterator i = inputEvents.begin(); i != inputEvents.end(); ++i)
	{
		i->sent = true;
		InputKeyMap::const_iterator keyMapEntry = inputKeyMap.find(i->keyCode);
		if(keyMapEntry != inputKeyMap.end())
		{
			switch(i->inputEvent)
			{
			case INPUTEVENT_KEYDOWN:
				keyMapEntry->second.targetDevice->HandleInputKeyDown(keyMapEntry->second.targetDeviceKeyCode);
				break;
			case INPUTEVENT_KEYUP:
				keyMapEntry->second.targetDevice->HandleInputKeyUp(keyMapEntry->second.targetDeviceKeyCode);
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void System::ClearSentStoredInputEvents()
{
	boost::mutex::scoped_lock lock(inputMutex);
	std::list<InputEventEntry>::iterator i = inputEvents.begin();
	while((i != inputEvents.end()) && i->sent)
	{
		++i;
	}
	inputEvents.erase(inputEvents.begin(), i);
}

//----------------------------------------------------------------------------------------
void System::UnmapAllKeyCodeMappingsForDevice(IDevice* adevice)
{
	boost::mutex::scoped_lock lock(inputMutex);
	std::list<IDeviceContext::KeyCode> keyCodesToRemove;
	for(InputKeyMap::iterator i = inputKeyMap.begin(); i != inputKeyMap.end(); ++i)
	{
		if(i->second.targetDevice == adevice)
		{
			keyCodesToRemove.push_back(i->second.keyCode);
		}
	}
	for(std::list<IDeviceContext::KeyCode>::const_iterator i = keyCodesToRemove.begin(); i != keyCodesToRemove.end(); ++i)
	{
		inputKeyMap.erase(*i);
	}
}

//----------------------------------------------------------------------------------------
//System setting functions
//----------------------------------------------------------------------------------------
bool System::ApplySystemStateChange(const SystemStateChange& stateChange)
{
	//Apply the specified system state change
	switch(stateChange.type)
	{
	case SYSTEMSTATECHANGETYPE_SETCLOCKFREQUENCY:{
		//Retrieve the target clock source
		ClockSource* clockSource = GetClockSource(stateChange.moduleID, stateChange.targetElementName);
		if(clockSource == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
			logEntry << L"Could not locate clock source with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Apply the clock rate change
		switch(stateChange.setClockRateClockType)
		{
		case ClockSource::CLOCKTYPE_DIRECT:
			return clockSource->TransparentSetClockFrequency(stateChange.setClockRateValue);
		case ClockSource::CLOCKTYPE_DIVIDER:
			return clockSource->TransparentSetClockDivider(stateChange.setClockRateValue);
		case ClockSource::CLOCKTYPE_MULTIPLIER:
			return clockSource->TransparentSetClockMultiplier(stateChange.setClockRateValue);
		default:
			LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
			logEntry << L"An attempt was made to change a system clock rate using an invalid clock type flag with a value of \"" << stateChange.setClockRateClockType << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}
		break;}
	case SYSTEMSTATECHANGETYPE_SETSYSTEMLINESTATE:{
		//Retrieve the target system line ID
		unsigned int systemLineID = GetSystemLineID(stateChange.moduleID, stateChange.targetElementName);
		if(systemLineID == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
			logEntry << L"Could not locate system line with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Retrieve the width of the target system line
		unsigned int systemLineWidth = GetSystemLineWidth(systemLineID);

		//Apply the line state change
		return SetSystemLineState(systemLineID, Data(systemLineWidth, stateChange.setLineStateValue));}
	case SYSTEMSTATECHANGETYPE_SETSYSTEMOPTION:{
		//Retrieve the target system setting ID
		unsigned int systemSettingID = GetSystemSettingID(stateChange.moduleID, stateChange.targetElementName);
		if(systemSettingID == 0)
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
			logEntry << L"Could not locate system setting with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Retrieve the target system setting
		SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(systemSettingID);
		if(systemSettingsIterator == systemSettings.end())
		{
			LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
			logEntry << L"Failed to retrieve system setting info for system setting with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}
		SystemSettingInfo& systemSettingInfo = systemSettingsIterator->second;

		//Attempt to locate the target option, and apply that option if it is found.
		for(unsigned int i = 0; i < systemSettingInfo.options.size(); ++i)
		{
			if(systemSettingInfo.options[i].name == stateChange.setSystemOptionValue)
			{
				//Replace the current option selection
				if(systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry != 0)
				{
					systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry->SetCheckedState(false);
				}
				systemSettingInfo.selectedOption = i;
				if(systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry != 0)
				{
					systemSettingInfo.options[systemSettingInfo.selectedOption].menuItemEntry->SetCheckedState(true);
				}

				//Attempt to apply all settings changes listed under this option
				bool appliedWithoutErrors = true;
				SystemSettingOption& settingOption = systemSettingInfo.options[systemSettingInfo.selectedOption];
				for(std::list<SystemStateChange>::const_iterator i = settingOption.stateChanges.begin(); i != settingOption.stateChanges.end(); ++i)
				{
					//Apply this system state change
					if(!ApplySystemStateChange(*i))
					{
						WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Failed to apply system setting change while setting option \"" + stateChange.setSystemOptionValue + L"\" on setting \"" + systemSettingInfo.name + L"\" in System::ApplySystemStateChange!"));
						appliedWithoutErrors = false;
					}
				}

				//Return the result of this settings change operation
				return appliedWithoutErrors;
			}
		}

		//If we failed to find the target option, log the error, and return false.
		LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
		logEntry << L"Could not locate option with name \"" << stateChange.setSystemOptionValue << L"\" under setting with name \"" << systemSettingInfo.name << L"\" on module with ID \"" << systemSettingInfo.moduleID << L"\" in System::ApplySystemStateChange!";
		WriteLogEvent(logEntry);
		return false;}
	default:
		LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
		logEntry << L"An attempt was made to apply an invalid system state change with a value of \"" << stateChange.type << L"\" in System::ApplySystemStateChange!";
		WriteLogEvent(logEntry);
		return false;
	}
}

//----------------------------------------------------------------------------------------
//System line functions
//----------------------------------------------------------------------------------------
unsigned int System::GetSystemLineWidth(unsigned int systemLineID) const
{
	//Retrieve the info for this system line
	SystemLineMap::const_iterator systemLinesIterator = systemLines.find(systemLineID);
	if(systemLinesIterator == systemLines.end())
	{
		return 0;
	}
	const SystemLineInfo& systemLineInfo = systemLinesIterator->second;

	//Return the width of this system line
	return systemLineInfo.lineWidth;
}

//----------------------------------------------------------------------------------------
bool System::SetSystemLineState(unsigned int systemLineID, const Data& lineData)
{
	//Retrieve the info for this system line
	SystemLineMap::iterator systemLinesIterator = systemLines.find(systemLineID);
	if(systemLinesIterator == systemLines.end())
	{
		LogEntry logEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"");
		logEntry << L"An attempt was made to change a system line using an invalid system line ID with a value of \"" << systemLineID << L"\" in System::SetSystemLineState!";
		WriteLogEvent(logEntry);
		return false;
	}
	SystemLineInfo& systemLineInfo = systemLinesIterator->second;

	//Record the new value for the system line
	systemLineInfo.currentValue = lineData.GetData();

	//Propagate the line state change to all devices which import this system line
	for(SystemLineMappingList::const_iterator i = systemLineMappings.begin(); i != systemLineMappings.end(); ++i)
	{
		const SystemLineMapping& lineMapping = *i;
		if(lineMapping.systemLineID == systemLineID)
		{
			Data tempData(lineMapping.targetLineBitCount, lineData.GetData());
			if(lineMapping.remapLines)
			{
				//Remap lines
				tempData = lineMapping.lineRemapTable.ConvertTo(lineData.GetData());
			}
			tempData &= lineMapping.lineMaskAND;
			tempData |= lineMapping.lineMaskOR;
			tempData ^= lineMapping.lineMaskXOR;
			lineMapping.targetDevice->TransparentSetLineState(lineMapping.targetLine, tempData);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Assembly handle functions
//----------------------------------------------------------------------------------------
void* System::GetAssemblyHandle() const
{
	return assemblyHandle;
}
