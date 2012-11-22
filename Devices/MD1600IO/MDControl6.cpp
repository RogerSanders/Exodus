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
	outputLineState.resize(outputLineCount);
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
	for(unsigned int i = 0; i < outputLineCount; ++i)
	{
		outputLineState[i].asserted = false;
		outputLineState[i].timeoutFlagged = false;
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::InitializeExternalConnections()
{
	memoryBus->SetLineState(LINE_D0, Data(1, (unsigned int)outputLineState[0].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_D1, Data(1, (unsigned int)outputLineState[1].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_D2, Data(1, (unsigned int)outputLineState[2].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_D3, Data(1, (unsigned int)outputLineState[3].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_TL, Data(1, (unsigned int)outputLineState[4].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_TR, Data(1, (unsigned int)outputLineState[5].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	memoryBus->SetLineState(LINE_TH, Data(1, (unsigned int)outputLineState[6].asserted), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

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
	//Go through each pending line state timeout and adjust the time of the access here,
	//to rebase it for the start of the new timeslice. We need to do this so that we can
	//revoke it later if required.
	for(unsigned int i = 0; i < outputLineCount; ++i)
	{
		if(outputLineState[i].timeoutFlagged)
		{
			outputLineState[i].timeoutTime -= currentTimesliceLength;
		}
	}

	currentTimesliceLength = nanoseconds;
	lastLineAccessTime = 0;
}

//----------------------------------------------------------------------------------------
bool MDControl6::SendNotifyAfterExecuteCalled() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void MDControl6::NotifyAfterExecuteCalled()
{
	bankswitchCounterToggleLastRisingEdge -= currentTimesliceLength;
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
	outputLineState = boutputLineState;
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
	boutputLineState = outputLineState;
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

	//If the timeout period elapsed for the bankswitch counter state to be maintained,
	//correct our internal state to reflect the changes after the timeout. Note that the
	//output line state would have been reverted already by this point, we're just
	//correcting the internal digital state to what it would have already been in the real
	//hardware after the timeout period elapsed.
	double elapsedTimeSinceLastTHLineStateChange = accessTime - bankswitchCounterToggleLastRisingEdge;
	if(elapsedTimeSinceLastTHLineStateChange >= bankswitchTimeoutInterval)
	{
		//Reset the bankswitch counter back to zero
		bankswitchCounter = 0;

		//Since we've passed the timeout times for all our lines now, clear the flags
		//indicating there is a timeout currently flagged for our output lines.
		for(unsigned int i = 0; i < outputLineCount; ++i)
		{
			outputLineState[i].timeoutFlagged = false;
		}
	}

	//If the TH line has been toggled, select the currently enabled bank.
	//##FIX## According to documentation, in the 6-button controller, there is some
	//latency from when the TH line is toggled, and when the output line state actually
	//changes. This latency period is reported to be "approximately two NOP instructions"
	//in length, or in other words, two NOP instructions are required after setting the HL
	//line state, and before reading the control port data, in order to ensure the correct
	//data is read. We need to emulate that here, with an option to disable this
	//behaviour.
	bool timeoutSettingsChanged = false;
	if(targetLine == LINE_TH)
	{
		bool newLineState = lineData.GetBit(0);
		if(lineInputStateTH != newLineState)
		{
			//Flag that the bankswitch timeout settings have changed now that a change has
			//been registered to the input HL line state.
			timeoutSettingsChanged = true;

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
				if(bankswitchingDisabled)
				{
					bankswitchCounter = 0;
				}
				else
				{
					bankswitchCounter = (bankswitchCounter + 1) % bankswitchCounterResetPoint;
				}

				//Record this TH rising edge transition time, to calculate the timeout for
				//the next access attempt.
				bankswitchCounterToggleLastRisingEdge = accessTime;
			}
		}
	}

	//We explicitly release our lock on lineMutex here so that we're not blocking access
	//to SetLineState() on this class before we modify the line state for other devices in
	//the code that follows. Adhering to this pattern helps to avoid deadlock cases that
	//could otherwise arise from valid line mappings.
	lock.unlock();

	//If an input line state has changed, re-evaluate the state of the output lines.
	UpdateLineState(timeoutSettingsChanged, caller, accessTime, accessContext);
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
	//##TODO## We have seen on the real hardware that a button press is not a "clean"
	//signal. In fact, the pressed state of a button often toggles many times while the
	//user is in the process of pressing the button, even if this button press happens
	//quickly. We should approximate this behaviour here, by firing a set pattern of
	//button press and release events for both a key down and a key up event. This will
	//allow input routines to be correctly tested on our emulator, which will also be
	//robust on the hardware.
	//##TODO## The real six button controller has a D-pad, where left and right, as well
	//as up and down, are mutually exclusive. Input routines on games are often coded
	//assuming this will be the case. On a keyboard though, there's nothing stopping the
	//user from pressing these normally exclusive input buttons simultaneously, possibly
	//causing games to exhibit errors. We should have an option, which is enabled by
	//default but possible to disable, that will ensure that if a mutually exclusive set
	//of key press events are received, the most recent key press will cause the set state
	//of its exclusive keys to be negated.
	buttonPressed[keyCode] = true;
	UpdateLineState(false, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::HandleInputKeyUp(unsigned int keyCode)
{
	buttonPressed[keyCode] = false;
	UpdateLineState(false, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::UpdateLineState(bool timeoutSettingsChanged, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Update the line state for each output line
	UpdateOutputLineStateForLine(LINE_D0, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_D1, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_D2, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_D3, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_TL, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_TR, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LINE_TH, timeoutSettingsChanged, caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void MDControl6::UpdateOutputLineStateForLine(unsigned int lineID, bool revokeAllTimeoutStateChanges, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Calculate the index of this line in the output line state array
	unsigned int lineIndex = lineID - LINE_D0;

	//Update the target line output state
	bool lineAssertedNew = GetDesiredLineState(bankswitchCounter, lineInputStateTH, buttonPressed, lineID);
	if(outputLineState[lineIndex].asserted != lineAssertedNew)
	{
		outputLineState[lineIndex].asserted = lineAssertedNew;
		memoryBus->SetLineState(lineID, Data(1, (unsigned int)lineAssertedNew), GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Revoke an existing timeout line state change if the timeout setting has changed
	if(revokeAllTimeoutStateChanges && outputLineState[lineIndex].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(lineID, Data(1, outputLineState[lineIndex].timeoutAssertedState), outputLineState[lineIndex].timeoutTime, GetDeviceContext(), caller, accessTime, accessContext);
		outputLineState[lineIndex].timeoutFlagged = false;
	}

	//Raise a target line output state change on bankswitch timeout if required, if an
	//existing line state change on bankswitch timeout has been set, but it is no longer
	//required, remove it.
	bool lineAssertedAfterTimeoutNew = GetDesiredLineState(0, lineInputStateTH, buttonPressed, lineID);
	if(lineAssertedNew != lineAssertedAfterTimeoutNew)
	{
		outputLineState[lineIndex].timeoutFlagged = true;
		outputLineState[lineIndex].timeoutAssertedState = lineAssertedAfterTimeoutNew;
		//##FIX## What if the TH line just changed from 1 to 0, which reportedly doesn't
		//update the bankswitch timeout? In this case, the timeout time would be relative
		//to the last 0 to 1 TH transition.
		//##TODO## Test this in hardware. It seems logical to me that the last time the TH
		//line is changed will be the time at which the timeout is relative to.
		outputLineState[lineIndex].timeoutTime = accessTime+bankswitchTimeoutInterval;
		memoryBus->SetLineState(lineID, Data(1, (unsigned int)outputLineState[lineIndex].timeoutAssertedState), GetDeviceContext(), caller, outputLineState[lineIndex].timeoutTime, accessContext);
	}
	else if(outputLineState[lineIndex].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(lineID, Data(1, outputLineState[lineIndex].timeoutAssertedState), outputLineState[lineIndex].timeoutTime, GetDeviceContext(), caller, accessTime, accessContext);
		outputLineState[lineIndex].timeoutFlagged = false;
	}
}

//----------------------------------------------------------------------------------------
bool MDControl6::GetDesiredLineState(unsigned int currentBankswitchCounter, unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, unsigned int lineID)
{
	switch(currentBankswitchCounter)
	{
	case 0:
	case 1:
		if(currentLineInputStateTH)
		{
			//This state is selected when TH is configured as an input and set to 0
			//D0 = Up
			//D1 = Down
			//D2 = Left
			//D3 = Right
			//TL = B
			//TR = C
			//TH = Null (+5v)
			switch(lineID)
			{
			case LINE_D0:
				return !currentButtonPressedState[BUTTON_UP];
			case LINE_D1:
				return !currentButtonPressedState[BUTTON_DOWN];
			case LINE_D2:
				return !currentButtonPressedState[BUTTON_LEFT];
			case LINE_D3:
				return !currentButtonPressedState[BUTTON_RIGHT];
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_B];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_C];
			case LINE_TH:
				return true;
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
			switch(lineID)
			{
			case LINE_D0:
				return !currentButtonPressedState[BUTTON_UP];
			case LINE_D1:
				return !currentButtonPressedState[BUTTON_DOWN];
			case LINE_D2:
				return false;
			case LINE_D3:
				return false;
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_A];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_START];
			case LINE_TH:
				return true;
			}
		}
		break;
	case 2:
		if(currentLineInputStateTH)
		{
			//D0 = Up
			//D1 = Down
			//D2 = Left
			//D3 = Right
			//TL = B
			//TR = C
			//TH = Null (+5v)
			switch(lineID)
			{
			case LINE_D0:
				return !currentButtonPressedState[BUTTON_UP];
			case LINE_D1:
				return !currentButtonPressedState[BUTTON_DOWN];
			case LINE_D2:
				return !currentButtonPressedState[BUTTON_LEFT];
			case LINE_D3:
				return !currentButtonPressedState[BUTTON_RIGHT];
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_B];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_C];
			case LINE_TH:
				return true;
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
			switch(lineID)
			{
			case LINE_D0:
				return false;
			case LINE_D1:
				return false;
			case LINE_D2:
				return false;
			case LINE_D3:
				return false;
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_A];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_START];
			case LINE_TH:
				return true;
			}
		}
		break;
	case 3:
		if(currentLineInputStateTH)
		{
			//D0 = Z
			//D1 = Y
			//D2 = X
			//D3 = Mode
			//TL = B
			//TR = C
			//TH = Null (+5v)
			switch(lineID)
			{
			case LINE_D0:
				return !currentButtonPressedState[BUTTON_Z];
			case LINE_D1:
				return !currentButtonPressedState[BUTTON_Y];
			case LINE_D2:
				return !currentButtonPressedState[BUTTON_X];
			case LINE_D3:
				return !currentButtonPressedState[BUTTON_MODE];
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_B];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_C];
			case LINE_TH:
				return true;
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
			switch(lineID)
			{
			case LINE_D0:
				return true;
			case LINE_D1:
				return true;
			case LINE_D2:
				return true;
			case LINE_D3:
				return true;
			case LINE_TL:
				return !currentButtonPressedState[BUTTON_A];
			case LINE_TR:
				return !currentButtonPressedState[BUTTON_START];
			case LINE_TH:
				return true;
			}
		}
		break;
	}

	//##TODO## Raise an assert if we end up here with an invalid setting for the
	//bankswitch counter or the target line.
	return false;
}
