//#ifndef __MODULEMANAGER_H__
//#define __MODULEMANAGER_H__
//#include <sqlite3.h>
//#include <string>
//
////##TODO## Do this in a neater way, perhaps in a header file we include?
//#ifdef _DEBUG
//	#ifdef _DLL
//		#ifdef _M_X64
//			#pragma comment(lib, "SQLitedrdx64")
//		#else
//			#pragma comment(lib, "SQLitedrd")
//		#endif
//	#else
//		#ifdef _M_X64
//			#pragma comment(lib, "SQLitedx64")
//		#else
//			#pragma comment(lib, "SQLited")
//		#endif
//	#endif
//#else
//	#ifdef _DLL
//		#ifdef _M_X64
//			#pragma comment(lib, "SQLitedrx64")
//		#else
//			#pragma comment(lib, "SQLitedr")
//		#endif
//	#else
//		#ifdef _M_X64
//			#pragma comment(lib, "SQLitex64")
//		#else
//			#pragma comment(lib, "SQLite")
//		#endif
//	#endif
//#endif
//
////##TODO## Implement this class, which handles all details of providing lists of available
////modules, and the associated metadata for those modules. Some of the requirements of this
////class are as follows:
////-Store cached module information in a database back-end, implemented in SQLite.
////-Functions to add or remove directories from the cache. Note that by adding a directory
////path, all files under that directory tree will be cached.
////-Upon scanning a directory, if a given file exists in the cache of scanned files, only
////open and re-read file data for files where the last modified timestamp or filesize is
////different to the stored values in the database.
////-Provide synchronous scanning of directories, and progress information on the scan.
////-Allow the cache to be invalidated, forcing a full re-scan.
////-Provide a refresh cache function, where cached directories are scanned for added,
////removed, or modified files.
////-Does not store attachments such as screenshots or boxart within database, but does have
////functions which handle the retrieval of these files. The database is queried to obtain a
////path or reference to the file, and the file is then opened and provided to the caller.
////In this way, as far as the caller is concerned, this class does provide access to these
////attachments, but the actual data is not cached in the database in the interests of
////keeping the database light weight.
////-Stores information on exported and imported connectors, to allow system connections to
////be validated and system designs to be constructed without actually parsing the system
////XML file.
////-Provides an option to check each file for changes before returning any information, to
////allow live updates to files to be handled, Eg, so that a system file can be live-edited
////in another program, and changes to its connectors will be seen immediately from within
////our program.
//
////Database design:
////Table: VersionInfo
//// -Int: Database version number
////Table: SearchPaths
//// -Int: Key (index)
//// -String[UTF16]: File search path (possibly relative to working dir)
////Table: FileCache
//// -Int: Key (index)
//// -Int: SearchPaths key
//// -String[UTF16]: File path relative to search path
//// -Int: Last modified timestamp
//// -Int: File size
////Table: ModuleInfo
//// -Int: Key (index)
//// -Int: TypeKey (1=Module/2=ModuleGroup)
//// -Int: FileCache key
//// -String[UTF16]: If cached file is an archive, stores relative file path within archive.
//// -Int: ModuleGroupInfo key
//// -String[UTF16]: Category (User-defined. Eg, System, Peripheral, Media, etc.)
//// -String[UTF16]: System class name
//// -String[UTF16]: Module class name
//// -String[UTF16]: Module instance name
//// -String[UTF16]: Manufacturer code
//// -String[UTF16]: Year
//// -String[UTF16]: Description
//// -<Any other desired metadata>
////Table: ModuleGroupInfo
//// -Int: Key (index)
//// -Int: TypeKey (1=Module/2=ModuleGroup)
//// -String[UTF16]: Group name
//// -String[UTF16]: Manufacturer code
//// -<Any other desired metadata>
////Table: Attachments
//// -Int: Key (index)
//// -Int: Type (1=Module/2=ModuleGroup, to support binding attachments to either one)
//// -Int: FileCache key
//// -String[UTF16]: If cached file is an archive, stores relative file path within archive.
//// -String[UTF16]: Category (User-defined. Eg, Screenshot, Manual, Boxart, etc.)
//// -String[UTF16]: Name
//// -String[UTF16]: Description
//// -<Any other desired metadata>
////Table: ExportedConnectors
//// -Int: Key (index)
//// -Int: ModuleInfo key
//// -String[UTF16]: ConnectorClassName
//// -String[UTF16]: ConnectorInstanceName
////Table: ImportedConnectors
//// -Int: Key (index)
//// -Int: ModuleInfo key
//// -String[UTF16]: ConnectorClassName
//// -String[UTF16]: ConnectorInstanceName
//class ModuleManager
//{
//public:
//	//Constructors
//	ModuleManager();
//
//	//Database functions
//	bool OpenDatabase(const std::wstring& databaseFilename);
//	void CloseDatabase();
//
//	//##TODO##
//
//private:
//	bool IsPlatformLittleEndian() const;
//
//	//Database functions
//	bool ExecuteSQL(const std::wstring& sqlStatement) const;
//
//private:
//	bool openedDatabase;
//	sqlite3* databaseConnection;
//};
//
//#endif
