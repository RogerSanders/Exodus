#include "Breakpoint.h"
#include <sstream>
#include "WindowFunctions/WindowFunctions.pkg"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Breakpoint logging functions
//----------------------------------------------------------------------------------------
std::wstring Breakpoint::GetLogString() const
{
	std::wstringstream stream;
	stream << std::hex << std::uppercase << L"Breakpoint triggered at 0x" << GetLocationConditionData1();
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Location condition functions
//----------------------------------------------------------------------------------------
bool Breakpoint::PassesLocationCondition(unsigned int location)
{
	bool result = true;
	switch(GetLocationCondition())
	{
	case CONDITION_EQUAL:
		result = (location == GetLocationConditionData1());
		break;
	case CONDITION_GREATER:
		result = (location > GetLocationConditionData1());
		break;
	case CONDITION_LESS:
		result = (location < GetLocationConditionData1());
		break;
	case CONDITION_GREATER_AND_LESS:
		result = (location > GetLocationConditionData1()) && (location < GetLocationConditionData2());
		break;
	}
	result ^= GetLocationConditionNot();
	return result;
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
bool Breakpoint::CheckHitCounter()
{
	IncrementHitCounter();
	if(GetBreakOnCounter())
	{
		if((GetLiveHitCounter() % GetBreakCounter()) != 0)
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Breakpoint::GenerateName(unsigned int addressCharWidth) const
{
	std::wstring newName;
	IntToStringBase16(GetLocationConditionData1(), newName, addressCharWidth, false);
	return newName;
}
