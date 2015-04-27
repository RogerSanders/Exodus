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
	lineInputStateTH = false;
	bankswitchingDisabled = false;
	bankswitchCounter = 0;
	bankswitchCounterToggleLastRisingEdge = 0;
	for(unsigned int i = 0; i < outputLineCount; ++i)
	{
		LineID lineID = (LineID)((unsigned int)LineID::D0 + i);
		outputLineState[i].asserted = GetDesiredLineState(bankswitchCounter, lineInputStateTH, buttonPressed, lineID);
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
bool MDControl6::AddReference(const MarshalSupport::Marshal::In<std::wstring>& referenceName, IBusInterface* target)
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
void MDControl6::RemoveReference(IBusInterface* target)
{
	if(memoryBus == target)
	{
		memoryBus = 0;
	}
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
	//If the bankswitch counter is currently greater than 0, a timeout could occur,
	//causing it to revert to 0 and the output line state of our output lines to be
	//altered. In this case, we need to check if a timeout has already occurred, in which
	//case we need to update the internal state of this device to reflect the new line
	//state after the timeout, or we need to rebase the timeout time values to be relative
	//to the start of the new timeslice.
	if(bankswitchCounter > 0)
	{
		double elapsedTimeSinceLastTHLineStateChange = nanoseconds - bankswitchCounterToggleLastRisingEdge;
		if(elapsedTimeSinceLastTHLineStateChange >= bankswitchTimeoutInterval)
		{
			//Reset the bankswitch counter back to zero
			bankswitchCounter = 0;
			bankswitchCounterToggleLastRisingEdge = 0;

			//Since we've passed the timeout times for all our lines now, change the
			//asserted state for our lines to the state we changed to after the timeout
			//was reached, and clear the flags indicating there is a timeout currently
			//flagged for our output lines.
			for(unsigned int i = 0; i < outputLineCount; ++i)
			{
				if(outputLineState[i].timeoutFlagged)
				{
					//##DEBUG##
					if(outputLineState[i].timeoutTime > nanoseconds)
					{
						std::wcout << "MDControl6 Timeout reverted before target time reached! " << i << '\t' << outputLineState[i].timeoutTime << '\t' << nanoseconds << '\n';
					}

					outputLineState[i].asserted = outputLineState[i].timeoutAssertedState;
					outputLineState[i].timeoutFlagged = false;
				}
			}
		}
		else
		{
			//Go through each pending line state timeout and adjust the time of the access
			//here, to rebase them for the start of the new timeslice. We need to do this
			//so that we can revoke them later if required.
			for(unsigned int i = 0; i < outputLineCount; ++i)
			{
				if(outputLineState[i].timeoutFlagged)
				{
					outputLineState[i].timeoutTime -= currentTimesliceLength;
				}
			}

			//Rebase the latched time at which the last input TH line rising edge occurred
			//to the start of the new timeslice
			if(bankswitchCounter > 0)
			{
				bankswitchCounterToggleLastRisingEdge -= currentTimesliceLength;
			}
		}
	}

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
unsigned int MDControl6::GetLineID(const MarshalSupport::Marshal::In<std::wstring>& lineName) const
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
MarshalSupport::Marshal::Ret<std::wstring> MDControl6::GetLineName(unsigned int lineID) const
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
unsigned int MDControl6::GetLineWidth(unsigned int lineID) const
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
void MDControl6::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(lineMutex);

	//Read the time at which this access is being made, and trigger a rollback if the
	//device has been accessed out of order.
	if(lastLineAccessTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastLineAccessTime = accessTime;

	//If the timeout period elapsed for the bankswitch counter state to be maintained,
	//correct our internal state to reflect the changes after the timeout. Note that the
	//output line state would have been reverted already by this point, we're just
	//correcting the internal digital state to what it would have already been in the real
	//hardware after the timeout period elapsed.
	if(bankswitchCounter > 0)
	{
		double elapsedTimeSinceLastTHLineStateChange = accessTime - bankswitchCounterToggleLastRisingEdge;
		if(elapsedTimeSinceLastTHLineStateChange >= bankswitchTimeoutInterval)
		{
			//Reset the bankswitch counter back to zero
			bankswitchCounter = 0;
			bankswitchCounterToggleLastRisingEdge = 0;

			//Since we've passed the timeout times for all our lines now, change the
			//asserted state for our lines to the state we changed to after the timeout
			//was reached, and clear the flags indicating there is a timeout currently
			//flagged for our output lines.
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
	if((LineID)targetLine == LineID::TH)
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
	SetLineState(targetLine, lineData, 0, currentTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
void MDControl6::AssertCurrentOutputLineState() const
{
	if(memoryBus == 0)
	{
		return;
	}

	//Assert the current line output state for the output lines
	if(outputLineState[0].asserted) memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[1].asserted) memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[2].asserted) memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[3].asserted) memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[4].asserted) memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[5].asserted) memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[6].asserted) memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

	//Re-assert any pending line state timeout changes
	if(outputLineState[GetLineIndex(LineID::D0)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D0, Data(1, outputLineState[GetLineIndex(LineID::D0)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D0)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, outputLineState[GetLineIndex(LineID::D0)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::D0)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::D1)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D1, Data(1, outputLineState[GetLineIndex(LineID::D1)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D1)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, outputLineState[GetLineIndex(LineID::D1)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::D1)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::D2)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D2, Data(1, outputLineState[GetLineIndex(LineID::D2)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D2)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, outputLineState[GetLineIndex(LineID::D2)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::D2)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::D3)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D3, Data(1, outputLineState[GetLineIndex(LineID::D3)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D3)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, outputLineState[GetLineIndex(LineID::D3)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::D3)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::TL)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TL, Data(1, outputLineState[GetLineIndex(LineID::TL)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TL)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, outputLineState[GetLineIndex(LineID::TL)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::TL)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::TR)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TR, Data(1, outputLineState[GetLineIndex(LineID::TR)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TR)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, outputLineState[GetLineIndex(LineID::TR)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::TR)].timeoutTime, 0);
	}
	if(outputLineState[GetLineIndex(LineID::TH)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TH, Data(1, outputLineState[GetLineIndex(LineID::TH)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TH)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, outputLineState[GetLineIndex(LineID::TH)].timeoutAssertedState), GetDeviceContext(), GetDeviceContext(), outputLineState[GetLineIndex(LineID::TH)].timeoutTime, 0);
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
	if(outputLineState[0].asserted) memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[1].asserted) memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[2].asserted) memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[3].asserted) memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[4].asserted) memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[5].asserted) memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(outputLineState[6].asserted) memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);

	//Revoke any pending line state timeout changes
	if(outputLineState[GetLineIndex(LineID::D0)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D0, Data(1, outputLineState[GetLineIndex(LineID::D0)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D0)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::D1)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D1, Data(1, outputLineState[GetLineIndex(LineID::D1)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D1)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::D2)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D2, Data(1, outputLineState[GetLineIndex(LineID::D2)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D2)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::D3)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::D3, Data(1, outputLineState[GetLineIndex(LineID::D3)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::D3)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::TL)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TL, Data(1, outputLineState[GetLineIndex(LineID::TL)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TL)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::TR)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TR, Data(1, outputLineState[GetLineIndex(LineID::TR)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TR)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
	if(outputLineState[GetLineIndex(LineID::TH)].timeoutFlagged)
	{
		memoryBus->RevokeSetLineState((unsigned int)LineID::TH, Data(1, outputLineState[GetLineIndex(LineID::TH)].timeoutAssertedState), outputLineState[GetLineIndex(LineID::TH)].timeoutTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::UpdateLineState(bool timeoutSettingsChanged, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Update the line state for each output line
	UpdateOutputLineStateForLine(LineID::D0, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::D1, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::D2, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::D3, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::TL, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::TR, timeoutSettingsChanged, caller, accessTime, accessContext);
	UpdateOutputLineStateForLine(LineID::TH, timeoutSettingsChanged, caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void MDControl6::UpdateOutputLineStateForLine(LineID lineID, bool revokeAllTimeoutStateChanges, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Calculate the index of this line in the output line state array
	unsigned int lineIndex = GetLineIndex(lineID);

	//Update the target line output state
	bool lineAssertedNew = GetDesiredLineState(bankswitchCounter, lineInputStateTH, buttonPressed, lineID);
	if(outputLineState[lineIndex].asserted != lineAssertedNew)
	{
		outputLineState[lineIndex].asserted = lineAssertedNew;
		memoryBus->SetLineState((unsigned int)lineID, Data(1, (unsigned int)lineAssertedNew), GetDeviceContext(), caller, accessTime, accessContext);
	}

	//Determine if we require a change to be made on the target line when the timeout
	//occurs
	bool timeoutLineStateChangeRequired = false;
	double elapsedTimeSinceLastTHLineStateChange;
	bool lineAssertedAfterTimeoutNew;
	if(bankswitchCounter > 0)
	{
		elapsedTimeSinceLastTHLineStateChange = accessTime - bankswitchCounterToggleLastRisingEdge;
		if(elapsedTimeSinceLastTHLineStateChange < bankswitchTimeoutInterval)
		{
			lineAssertedAfterTimeoutNew = GetDesiredLineState(0, lineInputStateTH, buttonPressed, lineID);
			timeoutLineStateChangeRequired = (lineAssertedNew != lineAssertedAfterTimeoutNew);
		}
	}

	//Revoke an existing timeout line state change if the timeout setting has changed, or
	//if a line state change is no longer required on timeout.
	if(outputLineState[lineIndex].timeoutFlagged && (revokeAllTimeoutStateChanges || !timeoutLineStateChangeRequired))
	{
		memoryBus->RevokeSetLineState((unsigned int)lineID, Data(1, outputLineState[lineIndex].timeoutAssertedState), outputLineState[lineIndex].timeoutTime, GetDeviceContext(), caller, accessTime, accessContext);
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
			std::wcout << "MDControl6 timeoutTime <= accessTime: " << timeoutTime << '\t' << accessTime << '\t' << (unsigned int)lineID << '\n';
		}

		//Record information on the timeout for the target line
		outputLineState[lineIndex].timeoutFlagged = true;
		outputLineState[lineIndex].timeoutAssertedState = lineAssertedAfterTimeoutNew;
		outputLineState[lineIndex].timeoutTime = timeoutTime;

		//Raise the future line state change for the timeout
		memoryBus->SetLineState((unsigned int)lineID, Data(1, (unsigned int)outputLineState[lineIndex].timeoutAssertedState), GetDeviceContext(), caller, outputLineState[lineIndex].timeoutTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------
bool MDControl6::GetDesiredLineState(unsigned int currentBankswitchCounter, unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, LineID lineID)
{
	switch(currentBankswitchCounter)
	{
	case 0:
	case 1:
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
			//D0 = Null (grounded)
			//D1 = Null (grounded)
			//D2 = Null (grounded)
			//D3 = Null (grounded)
			//TL = A
			//TR = Start
			//TH = Null (+5v)
			switch(lineID)
			{
			case LineID::D0:
				return false;
			case LineID::D1:
				return false;
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
			case LineID::D0:
				return !currentButtonPressedState[BUTTONINDEX_Z];
			case LineID::D1:
				return !currentButtonPressedState[BUTTONINDEX_Y];
			case LineID::D2:
				return !currentButtonPressedState[BUTTONINDEX_X];
			case LineID::D3:
				return !currentButtonPressedState[BUTTONINDEX_MODE];
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
			//D0 = Null (+5v)
			//D1 = Null (+5v)
			//D2 = Null (+5v)
			//D3 = Null (+5v)
			//TL = A
			//TR = Start
			//TH = Null (+5v)
			switch(lineID)
			{
			case LineID::D0:
				return true;
			case LineID::D1:
				return true;
			case LineID::D2:
				return true;
			case LineID::D3:
				return true;
			case LineID::TL:
				return !currentButtonPressedState[BUTTONINDEX_A];
			case LineID::TR:
				return !currentButtonPressedState[BUTTONINDEX_START];
			case LineID::TH:
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
unsigned int MDControl6::GetLineIndex(LineID lineID)
{
	return (unsigned int)lineID - 1;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int MDControl6::GetKeyCodeID(const MarshalSupport::Marshal::In<std::wstring>& keyCodeName) const
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
	else if(keyCodeName == L"X")
	{
		return BUTTONINDEX_X+1;
	}
	else if(keyCodeName == L"Y")
	{
		return BUTTONINDEX_Y+1;
	}
	else if(keyCodeName == L"Z")
	{
		return BUTTONINDEX_Z+1;
	}
	else if(keyCodeName == L"Mode")
	{
		return BUTTONINDEX_MODE+1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> MDControl6::GetKeyCodeName(unsigned int keyCodeID) const
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
	case BUTTONINDEX_X+1:
		return L"X";
	case BUTTONINDEX_Y+1:
		return L"Y";
	case BUTTONINDEX_Z+1:
		return L"Z";
	case BUTTONINDEX_MODE+1:
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
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
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
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
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
void MDControl6::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageNode& node = *(*i);
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
			IHierarchicalStorageAttribute* lineNumberAttribute = node.GetAttribute(L"LineNo");
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
	}
}

//----------------------------------------------------------------------------------------
void MDControl6::SaveState(IHierarchicalStorageNode& node) const
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
