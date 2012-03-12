#include "Watchpoint.h"
#include <sstream>

//----------------------------------------------------------------------------------------
//Breakpoint logging functions
//----------------------------------------------------------------------------------------
std::wstring Watchpoint::GetLogString() const
{
	std::wstringstream stream;
	stream << std::hex << std::uppercase << L"Watchpoint triggered at 0x" << GetLocationConditionData1();
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Read condition functions
//----------------------------------------------------------------------------------------
bool Watchpoint::PassesReadCondition(unsigned int data)
{
	bool result = true;
	if(GetReadConditionEnabled())
	{
		switch(GetReadCondition())
		{
		case CONDITION_EQUAL:
			result = (data == GetReadConditionData1());
			break;
		case CONDITION_GREATER:
			result = (data > GetReadConditionData1());
			break;
		case CONDITION_LESS:
			result = (data < GetReadConditionData1());
			break;
		case CONDITION_GREATER_AND_LESS:
			result = (data > GetReadConditionData1()) && (data < GetReadConditionData2());
			break;
		}
		result ^= GetReadConditionNot();
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Write condition functions
//----------------------------------------------------------------------------------------
bool Watchpoint::PassesWriteCondition(unsigned int data)
{
	bool result = true;
	if(GetWriteConditionEnabled())
	{
		switch(GetWriteCondition())
		{
		case CONDITION_EQUAL:
			result = (data == GetWriteConditionData1());
			break;
		case CONDITION_GREATER:
			result = (data > GetWriteConditionData1());
			break;
		case CONDITION_LESS:
			result = (data < GetWriteConditionData1());
			break;
		case CONDITION_GREATER_AND_LESS:
			result = (data > GetWriteConditionData1()) && (data < GetWriteConditionData2());
			break;
		}
		result ^= GetWriteConditionNot();
	}
	return result;
}
