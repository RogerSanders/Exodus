#include "System.h"
#include "HierarchicalStorage/HierarchicalStorage.pkg"
#include "Stream/Stream.pkg"
#include "ZIP/ZIP.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Image/Image.pkg"
#include <time.h>
#include <functional>
#include <thread>
#include <sstream>
#include <algorithm>
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::System(IGUIExtensionInterface& aguiExtensionInterface)
:guiExtensionInterface(aguiExtensionInterface), stopSystem(false), systemStopped(true), initialize(true), rollback(false), performingSingleDeviceStep(false), enableThrottling(true), runWhenProgramModuleLoaded(true), enablePersistentState(true)
{
	eventLogSize = 500;
	eventLogLastModifiedToken = 0;

	embeddedROMInfoLastModifiedToken = 0;

	inputDeviceListLastModifiedToken = 0;

	nextFreeModuleID = 0;
	nextFreeConnectorID = 1000;
	nextFreeLineGroupID = 2000;
	nextFreeSystemLineID = 3000;
	nextFreeSystemSettingID = 4000;
	nextFreeEmbeddedROMID = 5000;
}

//----------------------------------------------------------------------------------------
System::~System()
{
	//Unload all currently loaded modules
	UnloadAllModules();

	//Unload all persistent global extensions. Persistent extensions should be all that is
	//left in the list of global extensions at this point.
	for(LoadedGlobalExtensionInfoList::const_iterator i = globalExtensionInfoList.begin(); i != globalExtensionInfoList.end(); ++i)
	{
		UnloadExtension(i->second.extension);
	}
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int System::GetISystemDeviceInterfaceVersion() const
{
	return ThisISystemDeviceInterfaceVersion();
}

//----------------------------------------------------------------------------------------
unsigned int System::GetISystemExtensionInterfaceVersion() const
{
	return ThisISystemExtensionInterfaceVersion();
}

//----------------------------------------------------------------------------------------
unsigned int System::GetISystemGUIInterfaceVersion() const
{
	return ThisISystemGUIInterfaceVersion();
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
bool System::LoadState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Open the target file
	FileStreamReference sourceStreamReference(guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the file could not be opened!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}
	Stream::IStream& source = *sourceStreamReference;

	HierarchicalStorageTree tree;
	if(fileType == FileType::ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the zip file structure could not be decoded!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the save.xml file could not be found within the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because there was an error decompressing the save.xml file from the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Load external binary data into the XML tree
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			ZIPFileEntry* entry = archive.GetFileEntry(binaryFileName);
			if(entry == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found within the zip archive!"));
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
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because there was an error decompressing the binary data file " + binaryFileName + L" from the zip archive!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
	}
	else if(fileType == FileType::XML)
	{
		//Determine the text format for the target file
		source.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		source.ProcessByteOrderMark();

		//Attempt to load the XML tree from the file
		if(!tree.LoadTree(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Load external binary data into the XML tree
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
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
			FileStreamReference binaryFileStreamReference(guiExtensionInterface);
			if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
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
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!binaryData.WriteData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
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
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"State")
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the root node in the XML tree wasn't of type \"State\"!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}

	//Restore system state from XML data
	ModuleRelationshipMap relationshipMap;
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring elementName = (*i)->GetName();

		//Load the device node
		if(elementName == L"Device")
		{
			//Extract the mandatory attributes
			IHierarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"Name");
			IHierarchicalStorageAttribute* moduleIDAttribute = (*i)->GetAttribute(L"ModuleID");
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
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"While loading state data from file " + filePath + L" state data was found for device " + deviceName + L" , which could not be located in the system. The state data for this device will be ignored, and the state will continue to load, but note that the system may not run as expected."));
				}
			}
		}
		//Load the ModuleRelationships node
		else if(elementName == L"ModuleRelationships")
		{
			if(!LoadModuleRelationshipsNode(*(*i), relationshipMap))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load state from file " + filePath + L" because the ModuleRelationships node could not be loaded!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized element: " + elementName + L" when loading state from file " + filePath + L"."));
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Loaded state from file " + filePath));

	//Restore running state
	if(running)
	{
		RunSystem();
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool System::SaveState(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType, bool debuggerState)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Create the new savestate XML tree
	HierarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"State");

	//Fill in general information about the savestate
	IHierarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
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
			screenshotPresent |= (*i)->GetTargetDevice().GetScreenshot(screenshot);
		}
		if(screenshotPresent)
		{
			stateInfo.CreateAttribute(L"Screenshot", screenshotFilename);
		}
	}

	//Save the ModuleRelationships node
	IHierarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	SaveModuleRelationshipsNode(moduleRelationshipsNode);

	//Save the system state to the XML tree
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		IHierarchicalStorageNode& node = tree.GetRootNode().CreateChild(L"Device");
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

	if(fileType == FileType::ZIP)
	{
		//Save the XML tree to a unicode buffer
		Stream::Buffer buffer(Stream::IStream::TextEncoding::UTF8, 0);
		buffer.InsertByteOrderMark();
		if(!tree.SaveTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the save.xml file!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		archive.AddFileEntry(entry);

		//Save external binary data to separate files
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			ZIPFileEntry entry;
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			entry.SetFileName(binaryFileName);
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry.Compress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the " + binaryFileName + L" file!"));
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
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the screenshot file with a file name of " + screenshotFilename + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			screenshotFile.SetStreamPos(0);
			if(!entry.Compress(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error compressing the " + screenshotFilename + L" file!"));
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
		if(!target.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the zip structure to the file!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
	}
	else if(fileType == FileType::XML)
	{
		//Save XML tree to the target file
		Stream::File file(Stream::IStream::TextEncoding::UTF8);
		if(!file.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		file.InsertByteOrderMark();
		if(!tree.SaveTree(file))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Save external binary data to separate files
		std::wstring fileName = PathGetFileName(filePath);
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = fileName + L" - " + (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = PathCombinePaths(fileDir, binaryFileName);
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the binary data file " + binaryFileName + L" at the full path of " + binaryFilePath + L"!"));
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
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error reading the source data from memory to save to the binary data file " + binaryFileName + L"!"));
					if(running)
					{
						RunSystem();
					}
					return false;
				}
				if(!binaryFile.WriteData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error writing to the binary data file " + binaryFileName + L"!"));
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
			if(!screenshotFile.Open(screenshotFilePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error creating the screenshot file with a file name of " + screenshotFilenameFull + L" with a full path of " + screenshotFilePath + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!screenshot.SavePNGImage(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save state to file " + filePath + L" because there was an error saving the screenshot to the " + screenshotFilenameFull + L" file!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Saved state to file " + filePath));

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
	FileStreamReference sourceStreamReference(guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		if(returnSuccessOnNoFilePresent)
		{
			return true;
		}
		else
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the file could not be opened!"));
			return false;
		}
	}
	Stream::IStream& source = *sourceStreamReference;

	HierarchicalStorageTree tree;
	if(fileType == FileType::ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the zip file structure could not be decoded!"));
			return false;
		}

		//Load XML tree from file
		ZIPFileEntry* entry = archive.GetFileEntry(L"save.xml");
		if(entry == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the save.xml file could not be found within the zip archive!"));
			return false;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error decompressing the save.xml file from the zip archive!"));
			return false;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Load external binary data into the XML tree
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			ZIPFileEntry* entry = archive.GetFileEntry(binaryFileName);
			if(entry == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found within the zip archive!"));
				return false;
			}
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry->Decompress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error decompressing the binary data file " + binaryFileName + L" from the zip archive!"));
				return false;
			}
		}
	}
	else if(fileType == FileType::XML)
	{
		//Determine the text format for the target file
		source.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		source.ProcessByteOrderMark();

		//Attempt to load the XML tree from the file
		if(!tree.LoadTree(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Load external binary data into the XML tree
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
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
			FileStreamReference binaryFileStreamReference(guiExtensionInterface);
			if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
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
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
				return false;
			}
			if(!binaryData.WriteData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
				return false;
			}
			delete[] buffer;
		}
	}

	//Validate the root node
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"PersistentState")
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load persistent state from file " + filePath + L" because the root node in the XML tree wasn't of type \"PersistentState\"!"));
		return false;
	}

	//Restore persistent system state from XML data
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		std::wstring elementName = (*i)->GetName();

		//Load the device node
		if(elementName == L"Device")
		{
			//Extract the mandatory attributes
			IHierarchicalStorageAttribute* nameAttribute = (*i)->GetAttribute(L"Name");
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
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"While loading persistent state data from file " + filePath + L" state data was found for device " + deviceName + L" , which could not be located in the system. The state data for this device will be ignored, and the state will continue to load, but note that the system may not run as expected."));
				}
			}
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized element: " + elementName + L" when loading persistent state from file " + filePath + L"."));
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Loaded persistent state from file " + filePath));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::SavePersistentStateForModule(const std::wstring& filePath, unsigned int moduleID, FileType fileType, bool generateNoFileIfNoContentPresent)
{
	//Create the new savestate XML tree
	HierarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"PersistentState");

	//Save the system state to the XML tree
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(i->moduleID == moduleID)
		{
			IHierarchicalStorageNode& deviceNode = tree.GetRootNode().CreateChild(L"Device");
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
	IHierarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
	Timestamp timestamp = GetTimestamp();
	stateInfo.CreateAttribute(L"CreationDate", timestamp.GetDate());
	stateInfo.CreateAttribute(L"CreationTime", timestamp.GetTime());

	if(fileType == FileType::ZIP)
	{
		//Save the XML tree to a unicode buffer
		Stream::Buffer buffer(Stream::IStream::TextEncoding::UTF8, 0);
		buffer.InsertByteOrderMark();
		if(!tree.SaveTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		ZIPArchive archive;
		ZIPFileEntry entry;
		entry.SetFileName(L"save.xml");
		buffer.SetStreamPos(0);
		if(!entry.Compress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error compressing the save.xml file!"));
			return false;
		}
		archive.AddFileEntry(entry);

		//Save external binary data to separate files
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			ZIPFileEntry entry;
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			entry.SetFileName(binaryFileName);
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			if(!entry.Compress(binaryData))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error compressing the " + binaryFileName + L" file!"));
				return false;
			}
			archive.AddFileEntry(entry);
		}

		//Create the target file
		Stream::File target;
		if(!target.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the zip structure to the file!"));
			return false;
		}
	}
	else if(fileType == FileType::XML)
	{
		//Save XML tree to the target file
		Stream::File file(Stream::IStream::TextEncoding::UTF8);
		if(!file.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			return false;
		}
		file.InsertByteOrderMark();
		if(!tree.SaveTree(file))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			return false;
		}

		//Save external binary data to separate files
		std::wstring fileName = PathGetFileName(filePath);
		std::wstring fileDir = PathGetDirectory(filePath);
		std::list<IHierarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = fileName + L" - " + (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = PathCombinePaths(fileDir, binaryFileName);
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error creating the binary data file " + binaryFileName + L" at the full path of " + binaryFilePath + L"!"));
				return false;
			}

			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);
			while(!binaryData.IsAtEnd())
			{
				unsigned char temp;
				if(!binaryData.ReadData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error reading the source data from memory to save to the binary data file " + binaryFileName + L"!"));
					return false;
				}
				if(!binaryFile.WriteData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save persistent state to file " + filePath + L" because there was an error writing to the binary data file " + binaryFileName + L"!"));
					return false;
				}
			}
		}
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Saved persistent state to file " + filePath));

	return true;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<System::StateInfo> System::GetStateInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, FileType fileType) const
{
	StateInfo stateInfo;
	stateInfo.valid = false;

	//Open the target file
	FileStreamReference sourceStreamReference(guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		return stateInfo;
	}
	Stream::IStream& source = *sourceStreamReference;

	HierarchicalStorageTree tree;
	if(fileType == FileType::ZIP)
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
		buffer.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			return stateInfo;
		}
	}
	else if(fileType == FileType::XML)
	{
		source.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
		source.ProcessByteOrderMark();
		if(!tree.LoadTree(source))
		{
			return stateInfo;
		}
	}

	//Load savestate info from XML data
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() == L"State")
	{
		stateInfo.valid = true;
		std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
		bool foundStateInfo = false;
		std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin();
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
bool System::LoadSavedRelationshipMap(IHierarchicalStorageNode& node, SavedRelationshipMap& relationshipMap) const
{
	//Validate the name of the node
	if(node.GetName() != L"ModuleRelationships")
	{
		return false;
	}

	//Load the saved module relationship data
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"Module")
		{
			//Extract the mandatory attributes
			IHierarchicalStorageAttribute* moduleIDAttribute = (*i)->GetAttribute(L"ModuleID");
			IHierarchicalStorageAttribute* systemClassNameAttribute = (*i)->GetAttribute(L"SystemClassName");
			IHierarchicalStorageAttribute* moduleClassNameAttribute = (*i)->GetAttribute(L"ModuleClassName");
			IHierarchicalStorageAttribute* moduleInstanceNameAttribute = (*i)->GetAttribute(L"ModuleInstanceName");
			if((moduleIDAttribute != 0) && (systemClassNameAttribute != 0) && (moduleClassNameAttribute != 0) && (moduleInstanceNameAttribute != 0))
			{
				//Extract info on this module
				SavedRelationshipModule savedData;
				savedData.moduleID = moduleIDAttribute->ExtractValue<unsigned int>();
				savedData.systemClassName = systemClassNameAttribute->GetValue();
				savedData.className = moduleClassNameAttribute->GetValue();
				savedData.instanceName = moduleInstanceNameAttribute->GetValue();
				IHierarchicalStorageAttribute* filePathAttribute = (*i)->GetAttribute(L"FilePath");
				if(filePathAttribute != 0)
				{
					savedData.filePath = filePathAttribute->GetValue();
				}

				//Extract any connector info
				std::list<IHierarchicalStorageNode*> moduleChildList = (*i)->GetChildList();
				for(std::list<IHierarchicalStorageNode*>::const_iterator moduleChildIterator = moduleChildList.begin(); moduleChildIterator != moduleChildList.end(); ++moduleChildIterator)
				{
					if((*moduleChildIterator)->GetName() == L"ExportConnector")
					{
						//Extract the mandatory attributes
						IHierarchicalStorageAttribute* connectorClassNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorClassName");
						IHierarchicalStorageAttribute* connectorInstanceNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceName");
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
						IHierarchicalStorageAttribute* connectorModuleIDAttribute = (*moduleChildIterator)->GetAttribute(L"ExportingModuleID");
						IHierarchicalStorageAttribute* connectorClassNameAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorClassName");
						IHierarchicalStorageAttribute* connectorInstanceNameLocalAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceNameLocal");
						IHierarchicalStorageAttribute* connectorInstanceNameRemoteAttribute = (*moduleChildIterator)->GetAttribute(L"ConnectorInstanceNameRemote");
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
bool System::LoadModuleRelationshipsNode(IHierarchicalStorageNode& node, const MarshalSupport::Marshal::Out<ModuleRelationshipMap>& relationshipMap) const
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
	ModuleRelationshipMap relationshipMapTemp;
	for(SavedRelationshipMap::const_iterator i = savedRelationshipData.begin(); i != savedRelationshipData.end(); ++i)
	{
		//Look for a loaded module which matches the saved module data
		bool foundMatchingModule = false;
		const SavedRelationshipModule& savedModuleInfo = i->second;
		LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.begin();
		while(!foundMatchingModule && (loadedModuleIterator != loadedModuleInfoMap.end()))
		{
			//Check if these modules match
			const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;
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
				relationshipMapTemp.insert(ModuleRelationshipEntry(moduleRelationship.savedModuleID, moduleRelationship));
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
			relationshipMapTemp.insert(ModuleRelationshipEntry(moduleRelationship.savedModuleID, moduleRelationship));
		}
	}
	relationshipMap = relationshipMapTemp;

	return true;
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsNode(IHierarchicalStorageNode& node, bool saveFilePathInfo, const MarshalSupport::Marshal::In<std::wstring>& relativePathBase) const
{
	for(LoadedModuleInfoMap::const_iterator i = loadedModuleInfoMap.begin(); i != loadedModuleInfoMap.end(); ++i)
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = i->second;
		IHierarchicalStorageNode& moduleNode = node.CreateChild(L"Module");
		moduleNode.CreateAttribute(L"ModuleID").SetValue(loadedModuleInfo.moduleID);
		moduleNode.CreateAttribute(L"SystemClassName", loadedModuleInfo.systemClassName);
		moduleNode.CreateAttribute(L"ModuleClassName", loadedModuleInfo.className);
		moduleNode.CreateAttribute(L"ModuleInstanceName", loadedModuleInfo.instanceName);
		if(saveFilePathInfo)
		{
			//Convert the file directory to a relative path from the base directory, if
			//the target directory is the same or contained within the target path.
			std::wstring fileDir = PathGetDirectory(loadedModuleInfo.filePath);
			if(PathStartsWithBasePath(relativePathBase, fileDir))
			{
				std::wstring relativePath;
				if(PathBuildRelativePathToTarget(relativePathBase, fileDir, false, relativePath))
				{
					fileDir = relativePath;
				}
			}

			std::wstring filePath = PathCombinePaths(fileDir, PathGetFileName(loadedModuleInfo.filePath));
			moduleNode.CreateAttribute(L"FilePath", filePath);
		}
		SaveModuleRelationshipsExportConnectors(moduleNode, loadedModuleInfo.moduleID);
		SaveModuleRelationshipsImportConnectors(moduleNode, loadedModuleInfo.moduleID);
	}
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsExportConnectors(IHierarchicalStorageNode& moduleNode, unsigned int moduleID) const
{
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		if(i->second.exportingModuleID == moduleID)
		{
			IHierarchicalStorageNode& exportNode = moduleNode.CreateChild(L"ExportConnector");
			exportNode.CreateAttribute(L"ConnectorClassName", i->second.connectorClassName);
			exportNode.CreateAttribute(L"ConnectorInstanceName", i->second.exportingModuleConnectorInstanceName);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::SaveModuleRelationshipsImportConnectors(IHierarchicalStorageNode& moduleNode, unsigned int moduleID) const
{
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		if(i->second.connectorUsed && (i->second.importingModuleID == moduleID))
		{
			IHierarchicalStorageNode& exportNode = moduleNode.CreateChild(L"ImportConnector");
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
						//connector, and recursively validate that the exporting module,
						//and all modules it is dependent on, match their corresponding
						//saved module info.
						LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.find(connectorInfo.GetExportingModuleID());
						if(loadedModuleIterator != loadedModuleInfoMap.end())
						{
							const LoadedModuleInfoInternal& loadedModuleInfoForExportingModule = loadedModuleIterator->second;
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
	std::unique_lock<std::mutex> lock(eventLogMutex);
	SystemLogEntry logEntryInternal;
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
	++eventLogLastModifiedToken;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::vector<System::SystemLogEntry>> System::GetEventLog() const
{
	std::unique_lock<std::mutex> lock(eventLogMutex);
	std::vector<SystemLogEntry> eventLogCopy(log.size());
	unsigned int eventLogCopyIndex = 0;
	for(std::list<SystemLogEntry>::const_iterator i = log.begin(); i != log.end(); ++i)
	{
		eventLogCopy[eventLogCopyIndex++] = *i;
	}
	return eventLogCopy;
}

//----------------------------------------------------------------------------------------
unsigned int System::GetEventLogLastModifiedToken() const
{
	std::unique_lock<std::mutex> lock(eventLogMutex);
	return eventLogLastModifiedToken;
}

//----------------------------------------------------------------------------------------
void System::ClearEventLog()
{
	std::unique_lock<std::mutex> lock(eventLogMutex);
	log.clear();
	++eventLogLastModifiedToken;
}

//----------------------------------------------------------------------------------------
unsigned int System::GetEventLogSize() const
{
	std::unique_lock<std::mutex> lock(eventLogMutex);
	return eventLogSize;
}

//----------------------------------------------------------------------------------------
void System::SetEventLogSize(unsigned int alogSize)
{
	std::unique_lock<std::mutex> lock(eventLogMutex);
	eventLogSize = alogSize;
	while(log.size() > eventLogSize)
	{
		log.pop_back();
	}
	++eventLogLastModifiedToken;
}

//----------------------------------------------------------------------------------------
//Embedded ROM functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<unsigned int>> System::GetEmbeddedROMIDs() const
{
	std::unique_lock<std::mutex> lock(embeddedROMMutex);
	std::list<unsigned int> embeddedROMIDList;
	for(std::map<unsigned int, EmbeddedROMInfoInternal>::const_iterator i = embeddedROMInfoSet.begin(); i != embeddedROMInfoSet.end(); ++i)
	{
		embeddedROMIDList.push_back(i->first);
	}
	return embeddedROMIDList;
}

//----------------------------------------------------------------------------------------
unsigned int System::GetEmbeddedROMInfoLastModifiedToken() const
{
	std::unique_lock<std::mutex> lock(embeddedROMMutex);
	return embeddedROMInfoLastModifiedToken;
}

//----------------------------------------------------------------------------------------
bool System::GetEmbeddedROMInfo(unsigned int embeddedROMID, IEmbeddedROMInfo& embeddedROMInfo) const
{
	//Attempt to locate the target embedded ROM entry
	std::unique_lock<std::mutex> lock(embeddedROMMutex);
	std::map<unsigned int, EmbeddedROMInfoInternal>::const_iterator embeddedROMInfoIterator = embeddedROMInfoSet.find(embeddedROMID);
	if(embeddedROMInfoIterator == embeddedROMInfoSet.end())
	{
		return false;
	}
	const EmbeddedROMInfoInternal& targetEmbeddedROMInfo = embeddedROMInfoIterator->second;

	//Return info on the target embedded ROM entry to the caller
	embeddedROMInfo.SetName(targetEmbeddedROMInfo.embeddedROMName);
	embeddedROMInfo.SetDisplayName(targetEmbeddedROMInfo.displayName);
	embeddedROMInfo.SetModuleID(targetEmbeddedROMInfo.moduleID);
	embeddedROMInfo.SetTargetDevice(targetEmbeddedROMInfo.targetDevice);
	embeddedROMInfo.SetTargetInterfaceNo(targetEmbeddedROMInfo.interfaceNumber);
	embeddedROMInfo.SetROMRegionSize(targetEmbeddedROMInfo.romRegionSize);
	embeddedROMInfo.SetROMEntryBitCount(targetEmbeddedROMInfo.romEntryBitCount);
	embeddedROMInfo.SetFilePath(targetEmbeddedROMInfo.filePath);
	return true;
}


//----------------------------------------------------------------------------------------
bool System::SetEmbeddedROMPath(unsigned int embeddedROMID, const MarshalSupport::Marshal::In<std::wstring>& filePath)
{
	//Attempt to locate the target embedded ROM entry
	std::unique_lock<std::mutex> lock(embeddedROMMutex);
	std::map<unsigned int, EmbeddedROMInfoInternal>::iterator embeddedROMInfoIterator = embeddedROMInfoSet.find(embeddedROMID);
	if(embeddedROMInfoIterator == embeddedROMInfoSet.end())
	{
		return false;
	}
	EmbeddedROMInfoInternal& targetEmbeddedROMInfo = embeddedROMInfoIterator->second;

	//Update the target file path for the embedded ROM info
	targetEmbeddedROMInfo.filePath = filePath;

	//Update the last modified token for embedded ROM data
	++embeddedROMInfoLastModifiedToken;

	//Reload the embedded ROM data for the target entry
	ReloadEmbeddedROMData(targetEmbeddedROMInfo);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::ReloadEmbeddedROMData(unsigned int embeddedROMID)
{
	//Attempt to locate the target embedded ROM entry
	std::unique_lock<std::mutex> lock(embeddedROMMutex);
	std::map<unsigned int, EmbeddedROMInfoInternal>::const_iterator embeddedROMInfoIterator = embeddedROMInfoSet.find(embeddedROMID);
	if(embeddedROMInfoIterator == embeddedROMInfoSet.end())
	{
		return false;
	}
	const EmbeddedROMInfoInternal& targetEmbeddedROMInfo = embeddedROMInfoIterator->second;

	//Reload the embedded ROM data for the target entry
	ReloadEmbeddedROMData(targetEmbeddedROMInfo);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::ReloadEmbeddedROMData(const EmbeddedROMInfoInternal& targetEmbeddedROMInfo)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Attempt to load this ROM data into the target device
	Stream::File file;
	bool result = file.Open(targetEmbeddedROMInfo.filePath, Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open);
	if(result)
	{
		Data romDataEntry(targetEmbeddedROMInfo.romEntryBitCount);
		Stream::ViewBinary viewBinary(file);
		unsigned int deviceAddress = 0;
		while(!viewBinary.IsAtEnd() && viewBinary.NoErrorsOccurred() && (deviceAddress < targetEmbeddedROMInfo.romRegionSize))
		{
			viewBinary >> romDataEntry;
			targetEmbeddedROMInfo.targetDevice->TransparentWriteInterface(targetEmbeddedROMInfo.interfaceNumber, deviceAddress, romDataEntry, 0, 0);
			++deviceAddress;
		}
	}

	//Restore the running state
	if(running)
	{
		RunSystem();
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Module setting functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<unsigned int>> System::GetModuleSettingIDs(unsigned int moduleID) const
{
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	std::list<unsigned int> moduleSettingIDList;
	ModuleSystemSettingMap::const_iterator moduleSettingsIterator = moduleSettings.find(moduleID);
	if(moduleSettingsIterator != moduleSettings.end())
	{
		moduleSettingIDList = moduleSettingsIterator->second;
	}
	return moduleSettingIDList;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleSettingInfo(unsigned int moduleID, unsigned int moduleSettingID, IModuleSettingInfo& moduleSettingInfo) const
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::const_iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return false;
	}
	const SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return false;
	}

	//Return info on the target module setting to the caller
	moduleSettingInfo.SetName(systemSettingInfo.name);
	moduleSettingInfo.SetDisplayName(systemSettingInfo.displayName.empty()? systemSettingInfo.name: systemSettingInfo.displayName);
	moduleSettingInfo.SetOptionCount((unsigned int)systemSettingInfo.options.size());
	moduleSettingInfo.SetDefaultOptionIndex(systemSettingInfo.defaultOption);
	moduleSettingInfo.SetToggleSettingOnOptionIndex(systemSettingInfo.onOption);
	moduleSettingInfo.SetToggleSettingOffOptionIndex(systemSettingInfo.offOption);
	moduleSettingInfo.SetIsToggleSetting(systemSettingInfo.toggleSetting);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleSettingOptionInfo(unsigned int moduleID, unsigned int moduleSettingID, unsigned int moduleSettingOptionIndex, IModuleSettingOptionInfo& moduleSettingOptionInfo) const
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::const_iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return false;
	}
	const SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return false;
	}

	//Locate the target option within this setting
	if(moduleSettingOptionIndex >= (unsigned int)systemSettingInfo.options.size())
	{
		return false;
	}
	const SystemSettingOption& settingOptionInfo = systemSettingInfo.options[moduleSettingOptionIndex];

	//Return info on the target option to the caller
	moduleSettingOptionInfo.SetName(settingOptionInfo.name);
	moduleSettingOptionInfo.SetDisplayName(settingOptionInfo.displayName.empty()? settingOptionInfo.name: settingOptionInfo.displayName);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int& activeModuleOptionIndex) const
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::const_iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return false;
	}
	const SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return false;
	}

	//Return the index of the currently active option for this setting to the caller
	activeModuleOptionIndex = systemSettingInfo.selectedOption;
	return true;
}

//----------------------------------------------------------------------------------------
bool System::SetModuleSettingActiveOptionIndex(unsigned int moduleID, unsigned int moduleSettingID, unsigned int activeOptionIndex)
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return false;
	}
	SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return false;
	}

	//Ensure the target option index is valid
	if(activeOptionIndex >= (unsigned int)systemSettingInfo.options.size())
	{
		return false;
	}

	//If the target option is already the selected option, abort any further processing.
	if(systemSettingInfo.selectedOption == activeOptionIndex)
	{
		return true;
	}

	//Save the current system running state, and stop the system.
	bool systemRunningState = SystemRunning();
	StopSystem();

	//Replace the current option selection
	if(!systemSettingInfo.toggleSetting || !systemSettingInfo.toggleSettingAutoRevert)
	{
		systemSettingInfo.selectedOption = activeOptionIndex;
	}

	//If requested, execute a lead-in time before applying the setting change.
	if(systemSettingInfo.settingChangeLeadInTime > 0)
	{
		//Calculate the time to execute until the setting should be changed. If a random
		//time has been requested, we generate a random number between the specified
		//minimum and maximum time constraints.
		double timeTillSettingChange = systemSettingInfo.settingChangeLeadInTime;
		if(systemSettingInfo.settingChangeLeadInTimeRandom)
		{
			timeTillSettingChange = (double)rand() * ((systemSettingInfo.settingChangeLeadInTimeEnd - systemSettingInfo.settingChangeLeadInTime) / (double)RAND_MAX);
		}

		//Execute the system up to the setting change time
		ExecuteSystemStep(timeTillSettingChange);
	}

	//Attempt to apply the new option selection
	bool result = true;
	SystemSettingOption& settingOption = systemSettingInfo.options[activeOptionIndex];
	for(std::list<SystemStateChange>::const_iterator i = settingOption.stateChanges.begin(); i != settingOption.stateChanges.end(); ++i)
	{
		result &= ApplySystemStateChange(*i);
	}

	//Notify any registered observers about the option change
	systemSettingInfo.settingChangeObservers.NotifyObservers();

	//If this is an auto-reverting toggle option, advance until the required time, and
	//revert to the previous setting.
	if(systemSettingInfo.toggleSettingAutoRevert)
	{
		//Calculate the time to execute until the setting should be reverted. If a random
		//time has been requested, we generate a random number between the specified
		//minimum and maximum time constraints.
		double timeTillAutoRevert = systemSettingInfo.toggleSettingAutoRevertTime;
		if(systemSettingInfo.toggleSettingAutoRevertTimeRandom)
		{
			timeTillAutoRevert = (double)rand() * ((systemSettingInfo.toggleSettingAutoRevertTimeEnd - systemSettingInfo.toggleSettingAutoRevertTime) / (double)RAND_MAX);
		}

		//Execute the system up to the revert time
		ExecuteSystemStep(timeTillAutoRevert);

		//Obtain info on the reverted option
		unsigned int revertTargetOption = systemSettingInfo.selectedOption;
		const System::SystemSettingOption& revertOptionInfo = systemSettingInfo.options[revertTargetOption];

		//Apply all settings to revert to the previous option state
		for(std::list<System::SystemStateChange>::const_iterator i = revertOptionInfo.stateChanges.begin(); i != revertOptionInfo.stateChanges.end(); ++i)
		{
			result &= ApplySystemStateChange(*i);
		}

		//Notify any registered observers about the option change
		systemSettingInfo.settingChangeObservers.NotifyObservers();
	}

	//Restore the system running state
	if(systemRunningState)
	{
		RunSystem();
	}

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
void System::ModuleSettingActiveOptionChangeNotifyRegister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer)
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return;
	}
	SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return;
	}

	//Add the specified observer to the list of observers for the target module setting
	systemSettingInfo.settingChangeObservers.AddObserver(observer);
}

