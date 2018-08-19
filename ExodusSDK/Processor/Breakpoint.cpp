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
	return _enabled;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetEnabled(bool state)
{
	_enabled = state;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetLogEvent() const
{
	return _logEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLogEvent(bool state)
{
	_logEvent = state;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakEvent() const
{
	return _breakEvent;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakEvent(bool state)
{
	_breakEvent = state;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> Breakpoint::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> Breakpoint::GenerateName() const
{
	std::wstring newName;
	switch(_locationCondition)
	{
	case Condition::Equal:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, _addressBusCharWidth, false);
		newName += (!_locationConditionNot)? locationData1AsString: L"!=" + locationData1AsString;
		break;}
	case Condition::Greater:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, _addressBusCharWidth, false);
		newName += (!_locationConditionNot)? L">" + locationData1AsString: L"<=" + locationData1AsString;
		break;}
	case Condition::Less:{
		std::wstring locationData1AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, _addressBusCharWidth, false);
		newName += (!_locationConditionNot)? L"<" + locationData1AsString: L">=" + locationData1AsString;
		break;}
	case Condition::GreaterAndLess:{
		std::wstring locationData1AsString;
		std::wstring locationData2AsString;
		IntToStringBase16(GetLocationConditionData1(), locationData1AsString, _addressBusCharWidth, false);
		IntToStringBase16(GetLocationConditionData2(), locationData2AsString, _addressBusCharWidth, false);
		newName += (!_locationConditionNot)? L">" + locationData1AsString + L" && " + L"<" + locationData2AsString: L"<=" + locationData1AsString + L" || " + L">=" + locationData2AsString;
		break;}
	}
	return newName;
}

//----------------------------------------------------------------------------------------
//Location condition functions
//----------------------------------------------------------------------------------------
bool Breakpoint::GetLocationConditionNot() const
{
	return _locationConditionNot;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionNot(bool state)
{
	_locationConditionNot = state;
}

//----------------------------------------------------------------------------------------
Breakpoint::Condition Breakpoint::GetLocationCondition() const
{
	return _locationCondition;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationCondition(Condition condition)
{
	_locationCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData1() const
{
	return _locationConditionData1;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData1(unsigned int data)
{
	_locationConditionData1 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationConditionData2() const
{
	return _locationConditionData2;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationConditionData2(unsigned int data)
{
	_locationConditionData2 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetLocationMask() const
{
	return _locationMask;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetLocationMask(unsigned int data)
{
	_locationMask = data & ((1 << _addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
bool Breakpoint::PassesLocationCondition(unsigned int location)
{
	bool result = true;
	unsigned int locationMasked = (location & _locationMask);
	switch(GetLocationCondition())
	{
	case Condition::Equal:
		result = (locationMasked == GetLocationConditionData1());
		break;
	case Condition::Greater:
		result = (locationMasked > GetLocationConditionData1());
		break;
	case Condition::Less:
		result = (locationMasked < GetLocationConditionData1());
		break;
	case Condition::GreaterAndLess:
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
	return _hitCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetHitCounter(unsigned int hitCounter)
{
	_hitCounter = hitCounter;
}

//----------------------------------------------------------------------------------------
bool Breakpoint::GetBreakOnCounter() const
{
	return _breakOnCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakOnCounter(bool state)
{
	_breakOnCounter = state;
}

//----------------------------------------------------------------------------------------
unsigned int Breakpoint::GetBreakCounter() const
{
	return _breakCounter;
}

//----------------------------------------------------------------------------------------
void Breakpoint::SetBreakCounter(unsigned int breakCounter)
{
	_breakCounter = (breakCounter > 0)? breakCounter: 1;
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
	node.ExtractAttribute(L"Name", _name);
	node.ExtractAttribute(L"Enabled", _enabled);
	node.ExtractAttribute(L"LogEvent", _logEvent);
	node.ExtractAttribute(L"BreakEvent", _breakEvent);
	node.ExtractAttribute(L"LocationConditionNot", _locationConditionNot);
	unsigned int locationConditionAsInt;
	node.ExtractAttribute(L"LocationCondition", locationConditionAsInt);
	_locationCondition = (Condition)locationConditionAsInt;
	node.ExtractAttributeHex(L"LocationConditionData1", _locationConditionData1);
	node.ExtractAttributeHex(L"LocationConditionData2", _locationConditionData2);
	node.ExtractAttributeHex(L"LocationMask", _locationMask);
	node.ExtractAttribute(L"HitCounter", _hitCounter);
	node.ExtractAttribute(L"HitCounterIncrement", _hitCounterIncrement);
	node.ExtractAttribute(L"BreakOnCounter", _breakOnCounter);
	node.ExtractAttribute(L"BreakCounter", _breakCounter);
}

//----------------------------------------------------------------------------------------
void Breakpoint::SaveState(IHierarchicalStorageNode& node) const
{
	node.CreateAttribute(L"Name", _name);
	node.CreateAttribute(L"Enabled", _enabled);
	node.CreateAttribute(L"LogEvent", _logEvent);
	node.CreateAttribute(L"BreakEvent", _breakEvent);
	node.CreateAttribute(L"LocationConditionNot", _locationConditionNot);
	node.CreateAttribute(L"LocationCondition", (unsigned int)_locationCondition);
	node.CreateAttributeHex(L"LocationConditionData1", _locationConditionData1, 8);
	node.CreateAttributeHex(L"LocationConditionData2", _locationConditionData2, 8);
	node.CreateAttributeHex(L"LocationMask", _locationMask, 8);
	node.CreateAttribute(L"HitCounter", _hitCounter);
	node.CreateAttribute(L"HitCounterIncrement", _hitCounterIncrement);
	node.CreateAttribute(L"BreakOnCounter", _breakOnCounter);
	node.CreateAttribute(L"BreakCounter", _breakCounter);
}
