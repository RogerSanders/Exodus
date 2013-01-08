#include "System.h"
#include "HeirarchicalStorage/HeirarchicalStorage.pkg"
#include "Stream/Stream.pkg"
#include "ZIP/ZIP.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "Image/Image.pkg"
#include <time.h>
#include <boost/bind.hpp>
#include <sstream>
#include <shlwapi.h>
//##DEBUG##
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool System::LoadState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState)
{
	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Open the target file
	std::wstring filePath = fileDir + L"\\" + fileName;
	Stream::File source;
	if(!source.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the file could not be opened!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}

	HeirarchicalStorageTree tree;
	if(fileType == FILETYPE_ZIP)
	{
		//Load the ZIP header structure
		ZIPArchive archive;
		if(!archive.LoadFromStream(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the zip file structure could not be decoded!"));
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the save.xml file could not be found within the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		Stream::Buffer buffer(0);
		if(!entry->Decompress(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because there was an error decompressing the save.xml file from the zip archive!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		buffer.SetStreamPos(0);
		buffer.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the binary data file " + binaryFileName + L" could not be found within the zip archive!"));
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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because there was an error decompressing the binary data file " + binaryFileName + L" from the zip archive!"));
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
		source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
		source.ProcessByteOrderMark();

		//Attempt to load the XML tree from the file
		if(!tree.LoadTree(source))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the xml structure could not be decoded! The xml decode error string is as follows: " + tree.GetErrorString()));
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
			//##TODO## Make this work with absolute paths as well as relative paths
			std::wstring binaryFileName = (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = fileDir + L"\\" + binaryFileName;
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
			binaryData.SetStreamPos(0);

			unsigned int bufferSize = (unsigned int)binaryFile.Size();
			unsigned char* buffer = new unsigned char[bufferSize];
			if(!binaryFile.ReadData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!binaryData.WriteData(buffer, bufferSize))
			{
				delete[] buffer;
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the root node in the XML tree wasn't of type \"State\"!"));
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
						device->LoadState(*(*i));
					}
				}

				//If a matching loaded device couldn't be located, log an error.
				if(!foundDevice)
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"While loading state data from file " + fileName + L" state data was found for device " + deviceName + L" , which could not be located in the system. The state data for this device will be ignored, and the state will continue to load, but note that the system may not run as expected."));
				}
			}
		}
		//Load the ModuleRelationships node
		else if(elementName == L"ModuleRelationships")
		{
			if(!LoadModuleRelationshipsNode(*(*i), relationshipMap))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load state from file " + fileName + L" because the ModuleRelationships node could not be loaded!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
		}
		else
		{
			//Log a warning for an unrecognised element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognised element: " + elementName + L" when loading state from file " + fileName + L"."));
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
bool System::SaveState(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType, bool debuggerState)
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

	std::wstring filePath = fileDir + L"\\" + fileName;
	if(fileType == FILETYPE_ZIP)
	{
		//Save the XML tree to a unicode buffer
		Stream::Buffer buffer(Stream::IStream::TEXTENCODING_UTF16, 0);
		buffer.InsertByteOrderMark();
		if(!tree.SaveTree(buffer))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error compressing the save.xml file!"));
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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error compressing the " + binaryFileName + L" file!"));
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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error creating the screenshot file with a file name of " + screenshotFilename + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			screenshotFile.SetStreamPos(0);
			if(!entry.Compress(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error compressing the " + screenshotFilename + L" file!"));
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		if(!archive.SaveToStream(target))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error saving the zip structure to the file!"));
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
		Stream::File file(Stream::IStream::TEXTENCODING_UTF16);
		if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error creating the file at the full path of " + filePath + L"!"));
			if(running)
			{
				RunSystem();
			}
			return false;
		}
		file.InsertByteOrderMark();
		if(!tree.SaveTree(file))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
			if(running)
			{
				RunSystem();
			}
			return false;
		}

		//Save external binary data to separate files
		std::list<IHeirarchicalStorageNode*> binaryList;
		binaryList = tree.GetBinaryDataNodeList();
		for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
		{
			std::wstring binaryFileName = fileName + L" - " + (*i)->GetBinaryDataBufferName() + L".bin";
			std::wstring binaryFilePath = fileDir + L"\\" + binaryFileName;
			Stream::File binaryFile;
			if(!binaryFile.Open(binaryFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error creating the binary data file " + binaryFileName + L" at the full path of " + binaryFilePath + L"!"));
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
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error reading the source data from memory to save to the binary data file " + binaryFileName + L"!"));
					if(running)
					{
						RunSystem();
					}
					return false;
				}
				if(!binaryFile.WriteData(temp))
				{
					WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error writing to the binary data file " + binaryFileName + L"!"));
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
			std::wstring screenshotFilePath = fileDir + L"\\" + screenshotFilenameFull;
			Stream::File screenshotFile;
			if(!screenshotFile.Open(screenshotFilePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error creating the screenshot file with a file name of " + screenshotFilenameFull + L" with a full path of " + screenshotFilePath + L"!"));
				if(running)
				{
					RunSystem();
				}
				return false;
			}
			if(!screenshot.SavePNGImage(screenshotFile))
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save state to file " + fileName + L" because there was an error saving the screenshot to the " + screenshotFilenameFull + L" file!"));
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
System::StateInfo System::GetStateInfo(const std::wstring& fileDir, const std::wstring& fileName, FileType fileType) const
{
	StateInfo stateInfo;
	stateInfo.valid = false;

	//Open the target file
	std::wstring filePath = fileDir + L"\\" + fileName;
	Stream::File source;
	if(!source.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		return stateInfo;
	}

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
		buffer.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
		buffer.ProcessByteOrderMark();
		if(!tree.LoadTree(buffer))
		{
			return stateInfo;
		}
	}
	else if(fileType == FILETYPE_XML)
	{
		source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
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
				IHeirarchicalStorageAttribute* fileDirAttribute = (*i)->GetAttribute(L"FileDir");
				IHeirarchicalStorageAttribute* fileNameAttribute = (*i)->GetAttribute(L"FileName");
				if((fileDirAttribute != 0) && (fileNameAttribute != 0))
				{
					savedData.fileDir = fileDirAttribute->GetValue();
					savedData.fileName = fileNameAttribute->GetValue();
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
		const ConnectorDetails& connectorDetails = i->second;
		if(connectorDetails.connectorUsed)
		{
			ConnectorInfo connectorInfo;
			connectorInfo.connectorID = connectorDetails.connectorID;
			connectorInfo.exportingModuleID = connectorDetails.exportingModuleID;
			connectorInfo.connectorClassName = connectorDetails.connectorClassName;
			connectorInfo.exportingModuleConnectorInstanceName = connectorDetails.exportingModuleConnectorInstanceName;
			connectorInfo.systemClassName = connectorDetails.systemClassName;
			connectorInfo.connectorUsed = connectorDetails.connectorUsed;
			connectorInfo.importingModuleID = connectorDetails.importingModuleID;
			connectorInfo.importingModuleConnectorInstanceName = connectorDetails.importingModuleConnectorInstanceName;
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
			const LoadedModuleInfo& loadedModuleInfo = *loadedModuleIterator;
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
bool System::DoesLoadedModuleMatchSavedModule(const SavedRelationshipMap& savedRelationshipData, const SavedRelationshipModule& savedModuleInfo, const LoadedModuleInfo& loadedModuleInfo, const ConnectorInfoMapOnImportingModuleID& connectorDetailsOnImportingModuleID) const
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
				if((connectorInfo.connectorClassName == i->className) && (connectorInfo.exportingModuleConnectorInstanceName == i->instanceNameRemote) && (connectorInfo.importingModuleConnectorInstanceName == i->instanceNameLocal))
				{
					//Find the saved info on the module which exports this connector
					SavedRelationshipMap::const_iterator savedRelationshipDataForExportingModuleIterator = savedRelationshipData.find(i->moduleID);
					if(savedRelationshipDataForExportingModuleIterator != savedRelationshipData.end())
					{
						const SavedRelationshipModule& savedModuleInfoForExportingModule = savedRelationshipDataForExportingModuleIterator->second;
						//Find the loaded info on the module which exports the matching
						//connector
						LoadedModuleInfo loadedModuleInfoForExportingModule;
						if(GetLoadedModuleInfo(connectorInfo.exportingModuleID, loadedModuleInfoForExportingModule))
						{
							//Recursively validate that the exporting module, and all
							//modules it is dependent on, match their corresponding saved
							//module info.
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
			std::wstring fileDir = i->fileDir;
			if(PathIsPrefix(&relativePathBase[0], &fileDir[0]) == TRUE)
			{
				TCHAR relativePath[MAX_PATH];
				if(PathRelativePathTo(&relativePath[0], &relativePathBase[0], FILE_ATTRIBUTE_DIRECTORY, &fileDir[0], FILE_ATTRIBUTE_DIRECTORY) == TRUE)
				{
					fileDir = relativePath;
				}
			}

			moduleNode.CreateAttribute(L"FileDir", fileDir);
			moduleNode.CreateAttribute(L"FileName", i->fileName);
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
//Logging functions
//----------------------------------------------------------------------------------------
void System::WriteLogEvent(const ILogEntry& entry) const
{
	boost::mutex::scoped_lock lock(debugMutex);
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
	boost::mutex::scoped_lock lock(debugMutex);
	log.clear();
}

//----------------------------------------------------------------------------------------
void System::SaveEventLog(const std::wstring& fileName)
{
	Stream::File target;
	if(target.Open(fileName, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		boost::mutex::scoped_lock lock(debugMutex);
		target.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
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
	boost::mutex::scoped_lock lock(debugMutex);
	return eventLogSize;
}

//----------------------------------------------------------------------------------------
void System::SetEventLogSize(unsigned int alogSize)
{
	boost::mutex::scoped_lock lock(debugMutex);
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
			executionManager.Initialize();

			//Start active device threads
			executionManager.BeginExecution();

			//Commit changes from initialization
			executionManager.Commit();

			//Clear the initialize flag
			initialize = false;
		}

		//##TODO## Make it possible to configure the maximum timeslice size
		//##TODO## Build in heuristics to calculate the optimal maximum timeslice size
		double systemStepTime = ExecuteSystemStep(40000000.0);
		accumulatedExecutionTime += systemStepTime;

		//##DEBUG##
		//Note that this kills performance
//		std::wcout << std::setprecision(16) << "System Step: " << systemStepTime << '\t' << accumulatedExecutionTime << '\n';

		//If we're running too fast (*chuckle*), delay execution until we get back in
		//sync.
		if(accumulatedExecutionTime >= 40000000.0)
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
		executionManager.Initialize();
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
		if(arollbackDevice->GetTargetDevice()->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
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
	//Make sure a valid device name has been supplied
	if(entry.GetDeviceName().empty())
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device. No device name was supplied."));
		return false;
	}

	//Make sure an allocator function has been supplied
	if(entry.GetAllocator() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device " + entry.GetDeviceName() + L". No allocator function was supplied."));
		return false;
	}

	//Make sure a destructor function has been supplied
	if(entry.GetDestructor() == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error registering device " + entry.GetDeviceName() + L". No destructor function was supplied."));
		return false;
	}

	//Check if we already have a device with the same name registered
	DeviceLibraryList::iterator existingEntry = deviceLibrary.find(entry.GetDeviceName());
	if(existingEntry != deviceLibrary.end())
	{
		if(existingEntry->second.deviceVersionNo >= entry.GetDeviceVersionNo())
		{
			//If we already have a newer version of this device registered, log the event, and
			//return true.
			std::wstringstream message;
			message << L"Ignored device " << entry.GetDeviceName() << L" with version number " << entry.GetDeviceVersionNo() << L" because another version of this device has already been registered with a version number of " << existingEntry->second.deviceVersionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));
			return true;
		}
		else
		{
			//Log the fact we just overrode an existing device registration
			std::wstringstream message;
			message << L"Device " << entry.GetDeviceName() << L" with version number " << entry.GetDeviceVersionNo() << L" overrode the existing registration for this device with version number " << existingEntry->second.deviceVersionNo << L".";
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", message.str()));

			//If the existing device registration is for an older version, remove the old
			//device registration. This new registration with a higher version number
			//supersedes the old one.
			UnregisterDevice(entry.GetDeviceName());
		}
	}

	//Register the device
	DeviceLibraryEntry listEntry;
	listEntry.Allocator = entry.GetAllocator();
	listEntry.Destructor = entry.GetDestructor();
	listEntry.deviceName = entry.GetDeviceName();
	listEntry.deviceVersionNo = entry.GetDeviceVersionNo();
	listEntry.assemblyHandle = assemblyHandle;
	deviceLibrary.insert(DeviceLibraryListEntry(entry.GetDeviceName(), listEntry));

	//Log the device registration
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Successfully registered device " + entry.GetDeviceName() + L"."));

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
//Device creation and deletion
//----------------------------------------------------------------------------------------
bool System::AddDevice(unsigned int moduleID, IDevice* device, DeviceContext* deviceContext)
{
	boost::mutex::scoped_lock lock(debugMutex);

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
		device = i->second.Allocator(instanceName.c_str(), moduleID);
		device->SetAssemblyHandle(i->second.assemblyHandle);
	}
	return device;
}

//----------------------------------------------------------------------------------------
void System::DestroyDevice(const std::wstring& deviceName, IDevice* device) const
{
	DeviceLibraryList::const_iterator i = deviceLibrary.find(deviceName);
	if(i != deviceLibrary.end())
	{
		i->second.Destructor(device);
	}
}

//----------------------------------------------------------------------------------------
void System::UnloadDevice(IDevice* adevice)
{
	boost::mutex::scoped_lock lock(debugMutex);

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

	//Remove the device itself from the system
	executionManager.RemoveDevice((DeviceContext*)adevice->GetDeviceContext());
	RemoveDeviceFromDeviceList(devices, adevice);

	//Destroy the device
	DestroyDevice(adevice->GetDeviceInstanceName(), adevice);
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
void System::LoadModuleSynchronous(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher)
{
	loadSystemComplete = false;
	loadSystemProgress = 0;
	loadSystemAbort = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&System::LoadModule), this, fileDir, fileName, boost::ref(connectorMappings), boost::ref(aviewModelLauncher)));
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
bool System::LoadModule(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, IViewModelLauncher& aviewModelLauncher)
{
	//Initialize the system load status
	loadSystemAbort = false;
	loadSystemResult = true;

	//Save running state and pause system
	bool running = SystemRunning();
	StopSystem();

	//Load the module file
	std::list<LoadedModuleInfo> addedModules;
	std::list<ViewModelOpenRequest> viewModelOpenRequests;
	std::list<InputRegistration> inputRegistrationRequests;
	if(!LoadModuleInternal(fileDir, fileName, connectorMappings, viewModelOpenRequests, inputRegistrationRequests, addedModules))
	{
		//If there's an error loading the module, log the failure, and return false.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + fileName + L"!"));
		for(std::list<LoadedModuleInfo>::const_iterator addedModuleIterator = addedModules.begin(); addedModuleIterator != addedModules.end(); ++addedModuleIterator)
		{
			UnloadModuleInternal(addedModuleIterator->moduleID);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + fileName + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
	}

	//Build a set of all loaded module ID's, and detect if we just loaded a program
	//module.
	std::set<unsigned int> addedModuleIDs;
	bool programModuleLoaded = false;
	for(std::list<LoadedModuleInfo>::const_iterator i = addedModules.begin(); i != addedModules.end(); ++i)
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"BindCELineMappings failed for BusInterface " + i->name + L" when loading module from file " + fileName + L"!"));
			for(std::list<LoadedModuleInfo>::const_iterator addedModuleIterator = addedModules.begin(); addedModuleIterator != addedModules.end(); ++addedModuleIterator)
			{
				UnloadModuleInternal(addedModuleIterator->moduleID);
			}
			if(!ValidateSystem())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + fileName + L" failed."));
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

	//Initialize external connections for all devices we just loaded
	//##FIX## This isn't going to do what we want in all cases. What about where modules
	//are loaded or unloaded at runtime? If an already loaded device sets the initial line
	//state when it is loaded, then another device is later loaded that uses those lines,
	//the second device is not going to receive the line state changes made by the first
	//device. Our old method had the same problem. It seems that in order to solve this,
	//we do need the BusInterface objects themselves to track the current state of all bus
	//lines. Note that we already plan to do this in order to support the logic analyzer
	//debug feature.
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		if(addedModuleIDs.find(i->moduleID) != addedModuleIDs.end())
		{
			i->deviceContext->InitializeExternalConnections();
		}
	}

	//Perform any additional construction tasks the devices within the system require
	if(!ValidateSystem())
	{
		//If there's an error building the system, log the failure, and return false.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"System validation failed after loading module from file " + fileName + L"!"));
		for(std::list<LoadedModuleInfo>::const_iterator addedModuleIterator = addedModules.begin(); addedModuleIterator != addedModules.end(); ++addedModuleIterator)
		{
			UnloadModuleInternal(addedModuleIterator->moduleID);
		}
		if(!ValidateSystem())
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Error restoring system after loading of module " + fileName + L" failed."));
		}
		loadSystemComplete = true;
		loadSystemResult = false;
		return false;
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
	WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"Loaded module from file " + fileName + L"."));

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
bool System::LoadModuleInternal(const std::wstring& fileDir, const std::wstring& fileName, const ConnectorMappingList& connectorMappings, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<LoadedModuleInfo>& addedModules)
{
	//Open the target file
	std::wstring filePath = fileDir + L"\\" + fileName;
	Stream::File source;
	if(!source.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not open module file " + fileName + L"!"));
		return false;
	}

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
	source.ProcessByteOrderMark();

	//Load the XML structure from the file
	HeirarchicalStorageTree tree;
	if(!tree.LoadTree(source))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error loading XML structure from module file " + fileName + L"! The xml error string is as follows: " + tree.GetErrorString()));
		return false;
	}
	IHeirarchicalStorageNode& rootNode = tree.GetRootNode();

	//Load external binary data into the XML tree
	std::list<IHeirarchicalStorageNode*> binaryList;
	binaryList = tree.GetBinaryDataNodeList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = binaryList.begin(); i != binaryList.end(); ++i)
	{
		std::wstring binaryFileName = (*i)->GetBinaryDataBufferName();
		std::wstring binaryFilePath = fileDir + L"\\" + binaryFileName;
		Stream::File binaryFile;
		if(!binaryFile.Open(binaryFilePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + fileName + L" because the binary data file " + binaryFileName + L" could not be found in the target path " + fileDir + L"!"));
			return false;
		}
		Stream::IStream& binaryData = (*i)->GetBinaryDataBufferStream();
		binaryData.SetStreamPos(0);

		unsigned int bufferSize = (unsigned int)binaryFile.Size();
		unsigned char* buffer = new unsigned char[bufferSize];
		if(!binaryFile.ReadData(buffer, bufferSize))
		{
			delete[] buffer;
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + fileName + L" because there was an error reading binary data from file " + binaryFileName + L"!"));
			return false;
		}
		if(!binaryData.WriteData(buffer, bufferSize))
		{
			delete[] buffer;
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load module from file " + fileName + L" because there was an error saving binary data read from file " + binaryFileName + L"!"));
			return false;
		}
		delete[] buffer;
	}

	//If this is a system module, defer processing of this module to the LoadSystem
	//method, otherwise verify that this file is marked as a module.
	if(rootNode.GetName() == L"System")
	{
		return LoadSystem(fileDir, fileName, rootNode, viewModelOpenRequests, inputRegistrationRequests, addedModules);
	}
	else if(rootNode.GetName() != L"Module")
	{
		//Neither a system nor a module root node was found. Abort any further processing.
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Error loading module file " + fileName + L"! The root node was not of type System or Module!"));
		return false;
	}

	//Create a new module info structure for this module
	LoadedModuleInfo moduleInfo;
	moduleInfo.moduleID = GenerateFreeModuleID();
	moduleInfo.fileDir = fileDir;
	moduleInfo.fileName = fileName;

	//Extract mandatory module metadata
	IHeirarchicalStorageAttribute* systemClassNameAttribute = rootNode.GetAttribute(L"SystemClassName");
	IHeirarchicalStorageAttribute* moduleClassNameAttribute = rootNode.GetAttribute(L"ModuleClassName");
	IHeirarchicalStorageAttribute* moduleInstanceNameAttribute = rootNode.GetAttribute(L"ModuleInstanceName");
	if((systemClassNameAttribute == 0) || (moduleClassNameAttribute == 0) || (moduleInstanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing SystemClassName, ModuleClassName, or ModuleInstanceName attribute on root node!"));
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

	//Generate the display name for the module if one hasn't been specified
	if(moduleInfo.displayName.empty())
	{
		moduleInfo.displayName = moduleInfo.instanceName;
	}

	//Check for existing modules with the same display name, and ensure a unique display
	//name is generated if a conflict is found.
	unsigned int currentPostfixNumber = 0;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	std::wstring initialModuleDisplayName = moduleInfo.displayName;
	while(loadedModuleIterator != loadedModuleInfoList.end())
	{
		//If the display name is the same, try and make a new unique display name for our
		//module.
		if(loadedModuleIterator->displayName == moduleInfo.displayName)
		{
			//Determine which postfix number to use
			unsigned int numericPostfix = ++currentPostfixNumber;

			//Convert the numeric postfix to a string
			std::wstringstream numericPostfixToString;
			numericPostfixToString << numericPostfix;

			//Rebuild our display name using the postfix
			moduleInfo.displayName = initialModuleDisplayName + L" [" + numericPostfixToString.str() + L"]";

			//Restart the loop
			loadedModuleIterator = loadedModuleInfoList.begin();
			continue;
		}
		++loadedModuleIterator;
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
		else if(elementName == L"System.ExportBusInterface")
		{
			loadedWithoutErrors &= LoadModule_System_ExportBusInterface(*(*i), moduleInfo.moduleID, connectorNameToIDMap, lineGroupNameToIDMap);
		}
		else if(elementName == L"System.ExportClockSource")
		{
			loadedWithoutErrors &= LoadModule_System_ExportClockSource(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
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
		else if(elementName == L"System.ImportBusInterface")
		{
			loadedWithoutErrors &= LoadModule_System_ImportBusInterface(*(*i), moduleInfo.moduleID, connectorNameToIDMap, lineGroupNameToIDMap);
		}
		else if(elementName == L"System.ImportClockSource")
		{
			loadedWithoutErrors &= LoadModule_System_ImportClockSource(*(*i), moduleInfo.moduleID, connectorNameToIDMap);
		}
		else
		{
			//Log a warning for an unrecognised element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognised element: " + elementName + L" when loading module file " + fileName + L"."));
		}
	}

	//Add the info for this module to the list of loaded modules
	addedModules.push_back(moduleInfo);
	loadedModuleInfoList.push_back(moduleInfo);

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading module from file " + fileName + L"."));
		return false;
	}

	//If any errors occurred while loading the module file, log the error and return
	//false.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Errors occurred while loading module from file " + fileName + L"."));
		return false;
	}

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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing target module ID, device instance name, system key code, or target key code, for Device.MapInput!"));
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
bool System::LoadSystem(const std::wstring& fileDir, const std::wstring& fileName, IHeirarchicalStorageNode& rootNode, std::list<ViewModelOpenRequest>& viewModelOpenRequests, std::list<InputRegistration>& inputRegistrationRequests, std::list<LoadedModuleInfo>& addedModules)
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to load system from file " + fileName + L" because the ModuleRelationships node could not be loaded!"));
		return false;
	}

	//Attempt to load each module referenced in the saved module relationship data
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
				const ConnectorDetails& connectorDetails = connectorMapIterator->second;
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
			const ConnectorDetails& connectorDetails = connectorMapIterator->second;
			ConnectorMapping connectorMapping;
			connectorMapping.connectorID = connectorDetails.connectorID;
			connectorMapping.importingModuleConnectorInstanceName = importedConnector->instanceNameLocal;
			targetModuleConnectorMappings.push_back(connectorMapping);
		}

		//If the system file contains a relative path to this module, resolve the relative
		//file path using the directory containing the system file as a base.
		std::wstring savedModuleFileDir = savedModuleInfo.fileDir;
		if(PathIsRelative(&savedModuleInfo.fileDir[0]) == TRUE)
		{
			TCHAR combinedPath[MAX_PATH];
			PathCombine(&combinedPath[0], &fileDir[0], &savedModuleInfo.fileDir[0]);
			savedModuleFileDir = combinedPath;
		}

		//Attempt to load this module
		modulesLoadedWithoutErrors &= LoadModuleInternal(savedModuleFileDir, savedModuleInfo.fileName, targetModuleConnectorMappings, viewModelOpenRequests, inputRegistrationRequests, addedModules);

		//Save the mapping between the saved module ID and the loaded module ID for the
		//module we just loaded.
		//##FIX## If we referenced another system file, this module ID will be
		//meaningless. But then again, we don't really want to support referencing a
		//system file from within a system file. Add some kind of verification during
		//module loading to prevent this. Once we've done that, this code below will be
		//acceptable.
		std::list<LoadedModuleInfo>::const_reverse_iterator addedModuleIterator = addedModules.rbegin();
		if(addedModuleIterator != addedModules.rend())
		{
			savedModuleIDToLoadedModuleIDMap.insert(std::pair<unsigned int, unsigned int>(savedModuleInfo.moduleID, addedModuleIterator->moduleID));
		}
	}

	//If the system load was aborted, log the event and return false.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading system from file " + fileName + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error and return
	//false.
	if(!modulesLoadedWithoutErrors)
	{
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
		else if(elementName == L"Device.MapInput")
		{
			loadedWithoutErrors &= LoadSystem_Device_MapInput(*(*i), savedModuleIDToLoadedModuleIDMap);
		}
		else
		{
			//Log a warning for an unrecognised element
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_WARNING, L"System", L"Unrecognised element: " + elementName + L" when loading system file " + fileName + L"."));
		}
	}

	//If the system load was aborted, log the event, and flag the module to unload.
	if(loadSystemAbort)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_INFO, L"System", L"The user aborted loading system from file " + fileName + L"."));
		return false;
	}

	//If any errors occurred while loading the system file, log the error, and flag the
	//module to unload.
	if(!loadedWithoutErrors)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Errors occurred while loading system from file " + fileName + L"."));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool System::SaveSystem(const std::wstring& fileDir, const std::wstring& fileName)
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
	IHeirarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	SaveModuleRelationshipsNode(moduleRelationshipsNode, true, fileDir);

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

	//Save XML tree to the target file
	std::wstring filePath = fileDir + L"\\" + fileName;
	Stream::File file(Stream::IStream::TEXTENCODING_UTF16);
	if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save system to file " + fileName + L" because there was an error creating the file at the full path of " + filePath + L"!"));
		if(running)
		{
			RunSystem();
		}
		return false;
	}
	file.InsertByteOrderMark();
	if(!tree.SaveTree(file))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Failed to save system to file " + fileName + L" because there was an error saving the xml tree. The xml error string is as follows: " + tree.GetErrorString()));
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

	return result;
}

