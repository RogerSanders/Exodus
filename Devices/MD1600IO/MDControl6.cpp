#include "MDControl6.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MDControl6::MDControl6(const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(L"MDControl6", ainstanceName, amoduleID)
{
	//##TODO## Perform our own hardware tests to confirm this value
	bankswitchTimeoutInterval = 1500000.0;

	memoryBus = 0;
	buttonPressed.resize(buttonCount);
	bbuttonPressed.resize(buttonCount);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool MDControl6::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDControl6::AddReference(const wchar_t* referenceName, IBusInterface* target)
{
	std::wstring referenceNameString = referenceName;
	if(referenceNameString == L"BusInterface")
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
bool MDControl6::RemoveReference(IBusInterface* target)
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
bool MDControl6::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void MDControl6::NotifyUpcomingTimeslice(double nanoseconds)
{
	bankswitchCounterToggleLastRisingEdge -= currentTimesliceLength;
	currentTimesliceLength = nanoseconds;
	lastLineAccessTime = 0;
}

//----------------------------------------------------------------------------------------
void MDControl6::ExecuteRollback()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		buttonPressed[i] = bbuttonPressed[i];
	}
	lineInputStateTH = blineInputStateTH;
	bankswitchingDisabled = bbankswitchingDisabled;
	bankswitchCounter = bbankswitchCounter;
	bankswitchCounterToggleLastRisingEdge = bbankswitchCounterToggleLastRisingEdge;
	lineAssertedD0 = blineAssertedD0;
	lineAssertedD1 = blineAssertedD1;
	lineAssertedD2 = blineAssertedD2;
	lineAssertedD3 = blineAssertedD3;
	lineAssertedTL = blineAssertedTL;
	lineAssertedTR = blineAssertedTR;
	lineAssertedTH = blineAssertedTH;
}

