#include "MDControl3.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MDControl3::MDControl3(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID)
{
	memoryBus = 0;
	buttonPressed.resize(buttonCount);
	bbuttonPressed.resize(buttonCount);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void MDControl3::Initialize()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		buttonPressed[i] = false;
	}
	lineInputStateTH = false;
	lineAssertedD0 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D0);
	lineAssertedD1 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D1);
	lineAssertedD2 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D2);
	lineAssertedD3 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D3);
	lineAssertedTL = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TL);
	lineAssertedTR = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TR);
	lineAssertedTH = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TH);
	lastLineAccessTime = 0;
	currentTimesliceLength = 0;
}

//----------------------------------------------------------------------------------------
bool MDControl3::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDControl3::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	if(referenceName == L"BusInterface")
	{
		memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void MDControl3::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
bool MDControl3::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void MDControl3::NotifyUpcomingTimeslice(double nanoseconds)
{
	currentTimesliceLength = nanoseconds;
	lastLineAccessTime = 0;
}

//----------------------------------------------------------------------------------------
void MDControl3::ExecuteRollback()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		buttonPressed[i] = bbuttonPressed[i];
	}
	lineInputStateTH = blineInputStateTH;
	lineAssertedD0 = blineAssertedD0;
	lineAssertedD1 = blineAssertedD1;
	lineAssertedD2 = blineAssertedD2;
	lineAssertedD3 = blineAssertedD3;
	lineAssertedTL = blineAssertedTL;
	lineAssertedTR = blineAssertedTR;
	lineAssertedTH = blineAssertedTH;
	currentTimesliceLength = bcurrentTimesliceLength;
}

