#include "S315_5313.h"

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineID(const std::wstring& lineName) const
{
	if(lineName == L"IPL")        //O
	{
		return LINE_IPL;
	}
	else if(lineName == L"INT")   //O
	{
		return LINE_INT;
	}
	else if(lineName == L"INTAK") //I
	{
		return LINE_INTAK;
	}
	else if(lineName == L"BR")    //O
	{
		return LINE_BR;
	}
	else if(lineName == L"BG")    //I
	{
		return LINE_BG;
	}
	else if(lineName == L"PAL")   //I
	{
		return LINE_PAL;
	}
	else if(lineName == L"RESET") //I
	{
		return LINE_RESET;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring S315_5313::GetLineName(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return L"IPL";
	case LINE_INT:
		return L"INT";
	case LINE_INTAK:
		return L"INTAK";
	case LINE_BR:
		return L"BR";
	case LINE_BG:
		return L"BG";
	case LINE_PAL:
		return L"PAL";
	case LINE_RESET:
		return L"RESET";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetLineWidth(unsigned int lineID) const
{
	switch(lineID)
	{
	case LINE_IPL:
		return 3;
	case LINE_INT:
		return 1;
	case LINE_INTAK:
		return 1;
	case LINE_BR:
		return 1;
	case LINE_BG:
		return 1;
	case LINE_PAL:
		return 1;
	case LINE_RESET:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(lineMutex);

	//Process the line state change
	switch(targetLine)
	{
	case LINE_INTAK:{
		//##DEBUG##
		//std::wcout << "SetLineState - VDP_LINE_INTAK:\t" << lineData.LSB() << '\n';

		//We explicitly release our lock on lineMutex here so that we're not blocking
		//access to SetLineState() on this class before we modify the line state for other
		//devices in the code that follows. Adhering to this pattern helps to avoid
		//deadlock cases that could otherwise arise from valid line mappings.
		lock.unlock();

		//Note that we take a lock on accessMutex here, to synchronize INTAK
		//acknowledgments with the memory interface for the VDP. These interfaces need to
		//be synchronized in order to correctly predict changes to the IPL line state.
		boost::mutex::scoped_lock lock(accessMutex);

		//Convert the access time into a cycle count relative to MCLK, rounding up the
		//result to the nearest MCLK cycle.
		unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
		double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
		if(accessMclkCycleInAccessTime < accessTime)
		{
			accessMclkCycle += 1;
		}

		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);

		//Determine which pending interrupt was just acknowledged
		if(vintEnabled && vintPending)
		{
			vintPending = false;
			//Since the interrupt has been acknowledged, clear the VINT occurrence flag.
			SetStatusFlagF(false);
		}
		else if(hintEnabled && hintPending)
		{
			hintPending = false;
		}
		else if(exintEnabled && exintPending)
		{
			exintPending = false;
		}

		//##DEBUG##
		unsigned int originalLineStateIPL = lineStateIPL;

		//Calculate the new IPL line state
		unsigned int newLineStateIPL = GetNewIPLLineState();
		bool assertIPLLine = false;
		if(lineStateIPL != newLineStateIPL)
		{
			lineStateIPL = newLineStateIPL;
			assertIPLLine = true;
		}

		//If the IPL line state has changed, apply the new line state.
		if(assertIPLLine)
		{
			//##DEBUG##
			if(outputInterruptDebugMessages)
			{
				std::wcout << "VDP - INTAK IPL line state change: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << originalLineStateIPL << '\t' << newLineStateIPL << '\n';
			}

			memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), newLineStateIPL), GetDeviceContext(), caller, accessTime, accessContext);
		}

		//Predict the time at which the next IPL line state change will occur, and
		//conditionally assert the IPL line.
		UpdatePredictedLineStateChanges(caller, accessTime, accessContext);

		break;}
	case LINE_BG:{
		//##DEBUG##
		//		std::wcout << "SetLineState - VDP_LINE_BG:\t" << lineData.LSB() << '\n';

		//Convert the access time into a cycle count relative to MCLK, rounding up the
		//result to the nearest MCLK cycle.
		unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
		double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
		if(accessMclkCycleInAccessTime < accessTime)
		{
			accessMclkCycle += 1;
		}

		//Obtain a lock on workerThreadMutex so we can safely work with DMA state data,
		//and notify the DMA worker thread of the BG line state change, if this line state
		//change is of interest to us.
		boost::mutex::scoped_lock workerThreadLock(workerThreadMutex);
		if(dmaTransferActive || busGranted)
		{
			busGranted = lineData.LSB();
			//##FIX## The DMA bitmap demos have revealed that the DMA worker thread cannot
			//access the bus freely on any clock cycle. In fact, the DMA worker thread can
			//only access the external bus once every 4 SC cycles, and those clock cycles
			//occur at fixed points. This means that the initial read time for a DMA
			//transfer operation will vary, and is not synchronized with the BG line
			//change time. We actually need to simply flag that a DMA transfer operation
			//has been initiated but a start cycle not latched, and then when the
			//processor state is next advanced, latch the data at the next available DMA
			//access slot.
			//##TODO## Measure the total execution time of DMA fill and copy operations
			//when the display is disabled, to determine if the same access slots and
			//execution times apply for DMA fill and copy operations as for DMA transfer
			//operations.
			dmaTransferNextReadMclk = accessMclkCycle;
			dmaTransferLastTimesliceUsedReadDelay = 0;

			//If the bus has just been granted in response to a DMA transfer operation
			//initiating a bus request, set the current timeslice progress for the VDP
			//core to match the time at which the bus was granted. This allows us to set
			//the device which controls bus ownership as a dependent device on the VDP
			//core, as the timeslice progress will be valid during a DMA transfer.
			if(busGranted)
			{
				GetDeviceContext()->SetCurrentTimesliceProgress(accessTime);
			}

			GetDeviceContext()->SetTransientExecutionActive(busGranted);
			workerThreadUpdate.notify_all();
		}
		break;}
	case LINE_PAL:{
		//##DEBUG##
		//std::wcout << "SetLineState - VDP_LINE_PAL:\t" << lineData.LSB() << '\n';

		palModeLineState = lineData.LSB();
		break;}
	case LINE_RESET:{
		bool resetLineStateNew = lineData.LSB();
		if(resetLineStateNew != resetLineState)
		{
			resetLineState = resetLineStateNew;
			Reset(accessTime);
		}
		break;}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, currentTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------
bool S315_5313::AdvanceToLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	switch(targetLine)
	{
	case LINE_BR:{
		boost::mutex::scoped_lock workerLock(workerThreadMutex);
		bool targetLineState = lineData.GetBit(0);
		bool busRequested = dmaTransferActive;
		if(busRequested == targetLineState)
		{
			//If the current state of the target line matches the target state, we have
			//nothing to do.
			return true;
		}
		else if(!busGranted && busRequested && !targetLineState)
		{
			//If we've requested the bus, but the M68000 hasn't granted it yet, request
			//the M68000 to advance until the bus is granted.
			workerLock.unlock();
			if(!memoryBus->AdvanceToLineState(LINE_BG, Data(1, 1), GetDeviceContext(), caller, accessTime, accessContext))
			{
				//##DEBUG##
				std::wcout << "VDP failed to advance M68000 to BG line state 1 for target BR line state! " << targetLineState << '\t' << busGranted << '\t' << busRequested << '\n';
				return false;
			}
			workerLock.lock();
			//We expect that the bus has now been granted, in which case, we proceed now
			//as if the bus had already been granted, but not reached the target line
			//state.
		}

		if(busGranted && busRequested && !targetLineState)
		{
			//If the worker thread isn't currently paused, wait for it to go idle. We know
			//it can't be stopped here, since we have a lock on workerThreadMutex, and the
			//busGranted flag is set.
			if(!workerThreadPaused)
			{
				workerThreadIdle.wait(workerLock);
			}

			//If we currently have the bus, but we've reached the end of the current
			//timeslice before a DMA operation could complete, and the caller is
			//requesting that we advance until the bus request line is negated, we can
			//accurately perform that operation. In this case, we instruct the DMA worker
			//thread to run the DMA operation to completion.
			if(workerThreadPaused && busGranted)
			{
				dmaAdvanceUntilDMAComplete = true;
				workerThreadUpdate.notify_all();
				workerThreadIdle.wait(workerLock);
				dmaAdvanceUntilDMAComplete = false;
			}

			//If we've released our request on the bus, request the M68000 to advance
			//until the bus is reclaimed.
			workerLock.unlock();
			if(!memoryBus->AdvanceToLineState(LINE_BG, Data(1, 0), GetDeviceContext(), caller, accessTime, accessContext))
			{
				//##DEBUG##
				std::wcout << "VDP failed to advance M68000 to BG line state 0 for target BR line state! " << targetLineState << '\t' << busGranted << '\t' << busRequested << '\n';
				return false;
			}
			workerLock.lock();

			bool advanceSucceeded = (busGranted == targetLineState);
			//##DEBUG##
			if(!advanceSucceeded)
			{
				std::wcout << "VDP failed to advance DMA worker thread to end of operation for target BR line state! " << targetLineState << '\t' << busGranted << '\t' << busRequested << '\n';
			}
			return advanceSucceeded;
		}
		//##DEBUG##
		else
		{
			std::wcout << "VDP couldn't advance to target BR line state! " << targetLineState << '\t' << busGranted << '\t' << busRequested << '\n';
		}
		break;}
	case LINE_INT:
		//##TODO## We don't require this for the Mega Drive, but implement it anyway.
		break;
	}
	return false;
}

//----------------------------------------------------------------------------------------
void S315_5313::AssertCurrentOutputLineState() const
{
	if(memoryBus == 0)
	{
		return;
	}

	//Assert the current line output state for the output lines
	if(busRequestLineState) memoryBus->SetLineState(LINE_BR, Data(GetLineWidth(LINE_BR), 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(lineStateIPL != 0)   memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), lineStateIPL), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);

	//Re-assert any pending IPL line state changes
	if(lineStateChangePendingVINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), lineStateChangeVINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), GetDeviceContext(), GetDeviceContext(), lineStateChangeVINTTime, ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingHINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), lineStateChangeHINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), GetDeviceContext(), GetDeviceContext(), lineStateChangeHINTTime, ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingEXINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), lineStateChangeEXINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
		memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), GetDeviceContext(), GetDeviceContext(), lineStateChangeEXINTTime, ACCESSCONTEXT_INTLINECHANGE);
	}

	//Re-assert any pending INT line state changes, and re-assert the INT line if it's
	//currently asserted.
	if(lineStateChangePendingINTAsserted)
	{
		memoryBus->RevokeSetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), lineStateChangeINTAssertedTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), GetDeviceContext(), GetDeviceContext(), lineStateChangeINTAssertedTime, ACCESSCONTEXT_INTLINECHANGE);
	}
	else if(lineStateChangePendingINTNegated)
	{
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingINTNegated)
	{
		memoryBus->RevokeSetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), lineStateChangeINTNegatedTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), GetDeviceContext(), GetDeviceContext(), lineStateChangeINTNegatedTime, ACCESSCONTEXT_INTLINECHANGE);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::NegateCurrentOutputLineState() const
{
	if(memoryBus == 0)
	{
		return;
	}

	//Negate the current line output state for the output lines
	if(busRequestLineState) memoryBus->SetLineState(LINE_BR, Data(GetLineWidth(LINE_BR), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	if(lineStateIPL != 0)   memoryBus->SetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);

	//Revoke any pending IPL line state changes
	if(lineStateChangePendingVINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), vintIPLLineState), lineStateChangeVINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingHINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), hintIPLLineState), lineStateChangeHINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingEXINT)
	{
		memoryBus->RevokeSetLineState(LINE_IPL, Data(GetLineWidth(LINE_IPL), exintIPLLineState), lineStateChangeEXINTTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}

	//Revoke any pending INT line state changes, and negate the INT line if it's currently
	//asserted.
	if(lineStateChangePendingINTAsserted)
	{
		memoryBus->RevokeSetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 1), lineStateChangeINTAssertedTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
	else if(lineStateChangePendingINTNegated)
	{
		memoryBus->SetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
	if(lineStateChangePendingINTNegated)
	{
		memoryBus->RevokeSetLineState(LINE_INT, Data(GetLineWidth(LINE_INT), 0), lineStateChangeINTNegatedTime, GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), ACCESSCONTEXT_INTLINECHANGE);
	}
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetNewIPLLineState()
{
	//Calculate the new IPL line state based on the current interrupts in the asserted
	//state. Hardware tests have shown that higher level interrupts mask pending
	//interrupts of a lower level, but once a higher level interrupt is acknowledged, the
	//the next lowest level pending interrupt that is still pending is immediately
	//asserted again. Using the current set of pending interrupts, we calculate the
	//effective state of the IPL lines below.
	//##TODO## Confirm that a pending external interrupt and a pending horizontal
	//interrupt doesn't end up asserting an effective IPL state of a VINT.
	unsigned int newLineStateIPL = 0;
	if(vintEnabled && vintPending)
	{
		newLineStateIPL = vintIPLLineState;
	}
	else if(hintEnabled && hintPending)
	{
		newLineStateIPL = hintIPLLineState;
	}
	else if(exintEnabled && exintPending)
	{
		newLineStateIPL = exintIPLLineState;
	}
	return newLineStateIPL;
}

//----------------------------------------------------------------------------------------
//##FIX## Write a version of the AddStepsToVCounter function which will take into account
//pending screen mode changes specified in an advance session. The current raw method will
//not.
void S315_5313::UpdatePredictedLineStateChanges(IDeviceContext* callingDevice, double accessTime, unsigned int accessContext)
{
	//Calculate the number of MCLK ticks required to reach the point at which the INT line
	//is asserted. Note that hardware tests show that the INT line is asserted at the same
	//time as a VINT is triggered, however the INT line is always asserted, regardless of
	//the state of the vertical interrupt enable flag. The INT line is cleared when the
	//vcounter is incremented.
	//##TODO## Confirm on the hardware that INT is triggered when VINT is disabled
	HVCounterAdvanceSession advanceSessionINTAsserted;
	BeginHVCounterAdvanceSessionFromCurrentState(advanceSessionINTAsserted);
	while(!AdvanceHVCounterSession(advanceSessionINTAsserted, advanceSessionINTAsserted.hscanSettings->fflagSetPoint, advanceSessionINTAsserted.vscanSettings->vblankSetPoint, true)) {}
	unsigned int mclkTicksBeforeTriggerTimeINTAsserted = advanceSessionINTAsserted.mclkTicksAdvanced;

	//Calculate the number of MCLK ticks required to reach the point at which the INT line
	//is negated.
	//##TODO## Confirm on the hardware that INT is negated when the vcounter is
	//incremented
	//##TODO## Perform hardware tests to confirm if the VDP negates the INT line when the
	//Z80 runs an interrupt acknowledge cycle.
	HVCounterAdvanceSession advanceSessionINTNegated;
	BeginHVCounterAdvanceSessionFromCurrentState(advanceSessionINTNegated);
	while(!AdvanceHVCounterSession(advanceSessionINTNegated, advanceSessionINTNegated.hscanSettings->vcounterIncrementPoint, advanceSessionINTNegated.vscanSettings->vblankSetPoint+1, true)) {}
	unsigned int mclkTicksBeforeTriggerTimeINTNegated = advanceSessionINTNegated.mclkTicksAdvanced;

	//Conditionally assert and negate the INT line
	UpdateLineStateChangePrediction(LINE_INT, 1, lineStateChangePendingINTAsserted, lineStateChangeINTAssertedMClkCountFromCurrent, lineStateChangeINTAssertedTime, true, mclkTicksBeforeTriggerTimeINTAsserted, callingDevice, accessTime, accessContext);
	UpdateLineStateChangePrediction(LINE_INT, 0, lineStateChangePendingINTNegated, lineStateChangeINTNegatedMClkCountFromCurrent, lineStateChangeINTNegatedTime, true, mclkTicksBeforeTriggerTimeINTNegated, callingDevice, accessTime, accessContext);

	//Predict when the next vertical interrupt is going to occur
	bool foundTriggerTimeVInt = false;
	unsigned int mclkTicksBeforeTriggerTimeVInt = 0;
	if(vintEnabled && (lineStateIPL < vintIPLLineState))
	{
		//Note that according to hardware tests performed by Eke, an internal interrupt
		//pending flag is set when a condition arises under which an interrupt could be
		//triggered, which remains set until an interrupt is both triggered and
		//acknowledged. The interrupt is only triggered when the interrupt enable bit is
		//set, which means that it's possible for an interrupt to be set as pending when
		//the enable bit is clear. In this case as soon as we enable the interrupt, the
		//interrupt is going to immediately be triggered if it is pending.
		//##TODO## Confirm that when an interrupt is being actively asserted over the IPL
		//lines but ignored by the M68000, that when the enabled bit for that interrupt is
		//changed in the VDP, that interrupt will stop being asserted over the IPL lines,
		//and any masked lower level interrupt will then be asserted instead.
		foundTriggerTimeVInt = true;
		if(vintPending)
		{
			mclkTicksBeforeTriggerTimeVInt = 0;
		}
		else
		{
			HVCounterAdvanceSession advanceSessionVInt;
			BeginHVCounterAdvanceSessionFromCurrentState(advanceSessionVInt);
			while(!AdvanceHVCounterSession(advanceSessionVInt, advanceSessionVInt.hscanSettings->fflagSetPoint, advanceSessionVInt.vscanSettings->vblankSetPoint, true)) {}
			mclkTicksBeforeTriggerTimeVInt = advanceSessionVInt.mclkTicksAdvanced;
		}
	}

	//Predict when the next horizontal interrupt is going to occur
	bool foundTriggerTimeHInt = false;
	unsigned int mclkTicksBeforeTriggerTimeHInt = 0;
	if(hintEnabled && (lineStateIPL < hintIPLLineState))
	{
		if(hintPending)
		{
			foundTriggerTimeHInt = true;
			mclkTicksBeforeTriggerTimeHInt = 0;
		}
		else
		{
			HVCounterAdvanceSession advanceSessionHInt;
			BeginHVCounterAdvanceSessionFromCurrentState(advanceSessionHInt);

			//If we're currently rendering within the active region of a frame, determine
			//the next point at which the hint counter will expire within this frame and
			//trigger an interrupt.
			if(advanceSessionHInt.vcounterCurrent < advanceSessionHInt.vscanSettings->vblankSetPoint)
			{
				//If the hint counter is going to expire before the end of the current
				//frame, calculate the number of pixel clock cycles before we reach that
				//point from the current render position, and set that as the hint trigger
				//time.
				unsigned int vcounterPlusHIntCounter = AddStepsToVCounter(*advanceSessionHInt.hscanSettings, advanceSessionHInt.hcounterCurrent, *advanceSessionHInt.vscanSettings, advanceSessionHInt.interlaceEnabledCurrent, advanceSessionHInt.oddFlagCurrent, advanceSessionHInt.vcounterCurrent, 1 + hintCounter);
				if((vcounterPlusHIntCounter <= advanceSessionHInt.vscanSettings->vblankSetPoint) && (vcounterPlusHIntCounter > advanceSessionHInt.vcounterCurrent))
				{
					foundTriggerTimeHInt = true;
					while(!AdvanceHVCounterSession(advanceSessionHInt, advanceSessionHInt.hscanSettings->vcounterIncrementPoint, vcounterPlusHIntCounter, true)) {}
					mclkTicksBeforeTriggerTimeHInt = advanceSessionHInt.mclkTicksAdvanced;
				}
			}

			//If we didn't manage to identify a point at which the hint counter will
			//expire before the start of active scan in a new frame, determine the point
			//at which the hint counter will expire and trigger an interrupt from the
			//start of a new frame.
			if(!foundTriggerTimeHInt)
			{
				//If the hint counter will expire before the end of active scan in a new
				//frame, calculate the number of MCLK cycles until we reach that point
				//from the current render position, and set that as the hint trigger time.
				while(!AdvanceHVCounterSession(advanceSessionHInt, advanceSessionHInt.hscanSettings->vcounterIncrementPoint, 0, true)) {}
				unsigned int hintCounterExpireVCounterPosFromStartOfFrame = AddStepsToVCounter(*advanceSessionHInt.hscanSettings, advanceSessionHInt.hcounterCurrent, *advanceSessionHInt.vscanSettings, advanceSessionHInt.interlaceEnabledCurrent, advanceSessionHInt.oddFlagCurrent, advanceSessionHInt.vcounterCurrent, hintCounterReloadValue);
				while(!AdvanceHVCounterSession(advanceSessionHInt, advanceSessionHInt.hscanSettings->vcounterIncrementPoint, hintCounterExpireVCounterPosFromStartOfFrame, false)) {}
				if(hintCounterExpireVCounterPosFromStartOfFrame <= advanceSessionHInt.vscanSettings->vblankSetPoint)
				{
					foundTriggerTimeHInt = true;
					mclkTicksBeforeTriggerTimeHInt = advanceSessionHInt.mclkTicksAdvanced;
				}
			}
		}
	}

	//Predict when the next external interrupt is going to occur
	bool foundTriggerTimeEXInt = false;
	unsigned int mclkTicksBeforeTriggerTimeEXInt = 0;
	if(exintEnabled && (lineStateIPL < exintIPLLineState))
	{
		if(exintPending)
		{
			foundTriggerTimeEXInt = true;
			mclkTicksBeforeTriggerTimeEXInt = 0;
		}
		else if(externalInterruptVideoTriggerPointPending)
		{
			foundTriggerTimeEXInt = true;
			HVCounterAdvanceSession advanceSessionEXInt;
			BeginHVCounterAdvanceSessionFromCurrentState(advanceSessionEXInt);
			while(!AdvanceHVCounterSession(advanceSessionEXInt, externalInterruptVideoTriggerPointHCounter, externalInterruptVideoTriggerPointVCounter, true)) {}
			mclkTicksBeforeTriggerTimeEXInt = advanceSessionEXInt.mclkTicksAdvanced;
		}
	}

	//Mask the horizontal and external interrupt events if a higher priority interrupt is
	//going to occur before them.
	foundTriggerTimeHInt &= (!foundTriggerTimeVInt || (mclkTicksBeforeTriggerTimeVInt > mclkTicksBeforeTriggerTimeHInt));
	foundTriggerTimeEXInt &= (!foundTriggerTimeVInt || (mclkTicksBeforeTriggerTimeVInt > mclkTicksBeforeTriggerTimeEXInt)) && (!foundTriggerTimeHInt || (mclkTicksBeforeTriggerTimeHInt > mclkTicksBeforeTriggerTimeEXInt));

	//Trigger pending IPL line state changes for each predicted interrupt time. Note that
	//it's possible for an interrupt to be masked, in which case it will not be
	//acknowledged. In this case, a higher priority interrupt may need to override that
	//interrupt later on, so we may need to assert more than one pending line state change
	//for the IPL lines.
	UpdateLineStateChangePrediction(LINE_IPL, vintIPLLineState, lineStateChangePendingVINT, lineStateChangeVINTMClkCountFromCurrent, lineStateChangeVINTTime, foundTriggerTimeVInt, mclkTicksBeforeTriggerTimeVInt, callingDevice, accessTime, accessContext);
	UpdateLineStateChangePrediction(LINE_IPL, hintIPLLineState, lineStateChangePendingHINT, lineStateChangeHINTMClkCountFromCurrent, lineStateChangeHINTTime, foundTriggerTimeHInt, mclkTicksBeforeTriggerTimeHInt, callingDevice, accessTime, accessContext);
	UpdateLineStateChangePrediction(LINE_IPL, exintIPLLineState, lineStateChangePendingEXINT, lineStateChangeEXINTMClkCountFromCurrent, lineStateChangeEXINTTime, foundTriggerTimeEXInt, mclkTicksBeforeTriggerTimeEXInt, callingDevice, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void S315_5313::UpdateLineStateChangePrediction(unsigned int lineNo, unsigned int lineStateChangeData, bool& lineStateChangePending, unsigned int& lineStateChangeMCLKCountdown, double& lineStateChangeTime, bool lineStateChangePendingNew, unsigned int lineStateChangeMCLKCountdownNew, IDeviceContext* callingDevice, double accessTime, unsigned int accessContext)
{
	//Adjust the calculated number of MCLK steps to the target to take into account unused
	//MCLK cycles from the last update step.
	if(lineStateChangeMCLKCountdownNew >= stateLastUpdateMclkUnused)
	{
		lineStateChangeMCLKCountdownNew -= stateLastUpdateMclkUnused;
	}
	else
	{
		lineStateChangeMCLKCountdownNew = 0;
	}

	//If we currently have a predicted line state change pending which doesn't match the
	//prediction we've just made, revoke the current pending line state change.
	if(lineStateChangePending && (!lineStateChangePendingNew || (lineStateChangeMCLKCountdown != lineStateChangeMCLKCountdownNew)))
	{
		//##DEBUG##
		if(outputInterruptDebugMessages)
		{
			std::wcout << "VDP - Revoked line state change: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << lineNo << '\t' << lineStateChangeData << '\t' << lineStateChangeMCLKCountdown << '\n';
		}

		memoryBus->RevokeSetLineState(lineNo, Data(GetLineWidth(lineNo), lineStateChangeData), lineStateChangeTime, GetDeviceContext(), callingDevice, accessTime, accessContext);
		lineStateChangePending = false;
	}

	//If we have a predicted line state change, and there's not currently a predicted line
	//state change pending, record information on the predicted change, and assert the
	//line.
	if(lineStateChangePendingNew && !lineStateChangePending)
	{
		lineStateChangePending = true;
		lineStateChangeMCLKCountdown = lineStateChangeMCLKCountdownNew;
		unsigned int lineStateChangeInMclkCycleTime = GetProcessorStateMclkCurrent() + lineStateChangeMCLKCountdownNew;
		lineStateChangeTime = ConvertMclkCountToAccessTime(lineStateChangeInMclkCycleTime);
		memoryBus->SetLineState(lineNo, Data(GetLineWidth(lineNo), lineStateChangeData), GetDeviceContext(), GetDeviceContext(), lineStateChangeTime, ACCESSCONTEXT_INTLINECHANGE);

		//##DEBUG##
		if(outputInterruptDebugMessages)
		{
			std::wcout << "VDP - Asserted conditional line state change: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << lineNo << '\t' << lineStateChangeData << '\t' << lineStateChangeMCLKCountdown << '\n';
		}
	}
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetClockSourceID(const std::wstring& clockSourceName) const
{
	if(clockSourceName == L"MCLK")
	{
		return CLOCK_MCLK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring S315_5313::GetClockSourceName(unsigned int clockSourceID) const
{
	switch(clockSourceID)
	{
	case CLOCK_MCLK:
		return L"MCLK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void S315_5313::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_MCLK)
	{
		clockMclkCurrent = clockRate;
	}

	//Since a clock rate change will affect our timing point calculations, trigger a
	//rollback.
	GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void S315_5313::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_MCLK)
	{
		clockMclkCurrent = clockRate;
	}
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::GetCELineID(const std::wstring& lineName, bool inputLine) const
{
	if(lineName == L"LDS")
	{
		return CELINE_LDS;
	}
	else if(lineName == L"UDS")
	{
		return CELINE_UDS;
	}
	else if(lineName == L"R/W")
	{
		return CELINE_RW;
	}
	else if(lineName == L"AS")
	{
		return CELINE_AS;
	}
	else if(lineName == L"RMWCycleInProgress")
	{
		return CELINE_RMWCYCLEINPROGRESS;
	}
	else if(lineName == L"RMWCycleFirstOperation")
	{
		return CELINE_RMWCYCLEFIRSTOPERATION;
	}
	else if(lineName == L"LWR")
	{
		return CELINE_LWR;
	}
	else if(lineName == L"UWR")
	{
		return CELINE_UWR;
	}
	else if(lineName == L"CAS0")
	{
		return CELINE_CAS0;
	}
	else if(lineName == L"RAS0")
	{
		return CELINE_RAS0;
	}
	else if(lineName == L"OE0")
	{
		return CELINE_OE0;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeInput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEINPROGRESS:
		ceLineMaskRMWCycleInProgress = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RMWCYCLEFIRSTOPERATION:
		ceLineMaskRMWCycleFirstOperation = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber)
{
	switch(lineID)
	{
	case CELINE_LDS:
		ceLineMaskLowerDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UDS:
		ceLineMaskUpperDataStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RW:
		ceLineMaskReadHighWriteLowOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_AS:
		ceLineMaskAddressStrobeOutput = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_LWR:
		ceLineMaskLWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_UWR:
		ceLineMaskUWR = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_CAS0:
		ceLineMaskCAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_RAS0:
		ceLineMaskRAS0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	case CELINE_OE0:
		ceLineMaskOE0 = !lineMapped? 0: 1 << lineStartBitNumber;
		break;
	}
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const
{
	bool vdpIsSource = (caller == GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, false, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const
{
	bool vdpIsSource = (caller == GetDeviceContext());
	bool currentLowerDataStrobe = (currentCELineState & ceLineMaskLowerDataStrobeInput) != 0;
	bool currentUpperDataStrobe = (currentCELineState & ceLineMaskUpperDataStrobeInput) != 0;
	bool operationIsWrite = (currentCELineState & ceLineMaskReadHighWriteLowInput) == 0;
	bool rmwCycleInProgress = (currentCELineState & ceLineMaskRMWCycleInProgress) != 0;
	bool rmwCycleFirstOperation = (currentCELineState & ceLineMaskRMWCycleFirstOperation) != 0;
	if(vdpIsSource)
	{
		currentLowerDataStrobe = true;
		currentUpperDataStrobe = true;
		operationIsWrite = false;
		rmwCycleInProgress = false;
		rmwCycleFirstOperation = false;
	}
	return BuildCELine(location, vdpIsSource, true, currentLowerDataStrobe, currentUpperDataStrobe, operationIsWrite, rmwCycleInProgress, rmwCycleFirstOperation);
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::BuildCELine(unsigned int targetAddress, bool vdpIsSource, bool transparentAccess, bool currentLowerDataStrobe, bool currentUpperDataStrobe, bool operationIsWrite, bool rmwCycleInProgress, bool rmwCycleFirstOperation) const
{
	//Calculate the state of all the various CE lines
	bool lineLWR = operationIsWrite && currentLowerDataStrobe;
	bool lineUWR = operationIsWrite && currentUpperDataStrobe;
	bool lineCAS0 = !operationIsWrite && ((!vdpIsSource && (targetAddress <= 0xDFFFFF)) || (vdpIsSource && (targetAddress <= 0xFFFFFF)));
	bool lineRAS0 = (!vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (vdpIsSource && (targetAddress >= 0x800000) && (targetAddress <= 0xFFFFFF));
	bool lineOE0 = (!operationIsWrite && !vdpIsSource && (targetAddress >= 0xE00000) && (targetAddress <= 0xFFFFFF)) || (!operationIsWrite && vdpIsSource && (targetAddress <= 0xFFFFFF));

	//If this bus operation is part of a Read-Modify-Write cycle, we need to handle it
	//slightly differently. The VDP asserts its CE lines when the AS line is asserted,
	//indicating that a valid address has been placed on the bus. Based on the state of
	//the address lines and other lines like R/W at that moment, the VDP determines the
	//state of its output CE lines, and asserts them. The VDP does NOT re-evaluate the
	//state of the CE lines over the course of the bus operation however, it simply holds
	//them at the state it determined when AS was first asserted. when AS is negated, the
	//VDP negates its CE lines. When the M68000 is driving the bus and executes a TAS
	//instruction however, this involves an indivisible read-modify-write cycle. A
	//read-modify-write cycle is exactly the same as a read followed by a write, except
	//that AS remains asserted through both the read and write, being asserted at the
	//start of the read, and negated when the write is complete. The effect this has on
	//the VDP is that the CE lines are frozen at the same state they would be for a read
	//throughout both the read and write cycles. In particular, this affects the LWR, UWR,
	//CAS0, and OE0 lines, which use the R/W line state to determine their output. This
	//can result in devices performing a second (ignored) read at the write phase of the
	//TAS instruction, essentially meaning the write phase of the TAS instruction is
	//ignored. This reportedly affects some games on the Sega Mega Drive (Gargoyles and
	//Ex-Mutants), which rely on the write phase of the TAS instruction failing. We
	//emulate this behaviour here, by caching the output CE line state on the read
	//component of a read-modify-write cycle, and retrieving the CE line state on the
	//write component. This is done through the use of two pseudo-CE lines from the
	//M68000, which indicate whether we are performing a read-modify-write cycle, and
	//whether this is the first operation of that cycle, respectively.
	if(rmwCycleInProgress && !transparentAccess)
	{
		if(rmwCycleFirstOperation)
		{
			lineLWRSavedStateRMW = lineLWR;
			lineUWRSavedStateRMW = lineUWR;
			lineCAS0SavedStateRMW = lineCAS0;
			lineRAS0SavedStateRMW = lineRAS0;
			lineOE0SavedStateRMW = lineOE0;
		}
		else
		{
			lineLWR = lineLWRSavedStateRMW;
			lineUWR = lineUWRSavedStateRMW;
			lineCAS0 = lineCAS0SavedStateRMW;
			lineRAS0 = lineRAS0SavedStateRMW;
			lineOE0 = lineOE0SavedStateRMW;
		}
	}

	//Build the actual CE line state based on the asserted CE lines
	unsigned int ceLineState = 0;
	ceLineState |= lineLWR? ceLineMaskLWR: 0x0;
	ceLineState |= lineUWR? ceLineMaskUWR: 0x0;
	ceLineState |= lineCAS0? ceLineMaskCAS0: 0x0;
	ceLineState |= lineRAS0? ceLineMaskRAS0: 0x0;
	ceLineState |= lineOE0? ceLineMaskOE0: 0x0;
	if(vdpIsSource)
	{
		//If the VDP has the bus, we need to drive the M68K CE lines ourselves.
		ceLineState |= ceLineMaskLowerDataStrobeOutput;
		ceLineState |= ceLineMaskUpperDataStrobeOutput;
		ceLineState |= ceLineMaskReadHighWriteLowOutput;
		ceLineState |= ceLineMaskAddressStrobeOutput;
	}

	//Return the generated CE line state
	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
//The real VDP does its own address line decoding, and doesn't have a dedicated CE line.
//The VDP has 23 address line inputs CA0-CA22, and it will respond to a read or write
//when the address lines match the following state: 110**000 ******** 000****. Note that
//this line decoding is based on how the VDP responds in the Mega Drive. It may be that
//some of the external pins allow the base address of the VDP to be adjusted by grounding
//or lifting certain pins, however if any pins perform this function, they are currently
//undocumented. Target register decoding is based on CA3-CA1:
//3210
//000* - Data Port
//001* - Control Port
//01** - HV Counter
//10** - SN76489 PSG
//110* - Unused
//111* - Test Register
//In our implementation, we allow the system to map the VDP to the bus as if it had a
//dedicated CE line, so this core can be made to respond to access on any address. Target
//register decoding assumes the lower 4 bits of the target address correspond with
//CA3-CA0.
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//Check that the access is targeting the VDP, not the embedded PSG.
	if((location & 0xC) != 0x8)
	{
		//##DEBUG##
		if(outputPortAccessDebugMessages)
		{
			std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceMclkCyclesRemainingTime << '\n';
		}

		//##DEBUG##
		if(commandCode.GetBit(5) && !dmd1)
		{
			std::wcout << "######################################################\n";
			std::wcout << "VDP ReadInterface called when a DMA transfer was in progress!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//##DEBUG##
		if(busGranted)
		{
			//outputPortAccessDebugMessages = true;
			std::wcout << "######################################################\n";
			std::wcout << "VDP ReadInterface called when busGranted was set!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0: //000 - Data Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Ensure we have a valid read target loaded
		if(!ValidReadTargetInCommandCode())
		{
			//##TODO## If we reach this point in code, we've attempted to read from an
			//invalid read target. This causes a lockup on the real hardware, as the
			//requested data never becomes available for reading. We should be logging a
			//critical error here.
		}
		else
		{
			//Advance the internal state until we have a data value buffered in the read
			//buffer.
			UpdateInternalState(accessMclkCycle, true, false, false, false, true, false, true);

			//Calculate any delay which occurred in processing this operation, and return
			//the delay time to the caller.
			accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
			if(accessResult.executionTime < 0.0)
			{
				//##DEBUG##
				std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
			}
		}

		//##DEBUG##
		//##TODO## Keep this check in at runtime, but log a hard error or assertion
		//failure.
		if(!readDataAvailable)
		{
			std::wcout << "VDP Error! Reached M5ReadFIFO when no valid data had been cached!\n";
			std::wcout << "\tCommandCode=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].codeRegData.GetData() << "\tAddress=0x" << std::hex << std::uppercase << fifoBuffer[fifoNextWriteEntry].addressRegData.GetData() << '\n';
		}

		//Now that we have a data value loaded into the read buffer, return it to the
		//caller.
		data = readBuffer;

		//Now that the cached data has been read from the buffer, the readDataAvailable
		//flag is cleared. This will cause the VDP to read the next word of data from the
		//read target on the next available access slot.
		readDataAvailable = false;
		readDataHalfCached = false;

		//Port monitor logging
		if(logDataPortRead)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"DP Read", caller->GetTargetDevice().GetDeviceInstanceName(), data.GetData(), accessTime, hcounter.GetData(), vcounter.GetData()));
		}
		break;
	case 1: //001 - Control Port
		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//Read the current value of the status register. Note that the status register is
		//a 10-bit register, so the call below will only affect the lower 10 bits of the
		//data value.
		data.SetLowerBits(status);

		//##DEBUG##
//		std::wcout << "VDP - ReadSR: 0x" << std::hex << std::uppercase << status.GetData() << '\n';

		//Hardware tests have shown that when reading the status register, only the lower
		//10 lines of the full 16 data lines are touched, corresponding with the 10-bit
		//status register value. The upper 6 lines are left floating. When the M68000
		//reads the status register from the VDP, this has the effect of the upper 6 data
		//lines retaining their previous values from the last bus read operation. We
		//emulate that behaviour here by using data line masking for the result.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = statusRegisterMask;

		//Since the status register has now been read, clear the sprite overflow and
		//collision flags.
		SetStatusFlagSpriteOverflow(false);
		SetStatusFlagSpriteCollision(false);

		//Port monitor logging
		if(logStatusRegisterRead)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"SR Read", caller->GetTargetDevice().GetDeviceInstanceName(), data.GetData(), accessTime, hcounter.GetData(), vcounter.GetData()));
		}
		break;
	case 2: //01* - HV Counter
	case 3:
		data = GetHVCounter();

		//Port monitor logging
		if(logHVCounterRead)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"HV Read", caller->GetTargetDevice().GetDeviceInstanceName(), data.GetData(), accessTime, hcounter.GetData(), vcounter.GetData()));
		}
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->ReadInterface(0, (location >> 1) & 0x1, data, caller, accessTime, accessContext);
		}
		break;
	case 6: //110 - Unused
		//##TODO## Perform hardware tests to confirm reads from this unused register
		//location don't assert any data lines.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0;
		break;
	case 7: //111 - Test Register
		//##TODO## Perform hardware tests to determine if reads from the test register
		//return valid data.
		accessResult.accessMaskUsed = true;
		accessResult.accessMask = 0;
		break;
	}

	//##DEBUG##
//	std::wcout << "VDPReadInterface:\t" << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessTime << '\n';

	return accessResult;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult S315_5313::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	IBusInterface::AccessResult accessResult(true);
	boost::mutex::scoped_lock lock(accessMutex);

	//Convert the access time into a cycle count relative to MCLK, rounding up the result
	//to the nearest MCLK cycle.
	unsigned int accessMclkCycle = ConvertAccessTimeToMclkCount(accessTime + lastTimesliceMclkCyclesRemainingTime);
	double accessMclkCycleInAccessTime = ConvertMclkCountToAccessTime(accessMclkCycle);
	if(accessMclkCycleInAccessTime < accessTime)
	{
		accessMclkCycle += 1;
	}

	//Check that the access is targeting the VDP, not the embedded PSG.
	//##TODO## Find a neater way to implement this
	if((location & 0xC) != 0x8)
	{
		//##DEBUG##
		if(outputPortAccessDebugMessages)
		{
			std::wcout << "VDPWriteInterface: " << std::hex << std::uppercase << location << '\t' << data.GetData() << '\t' << accessMclkCycle << '\t' << GetProcessorStateMclkCurrent() << '\t' << accessTime << '\t' << lastTimesliceMclkCyclesRemainingTime << '\n';
		}

		//This is a hack to handle VDP port access while a DMA transfer operation is
		//pending, but the bus hasn't yet been granted. This is probably not the correct
		//way the hardware would handle this kind of event. We have this hack in place to
		//work around a limitation in our current M68000 core, which isn't able to grant
		//the bus between two halves of a long-word operation. Until we have a microcode
		//level M68000 core online, we cache these invalid reads, and process them when
		//the DMA operation is complete.
		if(commandCode.GetBit(5) && !dmd1 && !busGranted)
		{
			//##DEBUG##
			if(outputPortAccessDebugMessages)
			{
				std::wcout << "VDP WriteInterface called while a bus request was pending! Caching the write.\n";
			}

			if(((location & 0xE) == 0) || ((location & 0xE) == 2))
			{
				dmaTransferInvalidPortWriteCached = true;
				dmaTransferInvalidPortWriteAddressCache = location;
				dmaTransferInvalidPortWriteDataCache = data;
			}
			return accessResult;
		}

		//##DEBUG##
		if(commandCode.GetBit(5) != GetStatusFlagDMA())
		{
			std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
			std::wcout << "VDP commandCode.GetBit(5) != GetStatusFlagDMA()\n";
			std::wcout << "__LINE__:\t" << __LINE__ << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		}

		//##DEBUG##
		if(commandCode.GetBit(5) && !dmd1)
		{
			std::wcout << "######################################################\n";
			std::wcout << "VDP WriteInterface called when a DMA transfer was in progress!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "data:\t" << data.GetData() << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//##DEBUG##
		if(busGranted && (caller != GetDeviceContext()))
		{
			//outputPortAccessDebugMessages = true;
			std::wcout << "######################################################\n";
			std::wcout << "VDP WriteInterface called when busGranted was set!\n";
			std::wcout << "location:\t" << location << "\n";
			std::wcout << "data:\t" << data.GetData() << "\n";
			std::wcout << "accessTime:\t" << accessTime << "\n";
			std::wcout << "accessMclkCycle:\t" << accessMclkCycle << "\n";
			std::wcout << "stateLastUpdateMclk:\t" << stateLastUpdateMclk << "\n";
			std::wcout << "stateLastUpdateMclkUnused:\t" << stateLastUpdateMclkUnused << "\n";
			std::wcout << "stateLastUpdateMclkUnusedFromLastTimeslice:\t" << stateLastUpdateMclkUnusedFromLastTimeslice << "\n";
			std::wcout << "busGranted:\t" << busGranted << "\n";
			std::wcout << "dmaEnabled:\t" << dmaEnabled << "\n";
			std::wcout << "commandCode:\t" << std::hex << commandCode.GetData() << "\t" << commandCode.GetBitCount() << "\t" << commandCode.GetBitMask() << "\n";
			std::wcout << "status:\t" << status.GetData() << "\n";
			std::wcout << "GetStatusFlagDMA:\t" << GetStatusFlagDMA() << "\n";
			std::wcout << "dmd1:\t" << dmd1 << "\n";
			std::wcout << "dmd0:\t" << dmd1 << "\n";
			std::wcout << "dmaFillOperationRunning:\t" << dmaFillOperationRunning << "\n";
			std::wcout << "######################################################\n";
		}

		//Trigger a system rollback if the device has been accessed out of order
		if(lastAccessTime > accessTime)
		{
			GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
		}
		lastAccessTime = accessTime;

		//Update the current VDP state to the target access time
		UpdateInternalState(accessMclkCycle, false, false, false, false, false, false, false);
	}

	//Decode the target address to determine which port was accessed
	switch((location >> 1) & 0x7)
	{
	case 0:{ //000 - Data Port
		//Port monitor logging
		if(logDataPortWrite)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"DP Write", caller->GetTargetDevice().GetDeviceInstanceName(), data.GetData(), accessTime, hcounter.GetData(), vcounter.GetData()));
		}

		//If the VDP was prepared to latch the next write to the control port as the
		//second half of a command data block, that state is cancelled when the data port
		//is read from or written to, or the control port is read.
		commandWritePending = false;

		//If the 4-word FIFO is full at the time a data port write is attempted, we
		//advance the device state forward one slot, until there's one space free in the
		//FIFO, and calculate the time during which the accessing device is stalled.
		UpdateInternalState(accessMclkCycle, true, false, false, true, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
		}

		//Save the code and address register data for this data port write
		if(codeAndAddressRegistersModifiedSinceLastWrite)
		{
			fifoBuffer[fifoNextWriteEntry].codeRegData = commandCode;
			fifoBuffer[fifoNextWriteEntry].addressRegData = commandAddress;
			codeAndAddressRegistersModifiedSinceLastWrite = false;
		}
		else
		{
			unsigned int fifoLastWriteEntry = (fifoNextWriteEntry+(fifoBufferSize-1)) % fifoBufferSize;
			fifoBuffer[fifoNextWriteEntry].codeRegData = fifoBuffer[fifoLastWriteEntry].codeRegData;
			fifoBuffer[fifoNextWriteEntry].addressRegData = fifoBuffer[fifoLastWriteEntry].addressRegData + autoIncrementData;
		}

		//Write the new data value to the FIFO
		//##TODO## Check if writes to invalid targets stay in the FIFO until an access
		//slot opens up for each one, at which point they are discarded, but the access
		//slot is consumed, or if they are discarded straight away and the next pending
		//write processed. Most likely, a write to an invalid target consumes an access
		//slot, but we should test by examining the FIFO full/empty flags to be sure.
		fifoBuffer[fifoNextWriteEntry].dataPortWriteData = data;
		fifoBuffer[fifoNextWriteEntry].dataWriteHalfWritten = false;
		fifoBuffer[fifoNextWriteEntry].pendingDataWrite = true;

		//Advance the FIFO to the next slot
		fifoNextWriteEntry = (fifoNextWriteEntry+1) % fifoBufferSize;

		//Update the FIFO full and empty flags in the status register
		bool fifoEmpty = IsWriteFIFOEmpty();
		bool fifoFull = IsWriteFIFOFull();
		SetStatusFlagFIFOEmpty(fifoEmpty);
		SetStatusFlagFIFOFull(fifoFull);

		//##TODO## I believe performing a write to a read target prevents subsequent read
		//operations from succeeding. Do more testing to determine if this is actually due
		//to CD4 being set, or rather, some flag or status indicating that data is
		//available for reading being cleared. By performing tests with the write,
		//previous read cache operation, and subsequent read in different states, we
		//should be able to gather more information. If at any point two successive read
		//operations can be made to work after an invalid port write, CD4 cannot be set by
		//a data port write.
		commandCode.SetBit(4, true);

		//##FIX## This comment is incorrect. We didn't just write over it.
		//Now that we've added a new write to the FIFO, we invalidate any cached read data
		//that may have been buffered, since if it existed, we just wrote over it.
		readDataAvailable = false;
		readDataHalfCached = false;
		break;}
	case 1:{ //001 - Control Port
		//Port monitor logging
		if(logControlPortWrite)
		{
			RecordPortMonitorEntry(PortMonitorEntry(L"CP Write", caller->GetTargetDevice().GetDeviceInstanceName(), data.GetData(), accessTime, hcounter.GetData(), vcounter.GetData()));
		}

		//It is almost certain that control port writes are not processed until the FIFO
		//is empty, otherwise the changes which are made to the the command code and
		//address register would interfere with pending writes held in the FIFO. See the
		//thread at http://gendev.spritesmind.net/forum/viewtopic.php?t=790 for
		//discussion. We emulate that here by forcing control port writes to wait until
		//the FIFO is empty.
		//##FIX## Hardware tests have now shown this assumption to be incorrect. Data port
		//writes and control port writes can both be made regardless of the current state
		//of the FIFO, so each entry in the FIFO must somehow cache its write address and
		//write target. Also note that control port writes don't seem to delay the
		//progression of the FIFO, and we have specifically tested a FIFO that had half
		//its writes going to one write target, and half the other writes going to another
		//write target, at different addresses, with the FIFO full and a full control port
		//write while the FIFO was not empty, followed by data port writes in order to set
		//all this up. All data was written correctly and was verified by reading it back.
		//This clearly demonstrates that the write FIFO must latch the current target
		//address and code register data, or at the very least, the write target from the
		//code register data. Writing a read target to the code register has no apparent
		//effect, but performing a data port read does fully flush the FIFO before
		//returning the read data. After a data port read, the FIFO is always empty.
//		UpdateInternalState(accessMclkCycle, true, true, false, false, false, true);

		//Calculate any delay which occurred in processing this operation, and return the
		//delay time to the caller.
		accessResult.executionTime += ((lastTimesliceMclkCyclesRemainingTime + GetProcessorStateTime()) - accessTime);
		if(accessResult.executionTime < 0.0)
		{
			//##DEBUG##
			std::wcout << "VDP negative executionTime: " << accessResult.executionTime << '\t' << GetProcessorStateTime() << '\t' << lastTimesliceMclkCyclesRemainingTime << '\t' << accessTime << '\n';
		}

		//Calculate the number of mclk cycles that this operation was delayed
		unsigned int accessMclkCycleDelay = GetProcessorStateMclkCurrent() - accessMclkCycle;

		//##FIX## Testing has indicated that control port writes are ignored while DMA
		//operations are in progress. Note that this does not simply mean when CD5 is set
		//however, as control port writes can be made while a DMA fill operation is setup,
		//but waiting for a data port write to trigger it, and we know from current
		//testing that the DMA busy flag reads as set after a DMA fill has been setup, but
		//before the fill data is written.
		//##TODO## Confirm that control port writes can be made while a DMA fill operation
		//is waiting for a data port write
		//##TODO## Check if the DMA busy flag remains set if a partial control port write
		//is made while a DMA fill operation is waiting for a data port write, in
		//particular when we write the same data over again for the first command word. If
		//it does not, it would appear that CD5 is cleared when a partial control port
		//write is made.
		bool assertBRLine = false;
		if(!commandWritePending)
		{
			ProcessCommandDataWriteFirstHalf(data);

			//Once a partial control port write has been made, hardware tests indicate
			//that CD4 is set. This occurs even if the partial control port write is
			//interpreted as a register write. CD4 is most likely set at this time for
			//performance reasons. When CD4 is set, no read targets are valid. If a valid
			//read target could be setup by a partial control port write, read pre-caching
			//could begin between the two halves of a full control port write, which would
			//be a bad thing if the indicated read target after the first control port
			//write was not actually what the caller wanted to access.
			commandCode.SetBit(4, true);

			//Contrary to other documentation, and the behaviour of other emulators,
			//register writes go directly to the code and address registers first as a
			//normal command word write. This is the cause of the apparent "clearing" of
			//the code register when a register write occurs, as reported by Charles
			//MacDonald. The register write is then processed from the new address
			//register data. Hardware tests have shown the upper bits of the command and
			//address registers remain unaffected.
			if(commandCode.GetBit(1) && !commandCode.GetBit(0))
			{
				//Register Write
				//-----------------------------------------------------------------
				//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
				//|---------------------------------------------------------------|
				//| 1 | 0 | / |    Register No    |             Data              |
				//|---------------------------------------------------------------|
				//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
				//-----------------------------------------------------------------

				//Decode the target register number and data from the command address data
				unsigned int registerNo = originalCommandAddress.GetDataSegment(8, 5);
				Data registerData(8, originalCommandAddress.GetDataSegment(0, 8));

				//Calculate the highest accessible register number available based on the
				//current screen mode. Documentation and hardware tests show that when
				//mode 4 is active, VDP registers above the register block defined under
				//mode 4 are inaccessible. See "Genesis Software Manual", "Addendum 4" for
				//further information.
				unsigned int accessibleRegisterCount = registerCount;
				if(!screenModeM5Cached)
				{
					accessibleRegisterCount = registerCountM4;
				}

				//If the target register is accessible in this video mode and isn't
				//currently locked, perform the register write.
				if((registerNo < accessibleRegisterCount) && !rawRegisterLocking[registerNo])
				{
					RegisterSpecialUpdateFunction(accessMclkCycle + accessMclkCycleDelay, accessTime, accessResult.executionTime, caller, accessContext, registerNo, registerData);
					AccessTarget accessTarget;
					accessTarget.AccessTime(accessMclkCycle + accessMclkCycleDelay);
					SetRegisterData(registerNo, accessTarget, registerData);

					//Fix any locked registers at their set value
					boost::mutex::scoped_lock lock2(registerLockMutex);
					for(std::map<unsigned int, std::wstring>::const_iterator i = lockedRegisterState.begin(); i != lockedRegisterState.end(); ++i)
					{
						WriteGenericData(i->first, 0, i->second);
					}
				}

				//Since the command word has just been latched as a register write, the
				//VDP is no longer expecting a second command word to be written.
				commandWritePending = false;
			}
		}
		else
		{
			ProcessCommandDataWriteSecondHalf(data);

			//Check if we need to obtain the bus due to an external DMA transfer operation
			if(commandCode.GetBit(5) //The command code has a DMA operation flagged
			&& !dmd1)                //DMA is set to perform an external copy operation
			{
				//Note that a DMA operation will be triggered regardless of the state of
				//CD4, as this bit isn't tested when deciding whether to trigger DMA, and
				//write target decoding only looks at the state of CD3-CD0.
				assertBRLine = true;

				//Set the initial DMA transfer register settings
				boost::mutex::scoped_lock lock(workerThreadMutex);
				dmaTransferActive = true;
				dmaTransferReadDataCached = false;
				//Note that we technically don't need to set these here, as they are only
				//correctly initialized once the bus request is granted, but we set them
				//here anyway as it can be useful for debugging purposes to know when the
				//DMA operation was triggered relative to when the bus was granted.
				dmaTransferNextReadMclk = accessMclkCycle;
				dmaTransferLastTimesliceUsedReadDelay = 0;

				//##DEBUG##
//				if(outputPortAccessDebugMessages)
//				{
//					std::wcout << "SetLineState - VDP_LINE_BR:\t" << true << '\t' << GetProcessorStateTime() << '\t' << GetProcessorStateMclkCurrent() << '\n';
//				}
			}
		}

		//Hardware tests have shown that the command address is cached internally,
		//separate from the command address register that gets written to on a command
		//port write. When the command address is incremented during read and write
		//operations, it is this internal copy of the command address which gets
		//incremented. Even doing a partial command port write will cause the internal
		//command address register to be fully reloaded from the original command address.
		//Even if the upper 2 bits of the command address have been changed since the
		//original address was setup, they will be reloaded on a partial command port
		//write. We emulate that behaviour here by keeping a separate copy of the command
		//address in its original, unmodified state.
		//##TODO## Recent hardware tests dispute these findings. Do more testing to
		//determine what the correct behaviour is.
		commandAddress = originalCommandAddress;

		//Flag that the command and address registers have been modified since the last
		//write. This is important in order to ensure that subsequent data port writes
		//latch the new data, rather than incrementing the write address from the previous
		//write target.
		codeAndAddressRegistersModifiedSinceLastWrite = true;

		//Now that the command code has changed, any cached read data is invalidated. We
		//clear this flag here now so that new data will be retrieved for the target on
		//the next available access slot, if it is a valid read target.
		readDataAvailable = false;
		readDataHalfCached = false;

		//If a request has been made to assert the BR line, release any critical locks
		//we're currently holding, and assert the line here.
		if(assertBRLine)
		{
			//Note that at this point, the VDP will request the bus. If the bus request is
			//granted, the DMA worker thread will go active to process the transfer. When
			//the transfer is completed, the VDP will release the bus.
			//##TODO## Determine what happens if the bus request is not granted straight
			//away. IE, What happens if another control port write occurs before the bus
			//is granted? Does the VDP process the write, or does it lock up until the bus
			//is granted? We can test this by physically preventing the VDP from asserting
			//the BR line, so that the M68000 never grants the bus, and see how the VDP
			//responds if we continue to access it.
			busRequestLineState = true;
			memoryBus->SetLineState(LINE_BR, Data(GetLineWidth(LINE_BR), (unsigned int)busRequestLineState), GetDeviceContext(), GetDeviceContext(), accessTime + accessResult.executionTime, ACCESSCONTEXT_BR_ASSERT);
		}
		break;}
	case 2: //01* - HV Counter
	case 3:
		break;
	case 4: //10* - SN76489 PSG
	case 5:
		if(psg != 0)
		{
			return psg->WriteInterface(0, (location >> 1) & 0x1, data, caller, accessTime, accessContext);
		}
		break;
	case 6: //110 - Unused
		break;
	case 7: //111 - Test Register
		//##TODO## Implement test register writes
		break;
	}

	return accessResult;
}

//----------------------------------------------------------------------------------------
Data S315_5313::GetHVCounter() const
{
	//Since the status register has already been updated when the device was accessed,
	//build the HV counter based on the current processor state. Note the implementation
	//of HV counter latching we have below. Hardware tests have shown that the current
	//value of the HV counter is latched at the time the HV counter latch bit is enabled,
	//and the returned value from the HV counter only changes while the bit is set if the
	//HL line is asserted, latching the current HV counter value at that time. This means
	//we always return the latched HV counter data if the HV counter latch bit is enabled.
	//The contents of this register are updated when the HL line triggers, or when the HV
	//counter latch bit is first set.

	//Build the 8-bit external hcounter
	//The internal hcounter is 9-bit, and the external hcounter is 8-bit. The upper 8 bits
	//of the 9-bit internal hcounter are used to build the 8-bit external hcounter, as
	//shown below:
	//Internal counter: 876543210
	//External counter: 87654321
	Data currentHCounter(9);
	currentHCounter = (hvCounterLatchEnabled)? hcounterLatchedData: hcounter;
	Data externalHCounter(8);
	externalHCounter = currentHCounter.GetUpperBits(8);

	//Build the 8-bit external vcounter
	//The internal vcounter is 9-bit, and the external vcounter is 8-bit. The way the
	//internal counter is mapped to the external counter depends on the interlace mode.
	//Given that the internal vcounter is 9-bit, the way the bits are mapped from the
	//internal counter to the external counter is given below:
	//Internal counter:           876543210
	//External, no Interlacing:    76543210
	//External, interlace normal:  76543218
	//External, interlace double:  65432107
	Data currentVCounter(9);
	currentVCounter = (hvCounterLatchEnabled)? vcounterLatchedData: vcounter;
	Data externalVCounter(8);
	if(interlaceEnabled && interlaceDouble)
	{
		currentVCounter <<= 1;
	}
	externalVCounter = currentVCounter.GetLowerBits(8);
	if(interlaceEnabled)
	{
		externalVCounter.SetBit(0, currentVCounter.GetBit(8));
	}

	//Build and return the combined 16-bit external HV counter
	//-----------------------------------------------------------------
	//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------|-------------------------------|
	//|   External V counter (8-bit)  |   External H counter (8-bit)  |
	//|---------------------------------------------------------------|
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|               External HV counter (16-bit)                    |
	//-----------------------------------------------------------------
	Data externalHVCounter(16);
	externalHVCounter.SetUpperHalf(externalVCounter);
	externalHVCounter.SetLowerHalf(externalHCounter);

	//##DEBUG##
	//std::wcout << "VDP - ReadHVCounter: 0x" << std::hex << std::uppercase << externalHVCounter.GetData() << '\n';

	return externalHVCounter;
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteFirstHalf(const Data& data)
{
	//Command data first half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//| Code  |                    Address                            |
	//|-------|-------------------------------------------------------|
	//|CD1|CD0|A13|A12|A11|A10|A9 |A8 |A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |
	//-----------------------------------------------------------------
	commandWritePending = true;
	commandCode.SetDataSegment(0, 2, data.GetDataSegment(14, 2));
	originalCommandAddress.SetDataSegment(0, 14, data.GetDataSegment(0, 14));
}

//----------------------------------------------------------------------------------------
void S315_5313::ProcessCommandDataWriteSecondHalf(const Data& data)
{
	//Command data second half
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|                               |     Code      |       |Address|
	//| /   /   /   /   /   /   /   / |---------------| /   / |-------|
	//|                               |CD5|CD4|CD3|CD2|       |A15|A14|
	//-----------------------------------------------------------------

	//Hardware tests have shown that when the DMA enable bit is cleared, the state of CD5
	//is unable to be modified by a control port write. The current state of CD5 is
	//retained, regardless of the new value of CD5 written in the second command port
	//write. This has been verified through hardware testing. We can verify that CD5
	//cannot be changed from clear to set by attempting to setup a valid DMA operation
	//while DMA is disabled, then enabling DMA operations and observing that the DMA
	//operation does not trigger. We can verify that CD5 cannot be changed from set to
	//clear by setting up a DMA fill operation, but not performing the data port write
	//immediately, but instead doing a register write to disable DMA operations, then
	//rewriting the first control word for the DMA fill. We can observe that the DMA busy
	//flag in the status register remains set after disabling DMA operations, and that the
	//DMA fill is still able to be triggered by writing to the data port. Additionally,
	//writing a new second command word where CD5 is clear doesn't stop the DMA fill
	//operation from being triggered when the data port is written to. Also note that this
	//shows that all the DMA enable bit does is prevent CD5 from being modified, it does
	//not actually disable DMA operations or affect the way a currently executing DMA
	//operation advances.
	unsigned int commandCodeBitsToSet = (dmaEnabled)? 4: 3;

	commandWritePending = false;
	commandCode.SetDataSegment(2, commandCodeBitsToSet, data.GetDataSegment(4, commandCodeBitsToSet));
	originalCommandAddress.SetDataSegment(14, 2, data.GetDataSegment(0, 2));

	//Keep the state of CD5 and the DMA flag in the status register in sync. On the real
	//hardware, the DMA flag in the status register is the literal realtime state of the
	//CD5 bit in the command code register. Note that this means that the DMA bit in the
	//status register is set when a control port write has been made to setup a DMA fill
	//operation, but the data port write has not yet been made to trigger the fill. This
	//is the way the real VDP behaves, and this has been confirmed though hardware tests.
	//Although the status register returns the realtime state of CD5 for the DMA busy
	//flag, we keep the two separated in our core for maximum efficiency on status
	//register reads, and keep them manually in sync whenever the CD5 bit is modified.
	SetStatusFlagDMA(commandCode.GetBit(5));
}

//----------------------------------------------------------------------------------------
void S315_5313::TransparentRegisterSpecialUpdateFunction(unsigned int registerNo, const Data& data)
{
	switch(registerNo)
	{
	case 0:
		hvCounterLatchEnabled = data.GetBit(1);
		hintEnabled = data.GetBit(4);
		break;
	case 1:
		screenModeM5Cached = data.GetBit(2);
		screenModeV30Cached = data.GetBit(3);
		dmaEnabled = data.GetBit(4);
		displayEnabledCached = data.GetBit(6);
		vintEnabled = data.GetBit(5);
		break;
	case 5:
		spriteAttributeTableBaseAddressDecoded = (data.GetDataSegment(0, 7) << 9);
		break;
	case 10:
		hintCounterReloadValue = data.GetData();
		break;
	case 11:
		verticalScrollModeCached = data.GetBit(2);
		exintEnabled = data.GetBit(3);
		break;
	case 12:
		screenModeRS0Cached = data.GetBit(7);
		screenModeRS1Cached = data.GetBit(0);
		interlaceDoubleCached = data.GetBit(2);
		interlaceEnabledCached = data.GetBit(1);
		break;
	case 15:
		autoIncrementData = data.GetData();
		break;
	case 19:
		dmaLengthCounter = (dmaLengthCounter & 0xFF00) | data.GetData();
		break;
	case 20:
		dmaLengthCounter = (dmaLengthCounter & 0x00FF) | (data.GetData() << 8);
		break;
	case 21:
		dmaSourceAddressByte1 = data.GetData();
		break;
	case 22:
		dmaSourceAddressByte2 = data.GetData();
		break;
	case 23:
		dmaSourceAddressByte3 = data.GetDataSegment(0, 7);
		dmd0 = data.GetBit(6);
		dmd1 = data.GetBit(7);
		break;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::RegisterSpecialUpdateFunction(unsigned int mclkCycle, double accessTime, double accessDelay, IDeviceContext* caller, unsigned int accessContext, unsigned int registerNo, const Data& data)
{
	switch(registerNo)
	{
	case 0:
		if(hvCounterLatchEnabled != data.GetBit(1))
		{
			//If the HV counter latch enable bit has changed from 0 to 1, latch the
			//current value of the HV counter as the latched HV counter data. Note that
			//hardware tests have shown that this only occurs when the HV latch enable bit
			//changes from 0 to 1, not when it is set to 1 again when it is already set,
			//and writes to other registers appear to have no effect on the latched data.
			//Note also that in hardware, this is most likely implemented simply by having
			//an internal HV counter and an external HV counter, and the external HV
			//counter is only updated with the internal contents when this register bit is
			//set to 1, or the HL line is toggled.
			hvCounterLatchEnabled = data.GetBit(1);
			hcounterLatchedData = hcounter;
			vcounterLatchedData = vcounter;
		}
		if(hintEnabled != data.GetBit(4))
		{
			//##DEBUG##
			if(outputInterruptDebugMessages)
			{
				std::wcout << "VDP - HINT enable state changed: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << hintEnabled << '\t' << data.GetBit(4) << '\n';
			}

			hintEnabled = data.GetBit(4);
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		break;
	case 1:
		screenModeM5Cached = data.GetBit(2);
		screenModeV30Cached = data.GetBit(3);
		dmaEnabled = data.GetBit(4);
		displayEnabledCached = data.GetBit(6);

		if(vintEnabled != data.GetBit(5))
		{
			//##DEBUG##
			if(outputInterruptDebugMessages)
			{
				std::wcout << "VDP - VINT enable state changed: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << vintEnabled << '\t' << data.GetBit(5) << '\n';
			}

			vintEnabled = data.GetBit(5);
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		break;
	case 5:
		//Handle changes to the sprite attribute (mapping) table base address register
		//affecting which data is written to the sprite cache.
		//##TODO## Perform hardware tests to confirm how we should be handling the AT9 bit
		//from the attribute table address. Apparently, this bit "should be 0 in H40 cell
		//mode", according to the official documentation, but there's a lot of room for
		//interpretation as to what should happen with that. If AT9 is set in H40 mode,
		//is the sprite cache centered around the address as if AT9 is set, or if it is
		//cleared? Is the sprite data from VRAM read as if AT9 is set, or cleared? Can a
		//register change to the sprite attribute table address cause pending writes in
		//the FIFO to miss being written to the sprite cache, or does the sprite cache
		//grab writes from the FIFO before they are written to VRAM? Does a H32/H40 screen
		//mode change have the same effect as changing the sprite table base address
		//register? If the screen mode is H32, are writes past the end of the maximum
		//sprite table size in H32 mode (0x200 bytes), but below the maximum table size in
		//H40 mode (0x280) stored in the cache, or are they ignored? If they're ignored,
		//does that mean that writing a H40 sprite table in H32 mode, then enabling H40
		//mode results in the sprite cache retaining whatever data it previously held in
		//the upper region of the cache? Does switching from H40 mode to H32 mode, then
		//back to H40 mode, result in the upper bytes of the sprite cache being cleared,
		//or do they retain their previous values? Hardware tests are definitely required
		//here.
		spriteAttributeTableBaseAddressDecoded = (data.GetDataSegment(0, 7) << 9);
		break;
	case 10:
		if(hintCounterReloadValue != data.GetData())
		{
			//##DEBUG##
			if(outputInterruptDebugMessages)
			{
				std::wcout << "VDP - HCounter data changed: " << hcounter.GetData() << '\t' << vcounter.GetData() << '\t' << hintCounterReloadValue << '\t' << data.GetData() << '\n';
			}

			hintCounterReloadValue = data.GetData();
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		break;
	case 11:
		verticalScrollModeCached = data.GetBit(2);
		if(exintEnabled != data.GetBit(3))
		{
			exintEnabled = data.GetBit(3);
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		break;
	case 12:
		if(screenModeRS0Cached != data.GetBit(7))
		{
			screenModeRS0Cached = data.GetBit(7);
			//If the RS0 bit has changed, we need to trigger a rollback, since any change
			//to this register will almost certainly affect when the next VDP timing point
			//event occurs.
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		if(screenModeRS1Cached != data.GetBit(0))
		{
			screenModeRS1Cached = data.GetBit(0);
			//If the RS1 bit has changed, we need to trigger a rollback, since any change
			//to this register will almost certainly affect when the next VDP timing point
			//event occurs.
			UpdatePredictedLineStateChanges(caller, accessTime + accessDelay, accessContext);
		}
		interlaceDoubleCached = data.GetBit(2);
		interlaceEnabledCached = data.GetBit(1);
		break;
	case 15:
		autoIncrementData = data.GetData();
		break;
	case 19:
		dmaLengthCounter = (dmaLengthCounter & 0xFF00) | data.GetData();
		break;
	case 20:
		dmaLengthCounter = (dmaLengthCounter & 0x00FF) | (data.GetData() << 8);
		break;
	case 21:
		dmaSourceAddressByte1 = data.GetData();
		break;
	case 22:
		dmaSourceAddressByte2 = data.GetData();
		break;
	case 23:
		dmaSourceAddressByte3 = data.GetDataSegment(0, 7);
		dmd0 = data.GetBit(6);
		dmd1 = data.GetBit(7);
		break;
	}
}

//----------------------------------------------------------------------------------------
//Port monitor functions
//----------------------------------------------------------------------------------------
void S315_5313::RecordPortMonitorEntry(const PortMonitorEntry& entry)
{
	boost::mutex::scoped_lock lock(portMonitorMutex);
	portMonitorList.push_front(entry);
	while(portMonitorList.size() > portMonitorListSize)
	{
		portMonitorList.pop_back();
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::ClearPortMonitorList()
{
	boost::mutex::scoped_lock lock(portMonitorMutex);
	portMonitorList.clear();
}
