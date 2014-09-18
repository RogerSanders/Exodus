#include "Breakpoint.h"
#include <sstream>
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetIBreakpointVersion() const
{
	return ThisIBreakpointVersion();
}

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
//Breakpoint event triggers
//----------------------------------------------------------------------------------------
bool Breakpoint::GetEnabled() const
{
	return enabled;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetEnabled(bool state)
{
	enabled = state;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetLogEvent() const
{
	return logEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLogEvent(bool state)
{
	logEvent = state;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakEvent() const
{
	return breakEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakEvent(bool state)
{
	breakEvent = state;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
void Breakpoint::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void Breakpoint::GenerateNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GenerateName());
}

//----------------------------------------------------------------------------------------
std::wstring Breakpoint::GenerateName() const
{
	std::wstring newName;
	switch(locationCondition)
	{
	case CONDITION_EQUAL:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, addressBusCharWidth, false);
		newName += (!locationConditionNot)? locationData1AsString: L"!=" + locationData1AsString;
		break;}
	case CONDITION_GREATER:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, addressBusCharWidth, false);
		newName += (!locationConditionNot)? L">" + locationData1AsString: L"<=" + locationData1AsString;
		break;}
	case CONDITION_LESS:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, addressBusCharWidth, false);
		newName += (!locationConditionNot)? L"<" + locationData1AsString: L">=" + locationData1AsString;
		break;}
	case CONDITION_GREATER_AND_LESS:{
		std::wstring locationData1AsString;
		std::wstring locationData2AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, addressBusCharWidth, false);
		IntToStringBase16(GetLocationConditionData2(), locationData2AsString, addressBusCharWidth, false);
		newName += (!locationConditionNot)? L">" + locationData1AsString + L" && " + L"<" + locationData2AsString: L"<=" + locationData1AsString + L" || " + L">=" + locationData2AsString;
		break;}
	}
	return newName;
}

//----------------------------------------------------------------------------------------
//Location condition functions
//----------------------------------------------------------------------------------------
bool Breakpoint::GetLocationConditionNot() const
{
	return locationConditionNot;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionNot(bool state)
{
	locationConditionNot = state;
}

//----------------------------------------------------------------------------------------
Breakpoint::Condition Breakpoint::GetLocationCondition() const
{
	return locationCondition;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationCondition(Condition condition)
{
	locationCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData1() const
{
	return locationConditionData1;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData1(unsigned int data)
{
	locationConditionData1 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData2() const
{
	return locationConditionData2;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData2(unsigned int data)
{
	locationConditionData2 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationMask() const
{
	return locationMask;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationMask(unsigned int data)
{
	locationMask = data & ((1 << addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
bool Breakpoint::PassesLocationCondition(unsigned int location)
{
	bool result = true;
	unsigned int locationMasked = (location & locationMask);
	switch(GetLocationCondition())
	{
	case CONDITION_EQUAL:
		result = (locationMasked == GetLocationConditionData1());
		break;
	case CONDITION_GREATER:
		result = (locationMasked > GetLocationConditionData1());
		break;
	case CONDITION_LESS:
		result = (locationMasked < GetLocationConditionData1());
		break;
	case CONDITION_GREATER_AND_LESS:
		result = (locationMasked > GetLocationConditionData1()) && (locationMasked < GetLocationConditionData2());
		break;
	}
	result ^= GetLocationConditionNot();
	return result;
}

//----------------------------------------------------------------------------------------
//Hit counter functions
//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetHitCounter() const
{
	return hitCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetHitCounter(unsigned int ahitCounter)
{
	hitCounter = ahitCounter;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakOnCounter() const
{
	return breakOnCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakOnCounter(bool state)
{
	breakOnCounter = state;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetBreakCounter() const
{
	return breakCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakCounter(unsigned int abreakCounter)
{
	breakCounter = (abreakCounter > 0)? abreakCounter: 1;
}

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
//Savestate functions
//----------------------------------------------------------------------------------------
void Breakpoint::LoadState(IHierarchicalStorageNode& node)
{
	node.ExtractAttribute(L"Name", name);
	node.ExtractAttribute(L"Enabled", enabled);
	node.ExtractAttribute(L"LogEvent", logEvent);
	node.ExtractAttribute(L"BreakEvent", breakEvent);
	node.ExtractAttribute(L"LocationConditionNot", locationConditionNot);
	unsigned int locationConditionAsInt;
	node.ExtractAttribute(L"LocationCondition", locationConditionAsInt);
	locationCondition = (Condition)locationConditionAsInt;
	node.ExtractAttributeHex(L"LocationConditionData1", locationConditionData1);
	node.ExtractAttributeHex(L"LocationConditionData2", locationConditionData2);
	node.ExtractAttributeHex(L"LocationMask", locationMask);
	node.ExtractAttribute(L"HitCounter", hitCounter);
	node.ExtractAttribute(L"HitCounterIncrement", hitCounterIncrement);
	node.ExtractAttribute(L"BreakOnCounter", breakOnCounter);
	node.ExtractAttribute(L"BreakCounter", breakCounter);
}

//----------------------------------------------------------------------------------------
void Breakpoint::SaveState(IHierarchicalStorageNode& node) const
{
	node.CreateAttribute(L"Name", name);
	node.CreateAttribute(L"Enabled", enabled);
	node.CreateAttribute(L"LogEvent", logEvent);
	node.CreateAttribute(L"BreakEvent", breakEvent);
	node.CreateAttribute(L"LocationConditionNot", locationConditionNot);
	node.CreateAttribute(L"LocationCondition", (unsigned int)locationCondition);
	node.CreateAttributeHex(L"LocationConditionData1", locationConditionData1, 8);
	node.CreateAttributeHex(L"LocationConditionData2", locationConditionData2, 8);
	node.CreateAttributeHex(L"LocationMask", locationMask, 8);
	node.CreateAttribute(L"HitCounter", hitCounter);
	node.CreateAttribute(L"HitCounterIncrement", hitCounterIncrement);
	node.CreateAttribute(L"BreakOnCounter", breakOnCounter);
	node.CreateAttribute(L"BreakCounter", breakCounter);
}
