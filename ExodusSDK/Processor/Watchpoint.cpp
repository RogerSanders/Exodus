#include "Watchpoint.h"
#include <sstream>
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetIWatchpointVersion() const
{
	return ThisIWatchpointVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Watchpoint logging functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring Watchpoint::GetLogString() const
{
	std::wstringstream stream;
	stream << std::hex << std::uppercase << L"Watchpoint triggered at 0x" << GetLocationConditionData1();
	return stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
// Watchpoint event triggers
//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetEnabled() const
{
	return _enabled;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetEnabled(bool state)
{
	_enabled = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetLogEvent() const
{
	return _logEvent;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLogEvent(bool state)
{
	_logEvent = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetBreakEvent() const
{
	return _breakEvent;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetBreakEvent(bool state)
{
	_breakEvent = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Name functions
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> Watchpoint::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> Watchpoint::GenerateName() const
{
	std::wstring newName;
	switch (_locationCondition)
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

//----------------------------------------------------------------------------------------------------------------------
// Location condition functions
//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetLocationConditionNot() const
{
	return _locationConditionNot;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionNot(bool state)
{
	_locationConditionNot = state;
}

//----------------------------------------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetLocationCondition() const
{
	return _locationCondition;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLocationCondition(Condition condition)
{
	_locationCondition = condition;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationConditionData1() const
{
	return _locationConditionData1;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionData1(unsigned int data)
{
	_locationConditionData1 = data;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationConditionData2() const
{
	return _locationConditionData2;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionData2(unsigned int data)
{
	_locationConditionData2 = data;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationMask() const
{
	return _locationMask;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetLocationMask(unsigned int data)
{
	_locationMask = data & ((1 << _addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::PassesLocationCondition(unsigned int location)
{
	bool result = true;
	unsigned int locationMasked = (location & _locationMask);
	switch (GetLocationCondition())
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

//----------------------------------------------------------------------------------------------------------------------
// Hit counter functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetHitCounter() const
{
	return _hitCounter;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetHitCounter(unsigned int hitCounter)
{
	_hitCounter = hitCounter;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetBreakOnCounter() const
{
	return _breakOnCounter;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetBreakOnCounter(bool state)
{
	_breakOnCounter = state;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetBreakCounter() const
{
	return _breakCounter;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetBreakCounter(unsigned int breakCounter)
{
	_breakCounter = (breakCounter > 0)? breakCounter: 1;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::CheckHitCounter()
{
	IncrementHitCounter();
	if (GetBreakOnCounter())
	{
		if ((GetLiveHitCounter() % GetBreakCounter()) != 0)
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Read/write condition flag functions
//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetOnRead() const
{
	return _read;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetOnRead(bool state)
{
	_read = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetOnWrite() const
{
	return _write;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetOnWrite(bool state)
{
	_write = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Read condition functions
//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionEnabled() const
{
	return _readConditionEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionEnabled(bool state)
{
	_readConditionEnabled = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionNot() const
{
	return _readConditionNot;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionNot(bool state)
{
	_readConditionNot = state;
}

//----------------------------------------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetReadCondition() const
{
	return _readCondition;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetReadCondition(Condition condition)
{
	_readCondition = condition;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData1() const
{
	return _readConditionData1;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData1(unsigned int data)
{
	_readConditionData1 = data & ((1 << _dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData2() const
{
	return _readConditionData2;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData2(unsigned int data)
{
	_readConditionData2 = data & ((1 << _dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::PassesReadCondition(unsigned int data)
{
	bool result = true;
	if (GetReadConditionEnabled())
	{
		switch (GetReadCondition())
		{
		case Condition::Equal:
			result = (data == GetReadConditionData1());
			break;
		case Condition::Greater:
			result = (data > GetReadConditionData1());
			break;
		case Condition::Less:
			result = (data < GetReadConditionData1());
			break;
		case Condition::GreaterAndLess:
			result = (data > GetReadConditionData1()) && (data < GetReadConditionData2());
			break;
		}
		result ^= GetReadConditionNot();
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Write condition functions
//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetWriteConditionEnabled() const
{
	return _writeConditionEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionEnabled(bool state)
{
	_writeConditionEnabled = state;
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::GetWriteConditionNot() const
{
	return _writeConditionNot;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionNot(bool state)
{
	_writeConditionNot = state;
}

//----------------------------------------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetWriteCondition() const
{
	return _writeCondition;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetWriteCondition(Condition condition)
{
	_writeCondition = condition;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData1() const
{
	return _writeConditionData1;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData1(unsigned int data)
{
	_writeConditionData1 = data & ((1 << _dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData2() const
{
	return _writeConditionData2;
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData2(unsigned int data)
{
	_writeConditionData2 = data & ((1 << _dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------------------------------------
bool Watchpoint::PassesWriteCondition(unsigned int data)
{
	bool result = true;
	if (GetWriteConditionEnabled())
	{
		switch (GetWriteCondition())
		{
		case Condition::Equal:
			result = (data == GetWriteConditionData1());
			break;
		case Condition::Greater:
			result = (data > GetWriteConditionData1());
			break;
		case Condition::Less:
			result = (data < GetWriteConditionData1());
			break;
		case Condition::GreaterAndLess:
			result = (data > GetWriteConditionData1()) && (data < GetWriteConditionData2());
			break;
		}
		result ^= GetWriteConditionNot();
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Savestate functions
//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::LoadState(IHierarchicalStorageNode& node)
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

	node.ExtractAttribute(L"Read", _read);
	node.ExtractAttribute(L"Write", _write);
	node.ExtractAttribute(L"ReadConditionEnabled", _readConditionEnabled);
	node.ExtractAttribute(L"ReadConditionNot", _readConditionNot);
	unsigned int readConditionAsInt;
	node.ExtractAttribute(L"ReadCondition", readConditionAsInt);
	_readCondition = (Condition)readConditionAsInt;
	node.ExtractAttributeHex(L"ReadConditionData1", _readConditionData1);
	node.ExtractAttributeHex(L"ReadConditionData2", _readConditionData1);
	node.ExtractAttribute(L"WriteConditionEnabled", _writeConditionEnabled);
	node.ExtractAttribute(L"WriteConditionNot", _writeConditionNot);
	unsigned int writeConditionAsInt;
	node.ExtractAttribute(L"WriteCondition", writeConditionAsInt);
	_writeCondition = (Condition)writeConditionAsInt;
	node.ExtractAttributeHex(L"WriteConditionData1", _writeConditionData1);
	node.ExtractAttributeHex(L"WriteConditionData2", _writeConditionData1);
}

//----------------------------------------------------------------------------------------------------------------------
void Watchpoint::SaveState(IHierarchicalStorageNode& node) const
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

	node.CreateAttribute(L"Read", _read);
	node.CreateAttribute(L"Write", _write);
	node.CreateAttribute(L"ReadConditionEnabled", _readConditionEnabled);
	node.CreateAttribute(L"ReadConditionNot", _readConditionNot);
	node.CreateAttribute(L"ReadCondition", (unsigned int)_readCondition);
	node.CreateAttributeHex(L"ReadConditionData1", _readConditionData1, 8);
	node.CreateAttributeHex(L"ReadConditionData2", _readConditionData1, 8);
	node.CreateAttribute(L"WriteConditionEnabled", _writeConditionEnabled);
	node.CreateAttribute(L"WriteConditionNot", _writeConditionNot);
	node.CreateAttribute(L"WriteCondition", (unsigned int)_writeCondition);
	node.CreateAttributeHex(L"WriteConditionData1", _writeConditionData1, 8);
	node.CreateAttributeHex(L"WriteConditionData2", _writeConditionData1, 8);
}
