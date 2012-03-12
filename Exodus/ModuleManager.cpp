#include "ModuleManager.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleManager::ModuleManager()
:openedDatabase(false), databaseConnection(0)
{}

//----------------------------------------------------------------------------------------
//Database functions
//----------------------------------------------------------------------------------------
bool ModuleManager::OpenDatabase(const std::wstring& databaseFilename)
{
	//Close the database if it is already open
	if(openedDatabase)
	{
		CloseDatabase();
	}

	//##TODO## Set the text encoding before creating the database
	//L"PRAGMA encoding = UTF-16;"

	//Attempt to open a connection to the database
	if(sqlite3_open16(databaseFilename.c_str(), &databaseConnection) != 0)
	{
		sqlite3_close(databaseConnection);
		databaseConnection = 0;
		return false;
	}
	openedDatabase = true;

	return true;
}

//----------------------------------------------------------------------------------------
void ModuleManager::CloseDatabase()
{
	//Close the database if it is open
	if(openedDatabase)
	{
		sqlite3_close(databaseConnection);
		databaseConnection = 0;
		openedDatabase = false;
	}
}

//----------------------------------------------------------------------------------------
bool ModuleManager::ExecuteSQL(const std::wstring& sqlStatement) const
{
	//Compile the SQL statement down to byte code
	sqlite3_stmt* statement = 0;
	const void* unusedStatement = 0;
	int statementPrepareReturn;
	int bytesInStatementIncludingTerminator = (int)((sqlStatement.size() + 1) * sizeof(wchar_t));
	statementPrepareReturn = sqlite3_prepare16_v2(databaseConnection, sqlStatement.c_str(), bytesInStatementIncludingTerminator, &statement, &unusedStatement);
	if(statementPrepareReturn != SQLITE_OK)
	{
		sqlite3_finalize(statement);
		return false;
	}

	//##TODO## Step through the statement results? How are we actually going to handle
	//results. We could enumerate all requested results here and pump them into a vector
	//to return to the caller. We could also provide an iterator of sorts though, which
	//the caller constructs. The constructor prepares the SQL statement, and the iteration
	//process steps through the results. The destructor for the iterator cleans up the
	//statement. The advantage of this is that the object itself that is being pointed at
	//by the iterator can provide functions which call the various column functions
	//directly, as the data is requested. By doing this, very large structures do not have
	//to have all their values retrieved when they are iterated over. Only the values
	//which the caller accesses are enumerated, at the time they are requested. On the
	//other hand, maybe it's just better to dump the entire database into memory on-mass
	//at load time, and always work directly from memory. This eliminates any performance
	//questions about the database access speed, and also makes it easier to structure the
	//interface to our manager.
	//##TODO## We will build the interface for our manager as if the database was being
	//used to retrieve every value as it is requested. We will test both methods however,
	//both with loading the database into memory, and working from it live. We might even
	//allow the user to choose which option they want, or make the call at runtime based
	//on the size of the database.

	//Delete the prepared SQL statement structures
	sqlite3_finalize(statement);

	return true;
}

//----------------------------------------------------------------------------------------
bool ModuleManager::IsPlatformLittleEndian() const
{
	unsigned short testVar = 0x1234;
	unsigned char* bytePointer = (unsigned char*)&testVar;
	return (*bytePointer == 0x34);
}