//----------------------------------------------------------------------------------------
void System::ModuleSettingActiveOptionChangeNotifyDeregister(unsigned int moduleID, unsigned int moduleSettingID, IObserverSubscription& observer)
{
	//Locate the target module setting
	std::unique_lock<std::recursive_mutex> lock(moduleSettingMutex);
	SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(moduleSettingID);
	if(systemSettingsIterator == systemSettings.end())
	{
		return;
	}
	SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);
	if(systemSettingInfo.moduleID != moduleID)
	{
		return;
	}

	//Remove the specified observer from the list of observers for the target module
	//setting
	systemSettingInfo.settingChangeObservers.RemoveObserver(observer);
}

//----------------------------------------------------------------------------------------
//Path functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> System::GetCapturePath() const
{
	return capturePath;
}

//----------------------------------------------------------------------------------------
void System::SetCapturePath(const MarshalSupport::Marshal::In<std::wstring>& apath)
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
			const SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);

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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", message));
			result = false;
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
void System::FlagInitialize()
{
	initialize = true;
}

//----------------------------------------------------------------------------------------
void System::Initialize()
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Initialize the system
	InitializeInternal();

	//Restore running state
	if(running)
	{
		RunSystem();
	}
}

//----------------------------------------------------------------------------------------
void System::InitializeInternal()
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
		for(LoadedModuleInfoMap::const_iterator i = loadedModuleInfoMap.begin(); i != loadedModuleInfoMap.end(); ++i)
		{
			const LoadedModuleInfoInternal& moduleInfo = i->second;
			std::wstring persistentModuleFileName = moduleInfo.className + L".zip";
			std::wstring persistentModuleFilePath = PathCombinePaths(guiExtensionInterface.GetGlobalPreferencePathPersistentState(), persistentModuleFileName);
			if(!LoadPersistentStateForModule(persistentModuleFilePath, moduleInfo.moduleID, FileType::ZIP, true))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Failed to load persistent state from file \"" + persistentModuleFilePath + L"\" when initializing module with name " + moduleInfo.displayName + L"."));
			}
		}
	}

	//Assert the specified unmapped line state for any unmapped lines in the system that
	//have an unmapped line state specified
	for(UnmappedLineStateList::const_iterator i = unmappedLineStateList.begin(); i != unmappedLineStateList.end(); ++i)
	{
		//Retrieve information on the module that contains the target device
		unsigned int deviceModuleID = i->targetDevice->GetDeviceModuleID();
		LoadedModuleInfoMap::const_iterator deviceLoadedModuleIterator = loadedModuleInfoMap.find(deviceModuleID);
		if(deviceLoadedModuleIterator == loadedModuleInfoMap.end())
		{
			continue;
		}
		const LoadedModuleInfoInternal& deviceModuleInfo = deviceLoadedModuleIterator->second;

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
void System::InitializeDevice(IDevice* device)
{
	//Save running state and pause system
	//##TODO## Determine if there are threading issues that need to be addressed for this
	//method and other similar methods that are now exposed to extensions.
	bool running = SystemRunning();
	StopSystem();

	//Initialize the target device
	//##TODO## See if there are additional initialize steps we should be performing here.
	//Refer to the full InitializeInternal() method above for reference.
	DeviceContext* deviceContext = (DeviceContext*)device->GetDeviceContext();
	device->NegateCurrentOutputLineState();
	deviceContext->Initialize();
	device->AssertCurrentOutputLineState();

	//Re-assert the current line state for all system lines. This is required, as devices
	//reset their input line state as a result of a call to the Initialize method.
	for(SystemLineMap::const_iterator i = systemLines.begin(); i != systemLines.end(); ++i)
	{
		SetSystemLineState(i->first, Data(i->second.lineWidth, i->second.currentValue));
	}

	//Restore running state
	if(running)
	{
		RunSystem();
	}
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
void System::SignalSystemStopped()
{
	std::unique_lock<std::mutex> lock(systemStateMutex);
	systemStopped = true;
	notifySystemStopped.notify_all();
}

//----------------------------------------------------------------------------------------
//System execution functions
//----------------------------------------------------------------------------------------
bool System::SystemRunning() const
{
	return !systemStopped;
}

//----------------------------------------------------------------------------------------
void System::RunSystem()
{
	if(!SystemRunning())
	{
		//Log the event
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"System entered the running state"));

		stopSystem = false;
		systemStopped = false;
		std::thread workerThread(std::bind(std::mem_fn(&System::ExecuteThread), this));
		workerThread.detach();
	}
}

//----------------------------------------------------------------------------------------
void System::StopSystem()
{
	std::unique_lock<std::mutex> lock(systemStateMutex);
	if(!systemStopped)
	{
		stopSystem = true;
		notifySystemStopped.wait(lock);
		stopSystem = false;

		//Log the event
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"System halted"));
	}
}

//----------------------------------------------------------------------------------------
void System::FlagStopSystem()
{
	stopSystem = true;
}

//----------------------------------------------------------------------------------------
void System::ExecuteDeviceStep(IDevice* device)
{
	//Initialize all devices if it has been requested
	if(initialize)
	{
		//Initialize the devices
		InitializeInternal();

		//Clear the initialize flag
		initialize = false;
	}

	//Start active device threads
	executionManager.BeginExecution();

	//Commit the current state of each device. We perform this task here to ensure that
	//manual changes made through the debug interface while the system was idle, and the
	//initialize step above, are not lost in the event of a rollback.
	executionManager.Commit();

	//Flag that we're performing a single device step
	performingSingleDeviceStep = true;

	//Notify upcoming timeslice
	executionManager.NotifyUpcomingTimeslice(0.0);

	//Notify before execute called
	executionManager.NotifyBeforeExecuteCalled();

	//Step through the target device
	//##FIX## This execution model is not only unusual, it now also causes a deadlock, due
	//to the missed NotifyAfterExecuteStepFinishedTimeslice() event. It could also cause
	//logic problems for timeslice execution devices, which receive a notification of an
	//upcoming timeslice, but never receive the timeslice itself. I think perhaps it's
	//time to unify device stepping with our normal execution model, and just make a
	//device step execute a system step for a time of 0.0, then step through the device
	//as a timing point. Actually, looking at our system implementation, it has the exact
	//same problems and deadlock case. We need to review our execution model here.
	rollback = false;
	DeviceContext* deviceContext = (DeviceContext*)device->GetDeviceContext();
	double timeslice = deviceContext->ExecuteStep();

	//Notify after execute called
	executionManager.NotifyAfterExecuteCalled();

	//Flag that we're no longer performing a single device step
	performingSingleDeviceStep = false;

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
			totalTimeExecuted += ExecuteSystemStepInternal(timeslice - totalTimeExecuted);
		}
	}

	//Stop active device threads
	executionManager.SuspendExecution();
}

//----------------------------------------------------------------------------------------
void System::ExecuteSystemStep(double targetTime)
{
	//Stop the system if it is currently running
	StopSystem();

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
		totalSystemExecutionTime += ExecuteSystemStepInternal(timeRemainingToTarget);
	}

	//Stop active device threads
	executionManager.SuspendExecution();
}

//----------------------------------------------------------------------------------------
double System::ExecuteSystemStepInternal(double maximumTimeslice)
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
		//Flag that we're performing a single device step
		performingSingleDeviceStep = true;

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

		//Flag that we're no longer performing a single device step
		performingSingleDeviceStep = false;
	}

	//Commit all changes
	executionManager.Commit();

	//Clear all input events which have been successfully processed
	ClearSentStoredInputEvents();

	return timeslice;
}

//----------------------------------------------------------------------------------------
void System::ExecuteThread()
{
	//Set the name of this thread to assist in debugging
	SetCallingThreadName(L"SystemExecuteThread");

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
			InitializeInternal();

			//Start active device threads
			executionManager.BeginExecution();

			//Commit changes from initialization
			executionManager.Commit();

			//Clear the initialize flag
			initialize = false;
		}

		//##TODO## Make it possible to configure the maximum timeslice size
		//##TODO## Build in heuristics to calculate the optimal maximum timeslice size
		double systemStepTime = ExecuteSystemStepInternal(20000000.0);
		accumulatedExecutionTime += systemStepTime;

		//##DEBUG##
		//Note that this kills performance
//		std::wcout << std::setprecision(16) << "System Step: " << systemStepTime << '\t' << accumulatedExecutionTime << '\n';

		//If we're running too fast (*chuckle*), delay execution until we get back in
		//sync.
		if(accumulatedExecutionTime >= 20000000.0)
//		if(accumulatedExecutionTime >= 1000000000.0)
		{
			timer.Sync(accumulatedExecutionTime, enableThrottling, guiExtensionInterface.GetGlobalPreferenceShowDebugConsole());
			accumulatedExecutionTime = 0;
		}
	}

	//Stop active device threads
	executionManager.SuspendExecution();

	SignalSystemStopped();
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
	message << L"Rollback flagged:\t" << atriggerDevice->GetTargetDevice().GetDeviceInstanceName() << '\t';
	if(arollbackDevice != 0)
	{
		message << arollbackDevice->GetTargetDevice().GetDeviceInstanceName() << '\t';
	}
	message << std::setprecision(16) << timeslice << '\n';
	std::wcout << message.str();

	std::unique_lock<std::mutex> lock(systemRollbackMutex);
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
		if((arollbackDevice != 0) && (arollbackDevice->GetTargetDevice().GetUpdateMethod() == IDevice::UpdateMethod::Step))
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
bool System::PerformingSingleDeviceStep() const
{
	return performingSingleDeviceStep;
}

//----------------------------------------------------------------------------------------
//Device registration
//----------------------------------------------------------------------------------------
bool System::RegisterDevice(const IDeviceInfo& entry, AssemblyHandle assemblyHandle)
{
	//Make sure a valid device class name has been supplied
	if(entry.GetDeviceClassName().Get().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering device. No device class name was supplied."));
		return false;
	}

	//Make sure a valid device implementation name has been supplied
	if(entry.GetDeviceImplementationName().Get().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering device. No device implementation name was supplied."));
		return false;
	}

	//Make sure an allocator function has been supplied
	if(entry.GetAllocator() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering device " + entry.GetDeviceImplementationName() + L". No allocator function was supplied."));
		return false;
	}

	//Make sure a destructor function has been supplied
	if(entry.GetDestructor() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering device " + entry.GetDeviceImplementationName() + L". No destructor function was supplied."));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", message.str()));
			return true;
		}
		else
		{
			//Log the fact we just overrode an existing device registration
			std::wstringstream message;
			message << L"Device " << entry.GetDeviceImplementationName() << L" with version number " << entry.GetDeviceVersionNo() << L" overrode the existing registration for this device with version number " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", message.str()));

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
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Successfully registered device " + entry.GetDeviceImplementationName() + L"."));

	return true;
}

//----------------------------------------------------------------------------------------
void System::UnregisterDevice(const MarshalSupport::Marshal::In<std::wstring>& deviceName)
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
bool System::RegisterExtension(const IExtensionInfo& entry, AssemblyHandle assemblyHandle)
{
	//Make sure a valid extension class name has been supplied
	if(entry.GetExtensionClassName().Get().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering extension. No extension class name was supplied."));
		return false;
	}

	//Make sure a valid extension implementation name has been supplied
	if(entry.GetExtensionImplementationName().Get().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering extension. No extension implementation name was supplied."));
		return false;
	}

	//Make sure an allocator function has been supplied
	if(entry.GetAllocator() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering extension " + entry.GetExtensionImplementationName() + L". No allocator function was supplied."));
		return false;
	}

	//Make sure a destructor function has been supplied
	if(entry.GetDestructor() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error registering extension " + entry.GetExtensionImplementationName() + L". No destructor function was supplied."));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", message.str()));
			return true;
		}
		else
		{
			//Log the fact we just overrode an existing extension registration
			std::wstringstream message;
			message << L"Extension " << entry.GetExtensionImplementationName() << L" with version number " << entry.GetExtensionVersionNo() << L" overrode the existing registration for this extension with version number " << existingEntry->second.versionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", message.str()));

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
	listEntry.persistentGlobalExtension = entry.GetIsPersistentGlobalExtension();
	listEntry.assemblyHandle = assemblyHandle;
	extensionLibrary.insert(ExtensionLibraryListEntry(entry.GetExtensionImplementationName(), listEntry));

	//Log the extension registration
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Successfully registered extension " + entry.GetExtensionImplementationName() + L"."));

	//If the registered extension is marked as a persistent global extension, attempt to
	//create an instance of it now.
	if(listEntry.persistentGlobalExtension)
	{
		//Log the creation attempt
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Creating instance of registered persistent global extension " + entry.GetExtensionImplementationName() + L"."));

		//Attempt to create an instance of the persistent global extension, and log an
		//error if the attempt fails.
		if(!LoadPersistentGlobalExtension(listEntry.implementationName))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to create instance of persistent global extension " + entry.GetExtensionImplementationName() + L"!"));
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void System::UnregisterExtension(const MarshalSupport::Marshal::In<std::wstring>& extensionName)
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
IDevice* System::CreateDevice(const std::wstring& deviceName, const std::wstring& instanceName, unsigned int moduleID) const
{
	IDevice* device = 0;
	DeviceLibraryList::const_iterator deviceLibraryIterator = deviceLibrary.find(deviceName);
	if(deviceLibraryIterator != deviceLibrary.end())
	{
		device = deviceLibraryIterator->second.Allocator(deviceName.c_str(), instanceName.c_str(), moduleID);
	}
	return device;
}

//----------------------------------------------------------------------------------------
void System::DestroyDevice(const std::wstring& deviceName, IDevice* device) const
{
	//##TODO## Log an error if a device cannot be located in the device library
	DeviceLibraryList::const_iterator deviceLibraryIterator = deviceLibrary.find(deviceName);
	if(deviceLibraryIterator != deviceLibrary.end())
	{
		deviceLibraryIterator->second.Destructor(device);
	}
}

//----------------------------------------------------------------------------------------
void System::UnloadDevice(IDevice* adevice)
{
	//Remove all references to the device from our input mappings
	UnmapAllKeyCodeMappingsForDevice(adevice);

	//Remove all registered input targets for this device from our input registration list
	std::unique_lock<std::mutex> inputLock(inputMutex);
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

	//Update the last modified token for the input device list
	++inputDeviceListLastModifiedToken;

	//Release our lock on the input state
	inputLock.unlock();

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

	//Unregister each attached menu handler for this device
	LoadedDeviceInfoList::iterator loadedDeviceInfoListIterator = loadedDeviceInfoList.begin();
	while(loadedDeviceInfoListIterator != loadedDeviceInfoList.end())
	{
		if(loadedDeviceInfoListIterator->device == adevice)
		{
			for(std::set<IExtension*>::const_iterator i = loadedDeviceInfoListIterator->menuHandlers.begin(); i != loadedDeviceInfoListIterator->menuHandlers.end(); ++i)
			{
				(*i)->UnregisterDeviceMenuHandler(adevice);
			}
			loadedDeviceInfoListIterator->menuHandlers.clear();
			break;
		}
		++loadedDeviceInfoListIterator;
	}

	//Remove the device itself from the system
	executionManager.RemoveDevice((DeviceContext*)adevice->GetDeviceContext());
	RemoveDeviceFromDeviceList(devices, adevice);

	//Destroy the device
	DestroyDevice(adevice->GetDeviceClassName(), adevice);
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
	ExtensionLibraryList::const_iterator extensionLibraryIterator = extensionLibrary.find(extensionName);
	if(extensionLibraryIterator != extensionLibrary.end())
	{
		extension = extensionLibraryIterator->second.Allocator(extensionName.c_str(), instanceName.c_str(), moduleID);
		extension->SetAssemblyHandle(extensionLibraryIterator->second.assemblyHandle);
	}
	return extension;
}

//----------------------------------------------------------------------------------------
bool System::LoadPersistentGlobalExtension(const std::wstring& extensionName)
{
	//Create the new extension object
	IExtension* extension = CreateGlobalExtension(extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"LoadPersistentGlobalExtension failed for " + extensionName + L"!"));
		return false;
	}

	//Bind to the system interface
	if(!extension->BindToSystemInterface(this))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToSystemInterface failed for  " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Bind to the GUI interface
	if(!extension->BindToGUIInterface(&guiExtensionInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToGUIInterface failed for  " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Construct the extension object
	HierarchicalStorageNode node;
	if(!extension->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Call BuildExtension() to perform any other required post-creation initialzation for
	//the extension.
	if(!extension->BuildExtension())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BuildExtension failed for " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Record information on the loaded persistent global extension
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedGlobalExtensionInfo extensionInfo;
	extensionInfo.extension = extension;
	extensionInfo.name = extensionName;
	extensionInfo.persistentExtension = true;
	globalExtensionInfoList.insert(LoadedGlobalExtensionInfoListEntry(extensionInfo.name, extensionInfo));

	//Attempt to register the persistent global extension as a system menu handler
	if(extension->RegisterSystemMenuHandler())
	{
		systemMenuHandlers.insert(extension);
	}

	return true;
}

//----------------------------------------------------------------------------------------
void System::DestroyExtension(const std::wstring& extensionName, IExtension* extension) const
{
	ExtensionLibraryList::const_iterator extensionLibraryIterator = extensionLibrary.find(extensionName);
	if(extensionLibraryIterator != extensionLibrary.end())
	{
		extensionLibraryIterator->second.Destructor(extension);
	}
}

//----------------------------------------------------------------------------------------
void System::UnloadExtension(IExtension* aextension)
{
	//Ensure this extension is removed as a system handler if it is currently bound as one
	std::set<IExtension*>::iterator systemMenuHandlerIterator = systemMenuHandlers.find(aextension);
	if(systemMenuHandlerIterator != systemMenuHandlers.end())
	{
		aextension->UnregisterSystemMenuHandler();
		systemMenuHandlers.erase(systemMenuHandlerIterator);
	}

	//Ensure this extension is removed as a menu handler from any loaded modules it may be
	//associated with
	for(LoadedModuleInfoMap::iterator i = loadedModuleInfoMap.begin(); i != loadedModuleInfoMap.end(); ++i)
	{
		LoadedModuleInfoInternal& loadedModuleInfo = i->second;
		std::set<IExtension*>::iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.find(aextension);
		if(menuHandlerIterator != loadedModuleInfo.menuHandlers.end())
		{
			aextension->UnregisterModuleMenuHandler(loadedModuleInfo.moduleID);
			loadedModuleInfo.menuHandlers.erase(menuHandlerIterator);
		}
	}

	//Ensure this extension is removed as a menu handler from any loaded devices it may be
	//associated with
	for(LoadedDeviceInfoList::iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		std::set<IExtension*>::iterator menuHandlerIterator = i->menuHandlers.find(aextension);
		if(menuHandlerIterator != i->menuHandlers.end())
		{
			aextension->UnregisterDeviceMenuHandler(i->device);
			i->menuHandlers.erase(menuHandlerIterator);
		}
	}

	//Unregister each attached menu handler for this extension
	LoadedExtensionInfoList::iterator loadedExtensionInfoListIterator = loadedExtensionInfoList.begin();
	while(loadedExtensionInfoListIterator != loadedExtensionInfoList.end())
	{
		if(loadedExtensionInfoListIterator->extension == aextension)
		{
			for(std::set<IExtension*>::const_iterator i = loadedExtensionInfoListIterator->menuHandlers.begin(); i != loadedExtensionInfoListIterator->menuHandlers.end(); ++i)
			{
				(*i)->UnregisterExtensionMenuHandler(aextension);
			}
			loadedExtensionInfoListIterator->menuHandlers.clear();
			break;
		}
		++loadedExtensionInfoListIterator;
	}

	//Destroy the extension
	DestroyExtension(aextension->GetExtensionClassName(), aextension);
}

//----------------------------------------------------------------------------------------
//Module loading and unloading
//----------------------------------------------------------------------------------------
void System::LoadModuleSynchronous(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings)
{
	loadSystemComplete = false;
	loadSystemProgress = 0;
	loadSystemAbort = false;
	std::thread workerThread(std::bind(std::mem_fn(&System::LoadModule), this, filePath.Get(), connectorMappings.Get()));
	workerThread.detach();
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
bool System::LoadModule(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::In<ConnectorMappingList>& connectorMappings)
{
	std::unique_lock<std::mutex> lock(moduleLoadMutex);

	//Initialize the system load status
	loadSystemAbort = false;
	loadSystemResult = true;

	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Load the module file
	std::list<unsigned int> addedModuleIDs;
	std::list<ViewOpenRequest> viewOpenRequests;
	std::list<InputRegistration> inputRegistrationRequests;
	std::list<SystemStateChange> systemSettingsChangeRequests;
	if(!LoadModuleInternal(filePath, connectorMappings, viewOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModuleIDs))
	{
		//If there's an error loading the module, log the failure, and return false.
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load module from file " + filePath + L"!"));
		for(std::list<unsigned int>::const_iterator addedModuleIterator = addedModuleIDs.begin(); addedModuleIterator != addedModuleIDs.end(); ++addedModuleIterator)
		{
			UnloadModuleInternal(*addedModuleIterator);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
	}

	//Detect if we just loaded a program module
	bool programModuleLoaded = false;
	for(std::list<unsigned int>::const_iterator i = addedModuleIDs.begin(); i != addedModuleIDs.end(); ++i)
	{
		programModuleLoaded |= loadedModuleInfoMap[*i].programModule;
	}

	//Bind all CE line mappings specified in all BusInterface objects to their referenced
	//devices.
	for(BusInterfaceList::iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		if(!i->busInterface->BindCELineMappings())
		{
			//If there's an error binding the ce line mappings, log the failure, and
			//return false.
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindCELineMappings failed for BusInterface " + i->name + L" when loading module from file " + filePath + L"!"));
			for(std::list<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
			{
				UnloadModuleInternal(*addedModuleIDsIterator);
			}
			if(!ValidateSystem())
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
			}
			loadSystemComplete = true;
			loadSystemResult = false;
			return false;
		}
	}

	//Explicitly initialize all devices we just loaded
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(std::find(addedModuleIDs.begin(), addedModuleIDs.end(), i->moduleID) != addedModuleIDs.end())
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"System validation failed after loading module from file " + filePath + L"!"));
		for(std::list<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
		{
			UnloadModuleInternal(*addedModuleIDsIterator);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Error restoring system after loading of module " + filePath + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
	}

	//Flag that all loaded modules have passed validation. After this point, we want to
	//call functions on a device which affect the state of other devices when unloading
	//them from the system.
	for(std::list<unsigned int>::const_iterator i = addedModuleIDs.begin(); i != addedModuleIDs.end(); ++i)
	{
		loadedModuleInfoMap[*i].moduleValidated = true;
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
	for(std::list<unsigned int>::const_iterator addedModuleIDsIterator = addedModuleIDs.begin(); addedModuleIDsIterator != addedModuleIDs.end(); ++addedModuleIDsIterator)
	{
		std::unique_lock<std::recursive_mutex> moduleSettingsLock(moduleSettingMutex);
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
				SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);

				//Apply the default option for this setting
				if(systemSettingInfo.defaultOption < (unsigned int)systemSettingInfo.options.size())
				{
					//Replace the current option selection
					systemSettingInfo.selectedOption = systemSettingInfo.defaultOption;

					//Apply the new option selection
					SystemSettingOption& settingOption = systemSettingInfo.options[systemSettingInfo.selectedOption];
					for(std::list<SystemStateChange>::const_iterator i = settingOption.stateChanges.begin(); i != settingOption.stateChanges.end(); ++i)
					{
						//Apply this system state change
						if(!ApplySystemStateChange(*i))
						{
							WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Failed to apply system setting change for element with name \"" + i->targetElementName + L"\" in system option \"" + settingOption.name + L"\" on system setting \"" + systemSettingInfo.name + L"\" when loading module from file " + filePath + L"!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Failed to apply system setting change operating on element with name \"" + i->targetElementName + L"\" when loading module from file " + filePath + L"!"));
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
				if(!SetDeviceKeyCodeMapping(defaultInputRegistration.targetDevice, defaultInputRegistration.deviceKeyCode, defaultInputRegistration.systemKeyCode))
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"SetDeviceKeyCodeMapping failed on default input registration for " + defaultInputRegistration.targetDevice->GetDeviceInstanceName() + L" with device key code " + defaultInputRegistration.targetDevice->GetKeyCodeName(defaultInputRegistration.deviceKeyCode) + L" and system key code " + GetKeyCodeName(defaultInputRegistration.systemKeyCode) + L"!"));
				}
			}
		}
	}

	//Bind all currently loaded global extensions as menu handlers where appropriate
	for(LoadedGlobalExtensionInfoList::iterator globalExtensionIterator = globalExtensionInfoList.begin(); globalExtensionIterator != globalExtensionInfoList.end(); ++globalExtensionIterator)
	{
		//Retrieve information on the next global extension, and determine if this global
		//extension was just loaded by the set of loaded modules.
		LoadedGlobalExtensionInfo& globalExtensionInfo = globalExtensionIterator->second;
		bool globalExtensionWasJustLoaded = (!globalExtensionInfo.persistentExtension && std::includes(addedModuleIDs.begin(), addedModuleIDs.end(), globalExtensionInfo.moduleIDs.begin(), globalExtensionInfo.moduleIDs.end()));

		//If this global extension was just loaded, attempt to add it as a menu handler
		//for the system.
		if(globalExtensionWasJustLoaded)
		{
			if(globalExtensionInfo.extension->RegisterSystemMenuHandler())
			{
				systemMenuHandlers.insert(globalExtensionInfo.extension);
			}
		}

		//Attempt to add this global extension as a menu handler for all modules which
		//were just loaded, or all currently loaded modules if this global extension was
		//just loaded.
		for(LoadedModuleInfoMap::iterator moduleIterator = loadedModuleInfoMap.begin(); moduleIterator != loadedModuleInfoMap.end(); ++moduleIterator)
		{
			LoadedModuleInfoInternal& moduleInfo = moduleIterator->second;
			if(globalExtensionWasJustLoaded || (std::find(addedModuleIDs.begin(), addedModuleIDs.end(), moduleInfo.moduleID) != addedModuleIDs.end()))
			{
				if(globalExtensionInfo.extension->RegisterModuleMenuHandler(moduleInfo.moduleID))
				{
					moduleInfo.menuHandlers.insert(globalExtensionInfo.extension);
				}
			}
		}

		//Attempt to add this global extension as a menu handler for all devices which
		//were just loaded, or all currently loaded devices if this global extension was
		//just loaded itself.
		for(LoadedDeviceInfoList::iterator deviceIterator = loadedDeviceInfoList.begin(); deviceIterator != loadedDeviceInfoList.end(); ++deviceIterator)
		{
			LoadedDeviceInfo& deviceInfo = *deviceIterator;
			if(globalExtensionWasJustLoaded || (std::find(addedModuleIDs.begin(), addedModuleIDs.end(), deviceInfo.moduleID) != addedModuleIDs.end()))
			{
				if(globalExtensionInfo.extension->RegisterDeviceMenuHandler(deviceInfo.device))
				{
					deviceInfo.menuHandlers.insert(globalExtensionInfo.extension);
				}
			}
		}

		//Attempt to add this global extension as a menu handler for all extensions which
		//were just loaded, or all currently loaded extensions if this global extension
		//was just loaded itself.
		for(LoadedExtensionInfoList::iterator extensionIterator = loadedExtensionInfoList.begin(); extensionIterator != loadedExtensionInfoList.end(); ++extensionIterator)
		{
			LoadedExtensionInfo& extensionInfo = *extensionIterator;
			if(globalExtensionWasJustLoaded || (std::find(addedModuleIDs.begin(), addedModuleIDs.end(), extensionInfo.moduleID) != addedModuleIDs.end()))
			{
				if(globalExtensionInfo.extension->RegisterExtensionMenuHandler(extensionInfo.extension))
				{
					extensionInfo.menuHandlers.insert(globalExtensionInfo.extension);
				}
			}
		}
	}

	//Save all input registration requests from the list of loaded modules into the list
	//of active input registrations.
	std::unique_lock<std::mutex> inputLock(inputMutex);
	inputRegistrationList.splice(inputRegistrationList.end(), inputRegistrationRequests);

	//Update the last modified token for the input device list
	++inputDeviceListLastModifiedToken;

	//Release our lock on the input state
	inputLock.unlock();

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Loaded module from file " + filePath + L"."));

	//Process any view open requests we encountered during the system load
	LoadModule_ProcessViewQueue(viewOpenRequests);

	//Restore running state, or trigger the system to run automatically if we just loaded
	//a program module and the user has instructed the system to run automatically when
	//program modules are loaded.
	if(running || (runWhenProgramModuleLoaded && programModuleLoaded))
	{
		RunSystem();
	}

	//Flag that the load system operation is complete
	loadSystemComplete = true;

	//Notify any registered observers that the set of loaded modules has now changed
	lock.unlock();
	loadedModuleChangeObservers.NotifyObservers();

	//Return the system load result
	return loadSystemResult;
}