//----------------------------------------------------------------------------------------
void MDControl3::ExecuteCommit()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		bbuttonPressed[i] = buttonPressed[i];
	}
	blineInputStateTH = lineInputStateTH;
	blineAssertedD0 = lineAssertedD0;
	blineAssertedD1 = lineAssertedD1;
	blineAssertedD2 = lineAssertedD2;
	blineAssertedD3 = lineAssertedD3;
	blineAssertedTL = lineAssertedTL;
	blineAssertedTR = lineAssertedTR;
	blineAssertedTH = lineAssertedTH;
	bcurrentTimesliceLength = currentTimesliceLength;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const
{
	if(lineName == L"D0")
	{
		return (unsigned int)LineID::D0;
	}
	else if(lineName == L"D1")
	{
		return (unsigned int)LineID::D1;
	}
	else if(lineName == L"D2")
	{
		return (unsigned int)LineID::D2;
	}
	else if(lineName == L"D3")
	{
		return (unsigned int)LineID::D3;
	}
	else if(lineName == L"TL")
	{
		return (unsigned int)LineID::TL;
	}
	else if(lineName == L"TR")
	{
		return (unsigned int)LineID::TR;
	}
	else if(lineName == L"TH")
	{
		return (unsigned int)LineID::TH;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> MDControl3::GetLineName(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::D0:
		return L"D0";
	case LineID::D1:
		return L"D1";
	case LineID::D2:
		return L"D2";
	case LineID::D3:
		return L"D3";
	case LineID::TL:
		return L"TL";
	case LineID::TR:
		return L"TR";
	case LineID::TH:
		return L"TH";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineWidth(unsigned int lineID) const
{
	switch((LineID)lineID)
	{
	case LineID::D0:
	case LineID::D1:
	case LineID::D2:
	case LineID::D3:
	case LineID::TL:
	case LineID::TR:
	case LineID::TH:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDControl3::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if the
	//device has been accessed out of order.
	if(lastLineAccessTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastLineAccessTime = accessTime;

	//If the TH line has been toggled, select the currently enabled bank.
	if(targetLine == LineID::TH)
	{
		lineInputStateTH = lineData.GetBit(0);
	}

	//We explicitly release our lock on lineMutex here so that we're not blocking access
	//to SetLineState() on this class before we modify the line state for other devices in
	//the code that follows. Adhering to this pattern helps to avoid deadlock cases that
	//could otherwise arise from valid line mappings.
	lock.unlock();

	//If an input line state has changed, re-evaluate the state of the output lines.
	UpdateLineState(caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void MDControl3::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, currentTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void MDControl3::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(lineAssertedD0) memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD1) memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD2) memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD3) memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTL) memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTR) memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTH) memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl3::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(lineAssertedD0) memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD1) memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD2) memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD3) memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTL) memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTR) memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTH) memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl3::UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	bool newLineStateD0 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D0);
	if(lineAssertedD0 != newLineStateD0)
	{
		lineAssertedD0 = newLineStateD0;
		memoryBus->SetLineState(LineID::D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD1 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D1);
	if(lineAssertedD1 != newLineStateD1)
	{
		lineAssertedD1 = newLineStateD1;
		memoryBus->SetLineState(LineID::D1, Data(1, (unsigned int)lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD2 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D2);
	if(lineAssertedD2 != newLineStateD2)
	{
		lineAssertedD2 = newLineStateD2;
		memoryBus->SetLineState(LineID::D2, Data(1, (unsigned int)lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD3 = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::D3);
	if(lineAssertedD3 != newLineStateD3)
	{
		lineAssertedD3 = newLineStateD3;
		memoryBus->SetLineState(LineID::D3, Data(1, (unsigned int)lineAssertedD3), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTL = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TL);
	if(lineAssertedTL != newLineStateTL)
	{
		lineAssertedTL = newLineStateTL;
		memoryBus->SetLineState(LineID::TL, Data(1, (unsigned int)lineAssertedTL), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTR = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TR);
	if(lineAssertedTR != newLineStateTR)
	{
		lineAssertedTR = newLineStateTR;
		memoryBus->SetLineState(LineID::TR, Data(1, (unsigned int)lineAssertedTR), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTH = GetDesiredLineState(lineInputStateTH, buttonPressed, LineID::TH);
	if(lineAssertedTH != newLineStateTH)
	{
		lineAssertedTH = newLineStateTH;
		memoryBus->SetLineState(LineID::TH, Data(1, (unsigned int)lineAssertedTH), GetDeviceContext(), caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
bool MDControl3::GetDesiredLineState(unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, LineID lineID)
{
	if(currentLineInputStateTH)
	{
		//This state is selected when TH is configured as an input and set to 1
		//D0 = Up
		//D1 = Down
		//D2 = Left
		//D3 = Right
		//TL = B
		//TR = C
		//TH = Null (+5v)
		switch(lineID)
		{
		case LineID::D0:
			return !currentButtonPressedState[BUTTONINDEX_UP];
		case LineID::D1:
			return !currentButtonPressedState[BUTTONINDEX_DOWN];
		case LineID::D2:
			return !currentButtonPressedState[BUTTONINDEX_LEFT];
		case LineID::D3:
			return !currentButtonPressedState[BUTTONINDEX_RIGHT];
		case LineID::TL:
			return !currentButtonPressedState[BUTTONINDEX_B];
		case LineID::TR:
			return !currentButtonPressedState[BUTTONINDEX_C];
		case LineID::TH:
			return true;
		}
	}
	else
	{
		//This state is selected when TH is configured as an input and set to 0
		//D0 = Up
		//D1 = Down
		//D2 = Null (grounded)
		//D3 = Null (grounded)
		//TL = A
		//TR = Start
		//TH = Null (+5v)
		switch(lineID)
		{
		case LineID::D0:
			return !currentButtonPressedState[BUTTONINDEX_UP];
		case LineID::D1:
			return !currentButtonPressedState[BUTTONINDEX_DOWN];
		case LineID::D2:
			return false;
		case LineID::D3:
			return false;
		case LineID::TL:
			return !currentButtonPressedState[BUTTONINDEX_A];
		case LineID::TR:
			return !currentButtonPressedState[BUTTONINDEX_START];
		case LineID::TH:
			return true;
		}
	}

	//##TODO## Raise an assert if we end up here with an invalid setting for the target
	//line.
	return false;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const
{
	if(keyCodeName == L"Up")
	{
		return BUTTONINDEX_UP+1;
	}
	else if(keyCodeName == L"Down")
	{
		return BUTTONINDEX_DOWN+1;
	}
	else if(keyCodeName == L"Left")
	{
		return BUTTONINDEX_LEFT+1;
	}
	else if(keyCodeName == L"Right")
	{
		return BUTTONINDEX_RIGHT+1;
	}
	else if(keyCodeName == L"A")
	{
		return BUTTONINDEX_A+1;
	}
	else if(keyCodeName == L"B")
	{
		return BUTTONINDEX_B+1;
	}
	else if(keyCodeName == L"C")
	{
		return BUTTONINDEX_C+1;
	}
	else if(keyCodeName == L"Start")
	{
		return BUTTONINDEX_START+1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> MDControl3::GetKeyCodeName(unsigned int keyCodeID) const
{
	switch(keyCodeID)
	{
	case BUTTONINDEX_UP+1:
		return L"Up";
	case BUTTONINDEX_DOWN+1:
		return L"Down";
	case BUTTONINDEX_LEFT+1:
		return L"Left";
	case BUTTONINDEX_RIGHT+1:
		return L"Right";
	case BUTTONINDEX_A+1:
		return L"A";
	case BUTTONINDEX_B+1:
		return L"B";
	case BUTTONINDEX_C+1:
		return L"C";
	case BUTTONINDEX_START+1:
		return L"Start";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyDown(unsigned int keyCodeID)
{
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
	buttonPressed[keyCode] = true;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyUp(unsigned int keyCodeID)
{
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
	buttonPressed[keyCode] = false;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void MDControl3::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageNode& node = *(*i);
		std::wstring nodeName = node.GetName();
		if(nodeName == L"LineInputStateTH")
		{
			node.ExtractData(lineInputStateTH);
		}
		else if(nodeName == L"LineAssertedD0")
		{
			node.ExtractData(lineAssertedD0);
		}
		else if(nodeName == L"LineAssertedD1")
		{
			node.ExtractData(lineAssertedD1);
		}
		else if(nodeName == L"LineAssertedD2")
		{
			node.ExtractData(lineAssertedD2);
		}
		else if(nodeName == L"LineAssertedD3")
		{
			node.ExtractData(lineAssertedD3);
		}
		else if(nodeName == L"LineAssertedTL")
		{
			node.ExtractData(lineAssertedTL);
		}
		else if(nodeName == L"LineAssertedTR")
		{
			node.ExtractData(lineAssertedTR);
		}
		else if(nodeName == L"LineAssertedTH")
		{
			node.ExtractData(lineAssertedTH);
		}
		else if(nodeName == L"ButtonPressed")
		{
			IHierarchicalStorageAttribute* buttonNumberAttribute = node.GetAttribute(L"ButtonNo");
			if(buttonNumberAttribute != 0)
			{
				unsigned int buttonNo = buttonNumberAttribute->ExtractValue<unsigned int>();
				if(buttonNo < buttonCount)
				{
					bool state;
					node.ExtractAttribute(L"Pressed", state);
					buttonPressed[buttonNo] = state;
				}
			}
		}
		else if(nodeName == L"CurrentTimesliceLength")
		{
			node.ExtractData(currentTimesliceLength);
		}
	}
}

//----------------------------------------------------------------------------------------
void MDControl3::SaveState(IHierarchicalStorageNode& node) const
{
	node.CreateChild(L"LineInputStateTH", lineInputStateTH);
	node.CreateChild(L"LineAssertedD0", lineAssertedD0);
	node.CreateChild(L"LineAssertedD1", lineAssertedD1);
	node.CreateChild(L"LineAssertedD2", lineAssertedD2);
	node.CreateChild(L"LineAssertedD3", lineAssertedD3);
	node.CreateChild(L"LineAssertedTL", lineAssertedTL);
	node.CreateChild(L"LineAssertedTR", lineAssertedTR);
	node.CreateChild(L"LineAssertedTH", lineAssertedTH);
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		node.CreateChild(L"ButtonPressed").CreateAttribute(L"ButtonNo", i).CreateAttribute(L"Pressed", buttonPressed[i]);
	}
	node.CreateChild(L"CurrentTimesliceLength", currentTimesliceLength);
}