//----------------------------------------------------------------------------------------
void MDControl6::ExecuteCommit()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		bbuttonPressed[i] = buttonPressed[i];
	}
	blineInputStateTH = lineInputStateTH;
	bbankswitchingDisabled = bankswitchingDisabled;
	bbankswitchCounter = bankswitchCounter;
	bbankswitchCounterToggleLastRisingEdge = bankswitchCounterToggleLastRisingEdge;
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
unsigned int MDControl6::GetLineID(const wchar_t* lineName) const
{
	std::wstring lineNameString = lineName;
	if(lineNameString == L"D0")
	{
		return LINE_D0;
	}
	else if(lineNameString == L"D1")
	{
		return LINE_D1;
	}
	else if(lineNameString == L"D2")
	{
		return LINE_D2;
	}
	else if(lineNameString == L"D3")
	{
		return LINE_D3;
	}
	else if(lineNameString == L"TL")
	{
		return LINE_TL;
	}
	else if(lineNameString == L"TR")
	{
		return LINE_TR;
	}
	else if(lineNameString == L"TH")
	{
		return LINE_TH;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
const wchar_t* MDControl6::GetLineName(unsigned int lineID) const
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
unsigned int MDControl6::GetLineWidth(unsigned int lineID) const
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
void MDControl6::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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
		bool newLineState = lineData.GetBit(0);
		if(lineInputStateTH != newLineState)
		{
			//Record the new TH line state
			lineInputStateTH = newLineState;

			//A 6-button controller has additional banks of buttons that can be queried
			//after repeated changes to the TH input line state, if these changes occur
			//within a certain timeout period. If the TH line just transitioned from 0 to
			//1, we need to recalculate the selected bank, and extend the timeout period
			//for our bankswitch changes. According to information provided at
			//http://segaretro.org/Six_Button_Control_Pad_(Mega_Drive), this timeout
			//period is 1.5ms.
			//##TODO## Confirm the controller timeout period through hardware tests
			if(lineInputStateTH)
			{
				//Increment the bankswitch counter
				++bankswitchCounter;

				//If the timeout period elapsed for the bankswitch counter state to be
				//maintained, or if the bankswitch counter has passed the reset point, or
				//if bankswitching has been disabled, reset the bankswitch counter back to
				//zero.
				double elapsedTimeSinceLastTHLineStateChange = accessTime - bankswitchCounterToggleLastRisingEdge;
				if((bankswitchCounter >= bankswitchCounterResetPoint)
				|| bankswitchingDisabled
				|| (elapsedTimeSinceLastTHLineStateChange >= bankswitchTimeoutInterval))
				{
					bankswitchCounter = 0;
				}

				//Record this TH rising edge transition time, to calculate the timeout for
				//the next access attempt.
				bankswitchCounterToggleLastRisingEdge = accessTime;
			}
		}
	}

	//If an input line state has changed, re-evaluate the state of the output lines.
	UpdateLineState(caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void MDControl6::Initialize()
{
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		buttonPressed[i] = false;
	}
	currentTimesliceLength = 0;
	lineInputStateTH = true;
	bankswitchingDisabled = false;
	bankswitchCounter = 0;
	bankswitchCounterToggleLastRisingEdge = 0;

	lineAssertedD0 = false;
	lineAssertedD1 = false;
	lineAssertedD2 = false;
	lineAssertedD3 = false;
	lineAssertedTL = false;
	lineAssertedTR = false;
	lineAssertedTH = false;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int MDControl6::GetKeyCodeID(const wchar_t* keyCodeName) const
{
	std::wstring keyCodeNameString = keyCodeName;
	if(keyCodeNameString == L"Up")
	{
		return BUTTON_UP;
	}
	else if(keyCodeNameString == L"Down")
	{
		return BUTTON_DOWN;
	}
	else if(keyCodeNameString == L"Left")
	{
		return BUTTON_LEFT;
	}
	else if(keyCodeNameString == L"Right")
	{
		return BUTTON_RIGHT;
	}
	else if(keyCodeNameString == L"A")
	{
		return BUTTON_A;
	}
	else if(keyCodeNameString == L"B")
	{
		return BUTTON_B;
	}
	else if(keyCodeNameString == L"C")
	{
		return BUTTON_C;
	}
	else if(keyCodeNameString == L"Start")
	{
		return BUTTON_START;
	}
	else if(keyCodeNameString == L"X")
	{
		return BUTTON_X;
	}
	else if(keyCodeNameString == L"Y")
	{
		return BUTTON_Y;
	}
	else if(keyCodeNameString == L"Z")
	{
		return BUTTON_Z;
	}
	else if(keyCodeNameString == L"Mode")
	{
		return BUTTON_MODE;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MDControl6::HandleInputKeyDown(unsigned int keyCode)
{
	buttonPressed[keyCode] = true;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::HandleInputKeyUp(unsigned int keyCode)
{
	buttonPressed[keyCode] = false;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	switch(bankswitchCounter)
	{
	default:
		//##TODO## Raise an assert if we end up here with an invalid setting for the
		//bankswitch counter
		break;
	case 0:
	case 1:
		if(lineInputStateTH)
		{
			//This state is selected when TH is configured as an input and set to 0
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
		else
		{
			//This state is selected when TH is configured as an input and set to 1
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
		break;
	case 2:
		if(lineInputStateTH)
		{
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
		else
		{
			//D0 = Null (grounded)
			//D1 = Null (grounded)
			//D2 = Null (grounded)
			//D3 = Null (grounded)
			//TL = A
			//TR = Start
			//TH = Null (+5v)
			if(lineAssertedD0 != false)
			{
				lineAssertedD0 = false;
				memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD1 != false)
			{
				lineAssertedD1 = false;
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
		break;
	case 3:
		if(lineInputStateTH)
		{
			//D0 = Z
			//D1 = Y
			//D2 = X
			//D3 = Mode
			//TL = B
			//TR = C
			//TH = Null (+5v)
			if(lineAssertedD0 != !buttonPressed[BUTTON_Z])
			{
				lineAssertedD0 = !buttonPressed[BUTTON_Z];
				memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD1 != !buttonPressed[BUTTON_Y])
			{
				lineAssertedD1 = !buttonPressed[BUTTON_Y];
				memoryBus->SetLineState(LINE_D1, Data(1, (unsigned int)lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD2 != !buttonPressed[BUTTON_X])
			{
				lineAssertedD2 = !buttonPressed[BUTTON_X];
				memoryBus->SetLineState(LINE_D2, Data(1, (unsigned int)lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD3 != !buttonPressed[BUTTON_MODE])
			{
				lineAssertedD3 = !buttonPressed[BUTTON_MODE];
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
		else
		{
			//D0 = Null (+5v)
			//D1 = Null (+5v)
			//D2 = Null (+5v)
			//D3 = Null (+5v)
			//TL = A
			//TR = Start
			//TH = Null (+5v)
			if(lineAssertedD0 != true)
			{
				lineAssertedD0 = true;
				memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD1 != true)
			{
				lineAssertedD1 = true;
				memoryBus->SetLineState(LINE_D1, Data(1, (unsigned int)lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD2 != true)
			{
				lineAssertedD2 = true;
				memoryBus->SetLineState(LINE_D2, Data(1, (unsigned int)lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
			}
			if(lineAssertedD3 != true)
			{
				lineAssertedD3 = true;
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
		break;
	}
}