//----------------------------------------------------------------------------------------
bool System::LoadModuleInternal(const std::wstring& filePath, const ConnectorMappingList& connectorMappings, std::list<ViewOpenRequest>& viewOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, std::list<unsigned int>& addedModuleIDs)
{
	//Update the name of the currently loading module
	std::wstring fileName = PathGetFileName(filePath);
	PushLoadModuleCurrentModuleName(fileName);

	//Open the target file
	FileStreamReference sourceStreamReference(guiExtensionInterface);
	if(!sourceStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not open module file " + filePath + L"!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	Stream::IStream& source = *sourceStreamReference;

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
	source.ProcessByteOrderMark();

	//Load the XML structure from the file
	HierarchicalStorageTree tree;
	if(!tree.LoadTree(source))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error loading XML structure from module file " + filePath + L"! The xml error string is as follows: " + tree.GetErrorString()));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();

	//Load external binary data into the XML tree
	std::list<IHierarchicalStorageNode*> binaryList;
	binaryList = tree.GetBinaryDataNodeList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
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
		FileStreamReference binaryFileStreamReference(guiExtensionInterface);
		if(!binaryFileStreamReference.OpenExistingFileForRead(binaryFilePath))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load module from file " + filePath + L" because a binary data file could not be found in the target path " + binaryFilePath + L"!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load module from file " + filePath + L" because there was an error reading binary data from file " + binaryFilePath + L"!"));
			PopLoadModuleCurrentModuleName();
			return false;
		}
		if(!binaryData.WriteData(buffer, bufferSize))
		{
			delete[] buffer;
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load module from file " + filePath + L" because there was an error saving binary data read from file " + binaryFilePath + L"!"));
			PopLoadModuleCurrentModuleName();
			return false;
		}
		delete[] buffer;
	}

	//If this is a system module, defer processing of this module to the LoadSystem
	//method, otherwise verify that this file is marked as a module.
	if(rootNode.GetName() == L"System")
	{
		bool result = LoadSystem(filePath, rootNode, viewOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModuleIDs);
		PopLoadModuleCurrentModuleName();
		return result;
	}
	else if(rootNode.GetName() != L"Module")
	{
		//Neither a system nor a module root node was found. Abort any further processing.
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error loading module file " + filePath + L"! The root node was not of type System or Module!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//Create a new module info structure for this module
	LoadedModuleInfoInternal moduleInfo;
	moduleInfo.moduleID = GenerateFreeModuleID();
	moduleInfo.filePath = filePath;

	//Extract mandatory module metadata
	IHierarchicalStorageAttribute* systemClassNameAttribute = rootNode.GetAttribute(L"SystemClassName");
	IHierarchicalStorageAttribute* moduleClassNameAttribute = rootNode.GetAttribute(L"ModuleClassName");
	IHierarchicalStorageAttribute* moduleInstanceNameAttribute = rootNode.GetAttribute(L"ModuleInstanceName");
	if((systemClassNameAttribute == 0) || (moduleClassNameAttribute == 0) || (moduleInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing SystemClassName, ModuleClassName, or ModuleInstanceName attribute on root node!"));
		PopLoadModuleCurrentModuleName();
		return false;
	}
	moduleInfo.systemClassName = systemClassNameAttribute->GetValue();
	moduleInfo.className = moduleClassNameAttribute->GetValue();
	moduleInfo.instanceName = moduleInstanceNameAttribute->GetValue();

	//Load any additional optional metadata
	IHierarchicalStorageAttribute* programModuleAttribute = rootNode.GetAttribute(L"ProgramModule");
	if(programModuleAttribute != 0)
	{
		moduleInfo.programModule = programModuleAttribute->ExtractValue<bool>();
	}
	IHierarchicalStorageAttribute* manufacturerCodeAttribute = rootNode.GetAttribute(L"ManufacturerCode");
	if(manufacturerCodeAttribute != 0)
	{
		moduleInfo.manufacturerCode = manufacturerCodeAttribute->GetValue();
	}
	IHierarchicalStorageAttribute* moduleDisplayNameAttribute = rootNode.GetAttribute(L"ModuleDisplayName");
	if(moduleDisplayNameAttribute != 0)
	{
		moduleInfo.displayName = moduleDisplayNameAttribute->GetValue();
	}

	//Check for existing modules with the same instance name, and ensure a unique instance
	//name is generated if a conflict is found.
	unsigned int instanceNameCurrentPostfixNumber = 0;
	LoadedModuleInfoMap::const_iterator instanceNameModuleIterator = loadedModuleInfoMap.begin();
	std::wstring initialModuleInstanceName = moduleInfo.instanceName;
	while(instanceNameModuleIterator != loadedModuleInfoMap.end())
	{
		//If the instance name is the same, try and make a new unique instance name for
		//our module.
		if(instanceNameModuleIterator->second.instanceName == moduleInfo.instanceName)
		{
			//Determine which postfix number to use
			unsigned int numericPostfix = ++instanceNameCurrentPostfixNumber;

			//Convert the numeric postfix to a string
			std::wstringstream numericPostfixToString;
			numericPostfixToString << numericPostfix;

			//Rebuild our instance name using the postfix
			moduleInfo.instanceName = initialModuleInstanceName + L" [" + numericPostfixToString.str() + L"]";

			//Restart the loop
			instanceNameModuleIterator = loadedModuleInfoMap.begin();
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
	LoadedModuleInfoMap::const_iterator displayNameModuleIterator = loadedModuleInfoMap.begin();
	std::wstring initialModuleDisplayName = moduleInfo.displayName;
	while(displayNameModuleIterator != loadedModuleInfoMap.end())
	{
		//If the display name is the same, try and make a new unique display name for our
		//module.
		if(displayNameModuleIterator->second.displayName == moduleInfo.displayName)
		{
			//Determine which postfix number to use
			unsigned int numericPostfix = ++displayNameCurrentPostfixNumber;

			//Convert the numeric postfix to a string
			std::wstringstream numericPostfixToString;
			numericPostfixToString << numericPostfix;

			//Rebuild our display name using the postfix
			moduleInfo.displayName = initialModuleDisplayName + L" [" + numericPostfixToString.str() + L"]";

			//Restart the loop
			displayNameModuleIterator = loadedModuleInfoMap.begin();
			continue;
		}
		++displayNameModuleIterator;
	}

	//Load the elements from the root node one by one
	bool loadedWithoutErrors = true;
	NameToIDMap connectorNameToIDMap;
	NameToIDMap lineGroupNameToIDMap;
	unsigned int entriesProcessed = 0;
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	unsigned int entryCount = (unsigned int)childList.size();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
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
		else if(elementName == L"System.OpenView")
		{
			//Note that we don't process view open requests immediately when they are
			//encountered. These requests are only processed once we're sure the module
			//has loaded successfully. This removes the need to worry about locked threads
			//waiting for views to open preventing us from doing cleanup of this module,
			//in the case that the load fails.
			loadedWithoutErrors &= LoadModule_System_OpenView(*(*i), moduleInfo.moduleID, viewOpenRequests);
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized element: " + elementName + L" when loading module file " + filePath + L"."));
		}
	}

	//Add the info for this module to the list of loaded modules
	addedModuleIDs.push_back(moduleInfo.moduleID);
	loadedModuleInfoMap.insert(LoadedModuleInfoMapEntry(moduleInfo.moduleID, moduleInfo));

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"The user aborted loading module from file " + filePath + L"."));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//If any errors occurred while loading the module file, log the error and return
	//false.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Errors occurred while loading module from file " + filePath + L"."));
		PopLoadModuleCurrentModuleName();
		return false;
	}

	//Update the currently loading module name stack
	PopLoadModuleCurrentModuleName();

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_Device_Settings(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID and device name
	IHierarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if((moduleIDAttribute == 0) || (deviceInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ModuleID or DeviceInstanceName attribute for Device.Settings!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for Device.Settings!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate target device with name " + deviceName + L" for Device.Settings!"));
		return false;
	}

	//Load the device settings from this node
	device->LoadSettingsState(node);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_Device_MapInput(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHierarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* systemKeyCodeAttribute = node.GetAttribute(L"SystemKeyCode");
	IHierarchicalStorageAttribute* deviceKeyCodeAttribute = node.GetAttribute(L"DeviceKeyCode");
	if((moduleIDAttribute == 0) || (deviceInstanceNameAttribute == 0) || (systemKeyCodeAttribute == 0) || (deviceKeyCodeAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ModuleID, DeviceInstanceName, SystemKeyCode or DeviceKeyCode attribute for Device.MapInput!"));
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
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for Device.MapInput!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate target device with name " + deviceName + L" for Device.MapInput!"));
		return false;
	}

	//Translate the device key code
	unsigned int deviceKeyCode = device->GetKeyCodeID(deviceKeyCodeString);
	if(deviceKeyCode == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device key code with name " + deviceKeyCodeString + L" on device " + deviceName + L" for Device.MapInput!"));
		return false;
	}

	//Translate the system key code
	KeyCode systemKeyCode = GetKeyCodeID(systemKeyCodeString);
	if(systemKeyCode == ISystemDeviceInterface::KeyCode::None)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate system key code with name " + systemKeyCodeString + L" for Device.MapInput!"));
		return false;
	}

	//Add the key code mapping to the system
	if(!SetDeviceKeyCodeMapping(device, deviceKeyCode, systemKeyCode))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"SetDeviceKeyCodeMapping failed on Device.MapInput key for " + deviceName + L" with device key code " + deviceKeyCodeString + L" and system key code " + systemKeyCodeString + L" for Device.MapInput!"));
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_System_LoadEmbeddedROMData(const std::wstring& fileDir, IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHierarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	IHierarchicalStorageAttribute* embeddedROMNameAttribute = node.GetAttribute(L"EmbeddedROMName");
	IHierarchicalStorageAttribute* filePathAttribute = node.GetAttribute(L"FilePath");
	if((moduleIDAttribute == 0) || (embeddedROMNameAttribute == 0) || (filePathAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ModuleID, EmbeddedROMName or FilePath attribute for System.LoadEmbeddedROMData!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();
	std::wstring embeddedROMName = embeddedROMNameAttribute->GetValue();
	std::wstring filePath = filePathAttribute->GetValue();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for System.LoadEmbeddedROMData!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Locate the defined ROM info for this embedded ROM
	std::unique_lock<std::mutex> embeddedROMLock(embeddedROMMutex);
	EmbeddedROMInfoInternal* embeddedROMInfoEntry = 0;
	std::map<unsigned int, EmbeddedROMInfoInternal>::iterator embeddedROMInfoIterator = embeddedROMInfoSet.begin();
	while((embeddedROMInfoEntry == 0) && (embeddedROMInfoIterator != embeddedROMInfoSet.end()))
	{
		EmbeddedROMInfoInternal& embeddedROMInfo = embeddedROMInfoIterator->second;
		if((embeddedROMInfo.moduleID == moduleID) && (embeddedROMInfo.embeddedROMName == embeddedROMName))
		{
			embeddedROMInfoEntry = &embeddedROMInfo;
			continue;
		}
		++embeddedROMInfoIterator;
	}
	if(embeddedROMInfoEntry == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate matching embedded ROM definition for embedded ROM with name \"" << embeddedROMName << L"\" in module with ID \"" << moduleID << L"\" for System.LoadEmbeddedROMData!";
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
	FileStreamReference fileStreamReference(guiExtensionInterface);
	if(!fileStreamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to open target file with path \"" + filePath + L"\" when attempting to load data for embedded ROM with name \"" + embeddedROMName + L"\" for System.LoadEmbeddedROMData!"));
		return false;
	}
	Stream::IStream& file = *fileStreamReference;

	//Load the data from the target file into this embedded ROM
	Data romDataEntry(embeddedROMInfoEntry->romEntryBitCount);
	Stream::ViewBinary viewBinary(file);
	unsigned int deviceAddress = 0;
	while(!viewBinary.IsAtEnd() && viewBinary.NoErrorsOccurred() && (deviceAddress < embeddedROMInfoEntry->romRegionSize))
	{
		viewBinary >> romDataEntry;
		embeddedROMInfoEntry->targetDevice->TransparentWriteInterface(embeddedROMInfoEntry->interfaceNumber, deviceAddress, romDataEntry, 0, 0);
		++deviceAddress;
	}
	bool romDataLoadedSuccessfully = viewBinary.NoErrorsOccurred();

	//Update the last modified token for embedded ROM data
	++embeddedROMInfoLastModifiedToken;

	return romDataLoadedSuccessfully;
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem_System_SelectSettingOption(IHierarchicalStorageNode& node, std::map<unsigned int, unsigned int>& savedModuleIDToLoadedModuleIDMap, SystemStateChange& stateChange)
{
	//Load the external module ID, device name, system key code, and target key code.
	IHierarchicalStorageAttribute* moduleIDAttribute = node.GetAttribute(L"ModuleID");
	if(moduleIDAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ModuleID attribute for System.SelectSettingOption!"));
		return false;
	}
	unsigned int moduleIDExternal = moduleIDAttribute->ExtractValue<unsigned int>();

	//Resolve the loaded module ID from the external module ID
	std::map<unsigned int, unsigned int>::const_iterator loadedModuleIDIterator = savedModuleIDToLoadedModuleIDMap.find(moduleIDExternal);
	if(loadedModuleIDIterator == savedModuleIDToLoadedModuleIDMap.end())
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate imported module with ID " << moduleIDExternal << L" for System.SelectSettingOption!";
		WriteLogEvent(logEntry);
		return false;
	}
	unsigned int moduleID = loadedModuleIDIterator->second;

	//Defer the remainder of this load operation to the module loader method
	return LoadModule_System_SelectSettingOption(node, moduleID, stateChange);
}

//----------------------------------------------------------------------------------------
bool System::LoadSystem(const std::wstring& filePath, IHierarchicalStorageNode& rootNode, std::list<ViewOpenRequest>& viewOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<SystemStateChange>& systemSettingsChangeRequests, std::list<unsigned int>& addedModuleIDs)
{
	//Extract the module relationships data from the file
	bool moduleRelationshipsLoaded = false;
	SavedRelationshipMap savedRelationshipData;
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load system from file " + filePath + L" because the ModuleRelationships node could not be loaded!"));
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
		modulesLoadedWithoutErrors &= LoadModuleInternal(savedModuleFilePath, targetModuleConnectorMappings, viewOpenRequests, inputRegistrationRequests, systemSettingsChangeRequests, addedModuleIDs);

		//Save the mapping between the saved module ID and the loaded module ID for the
		//module we just loaded.
		//##FIX## If we referenced another system file, this module ID will be
		//meaningless. But then again, we don't really want to support referencing a
		//system file from within a system file. Add some kind of verification during
		//module loading to prevent this. Once we've done that, this code below will be
		//acceptable.
		std::list<unsigned int>::const_reverse_iterator addedModuleIterator = addedModuleIDs.rbegin();
		if(addedModuleIterator != addedModuleIDs.rend())
		{
			savedModuleIDToLoadedModuleIDMap.insert(std::pair<unsigned int, unsigned int>(savedModuleInfo.moduleID, *addedModuleIterator));
		}
	}

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"The user aborted loading system from file " + filePath + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error and return
	//false.
	if(!modulesLoadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"One or more modules failed to load successfully when loading system from file " + filePath + L"."));
		return false;
	}

	//Load the elements from the root node one by one
	bool loadedWithoutErrors = true;
	NameToIDMap connectorNameToIDMap;
	NameToIDMap lineGroupNameToIDMap;
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized element: " + elementName + L" when loading system file " + filePath + L"."));
		}
	}

	//If the system load was aborted, log the event, and flag the module to unload.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"The user aborted loading system from file " + filePath + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error, and flag the
	//module to unload.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Errors occurred while loading system from file " + filePath + L"."));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::SaveSystem(const MarshalSupport::Marshal::In<std::wstring>& filePath)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Create the new system XML tree
	HierarchicalStorageTree tree;
	tree.GetRootNode().SetName(L"System");

	//Fill in general information about the file
	IHierarchicalStorageNode& stateInfo = tree.GetRootNode().CreateChild(L"Info");
	Timestamp timestamp = GetTimestamp();
	stateInfo.CreateAttribute(L"CreationDate", timestamp.GetDate());
	stateInfo.CreateAttribute(L"CreationTime", timestamp.GetTime());

	//Save the ModuleRelationships node
	std::wstring fileDir = PathGetDirectory(filePath);
	IHierarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	SaveModuleRelationshipsNode(moduleRelationshipsNode, true, fileDir);

	//Save device settings to the system file
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		IHierarchicalStorageNode& node = tree.GetRootNode().CreateChild(L"Device.Settings");
		node.CreateAttribute(L"DeviceInstanceName", (*i).device->GetDeviceInstanceName());
		node.CreateAttribute(L"ModuleID").SetValue((*i).moduleID);
		(*i).device->SaveSettingsState(node);
	}

	//Save mapped key inputs to the system file
	std::unique_lock<std::mutex> inputLock(inputMutex);
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
		IHierarchicalStorageNode& registerInputNode = tree.GetRootNode().CreateChild(L"Device.MapInput");
		registerInputNode.CreateAttribute(L"ModuleID", moduleID);
		registerInputNode.CreateAttribute(L"DeviceInstanceName", inputMapEntry.targetDevice->GetDeviceInstanceName());
		registerInputNode.CreateAttribute(L"SystemKeyCode", GetKeyCodeName(inputMapEntry.keyCode));
		registerInputNode.CreateAttribute(L"DeviceKeyCode", inputMapEntry.targetDevice->GetKeyCodeName(inputMapEntry.targetDeviceKeyCode));
	}
	inputLock.unlock();

	//Save current system setting selections to the system file
	std::unique_lock<std::recursive_mutex> moduleSettingsLock(moduleSettingMutex);
	for(SystemSettingsMap::const_iterator i = systemSettings.begin(); i != systemSettings.end(); ++i)
	{
		const SystemSettingInfo& systemSettingInfo = *(i->second);
		if(systemSettingInfo.selectedOption < (unsigned int)systemSettingInfo.options.size())
		{
			//Create the system option selection node for this setting
			IHierarchicalStorageNode& selectSettingOptionNode = tree.GetRootNode().CreateChild(L"System.SelectSettingOption");
			selectSettingOptionNode.CreateAttribute(L"ModuleID", systemSettingInfo.moduleID);
			selectSettingOptionNode.CreateAttribute(L"SettingName", systemSettingInfo.name);
			selectSettingOptionNode.CreateAttribute(L"OptionName", systemSettingInfo.options[systemSettingInfo.selectedOption].name);
		}
	}
	moduleSettingsLock.unlock();

	//Save embedded ROM file selections to the system file
	std::unique_lock<std::mutex> embeddedROMLock(embeddedROMMutex);
	for(std::map<unsigned int, EmbeddedROMInfoInternal>::const_iterator i = embeddedROMInfoSet.begin(); i != embeddedROMInfoSet.end(); ++i)
	{
		//If no file path has been specified for this embedded ROM, skip it.
		const EmbeddedROMInfoInternal& embeddedROMInfo = i->second;
		if(embeddedROMInfo.filePath.empty())
		{
			continue;
		}

		//If the target file is contained within the same directory or a subdirectory of
		//the location where the system file is being saved, convert the path to the ROM
		//file into a relative path.
		std::wstring filePath = embeddedROMInfo.filePath;
		if(PathStartsWithBasePath(fileDir, filePath))
		{
			std::wstring relativePath;
			if(PathBuildRelativePathToTarget(fileDir, filePath, true, relativePath))
			{
				filePath = relativePath;
			}
		}

		//Create the embedded ROM selection node for this setting
		IHierarchicalStorageNode& loadROMDataNode = tree.GetRootNode().CreateChild(L"System.LoadEmbeddedROMData");
		loadROMDataNode.CreateAttribute(L"ModuleID", embeddedROMInfo.moduleID);
		loadROMDataNode.CreateAttribute(L"DeviceInstanceName", embeddedROMInfo.targetDevice->GetDeviceInstanceName());
		loadROMDataNode.CreateAttribute(L"InterfaceNumber", embeddedROMInfo.interfaceNumber);
		loadROMDataNode.CreateAttribute(L"FilePath", filePath);
	}
	embeddedROMLock.unlock();

	//Save XML tree to the target file
	Stream::File file(Stream::IStream::TextEncoding::UTF8);
	if(!file.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save system to file " + filePath + L" because there was an error creating the file at the full path of " + filePath + L"!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}
	file.InsertByteOrderMark();
	if(!tree.SaveTree(file))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to save system to file " + filePath + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
		if(running)
		{
			RunSystem();
		}
		return false;
	}

	//Log the event
	WriteLogEvent(LogEntry(LogEntry::EventLevel::Info, L"System", L"Saved system to file " + filePath));

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
	std::unique_lock<std::mutex> lock(moduleLoadMutex);

	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Unload the module
	UnloadModuleInternal(moduleID);

	//Attempt to validate the system now that the module has been unloaded
	bool result = ValidateSystem();

	//Restore running state
	if(running && result)
	{
		RunSystem();
	}

	//Notify any registered observers that the set of loaded modules has now changed
	lock.unlock();
	loadedModuleChangeObservers.NotifyObservers();

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
void System::UnloadModuleInternal(unsigned int moduleID)
{
	//Retrieve information on the target module
	LoadedModuleInfoMap::iterator loadedModuleIterator = loadedModuleInfoMap.find(moduleID);
	if(loadedModuleIterator == loadedModuleInfoMap.end())
	{
		return;
	}
	LoadedModuleInfoInternal& moduleInfo = loadedModuleIterator->second;

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
			std::wstring persistentModuleFilePath = PathCombinePaths(guiExtensionInterface.GetGlobalPreferencePathPersistentState(), persistentModuleFileName);
			if(!SavePersistentStateForModule(persistentModuleFilePath, moduleID, FileType::ZIP, true))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Failed to save persistent state data for module with name " + moduleInfo.displayName + L" to file \"" + persistentModuleFilePath + L"\"."));
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

	//Unregister any menu handlers currently bound to this module
	for(std::set<IExtension*>::const_iterator i = moduleInfo.menuHandlers.begin(); i != moduleInfo.menuHandlers.end(); ++i)
	{
		(*i)->UnregisterModuleMenuHandler(moduleID);
	}
	moduleInfo.menuHandlers.clear();

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
		LoadedGlobalExtensionInfo& extensionInfo = currentElement->second;
		++nextGlobalExtensionEntry;

		//Remove this module from the list of referencing modules for this global
		//extension
		extensionInfo.moduleIDs.erase(moduleID);

		//If the global extension is no longer referenced by any modules, remove it.
		if(!extensionInfo.persistentExtension && extensionInfo.moduleIDs.empty())
		{
			//Remove any references to this extension, IE, through ReferenceExtension.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(extensionInfo.extension);
			}
			for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
			{
				i->extension->RemoveReference(extensionInfo.extension);
			}

			//Delete the extension
			UnloadExtension(extensionInfo.extension);
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
	std::unique_lock<std::recursive_mutex> moduleSettingsLock(moduleSettingMutex);
	ModuleSystemSettingMap::iterator moduleSettingsEntry = moduleSettings.find(moduleID);
	if(moduleSettingsEntry != moduleSettings.end())
	{
		for(SystemSettingsIDList::const_iterator i = moduleSettingsEntry->second.begin(); i != moduleSettingsEntry->second.end(); ++i)
		{
			SystemSettingInfo* systemSettingsObject = systemSettings[*i];
			systemSettings.erase(*i);
			systemSettingsObjects.erase(systemSettingsObject);
			delete systemSettingsObject;
		}
		moduleSettings.erase(moduleSettingsEntry);
	}
	moduleSettingsLock.unlock();

	//Remove any embedded ROM entries defined by this module
	std::unique_lock<std::mutex> embeddedROMLock(embeddedROMMutex);
	std::map<unsigned int, EmbeddedROMInfoInternal>::iterator nextEmbeddedROMInfoEntry = embeddedROMInfoSet.begin();
	while(nextEmbeddedROMInfoEntry != embeddedROMInfoSet.end())
	{
		std::map<unsigned int, EmbeddedROMInfoInternal>::iterator currentElement = nextEmbeddedROMInfoEntry;
		++nextEmbeddedROMInfoEntry;
		if(currentElement->second.moduleID == moduleID)
		{
			embeddedROMInfoSet.erase(currentElement);
			++embeddedROMInfoLastModifiedToken;
		}
	}
	embeddedROMLock.unlock();

	//Remove this module from the list of loaded modules
	loadedModuleInfoMap.erase(moduleID);

	//Assert the specified unmapped line state for any unmapped lines in the system that
	//have an unmapped line state specified
	for(UnmappedLineStateList::const_iterator i = unmappedLineStateList.begin(); i != unmappedLineStateList.end(); ++i)
	{
		//Retrieve information on the module that contains the target device
		LoadedModuleInfoMap::const_iterator deviceLoadedModuleIterator = loadedModuleInfoMap.find(moduleID);
		if(deviceLoadedModuleIterator == loadedModuleInfoMap.end())
		{
			continue;
		}
		const LoadedModuleInfoInternal& deviceModuleInfo = deviceLoadedModuleIterator->second;

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
unsigned int System::GetFirstAvailableDeviceIndex() const
{
	bool deviceIndexFree;
	unsigned int deviceIndex;
	unsigned int nextDeviceIndexToCheck = 0;
	do
	{
		deviceIndex = nextDeviceIndexToCheck++;
		deviceIndexFree = true;
		for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
		{
			if(i->deviceContext->GetDeviceIndexNo() == deviceIndex)
			{
				deviceIndexFree = false;
				break;
			}
		}
	}
	while (!deviceIndexFree);
	return deviceIndex;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeModuleID() const
{
	bool moduleIDFree;
	unsigned int moduleID;
	do
	{
		moduleID = nextFreeModuleID++;
		moduleIDFree = (loadedModuleInfoMap.find(moduleID) == loadedModuleInfoMap.end());
	}
	while (!moduleIDFree);
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
		connectorIDFree = (connectorDetailsMap.find(connectorID) == connectorDetailsMap.end());
	}
	while (!connectorIDFree);
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
		lineGroupIDFree = (connectorDetailsMap.find(lineGroupID) == connectorDetailsMap.end());
	}
	while (!lineGroupIDFree);
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
		systemLineIDFree = (systemLines.find(systemLineID) == systemLines.end());
	}
	while (!systemLineIDFree);
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
		systemSettingIDFree = (systemSettings.find(systemSettingID) == systemSettings.end());
	}
	while (!systemSettingIDFree);
	return systemSettingID;
}