//----------------------------------------------------------------------------------------
void System::UnloadModuleInternal(unsigned int moduleID)
{
	//Build a list of any modules which import connectors exported by this module. These
	//modules are dependent on our module, and must be unloaded first.
	std::list<unsigned int> dependentModulesToUnload;
	for(ConnectorDetailsMap::const_iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		const ConnectorDetails& connectorDetails = i->second;
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

	//Free any connectors this module was importing
	for(ConnectorDetailsMap::iterator i = connectorDetailsMap.begin(); i != connectorDetailsMap.end(); ++i)
	{
		ConnectorDetails& connectorDetails = i->second;
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
			//Delete this connector
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
			//Remove any references to this bus, IE, through ReferenceDevice or
			//SetDependentDevice.
			for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
			{
				i->device->RemoveReference(currentElement->device);
				i->deviceContext->RemoveDeviceDependency(currentElement->deviceContext);
			}

			//Delete the device
			UnloadDevice(currentElement->device);
			loadedDeviceInfoList.erase(currentElement);
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
			//Delete this clock source import entry
			importedClockSources.erase(currentElement);
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
			//Delete this bus interface import entry
			importedBusInterfaces.erase(currentElement);
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
			//Delete this device import entry
			importedDeviceInfoList.erase(currentElement);
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
		lineGroupID = nextFreeConnectorID++;
		lineGroupIDFree = true;
		lineGroupIDFree = (connectorDetailsMap.find(lineGroupID) == connectorDetailsMap.end());
	} while (!lineGroupIDFree);
	return lineGroupID;
}

//----------------------------------------------------------------------------------------
bool System::LoadModule_Device(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the device class and instance names
	IHeirarchicalStorageAttribute* deviceNameAttribute = node.GetAttribute(L"DeviceName");
	IHeirarchicalStorageAttribute* instanceNameAttribute = node.GetAttribute(L"InstanceName");
	if((deviceNameAttribute == 0) || (instanceNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing device name or instance name for Device entry!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing source or target device instance name for Device.SetDependentDevice!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing source or target device instance name, or reference name, for Device.ReferenceDevice!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceDevice failed for reference from " + deviceName + L" to " + targetName + L"!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for Device.ReferenceBus!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceBus failed for reference from " + deviceName + L" to bus " + busInterfaceName + L"!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing device instance name, clock source name, or reference name, for Device.ReferenceClockSource!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate device with name " + deviceName + L" for Device.ReferenceClockSource!"));
		return false;
	}

	//Add the specified clock source reference to the device
	if(!device->AddReference(referenceName, clockSource))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Device.ReferenceClockSource failed for reference from " + deviceName + L" to " + clockSourceName + L"!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing target device instance name or device key code for Device.RegisterInput!"));
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
bool System::LoadModule_BusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the bus name
	IHeirarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing name for BusInterface!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.DefineLineGroup!"));
		return false;
	}

	//Extract the LineGroupName attribute
	IHeirarchicalStorageAttribute* lineGroupNameAttribute = node.GetAttribute(L"LineGroupName");
	if(lineGroupNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No LineGroupName attribute specified for BusInterface.DefineLineGroup!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.DefineCELineMemory!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.DefineCELinePort!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapCELineInputMemory!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapCELineInputPort!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapCELineOutputMemory!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapCELineOutputPort!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapDevice!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapPort!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No BusInterfaceName attribute specified for BusInterface.MapClockSource!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"No ClockSourceName attribute specified for BusInterface.MapClockSource!"));
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
bool System::LoadModule_ClockSource(IHeirarchicalStorageNode& node, unsigned int moduleID)
{
	//Load the clock source name
	IHeirarchicalStorageAttribute* nameAttribute = node.GetAttribute(L"Name");
	if(nameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing name for ClockSource!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing name for input ClockSource!"));
		return false;
	}
	std::wstring inputClockSourceName = inputClockSourceNameAttribute->GetValue();

	//Load the target clock source name
	IHeirarchicalStorageAttribute* targetClockSourceNameAttribute = node.GetAttribute(L"TargetClockSourceName");
	if(targetClockSourceNameAttribute == 0)
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing name for target ClockSource!"));
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
	IHeirarchicalStorageAttribute* menuHandlerNameAttribute = node.GetAttribute(L"MenuHandlerName");
	IHeirarchicalStorageAttribute* viewIDAttribute = node.GetAttribute(L"ViewID");
	if((ownerAttribute == 0) || (menuHandlerNameAttribute == 0) || (viewIDAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either Owner, MenuHandlerName, or ViewID attribute for System.OpenViewModel!"));
		return false;
	}
	std::wstring owner = ownerAttribute->GetValue();
	std::wstring menuHandlerName = menuHandlerNameAttribute->GetValue();
	int viewID = viewIDAttribute->ExtractValue<int>();

	if(owner == L"System")
	{
		//Add the open request to the queue
		ViewModelOpenRequest openRequest;
		openRequest.ownerSystem = true;
		openRequest.menuHandlerName = menuHandlerName;
		openRequest.viewID = viewID;
		viewModelOpenRequests.push_back(openRequest);
	}
	else if(owner == L"Device")
	{
		//Extract the device instance name attribute
		IHeirarchicalStorageAttribute* deviceInstanceNameAttribute = node.GetAttribute(L"DeviceInstanceName");
		if(deviceInstanceNameAttribute == 0)
		{
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing the DeviceInstanceName attribute for System.OpenViewModel!"));
			return false;
		}
		std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();

		//Add the open request to the queue
		ViewModelOpenRequest openRequest;
		openRequest.ownerSystem = false;
		openRequest.moduleID = moduleID;
		openRequest.menuHandlerName = menuHandlerName;
		openRequest.viewID = viewID;
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorClassName or ConnectorInstanceName attribute for System.ExportConnector!"));
		return false;
	}
	std::wstring connectorClassName = connectorClassNameAttribute->GetValue();
	std::wstring connectorInstanceName = connectorInstanceNameAttribute->GetValue();

	//Add details of this connector to the list of connectors which have been defined
	ConnectorDetails connectorDetails;
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ExportDevice!"));
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
	ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

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
bool System::LoadModule_System_ExportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, const NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ExportBusInterface!"));
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
	ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either LineGroupName or ImportName attribute for ExportLineGroup!"));
				return false;
			}
			std::wstring lineGroupName = childLineGroupNameAttribute->GetValue();
			std::wstring importName = childImportNameAttribute->GetValue();

			//Find the ID for the referenced line group
			NameToIDMap::const_iterator lineGroupNameToIDMapIterator = lineGroupNameToIDMap.find(lineGroupName);
			if(lineGroupNameToIDMapIterator == lineGroupNameToIDMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate line group with name \"" + lineGroupName + L"\" for ExportLineGroup!"));
				return false;
			}
			unsigned int lineGroupID = lineGroupNameToIDMapIterator->second;

			//Retrieve the target line group info
			LineGroupDetailsMap::const_iterator lineGroupDetailsMapIterator = lineGroupDetailsMap.find(lineGroupID);
			if(lineGroupDetailsMapIterator == lineGroupDetailsMap.end())
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not retrieve line group details for line group with name \"" + lineGroupName + L"\" for ExportLineGroup!"));
				return false;
			}
			const LineGroupDetails& lineGroupDetails = lineGroupDetailsMapIterator->second;

			//Verify that the specified line group belongs to the BusInterface object
			//being exported.
			if(lineGroupDetails.busInterface != busInterface)
			{
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"The specified line group with name \"" + lineGroupName + L"\" does not belong to the specified bus interface with name \"" + busInterfaceName + L"\" for ExportLineGroup!"));
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Unrecognised child element: \"" + elementName + L"\" encountered when processing System.ExportBusInterface!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ExportClockSource!"));
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
	ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

	//Add details of this exported object to the connector details
	ExportedClockSourceInfo exportedClockSourceInfo;
	exportedClockSourceInfo.clockSource = clockSource;
	exportedClockSourceInfo.exportingModuleClockSourceName = clockSourceName;
	exportedClockSourceInfo.importName = importName;
	connectorDetails.exportedClockSourceInfo.insert(std::pair<std::wstring, ExportedClockSourceInfo>(exportedClockSourceInfo.importName, exportedClockSourceInfo));

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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorClassName or ConnectorInstanceName attribute for System.ImportConnector!"));
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
				ConnectorDetails& connectorDetails = connectorDetailsMapIterator->second;
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, DeviceInstanceName, or ImportName attribute for System.ImportDevice!"));
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
	const ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

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
bool System::LoadModule_System_ImportBusInterface(IHeirarchicalStorageNode& node, unsigned int moduleID, const NameToIDMap& connectorNameToIDMap, NameToIDMap& lineGroupNameToIDMap)
{
	//Extract the ConnectorInstanceName, BusInterfaceName, and ImportName attributes
	IHeirarchicalStorageAttribute* connectorInstanceNameAttribute = node.GetAttribute(L"ConnectorInstanceName");
	IHeirarchicalStorageAttribute* busInterfaceNameAttribute = node.GetAttribute(L"BusInterfaceName");
	IHeirarchicalStorageAttribute* importNameAttribute = node.GetAttribute(L"ImportName");
	if((connectorInstanceNameAttribute == 0) || (busInterfaceNameAttribute == 0) || (importNameAttribute == 0))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, BusInterfaceName, or ImportName attribute for System.ImportBusInterface!"));
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
	const ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either LineGroupName or ImportName attribute for ImportLineGroup!"));
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
				WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not locate exported line group with name " + lineGroupName + L" on exported bus interface with name " + busInterfaceName + L" for ImportLineGroup!"));
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
			WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Unrecognised child element: " + elementName + L" encountered when processing System.ImportBusInterface!"));
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
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Missing either ConnectorInstanceName, ClockSourceName, or ImportName attribute for System.ImportClockSource!"));
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
	const ConnectorDetails& connectorDetails = connectorDetailsIterator->second;

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
			debugMenuHandler->OpenViewModel(request.menuHandlerName, request.viewID, aviewModelLauncher);
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
			device->OpenViewModel(request.menuHandlerName, request.viewID, aviewModelLauncher);
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

	//Remove key code mappings
	ClearKeyCodeMap();

	//Remove all registered input targets
	inputRegistrationList.clear();

	//Remove all connector details
	connectorDetailsMap.clear();

	//Remove all clock sources
	for(ClockSourceList::iterator i = clockSources.begin(); i != clockSources.end(); ++i)
	{
		delete (i->clockSource);
	}
	clockSources.clear();

	//Remove all bus interfaces
	for(BusInterfaceList::iterator i = busInterfaces.begin(); i != busInterfaces.end(); ++i)
	{
		delete (i->busInterface);
	}
	busInterfaces.clear();

	//Remove all devices
	for(LoadedDeviceInfoList::const_iterator i = loadedDeviceInfoList.begin(); i != loadedDeviceInfoList.end(); ++i)
	{
		lock.unlock();
		DestroyDevice(i->device->GetDeviceInstanceName(), i->device);
		lock.lock();
	}
	loadedDeviceInfoList.clear();
	executionManager.ClearAllDevices();
	devices.clear();

	//Remove all loaded modules
	loadedModuleInfoList.clear();

	clearSystemComplete = true;
}

