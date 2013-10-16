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

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void Watchpoint::LoadState(IHierarchicalStorageNode& node)
{
	Breakpoint::LoadState(node);

	node.ExtractAttribute(L"Read", read);
	node.ExtractAttribute(L"Write", write);
	node.ExtractAttribute(L"ReadConditionEnabled", readConditionEnabled);
	node.ExtractAttribute(L"ReadConditionNot", readConditionNot);
	unsigned int readConditionAsInt;
	node.ExtractAttribute(L"ReadCondition", readConditionAsInt);
	readCondition = (Condition)readConditionAsInt;
	node.ExtractAttributeHex(L"ReadConditionData1", readConditionData1);
	node.ExtractAttributeHex(L"ReadConditionData2", readConditionData1);
	node.ExtractAttribute(L"WriteConditionEnabled", writeConditionEnabled);
	node.ExtractAttribute(L"WriteConditionNot", writeConditionNot);
	unsigned int writeConditionAsInt;
	node.ExtractAttribute(L"WriteCondition", writeConditionAsInt);
	writeCondition = (Condition)writeConditionAsInt;
	node.ExtractAttributeHex(L"WriteConditionData1", writeConditionData1);
	node.ExtractAttributeHex(L"WriteConditionData2", writeConditionData1);
}

//----------------------------------------------------------------------------------------
void Watchpoint::SaveState(IHierarchicalStorageNode& node) const
{
	Breakpoint::SaveState(node);

	node.CreateAttribute(L"Read", read);
	node.CreateAttribute(L"Write", write);
	node.CreateAttribute(L"ReadConditionEnabled", readConditionEnabled);
	node.CreateAttribute(L"ReadConditionNot", readConditionNot);
	node.CreateAttribute(L"ReadCondition", (unsigned int)readCondition);
	node.CreateAttributeHex(L"ReadConditionData1", readConditionData1, 8);
	node.CreateAttributeHex(L"ReadConditionData2", readConditionData1, 8);
	node.CreateAttribute(L"WriteConditionEnabled", writeConditionEnabled);
	node.CreateAttribute(L"WriteConditionNot", writeConditionNot);
	node.CreateAttribute(L"WriteCondition", (unsigned int)writeCondition);
	node.CreateAttributeHex(L"WriteConditionData1", writeConditionData1, 8);
	node.CreateAttributeHex(L"WriteConditionData2", writeConditionData1, 8);
}