//----------------------------------------------------------------------------------------
unsigned int System::GenerateFreeEmbeddedROMID() const
{
	bool embeddedROMIDFree;
	unsigned int embeddedROMID;
	do
	{
		embeddedROMID = nextFreeEmbeddedROMID++;
		embeddedROMIDFree = (embeddedROMInfoSet.find(embeddedROMID) == embeddedROMInfoSet.end());
	}
	while (!embeddedROMIDFree);
	return embeddedROMID;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device class and instance names
	IHierarchicalStorageAttribute* deviceNameAttribute = node.GetAttribute(L"DeviceName");
	IHierarchicalStorageAttribute* instanceNameAttribute = node.GetAttribute(L"InstanceName");
	if((deviceNameAttribute == 0) || (instanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceName or InstanceName attribute for Device entry!"));
		return false;
	}
	std::wstring deviceName = deviceNameAttribute->GetValue();
	std::wstring instanceName = instanceNameAttribute->GetValue();

	//Retrieve the optional DisplayName attribute
	IHierarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	std::wstring displayName = instanceName;
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Create the new device object
	IDevice* device = CreateDevice(deviceName, instanceName, moduleID);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"CreateDevice failed for " + instanceName + L"!"));
		return false;
	}

	//Bind to the system interface
	if(!device->BindToSystemInterface(this))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToSystemInterface failed for  " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Create a new device context for this device
	DeviceContext* deviceContext = new DeviceContext(*device, *this);

	//Associate this device with the first available device index number
	unsigned int newDeviceIndexNumber = GetFirstAvailableDeviceIndex();
	deviceContext->SetDeviceIndexNo(newDeviceIndexNumber);

	//Bind our device to the device context object
	if(!device->BindToDeviceContext(deviceContext))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToDeviceContext failed for  " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Construct the device object
	if(!device->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Call BuildDevice() to perform any other required post-creation initialzation for the
	//device
	if(!device->BuildDevice())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BuildDevice failed for " + instanceName + L"!"));
		DestroyDevice(deviceName, device);
		return false;
	}

	//Add the device object to the system
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedDeviceInfo deviceInfo;
	deviceInfo.moduleID = moduleID;
	deviceInfo.device = device;
	deviceInfo.deviceContext = deviceContext;
	deviceInfo.name = instanceName;
	deviceInfo.displayName = displayName;
	loadedDeviceInfoList.push_back(deviceInfo);
	devices.push_back(deviceContext);
	executionManager.AddDevice(deviceContext);

	return true;

}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_SetDependentDevice(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device names
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	if((deviceInstanceNameAttribute == 0) || (targetInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName or TargetInstanceName attribute for Device.SetDependentDevice!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring targetName = targetInstanceNameAttribute->GetValue();

	//Retrieve the specified devices from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	IDevice* target = GetDevice(moduleID, targetName);
	if((device == 0) || (target == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate source device with name " + deviceName + L" or target device with name " + targetName + L" for Device.SetDependentDevice!"));
		return false;
	}

	//Set the target device as a dependent device
	DeviceContext* deviceContext = (DeviceContext*)device->GetDeviceContext();
	deviceContext->AddDeviceDependency((DeviceContext*)target->GetDeviceContext());

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceDevice(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device names, and reference name.
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (targetInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName, TargetInstanceName, or ReferenceName attribute for Device.ReferenceDevice!"));
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate source device with name " + deviceName + L" or target device with name " + targetName + L" for Device.ReferenceDevice!"));
		return false;
	}

	//Add the specified device reference to the device
	if(!device->AddReference(referenceName, target))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Device.ReferenceDevice failed for reference from " + deviceName + L" to " + targetName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceExtension(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the GlobalExtensionName or ExtensionInstanceName attributes
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* globalExtensionNameAttribute = node.GetAttribute(L"GlobalExtensionName");
	if((extensionInstanceNameAttribute == 0) && (globalExtensionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Either the ExtensionInstanceName or the GlobalExtensionName attribute must be specified for Device.ReferenceExtension!"));
		return false;
	}
	else if((extensionInstanceNameAttribute != 0) && (globalExtensionNameAttribute != 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The ExtensionInstanceName and GlobalExtensionName attributes cannot be used together for Device.ReferenceExtension!"));
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
			LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
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
			LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
			logEntry << L"Could not locate extension with name " << extensionName << L" in module " << moduleID << L" for Device.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}

	//Load the device name and reference name.
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName or ReferenceName attribute for Device.ReferenceExtension!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceExtension!"));
		return false;
	}

	//Add the specified extension reference to the device
	if(!device->AddReference(referenceName, extension))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Device.ReferenceExtension failed for reference from " + deviceName + L" to extension " + extensionName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceBus(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for Device.ReferenceBus!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for Device.ReferenceBus!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name and reference name.
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName or ReferenceName attribute for Device.ReferenceBus!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceBus!"));
		return false;
	}

	//Add the specified bus reference to the device
	if(!device->AddReference(referenceName, busInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Device.ReferenceBus failed for reference from " + deviceName + L" to bus " + busInterfaceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_ReferenceClockSource(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name, device name, and reference name.
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((deviceInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName, ClockSourceName, or ReferenceName attribute for Device.ReferenceClockSource!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceClockSource!"));
		return false;
	}

	//Retrieve the specified clock source from the system
	IClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate clock source with name " + clockSourceName + L" for Device.ReferenceClockSource!"));
		return false;
	}

	//Add the specified clock source reference to the device
	if(!device->AddReference(referenceName, clockSource))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Device.ReferenceClockSource failed for reference from " + deviceName + L" to " + clockSourceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device_RegisterInput(IHierarchicalStorageNode& node, unsigned int moduleID, std::list<InputRegistration>& inputRegistrationRequests)
{
	//Load the device name, system key code, and target key code.
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* deviceKeyCodeAttribute = node.GetAttribute(L"DeviceKeyCode");
	if((deviceInstanceNameAttribute == 0) || (deviceKeyCodeAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName or DeviceKeyCode attribute for Device.RegisterInput!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();
	std::wstring deviceKeyCodeString = deviceKeyCodeAttribute->GetValue();

	//Retrieve the specified device from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate target device with name " + deviceName + L" for Device.RegisterInput!"));
		return false;
	}

	//Translate the device key code
	unsigned int deviceKeyCode = device->GetKeyCodeID(deviceKeyCodeString);
	if(deviceKeyCode == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device key code with name " + deviceKeyCodeString + L" on device " + deviceName + L" for Device.RegisterInput!"));
		return false;
	}

	//Retrieve the preferred system key code attribute if one has been specified
	bool preferredSystemKeyCodeSpecified = false;
	KeyCode systemKeyCode = ISystemDeviceInterface::KeyCode::None;
	IHierarchicalStorageAttribute* systemKeyCodeAttribute = node.GetAttribute(L"PreferredSystemKeyCode");
	if(systemKeyCodeAttribute != 0)
	{
		//Extract the system key code string
		std::wstring systemKeyCodeString = systemKeyCodeAttribute->GetValue();

		//Translate the system key code
		systemKeyCode = GetKeyCodeID(systemKeyCodeString);
		if(systemKeyCode == ISystemDeviceInterface::KeyCode::None)
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate system key code with name " + systemKeyCodeString + L" for Device.RegisterInput!"));
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
bool System::LoadModule_GlobalExtension(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the extension class name
	IHierarchicalStorageAttribute* extensionNameAttribute = node.GetAttribute(L"ExtensionName");
	if(extensionNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionName attribute for GlobalExtension entry!"));
		return false;
	}
	std::wstring extensionName = extensionNameAttribute->GetValue();

	//If the specified global extension is already loaded, add the calling module to the
	//list of referencing modules, and abort any further processing.
	LoadedGlobalExtensionInfoList::iterator loadedGlobalExtensionInfoIterator = globalExtensionInfoList.find(extensionName);
	if(loadedGlobalExtensionInfoIterator != globalExtensionInfoList.end())
	{
		loadedGlobalExtensionInfoIterator->second.moduleIDs.insert(moduleID);
		return true;
	}

	//Create the new extension object
	IExtension* extension = CreateGlobalExtension(extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"CreateGlobalExtension failed for " + extensionName + L"!"));
		return false;
	}

	//Bind to the system interface
	if(!extension->BindToSystemInterface(this))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToSystemInterface failed for  " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Bind to the GUI interface
	if(!extension->BindToGUIInterface(&guiExtensionInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToGUIInterface failed for  " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Construct the extension object
	if(!extension->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Call BuildExtension() to perform any other required post-creation initialzation
	//for the extension.
	if(!extension->BuildExtension())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BuildExtension failed for " + extensionName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Record information on the loaded global extension, and add the specified module as a
	//referencing module.
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedGlobalExtensionInfo extensionInfo;
	extensionInfo.extension = extension;
	extensionInfo.name = extensionName;
	extensionInfo.persistentExtension = false;
	extensionInfo.moduleIDs.insert(moduleID);
	globalExtensionInfoList.insert(LoadedGlobalExtensionInfoListEntry(extensionInfo.name, extensionInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the extension class and instance names
	IHierarchicalStorageAttribute* extensionNameAttribute = node.GetAttribute(L"ExtensionName");
	IHierarchicalStorageAttribute* instanceNameAttribute = node.GetAttribute(L"InstanceName");
	if((extensionNameAttribute == 0) || (instanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionName or InstanceName attribute for Extension entry!"));
		return false;
	}
	std::wstring extensionName = extensionNameAttribute->GetValue();
	std::wstring instanceName = instanceNameAttribute->GetValue();

	//Create the new extension object
	IExtension* extension = CreateExtension(extensionName, instanceName, moduleID);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"CreateExtension failed for " + instanceName + L"!"));
		return false;
	}

	//Bind to the system interface
	if(!extension->BindToSystemInterface(this))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToSystemInterface failed for  " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Bind to the GUI interface
	if(!extension->BindToGUIInterface(&guiExtensionInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BindToGUIInterface failed for  " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Construct the extension object
	if(!extension->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Call BuildExtension() to perform any other required post-creation initialzation for
	//the extension.
	if(!extension->BuildExtension())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BuildExtension failed for " + instanceName + L"!"));
		DestroyExtension(extensionName, extension);
		return false;
	}

	//Save information on this loaded extension
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedExtensionInfo extensionInfo;
	extensionInfo.moduleID = moduleID;
	extensionInfo.extension = extension;
	extensionInfo.name = extension->GetExtensionInstanceName();
	loadedExtensionInfoList.push_back(extensionInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceDevice(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the ExtensionInstanceName attribute
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for Extension.ReferenceDevice!"));
		return false;
	}
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for Extension.ReferenceDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name and reference name.
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceDevice!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension object from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceDevice!"));
		return false;
	}

	//Add the specified device reference to the extension
	if(!extension->AddReference(referenceName, device))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Extension.ReferenceDevice failed for reference from " + extensionName + L" to device " + deviceInstanceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceExtension(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the GlobalExtensionName or ExtensionInstanceName attributes
	IHierarchicalStorageAttribute* targetInstanceNameAttribute = node.GetAttribute(L"TargetInstanceName");
	IHierarchicalStorageAttribute* targetGlobalExtensionNameAttribute = node.GetAttribute(L"TargetGlobalExtensionName");
	if((targetInstanceNameAttribute == 0) && (targetGlobalExtensionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Either the TargetInstanceName or the TargetGlobalExtensionName attribute must be specified for Extension.ReferenceExtension!"));
		return false;
	}
	else if((targetInstanceNameAttribute != 0) && (targetGlobalExtensionNameAttribute != 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The TargetInstanceName and TargetGlobalExtensionName attributes cannot be used together for Extension.ReferenceExtension!"));
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
			LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
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
			LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
			logEntry << L"Could not locate target extension with name " << targetExtensionName << L" in module " << moduleID << L" for Extension.ReferenceExtension!";
			WriteLogEvent(logEntry);
			return false;
		}
	}

	//Load the ExtensionInstanceName and ReferenceName attributes
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceExtension!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extensions from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate source extension with name " + extensionName + L" for Extension.ReferenceExtension!"));
		return false;
	}

	//Add the specified extension reference to the extension
	if(!extension->AddReference(referenceName, target))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Extension.ReferenceExtension failed for reference from " + extensionName + L" to " + targetExtensionName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceBus(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for Extension.ReferenceBus!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for Extension.ReferenceBus!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the extension name and reference name.
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionInstanceName or ReferenceName attribute for Extension.ReferenceBus!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension object from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceBus!"));
		return false;
	}

	//Add the specified bus reference to the device
	if(!extension->AddReference(referenceName, busInterface))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Extension.ReferenceBus failed for reference from " + extensionName + L" to bus " + busInterfaceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Extension_ReferenceClockSource(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name, device name, and reference name.
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHierarchicalStorageAttribute* referenceNameAttribute = node.GetAttribute(L"ReferenceName");
	if((extensionInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (referenceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionInstanceName, ClockSourceName, or ReferenceName attribute for Extension.ReferenceClockSource!"));
		return false;
	}
	std::wstring extensionName = extensionInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring referenceName = referenceNameAttribute->GetValue();

	//Retrieve the specified extension from the system
	IExtension* extension = GetExtension(moduleID, extensionName);
	if(extension == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate extension with name " + extensionName + L" for Extension.ReferenceClockSource!"));
		return false;
	}

	//Retrieve the specified clock source from the system
	IClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate clock source with name " + clockSourceName + L" for Extension.ReferenceClockSource!"));
		return false;
	}

	//Add the specified clock source reference to the device
	if(!extension->AddReference(referenceName, clockSource))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Extension.ReferenceClockSource failed for reference from " + extensionName + L" to " + clockSourceName + L" using reference name " + referenceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the bus name
	IHierarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Name attribute for BusInterface!"));
		return false;
	}
	std::wstring busInterfaceName = nameAttribute->GetValue();

	//Ensure a bus object with the specified name doesn't already exist in the system
	for(BusInterfaceList::const_iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		const LoadedBusInfo& busInfo = *i;
		if((busInfo.moduleID == moduleID) && (busInfo.name == busInterfaceName))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface with name of " + busInterfaceName + L" already exists!"));
			return false;
		}
	}

	//Create and construct a new bus interface object
	BusInterface* busInterface = new BusInterface();
	if(!busInterface->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for BusInterface " + busInterfaceName + L"!"));
		delete busInterface;
		return false;
	}

	//Add the bus interface object to the system
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedBusInfo busInfo;
	busInfo.busInterface = busInterface;
	busInfo.moduleID = moduleID;
	busInfo.name = busInterfaceName;
	busInterfaces.push_back(busInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_DefineLineGroup(IHierarchicalStorageNode& node, unsigned int moduleID, NameToIDMap& lineGroupNameToIDMap)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineLineGroup!"));
		return false;
	}

	//Extract the LineGroupName attribute
	IHierarchicalStorageAttribute* lineGroupNameAttribute = node.GetAttribute(L"LineGroupName");
	if(lineGroupNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing LineGroupName attribute for BusInterface.DefineLineGroup!"));
		return false;
	}
	std::wstring lineGroupName = lineGroupNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
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
bool System::LoadModule_BusInterface_DefineCELineMemory(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineCELineMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.DefineCELineMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->DefineCELineMemory(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.DefineCELineMemory failed on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_DefineCELinePort(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.DefineCELinePort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.DefineCELinePort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->DefineCELinePort(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.DefineCELinePort failed on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineInputMemory(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineInputMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineInputMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineInputMemory!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineInputMemory!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineInputMemory(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapCELineInputMemory failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineInputPort(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineInputPort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineInputPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineInputPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineInputPort!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineInputPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapCELineInputPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineOutputMemory(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineOutputMemory!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineOutputMemory!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineOutputMemory!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineOutputMemory!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineOutputMemory(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapCELineOutputMemory failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapCELineOutputPort(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Locate the bus interface name and module ID
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapCELineOutputPort!"));
		return false;
	}

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(busInterfaceModuleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << busInterfaceModuleID << L" for BusInterface.MapCELineOutputPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapCELineOutputPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapCELineOutputPort!"));
		return false;
	}

	//Define the CE line in the referenced bus interface
	if(!busInterface->MapCELineOutputPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapCELineOutputPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapDevice(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName for BusInterface.MapDevice!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapDevice!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapDevice!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapDevice(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapDevice failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapPort(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapPort!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapPort!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapPort!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapPort!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapPort(device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapPort failed for " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& lineGroupNameToIDMap)
{
	//Attempt to extract all possible attributes defining the source and target of the
	//line mapping.
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHierarchicalStorageAttribute* sourceDeviceNameAttribute = node.GetAttribute(L"SourceDeviceInstanceName");
	IHierarchicalStorageAttribute* sourceLineGroupNameAttribute = node.GetAttribute(L"SourceLineGroupName");
	IHierarchicalStorageAttribute* targetDeviceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHierarchicalStorageAttribute* targetLineGroupNameAttribute = node.GetAttribute(L"TargetLineGroupName");
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate source device with name \"" + sourceDeviceName + L"\" or target device with name \"" + targetDeviceName + L"\" or bus interface with name \"" + busInterfaceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Add the bus interface mapping
		if(!busInterface->MapLine(sourceDevice, targetDevice, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapLine failed for mapping from \"" + sourceDeviceName + L"\" to \"" + targetDeviceName + L"\" on bus \"" + busInterfaceName + L"\"!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate source device with name \"" + sourceDeviceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Find the ID for the referenced line group
		NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(targetLineGroupName);
		if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate line group with name \"" + targetLineGroupName + L"\" for BusInterface.MapLine!"));
			return false;
		}
		unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

		//Retrieve the target line group info
		LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
		if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not retrieve line group details for line group with name \"" + targetLineGroupName + L"\" in BusInterface.MapLine!"));
			return false;
		}
		const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

		//Retrieve the referenced bus interface
		BusInterface* busInterface = lineGroupDetails.busInterface;

		//Add the bus interface mapping
		if(!busInterface->MapLine(sourceDevice, lineGroupID, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapLine failed for mapping from device \"" + sourceDeviceName + L"\" to line group \"" + targetLineGroupName + L"\"!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate target device with name \"" + targetDeviceName + L"\" for BusInterface.MapLine!"));
			return false;
		}

		//Find the ID for the referenced line group
		NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(sourceLineGroupName);
		if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate line group with name \"" + sourceLineGroupName + L"\" for BusInterface.MapLine!"));
			return false;
		}
		unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

		//Retrieve the target line group info
		LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
		if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not retrieve line group details for line group with name \"" + sourceLineGroupName + L"\" in BusInterface.MapLine!"));
			return false;
		}
		const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

		//Retrieve the referenced bus interface
		BusInterface* busInterface = lineGroupDetails.busInterface;

		//Add the bus interface mapping
		if(!busInterface->MapLine(lineGroupID, targetDevice, node))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapLine failed for mapping from line group \"" + sourceLineGroupName + L"\" to device \"" + targetDeviceName + L"\"!"));
			return false;
		}
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"No valid combination of source and target mappings exists for BusInterface.MapLine!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_MapClockSource(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the BusInterfaceName attribute
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	if(busInterfaceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing BusInterfaceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name " << busInterfaceName << L" in module " << moduleID << L" for BusInterface.MapClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Extract the ClockSourceName attribute
	IHierarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	if(clockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ClockSourceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();

	//Retrieve the referenced clock source
	ClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate clock source with name " << clockSourceName << L" in module " << moduleID << L" for BusInterface.MapClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Load the device name
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	if(deviceInstanceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for BusInterface.MapClockSource!"));
		return false;
	}
	std::wstring deviceName = deviceInstanceNameAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + deviceName + L" for BusInterface.MapClockSource!"));
		return false;
	}

	//Add the bus interface mapping
	if(!busInterface->MapClockSource(clockSource, device, node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"BusInterface.MapClockSource failed when mapping clock source with name " + clockSourceName + L" to device with name " + deviceName + L" on bus " + busInterfaceName + L"!"));
		return false;
	}

	//Add a reference to the target bus interface to the clock source
	clockSource->AddReference(busInterface);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_BusInterface_UnmappedLineState(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract all required attributes
	IHierarchicalStorageAttribute* deviceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* targetLineAttribute = node.GetAttribute(L"TargetLine");
	IHierarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((deviceNameAttribute == 0) || (targetLineAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName, TargetLine, or Value attribute for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Load the device name and target line name attributes
	std::wstring deviceName = deviceNameAttribute->GetValue();
	std::wstring targetLineName = targetLineAttribute->GetValue();

	//Retrieve the device object from the system
	IDevice* device = GetDevice(moduleID, deviceName);
	if(device == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name \"" + deviceName + L"\" for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Obtain the line ID for the target line on the device
	unsigned int targetLineID = device->GetLineID(targetLineName);
	if(targetLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate line with name \"" + targetLineName + L"\" on device with name \"" + deviceName + L"\" for BusInterface.UnmappedLineState!"));
		return false;
	}

	//Obtain the bit count for the target line
	unsigned int lineBitCount = device->GetLineWidth(targetLineID);
	if(lineBitCount == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Target line with name \"" + targetLineName + L"\" on device with name \"" + deviceName + L"\" returned a bit count of 0 for BusInterface.UnmappedLineState!"));
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
bool System::LoadModule_ClockSource(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name
	IHierarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Name attribute for ClockSource!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"ClockSource with name of " + clockSourceName + L" already exists!"));
			return false;
		}
	}

	//Create and construct a new clock source object
	ClockSource* clockSource = new ClockSource();
	if(!clockSource->Construct(node))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Construct failed for ClockSource " + clockSourceName + L"!"));
		delete clockSource;
		return false;
	}

	//Add the clock source object to the system
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	LoadedClockSourceInfo clockSourceInfo;
	clockSourceInfo.clockSource = clockSource;
	clockSourceInfo.moduleID = moduleID;
	clockSourceInfo.name = clockSourceName;
	clockSources.push_back(clockSourceInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_ClockSource_SetInputClockSource(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the source clock source name
	IHierarchicalStorageAttribute* inputClockSourceNameAttribute = node.GetAttribute(L"InputClockSourceName");
	if(inputClockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing InputClockSourceName attribute for ClockSource.SetInputClockSource!"));
		return false;
	}
	std::wstring inputClockSourceName = inputClockSourceNameAttribute->GetValue();

	//Load the target clock source name
	IHierarchicalStorageAttribute* targetClockSourceNameAttribute = node.GetAttribute(L"TargetClockSourceName");
	if(targetClockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing TargetClockSourceName attribute for ClockSource.SetInputClockSource!"));
		return false;
	}
	std::wstring targetClockSourceName = targetClockSourceNameAttribute->GetValue();

	//Retrieve the referenced input clock source object
	ClockSource* inputClockSource = GetClockSource(moduleID, inputClockSourceName);
	if(inputClockSource == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate clock source with name " << inputClockSourceName << L" in module " << moduleID << L" for ClockSource.SetInputClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the referenced target clock source object
	ClockSource* targetClockSource = GetClockSource(moduleID, targetClockSourceName);
	if(targetClockSource == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate clock source with name " << targetClockSourceName << L" in module " << moduleID << L" for ClockSource.SetInputClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Attempt to add the target clock source as a dependent clock source of the input
	return inputClockSource->AddDependentClockSource(targetClockSource);
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_OpenView(IHierarchicalStorageNode& node, unsigned int moduleID, std::list<ViewOpenRequest>& viewOpenRequests)
{
	//Extract the Owner, ViewGroupName, and ViewName attributes
	IHierarchicalStorageAttribute* targetAttribute = node.GetAttribute(L"Target");
	IHierarchicalStorageAttribute* viewGroupAttribute = node.GetAttribute(L"ViewGroupName");
	IHierarchicalStorageAttribute* viewNameAttribute = node.GetAttribute(L"ViewName");
	if((targetAttribute == 0) || (viewGroupAttribute == 0) || (viewNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Owner, ViewGroupName, or ViewName attribute for System.OpenView!"));
		return false;
	}
	std::wstring target = targetAttribute->GetValue();
	std::wstring viewGroupName = viewGroupAttribute->GetValue();
	std::wstring viewName = viewNameAttribute->GetValue();

	if(target == L"System")
	{
		//Add the open request to the queue
		ViewOpenRequest openRequest;
		openRequest.viewTarget = IViewPresenter::ViewTarget::System;
		openRequest.viewGroupName = viewGroupName;
		openRequest.viewName = viewName;
		viewOpenRequests.push_back(openRequest);
	}
	else if(target == L"Module")
	{
		//Add the open request to the queue
		ViewOpenRequest openRequest;
		openRequest.viewTarget = IViewPresenter::ViewTarget::Module;
		openRequest.moduleID = moduleID;
		openRequest.viewGroupName = viewGroupName;
		openRequest.viewName = viewName;
		viewOpenRequests.push_back(openRequest);
	}
	else if(target == L"Device")
	{
		//Extract the device instance name attribute
		IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
		if(deviceInstanceNameAttribute == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing DeviceInstanceName attribute for System.OpenView!"));
			return false;
		}
		std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();

		//Add the open request to the queue
		ViewOpenRequest openRequest;
		openRequest.viewTarget = IViewPresenter::ViewTarget::Device;
		openRequest.moduleID = moduleID;
		openRequest.viewGroupName = viewGroupName;
		openRequest.viewName = viewName;
		openRequest.deviceInstanceName = deviceInstanceName;
		viewOpenRequests.push_back(openRequest);
	}
	else if(target == L"Extension")
	{
		//Extract the extension instance name attribute
		IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
		if(extensionInstanceNameAttribute == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ExtensionInstanceName attribute for System.OpenView!"));
			return false;
		}
		std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();

		//Extract the global extension attribute
		bool globalExtension = false;
		IHierarchicalStorageAttribute* globalExtensionAttribute = node.GetAttribute(L"GlobalExtension");
		if(globalExtensionAttribute != 0)
		{
			globalExtension = globalExtensionAttribute->ExtractValue<bool>();
		}

		//Add the open request to the queue
		ViewOpenRequest openRequest;
		openRequest.viewTarget = IViewPresenter::ViewTarget::Device;
		openRequest.moduleID = moduleID;
		openRequest.viewGroupName = viewGroupName;
		openRequest.viewName = viewName;
		openRequest.extensionInstanceName = extensionInstanceName;
		openRequest.globalExtension = globalExtension;
		viewOpenRequests.push_back(openRequest);
	}
	else
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Invalid view target attribute of \"" + target + L"\" was specified for System.OpenView!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportConnector(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, NameToIDMap& connectorNameToIDMap)
{
	//Extract the BusInterfaceName, ConnectorClassName, and ConnectorInstanceName attributes
	IHierarchicalStorageAttribute* connectorClassNameAttribute = node.GetAttribute(L"ConnectorClassName");
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	if((connectorClassNameAttribute == 0) || (connectorInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorClassName or ConnectorInstanceName attribute for System.ExportConnector!"));
		return false;
	}
	std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();

	//Add details of this connector to the list of connectors which have been defined
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
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
bool System::LoadModule_System_ExportDevice(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, DeviceInstanceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (deviceInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ExportDevice!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for System.ExportDevice!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportDevice!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportDevice!"));
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
bool System::LoadModule_System_ExportExtension(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ExtensionInstanceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (extensionInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, ExtensionInstanceName, or ImportName attribute for System.ExportExtension!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced extension
	IExtension* extension = GetExtension(moduleID, extensionInstanceName);
	if(extension == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate extension with name " << extensionInstanceName << L" in module " << moduleID << L" for System.ExportExtension!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportExtension!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportExtension!"));
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
bool System::LoadModule_System_ExportBusInterface(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, const NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ExportBusInterface!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced bus interface
	BusInterface* busInterface = GetBusInterface(moduleID, busInterfaceName);
	if(busInterface == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate bus interface with name \"" << busInterfaceName << L"\" in module " << moduleID << L" for System.ExportBusInterface!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name \"" + connectorInstanceName + L"\" for System.ExportBusInterface!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name \"" + connectorInstanceName + L"\" for System.ExportBusInterface!"));
		return false;
	}
	ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Build the info about this exported bus interface
	ExportedBusInfo exportedBusInfo;
	exportedBusInfo.busInterface = busInterface;
	exportedBusInfo.exportingModuleBusInterfaceName = busInterfaceName;
	exportedBusInfo.importName = importName;

	//Process any child elements of this node
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageNode& childNode = *(*i);
		std::wstring elementName = childNode.GetName();
		if(elementName == L"ExportLineGroup")
		{
			//Extract the LineGroupName and ImportName attributes
			IHierarchicalStorageAttribute* childLineGroupNameAttribute = childNode.GetAttribute(L"LineGroupName");
			IHierarchicalStorageAttribute* childImportNameAttribute = childNode.GetAttribute(L"ImportName");
			if((childLineGroupNameAttribute == 0) || (childImportNameAttribute == 0))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing LineGroupName or ImportName attribute for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			std::wstring lineGroupName = childLineGroupNameAttribute->GetValue();
			std::wstring importName = childImportNameAttribute->GetValue();

			//Find the ID for the referenced line group
			NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(lineGroupName);
			if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate line group with name \"" + lineGroupName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

			//Retrieve the target line group info
			LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
			if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not retrieve line group details for line group with name \"" + lineGroupName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
				return false;
			}
			const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

			//Verify that the specified line group belongs to the BusInterface object
			//being exported.
			if(lineGroupDetails.busInterface != busInterface)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The specified line group with name \"" + lineGroupName + L"\" does not belong to the specified bus interface with name \"" + busInterfaceName + L"\" for ExportLineGroup on System.ExportBusInterface!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Unrecognized child element: \"" + elementName + L"\" encountered when processing System.ExportBusInterface!"));
			return false;
		}
	}

	//Add details of this exported object to the connector details
	connectorDetails.exportedBusInfo.insert(std::pair<std::wstring, ExportedBusInfo>(exportedBusInfo.importName, exportedBusInfo));

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ExportClockSource(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ClockSourceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ExportClockSource!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the referenced clock source
	ClockSource* clockSource = GetClockSource(moduleID, clockSourceName);
	if(clockSource == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate clock source with name " << clockSourceName << L" in module " << moduleID << L" for System.ExportClockSource!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportClockSource!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportClockSource!"));
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
bool System::LoadModule_System_ExportSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemLineName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemLineNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, SystemLineName, or ImportName attribute for System.ExportSystemLine!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the ID number for the referenced system line
	unsigned int systemLineID = GetSystemLineID(moduleID, systemLineName);
	if(systemLineID == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate system line with name " << systemLineName << L" in module " << moduleID << L" for System.ExportSystemLine!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportSystemLine!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportSystemLine!"));
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
bool System::LoadModule_System_ExportSystemSetting(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemSettingName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* systemSettingNameAttribute = node.GetAttribute(L"SystemSettingName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemSettingNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, SystemSettingName, or ImportName attribute for System.ExportSystemSetting!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemSettingName = systemSettingNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the ID number for the referenced system setting
	std::unique_lock<std::recursive_mutex> moduleSettingsLock(moduleSettingMutex);
	unsigned int systemSettingID = GetSystemSettingID(moduleID, systemSettingName);
	if(systemSettingID == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate system setting with name " << systemSettingName << L" in module " << moduleID << L" for System.ExportSystemSetting!";
		WriteLogEvent(logEntry);
		return false;
	}
	moduleSettingsLock.unlock();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ExportSystemSetting!"));
		return false;
	}

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorNameToIDMapIterator->second);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ExportSystemSetting!"));
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
bool System::LoadModule_System_ImportConnector(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& systemClassName, const ConnectorMappingList& connectorMappings, NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorClassName and ConnectorInstanceName attributes
	IHierarchicalStorageAttribute* connectorClassNameAttribute = node.GetAttribute(L"ConnectorClassName");
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	if((connectorClassNameAttribute == 0) || (connectorInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorClassName or ConnectorInstanceName attribute for System.ImportConnector!"));
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
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"A connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the connector class name or system class name didn't match, while processing System.ImportConnector!"));
				}
				else if(connectorDetails.connectorUsed)
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"A connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the connector was already in use, while processing System.ImportConnector!"));
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"No connector mapping was specified for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\" while processing System.ImportConnector!"));
	}
	if(foundConnectorMapping && !connectorImportSucceeded)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"A connector import mapping was found for connector of class \"" + connectorClassName + L"\" with instance name \"" + connectorInstanceName + L"\", however the target connector was already in use when processing System.ImportConnector!"));
	}

	//Return the result of the connector import operation
	return foundConnectorMapping && connectorImportSucceeded;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportDevice(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, DeviceInstanceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (deviceInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ImportDevice!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported device
	std::map<std::wstring, ExportedDeviceInfo>::const_iterator exportedDeviceInfoIterator = connectorDetails.exportedDeviceInfo.find(importName);
	if(exportedDeviceInfoIterator == connectorDetails.exportedDeviceInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported device with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportDevice!"));
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
bool System::LoadModule_System_ImportExtension(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ExtensionInstanceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* extensionInstanceNameAttribute = node.GetAttribute(L"ExtensionInstanceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (extensionInstanceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, ExtensionInstanceName, or ImportName attribute for System.ImportExtension!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported extension
	std::map<std::wstring, ExportedExtensionInfo>::const_iterator exportedExtensionInfoIterator = connectorDetails.exportedExtensionInfo.find(importName);
	if(exportedExtensionInfoIterator == connectorDetails.exportedExtensionInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported extension with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportExtension!"));
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
bool System::LoadModule_System_ImportBusInterface(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ImportBusInterface!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring busInterfaceName = busInterfaceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported bus interface
	std::map<std::wstring, ExportedBusInfo>::const_iterator exportedBusInfoIterator = connectorDetails.exportedBusInfo.find(importName);
	if(exportedBusInfoIterator == connectorDetails.exportedBusInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported bus interface with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportBusInterface!"));
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
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageNode& childNode = *(*i);
		std::wstring elementName = childNode.GetName();
		if(elementName == L"ImportLineGroup")
		{
			//Extract the LineGroupName and ImportName attributes
			IHierarchicalStorageAttribute* childLineGroupNameAttribute = childNode.GetAttribute(L"LineGroupName");
			IHierarchicalStorageAttribute* childImportNameAttribute = childNode.GetAttribute(L"ImportName");
			if((childLineGroupNameAttribute == 0) || (childImportNameAttribute == 0))
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing LineGroupName or ImportName attribute for ImportLineGroup on System.ImportBusInterface!"));
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
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate exported line group with name " + lineGroupName + L" on exported bus interface with name " + busInterfaceName + L" for ImportLineGroup on System.ImportBusInterface!"));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Unrecognized child element: " + elementName + L" encountered when processing System.ImportBusInterface!"));
			return false;
		}
	}

	//Store the details of this imported bus interface
	importedBusInterfaces.push_back(importedBusInfo);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_ImportClockSource(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, ClockSourceName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* clockSourceNameAttribute = node.GetAttribute(L"ClockSourceName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (clockSourceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ImportClockSource!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring clockSourceName = clockSourceNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported clock source
	std::map<std::wstring, ExportedClockSourceInfo>::const_iterator exportedClockSourceInfoIterator = connectorDetails.exportedClockSourceInfo.find(importName);
	if(exportedClockSourceInfoIterator == connectorDetails.exportedClockSourceInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported clock source with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportClockSource!"));
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
bool System::LoadModule_System_ImportSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemLineName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemLineNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, SystemLineName, or ImportName attribute for System.ImportSystemLine!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported system line
	std::map<std::wstring, ExportedSystemLineInfo>::const_iterator exportedSystemLineInfoIterator = connectorDetails.exportedSystemLineInfo.find(importName);
	if(exportedSystemLineInfoIterator == connectorDetails.exportedSystemLineInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported system line with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportSystemLine!"));
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
bool System::LoadModule_System_ImportSystemSetting(IHierarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap)
{
	//Extract the ConnectorInstanceName, SystemOptionName, and ImportName attributes
	IHierarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHierarchicalStorageAttribute* systemSettingNameAttribute = node.GetAttribute(L"SystemSettingName");
	IHierarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (systemSettingNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing ConnectorInstanceName, SystemSettingName, or ImportName attribute for System.ImportSystemSetting!"));
		return false;
	}
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
	std::wstring systemSettingName = systemSettingNameAttribute->GetValue();
	std::wstring importName = importNameAttribute->GetValue();

	//Retrieve the connector ID for the referenced connector
	NameToIDMap::const_iterator connectorNameToIDMapIterator = connectorNameToIDMap.find(connectorInstanceName);
	if(connectorNameToIDMapIterator == connectorNameToIDMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find referenced connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
		return false;
	}
	unsigned int connectorID = connectorNameToIDMapIterator->second;

	//Retrieve the details for the referenced connector
	ConnectorDetailsMap::iterator connectorDetailsIterator = connectorDetailsMap.find(connectorID);
	if(connectorDetailsIterator == connectorDetailsMap.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error retrieving connector details for connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
		return false;
	}
	const ConnectorInfoInternal& connectorDetails = connectorDetailsIterator->second;

	//Retrieve the details of the target exported system line
	std::map<std::wstring, ExportedSystemSettingInfo>::const_iterator exportedSystemSettingInfoIterator = connectorDetails.exportedSystemSettingInfo.find(importName);
	if(exportedSystemSettingInfoIterator == connectorDetails.exportedSystemSettingInfo.end())
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not find exported system setting with import name " + importName + L" on connector with name " + connectorInstanceName + L" for System.ImportSystemSetting!"));
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
bool System::LoadModule_System_DefineEmbeddedROM(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract all the required attributes
	IHierarchicalStorageAttribute* embeddedROMNameAttribute = node.GetAttribute(L"EmbeddedROMName");
	IHierarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHierarchicalStorageAttribute* interfaceNumberAttribute = node.GetAttribute(L"InterfaceNumber");
	IHierarchicalStorageAttribute* romRegionSizeAttribute = node.GetAttribute(L"ROMRegionSize");
	IHierarchicalStorageAttribute* romEntryBitCountAttribute = node.GetAttribute(L"ROMEntryBitCount");
	if((embeddedROMNameAttribute == 0) || (deviceInstanceNameAttribute == 0) || (interfaceNumberAttribute == 0) || (romRegionSizeAttribute == 0) || (romEntryBitCountAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing EmbeddedROMName, TargetDeviceInstanceName, InterfaceNumber, ROMRegionSize, or ROMEntryBitCount attribute for System.DefineEmbeddedROM!"));
		return false;
	}
	std::wstring embeddedROMName = embeddedROMNameAttribute->GetValue();
	std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
	unsigned int interfaceNumber = interfaceNumberAttribute->ExtractValue<unsigned int>();
	unsigned int romRegionSize = romRegionSizeAttribute->ExtractHexValue<unsigned int>();
	unsigned int romEntryBitCount = romEntryBitCountAttribute->ExtractValue<unsigned int>();

	//Retrieve the optional DisplayName attribute
	IHierarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	std::wstring displayName = embeddedROMName;
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Retrieve the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		LogEntry logEntry(LogEntry::EventLevel::Error, L"System", L"");
		logEntry << L"Could not locate device with name " << deviceInstanceName << L" in module " << moduleID << L" for System.DefineEmbeddedROM!";
		WriteLogEvent(logEntry);
		return false;
	}

	//Generate an ID number for this embedded ROM
	std::unique_lock<std::mutex> embeddedROMLock(embeddedROMMutex);
	unsigned int embeddedROMID = GenerateFreeEmbeddedROMID();

	//Record details of this embedded ROM
	EmbeddedROMInfoInternal embeddedROM;
	embeddedROM.moduleID = moduleID;
	embeddedROM.embeddedROMName = embeddedROMName;
	embeddedROM.displayName = displayName;
	embeddedROM.targetDevice = device;
	embeddedROM.interfaceNumber = interfaceNumber;
	embeddedROM.romRegionSize = romRegionSize;
	embeddedROM.romEntryBitCount = romEntryBitCount;
	embeddedROMInfoSet[embeddedROMID] = embeddedROM;

	//Update the last modified token for embedded ROM data
	++embeddedROMInfoLastModifiedToken;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_DefineSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the Name and Width attributes
	IHierarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	IHierarchicalStorageAttribute* widthAttribute = node.GetAttribute(L"Width");
	if((nameAttribute == 0) || (widthAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Name or Width attribute for System.DefineSystemLine!"));
		return false;
	}
	std::wstring lineName = nameAttribute->GetValue();
	unsigned int lineWidth = widthAttribute->ExtractValue<unsigned int>();

	//Validate the supplied line width
	if(lineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The system line with name \"" + lineName + L"\" was declared with a specified width of 0 for System.DefineSystemLine!"));
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
bool System::LoadModule_System_MapSystemLine(IHierarchicalStorageNode& node, unsigned int moduleID)
{
	//Extract the Name and Width attributes
	IHierarchicalStorageAttribute* sourceSystemLineNameAttribute = node.GetAttribute(L"SourceSystemLineName");
	IHierarchicalStorageAttribute* targetDeviceInstanceNameAttribute = node.GetAttribute(L"TargetDeviceInstanceName");
	IHierarchicalStorageAttribute* targetLineAttribute = node.GetAttribute(L"TargetLine");
	if((sourceSystemLineNameAttribute == 0) || (targetDeviceInstanceNameAttribute == 0) || (targetLineAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing SourceSystemLineName, TargetDeviceInstanceName, or TargetLine attribute for System.MapSystemLine!"));
		return false;
	}
	std::wstring systemLineName = sourceSystemLineNameAttribute->GetValue();
	std::wstring targetDeviceName = targetDeviceInstanceNameAttribute->GetValue();
	std::wstring targetLineName = targetLineAttribute->GetValue();

	//Extract the LineMapping attribute if one has been specified
	bool lineMappingSpecified = false;
	std::wstring lineMapping;
	IHierarchicalStorageAttribute* lineMappingAttribute = node.GetAttribute(L"LineMapping");
	if(lineMappingAttribute != 0)
	{
		lineMappingSpecified = true;
		lineMapping = lineMappingAttribute->GetValue();
	}

	//Retrieve the target device object from the system
	IDevice* targetDevice = GetDevice(moduleID, targetDeviceName);
	if(targetDevice == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate target device with name \"" + targetDeviceName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the ID number for the system line
	unsigned int systemLineID = GetSystemLineID(moduleID, systemLineName);
	if(systemLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to locate system line with name \"" + systemLineName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the width of the system line
	unsigned int systemLineWidth = GetSystemLineWidth(systemLineID);
	if(systemLineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The system line with name \"" + systemLineName + L"\" returned a line width of 0 for System.MapSystemLine!"));
		return false;
	}

	//Obtain the ID number for the target line
	unsigned int targetLineID = targetDevice->GetLineID(targetLineName);
	if(targetLineID == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to locate line with name \"" + targetLineName + L"\" on device with name \"" + targetDeviceName + L"\" for System.MapSystemLine!"));
		return false;
	}

	//Obtain the width of the target line
	unsigned int targetLineWidth = targetDevice->GetLineWidth(targetLineID);
	if(targetLineWidth == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"The line with name \"" + targetLineName + L"\" on device with name \"" + targetDeviceName + L"\" returned a line width of 0 for System.MapSystemLine!"));
		return false;
	}

	//Extract any line bitmasks that have been specified
	unsigned int lineMaskAND = 0xFFFFFFFF;
	IHierarchicalStorageAttribute* lineMaskANDAttribute = node.GetAttribute(L"ANDMask");
	if(lineMaskANDAttribute != 0)
	{
		lineMaskAND = lineMaskANDAttribute->ExtractHexValue<unsigned int>();
	}
	unsigned int lineMaskOR = 0;
	IHierarchicalStorageAttribute* lineMaskORAttribute = node.GetAttribute(L"ORMask");
	if(lineMaskORAttribute != 0)
	{
		lineMaskOR = lineMaskORAttribute->ExtractHexValue<unsigned int>();
	}
	unsigned int lineMaskXOR = 0;
	IHierarchicalStorageAttribute* lineMaskXORAttribute = node.GetAttribute(L"XORMask");
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
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to load the system line with name \"" + systemLineName + L"\" because an error occurred attempting to construct a DataRemapTable from the LineMapping attribute for System.MapSystemLine!"));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_Setting(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName)
{
	//Extract the Name attribute
	IHierarchicalStorageAttribute* settingNameAttribute = node.GetAttribute(L"Name");
	if(settingNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Name attribute for System.Setting!"));
		return false;
	}
	std::wstring settingName = settingNameAttribute->GetValue();

	//Build the display name
	std::wstring displayName = settingName;
	IHierarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Extract any supplied optional lead-in time attributes
	bool settingChangeLeadInTimeRandom = false;
	double settingChangeLeadInTime = 0;
	double settingChangeLeadInTimeEnd = 0;
	IHierarchicalStorageAttribute* settingChangeLeadInTimeAttribute = node.GetAttribute(L"SettingChangeLeadInTime");
	IHierarchicalStorageAttribute* settingChangeLeadInRandomTimeRangeBeginAttribute = node.GetAttribute(L"SettingChangeLeadInRandomTimeRangeBegin");
	IHierarchicalStorageAttribute* settingChangeLeadInRandomTimeRangeEndAttribute = node.GetAttribute(L"SettingChangeLeadInRandomTimeRangeEnd");
	if(((settingChangeLeadInTimeAttribute != 0) && ((settingChangeLeadInRandomTimeRangeBeginAttribute != 0) || (settingChangeLeadInRandomTimeRangeEndAttribute != 0)))
	|| ((settingChangeLeadInRandomTimeRangeBeginAttribute != 0) != (settingChangeLeadInRandomTimeRangeEndAttribute != 0)))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"An invalid combination of lead in time attributes were specified for setting with name : " + settingName + L" when loading System.Setting node under module file " + fileName + L"."));
		return false;
	}
	if(settingChangeLeadInTimeAttribute != 0)
	{
		settingChangeLeadInTimeRandom = false;
		settingChangeLeadInTime = settingChangeLeadInTimeAttribute->ExtractValue<double>();
	}
	if((settingChangeLeadInRandomTimeRangeBeginAttribute != 0) && (settingChangeLeadInRandomTimeRangeEndAttribute != 0))
	{
		settingChangeLeadInTimeRandom = true;
		settingChangeLeadInTime = settingChangeLeadInRandomTimeRangeBeginAttribute->ExtractValue<double>();
		settingChangeLeadInTimeEnd = settingChangeLeadInRandomTimeRangeEndAttribute->ExtractValue<double>();
		if(settingChangeLeadInTimeEnd < settingChangeLeadInTime)
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"The start lead in time was greater than the end lead in time for setting with name : " + settingName + L" when loading System.Setting node under module file " + fileName + L"."));
			return false;
		}
	}

	//##TODO## Finish migrating over all other setting attributes to discrete variables as
	//per the above
	//Extract any optional toggle setting attributes
	bool toggleSetting = false;
	IHierarchicalStorageAttribute* toggleSettingAttribute = node.GetAttribute(L"ToggleSetting");
	if(toggleSettingAttribute != 0)
	{
		toggleSetting = toggleSettingAttribute->ExtractValue<bool>();
	}

	//Extract any optional toggle setting auto-revert attributes
	bool toggleSettingAutoRevert = false;
	bool toggleSettingAutoRevertTimeRandom = false;
	double toggleSettingAutoRevertTime = 0;
	double toggleSettingAutoRevertTimeEnd = 0;
	if(toggleSetting)
	{
		IHierarchicalStorageAttribute* toggleSettingAutoRevertAttribute = node.GetAttribute(L"ToggleSettingAutoRevert");
		IHierarchicalStorageAttribute* toggleSettingAutoRevertTimeAttribute = node.GetAttribute(L"ToggleSettingAutoRevertTime");
		IHierarchicalStorageAttribute* toggleSettingAutoRevertRandomTimeRangeBeginAttribute = node.GetAttribute(L"ToggleSettingAutoRevertRandomTimeRangeBegin");
		IHierarchicalStorageAttribute* toggleSettingAutoRevertRandomTimeRangeEndAttribute = node.GetAttribute(L"ToggleSettingAutoRevertRandomTimeRangeEnd");
		if((toggleSettingAutoRevertAttribute != 0) && (((toggleSettingAutoRevertTimeAttribute == 0) && (toggleSettingAutoRevertRandomTimeRangeBeginAttribute == 0) && (toggleSettingAutoRevertRandomTimeRangeEndAttribute == 0))
		|| ((toggleSettingAutoRevertTimeAttribute != 0) && ((toggleSettingAutoRevertRandomTimeRangeBeginAttribute != 0) || (toggleSettingAutoRevertRandomTimeRangeEndAttribute != 0)))
		|| ((toggleSettingAutoRevertRandomTimeRangeBeginAttribute != 0) != (toggleSettingAutoRevertRandomTimeRangeEndAttribute != 0))))
		{
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"An invalid combination of toggle setting auto revert attributes were specified for setting with name : " + settingName + L" when loading System.Setting node under module file " + fileName + L"."));
			return false;
		}
		if(toggleSettingAutoRevertAttribute != 0)
		{
			toggleSettingAutoRevert = toggleSettingAutoRevertAttribute->ExtractValue<bool>();
			if(toggleSettingAutoRevertTimeAttribute != 0)
			{
				toggleSettingAutoRevertTimeRandom = false;
				toggleSettingAutoRevertTime = toggleSettingAutoRevertTimeAttribute->ExtractValue<double>();
			}
			else
			{
				toggleSettingAutoRevertTimeRandom = true;
				toggleSettingAutoRevertTime = toggleSettingAutoRevertRandomTimeRangeBeginAttribute->ExtractValue<double>();
				toggleSettingAutoRevertTimeEnd = toggleSettingAutoRevertRandomTimeRangeEndAttribute->ExtractValue<double>();
				if(toggleSettingAutoRevertTimeEnd < toggleSettingAutoRevertTime)
				{
					WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"The start auto revert time was greater than the end auto revert time for setting with name : " + settingName + L" when loading System.Setting node under module file " + fileName + L"."));
					return false;
				}
			}
		}
	}

	//Load the child elements from this setting node
	unsigned int defaultOption = 0;
	unsigned int toggleSettingOnOptionIndex;
	bool toggleSettingOnOptionIndexDefined = false;
	std::vector<SystemSettingOption> options;
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		bool loadedSettingOptionSuccessfully = false;
		SystemSettingOption settingOption;
		bool thisSettingIsDefaultOption = false;
		bool toggleSettingOnOption = false;
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"System.Setting.Option")
		{
			loadedSettingOptionSuccessfully = LoadModule_System_Setting_Option(*(*i), moduleID, fileName, settingOption, thisSettingIsDefaultOption, toggleSettingOnOption);
		}
		else
		{
			//Log a warning for an unrecognized element
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting node under module file " + fileName + L"."));
			continue;
		}

		//If this option defines the on state for a toggle setting, latch this option
		//index.
		if(toggleSettingOnOption)
		{
			toggleSettingOnOptionIndex = (unsigned int)options.size();
			toggleSettingOnOptionIndexDefined = true;
		}

		//If we managed to load the setting option node successfully, record its
		//information within this system setting, otherwise log an error and return false.
		if(loadedSettingOptionSuccessfully)
		{
			//Store the index for this new option as the default option for this setting
			//if it was flagged as the default option
			if(thisSettingIsDefaultOption)
			{
				defaultOption = (unsigned int)options.size();
			}

			//Store this option within this system setting
			options.push_back(settingOption);
		}
		else
		{
			//Log an error if we failed to load an element correctly, and return false.
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting node under module file " + fileName + L"."));
			return false;
		}
	}

	//If this setting is a toggle setting, and there are more or less than 2 options
	//defined for this setting, log an error, and return false.
	unsigned int optionCount = (unsigned int)options.size();
	if(toggleSetting && (optionCount != 2))
	{
		LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
		logEntry << L"A total of " << optionCount << L" options were found for a toggle setting when loading System.Setting node under module file " << fileName << L". Only 2 options can be defined for a toggle setting.";
		WriteLogEvent(logEntry);
		return false;
	}

	//If this setting is a toggle setting, and no option was defined as the on state for
	//the toggle setting, log an error, and return false.
	if(toggleSetting && !toggleSettingOnOptionIndexDefined)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"No options set the ToggleSettingOnOption attribute for toggle setting with name : " + settingName + L" when loading System.Setting node under module file " + fileName + L"."));
		return false;
	}

	//If this setting is a toggle setting, set the option index numbers for the on and off
	//options.
	unsigned int onOption = 0;
	unsigned int offOption = 0;
	if(toggleSetting)
	{
		onOption = toggleSettingOnOptionIndex;
		offOption = (toggleSettingOnOptionIndex != 0)? 0: 1;
	}

	//Generate an ID number for this system setting
	std::unique_lock<std::recursive_mutex> moduleSettingsLock(moduleSettingMutex);
	unsigned int systemSettingID = GenerateFreeSystemSettingID();

	//Populate the system setting object with this setting info
	SystemSettingInfo* setting = new SystemSettingInfo();
	setting->systemSettingID = systemSettingID;
	setting->moduleID = moduleID;
	setting->name = settingName;
	setting->displayName = displayName;
	setting->settingChangeLeadInTime = settingChangeLeadInTime;
	setting->settingChangeLeadInTimeEnd = settingChangeLeadInTimeEnd;
	setting->settingChangeLeadInTimeRandom = settingChangeLeadInTimeRandom;
	setting->toggleSetting = toggleSetting;
	setting->toggleSettingAutoRevert = toggleSettingAutoRevert;
	setting->toggleSettingAutoRevertTimeRandom = toggleSettingAutoRevertTimeRandom;
	setting->toggleSettingAutoRevertTime = toggleSettingAutoRevertTime;
	setting->toggleSettingAutoRevertTimeEnd = toggleSettingAutoRevertTimeEnd;
	setting->defaultOption = defaultOption;
	setting->onOption = onOption;
	setting->offOption = offOption;
	setting->options = options;

	//Add this system setting to the set of defined system settings
	systemSettingsObjects.insert(setting);
	systemSettings.insert(SystemSettingsMapEntry(setting->systemSettingID, setting));
	moduleSettings[moduleID].push_back(setting->systemSettingID);

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_Setting_Option(IHierarchicalStorageNode& node, unsigned int moduleID, const std::wstring& fileName, SystemSettingOption& option, bool& defaultOption, bool& toggleSettingOnOption)
{
	//Extract the Name attribute
	IHierarchicalStorageAttribute* optionNameAttribute = node.GetAttribute(L"Name");
	if(optionNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing Name attribute for System.Setting.Option!"));
		return false;
	}
	std::wstring optionName = optionNameAttribute->GetValue();

	//Build the display name
	std::wstring displayName = optionName;
	IHierarchicalStorageAttribute* displayNameAttribute = node.GetAttribute(L"DisplayName");
	if(displayNameAttribute != 0)
	{
		displayName = displayNameAttribute->GetValue();
	}

	//Extract the Default attribute
	defaultOption = false;
	IHierarchicalStorageAttribute* defaultAttribute = node.GetAttribute(L"Default");
	if(defaultAttribute != 0)
	{
		defaultOption = defaultAttribute->ExtractValue<bool>();
	}

	//Extract the ToggleSettingOnOption attribute
	toggleSettingOnOption = false;
	IHierarchicalStorageAttribute* toggleSettingOnOptionAttribute = node.GetAttribute(L"ToggleSettingOnOption");
	if(toggleSettingOnOptionAttribute != 0)
	{
		toggleSettingOnOption = toggleSettingOnOptionAttribute->ExtractValue<bool>();
	}

	//Populate the system setting option object with this option info
	option.name = optionName;
	option.displayName = displayName;

	//Load the child elements from this option node
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting.Option node under module file " + fileName + L"."));
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
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Unrecognized child element: " + elementName + L" when loading System.Setting.Option node under module file " + fileName + L"."));
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SelectSettingOption(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the SettingName and OptionName attributes
	IHierarchicalStorageAttribute* settingNameAttribute = node.GetAttribute(L"SettingName");
	IHierarchicalStorageAttribute* optionNameAttribute = node.GetAttribute(L"OptionName");
	if((settingNameAttribute == 0) || (optionNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing SettingName or OptionName attribute for System.SelectSettingOption!"));
		return false;
	}
	std::wstring settingName = settingNameAttribute->GetValue();
	std::wstring optionName = optionNameAttribute->GetValue();

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SystemStateChangeType::SetSystemOption;
	stateChange.targetElementName = settingName;
	stateChange.setSystemOptionValue = optionName;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SetClockFrequency(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the TargetClockName, ClockType, and Value attributes
	IHierarchicalStorageAttribute* targetClockNameAttribute = node.GetAttribute(L"TargetClockName");
	IHierarchicalStorageAttribute* clockTypeAttribute = node.GetAttribute(L"ClockType");
	IHierarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((targetClockNameAttribute == 0) || (clockTypeAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing TargetClockName, ClockType, or Value attribute for System.SetClockFrequency!"));
		return false;
	}
	std::wstring targetClockName = targetClockNameAttribute->GetValue();
	std::wstring clockTypeString = clockTypeAttribute->GetValue();
	double clockRate = valueAttribute->ExtractValue<double>();

	//Decode the clock type string
	ClockSource::ClockType clockType;
	if(!ClockSource::DecodeClockTypeString(clockTypeString, clockType))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Failed to decode the clock type string \"" + clockTypeString + L"\" for the target clock with name \"" + targetClockName + L"\" for System.SetClockFrequency!"));
		return false;
	}

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SystemStateChangeType::SetClockFrequency;
	stateChange.targetElementName = targetClockName;
	stateChange.setClockRateClockType = clockType;
	stateChange.setClockRateValue = clockRate;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_System_SetLineState(IHierarchicalStorageNode& node, unsigned int moduleID, SystemStateChange& stateChange)
{
	//Extract the SystemLineName and Value attributes
	IHierarchicalStorageAttribute* systemLineNameAttribute = node.GetAttribute(L"SystemLineName");
	IHierarchicalStorageAttribute* valueAttribute = node.GetAttribute(L"Value");
	if((systemLineNameAttribute == 0) || (valueAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing SystemLineName or Value attribute for System.SetClockFrequency!"));
		return false;
	}
	std::wstring systemLineName = systemLineNameAttribute->GetValue();
	unsigned int lineValue = valueAttribute->ExtractHexValue<unsigned int>();

	//Populate the system state change object with this state change info
	stateChange.moduleID = moduleID;
	stateChange.type = SystemStateChangeType::SetSystemLineState;
	stateChange.targetElementName = systemLineName;
	stateChange.setLineStateValue = lineValue;

	return true;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_ProcessViewQueue(const std::list<ViewOpenRequest>& viewOpenRequests)
{
	//Process each view open request
	bool result = true;
	for(std::list<ViewOpenRequest>::const_iterator i = viewOpenRequests.begin(); i != viewOpenRequests.end(); ++i)
	{
		bool requestResult = false;
		const ViewOpenRequest& request = *i;
		if(request.viewTarget == IViewPresenter::ViewTarget::System)
		{
			//Open the view
			for(std::set<IExtension*>::const_iterator menuHandlerIterator = systemMenuHandlers.begin(); menuHandlerIterator != systemMenuHandlers.end(); ++menuHandlerIterator)
			{
				requestResult |= (*menuHandlerIterator)->OpenSystemView(request.viewGroupName, request.viewName);
			}
		}
		else if(request.viewTarget == IViewPresenter::ViewTarget::Module)
		{
			//Retrieve info on the target module
			LoadedModuleInfoMap::const_iterator loadedModuleInfoIterator = loadedModuleInfoMap.find(request.moduleID);
			if(loadedModuleInfoIterator == loadedModuleInfoMap.end())
			{
				std::wstringstream message;
				message << L"Could not locate module with ID " << request.moduleID << L" for System.OpenView!";
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", message.str()));
				result = false;
				continue;
			}
			const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleInfoIterator->second;

			//Open the view
			for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
			{
				requestResult |= (*menuHandlerIterator)->OpenModuleView(request.viewGroupName, request.viewName, request.moduleID);
			}
		}
		else if(request.viewTarget == IViewPresenter::ViewTarget::Device)
		{
			//Retrieve the device object from the system
			IDevice* device = GetDevice(request.moduleID, request.deviceInstanceName);
			if(device == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate device with name " + request.deviceInstanceName + L" for System.OpenView!"));
				result = false;
				continue;
			}

			//Open the view
			for(LoadedDeviceInfoList::iterator loadedDeviceIterator = loadedDeviceInfoList.begin(); loadedDeviceIterator != loadedDeviceInfoList.end(); ++loadedDeviceIterator)
			{
				if(loadedDeviceIterator->device == device)
				{
					for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedDeviceIterator->menuHandlers.begin(); menuHandlerIterator != loadedDeviceIterator->menuHandlers.end(); ++menuHandlerIterator)
					{
						requestResult |= (*menuHandlerIterator)->OpenDeviceView(request.viewGroupName, request.viewName, device);
					}
				}
			}
		}
		else if((request.viewTarget == IViewPresenter::ViewTarget::Extension) && !request.globalExtension)
		{
			//Retrieve the extension object from the system
			IExtension* extension = GetExtension(request.moduleID, request.extensionInstanceName);
			if(extension == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate extension with name " + request.extensionInstanceName + L" for System.OpenView!"));
				result = false;
				continue;
			}

			//Open the view
			for(LoadedExtensionInfoList::iterator loadedExtensionIterator = loadedExtensionInfoList.begin(); loadedExtensionIterator != loadedExtensionInfoList.end(); ++loadedExtensionIterator)
			{
				if(loadedExtensionIterator->extension == extension)
				{
					for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedExtensionIterator->menuHandlers.begin(); menuHandlerIterator != loadedExtensionIterator->menuHandlers.end(); ++menuHandlerIterator)
					{
						requestResult |= (*menuHandlerIterator)->OpenExtensionView(request.viewGroupName, request.viewName, extension);
					}
				}
			}
		}
		else if((request.viewTarget == IViewPresenter::ViewTarget::Extension) && request.globalExtension)
		{
			//Retrieve the extension object from the system
			IExtension* extension = GetGlobalExtension(request.extensionInstanceName);
			if(extension == 0)
			{
				WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not locate global extension with name " + request.extensionInstanceName + L" for System.OpenView!"));
				result = false;
				continue;
			}

			//Open the view
			for(LoadedGlobalExtensionInfoList::iterator loadedGlobalExtensionIterator = globalExtensionInfoList.begin(); loadedGlobalExtensionIterator != globalExtensionInfoList.end(); ++loadedGlobalExtensionIterator)
			{
				LoadedGlobalExtensionInfo& globalExtensionInfo = loadedGlobalExtensionIterator->second;
				if(globalExtensionInfo.extension == extension)
				{
					for(std::set<IExtension*>::const_iterator menuHandlerIterator = globalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != globalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
					{
						requestResult |= (*menuHandlerIterator)->OpenExtensionView(request.viewGroupName, request.viewName, extension);
					}
				}
			}
		}
		else
		{
			std::wstringstream message;
			message << L"An invalid view target of " << (unsigned int)request.viewTarget << L" was specified for System.OpenView!";
			WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", message.str()));
			result = false;
			continue;
		}
		result &= requestResult;
	}
	return result;
}

//----------------------------------------------------------------------------------------
void System::UnloadAllModulesSynchronous()
{
	clearSystemComplete = false;
	std::thread workerThread(std::bind(std::mem_fn(&System::UnloadAllModules), this));
	workerThread.detach();
}

//----------------------------------------------------------------------------------------
bool System::UnloadAllModulesSynchronousComplete() const
{
	return clearSystemComplete;
}

//----------------------------------------------------------------------------------------
void System::UnloadAllModules()
{
	std::unique_lock<std::mutex> lock(moduleLoadMutex);

	//Stop the system if it is currently running
	StopSystem();

	//Unload each loaded module
	while(!loadedModuleInfoMap.empty())
	{
		const LoadedModuleInfoInternal& moduleInfo = loadedModuleInfoMap.begin()->second;
		UnloadModuleInternal(moduleInfo.moduleID);
	}

	//Flag that the operation is complete
	clearSystemComplete = true;

	//Notify any registered observers that the set of loaded modules has now changed
	lock.unlock();
	loadedModuleChangeObservers.NotifyObservers();
}

//----------------------------------------------------------------------------------------
bool System::ReadModuleConnectorInfo(const MarshalSupport::Marshal::In<std::wstring>& filePath, const MarshalSupport::Marshal::Out<std::wstring>& systemClassName, const MarshalSupport::Marshal::Out<ConnectorImportList>& connectorsImported, const MarshalSupport::Marshal::Out<ConnectorExportList>& connectorsExported) const
{
	//Open the target file
	FileStreamReference streamReference(guiExtensionInterface);
	if(!streamReference.OpenExistingFileForRead(filePath))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Could not open module file " + filePath + L"!"));
		return false;
	}
	Stream::IStream& source = *streamReference;

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
	source.ProcessByteOrderMark();

	//Load the XML structure from the file
	HierarchicalStorageTree tree;
	if(!tree.LoadTree(source))
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Error loading XML structure from module file " + filePath + L"! The xml error string is as follows: " + tree.GetErrorString()));
		return false;
	}
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();

	//If this is a system module, no connector info exists. In this case, return true with
	//no data in the connector import and export lists, and a blank system class name.
	if(rootNode.GetName() == L"System")
	{
		return true;
	}

	//Extract the system class name
	IHierarchicalStorageAttribute* systemClassNameAttribute = rootNode.GetAttribute(L"SystemClassName");
	if(systemClassNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EventLevel::Error, L"System", L"Missing SystemClassName on root node!"));
		return false;
	}
	systemClassName = systemClassNameAttribute->GetValue();

	//Extract connector definitions
	ConnectorImportList connectorsImportedTemp;
	ConnectorExportList connectorsExportedTemp;
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); !loadSystemAbort && (i != childList.end()); ++i)
	{
		std::wstring elementName = (*i)->GetName();
		if(elementName == L"System.ExportConnector")
		{
			IHierarchicalStorageAttribute* connectorClassNameAttribute = (*i)->GetAttribute(L"ConnectorClassName");
			IHierarchicalStorageAttribute* connectorInstanceNameAttribute = (*i)->GetAttribute(L"ConnectorInstanceName");
			if((connectorClassNameAttribute != 0) && (connectorInstanceNameAttribute != 0))
			{
				std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
				std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
				ConnectorDefinitionExport connectorDefinition;
				connectorDefinition.className = connectorClassName;
				connectorDefinition.instanceName = connectorInstanceName;
				connectorsExportedTemp.push_back(connectorDefinition);
			}
		}
		else if(elementName == L"System.ImportConnector")
		{
			IHierarchicalStorageAttribute* connectorClassNameAttribute = (*i)->GetAttribute(L"ConnectorClassName");
			IHierarchicalStorageAttribute* connectorInstanceNameAttribute = (*i)->GetAttribute(L"ConnectorInstanceName");
			if((connectorClassNameAttribute != 0) && (connectorInstanceNameAttribute != 0))
			{
				std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
				std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();
				ConnectorDefinitionImport connectorDefinition;
				connectorDefinition.className = connectorClassName;
				connectorDefinition.instanceName = connectorInstanceName;
				connectorsImportedTemp.push_back(connectorDefinition);
			}
		}
	}
	connectorsImported = connectorsImportedTemp;
	connectorsExported = connectorsExportedTemp;

	return true;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> System::LoadModuleSynchronousCurrentModuleName() const
{
	std::unique_lock<std::mutex> lock(moduleNameMutex);
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
	std::unique_lock<std::mutex> lock(moduleNameMutex);
	loadSystemCurrentModuleNameStack.push_back(moduleName);
}

//----------------------------------------------------------------------------------------
void System::PopLoadModuleCurrentModuleName()
{
	std::unique_lock<std::mutex> lock(moduleNameMutex);
	loadSystemCurrentModuleNameStack.pop_back();
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> System::UnloadModuleSynchronousCurrentModuleName() const
{
	std::unique_lock<std::mutex> lock(moduleNameMutex);
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
	std::unique_lock<std::mutex> lock(moduleNameMutex);
	unloadSystemCurrentModuleNameStack.push_back(moduleName);
}

//----------------------------------------------------------------------------------------
void System::PopUnloadModuleCurrentModuleName()
{
	std::unique_lock<std::mutex> lock(moduleNameMutex);
	unloadSystemCurrentModuleNameStack.pop_back();
}

//----------------------------------------------------------------------------------------
//Loaded module info functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<unsigned int>> System::GetLoadedModuleIDs() const
{
	std::list<unsigned int> idList;
	for(LoadedModuleInfoMap::const_iterator i = loadedModuleInfoMap.begin(); i != loadedModuleInfoMap.end(); ++i)
	{
		idList.push_back(i->first);
	}
	return idList;
}

//----------------------------------------------------------------------------------------
bool System::GetLoadedModuleInfo(unsigned int moduleID, ILoadedModuleInfo& moduleInfo) const
{
	LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.find(moduleID);
	if(loadedModuleIterator == loadedModuleInfoMap.end())
	{
		return false;
	}
	const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

	moduleInfo.SetModuleID(loadedModuleInfo.moduleID);
	moduleInfo.SetModuleFilePath(loadedModuleInfo.filePath);
	moduleInfo.SetIsProgramModule(loadedModuleInfo.programModule);
	moduleInfo.SetSystemClassName(loadedModuleInfo.systemClassName);
	moduleInfo.SetModuleClassName(loadedModuleInfo.className);
	moduleInfo.SetModuleInstanceName(loadedModuleInfo.instanceName);
	moduleInfo.SetModuleDisplayName(loadedModuleInfo.displayName);
	moduleInfo.SetProductionYear(loadedModuleInfo.productionYear);
	moduleInfo.SetManufacturerCode(loadedModuleInfo.manufacturerCode);
	moduleInfo.SetManufacturerDisplayName(loadedModuleInfo.manufacturerDisplayName);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleDisplayName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleDisplayName) const
{
	LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.find(moduleID);
	if(loadedModuleIterator == loadedModuleInfoMap.end())
	{
		return false;
	}
	const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

	moduleDisplayName = loadedModuleInfo.displayName;
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetModuleInstanceName(unsigned int moduleID, const MarshalSupport::Marshal::Out<std::wstring>& moduleInstanceName) const
{
	LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.find(moduleID);
	if(loadedModuleIterator == loadedModuleInfoMap.end())
	{
		return false;
	}
	const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

	moduleInstanceName = loadedModuleInfo.instanceName;
	return true;
}

//----------------------------------------------------------------------------------------
void System::LoadedModulesChangeNotifyRegister(IObserverSubscription& observer)
{
	loadedModuleChangeObservers.AddObserver(observer);
}

//----------------------------------------------------------------------------------------
void System::LoadedModulesChangeNotifyDeregister(IObserverSubscription& observer)
{
	loadedModuleChangeObservers.RemoveObserver(observer);
}

//----------------------------------------------------------------------------------------
//Connector info functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<unsigned int>> System::GetConnectorIDs() const
{
	std::list<unsigned int> idList;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		idList.push_back(i->first);
	}
	return idList;
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
//Loaded device info functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IDevice*>> System::GetLoadedDevices() const
{
	//##TODO## Consider thread safety for all functions exposed over the system extension
	//interface
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	std::list<IDevice*> deviceList;
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		deviceList.push_back(i->device);
	}
	return deviceList;
}

//----------------------------------------------------------------------------------------
bool System::GetDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceDisplayName) const
{
	//Ensure the specified target device is one of the currently loaded devices
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	bool foundTargetDevice = false;
	LoadedDeviceInfoList::const_iterator loadedDeviceInfoListIterator = loadedDeviceInfoList.begin();
	while(!foundTargetDevice && (loadedDeviceInfoListIterator != loadedDeviceInfoList.end()))
	{
		if(loadedDeviceInfoListIterator->device == device)
		{
			foundTargetDevice = true;
			continue;
		}
		++loadedDeviceInfoListIterator;
	}
	if(!foundTargetDevice)
	{
		return false;
	}
	const LoadedDeviceInfo& loadedDeviceInfo = *loadedDeviceInfoListIterator;

	//Return the device display name to the caller
	deviceDisplayName = loadedDeviceInfo.displayName;
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetDeviceInstanceName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& deviceInstanceName) const
{
	//Ensure the specified target device is one of the currently loaded devices
	bool foundTargetDevice = false;
	LoadedDeviceInfoList::const_iterator loadedDeviceInfoListIterator = loadedDeviceInfoList.begin();
	while(!foundTargetDevice && (loadedDeviceInfoListIterator != loadedDeviceInfoList.end()))
	{
		if(loadedDeviceInfoListIterator->device == device)
		{
			foundTargetDevice = true;
			continue;
		}
		++loadedDeviceInfoListIterator;
	}
	if(!foundTargetDevice)
	{
		return false;
	}
	const LoadedDeviceInfo& loadedDeviceInfo = *loadedDeviceInfoListIterator;

	//Return the device instance name to the caller
	deviceInstanceName = loadedDeviceInfo.name;
	return true;
}

//----------------------------------------------------------------------------------------
bool System::GetFullyQualifiedDeviceDisplayName(IDevice* device, const MarshalSupport::Marshal::Out<std::wstring>& fullyQualifiedDeviceDisplayName) const
{
	//Ensure the specified target device is one of the currently loaded devices
	std::unique_lock<std::mutex> loadedElementLock(loadedElementMutex);
	bool foundTargetDevice = false;
	LoadedDeviceInfoList::const_iterator loadedDeviceInfoListIterator = loadedDeviceInfoList.begin();
	while(!foundTargetDevice && (loadedDeviceInfoListIterator != loadedDeviceInfoList.end()))
	{
		if(loadedDeviceInfoListIterator->device == device)
		{
			foundTargetDevice = true;
			continue;
		}
		++loadedDeviceInfoListIterator;
	}
	if(!foundTargetDevice)
	{
		return false;
	}
	const LoadedDeviceInfo& loadedDeviceInfo = *loadedDeviceInfoListIterator;

	//Build a list of each connector that's imported by the module containing the target
	//device
	//##TODO## Ensure that we use a mutex to protect access to connectorDetailsMap in all
	//cases
	std::list<const ConnectorInfoInternal*> importedConnectorList;
	for(ConnectorDetailsMap::const_iterator connectorDetailsMapIterator = connectorDetailsMap.begin(); connectorDetailsMapIterator != connectorDetailsMap.end(); ++connectorDetailsMapIterator)
	{
		const ConnectorInfoInternal& connectorDetails = connectorDetailsMapIterator->second;
		if(connectorDetails.connectorUsed && (connectorDetails.importingModuleID == loadedDeviceInfo.moduleID))
		{
			importedConnectorList.push_back(&connectorDetails);
		}
	}

	//Build a list of connectors imported by the parent module of the target device which
	//should have the connector name qualified
	//##TODO## Extend this operation to all parent modules of the module which contains
	//the target device, so that each connector in each parent module is listed where more
	//than one eligible connector for importing exists.
	//##TODO## For each root module we reach, if there is more than one root module of the
	//same type, include the root module display name in the device display name.
	std::list<const ConnectorInfoInternal*> connectorsToQualify;
	for(std::list<const ConnectorInfoInternal*>::const_iterator importedConnectorListIterator = importedConnectorList.begin(); importedConnectorListIterator != importedConnectorList.end(); ++importedConnectorListIterator)
	{
		const ConnectorInfoInternal& importedConnectorDetails = *(*importedConnectorListIterator);
		std::list<const ConnectorInfoInternal*> compatibleConnectorList;
		for(ConnectorDetailsMap::const_iterator connectorDetailsMapIterator = connectorDetailsMap.begin(); connectorDetailsMapIterator != connectorDetailsMap.end(); ++connectorDetailsMapIterator)
		{
			const ConnectorInfoInternal& connectorDetails = connectorDetailsMapIterator->second;
			if((connectorDetails.systemClassName == importedConnectorDetails.systemClassName) && (connectorDetails.connectorClassName == importedConnectorDetails.connectorClassName))
			{
				compatibleConnectorList.push_back(&connectorDetails);
			}
		}

		//If we found more than one compatible connector which could be imported, add this
		//connector to the list of connectors to qualify in the device display name.
		if(compatibleConnectorList.size() > 1)
		{
			connectorsToQualify.push_back(&importedConnectorDetails);
		}
	}

	//Generate the fully qualified device display name, and return it to the caller.
	std::wstring fullyQualifiedDeviceDisplayNameTemp = loadedDeviceInfo.displayName;
	if(!connectorsToQualify.empty())
	{
		fullyQualifiedDeviceDisplayNameTemp += L" {";
		bool firstConnector = true;
		for(std::list<const ConnectorInfoInternal*>::const_iterator connectorListIterator = connectorsToQualify.begin(); connectorListIterator != connectorsToQualify.end(); ++connectorListIterator)
		{
			const ConnectorInfoInternal& connectorDetails = *(*connectorListIterator);
			fullyQualifiedDeviceDisplayNameTemp += (!firstConnector? L", ": L"") + connectorDetails.exportingModuleConnectorInstanceName;
			firstConnector = false;
		}
		fullyQualifiedDeviceDisplayNameTemp += L"}";
	}
	fullyQualifiedDeviceDisplayName = fullyQualifiedDeviceDisplayNameTemp;

	return true;
}

//----------------------------------------------------------------------------------------
//Loaded extension info functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IExtension*>> System::GetLoadedExtensions() const
{
	//##TODO## Consider thread safety for all functions exposed over the system extension
	//interface
	std::list<IExtension*> extensionList;
	for(LoadedGlobalExtensionInfoList::const_iterator i = globalExtensionInfoList.begin(); i != globalExtensionInfoList.end(); ++i)
	{
		extensionList.push_back(i->second.extension);
	}
	for(LoadedExtensionInfoList::const_iterator i = loadedExtensionInfoList.begin(); i != loadedExtensionInfoList.end(); ++i)
	{
		extensionList.push_back(i->extension);
	}
	return extensionList;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
System::KeyCode System::GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const
{
	//Control keys
	std::wstring keyCodeNameTemp = keyCodeName.Get();
	if(keyCodeNameTemp == L"Esc")             return ISystemDeviceInterface::KeyCode::Escape;
	else if(keyCodeNameTemp == L"Tab")        return ISystemDeviceInterface::KeyCode::Tab;
	else if(keyCodeNameTemp == L"Enter")      return ISystemDeviceInterface::KeyCode::Enter;
	else if(keyCodeNameTemp == L"Space")      return ISystemDeviceInterface::KeyCode::Space;
	else if(keyCodeNameTemp == L"Backspace")  return ISystemDeviceInterface::KeyCode::Backspace;
	else if(keyCodeNameTemp == L"Insert")     return ISystemDeviceInterface::KeyCode::Insert;
	else if(keyCodeNameTemp == L"Delete")     return ISystemDeviceInterface::KeyCode::Delete;
	else if(keyCodeNameTemp == L"PgUp")       return ISystemDeviceInterface::KeyCode::PageUp;
	else if(keyCodeNameTemp == L"PgDn")       return ISystemDeviceInterface::KeyCode::PageDown;
	else if(keyCodeNameTemp == L"Home")       return ISystemDeviceInterface::KeyCode::Home;
	else if(keyCodeNameTemp == L"End")        return ISystemDeviceInterface::KeyCode::End;
	else if(keyCodeNameTemp == L"Up")         return ISystemDeviceInterface::KeyCode::Up;
	else if(keyCodeNameTemp == L"Down")       return ISystemDeviceInterface::KeyCode::Down;
	else if(keyCodeNameTemp == L"Left")       return ISystemDeviceInterface::KeyCode::Left;
	else if(keyCodeNameTemp == L"Right")      return ISystemDeviceInterface::KeyCode::Right;
	else if(keyCodeNameTemp == L"Print")      return ISystemDeviceInterface::KeyCode::PrintScreen;
	else if(keyCodeNameTemp == L"Pause")      return ISystemDeviceInterface::KeyCode::Pause;
	else if(keyCodeNameTemp == L"NumLock")    return ISystemDeviceInterface::KeyCode::NumLock;
	else if(keyCodeNameTemp == L"CapsLock")   return ISystemDeviceInterface::KeyCode::CapsLock;
	else if(keyCodeNameTemp == L"ScrollLock") return ISystemDeviceInterface::KeyCode::ScrollLock;
	else if(keyCodeNameTemp == L"LeftWin")    return ISystemDeviceInterface::KeyCode::LeftWindows;
	else if(keyCodeNameTemp == L"RightWin")   return ISystemDeviceInterface::KeyCode::RightWindows;
	else if(keyCodeNameTemp == L"Menu")       return ISystemDeviceInterface::KeyCode::Menu;
	//Modifier keys
	else if(keyCodeNameTemp == L"Ctrl")       return ISystemDeviceInterface::KeyCode::Ctrl;
	else if(keyCodeNameTemp == L"Alt")        return ISystemDeviceInterface::KeyCode::Alt;
	else if(keyCodeNameTemp == L"Shift")      return ISystemDeviceInterface::KeyCode::Shift;
	//Function keys
	else if(keyCodeNameTemp == L"F1")         return ISystemDeviceInterface::KeyCode::F1;
	else if(keyCodeNameTemp == L"F2")         return ISystemDeviceInterface::KeyCode::F2;
	else if(keyCodeNameTemp == L"F3")         return ISystemDeviceInterface::KeyCode::F3;
	else if(keyCodeNameTemp == L"F4")         return ISystemDeviceInterface::KeyCode::F4;
	else if(keyCodeNameTemp == L"F5")         return ISystemDeviceInterface::KeyCode::F5;
	else if(keyCodeNameTemp == L"F6")         return ISystemDeviceInterface::KeyCode::F6;
	else if(keyCodeNameTemp == L"F7")         return ISystemDeviceInterface::KeyCode::F7;
	else if(keyCodeNameTemp == L"F8")         return ISystemDeviceInterface::KeyCode::F8;
	else if(keyCodeNameTemp == L"F9")         return ISystemDeviceInterface::KeyCode::F9;
	else if(keyCodeNameTemp == L"F10")        return ISystemDeviceInterface::KeyCode::F10;
	else if(keyCodeNameTemp == L"F11")        return ISystemDeviceInterface::KeyCode::F11;
	else if(keyCodeNameTemp == L"F12")        return ISystemDeviceInterface::KeyCode::F12;
	//Numbers
	else if(keyCodeNameTemp == L"0")          return ISystemDeviceInterface::KeyCode::Number0;
	else if(keyCodeNameTemp == L"1")          return ISystemDeviceInterface::KeyCode::Number1;
	else if(keyCodeNameTemp == L"2")          return ISystemDeviceInterface::KeyCode::Number2;
	else if(keyCodeNameTemp == L"3")          return ISystemDeviceInterface::KeyCode::Number3;
	else if(keyCodeNameTemp == L"4")          return ISystemDeviceInterface::KeyCode::Number4;
	else if(keyCodeNameTemp == L"5")          return ISystemDeviceInterface::KeyCode::Number5;
	else if(keyCodeNameTemp == L"6")          return ISystemDeviceInterface::KeyCode::Number6;
	else if(keyCodeNameTemp == L"7")          return ISystemDeviceInterface::KeyCode::Number7;
	else if(keyCodeNameTemp == L"8")          return ISystemDeviceInterface::KeyCode::Number8;
	else if(keyCodeNameTemp == L"9")          return ISystemDeviceInterface::KeyCode::Number9;
	//Letters
	else if(keyCodeNameTemp == L"A")          return ISystemDeviceInterface::KeyCode::A;
	else if(keyCodeNameTemp == L"B")          return ISystemDeviceInterface::KeyCode::B;
	else if(keyCodeNameTemp == L"C")          return ISystemDeviceInterface::KeyCode::C;
	else if(keyCodeNameTemp == L"D")          return ISystemDeviceInterface::KeyCode::D;
	else if(keyCodeNameTemp == L"E")          return ISystemDeviceInterface::KeyCode::E;
	else if(keyCodeNameTemp == L"F")          return ISystemDeviceInterface::KeyCode::F;
	else if(keyCodeNameTemp == L"G")          return ISystemDeviceInterface::KeyCode::G;
	else if(keyCodeNameTemp == L"H")          return ISystemDeviceInterface::KeyCode::H;
	else if(keyCodeNameTemp == L"I")          return ISystemDeviceInterface::KeyCode::I;
	else if(keyCodeNameTemp == L"J")          return ISystemDeviceInterface::KeyCode::J;
	else if(keyCodeNameTemp == L"K")          return ISystemDeviceInterface::KeyCode::K;
	else if(keyCodeNameTemp == L"L")          return ISystemDeviceInterface::KeyCode::L;
	else if(keyCodeNameTemp == L"M")          return ISystemDeviceInterface::KeyCode::M;
	else if(keyCodeNameTemp == L"N")          return ISystemDeviceInterface::KeyCode::N;
	else if(keyCodeNameTemp == L"O")          return ISystemDeviceInterface::KeyCode::O;
	else if(keyCodeNameTemp == L"P")          return ISystemDeviceInterface::KeyCode::P;
	else if(keyCodeNameTemp == L"Q")          return ISystemDeviceInterface::KeyCode::Q;
	else if(keyCodeNameTemp == L"R")          return ISystemDeviceInterface::KeyCode::R;
	else if(keyCodeNameTemp == L"S")          return ISystemDeviceInterface::KeyCode::S;
	else if(keyCodeNameTemp == L"T")          return ISystemDeviceInterface::KeyCode::T;
	else if(keyCodeNameTemp == L"U")          return ISystemDeviceInterface::KeyCode::U;
	else if(keyCodeNameTemp == L"V")          return ISystemDeviceInterface::KeyCode::V;
	else if(keyCodeNameTemp == L"W")          return ISystemDeviceInterface::KeyCode::W;
	else if(keyCodeNameTemp == L"X")          return ISystemDeviceInterface::KeyCode::X;
	else if(keyCodeNameTemp == L"Y")          return ISystemDeviceInterface::KeyCode::Y;
	else if(keyCodeNameTemp == L"Z")          return ISystemDeviceInterface::KeyCode::Z;
	//Symbol keys
	else if(keyCodeNameTemp == L"OEM1")       return ISystemDeviceInterface::KeyCode::OEM1;
	else if(keyCodeNameTemp == L"OEMPlus")    return ISystemDeviceInterface::KeyCode::OEMPLUS;
	else if(keyCodeNameTemp == L"OEMComma")   return ISystemDeviceInterface::KeyCode::OEMCOMMA;
	else if(keyCodeNameTemp == L"OEMMinus")   return ISystemDeviceInterface::KeyCode::OEMMINUS;
	else if(keyCodeNameTemp == L"OEMPeriod")  return ISystemDeviceInterface::KeyCode::OEMPERIOD;
	else if(keyCodeNameTemp == L"OEM2")       return ISystemDeviceInterface::KeyCode::OEM2;
	else if(keyCodeNameTemp == L"OEM3")       return ISystemDeviceInterface::KeyCode::OEM3;
	else if(keyCodeNameTemp == L"OEM4")       return ISystemDeviceInterface::KeyCode::OEM4;
	else if(keyCodeNameTemp == L"OEM5")       return ISystemDeviceInterface::KeyCode::OEM5;
	else if(keyCodeNameTemp == L"OEM6")       return ISystemDeviceInterface::KeyCode::OEM6;
	else if(keyCodeNameTemp == L"OEM7")       return ISystemDeviceInterface::KeyCode::OEM7;
	else if(keyCodeNameTemp == L"OEM8")       return ISystemDeviceInterface::KeyCode::OEM8;
	else if(keyCodeNameTemp == L"OEMAX")      return ISystemDeviceInterface::KeyCode::OEMAX;
	else if(keyCodeNameTemp == L"OEM102")     return ISystemDeviceInterface::KeyCode::OEM102;
	//Numpad keys
	else if(keyCodeNameTemp == L"Numpad0")    return ISystemDeviceInterface::KeyCode::NUMPAD0;
	else if(keyCodeNameTemp == L"Numpad1")    return ISystemDeviceInterface::KeyCode::NUMPAD1;
	else if(keyCodeNameTemp == L"Numpad2")    return ISystemDeviceInterface::KeyCode::NUMPAD2;
	else if(keyCodeNameTemp == L"Numpad3")    return ISystemDeviceInterface::KeyCode::NUMPAD3;
	else if(keyCodeNameTemp == L"Numpad4")    return ISystemDeviceInterface::KeyCode::NUMPAD4;
	else if(keyCodeNameTemp == L"Numpad5")    return ISystemDeviceInterface::KeyCode::NUMPAD5;
	else if(keyCodeNameTemp == L"Numpad6")    return ISystemDeviceInterface::KeyCode::NUMPAD6;
	else if(keyCodeNameTemp == L"Numpad7")    return ISystemDeviceInterface::KeyCode::NUMPAD7;
	else if(keyCodeNameTemp == L"Numpad8")    return ISystemDeviceInterface::KeyCode::NUMPAD8;
	else if(keyCodeNameTemp == L"Numpad9")    return ISystemDeviceInterface::KeyCode::NUMPAD9;
	else if(keyCodeNameTemp == L"Numpad*")    return ISystemDeviceInterface::KeyCode::NUMPADMULTIPLY;
	else if(keyCodeNameTemp == L"Numpad/")    return ISystemDeviceInterface::KeyCode::NUMPADDIVIDE;
	else if(keyCodeNameTemp == L"Numpad-")    return ISystemDeviceInterface::KeyCode::NUMPADSUBTRACT;
	else if(keyCodeNameTemp == L"Numpad+")    return ISystemDeviceInterface::KeyCode::NUMPADADD;
	else if(keyCodeNameTemp == L"Numpad.")    return ISystemDeviceInterface::KeyCode::NUMPADDECIMAL;
	//Joysticks
	else if(keyCodeNameTemp.compare(0, 3, L"Joy") == 0)
	{
		if(keyCodeNameTemp.find(L"Btn") != std::wstring::npos)
		{
			//Extract the joystick and button numbers from the string
			std::wstring joystickText = L"Joy";
			std::wstring buttonText = L"Btn";
			unsigned int joystickNo;
			unsigned int buttonNo;
			std::wstringstream keyCodeStream;
			keyCodeStream.str(keyCodeNameTemp);
			keyCodeStream.read(&joystickText[0], 3);
			keyCodeStream >> joystickNo;
			keyCodeStream.read(&buttonText[0], 3);
			keyCodeStream >> buttonNo;

			//Validate the text string components
			if((joystickText != L"Joy") || (buttonText != L"Btn"))
			{
				return ISystemDeviceInterface::KeyCode::None;
			}

			//Ensure the joystick and button numbers are within the valid range
			const unsigned int maxJoystickCount = 16;
			const unsigned int maxJoystickButtonCount = 32;
			if((joystickNo >= maxJoystickCount) || (buttonNo >= maxJoystickButtonCount))
			{
				return ISystemDeviceInterface::KeyCode::None;
			}

			//Return the corresponding key code for the target joystick button
			return (KeyCode)((unsigned int)ISystemDeviceInterface::KeyCode::JOYSTICK00BUTTON00 + (joystickNo * maxJoystickButtonCount) + buttonNo);
		}
		else if(keyCodeNameTemp.find(L"Axis") != std::wstring::npos)
		{
			//Extract the joystick and axis numbers, and the axis direction, from the
			//string.
			std::wstring joystickText = L"Joy";
			std::wstring axisText = L"Axis";
			std::wstring directionText = L" ";
			unsigned int joystickNo;
			unsigned int axisNo;
			std::wstringstream keyCodeStream;
			keyCodeStream.str(keyCodeNameTemp);
			keyCodeStream.read(&joystickText[0], 3);
			keyCodeStream >> joystickNo;
			keyCodeStream.read(&axisText[0], 4);
			keyCodeStream >> axisNo;
			keyCodeStream.read(&directionText[0], 1);

			//Validate the text string components
			if((joystickText != L"Joy") || (axisText != L"Axis") || ((directionText != L"+") && (directionText != L"-")))
			{
				return ISystemDeviceInterface::KeyCode::None;
			}

			//Ensure the joystick and axis numbers are within the valid range
			const unsigned int maxJoystickCount = 16;
			const unsigned int maxJoystickAxisCount = 6;
			const unsigned int joystickAxisDirectionCount = 2;
			if((joystickNo >= maxJoystickCount) || (axisNo >= maxJoystickAxisCount))
			{
				return ISystemDeviceInterface::KeyCode::None;
			}

			//Decode the axis direction
			bool axisPlus = (directionText[0] == L'+');
			unsigned int axisDirectionOffset = (axisPlus)? 0: 1;

			//Return the corresponding key code for the target joystick axis acting as a
			//button
			return (KeyCode)((unsigned int)ISystemDeviceInterface::KeyCode::JOYSTICK00AXIS0PLUS + (joystickNo * maxJoystickAxisCount * joystickAxisDirectionCount) + (axisNo * joystickAxisDirectionCount) + axisDirectionOffset);
		}
	}

	return ISystemDeviceInterface::KeyCode::None;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> System::GetKeyCodeName(KeyCode keyCode) const
{
	switch(keyCode)
	{
	//Control keys
	case ISystemDeviceInterface::KeyCode::Escape:            return L"Esc";
	case ISystemDeviceInterface::KeyCode::Tab:               return L"Tab";
	case ISystemDeviceInterface::KeyCode::Enter:             return L"Enter";
	case ISystemDeviceInterface::KeyCode::Space:             return L"Space";
	case ISystemDeviceInterface::KeyCode::Backspace:         return L"Backspace";
	case ISystemDeviceInterface::KeyCode::Insert:            return L"Insert";
	case ISystemDeviceInterface::KeyCode::Delete:            return L"Delete";
	case ISystemDeviceInterface::KeyCode::PageUp:            return L"PgUp";
	case ISystemDeviceInterface::KeyCode::PageDown:          return L"PgDn";
	case ISystemDeviceInterface::KeyCode::Home:              return L"Home";
	case ISystemDeviceInterface::KeyCode::End:               return L"End";
	case ISystemDeviceInterface::KeyCode::Up:                return L"Up";
	case ISystemDeviceInterface::KeyCode::Down:              return L"Down";
	case ISystemDeviceInterface::KeyCode::Left:              return L"Left";
	case ISystemDeviceInterface::KeyCode::Right:             return L"Right";
	case ISystemDeviceInterface::KeyCode::PrintScreen:       return L"Print";
	case ISystemDeviceInterface::KeyCode::Pause:             return L"Pause";
	case ISystemDeviceInterface::KeyCode::NumLock:           return L"NumLock";
	case ISystemDeviceInterface::KeyCode::CapsLock:          return L"CapsLock";
	case ISystemDeviceInterface::KeyCode::ScrollLock:        return L"ScrollLock";
	case ISystemDeviceInterface::KeyCode::LeftWindows:       return L"LeftWin";
	case ISystemDeviceInterface::KeyCode::RightWindows:      return L"RightWin";
	case ISystemDeviceInterface::KeyCode::Menu:              return L"Menu";
	//Modifier keys
	case ISystemDeviceInterface::KeyCode::Ctrl:              return L"Ctrl";
	case ISystemDeviceInterface::KeyCode::Alt:               return L"Alt";
	case ISystemDeviceInterface::KeyCode::Shift:             return L"Shift";
	//Function keys
	case ISystemDeviceInterface::KeyCode::F1:                return L"F1";
	case ISystemDeviceInterface::KeyCode::F2:                return L"F2";
	case ISystemDeviceInterface::KeyCode::F3:                return L"F3";
	case ISystemDeviceInterface::KeyCode::F4:                return L"F4";
	case ISystemDeviceInterface::KeyCode::F5:                return L"F5";
	case ISystemDeviceInterface::KeyCode::F6:                return L"F6";
	case ISystemDeviceInterface::KeyCode::F7:                return L"F7";
	case ISystemDeviceInterface::KeyCode::F8:                return L"F8";
	case ISystemDeviceInterface::KeyCode::F9:                return L"F9";
	case ISystemDeviceInterface::KeyCode::F10:               return L"F10";
	case ISystemDeviceInterface::KeyCode::F11:               return L"F11";
	case ISystemDeviceInterface::KeyCode::F12:               return L"F12";
	//Numbers
	case ISystemDeviceInterface::KeyCode::Number0:                 return L"0";
	case ISystemDeviceInterface::KeyCode::Number1:                 return L"1";
	case ISystemDeviceInterface::KeyCode::Number2:                 return L"2";
	case ISystemDeviceInterface::KeyCode::Number3:                 return L"3";
	case ISystemDeviceInterface::KeyCode::Number4:                 return L"4";
	case ISystemDeviceInterface::KeyCode::Number5:                 return L"5";
	case ISystemDeviceInterface::KeyCode::Number6:                 return L"6";
	case ISystemDeviceInterface::KeyCode::Number7:                 return L"7";
	case ISystemDeviceInterface::KeyCode::Number8:                 return L"8";
	case ISystemDeviceInterface::KeyCode::Number9:                 return L"9";
	//Letters
	case ISystemDeviceInterface::KeyCode::A:                 return L"A";
	case ISystemDeviceInterface::KeyCode::B:                 return L"B";
	case ISystemDeviceInterface::KeyCode::C:                 return L"C";
	case ISystemDeviceInterface::KeyCode::D:                 return L"D";
	case ISystemDeviceInterface::KeyCode::E:                 return L"E";
	case ISystemDeviceInterface::KeyCode::F:                 return L"F";
	case ISystemDeviceInterface::KeyCode::G:                 return L"G";
	case ISystemDeviceInterface::KeyCode::H:                 return L"H";
	case ISystemDeviceInterface::KeyCode::I:                 return L"I";
	case ISystemDeviceInterface::KeyCode::J:                 return L"J";
	case ISystemDeviceInterface::KeyCode::K:                 return L"K";
	case ISystemDeviceInterface::KeyCode::L:                 return L"L";
	case ISystemDeviceInterface::KeyCode::M:                 return L"M";
	case ISystemDeviceInterface::KeyCode::N:                 return L"N";
	case ISystemDeviceInterface::KeyCode::O:                 return L"O";
	case ISystemDeviceInterface::KeyCode::P:                 return L"P";
	case ISystemDeviceInterface::KeyCode::Q:                 return L"Q";
	case ISystemDeviceInterface::KeyCode::R:                 return L"R";
	case ISystemDeviceInterface::KeyCode::S:                 return L"S";
	case ISystemDeviceInterface::KeyCode::T:                 return L"T";
	case ISystemDeviceInterface::KeyCode::U:                 return L"U";
	case ISystemDeviceInterface::KeyCode::V:                 return L"V";
	case ISystemDeviceInterface::KeyCode::W:                 return L"W";
	case ISystemDeviceInterface::KeyCode::X:                 return L"X";
	case ISystemDeviceInterface::KeyCode::Y:                 return L"Y";
	case ISystemDeviceInterface::KeyCode::Z:                 return L"Z";
	//Symbol keys
	case ISystemDeviceInterface::KeyCode::OEM1:              return L"OEM1";
	case ISystemDeviceInterface::KeyCode::OEMPLUS:           return L"OEMPlus";
	case ISystemDeviceInterface::KeyCode::OEMCOMMA:          return L"OEMComma";
	case ISystemDeviceInterface::KeyCode::OEMMINUS:          return L"OEMMinus";
	case ISystemDeviceInterface::KeyCode::OEMPERIOD:         return L"OEMPeriod";
	case ISystemDeviceInterface::KeyCode::OEM2:              return L"OEM2";
	case ISystemDeviceInterface::KeyCode::OEM3:              return L"OEM3";
	case ISystemDeviceInterface::KeyCode::OEM4:              return L"OEM4";
	case ISystemDeviceInterface::KeyCode::OEM5:              return L"OEM5";
	case ISystemDeviceInterface::KeyCode::OEM6:              return L"OEM6";
	case ISystemDeviceInterface::KeyCode::OEM7:              return L"OEM7";
	case ISystemDeviceInterface::KeyCode::OEM8:              return L"OEM8";
	case ISystemDeviceInterface::KeyCode::OEMAX:             return L"OEMAX";
	case ISystemDeviceInterface::KeyCode::OEM102:            return L"OEM102";
	//Numpad keys
	case ISystemDeviceInterface::KeyCode::NUMPAD0:           return L"Numpad0";
	case ISystemDeviceInterface::KeyCode::NUMPAD1:           return L"Numpad1";
	case ISystemDeviceInterface::KeyCode::NUMPAD2:           return L"Numpad2";
	case ISystemDeviceInterface::KeyCode::NUMPAD3:           return L"Numpad3";
	case ISystemDeviceInterface::KeyCode::NUMPAD4:           return L"Numpad4";
	case ISystemDeviceInterface::KeyCode::NUMPAD5:           return L"Numpad5";
	case ISystemDeviceInterface::KeyCode::NUMPAD6:           return L"Numpad6";
	case ISystemDeviceInterface::KeyCode::NUMPAD7:           return L"Numpad7";
	case ISystemDeviceInterface::KeyCode::NUMPAD8:           return L"Numpad8";
	case ISystemDeviceInterface::KeyCode::NUMPAD9:           return L"Numpad9";
	case ISystemDeviceInterface::KeyCode::NUMPADMULTIPLY:    return L"Numpad*";
	case ISystemDeviceInterface::KeyCode::NUMPADDIVIDE:      return L"Numpad/";
	case ISystemDeviceInterface::KeyCode::NUMPADSUBTRACT:    return L"Numpad-";
	case ISystemDeviceInterface::KeyCode::NUMPADADD:         return L"Numpad+";
	case ISystemDeviceInterface::KeyCode::NUMPADDECIMAL:     return L"Numpad.";
	}

	//Joystick buttons
	if((keyCode >= ISystemDeviceInterface::KeyCode::JOYSTICK00BUTTON00) && (keyCode <= ISystemDeviceInterface::KeyCode::JOYSTICK15BUTTON31))
	{
		//Calculate the joystick and button numbers for this keycode
		const unsigned int maxJoystickButtonCount = 32;
		unsigned int keyCodeRebased = (unsigned int)keyCode - (unsigned int)ISystemDeviceInterface::KeyCode::JOYSTICK00BUTTON00;
		unsigned int joystickNo = keyCodeRebased / maxJoystickButtonCount;
		unsigned int buttonNo = keyCodeRebased % maxJoystickButtonCount;

		//Build a string representing this joystick button keycode
		std::wstringstream keyCodeStream;
		keyCodeStream << L"Joy" << joystickNo << L"Btn" << buttonNo;
		return keyCodeStream.str();
	}

	//Joystick axes
	if((keyCode >= ISystemDeviceInterface::KeyCode::JOYSTICK00AXIS0PLUS) && (keyCode <= ISystemDeviceInterface::KeyCode::JOYSTICK15AXIS5MINUS))
	{
		//Calculate the joystick, axis numbers, and direction, for this keycode.
		const unsigned int maxJoystickAxisCount = 6;
		const unsigned int joystickAxisDirectionCount = 2;
		unsigned int keyCodeRebased = (unsigned int)keyCode - (unsigned int)ISystemDeviceInterface::KeyCode::JOYSTICK00AXIS0PLUS;
		unsigned int joystickNo = keyCodeRebased / (maxJoystickAxisCount * joystickAxisDirectionCount);
		unsigned int axesNo = (keyCodeRebased % (maxJoystickAxisCount * joystickAxisDirectionCount)) / joystickAxisDirectionCount;
		bool axisPlus = ((keyCodeRebased % (maxJoystickAxisCount * joystickAxisDirectionCount)) % joystickAxisDirectionCount) == 0;

		//Build a string representing this joystick axis keycode acting as a button
		std::wstring directionString = (axisPlus)? L"+": L"-";
		std::wstringstream keyCodeStream;
		keyCodeStream << L"Joy" << joystickNo << L"Axis" << axesNo << directionString;
		return keyCodeStream.str();
	}

	return L"None";
}

//----------------------------------------------------------------------------------------
bool System::TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode) const
{
	switch(platformKeyCode)
	{
		//Control keys
	case VK_ESCAPE:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Escape;
		return true;
	case VK_TAB:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Tab;
		return true;
	case VK_RETURN:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Enter;
		return true;
	case VK_SPACE:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Space;
		return true;
	case VK_BACK:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Backspace;
		return true;
	case VK_INSERT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Insert;
		return true;
	case VK_DELETE:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Delete;
		return true;
	case VK_PRIOR:
		inputKeyCode = ISystemDeviceInterface::KeyCode::PageUp;
		return true;
	case VK_NEXT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::PageDown;
		return true;
	case VK_HOME:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Home;
		return true;
	case VK_END:
		inputKeyCode = ISystemDeviceInterface::KeyCode::End;
		return true;
	case VK_UP:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Up;
		return true;
	case VK_DOWN:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Down;
		return true;
	case VK_LEFT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Left;
		return true;
	case VK_RIGHT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Right;
		return true;
	case VK_SNAPSHOT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::PrintScreen;
		return true;
	case VK_PAUSE:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Pause;
		return true;
	case VK_NUMLOCK:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NumLock;
		return true;
	case VK_CAPITAL:
		inputKeyCode = ISystemDeviceInterface::KeyCode::CapsLock;
		return true;
	case VK_SCROLL:
		inputKeyCode = ISystemDeviceInterface::KeyCode::ScrollLock;
		return true;
	case VK_LWIN:
		inputKeyCode = ISystemDeviceInterface::KeyCode::LeftWindows;
		return true;
	case VK_RWIN:
		inputKeyCode = ISystemDeviceInterface::KeyCode::RightWindows;
		return true;
	case VK_APPS:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Menu;
		return true;

		//Modifier keys
	case VK_CONTROL:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Ctrl;
		return true;
	case VK_MENU:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Alt;
		return true;
	case VK_SHIFT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::Shift;
		return true;

		//Function keys
	case VK_F1:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F1;
		return true;
	case VK_F2:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F2;
		return true;
	case VK_F3:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F3;
		return true;
	case VK_F4:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F4;
		return true;
	case VK_F5:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F5;
		return true;
	case VK_F6:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F6;
		return true;
	case VK_F7:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F7;
		return true;
	case VK_F8:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F8;
		return true;
	case VK_F9:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F9;
		return true;
	case VK_F10:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F10;
		return true;
	case VK_F11:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F11;
		return true;
	case VK_F12:
		inputKeyCode = ISystemDeviceInterface::KeyCode::F12;
		return true;

		//Numbers
	case '0':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number0;
		return true;
	case '1':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number1;
		return true;
	case '2':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number2;
		return true;
	case '3':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number3;
		return true;
	case '4':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number4;
		return true;
	case '5':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number5;
		return true;
	case '6':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number6;
		return true;
	case '7':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number7;
		return true;
	case '8':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number8;
		return true;
	case '9':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Number9;
		return true;

		//Letters
	case 'A':
		inputKeyCode = ISystemDeviceInterface::KeyCode::A;
		return true;
	case 'B':
		inputKeyCode = ISystemDeviceInterface::KeyCode::B;
		return true;
	case 'C':
		inputKeyCode = ISystemDeviceInterface::KeyCode::C;
		return true;
	case 'D':
		inputKeyCode = ISystemDeviceInterface::KeyCode::D;
		return true;
	case 'E':
		inputKeyCode = ISystemDeviceInterface::KeyCode::E;
		return true;
	case 'F':
		inputKeyCode = ISystemDeviceInterface::KeyCode::F;
		return true;
	case 'G':
		inputKeyCode = ISystemDeviceInterface::KeyCode::G;
		return true;
	case 'H':
		inputKeyCode = ISystemDeviceInterface::KeyCode::H;
		return true;
	case 'I':
		inputKeyCode = ISystemDeviceInterface::KeyCode::I;
		return true;
	case 'J':
		inputKeyCode = ISystemDeviceInterface::KeyCode::J;
		return true;
	case 'K':
		inputKeyCode = ISystemDeviceInterface::KeyCode::K;
		return true;
	case 'L':
		inputKeyCode = ISystemDeviceInterface::KeyCode::L;
		return true;
	case 'M':
		inputKeyCode = ISystemDeviceInterface::KeyCode::M;
		return true;
	case 'N':
		inputKeyCode = ISystemDeviceInterface::KeyCode::N;
		return true;
	case 'O':
		inputKeyCode = ISystemDeviceInterface::KeyCode::O;
		return true;
	case 'P':
		inputKeyCode = ISystemDeviceInterface::KeyCode::P;
		return true;
	case 'Q':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Q;
		return true;
	case 'R':
		inputKeyCode = ISystemDeviceInterface::KeyCode::R;
		return true;
	case 'S':
		inputKeyCode = ISystemDeviceInterface::KeyCode::S;
		return true;
	case 'T':
		inputKeyCode = ISystemDeviceInterface::KeyCode::T;
		return true;
	case 'U':
		inputKeyCode = ISystemDeviceInterface::KeyCode::U;
		return true;
	case 'V':
		inputKeyCode = ISystemDeviceInterface::KeyCode::V;
		return true;
	case 'W':
		inputKeyCode = ISystemDeviceInterface::KeyCode::W;
		return true;
	case 'X':
		inputKeyCode = ISystemDeviceInterface::KeyCode::X;
		return true;
	case 'Y':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Y;
		return true;
	case 'Z':
		inputKeyCode = ISystemDeviceInterface::KeyCode::Z;
		return true;

		//Symbol keys
	case VK_OEM_1:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM1;
		return true;
	case VK_OEM_PLUS:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEMPLUS;
		return true;
	case VK_OEM_COMMA:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEMCOMMA;
		return true;
	case VK_OEM_MINUS:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEMMINUS;
		return true;
	case VK_OEM_PERIOD:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEMPERIOD;
		return true;
	case VK_OEM_2:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM2;
		return true;
	case VK_OEM_3:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM3;
		return true;
	case VK_OEM_4:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM4;
		return true;
	case VK_OEM_5:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM5;
		return true;
	case VK_OEM_6:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM6;
		return true;
	case VK_OEM_7:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM7;
		return true;
	case VK_OEM_8:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM8;
		return true;
	case VK_OEM_AX:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEMAX;
		return true;
	case VK_OEM_102:
		inputKeyCode = ISystemDeviceInterface::KeyCode::OEM102;
		return true;

		//Numpad keys
	case VK_NUMPAD0:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD0;
		return true;
	case VK_NUMPAD1:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD1;
		return true;
	case VK_NUMPAD2:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD2;
		return true;
	case VK_NUMPAD3:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD3;
		return true;
	case VK_NUMPAD4:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD4;
		return true;
	case VK_NUMPAD5:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD5;
		return true;
	case VK_NUMPAD6:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD6;
		return true;
	case VK_NUMPAD7:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD7;
		return true;
	case VK_NUMPAD8:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD8;
		return true;
	case VK_NUMPAD9:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPAD9;
		return true;
	case VK_MULTIPLY:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPADMULTIPLY;
		return true;
	case VK_DIVIDE:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPADDIVIDE;
		return true;
	case VK_SUBTRACT:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPADSUBTRACT;
		return true;
	case VK_ADD:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPADADD;
		return true;
	case VK_DECIMAL:
		inputKeyCode = ISystemDeviceInterface::KeyCode::NUMPADDECIMAL;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool System::TranslateJoystickButton(unsigned int joystickNo, unsigned int buttonNo, KeyCode& inputKeyCode) const
{
	//Ensure the joystick and button numbers are within the valid range
	const unsigned int maxJoystickCount = 16;
	const unsigned int maxJoystickButtonCount = 32;
	if((joystickNo >= maxJoystickCount) || (buttonNo >= maxJoystickButtonCount))
	{
		return false;
	}

	//Map the input joystick and button numbers to the corresponding keycode
	inputKeyCode = (KeyCode)((unsigned int)KeyCode::JOYSTICK00BUTTON00 + (joystickNo * maxJoystickButtonCount) + buttonNo);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::TranslateJoystickAxisAsButton(unsigned int joystickNo, unsigned int axisNo, bool positiveAxis, KeyCode& inputKeyCode) const
{
	//Ensure the joystick and axis numbers are within the valid range
	const unsigned int maxJoystickCount = 16;
	const unsigned int maxJoystickAxisCount = 6;
	const unsigned int joystickAxisDirectionCount = 2;
	if((joystickNo >= maxJoystickCount) || (axisNo >= maxJoystickAxisCount))
	{
		return false;
	}

	//Map the input joystick and axis numbers to the corresponding keycode
	unsigned int axisDirectionOffset = (positiveAxis)? 0: 1;
	inputKeyCode = (KeyCode)((unsigned int)KeyCode::JOYSTICK00AXIS0PLUS + (joystickNo * maxJoystickAxisCount * joystickAxisDirectionCount) + (axisNo * joystickAxisDirectionCount) + axisDirectionOffset);
	return true;
}

//----------------------------------------------------------------------------------------
bool System::TranslateJoystickAxis(unsigned int joystickNo, unsigned int axisNo, AxisCode& inputAxisCode) const
{
	//Ensure the joystick and axis numbers are within the valid range
	const unsigned int maxJoystickCount = 16;
	const unsigned int maxJoystickAxisCount = 6;
	if((joystickNo >= maxJoystickCount) || (axisNo >= maxJoystickAxisCount))
	{
		return false;
	}

	//Map the input joystick and axis numbers to the corresponding axis code
	inputAxisCode = (AxisCode)((unsigned int)AxisCode::Joystick00Axis0 + (joystickNo * maxJoystickAxisCount) + axisNo);
	return true;
}

//----------------------------------------------------------------------------------------
void System::HandleInputKeyDown(KeyCode keyCode)
{
	std::unique_lock<std::mutex> lock(inputMutex);
	if(!systemStopped)
	{
		InputKeyMap::const_iterator i = inputKeyMap.find(keyCode);
		if(i != inputKeyMap.end())
		{
			InputEventEntry entry;
			entry.sent = false;
			entry.inputEvent = InputEvent::KeyDown;
			entry.keyCode = keyCode;
			inputEvents.push_back(entry);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::HandleInputKeyUp(KeyCode keyCode)
{
	std::unique_lock<std::mutex> lock(inputMutex);
	if(!systemStopped)
	{
		InputKeyMap::const_iterator i = inputKeyMap.find(keyCode);
		if(i != inputKeyMap.end())
		{
			InputEventEntry entry;
			entry.sent = false;
			entry.inputEvent = InputEvent::KeyUp;
			entry.keyCode = keyCode;
			inputEvents.push_back(entry);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::HandleInputAxisUpdate(AxisCode axisCode, float newValue)
{
	//##TODO## Add support for axis devices
}

//----------------------------------------------------------------------------------------
void System::HandleInputScrollUpdate(ScrollCode scrollCode, int scrollTicks)
{
	//##TODO## Add support for scroll devices
}

//----------------------------------------------------------------------------------------
unsigned int System::GetInputDeviceListLastModifiedToken() const
{
	std::unique_lock<std::mutex> lock(inputMutex);
	return inputDeviceListLastModifiedToken;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IDevice*>> System::GetInputDeviceList() const
{
	std::unique_lock<std::mutex> lock(inputMutex);
	std::set<IDevice*> inputDeviceSet;
	for(InputRegistrationList::const_iterator i = inputRegistrationList.begin(); i != inputRegistrationList.end(); ++i)
	{
		const InputRegistration& inputRegistration = *i;
		inputDeviceSet.insert(inputRegistration.targetDevice);
	}
	std::list<IDevice*> inputDeviceList(inputDeviceSet.begin(), inputDeviceSet.end());
	return inputDeviceList;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<unsigned int>> System::GetDeviceKeyCodeList(IDevice* targetDevice) const
{
	std::unique_lock<std::mutex> lock(inputMutex);
	std::list<unsigned int> deviceKeyCodeList;
	for(InputRegistrationList::const_iterator i = inputRegistrationList.begin(); i != inputRegistrationList.end(); ++i)
	{
		const InputRegistration& inputRegistration = *i;
		if(inputRegistration.targetDevice == targetDevice)
		{
			deviceKeyCodeList.push_back(inputRegistration.deviceKeyCode);
		}
	}
	return deviceKeyCodeList;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<System::KeyCode>> System::GetDeviceKeyCodePreferredDefaultMappingList(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const
{
	std::unique_lock<std::mutex> lock(inputMutex);
	std::list<KeyCode> preferredDefaultMappingList;
	InputRegistrationList::const_iterator inputRegistrationListIterator = inputRegistrationList.begin();
	while(inputRegistrationListIterator != inputRegistrationList.end())
	{
		const InputRegistration& inputRegistration = *inputRegistrationListIterator;
		if((inputRegistration.targetDevice == targetDevice) && (inputRegistration.deviceKeyCode == targetDeviceKeyCode))
		{
			if(inputRegistration.preferredSystemKeyCodeSpecified)
			{
				preferredDefaultMappingList.push_back(inputRegistration.systemKeyCode);
			}
			return preferredDefaultMappingList;
		}
		++inputRegistrationListIterator;
	}
	return preferredDefaultMappingList;
}

//----------------------------------------------------------------------------------------
bool System::IsKeyCodeMapped(KeyCode keyCode) const
{
	std::unique_lock<std::mutex> lock(inputMutex);
	return (inputKeyMap.find(keyCode) != inputKeyMap.end());
}

//----------------------------------------------------------------------------------------
bool System::IsDeviceKeyCodeMapped(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const
{
	std::unique_lock<std::mutex> lock(inputMutex);
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
System::KeyCode System::GetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int targetDeviceKeyCode) const
{
	std::unique_lock<std::mutex> lock(inputMutex);
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
	return ISystemDeviceInterface::KeyCode::None;
}

//----------------------------------------------------------------------------------------
bool System::SetDeviceKeyCodeMapping(IDevice* targetDevice, unsigned int deviceKeyCode, KeyCode systemKeyCode)
{
	std::unique_lock<std::mutex> lock(inputMutex);
	InputMapEntry mapEntry;
	mapEntry.keyCode = systemKeyCode;
	mapEntry.targetDevice = targetDevice;
	mapEntry.targetDeviceKeyCode = deviceKeyCode;
	inputKeyMap[systemKeyCode] = mapEntry;
	return true;
}

//----------------------------------------------------------------------------------------
void System::UnmapAllKeyCodeMappingsForDevice(IDevice* adevice)
{
	std::unique_lock<std::mutex> lock(inputMutex);
	std::list<KeyCode> keyCodesToRemove;
	for(InputKeyMap::iterator i = inputKeyMap.begin(); i != inputKeyMap.end(); ++i)
	{
		if(i->second.targetDevice == adevice)
		{
			keyCodesToRemove.push_back(i->second.keyCode);
		}
	}
	for(std::list<KeyCode>::const_iterator i = keyCodesToRemove.begin(); i != keyCodesToRemove.end(); ++i)
	{
		inputKeyMap.erase(*i);
	}
}

//----------------------------------------------------------------------------------------
void System::SendStoredInputEvents()
{
	std::unique_lock<std::mutex> lock(inputMutex);
	for(std::list<InputEventEntry>::iterator i = inputEvents.begin(); i != inputEvents.end(); ++i)
	{
		i->sent = true;
		InputKeyMap::const_iterator keyMapEntry = inputKeyMap.find(i->keyCode);
		if(keyMapEntry != inputKeyMap.end())
		{
			switch(i->inputEvent)
			{
			case InputEvent::KeyDown:
				keyMapEntry->second.targetDevice->HandleInputKeyDown(keyMapEntry->second.targetDeviceKeyCode);
				break;
			case InputEvent::KeyUp:
				keyMapEntry->second.targetDevice->HandleInputKeyUp(keyMapEntry->second.targetDeviceKeyCode);
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void System::ClearSentStoredInputEvents()
{
	std::unique_lock<std::mutex> lock(inputMutex);
	std::list<InputEventEntry>::iterator i = inputEvents.begin();
	while((i != inputEvents.end()) && i->sent)
	{
		++i;
	}
	inputEvents.erase(inputEvents.begin(), i);
}

//----------------------------------------------------------------------------------------
//System setting functions
//----------------------------------------------------------------------------------------
bool System::ApplySystemStateChange(const SystemStateChange& stateChange)
{
	//Apply the specified system state change
	switch(stateChange.type)
	{
	case SystemStateChangeType::SetClockFrequency:{
		//Retrieve the target clock source
		ClockSource* clockSource = GetClockSource(stateChange.moduleID, stateChange.targetElementName);
		if(clockSource == 0)
		{
			LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
			logEntry << L"Could not locate clock source with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Apply the clock rate change
		switch(stateChange.setClockRateClockType)
		{
		case ClockSource::ClockType::Direct:
			return clockSource->TransparentSetClockFrequency(stateChange.setClockRateValue);
		case ClockSource::ClockType::Divider:
			return clockSource->TransparentSetClockDivider(stateChange.setClockRateValue);
		case ClockSource::ClockType::Multiplier:
			return clockSource->TransparentSetClockMultiplier(stateChange.setClockRateValue);
		default:
			LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
			logEntry << L"An attempt was made to change a system clock rate using an invalid clock type flag with a value of \"" << (unsigned int)stateChange.setClockRateClockType << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}
		break;}
	case SystemStateChangeType::SetSystemLineState:{
		//Retrieve the target system line ID
		unsigned int systemLineID = GetSystemLineID(stateChange.moduleID, stateChange.targetElementName);
		if(systemLineID == 0)
		{
			LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
			logEntry << L"Could not locate system line with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Retrieve the width of the target system line
		unsigned int systemLineWidth = GetSystemLineWidth(systemLineID);

		//Apply the line state change
		return SetSystemLineState(systemLineID, Data(systemLineWidth, stateChange.setLineStateValue));}
	case SystemStateChangeType::SetSystemOption:{
		//Retrieve the target system setting ID
		unsigned int systemSettingID = GetSystemSettingID(stateChange.moduleID, stateChange.targetElementName);
		if(systemSettingID == 0)
		{
			LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
			logEntry << L"Could not locate system setting with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}

		//Retrieve the target system setting
		SystemSettingsMap::iterator systemSettingsIterator = systemSettings.find(systemSettingID);
		if(systemSettingsIterator == systemSettings.end())
		{
			LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
			logEntry << L"Failed to retrieve system setting info for system setting with name \"" << stateChange.targetElementName << L"\" on module with ID \"" << stateChange.moduleID << L"\" in System::ApplySystemStateChange!";
			WriteLogEvent(logEntry);
			return false;
		}
		SystemSettingInfo& systemSettingInfo = *(systemSettingsIterator->second);

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
						WriteLogEvent(LogEntry(LogEntry::EventLevel::Warning, L"System", L"Failed to apply system setting change while setting option \"" + stateChange.setSystemOptionValue + L"\" on setting \"" + systemSettingInfo.name + L"\" in System::ApplySystemStateChange!"));
						appliedWithoutErrors = false;
					}
				}

				//Return the result of this settings change operation
				return appliedWithoutErrors;
			}
		}

		//If we failed to find the target option, log the error, and return false.
		LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
		logEntry << L"Could not locate option with name \"" << stateChange.setSystemOptionValue << L"\" under setting with name \"" << systemSettingInfo.name << L"\" on module with ID \"" << systemSettingInfo.moduleID << L"\" in System::ApplySystemStateChange!";
		WriteLogEvent(logEntry);
		return false;}
	default:
		LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
		logEntry << L"An attempt was made to apply an invalid system state change with a value of \"" << (unsigned int)stateChange.type << L"\" in System::ApplySystemStateChange!";
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
		LogEntry logEntry(LogEntry::EventLevel::Warning, L"System", L"");
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
