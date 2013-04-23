#include "MDControl6.h"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MDControl6::MDControl6(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID)
{
	//##TODO## Perform our own hardware tests to confirm this value
	bankswitchTimeoutInterval = 1600000.0; //1500000.0;

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
	lastLineAccessTime = 0;
}

//----------------------------------------------------------------------------------------
bool MDControl6::ValidateDevice()
{
	return (memoryBus != 0);
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool MDControl6::AddReference(const std::wstring& referenceName, IBusInterface* target)
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

			//If the timeout has now passed on this output line, change the asserted state
			//for the line to the state we changed to after the timeout was reached, and
			//clear the flags indicating there is a timeout currently flagged for this
			//output line. Note that we do this here so that bogus line state changes
			//don't stick around in the event that the controller isn't accessed by the
			//system. If we don't clear these old events, we can end up trying to revoke
			//them later with a negative time specified.
			if(outputLineState[i].timeoutTime <= 0)
			{
				outputLineState[i].asserted = outputLineState[i].timeoutAssertedState;
				outputLineState[i].timeoutFlagged = false;
			}
		}
	}

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
	currentTimesliceLength = bcurrentTimesliceLength;
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
	bcurrentTimesliceLength = currentTimesliceLength;
	blineInputStateTH = lineInputStateTH;
	bbankswitchingDisabled = bankswitchingDisabled;
	bbankswitchCounter = bankswitchCounter;
	bbankswitchCounterToggleLastRisingEdge = bankswitchCounterToggleLastRisingEdge;
	boutputLineState = outputLineState;
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int MDControl6::GetLineID(const std::wstring& lineName) const
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
std::wstring MDControl6::GetLineName(unsigned int lineID) const
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

		//Since we've passed the timeout times for all our lines now, change the asserted
		//state for our lines to the state we changed to after the timeout was reached,
		//and clear the flags indicating there is a timeout currently flagged for our
		//output lines.
		for(unsigned int i = 0; i < outputLineCount; ++i)
		{
			if(outputLineState[i].timeoutFlagged)
			{
				//##DEBUG##
				if(outputLineState[i].timeoutTime > accessTime)
				{
					std::wcout << "MDControl6 Timeout reverted before target time reached! " << i << '\t' << outputLineState[i].timeoutTime << '\t' << accessTime << '\n';
				}

				outputLineState[i].asserted = outputLineState[i].timeoutAssertedState;
				outputLineState[i].timeoutFlagged = false;
			}
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
			//##FIX## Based on "Joystick Test Program (PD).bin", we now believe that when
			//a timeout occurs, it isn't like we do here, where if the TH line is
			//currently negated at the time the timeout occurs, the very next time the TH
			//line state is toggled after the timeout, the bank will increment to 1.
			//Rather, it appears that the bankswitch increment will be missed for this
			//first time. Actually, on further consideration, this might not be correct.
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
void MDControl6::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, 0.0, 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::AssertCurrentOutputLineState() const
{
	if(memoryBus == 0)
	{
		return;
	}

	//Assert the current line output state for the output lines
	if(outputLineState[0].asserted) memoryBus->SetLineState(LINE_D0, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[1].asserted) memoryBus->SetLineState(LINE_D1, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[2].asserted) memoryBus->SetLineState(LINE_D2, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[3].asserted) memoryBus->SetLineState(LINE_D3, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[4].asserted) memoryBus->SetLineState(LINE_TL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[5].asserted) memoryBus->SetLineState(LINE_TR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[6].asserted) memoryBus->SetLineState(LINE_TH, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

	//Re-assert any pending line state timeout changes
	if(outputLineState[LINE_D0-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D0, Data(1, outputLineState[LINE_D0-LINE_D0].timeoutAssertedState), outputLineState[LINE_D0-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_D0, Data(1, outputLineState[LINE_D0-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_D0-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_D1-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D1, Data(1, outputLineState[LINE_D1-LINE_D0].timeoutAssertedState), outputLineState[LINE_D1-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_D1, Data(1, outputLineState[LINE_D1-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_D1-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_D2-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D2, Data(1, outputLineState[LINE_D2-LINE_D0].timeoutAssertedState), outputLineState[LINE_D2-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_D2, Data(1, outputLineState[LINE_D2-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_D2-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_D3-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D3, Data(1, outputLineState[LINE_D3-LINE_D0].timeoutAssertedState), outputLineState[LINE_D3-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_D3, Data(1, outputLineState[LINE_D3-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_D3-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_TL-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TL, Data(1, outputLineState[LINE_TL-LINE_D0].timeoutAssertedState), outputLineState[LINE_TL-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_TL, Data(1, outputLineState[LINE_TL-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_TL-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_TR-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TR, Data(1, outputLineState[LINE_TR-LINE_D0].timeoutAssertedState), outputLineState[LINE_TR-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_TR, Data(1, outputLineState[LINE_TR-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_TR-LINE_D0].timeoutTime, 0);
	}
	if(outputLineState[LINE_TH-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TH, Data(1, outputLineState[LINE_TH-LINE_D0].timeoutAssertedState), outputLineState[LINE_TH-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState(LINE_TH, Data(1, outputLineState[LINE_TH-LINE_D0].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[LINE_TH-LINE_D0].timeoutTime, 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::NegateCurrentOutputLineState() const
{
	if(memoryBus == 0)
	{
		return;
	}

	//Negate the current line output state for the output lines
	if(outputLineState[0].asserted) memoryBus->SetLineState(LINE_D0, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[1].asserted) memoryBus->SetLineState(LINE_D1, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[2].asserted) memoryBus->SetLineState(LINE_D2, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[3].asserted) memoryBus->SetLineState(LINE_D3, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[4].asserted) memoryBus->SetLineState(LINE_TL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[5].asserted) memoryBus->SetLineState(LINE_TR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[6].asserted) memoryBus->SetLineState(LINE_TH, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

	//Revoke any pending line state timeout changes
	if(outputLineState[LINE_D0-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D0, Data(1, outputLineState[LINE_D0-LINE_D0].timeoutAssertedState), outputLineState[LINE_D0-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_D1-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D1, Data(1, outputLineState[LINE_D1-LINE_D0].timeoutAssertedState), outputLineState[LINE_D1-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_D2-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D2, Data(1, outputLineState[LINE_D2-LINE_D0].timeoutAssertedState), outputLineState[LINE_D2-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_D3-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_D3, Data(1, outputLineState[LINE_D3-LINE_D0].timeoutAssertedState), outputLineState[LINE_D3-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_TL-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TL, Data(1, outputLineState[LINE_TL-LINE_D0].timeoutAssertedState), outputLineState[LINE_TL-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_TR-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TR, Data(1, outputLineState[LINE_TR-LINE_D0].timeoutAssertedState), outputLineState[LINE_TR-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[LINE_TH-LINE_D0].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState(LINE_TH, Data(1, outputLineState[LINE_TH-LINE_D0].timeoutAssertedState), outputLineState[LINE_TH-LINE_D0].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
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

	//Determine if we require a change to be made on the target line when the timeout
	//occurs
	double elapsedTimeSinceLastTHLineStateChange = accessTime - bankswitchCounterToggleLastRisingEdge;
	bool lineAssertedAfterTimeoutNew = GetDesiredLineState(0, lineInputStateTH, buttonPressed, lineID);
	bool timeoutLineStateChangeRequired = (lineAssertedNew != lineAssertedAfterTimeoutNew) && (elapsedTimeSinceLastTHLineStateChange < bankswitchTimeoutInterval);

	//Revoke an existing timeout line state change if the timeout setting has changed, or
	//if a line state change is no longer required on timeout.
	if(outputLineState[lineIndex].timeoutFlagged && (revokeAllTimeoutStateChanges || !timeoutLineStateChangeRequired))
	{
		memoryBus->RevokeSetLineState(lineID, Data(1, outputLineState[lineIndex].timeoutAssertedState), outputLineState[lineIndex].timeoutTime, GetDeviceContext(), caller, accessTime, accessContext);
		outputLineState[lineIndex].timeoutFlagged = false;
	}

	//Raise a target line output state change on bankswitch timeout if required
	if(timeoutLineStateChangeRequired)
	{
		//Calculate the time at which a timeout will occur, and the state of the target
		//line should be altered.
		double timeoutTime = accessTime + (bankswitchTimeoutInterval - elapsedTimeSinceLastTHLineStateChange);

		//##DEBUG##
		if(timeoutTime <= accessTime)
		{
			std::wcout << "MDControl6 timeoutTime <= accessTime: " << timeoutTime << '\t' << accessTime << '\t' << lineID << '\n';
		}

		//Record information on the timeout for the target line
		outputLineState[lineIndex].timeoutFlagged = true;
		outputLineState[lineIndex].timeoutAssertedState = lineAssertedAfterTimeoutNew;
		outputLineState[lineIndex].timeoutTime = timeoutTime;

		//Raise the future line state change for the timeout
		memoryBus->SetLineState(lineID, Data(1, (unsigned int)outputLineState[lineIndex].timeoutAssertedState), GetDeviceContext(), caller, outputLineState[lineIndex].timeoutTime, accessContext);
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

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int MDControl6::GetKeyCodeID(const std::wstring& keyCodeName) const
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
	else if(keyCodeName == L"X")
	{
		return BUTTON_X+1;
	}
	else if(keyCodeName == L"Y")
	{
		return BUTTON_Y+1;
	}
	else if(keyCodeName == L"Z")
	{
		return BUTTON_Z+1;
	}
	else if(keyCodeName == L"Mode")
	{
		return BUTTON_MODE+1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring MDControl6::GetKeyCodeName(unsigned int keyCodeID) const
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
	case BUTTON_X+1:
		return L"X";
	case BUTTON_Y+1:
		return L"Y";
	case BUTTON_Z+1:
		return L"Z";
	case BUTTON_MODE+1:
		return L"Mode";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void MDControl6::HandleInputKeyDown(unsigned int keyCodeID)
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
	Button keyCode = (Button)(keyCodeID-1);
	bool newButtonPressState = true;
	if(buttonPressed[keyCode] != newButtonPressState)
	{
		buttonPressed[keyCode] = newButtonPressState;
		UpdateLineState(false, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::HandleInputKeyUp(unsigned int keyCodeID)
{
	Button keyCode = (Button)(keyCodeID-1);
	bool newButtonPressState = false;
	if(buttonPressed[keyCode] != newButtonPressState)
	{
		buttonPressed[keyCode] = newButtonPressState;
		UpdateLineState(false, GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void MDControl6::LoadState(IHeirarchicalStorageNode& node)
{
	std::list<IHeirarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHeirarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHeirarchicalStorageNode& node = *(*i);
		std::wstring nodeName = node.GetName();
		if(nodeName == L"CurrentTimesliceLength")
		{
			node.ExtractData(currentTimesliceLength);
		}
		else if(nodeName == L"LineInputStateTH")
		{
			node.ExtractData(lineInputStateTH);
		}
		else if(nodeName == L"BankswitchingDisabled")
		{
			node.ExtractData(bankswitchingDisabled);
		}
		else if(nodeName == L"BankswitchCounter")
		{
			node.ExtractData(bankswitchCounter);
		}
		else if(nodeName == L"BankswitchCounterToggleLastRisingEdge")
		{
			node.ExtractData(bankswitchCounterToggleLastRisingEdge);
		}
		else if(nodeName == L"OutputLineState")
		{
			IHeirarchicalStorageAttribute* lineNumberAttribute = node.GetAttribute(L"LineNo");
			if(lineNumberAttribute != 0)
			{
				unsigned int lineNo = lineNumberAttribute->ExtractValue<unsigned int>();
				if(lineNo < outputLineCount)
				{
					node.ExtractAttribute(L"Asserted", outputLineState[lineNo].asserted);
					node.ExtractAttribute(L"TimeoutFlagged", outputLineState[lineNo].timeoutFlagged);
					node.ExtractAttribute(L"TimeoutAssertedState", outputLineState[lineNo].timeoutAssertedState);
					node.ExtractAttribute(L"TimeoutTime", outputLineState[lineNo].timeoutTime);
				}
			}
		}
		else if(nodeName == L"ButtonPressed")
		{
			IHeirarchicalStorageAttribute* buttonNumberAttribute = node.GetAttribute(L"ButtonNo");
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
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::SaveState(IHeirarchicalStorageNode& node) const
{
	node.CreateChild(L"CurrentTimesliceLength", currentTimesliceLength);
	node.CreateChild(L"LineInputStateTH", lineInputStateTH);
	node.CreateChild(L"BankswitchingDisabled", bankswitchingDisabled);
	node.CreateChild(L"BankswitchCounter", bankswitchCounter);
	node.CreateChild(L"BankswitchCounterToggleLastRisingEdge", bankswitchCounterToggleLastRisingEdge);

	for(unsigned int i = 0; i < outputLineCount; ++i)
	{
		node.CreateChild(L"OutputLineState").CreateAttribute(L"LineNo", i).CreateAttribute(L"Asserted", outputLineState[i].asserted).CreateAttribute(L"TimeoutFlagged", outputLineState[i].timeoutFlagged).CreateAttribute(L"TimeoutAssertedState", outputLineState[i].timeoutAssertedState).CreateAttribute(L"TimeoutTime", outputLineState[i].timeoutTime);
	}
	for(unsigned int i = 0; i < buttonCount; ++i)
	{
		node.CreateChild(L"ButtonPressed").CreateAttribute(L"ButtonNo", i).CreateAttribute(L"Pressed", buttonPressed[i]);
	}
}