//----------------------------------------------------------------------------------------
bool System::ReadModuleConnectorInfo(const std::wstring& fileDir, const std::wstring& fileName, std::wstring& systemClassName, ConnectorImportList& connectorsImported, ConnectorExportList& connectorsExported) const
{
	std::wstring filePath = fileDir + L"\\" + fileName;

	//Open the target file
	Stream::File source;
	if(!source.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		WriteLogEvent(LogEntry(LogEntry::EVENTLEVEL_ERROR, L"System", L"Could not open module file " + filePath + L"!"));
		return false;
	}

	//Determine the text format for the file, and strip any present byte order mark.
	source.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF16);
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
bool System::GetLoadedModuleInfo(unsigned int moduleID, LoadedModuleInfo& moduleInfo) const
{
	bool foundEntry = false;
	LoadedModuleInfoList::const_iterator loadedModuleIterator = loadedModuleInfoList.begin();
	while(!foundEntry && (loadedModuleIterator != loadedModuleInfoList.end()))
	{
		if(loadedModuleIterator->moduleID == moduleID)
		{
			moduleInfo = *loadedModuleIterator;
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
bool System::GetConnectorInfo(unsigned int connectorID, ConnectorInfo& connectorInfo) const
{
	bool foundEntry = false;
	ConnectorDetailsMap::const_iterator connectorDetailsIterator = connectorDetailsMap.begin();
	while(!foundEntry && (connectorDetailsIterator != connectorDetailsMap.end()))
	{
		if(connectorDetailsIterator->second.connectorID == connectorID)
		{
			const ConnectorDetails& connectorDetails = connectorDetailsIterator->second;
			connectorInfo.connectorID = connectorDetails.connectorID;
			connectorInfo.exportingModuleID = connectorDetails.exportingModuleID;
			connectorInfo.connectorClassName = connectorDetails.connectorClassName;
			connectorInfo.exportingModuleConnectorInstanceName = connectorDetails.exportingModuleConnectorInstanceName;
			connectorInfo.systemClassName = connectorDetails.systemClassName;
			connectorInfo.connectorUsed = connectorDetails.connectorUsed;
			connectorInfo.importingModuleID = connectorDetails.importingModuleID;
			connectorInfo.importingModuleConnectorInstanceName = connectorDetails.importingModuleConnectorInstanceName;
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
//Assembly handle functions
//----------------------------------------------------------------------------------------
void* System::GetAssemblyHandle() const
{
	return assemblyHandle;
}
