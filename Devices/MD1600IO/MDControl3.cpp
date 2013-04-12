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

	lineAssertedD0 = false;
	lineAssertedD1 = false;
	lineAssertedD2 = false;
	lineAssertedD3 = false;
	lineAssertedTL = false;
	lineAssertedTR = false;
	lineAssertedTH = false;
}

//----------------------------------------------------------------------------------------
bool MDControl3::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDControl3::AddReference(const std::wstring& referenceName, IBusInterface* target)
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
bool MDControl3::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
	else
	{
		return false;
	}
	return true;
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
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"D0")
	{
		return LINE_D0;
	}
	else if(lineName == L"D1")
	{
		return LINE_D1;
	}
	else if(lineName == L"D2")
	{
		return LINE_D2;
	}
	else if(lineName == L"D3")
	{
		return LINE_D3;
	}
	else if(lineName == L"TL")
	{
		return LINE_TL;
	}
	else if(lineName == L"TR")
	{
		return LINE_TR;
	}
	else if(lineName == L"TH")
	{
		return LINE_TH;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring MDControl3::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_D0:
		return L"D0";
	case LINE_D1:
		return L"D1";
	case LINE_D2:
		return L"D2";
	case LINE_D3:
		return L"D3";
	case LINE_TL:
		return L"TL";
	case LINE_TR:
		return L"TR";
	case LINE_TH:
		return L"TH";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_D0:
	case LINE_D1:
	case LINE_D2:
	case LINE_D3:
	case LINE_TL:
	case LINE_TR:
	case LINE_TH:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDControl3::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if the
	//device has been accessed out of order.
	if(lastLineAccessTime > accessTime)
	{
		GetDeviceContext()->SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastLineAccessTime = accessTime;

	//If the TH line has been toggled, select the currently enabled bank.
	if(targetLine == LINE_TH)
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
	SetLineState(targetLine, lineData, 0, 0.0, 0);
}

//----------------------------------------------------------------------------------------
void MDControl3::AssertCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(lineAssertedD0) memoryBus->SetLineState(LINE_D0, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD1) memoryBus->SetLineState(LINE_D1, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD2) memoryBus->SetLineState(LINE_D2, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD3) memoryBus->SetLineState(LINE_D3, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTL) memoryBus->SetLineState(LINE_TL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTR) memoryBus->SetLineState(LINE_TR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTH) memoryBus->SetLineState(LINE_TH, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl3::NegateCurrentOutputLineState() const
{
	if(memoryBus != 0)
	{
		if(lineAssertedD0) memoryBus->SetLineState(LINE_D0, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD1) memoryBus->SetLineState(LINE_D1, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD2) memoryBus->SetLineState(LINE_D2, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedD3) memoryBus->SetLineState(LINE_D3, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTL) memoryBus->SetLineState(LINE_TL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTR) memoryBus->SetLineState(LINE_TR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if(lineAssertedTH) memoryBus->SetLineState(LINE_TH, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl3::UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	if(!lineInputStateTH)
	{
		//This state is selected when TH is configured as an input and set to 0
		//D0 = Up
		//D1 = Down
		//D2 = Null (grounded)
		//D3 = Null (grounded)
		//TL = A
		//TR = Start
		//TH = Null (+5v)
		if(lineAssertedD0 != !buttonPressed[BUTTON_UP])
		{
			lineAssertedD0 = !buttonPressed[BUTTON_UP];
			memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD1 != !buttonPressed[BUTTON_DOWN])
		{
			lineAssertedD1 = !buttonPressed[BUTTON_DOWN];
			memoryBus->SetLineState(LINE_D1, Data(1, (unsigned int)lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD2 != false)
		{
			lineAssertedD2 = false;
			memoryBus->SetLineState(LINE_D2, Data(1, (unsigned int)lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD3 != false)
		{
			lineAssertedD3 = false;
			memoryBus->SetLineState(LINE_D3, Data(1, (unsigned int)lineAssertedD3), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTL != !buttonPressed[BUTTON_A])
		{
			lineAssertedTL = !buttonPressed[BUTTON_A];
			memoryBus->SetLineState(LINE_TL, Data(1, (unsigned int)lineAssertedTL), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTR != !buttonPressed[BUTTON_START])
		{
			lineAssertedTR = !buttonPressed[BUTTON_START];
			memoryBus->SetLineState(LINE_TR, Data(1, (unsigned int)lineAssertedTR), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTH != true)
		{
			lineAssertedTH = true;
			memoryBus->SetLineState(LINE_TH, Data(1, (unsigned int)lineAssertedTH), GetDeviceContext(), caller, accessTime, accessContext);
		}
	}
	else
	{
		//This state is selected when TH is configured as an input and set to 1
		//D0 = Up
		//D1 = Down
		//D2 = Left
		//D3 = Right
		//TL = B
		//TR = C
		//TH = Null (+5v)
		if(lineAssertedD0 != !buttonPressed[BUTTON_UP])
		{
			lineAssertedD0 = !buttonPressed[BUTTON_UP];
			memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD1 != !buttonPressed[BUTTON_DOWN])
		{
			lineAssertedD1 = !buttonPressed[BUTTON_DOWN];
			memoryBus->SetLineState(LINE_D1, Data(1, (unsigned int)lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD2 != !buttonPressed[BUTTON_LEFT])
		{
			lineAssertedD2 = !buttonPressed[BUTTON_LEFT];
			memoryBus->SetLineState(LINE_D2, Data(1, (unsigned int)lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedD3 != !buttonPressed[BUTTON_RIGHT])
		{
			lineAssertedD3 = !buttonPressed[BUTTON_RIGHT];
			memoryBus->SetLineState(LINE_D3, Data(1, (unsigned int)lineAssertedD3), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTL != !buttonPressed[BUTTON_B])
		{
			lineAssertedTL = !buttonPressed[BUTTON_B];
			memoryBus->SetLineState(LINE_TL, Data(1, (unsigned int)lineAssertedTL), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTR != !buttonPressed[BUTTON_C])
		{
			lineAssertedTR = !buttonPressed[BUTTON_C];
			memoryBus->SetLineState(LINE_TR, Data(1, (unsigned int)lineAssertedTR), GetDeviceContext(), caller, accessTime, accessContext);
		}
		if(lineAssertedTH != true)
		{
			lineAssertedTH = true;
			memoryBus->SetLineState(LINE_TH, Data(1, (unsigned int)lineAssertedTH), GetDeviceContext(), caller, accessTime, accessContext);
		}
	}
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int MDControl3::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	if(keyCodeName == L"Up")
	{
		return BUTTON_UP+1;
	}
	else if(keyCodeName == L"Down")
	{
		return BUTTON_DOWN+1;
	}
	else if(keyCodeName == L"Left")
	{
		return BUTTON_LEFT+1;
	}
	else if(keyCodeName == L"Right")
	{
		return BUTTON_RIGHT+1;
	}
	else if(keyCodeName == L"A")
	{
		return BUTTON_A+1;
	}
	else if(keyCodeName == L"B")
	{
		return BUTTON_B+1;
	}
	else if(keyCodeName == L"C")
	{
		return BUTTON_C+1;
	}
	else if(keyCodeName == L"Start")
	{
		return BUTTON_START+1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring MDControl3::GetKeyCodeName(unsigned int keyCodeID) const
{
	switch(keyCodeID)
	{
	case BUTTON_UP+1:
		return L"Up";
	case BUTTON_DOWN+1:
		return L"Down";
	case BUTTON_LEFT+1:
		return L"Left";
	case BUTTON_RIGHT+1:
		return L"Right";
	case BUTTON_A+1:
		return L"A";
	case BUTTON_B+1:
		return L"B";
	case BUTTON_C+1:
		return L"C";
	case BUTTON_START+1:
		return L"Start";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyDown(unsigned int keyCodeID)
{
	Button keyCode = (Button)(keyCodeID-1);
	buttonPressed[keyCode] = true;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyUp(unsigned int keyCodeID)
{
	Button keyCode = (Button)(keyCodeID-1);
	buttonPressed[keyCode] = false;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}
