#include "Breakpoint.h"
#include <sstream>
#include "WindowFunctions/WindowFunctions.pkg"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Breakpoint::~Breakpoint()
{}

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

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Breakpoint::LoadState(IHeirarchicalStorageNode& node)
{
	node.ExtractAttribute(L"Name", name);
	node.ExtractAttribute(L"LogEvent", logEvent);
	node.ExtractAttribute(L"BreakEvent", breakEvent);
	node.ExtractAttribute(L"LocationConditionNot", locationConditionNot);
	unsigned int locationConditionAsInt;
	node.ExtractAttribute(L"LocationCondition", locationConditionAsInt);
	locationCondition = (Condition)locationConditionAsInt;
	node.ExtractAttributeHex(L"LocationConditionData1", locationConditionData1);
	node.ExtractAttributeHex(L"LocationConditionData2", locationConditionData2);
	node.ExtractAttribute(L"HitCounter", hitCounter);
	node.ExtractAttribute(L"HitCounterIncrement", hitCounterIncrement);
	node.ExtractAttribute(L"BreakOnCounter", breakOnCounter);
	node.ExtractAttribute(L"BreakCounter", breakCounter);
}

//----------------------------------------------------------------------------------------
void Breakpoint::SaveState(IHeirarchicalStorageNode& node) const
{
	node.CreateAttribute(L"Name", name);
	node.CreateAttribute(L"LogEvent", logEvent);
	node.CreateAttribute(L"BreakEvent", breakEvent);
	node.CreateAttribute(L"LocationConditionNot", locationConditionNot);
	node.CreateAttribute(L"LocationCondition", (unsigned int)locationCondition);
	node.CreateAttributeHex(L"LocationConditionData1", locationConditionData1, 8);
	node.CreateAttributeHex(L"LocationConditionData2", locationConditionData2, 8);
	node.CreateAttribute(L"HitCounter", hitCounter);
	node.CreateAttribute(L"HitCounterIncrement", hitCounterIncrement);
	node.CreateAttribute(L"BreakOnCounter", breakOnCounter);
	node.CreateAttribute(L"BreakCounter", breakCounter);
}
