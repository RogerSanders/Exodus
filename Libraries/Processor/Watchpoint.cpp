#include "Watchpoint.h"
#include <sstream>
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetIWatchpointVersion() const
{
	return ThisIWatchpointVersion();
}

//----------------------------------------------------------------------------------------
//Watchpoint logging functions
//----------------------------------------------------------------------------------------
std::wstring Watchpoint::GetLogString() const
{
	std::wstringstream stream;
	stream << std::hex << std::uppercase << L"Watchpoint triggered at 0x" << GetLocationConditionData1();
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Watchpoint event triggers
//----------------------------------------------------------------------------------------
bool Watchpoint::GetEnabled() const
{
	return enabled;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetEnabled(bool state)
{
	enabled = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetLogEvent() const
{
	return logEvent;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLogEvent(bool state)
{
	logEvent = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetBreakEvent() const
{
	return breakEvent;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetBreakEvent(bool state)
{
	breakEvent = state;
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
void Watchpoint::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void Watchpoint::GenerateNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GenerateName());
}

//----------------------------------------------------------------------------------------
std::wstring Watchpoint::GenerateName() const
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
bool Watchpoint::GetLocationConditionNot() const
{
	return locationConditionNot;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionNot(bool state)
{
	locationConditionNot = state;
}

//----------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetLocationCondition() const
{
	return locationCondition;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLocationCondition(Condition condition)
{
	locationCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationConditionData1() const
{
	return locationConditionData1;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionData1(unsigned int data)
{
	locationConditionData1 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationConditionData2() const
{
	return locationConditionData2;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLocationConditionData2(unsigned int data)
{
	locationConditionData2 = data;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetLocationMask() const
{
	return locationMask;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetLocationMask(unsigned int data)
{
	locationMask = data & ((1 << addressBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
bool Watchpoint::PassesLocationCondition(unsigned int location)
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
unsigned int Watchpoint::GetHitCounter() const
{
	return hitCounter;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetHitCounter(unsigned int ahitCounter)
{
	hitCounter = ahitCounter;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetBreakOnCounter() const
{
	return breakOnCounter;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetBreakOnCounter(bool state)
{
	breakOnCounter = state;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetBreakCounter() const
{
	return breakCounter;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetBreakCounter(unsigned int abreakCounter)
{
	breakCounter = (abreakCounter > 0)? abreakCounter: 1;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::CheckHitCounter()
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
//Read/write condition flag functions
//----------------------------------------------------------------------------------------
bool Watchpoint::GetOnRead() const
{
	return read;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetOnRead(bool state)
{
	read = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetOnWrite() const
{
	return write;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetOnWrite(bool state)
{
	write = state;
}

//----------------------------------------------------------------------------------------
//Read condition functions
//----------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionEnabled() const
{
	return readConditionEnabled;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionEnabled(bool state)
{
	readConditionEnabled = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetReadConditionNot() const
{
	return readConditionNot;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionNot(bool state)
{
	readConditionNot = state;
}

//----------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetReadCondition() const
{
	return readCondition;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadCondition(Condition condition)
{
	readCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData1() const
{
	return readConditionData1;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData1(unsigned int data)
{
	readConditionData1 = data & ((1 << dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetReadConditionData2() const
{
	return readConditionData2;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetReadConditionData2(unsigned int data)
{
	readConditionData2 = data & ((1 << dataBusWidth) - 1);
}

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
bool Watchpoint::GetWriteConditionEnabled() const
{
	return writeConditionEnabled;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionEnabled(bool state)
{
	writeConditionEnabled = state;
}

//----------------------------------------------------------------------------------------
bool Watchpoint::GetWriteConditionNot() const
{
	return writeConditionNot;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionNot(bool state)
{
	writeConditionNot = state;
}

//----------------------------------------------------------------------------------------
Watchpoint::Condition Watchpoint::GetWriteCondition() const
{
	return writeCondition;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteCondition(Condition condition)
{
	writeCondition = condition;
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData1() const
{
	return writeConditionData1;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData1(unsigned int data)
{
	writeConditionData1 = data & ((1 << dataBusWidth) - 1);
}

//----------------------------------------------------------------------------------------
unsigned int Watchpoint::GetWriteConditionData2() const
{
	return writeConditionData2;
}

//----------------------------------------------------------------------------------------
void Watchpoint::SetWriteConditionData2(unsigned int data)
{
	writeConditionData2 = data & ((1 << dataBusWidth) - 1);
}

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
