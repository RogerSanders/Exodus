#include "S315_5313.h"

//----------------------------------------------------------------------------------------
//Rendering functions
//----------------------------------------------------------------------------------------
void S315_5313::RenderThread()
{
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Start the render loop
	bool done = false;
	while(!done)
	{
		//Obtain a copy of the latest completed timeslice period
		bool renderTimesliceObtained = false;
		TimesliceRenderInfo timesliceRenderInfo;
		{
			boost::mutex::scoped_lock timesliceLock(timesliceMutex);

			//If there is at least one render timeslice pending, grab it from the queue.
			//##FIX## Our pendingRenderOperationCount doesn't really work the way we want.
			//Currently, here in the render thread, we decrement it once for each
			//timeslice received, but we don't increment it once for each timeslice.
			//##FIX## This problem with pendingRenderOperationCount is now breaking our
			//PSG and YM2612 cores, since the render thread marks itself as lagging when
			//it's not, and the execute thread waits forever for it to fix itself. We need
			//to get this working, and replicate it in the PSG and YM2612 cores.
//			if((pendingRenderOperationCount > 0) && !regTimesliceList.empty() && !vramTimesliceList.empty() && !cramTimesliceList.empty() && !vsramTimesliceList.empty())
			if(!regTimesliceList.empty() && !vramTimesliceList.empty() && !cramTimesliceList.empty() && !vsramTimesliceList.empty() && !spriteCacheTimesliceList.empty())
			{
				//Update the lagging state for the render thread
				--pendingRenderOperationCount;
				renderThreadLagging = (pendingRenderOperationCount > maxPendingRenderOperationCount);
				renderThreadLaggingStateChange.notify_all();

				//Grab the next completed timeslice from the timeslice list
				timesliceRenderInfo = *timesliceRenderInfoList.begin();
				regTimesliceCopy = *regTimesliceList.begin();
				vramTimesliceCopy = *vramTimesliceList.begin();
				cramTimesliceCopy = *cramTimesliceList.begin();
				vsramTimesliceCopy = *vsramTimesliceList.begin();
				spriteCacheTimesliceCopy = *spriteCacheTimesliceList.begin();
				timesliceRenderInfoList.pop_front();
				regTimesliceList.pop_front();
				vramTimesliceList.pop_front();
				cramTimesliceList.pop_front();
				vsramTimesliceList.pop_front();
				spriteCacheTimesliceList.pop_front();

				//Flag that we managed to obtain a render timeslice
				renderTimesliceObtained = true;
			}
		}

		//If no render timeslice was available, we need to wait for a thread suspension
		//request or a new timeslice to be received, then begin the loop again.
		if(!renderTimesliceObtained)
		{
			//If the render thread has not already been instructed to stop, suspend this
			//thread until a timeslice becomes available or this thread is instructed to
			//stop.
			if(renderThreadActive)
			{
				renderThreadUpdate.wait(lock);
			}

			//If the render thread has been suspended, flag that we need to exit this
			//render loop.
			done = !renderThreadActive;

			//Begin the loop again
			continue;
		}

		//Begin advance sessions for each of our timed buffers
		reg.BeginAdvanceSession(regSession, regTimesliceCopy, false);
		vram->BeginAdvanceSession(vramSession, vramTimesliceCopy, false);
		cram->BeginAdvanceSession(cramSession, cramTimesliceCopy, true);
		vsram->BeginAdvanceSession(vsramSession, vsramTimesliceCopy, false);
		spriteCache->BeginAdvanceSession(spriteCacheSession, spriteCacheTimesliceCopy, false);

		//Calculate the number of cycles to advance in this update step, and reset the
		//current advance progress through this timeslice.
		unsigned int mclkCyclesToAdvance = timesliceRenderInfo.timesliceEndPosition - timesliceRenderInfo.timesliceStartPosition;
		renderDigitalMclkCycleProgress = timesliceRenderInfo.timesliceStartPosition;

		if(!videoDisableRenderOutput)
		{
			//##DEBUG##
			if(outputTimingDebugMessages)
			{
				std::wcout << "VDPRenderThreadAdvance(Before): " << renderDigitalHCounterPos << '\t' << renderDigitalVCounterPos << '\t' << renderDigitalMclkCycleProgress << '\t' << renderDigitalRemainingMclkCycles << '\t' << timesliceRenderInfo.timesliceEndPosition << '\t' << mclkCyclesToAdvance << '\t' << renderDigitalScreenModeRS0Active << '\t' << renderDigitalScreenModeRS1Active << '\n';
			}

			//Advance the digital render process
			AdvanceRenderProcess(mclkCyclesToAdvance);

			//##DEBUG##
			if(outputTimingDebugMessages)
			{
				std::wcout << "VDPRenderThreadAdvance(After): " << renderDigitalHCounterPos << '\t' << renderDigitalVCounterPos << '\t' << renderDigitalMclkCycleProgress << '\t' << renderDigitalRemainingMclkCycles << '\t' << timesliceRenderInfo.timesliceEndPosition << '\t' << mclkCyclesToAdvance << '\t' << renderDigitalScreenModeRS0Active << '\t' << renderDigitalScreenModeRS1Active << '\n';
			}
		}

		//Advance past the timeslice we've just rendered from
		{
			//##TODO## I don't think we need this lock here anymore. Confirm that we can
			//remove it.
			boost::mutex::scoped_lock timesliceLock(timesliceMutex);
			reg.AdvancePastTimeslice(regTimesliceCopy);
			vram->AdvancePastTimeslice(vramTimesliceCopy);
			cram->AdvancePastTimeslice(cramTimesliceCopy);
			vsram->AdvancePastTimeslice(vsramTimesliceCopy);
			spriteCache->AdvancePastTimeslice(spriteCacheTimesliceCopy);
			vram->FreeTimesliceReference(vramTimesliceCopy);
			cram->FreeTimesliceReference(cramTimesliceCopy);
			vsram->FreeTimesliceReference(vsramTimesliceCopy);
			spriteCache->FreeTimesliceReference(spriteCacheTimesliceCopy);
		}
	}
	renderThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void S315_5313::AdvanceRenderProcess(unsigned int mclkCyclesRemainingToAdvance)
{
	//Create our reg buffer access target. Since we're working with the committed state,
	//access will be very fast, so there's no need to worry about caching settings.
	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Get the hscan and vscan settings for this scanline
	const HScanSettings* hscanSettings = &GetHScanSettings(renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);
	const VScanSettings* vscanSettings = &GetVScanSettings(renderDigitalScreenModeV30Active, renderDigitalPalModeActive, renderDigitalInterlaceEnabledActive);

	//Combine any remaining mclk cycles from the previous render update step into this
	//update step.
	mclkCyclesRemainingToAdvance += renderDigitalRemainingMclkCycles;

	//Advance until we've consumed all update cycles
	while(mclkCyclesRemainingToAdvance > 0)
	{
		//Advance the register buffer up to the current time. Register changes can occur
		//at any time, so we need to ensure this buffer is always current.
		reg.AdvanceBySession(renderDigitalMclkCycleProgress, regSession, regTimesliceCopy);

		//If we've reached a point where horizontal screen mode settings need to be
		//latched, cache the new settings now.
		if(renderDigitalHCounterPos == hscanSettings->hblankSetPoint)
		{
			//Cache the new settings
			renderDigitalScreenModeRS0Active = M5GetRS0(accessTarget);
			renderDigitalScreenModeRS1Active = M5GetRS1(accessTarget);

			//Latch updated screen mode settings
			hscanSettings = &GetHScanSettings(renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);
		}

		//If we've reached a point where vertical screen mode settings need to be latched,
		//cache the new settings now.
		if((renderDigitalVCounterPos == vscanSettings->vblankSetPoint) && (renderDigitalHCounterPos == hscanSettings->vcounterIncrementPoint))
		{
			//Cache the new settings
			renderDigitalScreenModeV30Active = M5GetV30CellModeEnabled(accessTarget);
			renderDigitalInterlaceEnabledActive = M5GetLSM0(accessTarget);
			renderDigitalInterlaceDoubleActive = M5GetLSM1(accessTarget);
			//##FIX## This is incorrect. This is retrieving the current live state of the
			//line. We need to store history information for this line, so that the
			//correct line state can be set here.
			renderDigitalPalModeActive = palMode;

			//Latch updated screen mode settings
			vscanSettings = &GetVScanSettings(renderDigitalScreenModeV30Active, renderDigitalPalModeActive, renderDigitalInterlaceEnabledActive);
		}

		//Calculate the number of mclk cycles required to advance the render process one
		//pixel clock step
		unsigned int mclkTicksForNextPixelClockTick;
		mclkTicksForNextPixelClockTick = GetMclkTicksForPixelClockTicks(*hscanSettings, 1, renderDigitalHCounterPos, renderDigitalScreenModeRS0Active, renderDigitalScreenModeRS1Active);

		//Advance a complete pixel clock step if we are able to complete it in this update
		//step, otherwise store the remaining mclk cycles, and terminate the loop.
		if(mclkCyclesRemainingToAdvance >= mclkTicksForNextPixelClockTick)
		{
			//Perform any digital render operations which need to occur on this cycle
			UpdateDigitalRenderProcess(accessTarget, *hscanSettings, *vscanSettings);

			//Perform any analog render operations which need to occur on this cycle
			UpdateAnalogRenderProcess(accessTarget, *hscanSettings, *vscanSettings);

			//If we're about to increment the vcounter, save the current value of it
			//before the increment, so that the analog render process can use it to
			//calculate the current analog output line.
			if((renderDigitalHCounterPos + 1) == hscanSettings->vcounterIncrementPoint)
			{
				renderDigitalVCounterPosPreviousLine = renderDigitalVCounterPos;
			}

			//Advance the HV counters for the digital render process
			AdvanceHVCounters(*hscanSettings, renderDigitalHCounterPos, *vscanSettings, renderDigitalInterlaceEnabledActive, renderDigitalOddFlagSet, renderDigitalVCounterPos, 1);

			//Advance the mclk cycle progress of the current render timeslice
			mclkCyclesRemainingToAdvance -= mclkTicksForNextPixelClockTick;
			renderDigitalMclkCycleProgress += mclkTicksForNextPixelClockTick;
			renderDigitalRemainingMclkCycles = mclkCyclesRemainingToAdvance;
		}
		else
		{
			//Save any remaining mclk cycles from this update step
			renderDigitalRemainingMclkCycles = mclkCyclesRemainingToAdvance;

			//Clear the count of mclk cycles remaining to advance now that we've reached a
			//step that we can't complete.
			mclkCyclesRemainingToAdvance = 0;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::UpdateDigitalRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings)
{
	//Use the current VCounter data to determine whether we are rendering an active line
	//of the display, and which active line number we're up to, based on the current
	//screen mode settings.
	bool insideActiveScanRow = false;
	int renderDigitalCurrentRow = -1;
	if((renderDigitalVCounterPos >= vscanSettings.activeDisplayVCounterFirstValue) && (renderDigitalVCounterPos <= vscanSettings.activeDisplayVCounterLastValue))
	{
		//We're inside the active display region. Calculate the active display row number
		//for this row.
		insideActiveScanRow = true;
		renderDigitalCurrentRow = renderDigitalVCounterPos - vscanSettings.activeDisplayVCounterFirstValue;
	}
	else if(renderDigitalVCounterPos == vscanSettings.vcounterMaxValue)
	{
		//We're in the first line before the active display. On this line, no actual pixel
		//data is output, but the same access restrictions apply as in an active scan
		//line. Sprite mapping data is also read during this line, to determine which
		//sprites to show on the first line of the display.
		insideActiveScanRow = true;
		renderDigitalCurrentRow = -1;
	}

	//Obtain the linear hcounter value for the current render position
	unsigned int hcounterLinear = HCounterValueFromVDPInternalToLinear(hscanSettings, renderDigitalHCounterPos);

	//Perform a VSRAM read cache operation if one is required. Note that hardware tests
	//have confirmed this runs even if the display is disabled, or the current line is not
	//within the active display region. Note that hardware tests have also shown that the
	//H32/H40 screen mode setting is not taken into account when reading this data, and
	//that the scroll data for cells only visible in H40 mode is still read in H32 mode
	//when the hcounter reaches the necessary target locations.
	//##TODO## Confirm the exact time this register is latched, using the stable
	//raster DMA method as a testing mechanism.
	//##TODO## Clean up the following old comments
	//##FIX## We've heard reports from Mask of Destiny, and our current corruption in
	//Panorama Cotton seems to confirm, that when the VSCROLL mode is set to overall,
	//the VSRAM data is apparently latched only once for the line, not read at the
	//start of every 2 cell block. Changing this behaviour fixes Panorama Cotton, but
	//it seems to contradict tests done to measure the VSRAM read slots. Actually,
	//this has yet to be confirmed. We need to repeat our VSRAM read tests for which
	//values are read, when the vscroll mode is set to overall. Most likely, the
	//correct VSRAM reads are always done for 2-cell vertical scrolling, but when
	//overall scrolling is enabled, only the read at the start of the line is latched
	//and used to update the cached value. We need to do hardware tests to confirm the
	//correct behaviour.
	//##NOTE## Implementing the caching for vertical scrolling has broken the Sonic 3D
	//blast special stages, but we believe this is caused by bad horizontal interrupt
	//timing rather than this caching itself.
	bool vsramCacheOperationRequired = ((renderDigitalHCounterPos & 0x007) == 0);
	if(vsramCacheOperationRequired)
	{
		//Calculate the target column and layer for this VSRAM cache operation
		unsigned int vsramColumnNumber = (renderDigitalHCounterPos >> 4);
		unsigned int vsramLayerNumber = (renderDigitalHCounterPos & 0x008) >> 3;

		//Determine if interlace mode 2 is currently active
		bool interlaceMode2Active = renderDigitalInterlaceEnabledActive && renderDigitalInterlaceDoubleActive;

		//Read registers which affect the read of vscroll data
		bool vscrState = M5GetVSCR(accessTarget);

		//Read vscroll data for the target layer if required. Note that new data is only
		//latched if column vertical scrolling is enabled, or if this is the first column
		//in the display. Panorama Cotton relies on this in the intro screen and during
		//levels.
		if(vscrState || (vsramColumnNumber == 0))
		{
			unsigned int& targetLayerPatternDisplacement = (vsramLayerNumber == 0)? renderLayerAVscrollPatternDisplacement: renderLayerBVscrollPatternDisplacement;
			unsigned int& targetLayerMappingDisplacement = (vsramLayerNumber == 0)? renderLayerAVscrollMappingDisplacement: renderLayerBVscrollMappingDisplacement;
			DigitalRenderReadVscrollDataNew(vsramColumnNumber, vsramLayerNumber, vscrState, interlaceMode2Active, targetLayerPatternDisplacement, targetLayerMappingDisplacement, renderVSRAMCachedRead);
		}
	}

	//Obtain the set of internal update steps for the current raster position based on the
	//current screen mode settings.
	unsigned int internalOperationArraySize = 0;
	const InternalRenderOp* internalOperationArray = 0;
	if(renderDigitalScreenModeRS1Active)
	{
		internalOperationArray = &internalOperationsH40[0];
		internalOperationArraySize = sizeof(internalOperationsH40) / sizeof(internalOperationsH40[0]);
	}
	else
	{
		internalOperationArray = &internalOperationsH32[0];
		internalOperationArraySize = sizeof(internalOperationsH32) / sizeof(internalOperationsH32[0]);
	}

	//Perform the next internal update step for the current hcounter location
	InternalRenderOp nextInternalOperation = internalOperationArray[hcounterLinear % internalOperationArraySize];
	PerformInternalRenderOperation(accessTarget, hscanSettings, vscanSettings, nextInternalOperation, renderDigitalCurrentRow);

	//Read the display enable register. If this register is cleared, the output for this
	//update step is forced to the background colour, and free access to VRAM is
	//permitted. Any read operations that would have been performed from VRAM are not
	//performed in this case. If this occurs during the active region of the display, it
	//corrupts the output, due to mapping and pattern data not being read at the
	//appropriate time. It is safe to disable the display during the portion of a scanline
	//that lies outside the active display area, however, this reduces the number of
	//sprite pattern blocks which can be read, creating a lower sprite limit on the
	//following scanline. A sprite overflow is flagged if not all the sprite pixels could
	//be read due to the display being disabled, even if there would normally be enough
	//time to read all the pattern data had the display not been disabled. The only game
	//which is known to rely on this is "Mickey Mania" for the Mega Drive. See
	//http://gendev.spritesmind.net/forum/viewtopic.php?t=541
	//##TODO## Determine if the display enable bit is effective when the VDP test
	//register has been set to one of the modes that disables the blanking of the
	//display in the border regions.
	bool displayEnabled = M5GetDisplayEnabled(accessTarget);

	//Obtain the set of VRAM update steps for the current raster position based on the
	//current screen mode settings. If this line is outside the active display area, IE,
	//in the top or bottom border areas or in vblank, or if the display is currently
	//disabled, free access is permitted to VRAM except during the memory refresh slots,
	//so a different set of update steps apply. Note that there is an additional refresh
	//slot in non-active lines compared to active lines.
	unsigned int vramOperationArraySize = 0;
	const VRAMRenderOp* vramOperationArray = 0;
	if(!displayEnabled || !insideActiveScanRow)
	{
		if(renderDigitalScreenModeRS1Active)
		{
			vramOperationArray = &vramOperationsH40InactiveLine[0];
			vramOperationArraySize = sizeof(vramOperationsH40InactiveLine) / sizeof(vramOperationsH40InactiveLine[0]);
		}
		else
		{
			vramOperationArray = &vramOperationsH32InactiveLine[0];
			vramOperationArraySize = sizeof(vramOperationsH32InactiveLine) / sizeof(vramOperationsH32InactiveLine[0]);
		}
	}
	else
	{
		if(renderDigitalScreenModeRS1Active)
		{
			vramOperationArray = &vramOperationsH40ActiveLine[0];
			vramOperationArraySize = sizeof(vramOperationsH40ActiveLine) / sizeof(vramOperationsH40ActiveLine[0]);
		}
		else
		{
			vramOperationArray = &vramOperationsH32ActiveLine[0];
			vramOperationArraySize = sizeof(vramOperationsH32ActiveLine) / sizeof(vramOperationsH32ActiveLine[0]);
		}
	}

	//Perform any VRAM render operations which need to occur on this cycle. Note that VRAM
	//render operations only occur once every 4 SC cycles, since it takes the VRAM 4 SC
	//cycles for each 32-bit serial memory read, which is performed by the VDP itself to
	//read VRAM data for the rendering process, or 4 SC cycles for an 8-bit direct memory
	//read or write, which can occur at an access slot. Every 2 SC cycles however, a pixel
	//is output to the analog output circuit to perform layer priority selection and video
	//output. Interestingly, the synchronization of the memory times with the hcounter
	//update process is different, depending on whether H40 mode is active. Where a H32
	//mode is selected, memory access occurs on odd hcounter values. Where H40 mode is
	//selected, memory access occurs on even hcounter values.
	//##TODO## Perform more hardware tests on this behaviour, to confirm the
	//synchronization differences, and determine whether it is the memory access timing or
	//the hcounter progression which changes at the time the H40 screen mode setting is
	//toggled.
	bool hcounterLowerBit = (renderDigitalHCounterPos & 0x1) != 0;
	if(renderDigitalScreenModeRS1Active != hcounterLowerBit)
	{
		//Determine what VRAM operation to perform at the current scanline position
		VRAMRenderOp nextVRAMOperation = vramOperationArray[(hcounterLinear >> 1) % vramOperationArraySize];

		//Perform the VRAM operation
		PerformVRAMRenderOperation(accessTarget, hscanSettings, vscanSettings, nextVRAMOperation, renderDigitalCurrentRow);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformInternalRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const InternalRenderOp& nextOperation, int renderDigitalCurrentRow)
{
	switch(nextOperation.operation)
	{
	case InternalRenderOp::SPRITEMAPPINGCLEAR:
		//If we're about to start reading sprite mapping data for the next line, we need
		//to reset some internal state here.
		//##TODO## Ensure that on a reset, this data is initialized correctly
		//##TODO## Test if the first line of the display always acts like a dot
		//overflow didn't occur on the previous line, or if the VDP remembers if a dot
		//overflow occurred on the very last line of the display when loading sprites
		//for the first line of the display.
		renderSpriteDotOverflowPreviousLine = renderSpriteDotOverflow;
		renderSpriteDotOverflow = false;
		renderSpriteDisplayCellCacheEntryCount = 0;
		nonSpriteMaskCellEncountered = false;
		renderSpriteMaskActive = false;
		renderSpriteCollision = false;

		//Advance the sprite pixel buffer plane
		renderSpritePixelBufferAnalogRenderPlane = (renderSpritePixelBufferAnalogRenderPlane + 1) % renderSpritePixelBufferPlaneCount;
		renderSpritePixelBufferDigitalRenderPlane = (renderSpritePixelBufferDigitalRenderPlane + 1) % renderSpritePixelBufferPlaneCount;
		break;
	case InternalRenderOp::SPRITEPATTERNCLEAR:
		//If we're about to start reading sprite pattern data for the next line, reset any
		//necessary state so that we can correctly start working with the new data.
		//##TODO## Ensure that on a reset, this data is initialized correctly
		renderSpriteSearchComplete = false;
		renderSpriteOverflow = false;
		renderSpriteNextAttributeTableEntryToRead = 0;
		renderSpriteDisplayCacheEntryCount = 0;
		renderSpriteDisplayCellCacheCurrentIndex = 0;
		renderSpriteDisplayCacheCurrentIndex = 0;

		//Clear the contents of the sprite pixel buffer for this line. Note that most
		//likely in the real VDP, the analog render process would do this for us as it
		//pulls sprite data out of the buffer.
		for(unsigned int i = 0; i < spritePixelBufferSize; ++i)
		{
			spritePixelBuffer[renderSpritePixelBufferDigitalRenderPlane][i].entryWritten = false;
		}
		break;
	case InternalRenderOp::SPRITECACHE:{
		//Calculate the row number of the next line of sprite data to fetch. We add a
		//value of 1 to the current row number because sprites are first parsed one line
		//before the line on which they are displayed, since there are three separate
		//rendering phases for sprites, the first of which is a full traversal of the
		//cached sprite attribute table data, which occurs on the row before the row at
		//which we're searching for sprites to render for. Note that we rely on the
		//current row number being set to -1 for the invisible line before the first
		//display line in the active display region here.
		unsigned int renderSpriteNextSpriteRow = renderDigitalCurrentRow + 1;

		//Determine if interlace mode 2 is currently active
		bool interlaceMode2Active = renderDigitalInterlaceEnabledActive && renderDigitalInterlaceDoubleActive;

		//Read the next entry out of the sprite cache
		DigitalRenderBuildSpriteList(renderSpriteNextSpriteRow, interlaceMode2Active, renderDigitalScreenModeRS1Active, renderSpriteNextAttributeTableEntryToRead, renderSpriteSearchComplete, renderSpriteOverflow, renderSpriteDisplayCacheEntryCount, renderSpriteDisplayCache);
		break;}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::PerformVRAMRenderOperation(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, const VRAMRenderOp& nextOperation, int renderDigitalCurrentRow)
{
	//Determine if interlace mode 2 is currently active
	bool interlaceMode2Active = renderDigitalInterlaceEnabledActive && renderDigitalInterlaceDoubleActive;

	//Perform the next operation
	switch(nextOperation.operation)
	{
	case VRAMRenderOp::HSCROLL:{
		//Skip the operation if this is the invisible line just before the display region
		if(renderDigitalCurrentRow < 0) break;

		//Read register settings which affect which hscroll data is loaded
		unsigned int hscrollDataBase = M5GetHScrollDataBase(accessTarget);
		bool hscrState = M5GetHSCR(accessTarget);
		bool lscrState = M5GetLSCR(accessTarget);

		//Load the hscroll data into the hscroll data cache
		DigitalRenderReadHscrollData(renderDigitalCurrentRow, hscrollDataBase, hscrState, lscrState, renderLayerAHscrollPatternDisplacement, renderLayerBHscrollPatternDisplacement, renderLayerAHscrollMappingDisplacement, renderLayerBHscrollMappingDisplacement);
		break;}
	case VRAMRenderOp::MAPPING_A:{
		//Skip the operation if this is the invisible line just before the display region
		if(renderDigitalCurrentRow < 0) break;

		//Calculate the index number of the current 2-cell column that's being rendered.
		//Note that we subtract 1 from the index number specified in the operation table.
		//The first mapping data we read is for the left-scrolled 2-cell column, which
		//reads its mapping data from cell block -1.
		unsigned int renderDigitalCurrentColumn = (nextOperation.index - 1) / cellsPerColumn;

		//Read the current window register settings
		bool windowAlignedRight = M5GetWindowRightAligned(accessTarget);
		bool windowAlignedBottom = M5GetWindowBottomAligned(accessTarget);
		unsigned int windowBasePointX = M5GetWindowBasePointX(accessTarget);
		unsigned int windowBasePointY = M5GetWindowBasePointY(accessTarget);

		//Calculate the screen position of the window layer
		unsigned int windowStartCellX = (windowAlignedRight)? windowBasePointX: 0;
		unsigned int windowEndCellX = (windowAlignedRight)? 9999: windowBasePointX;
		unsigned int windowStartCellY = (windowAlignedBottom)? windowBasePointY: 0;
		unsigned int windowEndCellY = (windowAlignedBottom)? 9999: windowBasePointY;

		//Determine if the window is active in the current column and row
		//##TODO## Perform hardware tests on changing the window register state mid-line.
		//We need to determine how the VDP responds, in particular, how the window
		//distortion bug behaves when the window is enabled or disabled mid-line.
		const unsigned int rowsToDisplayPerCell = 8;
		unsigned int currentCellRow = renderDigitalCurrentRow / rowsToDisplayPerCell;
		bool windowActiveInThisColumn = false;
		if(nextOperation.index > 0)
		{
			windowActiveInThisColumn = ((renderDigitalCurrentColumn >= windowStartCellX) && (renderDigitalCurrentColumn < windowEndCellX))
			                        || ((currentCellRow >= windowStartCellY) && (currentCellRow < windowEndCellY));
			renderWindowActiveCache[renderDigitalCurrentColumn] = windowActiveInThisColumn;
		}

		//Calculate the effective screen row number to use when calculating the mapping
		//data row for this line. Note that the screen row number is just used directly
		//under normal circumstances, but when interlace mode 2 is active, there are
		//effectively double the number of vertical screen rows, with all the odd rows in
		//one frame, and all the even rows in another frame, so if interlace mode 2 is
		//active we double the screen row number and select either odd or even rows based
		//on the current state of the odd flag.
		unsigned int screenRowIndex = renderDigitalCurrentRow;
		if(interlaceMode2Active)
		{
			screenRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenRowIndex;
			}
		}

		//Read the correct mapping data for this column, taking into account whether that
		//mapping data should be read from the window mapping table or the layer A mapping
		//table.
		if(windowActiveInThisColumn)
		{
			//Read register settings which affect which mapping data is loaded
			//##FIX## Documentation and hardware tests have shown that WD11 is masked in
			//H40 mode.
			unsigned int nameTableBase = M5GetNameTableBaseWindow(accessTarget);

			//The window mapping table dimensions are determined based on the H40 screen
			//mode state. If H40 mode is active, the window mapping table is 64 cells
			//wide, otherwise, it is 32 cells wide. We emulate that here by calculating
			//the mapping table dimensions ourselves and passing this data in when reading
			//the window layer mapping data.
			unsigned int hszState = (renderDigitalScreenModeRS1Active)? 0x1: 0x0;
			unsigned int vszState = 0x0;

			//Read window mapping data
			DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, 0, 0, 0, hszState, vszState, renderMappingDataCacheLayerA[nextOperation.index], renderMappingDataCacheLayerA[nextOperation.index+1]);

			//Apply layer removal for the window layer
			if(!enableWindowHigh || !enableWindowLow)
			{
				//Read register settings which affect which mapping data is loaded
				nameTableBase = M5GetNameTableBaseScrollA(accessTarget);
				hszState = M5GetHSZ(accessTarget);
				vszState = M5GetVSZ(accessTarget);

				//Read layer A mapping data
				Data layerAMapping1(16);
				Data layerAMapping2(16);
				DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, renderLayerAHscrollMappingDisplacement, renderLayerAVscrollMappingDisplacement, renderLayerAVscrollPatternDisplacement, hszState, vszState, layerAMapping1, layerAMapping2);

				//Substitute layer A mapping data for any mapping blocks in the window
				//region that are hidden by layer removal
				bool windowMapping1Priority = renderMappingDataCacheLayerA[nextOperation.index].MSB();
				bool windowMapping2Priority = renderMappingDataCacheLayerA[nextOperation.index+1].MSB();
				if((windowMapping1Priority && !enableWindowHigh) || (!windowMapping1Priority && !enableWindowLow)
				|| (windowMapping2Priority && !enableWindowHigh) || (!windowMapping2Priority && !enableWindowLow))
				{
					renderWindowActiveCache[renderDigitalCurrentColumn] = false;
					renderMappingDataCacheLayerA[nextOperation.index] = layerAMapping1;
					renderMappingDataCacheLayerA[nextOperation.index+1] = layerAMapping2;
				}
			}
		}
		else
		{
			//Read register settings which affect which mapping data is loaded
			unsigned int nameTableBase = M5GetNameTableBaseScrollA(accessTarget);
			unsigned int hszState = M5GetHSZ(accessTarget);
			unsigned int vszState = M5GetVSZ(accessTarget);

			//Read layer A mapping data
			DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, renderLayerAHscrollMappingDisplacement, renderLayerAVscrollMappingDisplacement, renderLayerAVscrollPatternDisplacement, hszState, vszState, renderMappingDataCacheLayerA[nextOperation.index], renderMappingDataCacheLayerA[nextOperation.index+1]);
		}

		break;}
	case VRAMRenderOp::PATTERN_A:{
		//Skip the operation if this is the invisible line just before the display region
		if(renderDigitalCurrentRow < 0) break;

		//Calculate the vertical pattern displacement value to use to calculate the row of
		//the pattern data to read. The only purpose of this code is to handle window
		//layer support. In regions where the window layer is active, we need to disable
		//vertical scrolling, so if this cell number is within the window region of the
		//display, we force the vertical pattern displacement to 0, otherwise, we use the
		//calculated vertical pattern displacement based on the layer A vscroll data.
		unsigned int verticalPatternDisplacement = renderLayerAVscrollPatternDisplacement;
		if(nextOperation.index >= 2)
		{
			unsigned int renderDigitalCurrentColumn = (nextOperation.index - 2) / cellsPerColumn;
			if(renderWindowActiveCache[renderDigitalCurrentColumn])
			{
				verticalPatternDisplacement = (interlaceMode2Active && renderDigitalOddFlagSet)? 1: 0;
			}
		}

		//Calculate the pattern row number to read for the selected pattern, based on the
		//current row being drawn on the screen, and the vertical pattern displacement due
		//to vscroll. We also factor in interlace mode 2 support here by doubling the
		//screen row number where interlace mode 2 is active. The state of the odd flag is
		//factored in when the vscroll data is read.
		const unsigned int rowsToDisplayPerTile = 8;
		unsigned int screenPatternRowIndex = renderDigitalCurrentRow % rowsToDisplayPerTile;
		if(interlaceMode2Active)
		{
			screenPatternRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenPatternRowIndex;
			}
		}
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		unsigned int patternRowNumberNoFlip = (screenPatternRowIndex + verticalPatternDisplacement) % rowsPerTile;

		//Read layer A pattern row
		DigitalRenderReadPatternDataRow(patternRowNumberNoFlip, 0, interlaceMode2Active, renderMappingDataCacheLayerA[nextOperation.index], renderPatternDataCacheLayerA[nextOperation.index]);
		break;}
	case VRAMRenderOp::MAPPING_B:{
		//Skip the operation if this is the invisible line just before the display region
		if(renderDigitalCurrentRow < 0) break;

		//Calculate the index number of the current 2-cell column that's being rendered.
		//Note that we subtract 1 from the index number specified in the operation table.
		//The first mapping data we read is for the left-scrolled 2-cell column, which
		//reads its mapping data from cell block -1.
		unsigned int renderDigitalCurrentColumn = (nextOperation.index - 1) / cellsPerColumn;

		//Calculate the effective screen row number to use when calculating the mapping
		//data row for this line. Note that the screen row number is just used directly
		//under normal circumstances, but when interlace mode 2 is active, there are
		//effectively double the number of vertical screen rows, with all the odd rows in
		//one frame, and all the even rows in another frame, so if interlace mode 2 is
		//active we double the screen row number and select either odd or even rows based
		//on the current state of the odd flag.
		unsigned int screenRowIndex = renderDigitalCurrentRow;
		if(interlaceMode2Active)
		{
			screenRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenRowIndex;
			}
		}

		//Read register settings which affect which mapping data is loaded
		unsigned int nameTableBase = M5GetNameTableBaseScrollB(accessTarget);
		unsigned int hszState = M5GetHSZ(accessTarget);
		unsigned int vszState = M5GetVSZ(accessTarget);

		//Read layer B mapping data
		DigitalRenderReadMappingDataPair(screenRowIndex, renderDigitalCurrentColumn, interlaceMode2Active, nameTableBase, renderLayerBHscrollMappingDisplacement, renderLayerBVscrollMappingDisplacement, renderLayerBVscrollPatternDisplacement, hszState, vszState, renderMappingDataCacheLayerB[nextOperation.index], renderMappingDataCacheLayerB[nextOperation.index+1]);
		break;}
	case VRAMRenderOp::PATTERN_B:{
		//Skip the operation if this is the invisible line just before the display region
		if(renderDigitalCurrentRow < 0) break;

		//Calculate the pattern row number to read for the selected pattern, based on the
		//current row being drawn on the screen, and the vertical pattern displacement due
		//to vscroll. We also factor in interlace mode 2 support here by doubling the
		//screen row number where interlace mode 2 is active. The state of the odd flag is
		//factored in when the vscroll data is read.
		const unsigned int rowsToDisplayPerTile = 8;
		unsigned int screenPatternRowIndex = renderDigitalCurrentRow % rowsToDisplayPerTile;
		if(interlaceMode2Active)
		{
			screenPatternRowIndex *= 2;
			if(renderDigitalOddFlagSet)
			{
				++screenPatternRowIndex;
			}
		}
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		unsigned int patternRowNumberNoFlip = (screenPatternRowIndex + renderLayerBVscrollPatternDisplacement) % rowsPerTile;

		//Read layer B pattern row
		DigitalRenderReadPatternDataRow(patternRowNumberNoFlip, 0, interlaceMode2Active, renderMappingDataCacheLayerB[nextOperation.index], renderPatternDataCacheLayerB[nextOperation.index]);
		break;}
	case VRAMRenderOp::MAPPING_S:{
		//Read sprite mapping data and decode sprite cells for the next sprite to display
		//on the current scanline.
		if(renderSpriteDisplayCacheCurrentIndex < renderSpriteDisplayCacheEntryCount)
		{
			//Read the sprite table base address register
			unsigned int spriteTableBaseAddress = M5GetNameTableBaseSprite(accessTarget);

			//According to official documentation, if we're in H40 mode, the AT9 bit of
			//the sprite table base address is masked. We emulate that here. Note that the
			//"Traveller's Tales" logo in Sonic 3D on the Mega Drive relies on AT9 being
			//valid in H32 mode.
			//##TODO## Perform hardware tests to confirm this behaviour
			if(renderDigitalScreenModeRS1Active)
			{
				spriteTableBaseAddress &= 0xFC00;
			}

			//Obtain a reference to the corresponding sprite display cache entry for this
			//sprite.
			SpriteDisplayCacheEntry& spriteDisplayCacheEntry = renderSpriteDisplayCache[renderSpriteDisplayCacheCurrentIndex];

			//Build a sprite cell list for the current sprite
			DigitalRenderBuildSpriteCellList(hscanSettings, vscanSettings, renderSpriteDisplayCacheCurrentIndex, spriteTableBaseAddress, interlaceMode2Active, renderDigitalScreenModeRS1Active, renderSpriteDotOverflow, spriteDisplayCacheEntry, renderSpriteDisplayCellCacheEntryCount, renderSpriteDisplayCellCache);

			//Advance to the next sprite mapping entry
			++renderSpriteDisplayCacheCurrentIndex;
		}
		break;}
	case VRAMRenderOp::PATTERN_S:{
		//Read sprite pattern data for the next sprite cell to display on the current
		//scanline. Note that we know that if the display is disabled during active scan,
		//sprite pattern data which was supposed to be read during the disabled region
		//isn't skipped, but rather, the entire sprite pattern data load queue remains in
		//its current state, and the next sprite pattern data block to read is retrieved
		//from the queue when a slot becomes available. This means we can't use a simple
		//index number to determine which sprite data to read, but rather, we need to keep
		//a running index of the sprite pattern data we're up to.
		//##TODO## Check if the sprite collision flag is still set for sprite pixels that
		//are masked.
		if(renderSpriteDisplayCellCacheCurrentIndex < renderSpriteDisplayCellCacheEntryCount)
		{
			//Obtain a reference to the corresponding sprite cell and sprite display cache
			//entries for this sprite cell.
			SpriteCellDisplayCacheEntry& spriteCellDisplayCacheEntry = renderSpriteDisplayCellCache[renderSpriteDisplayCellCacheCurrentIndex];
			SpriteDisplayCacheEntry& spriteDisplayCacheEntry = renderSpriteDisplayCache[spriteCellDisplayCacheEntry.spriteDisplayCacheIndex];

			//Read pattern data for the sprite cell
			DigitalRenderReadPatternDataRow(spriteCellDisplayCacheEntry.patternRowOffset, spriteCellDisplayCacheEntry.patternCellOffset, interlaceMode2Active, spriteDisplayCacheEntry.mappingData, spriteCellDisplayCacheEntry.patternData);

			//Read all used bits of the horizontal position data for this sprite
			Data spritePosH(9);
			spritePosH = spriteDisplayCacheEntry.hpos;

			//Take into account sprite masking
			//##TODO## Add a much longer comment here describing sprite masking
			if(spritePosH == 0)
			{
				if(nonSpriteMaskCellEncountered || renderSpriteDotOverflowPreviousLine)
				{
					renderSpriteMaskActive = true;
				}
			}
			else
			{
				//Since we've now encountered at least one sprite pattern cell which was
				//not a mask sprite cell, set a flag indicating that that we've
				//encountered a non mask cell. This is required in order to correctly
				//support sprite masking.
				nonSpriteMaskCellEncountered = true;
			}

			//If this sprite cell was not masked, load visible pixel information into the
			//sprite pixel buffer.
			//##TODO## Greatly improve commenting here.
			if(!renderSpriteMaskActive)
			{
				const unsigned int spritePosScreenStartH = 0x80;
				const unsigned int cellPixelWidth = 8;
				for(unsigned int cellPixelIndex = 0; cellPixelIndex < cellPixelWidth; ++cellPixelIndex)
				{
					//Check that this sprite pixel is within the visible screen boundaries
					unsigned int spritePixelH = spritePosH.GetData() + (spriteCellDisplayCacheEntry.spriteCellColumnNo * cellPixelWidth) + cellPixelIndex;
					if((spritePixelH >= spritePosScreenStartH) && ((spritePixelH - spritePosScreenStartH) < spritePixelBufferSize))
					{
						//Check that a non-transparent sprite pixel has not already been
						//written to this pixel location. Higher priority sprites are
						//listed first, with lower priority sprites appearing under higher
						//priority sprites, so if a non-transparent sprite pixel has
						//already been output to this location, the lower priority sprite
						//pixel is discarded, and the sprite collision flag is set in the
						//status register.
						unsigned int spritePixelBufferIndex = (spritePixelH - spritePosScreenStartH);
						SpritePixelBufferEntry& spritePixelBufferEntry = spritePixelBuffer[renderSpritePixelBufferDigitalRenderPlane][spritePixelBufferIndex];
						if(!spritePixelBufferEntry.entryWritten || (spritePixelBufferEntry.paletteIndex == 0))
						{
							//Mapping (Pattern Name) data format:
							//-----------------------------------------------------------------
							//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
							//|---------------------------------------------------------------|
							//|Pri|PalRow |VF |HF |              Pattern Number               |
							//-----------------------------------------------------------------
							//Pri:    Priority Bit
							//PalRow: The palette row number to use when displaying the pattern data
							//VF:     Vertical Flip
							//HF:     Horizontal Flip
							spritePixelBufferEntry.entryWritten = true;
							spritePixelBufferEntry.layerPriority = spriteDisplayCacheEntry.mappingData.GetBit(15);
							spritePixelBufferEntry.paletteLine = spriteDisplayCacheEntry.mappingData.GetDataSegment(13, 2);
							spritePixelBufferEntry.paletteIndex = DigitalRenderReadPixelIndex(spriteCellDisplayCacheEntry.patternData, spriteDisplayCacheEntry.mappingData.GetBit(11), cellPixelIndex);
						}
						else
						{
							renderSpriteCollision = true;
						}
					}
				}
			}

			//Advance to the next sprite cell entry
			++renderSpriteDisplayCellCacheCurrentIndex;
		}
		break;}
	case VRAMRenderOp::ACC_SLOT:
		//Since we've reached an external access slot, changes may now be made to VRAM or
		//VSRAM, so we need to advance the VRAM and VSRAM buffers up to this time so any
		//changes which occur at this access slot can take effect.
		vram->AdvanceBySession(renderDigitalMclkCycleProgress, vramSession, vramTimesliceCopy);
		vsram->AdvanceBySession(renderDigitalMclkCycleProgress, vsramSession, vsramTimesliceCopy);
		spriteCache->AdvanceBySession(renderDigitalMclkCycleProgress, spriteCacheSession, spriteCacheTimesliceCopy);
		break;
	case VRAMRenderOp::REFRESH:
		//Nothing to do on a memory refresh cycle
		break;
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::UpdateAnalogRenderProcess(const AccessTarget& accessTarget, const HScanSettings& hscanSettings, const VScanSettings& vscanSettings)
{
	bool outputNothing = false;
	bool forceOutputBackgroundPixel = false;

	//If the digital vcounter has already been incremented, but we haven't reached the end
	//of the analog line yet, move the digital vcounter back one step so we can calculate
	//the analog line number.
	unsigned int renderDigitalVCounterPosIncrementAtHBlank = renderDigitalVCounterPos;
	if((renderDigitalHCounterPos >= hscanSettings.vcounterIncrementPoint) && (renderDigitalHCounterPos < hscanSettings.hblankSetPoint))
	{
		renderDigitalVCounterPosIncrementAtHBlank = renderDigitalVCounterPosPreviousLine;
	}

	//Use the current VCounter data to determine which data is being displayed on this
	//row, based on the current screen mode settings.
	bool insidePixelBufferRegion = true;
	bool insideActiveScanVertically = false;
	unsigned int renderAnalogCurrentRow = 0;
	if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.activeDisplayVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.activeDisplayVCounterLastValue))
	{
		//We're inside the active display region
		renderAnalogCurrentRow = vscanSettings.topBorderLineCount + (renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.activeDisplayVCounterFirstValue);
		insideActiveScanVertically = true;
	}
	else
	{
		//Check if we're in a border region, or in the blanking region.
		if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.topBorderVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.topBorderVCounterLastValue))
		{
			//We're in the top border. In this case, we need to force the pixel output to
			//the current backdrop colour.
			renderAnalogCurrentRow = renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.topBorderVCounterFirstValue;
			forceOutputBackgroundPixel = true;
		}
		else if((renderDigitalVCounterPosIncrementAtHBlank >= vscanSettings.bottomBorderVCounterFirstValue) && (renderDigitalVCounterPosIncrementAtHBlank <= vscanSettings.bottomBorderVCounterLastValue))
		{
			//We're in the bottom border. In this case, we need to force the pixel output
			//to the current backdrop colour.
			renderAnalogCurrentRow = vscanSettings.topBorderLineCount + vscanSettings.activeDisplayLineCount + (renderDigitalVCounterPosIncrementAtHBlank - vscanSettings.bottomBorderVCounterFirstValue);
			forceOutputBackgroundPixel = true;
		}
		else
		{
			//We're in a blanking region. In this case, we need to force the pixel output
			//to black.
			insidePixelBufferRegion = false;
			outputNothing = true;
		}
	}

	//Use the current HCounter data to determine which data is next to be displayed on
	//this line, based on the current screen mode settings.
	bool insideActiveScanHorizontally = false;
	unsigned int renderAnalogCurrentPixel = 0;
	unsigned int activeScanPixelIndex = 0;
	if((renderDigitalHCounterPos >= hscanSettings.activeDisplayHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.activeDisplayHCounterLastValue))
	{
		//We're inside the active display region. Calculate the pixel number of the
		//current pixel to output on this update cycle.
		renderAnalogCurrentPixel = hscanSettings.leftBorderPixelCount + (renderDigitalHCounterPos - hscanSettings.activeDisplayHCounterFirstValue);
		activeScanPixelIndex = (renderDigitalHCounterPos - hscanSettings.activeDisplayHCounterFirstValue);
		insideActiveScanHorizontally = true;
	}
	else if((renderDigitalHCounterPos >= hscanSettings.leftBorderHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.leftBorderHCounterLastValue))
	{
		//We're in the left border. In this case, we need to force the pixel output to the
		//current backdrop colour.
		renderAnalogCurrentPixel = (renderDigitalHCounterPos - hscanSettings.leftBorderHCounterFirstValue);
		forceOutputBackgroundPixel = true;
	}
	else if((renderDigitalHCounterPos >= hscanSettings.rightBorderHCounterFirstValue) && (renderDigitalHCounterPos <= hscanSettings.rightBorderHCounterLastValue))
	{
		//We're in the right border. In this case, we need to force the pixel output to
		//the current backdrop colour.
		renderAnalogCurrentPixel = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + (renderDigitalHCounterPos - hscanSettings.rightBorderHCounterFirstValue);
		forceOutputBackgroundPixel = true;
	}
	else
	{
		//We're in a blanking region or in the hscan region. In this case, there's nothing
		//to output.
		insidePixelBufferRegion = false;
		outputNothing = true;
	}

	//Update the current screen raster position of the render output for debug output
	currentRenderPosOnScreen = false;
	if(insidePixelBufferRegion)
	{
		currentRenderPosScreenX = renderAnalogCurrentPixel;
		currentRenderPosScreenY = renderAnalogCurrentRow;
		currentRenderPosOnScreen = true;
	}

	//Roll our image buffers on to the next line and the next frame when appropriate
	if(renderDigitalHCounterPos == hscanSettings.hsyncNegated)
	{
		//Record the number of output pixels we're going to generate in this line
		imageBufferLineWidth[drawingImageBufferPlane][renderAnalogCurrentRow] = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount + hscanSettings.rightBorderPixelCount;

		//Record the active scan start and end positions for this line
		imageBufferActiveScanPosXStart[drawingImageBufferPlane][renderAnalogCurrentRow] = hscanSettings.leftBorderPixelCount;
		imageBufferActiveScanPosXEnd[drawingImageBufferPlane][renderAnalogCurrentRow] = hscanSettings.leftBorderPixelCount + hscanSettings.activeDisplayPixelCount;
	}
	else if(renderDigitalHCounterPos == hscanSettings.vcounterIncrementPoint && (renderDigitalVCounterPos == vscanSettings.vsyncClearedPoint))
	{
		boost::mutex::scoped_lock lock(imageBufferMutex);

		//Advance the image buffer to the next plane
		drawingImageBufferPlane = videoSingleBuffering? drawingImageBufferPlane: (drawingImageBufferPlane + 1) % imageBufferPlanes;

		//Now that we've completed another frame, notify the image window that a new
		//frame is ready to display.
		frameReadyInImageBuffer = true;

		//Record the odd interlace frame flag
		imageBufferLineCount[drawingImageBufferPlane] = renderDigitalOddFlagSet;

		//Record the number of raster lines we're going to render in the new frame
		imageBufferLineCount[drawingImageBufferPlane] = vscanSettings.topBorderLineCount + vscanSettings.activeDisplayLineCount + vscanSettings.bottomBorderLineCount;

		//Record the active scan start and end positions for this frame
		imageBufferActiveScanPosYStart[drawingImageBufferPlane] = vscanSettings.topBorderLineCount;
		imageBufferActiveScanPosYEnd[drawingImageBufferPlane] = vscanSettings.topBorderLineCount + vscanSettings.activeDisplayLineCount;

		//Clear the cache of sprite boundary lines in this frame
		boost::mutex::scoped_lock spriteLock(spriteBoundaryMutex[drawingImageBufferPlane]);
		imageBufferSpriteBoundaryLines[drawingImageBufferPlane].clear();
	}

	//Read the display enable register. If this register is cleared, the output for this
	//update step is forced to the background colour, and free access to VRAM is
	//permitted.
	bool displayEnabled = M5GetDisplayEnabled(accessTarget);
	if(!displayEnabled)
	{
		forceOutputBackgroundPixel = true;
	}

	//##TODO## Handle reg 0, bit 0, which completely disables video output while it is
	//set. In our case here, we should force the output colour to black.
	//##TODO## Test on the hardware if we should disable the actual rendering process
	//and allow free access to VRAM if reg 0 bit 0 is set, or if this bit only
	//disables the analog video output.
	//##NOTE## Hardware tests have shown this register only affects the CSYNC output line.
	//Clean up these comments, and ensure we're not doing anything to affect rendering
	//based on this register state.

	//Determine the palette line and index numbers and the shadow/highlight state for this
	//pixel.
	bool shadow = false;
	bool highlight = false;
	unsigned int paletteLine = 0;
	unsigned int paletteIndex = 0;
	if(outputNothing)
	{
		//If a pixel is being forced to black, we currently don't have anything to do
		//here.
	}
	else if(forceOutputBackgroundPixel)
	{
		//If this pixel is being forced to the background colour, read the current
		//background palette index and line data.
		paletteLine = M5GetBackgroundColorPalette(accessTarget);
		paletteIndex = M5GetBackgroundColorIndex(accessTarget);
	}
	else if(insideActiveScanVertically && insideActiveScanHorizontally)
	{
		//If we're displaying a pixel in the active display region, determine the correct
		//palette index for this pixel.

		//Collect the pattern and priority data for this pixel from each of the various
		//layers.
		//Mapping (Pattern Name) data format:
		//-----------------------------------------------------------------
		//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|---------------------------------------------------------------|
		//|Pri|PalRow |VF |HF |              Pattern Number               |
		//-----------------------------------------------------------------
		//Pri:    Priority Bit
		//PalRow: The palette row number to use when displaying the pattern data
		//VF:     Vertical Flip
		//HF:     Horizontal Flip
		unsigned int paletteLineData[4];
		unsigned int paletteIndexData[4];
		bool layerPriority[4];

		//Decode the sprite mapping and pattern data
		layerPriority[LAYER_SPRITE] = false;
		paletteIndexData[LAYER_SPRITE] = 0;
		if(spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex].entryWritten)
		{
			const SpritePixelBufferEntry& spritePixelBufferEntry = spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex];
			layerPriority[LAYER_SPRITE] = spritePixelBufferEntry.layerPriority;
			paletteLineData[LAYER_SPRITE] = spritePixelBufferEntry.paletteLine;
			paletteIndexData[LAYER_SPRITE] = spritePixelBufferEntry.paletteIndex;
		}

		//Decode the layer A mapping and pattern data
		unsigned int screenCellNo = activeScanPixelIndex / (cellBlockSizeH);
		unsigned int screenColumnNo = screenCellNo / cellsPerColumn;
		bool windowEnabledAtCell = renderWindowActiveCache[screenColumnNo];
		if(windowEnabledAtCell)
		{
			//Read the pixel data from the window plane
			unsigned int mappingNumberWindow = ((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) / cellBlockSizeH;
			unsigned int pixelNumberWindow = ((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) % cellBlockSizeH;
			const Data& windowMappingData = renderMappingDataCacheLayerA[mappingNumberWindow];
			layerPriority[LAYER_LAYERA] = windowMappingData.GetBit(15);
			paletteLineData[LAYER_LAYERA] = windowMappingData.GetDataSegment(13, 2);
			paletteIndexData[LAYER_LAYERA] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerA[mappingNumberWindow], windowMappingData.GetBit(11), pixelNumberWindow);
		}
		else
		{
			//Calculate the mapping number and pixel number within the layer
			unsigned int scrolledMappingNumberLayerA = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerAHscrollPatternDisplacement) / cellBlockSizeH;
			unsigned int scrolledPixelNumberLayerA = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerAHscrollPatternDisplacement) % cellBlockSizeH;

			//Take the window distortion bug into account. Due to an implementation quirk,
			//the real VDP apparently reads the window mapping and pattern data at the
			//start of the normal cell block reads, and doesn't use the left scrolled
			//2-cell block to read window data. As a result, the VDP doesn't have enough
			//VRAM access slots left to handle a scrolled playfield when transitioning
			//from a left-aligned window to the scrolled layer A plane. As a result of
			//their implementation, the partially visible 2-cell region immediately
			//following the end of the window uses the mapping and pattern data of the
			//column that follows it. Our implementation here has a similar effect, except
			//that we would fetch the mapping and pattern data for the 2-cell block
			//immediately before it, IE from the last column of the window region. We
			//adjust the mapping number here to correctly fetch the next column mapping
			//data instead when a partially visible column follows a left-aligned window,
			//as would the real hardware.
			//##TODO## Confirm through hardware tests that window mapping and pattern data
			//is read after the left scrolled 2-cell block.
			unsigned int currentScreenColumnPixelIndex = activeScanPixelIndex - (cellBlockSizeH * cellsPerColumn * screenColumnNo);
			unsigned int distortedPixelCount = renderLayerAHscrollPatternDisplacement + ((scrolledMappingNumberLayerA & 0x1) * cellBlockSizeH);
			if((screenColumnNo > 0) && renderWindowActiveCache[screenColumnNo-1] && (currentScreenColumnPixelIndex < distortedPixelCount))
			{
				scrolledMappingNumberLayerA += cellsPerColumn;
			}

			//Read the pixel data from the layer A plane
			const Data& layerAMappingData = renderMappingDataCacheLayerA[scrolledMappingNumberLayerA];
			layerPriority[LAYER_LAYERA] = layerAMappingData.GetBit(15);
			paletteLineData[LAYER_LAYERA] = layerAMappingData.GetDataSegment(13, 2);
			paletteIndexData[LAYER_LAYERA] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerA[scrolledMappingNumberLayerA], layerAMappingData.GetBit(11), scrolledPixelNumberLayerA);
		}

		//Decode the layer B mapping and pattern data
		unsigned int scrolledMappingNumberLayerB = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerBHscrollPatternDisplacement) / cellBlockSizeH;
		unsigned int scrolledPixelNumberLayerB = (((cellBlockSizeH * cellsPerColumn) + activeScanPixelIndex) - renderLayerBHscrollPatternDisplacement) % cellBlockSizeH;
		const Data& layerBMappingData = renderMappingDataCacheLayerB[scrolledMappingNumberLayerB];
		layerPriority[LAYER_LAYERB] = layerBMappingData.GetBit(15);
		paletteLineData[LAYER_LAYERB] = layerBMappingData.GetDataSegment(13, 2);
		paletteIndexData[LAYER_LAYERB] = DigitalRenderReadPixelIndex(renderPatternDataCacheLayerB[scrolledMappingNumberLayerB], layerBMappingData.GetBit(11), scrolledPixelNumberLayerB);

		//Read the background palette settings
		layerPriority[LAYER_BACKGROUND] = false;
		paletteLineData[LAYER_BACKGROUND] = M5GetBackgroundColorPalette(accessTarget);
		paletteIndexData[LAYER_BACKGROUND] = M5GetBackgroundColorIndex(accessTarget);

		//Determine if any of the palette index values for any of the layers indicate a
		//transparent pixel.
		//##TODO## Consider renaming and reversing the logic of these flags to match the
		//comment above. The name of "found pixel" isn't very descriptive, and in the case
		//of the sprite layer "isPixelOpaque" could be misleading when the sprite pixel is
		//being used as an operator in shadow/highlight mode. A flag with a name like
		//isPixelTransparent would be much more descriptive.
		bool foundSpritePixel = (paletteIndexData[LAYER_SPRITE] != 0);
		bool foundLayerAPixel = (paletteIndexData[LAYER_LAYERA] != 0);
		bool foundLayerBPixel = (paletteIndexData[LAYER_LAYERB] != 0);

		//Read the shadow/highlight mode settings. Note that hardware tests have confirmed
		//that changes to this register take effect immediately, at any point in a line.
		//##TODO## Confirm whether shadow highlight is active in border areas
		//##TODO## Confirm whether shadow highlight is active when the display is disabled
		bool shadowHighlightEnabled = M5GetShadowHighlightEnabled(accessTarget);
		bool spriteIsShadowOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 15);
		bool spriteIsHighlightOperator = (paletteLineData[LAYER_SPRITE] == 3) && (paletteIndexData[LAYER_SPRITE] == 14);

		//Implement the layer removal debugging feature
		foundSpritePixel &= ((enableSpriteHigh && enableSpriteLow) || (enableSpriteHigh && layerPriority[LAYER_SPRITE]) || (enableSpriteLow && !layerPriority[LAYER_SPRITE]));
		foundLayerAPixel &= ((enableLayerAHigh && enableLayerALow) || (enableLayerAHigh && layerPriority[LAYER_LAYERA]) || (enableLayerALow && !layerPriority[LAYER_LAYERA]));
		foundLayerBPixel &= ((enableLayerBHigh && enableLayerBLow) || (enableLayerBHigh && layerPriority[LAYER_LAYERB]) || (enableLayerBLow && !layerPriority[LAYER_LAYERB]));

		//##NOTE## The following code is disabled, because we use a lookup table to cache
		//the result of layer priority calculations. This gives us a significant
		//performance boost. The code below is provided for future reference and debugging
		//purposes. This code should, in all instances, produce the same result as the
		//table lookup below.
		//Perform layer priority calculations, and determine the layer to use, as well as
		//the resulting state of the shadow and highlight bits.
		//unsigned int layerIndex;
		//bool shadow;
		//bool highlight;
		//CalculateLayerPriorityIndex(layerIndex, shadow, highlight, shadowHighlightEnabled, spriteIsShadowOperator, spriteIsHighlightOperator, foundSpritePixel, foundLayerAPixel, foundLayerBPixel, prioritySprite, priorityLayerA, priorityLayerB);

		//Encode the parameters for the layer priority calculation into an index value for
		//the priority lookup table.
		unsigned int priorityIndex = 0;
		priorityIndex |= (unsigned int)shadowHighlightEnabled << 8;
		priorityIndex |= (unsigned int)spriteIsShadowOperator << 7;
		priorityIndex |= (unsigned int)spriteIsHighlightOperator << 6;
		priorityIndex |= (unsigned int)foundSpritePixel << 5;
		priorityIndex |= (unsigned int)foundLayerAPixel << 4;
		priorityIndex |= (unsigned int)foundLayerBPixel << 3;
		priorityIndex |= (unsigned int)layerPriority[LAYER_SPRITE] << 2;
		priorityIndex |= (unsigned int)layerPriority[LAYER_LAYERA] << 1;
		priorityIndex |= (unsigned int)layerPriority[LAYER_LAYERB];

		//Lookup the pre-calculated layer priority from the lookup table. We use a lookup
		//table to eliminate branching, which should yield a significant performance
		//boost.
		unsigned int layerSelectionResult = layerPriorityLookupTable[priorityIndex];

		//Extract the layer index, shadow, and highlight data from the combined result
		//returned from the layer priority lookup table.
		unsigned int layerIndex = layerSelectionResult & 0x03;
		shadow = (layerSelectionResult & 0x08) != 0;
		highlight = (layerSelectionResult & 0x04) != 0;

		//Read the palette line and index to use for the selected layer
		paletteLine = paletteLineData[layerIndex];
		paletteIndex = paletteIndexData[layerIndex];
	}

	//If we just determined the palette line and index for a pixel within the active scan
	//region of the screen, initialize the sprite pixel buffer at the corresponding pixel
	//location, so that it is clear and ready to receive new sprite data on the next line.
	//##TODO## Remove this old code
	//if(insideActiveScanVertically && insideActiveScanHorizontally)
	//{
	//	spritePixelBuffer[renderSpritePixelBufferAnalogRenderPlane][activeScanPixelIndex].entryWritten = false;
	//}

	//##TODO## Write a much longer comment here
	//##FIX## This comment doesn't actually reflect what we do right now
	//If a CRAM write has occurred at the same time as we're outputting this next
	//pixel, retrieve the value written to CRAM and output that value instead.
	//##TODO## Consider only advancing the CRAM buffer if we're passing the next write
	//time in this step. If we're not, there's no point doing the advance, since it
	//contains the same test we're doing here to decide if any work needs to be done.
	//##FIX## Correct a timing problem with all our buffers. Currently, writes to our
	//buffers are performed relative to the state update time, which factors in extra time
	//we may have rendered past the end of the timeslice in the last step. If we move past
	//the end of a timeslice, the following timeslice is shortened, and writes in that
	//timeslice are offset by the amount we ran over the last timeslice. We need to ensure
	//that the digital renderer knows about, and uses, these "stateLastUpdateMclk" values
	//to advance each timeslice, not the indicated timeslice length. This will ensure that
	//during the render process, values in the buffer will be committed at the same
	//relative time at which they were written.
	//##TODO## As part of the above, consider solving this issue more permanently, with an
	//upgrade to our timed buffers to roll writes past the end of a timeslice into the
	//next timeslice.
	if(cramSession.writeInfo.exists && (cramSession.nextWriteTime <= renderDigitalMclkCycleProgress))
	{
		const unsigned int paletteEntriesPerLine = 16;
		const unsigned int paletteEntrySize = 2;
		unsigned int cramWriteAddress = cramSession.writeInfo.writeAddress;
		paletteLine = (cramWriteAddress / paletteEntrySize) / paletteEntriesPerLine;
		paletteIndex = (cramWriteAddress / paletteEntrySize) % paletteEntriesPerLine;
	}

	//Now that we've advanced the analog render cycle and handled CRAM write flicker,
	//advance the committed state of the CRAM buffer. If a write occurred to CRAM at
	//the same time as this pixel was being drawn, it will now have been committed to
	//CRAM.
	cram->AdvanceBySession(renderDigitalMclkCycleProgress, cramSession, cramTimesliceCopy);

	//If we're drawing a pixel which is within the area of the screen we're rendering
	//pixel data for, output the pixel data to the image buffer.
	if(insidePixelBufferRegion)
	{
		//##TODO## Our new colour values are basically correct, assuming what is suspected
		//after analysis posted on SpritesMind, that the Mega Drive VDP never actually outputs
		//at full intensity. We haven't taken the apparent "ladder effect" into account
		//however. It is highly recommended that we perform our own tests on the hardware, and
		//make some comparisons between captured video output from the real system, and the
		//output from our emulator, when playing back on the same physical screen. If the
		//ladder effect is real and does have an effect on the way the intensity is perceived
		//on the screen, we should emulate it. We also need to confirm the maximum intensity
		//output by the VDP. A step size of 18 for example would get a max value of 252, which
		//would be more logical.
		//const unsigned char paletteEntryTo8Bit[8] = {0, 36, 73, 109, 146, 182, 219, 255};
		//const unsigned char paletteEntryTo8BitShadow[8] = {0, 18, 37, 55, 73, 91, 110, 128};
		//const unsigned char paletteEntryTo8BitHighlight[8] = {128, 146, 165, 183, 201, 219, 238, 255};
		const unsigned char paletteEntryTo8Bit[8] = {0, 34, 68, 102, 136, 170, 204, 238};
		const unsigned char paletteEntryTo8BitShadow[8] = {0, 17, 34, 51, 68, 85, 102, 119};
		const unsigned char paletteEntryTo8BitHighlight[8] = {119, 136, 153, 170, 187, 204, 221, 238};
		const unsigned int paletteEntriesPerLine = 16;
		const unsigned int paletteEntrySize = 2;

		//Calculate the address of the colour value to read from the palette
		unsigned int paletteEntryAddress = (paletteIndex + (paletteLine * paletteEntriesPerLine)) * paletteEntrySize;

		//Read the target palette entry
		Data paletteData(16);
		paletteData = (unsigned int)(cram->ReadCommitted(paletteEntryAddress+0) << 8) | (unsigned int)cram->ReadCommitted(paletteEntryAddress+1);

		//Decode the target palette entry, and extract the individual 7-bit R, G, and B
		//intensity values.
		//-----------------------------------------------------------------
		//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|---------------------------------------------------------------|
		//| /   /   /   / |   Blue    | / |   Green   | / |    Red    | / |
		//-----------------------------------------------------------------
		unsigned int colorIntensityR = paletteData.GetDataSegment(1, 3);
		unsigned int colorIntensityG = paletteData.GetDataSegment(5, 3);
		unsigned int colorIntensityB = paletteData.GetDataSegment(9, 3);

		//If a reduced palette is in effect, due to bit 2 of register 1 being cleared,
		//only the lowest bit of each intensity value has any effect, and it selects
		//between half intensity and minimum intensity. Note that hardware tests have
		//shown that changes to this register take effect immediately, at any point in a
		//line.
		//##TODO## Confirm the interaction of shadow highlight mode with the palette
		//select bit.
		//##TODO## Confirm the mapping of intensity values when the palette select bit is
		//cleared.
		if(!M5GetPSEnabled(accessTarget))
		{
			colorIntensityR = (colorIntensityR & 0x01) << 2;
			colorIntensityG = (colorIntensityG & 0x01) << 2;
			colorIntensityB = (colorIntensityB & 0x01) << 2;
		}

		//Convert the palette data to a 32-bit RGBA triple and write it to the image
		//buffer
		//##TODO## As an optimization, use a combined lookup table for colour value
		//decoding, and eliminate the branching logic here.
		ImageBufferColorEntry& imageBufferEntry = *((ImageBufferColorEntry*)&imageBuffer[drawingImageBufferPlane][((renderAnalogCurrentRow * imageBufferWidth) + renderAnalogCurrentPixel) * 4]);
		if(outputNothing)
		{
			imageBufferEntry.r = 0;
			imageBufferEntry.g = 0;
			imageBufferEntry.b = 0;
			imageBufferEntry.a = 0xFF;
		}
		else if(shadow && !highlight)
		{
			imageBufferEntry.r = paletteEntryTo8BitShadow[colorIntensityR];
			imageBufferEntry.g = paletteEntryTo8BitShadow[colorIntensityG];
			imageBufferEntry.b = paletteEntryTo8BitShadow[colorIntensityB];
			imageBufferEntry.a = 0xFF;
		}
		else if(highlight && !shadow)
		{
			imageBufferEntry.r = paletteEntryTo8BitHighlight[colorIntensityR];
			imageBufferEntry.g = paletteEntryTo8BitHighlight[colorIntensityG];
			imageBufferEntry.b = paletteEntryTo8BitHighlight[colorIntensityB];
			imageBufferEntry.a = 0xFF;
		}
		else
		{
			imageBufferEntry.r = paletteEntryTo8Bit[colorIntensityR];
			imageBufferEntry.g = paletteEntryTo8Bit[colorIntensityG];
			imageBufferEntry.b = paletteEntryTo8Bit[colorIntensityB];
			imageBufferEntry.a = 0xFF;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadHscrollData(unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, unsigned int& layerAHscrollPatternDisplacement, unsigned int& layerBHscrollPatternDisplacement, unsigned int& layerAHscrollMappingDisplacement, unsigned int& layerBHscrollMappingDisplacement) const
{
	//Calculate the address of the hscroll data to read for this line
	unsigned int hscrollDataAddress = hscrollDataBase;
	//##TODO## Based on the EA logo for Populous, it appears that the state of LSCR is
	//ignored when HSCR is not set. We should confirm this on hardware.
	if(hscrState)
	{
		const unsigned int hscrollDataPairSize = 4;
		hscrollDataAddress += lscrState? (screenRowNumber * hscrollDataPairSize): (((screenRowNumber / renderDigitalBlockPixelSizeY) * renderDigitalBlockPixelSizeY) * hscrollDataPairSize);
	}

	//Read the hscroll data for this line
	//##TODO## Confirm the way scrolling data is interpreted through hardware tests. Eg,
	//does -1 actually scroll to the left by one place, or are 0 and -1 equivalent?
	//##TODO## According to the official documentation, the upper 6 bits of the hscroll
	//data are unused, and are allowed to be used by software to store whatever values
	//they want. Confirm this on hardware.
	unsigned int layerAHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+1);
	unsigned int layerBHscrollOffset = ((unsigned int)vram->ReadCommitted(hscrollDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(hscrollDataAddress+3);

	//Break the hscroll data into its two component parts. The lower 4 bits represent a
	//displacement into the 2-cell column, or in other words, the displacement of the
	//starting pixel within each column, while the upper 6 bits represent an offset for
	//the column mapping data itself.
	//-----------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------|
	//|  Column Shift Value   | Displacement  |
	//-----------------------------------------
	layerAHscrollPatternDisplacement = (layerAHscrollOffset & 0x00F);
	layerAHscrollMappingDisplacement = (layerAHscrollOffset & 0x3F0) >> 4;
	layerBHscrollPatternDisplacement = (layerBHscrollOffset & 0x00F);
	layerBHscrollMappingDisplacement = (layerBHscrollOffset & 0x3F0) >> 4;
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadVscrollData(unsigned int screenColumnNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerAVscrollPatternDisplacement, unsigned int& layerBVscrollPatternDisplacement, unsigned int& layerAVscrollMappingDisplacement, unsigned int& layerBVscrollMappingDisplacement) const
{
	//Calculate the address of the vscroll data to read for this block
	const unsigned int vscrollDataPairSize = 4;
	unsigned int vscrollDataAddress = vscrState? (screenColumnNumber * vscrollDataPairSize): 0;

	//##FIX## This is a temporary workaround for handling render reads past the end of the
	//VSRAM buffer. We need to do more hardware tests on this behaviour, and implement
	//this properly.
	unsigned int layerAVscrollOffset = 0;
	unsigned int layerBVscrollOffset = 0;
	if(vscrollDataAddress < 0x50)
	{
		//Read the vscroll data for this line. Note only the lower 10 bits are
		//effective, or the lower 11 bits in the case of interlace mode 2, due to the
		//scrolled address being wrapped to lie within the total field boundaries,
		//which never exceed 128 blocks.
		layerAVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
		layerBVscrollOffset = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+2) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+3);
	}

	//Break the vscroll data into its two component parts. The format of the vscroll data
	//varies depending on whether interlace mode 2 is active. When interlace mode 2 is not
	//active, the vscroll data is interpreted as a 10-bit value, where the lower 3 bits
	//represent a vertical shift on the pattern line for the selected block mapping, or in
	//other words, the displacement of the starting row within each pattern, while the
	//upper 7 bits represent an offset for the mapping data itself, like so:
	//------------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
	//|----------------------------------------|
	//|    Column Shift Value     |Displacement|
	//------------------------------------------
	//Where interlace mode 2 is active, pattern data is 8x16 pixels, not 8x8 pixels. In
	//this case, the vscroll data is treated as an 11-bit value, where the lower 4 bits
	//give the row offset, and the upper 7 bits give the mapping offset, like so:
	//---------------------------------------------
	//|10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------------------|
	//|    Column Shift Value     | Displacement  |
	//---------------------------------------------
	//Note that the unused upper bits in the vscroll data are simply discarded, since they
	//fall outside the maximum virtual playfield size for the mapping data. Since the
	//virtual playfield wraps, this means they have no effect.
	layerAVscrollPatternDisplacement = interlaceMode2Active? (layerAVscrollOffset & 0x00F):      (layerAVscrollOffset & 0x007);
	layerAVscrollMappingDisplacement = interlaceMode2Active? (layerAVscrollOffset & 0x7F0) >> 4: (layerAVscrollOffset & 0x3F8) >> 3;
	layerBVscrollPatternDisplacement = interlaceMode2Active? (layerBVscrollOffset & 0x00F):      (layerBVscrollOffset & 0x007);
	layerBVscrollMappingDisplacement = interlaceMode2Active? (layerBVscrollOffset & 0x7F0) >> 4: (layerBVscrollOffset & 0x3F8) >> 3;
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadVscrollDataNew(unsigned int screenColumnNumber, unsigned int layerNumber, bool vscrState, bool interlaceMode2Active, unsigned int& layerVscrollPatternDisplacement, unsigned int& layerVscrollMappingDisplacement, Data& vsramReadCache) const
{
	//Calculate the address of the vscroll data to read for this block
	const unsigned int vscrollDataLayerCount = 2;
	const unsigned int vscrollDataEntrySize = 2;
	unsigned int vscrollDataAddress = vscrState? (screenColumnNumber * vscrollDataLayerCount * vscrollDataEntrySize) + (layerNumber * vscrollDataEntrySize): (layerNumber * vscrollDataEntrySize);

	//##NOTE## This implements what appears to be the correct behaviour for handling reads
	//past the end of the VSRAM buffer during rendering. This can occur when horizontal
	//scrolling is applied along with vertical scrolling, in which case the leftmost
	//column can be reading data from a screen column of -1, wrapping around to the end of
	//the VSRAM buffer. In this case, the last successfully read value from the VSRAM
	//appears to be used as the read value. This also applies when performing manual reads
	//from VSRAM externally using the data port. See data port reads from VSRAM for more
	//info.
	//##TODO## This needs more through hardware tests, to definitively confirm the correct
	//behaviour.
	if(vscrollDataAddress < 0x50)
	{
		//Read the vscroll data for this line. Note only the lower 10 bits are
		//effective, or the lower 11 bits in the case of interlace mode 2, due to the
		//scrolled address being wrapped to lie within the total field boundaries,
		//which never exceed 128 blocks.
		vsramReadCache = ((unsigned int)vsram->ReadCommitted(vscrollDataAddress+0) << 8) | (unsigned int)vsram->ReadCommitted(vscrollDataAddress+1);
	}
	else
	{
		//##FIX## This is a temporary patch until we complete our hardware testing on the
		//behaviour of VSRAM. Hardware tests do seem to confirm that when the VSRAM read
		//process passes into the undefined upper region of VSRAM, the returned value is
		//the ANDed result of the last two entries in VSRAM.
		vsramReadCache = ((unsigned int)vsram->ReadCommitted(0x4C+0) << 8) | (unsigned int)vsram->ReadCommitted(0x4D+1);
		vsramReadCache &= ((unsigned int)vsram->ReadCommitted(0x4E+0) << 8) | (unsigned int)vsram->ReadCommitted(0x4E+1);
	}

	//Break the vscroll data into its two component parts. The format of the vscroll data
	//varies depending on whether interlace mode 2 is active. When interlace mode 2 is not
	//active, the vscroll data is interpreted as a 10-bit value, where the lower 3 bits
	//represent a vertical shift on the pattern line for the selected block mapping, or in
	//other words, the displacement of the starting row within each pattern, while the
	//upper 7 bits represent an offset for the mapping data itself, like so:
	//------------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
	//|----------------------------------------|
	//|    Column Shift Value     |Displacement|
	//------------------------------------------
	//Where interlace mode 2 is active, pattern data is 8x16 pixels, not 8x8 pixels. In
	//this case, the vscroll data is treated as an 11-bit value, where the lower 4 bits
	//give the row offset, and the upper 7 bits give the mapping offset, like so:
	//---------------------------------------------
	//|10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|-------------------------------------------|
	//|    Column Shift Value     | Displacement  |
	//---------------------------------------------
	//Note that the unused upper bits in the vscroll data are simply discarded, since they
	//fall outside the maximum virtual playfield size for the mapping data. Since the
	//virtual playfield wraps, this means they have no effect.
	layerVscrollPatternDisplacement = interlaceMode2Active? vsramReadCache.GetDataSegment(0, 4): vsramReadCache.GetDataSegment(0, 3);
	layerVscrollMappingDisplacement = interlaceMode2Active? vsramReadCache.GetDataSegment(4, 7): vsramReadCache.GetDataSegment(3, 7);
}

//----------------------------------------------------------------------------------------
//This function performs all the necessary calculations to determine which mapping data to
//read for a given playfield position, and reads the corresponding mapping data pair from
//VRAM. The calculations performed appear to produce the same result as the real VDP
//hardware under all modes and settings, including when invalid scroll size modes are
//used.
//
//The following comments are provided as a supplement to the comments within this
//function, and show how the internally calculated row and column numbers are combined
//with the mapping base address data to produce a final VRAM address for the mapping
//block. All possible combinations of screen mode settings are shown, including invalid
//modes (VSZ="10" or HSZ="10"). Note that invalid combinations of screen mode settings are
//not shown, since invalid combinations never actually occur, due to the vertical screen
//mode being adjusted based on the horizontal screen mode, as outlined in the function
//comments below.
//
//Officially supported screen mode settings. Note that the lower two bits of the resulting
//address are masked before the address is used.
//Mapping data VRAM address (HSZ=00 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 |        Row        |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=01 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 |        Row        |         Column        | 0 | (Row Shift Count = 7)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=11 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |        Row        |           Column          | 0 | (Row Shift Count = 8)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=00 VSZ=01):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 |          Row          |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=01 VSZ=01):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |          Row          |         Column        | 0 | (Row Shift Count = 7)
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=00 VSZ=11):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr |            Row            |       Column      | 0 | (Row Shift Count = 6)
//-----------------------------------------------------------------
//
//Officially unsupported screen modes. In this case, the row and column data may be
//interleaved, and the row shift count may be 0, as shown below:
//Mapping data VRAM address (HSZ=00 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|           |Row| 0 |        Row        |                       | (Row Shift Count = 6)
//|           -------------------------------------------------   |
//|                                       |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr |Row| 0 |        Row        |       Column      | 0 |
//-----------------------------------------------------------------
//##FIX## Hardware tests have shown the two cases below to be incorrect. It appears the
//upper bit of the column data is never applied, and the row is never incremented when the
//invalid horizontal mode is active.
//Mapping data VRAM address (HSZ=10 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|                                           |        Row        | (Row Shift Count = 0)
//|                                       ------------------------|
//|                                       |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 |Col| 0 |       Column      |Row|
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=10 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//|                                   |Row| 0 |        Row        | (Row Shift Count = 0)
//|                               --------------------------------|
//|                               |Col| 0 |       Column      |   |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 |Col|Row|       Column      |Row|
//-----------------------------------------------------------------
//##TODO## Implement the correct mappings, which are as follows:
//Mapping data VRAM address (HSZ=10 VSZ=00):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
//-----------------------------------------------------------------
//Mapping data VRAM address (HSZ=10 VSZ=10):
//-----------------------------------------------------------------
//| 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//|---------------------------------------------------------------|
//| Base Addr | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       Column      | 0 |
//-----------------------------------------------------------------
//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadMappingDataPair(unsigned int screenRowNumber, unsigned int screenColumnNumber, bool interlaceMode2Active, unsigned int nameTableBaseAddress, unsigned int layerHscrollMappingDisplacement, unsigned int layerVscrollMappingDisplacement, unsigned int layerVscrollPatternDisplacement, unsigned int hszState, unsigned int vszState, Data& mappingDataEntry1, Data& mappingDataEntry2) const
{
	//The existence and contents of this table have been determined through analysis of
	//the behaviour of the VDP when invalid field size settings are selected. In
	//particular, the third table entry of 0 is used when the invalid HSZ mode of "10" is
	//selected. This causes the row and column to overlap when building the final address
	//value. The way the address is built in these circumstances can only be logically
	//explained by a lookup table being used for the row shift count, with the third entry
	//being set to 0, as we have implemented here.
	//##FIX## Hardware tests have shown this is not actually the case
	static const unsigned int rowShiftCountTable[4] = {6, 7, 0, 8};

	//The following calculation limits the vertical playfield size, based on the
	//horizontal playfield size. This calculation is quite simple in hardware, but looks
	//more complicated in code than it really is. Basically, the upper bit of the vertical
	//scroll mode is run through a NAND operation with the lower bit of the horizontal
	//scroll mode, and likewise, the lower bit of the vertical scroll mode is run through
	//a NAND operation with the upper bit of the horizontal scroll mode. This limits the
	//vertical scroll mode in the exact same way the real hardware does, including when
	//invalid scroll modes are being used.
	unsigned int screenSizeModeH = hszState;
	unsigned int screenSizeModeV = ((vszState & 0x1) & ((~hszState & 0x02) >> 1)) | ((vszState & 0x02) & ((~hszState & 0x01) << 1));

	//Build the playfield block masks. These masks ultimately determine the boundaries of
	//the playfield horizontally and vertically. Hardware testing and analysis on the
	//scrolling behaviour of the VDP indicate that these masks are built by mapping the
	//HSZ and VSZ bits to the upper bits of a 7-bit mask value, as shown below. Note that
	//the invalid mode setting of "10" results in a mask where bit 7 is set, and bit 6 is
	//unset. This has been confirmed through hardware tests.
	//##TODO## Test this on hardware.
	//##TODO## Test and confirm how the window distortion bug interacts with this block
	//mapping selection process.
	unsigned int playfieldBlockMaskX = (screenSizeModeH << 5) | 0x1F;
	unsigned int playfieldBlockMaskY = (screenSizeModeV << 5) | 0x1F;

	//##TODO## Update this comment
	//Calculate the row and column numbers for the mapping data. This is simply done by
	//converting the calculated playfield position from a pixel index into a block index,
	//then masking the X and Y coordinates by the horizontal and vertical block masks.
	//This gives us a row and column number, wrapped to the playfield boundaries.
	//##TODO## We want to keep this shift method for calculating the row and column,
	//rather than using division, but we should be using a constant, or at least
	//explaining why the magic number "3" is being used.
	//##TODO## Update these comments
	//##TODO## Document why we add the horizontal scroll value, but subtract the vertical
	//scroll value.
	const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
	unsigned int mappingDataRowNumber = (((screenRowNumber + layerVscrollPatternDisplacement) / rowsPerTile) + layerVscrollMappingDisplacement) & playfieldBlockMaskY;
	unsigned int mappingDataColumnNumber = ((screenColumnNumber - layerHscrollMappingDisplacement) * cellsPerColumn) & playfieldBlockMaskX;

	//Based on the horizontal playfield mode, lookup the row shift count to use when
	//building the final mapping data address value. The column shift count is always
	//fixed to 1.
	unsigned int rowShiftCount = rowShiftCountTable[screenSizeModeH];
	const unsigned int columnShiftCount = 1;

	//Calculate the final mapping data address. Note that the row number is masked with
	//the inverted mask for the column number, so that row data is only allowed to appear
	//where column data is not allowed to appear. This is based on the observed behaviour
	//of the system, as is critical in order to correctly emulate the scrolling behaviour
	//where an invalid horizontal scroll mode of "10" is applied. In this case, the row
	//data can be interleaved with the column data, since the row shift count under this
	//mode is 0.
	unsigned int mappingDataAddress = nameTableBaseAddress | ((mappingDataRowNumber << rowShiftCount) & (~playfieldBlockMaskX << columnShiftCount)) | (mappingDataColumnNumber << columnShiftCount);

	//Mask the lower two bits of the mapping data address, to align the mapping address
	//with a 4-byte boundary. The VDP can only read data from the VRAM in aligned 4-byte
	//blocks, so the lower two bits of the address are ineffective. We read a pair of
	//2-byte block mappings from the masked address.
	mappingDataAddress &= 0xFFFC;

	//Read target layer mapping data
	mappingDataEntry1 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+0) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+1);
	mappingDataEntry2 = ((unsigned int)vram->ReadCommitted(mappingDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(mappingDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderReadPatternDataRow(unsigned int patternRowNumberNoFlip, unsigned int patternCellOffset, bool interlaceMode2Active, const Data& mappingData, Data& patternData) const
{
	//Calculate the final number of the pattern row to read, taking into account vertical
	//flip if it is specified in the block mapping.
	//Mapping (Pattern Name) data format:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|Pri|PalRow |VF |HF |              Pattern Number               |
	//-----------------------------------------------------------------
	//Pri:    Priority Bit
	//PalRow: The palette row number to use when displaying the pattern data
	//VF:     Vertical Flip
	//HF:     Horizontal Flip
	const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
	unsigned int patternRowNumber = mappingData.GetBit(12)? (rowsPerTile - 1) - patternRowNumberNoFlip: patternRowNumberNoFlip;

	//The address of the pattern data to read is determined by combining the number of the
	//pattern (tile) with the row of the pattern to be read. The way the data is combined
	//is different under interlace mode 2, where patterns are 16 pixels high instead of
	//the usual 8 pixels. The format for pattern data address decoding is as follows when
	//interlace mode 2 is not active:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|              Pattern Number               |Pattern Row| 0 | 0 |
	//-----------------------------------------------------------------
	//When interlace mode 2 is active, the pattern data address decoding is as follows:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|            Pattern Number             |  Pattern Row  | 0 | 0 |
	//-----------------------------------------------------------------
	//Note that we grab the entire mapping data block as the block number when calculating
	//the address. This is because the resulting address is wrapped to keep it within the
	//VRAM boundaries. Due to this wrapping, in reality only the lower 11 bits of the
	//mapping data are effective when determining the block number, or the lower 10 bits
	//in the case of interlace mode 2.
	//##TODO## Test the above assertion on the TeraDrive with the larger VRAM mode active
	const unsigned int patternDataRowByteSize = 4;
	const unsigned int blockPatternByteSize = rowsPerTile * patternDataRowByteSize;
	unsigned int patternDataAddress = (((mappingData.GetData() + patternCellOffset) * blockPatternByteSize) + (patternRowNumber * patternDataRowByteSize)) % vramSize;

	//Read the target pattern row
	patternData = ((unsigned int)vram->ReadCommitted(patternDataAddress+0) << 24) | ((unsigned int)vram->ReadCommitted(patternDataAddress+1) << 16) | ((unsigned int)vram->ReadCommitted(patternDataAddress+2) << 8) | (unsigned int)vram->ReadCommitted(patternDataAddress+3);
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderBuildSpriteList(unsigned int screenRowNumber, bool interlaceMode2Active, bool screenModeRS1Active, unsigned int& nextTableEntryToRead, bool& spriteSearchComplete, bool& spriteOverflow, unsigned int& spriteDisplayCacheEntryCount, std::vector<SpriteDisplayCacheEntry>& spriteDisplayCache) const
{
	if(!spriteSearchComplete && !spriteOverflow)
	{
		const unsigned int spriteCacheEntrySize = 4;
		const unsigned int spriteAttributeTableSize = (screenModeRS1Active)? 80: 64;
		const unsigned int spritePosScreenStartH = 0x80;
		const unsigned int spritePosScreenStartV = (interlaceMode2Active)? 0x100: 0x80;
		const unsigned int spritePosBitCountV = (interlaceMode2Active)? 10: 9;
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		const unsigned int renderSpriteDisplayCacheSize = (screenModeRS1Active)? 20: 16;
		const unsigned int renderSpriteCellDisplayCacheSize = (screenModeRS1Active)? 40: 32;

		//Calculate the address in the sprite cache of the next sprite to read data for
		unsigned int spriteCacheAddress = (nextTableEntryToRead * spriteCacheEntrySize);

		//Read all available data on the next sprite from the sprite cache
		Data spriteVPosData(16);
		Data spriteSizeAndLinkData(16);
		spriteVPosData = ((unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+0) << 8) | (unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+1);
		spriteSizeAndLinkData = ((unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+2) << 8) | (unsigned int)spriteCache->ReadCommitted(spriteCacheAddress+3);

		//Calculate the width and height of this sprite in cells
		unsigned int spriteHeightInCells = spriteSizeAndLinkData.GetDataSegment(8, 2) + 1;

		//Calculate the relative position of the current active display line in sprite
		//space.
		//##TODO## Lay this code out better, and provide more comments on what's being
		//done to support interlace mode 2.
		//##TODO## Handle interlace mode 2 properly. In interlace mode 2, the current
		//screen row number is effectively doubled, with the current state of the odd flag
		//used as the LSB of the line number. We need to do that here.
		unsigned int currentScreenRowInSpriteSpace = screenRowNumber + spritePosScreenStartV;
		if(interlaceMode2Active)
		{
			currentScreenRowInSpriteSpace = (screenRowNumber * 2) + spritePosScreenStartV;
			if(renderDigitalOddFlagSet)
			{
				currentScreenRowInSpriteSpace += 1;
			}
		}

		//Calculate the vertical position of the sprite, discarding any unused bits.
		Data spriteVPos(spritePosBitCountV);
		spriteVPos = spriteVPosData;

		//If this next sprite is within the current display row, add it to the list of
		//sprites to display on this line.
		unsigned int spriteHeightInPixels = spriteHeightInCells * rowsPerTile;
		if((spriteVPos <= currentScreenRowInSpriteSpace) && ((spriteVPos + spriteHeightInPixels) > currentScreenRowInSpriteSpace))
		{
			//We perform a check for a sprite overflow here. If we exceed the maximum
			//number of sprites for this line, we set the sprite overflow flag, otherwise
			//we load all the sprite data from the sprite cache for this sprite.
			if(spriteDisplayCacheEntryCount < renderSpriteDisplayCacheSize)
			{
				spriteDisplayCache[spriteDisplayCacheEntryCount].spriteTableIndex = nextTableEntryToRead;
				spriteDisplayCache[spriteDisplayCacheEntryCount].spriteRowIndex = (currentScreenRowInSpriteSpace - spriteVPos.GetData());
				spriteDisplayCache[spriteDisplayCacheEntryCount].vpos = spriteVPosData;
				spriteDisplayCache[spriteDisplayCacheEntryCount].sizeAndLinkData = spriteSizeAndLinkData;
				++spriteDisplayCacheEntryCount;
			}
			else
			{
				spriteOverflow = true;
			}
		}

		//Use the link data to determine which sprite table entry to read data for next.
		//The sprite search is terminated if we encounter a sprite with a link data value
		//of 0, or if a link data value is specified which is outside the bounds of the
		//sprite table, based on the current screen mode settings.
		nextTableEntryToRead = spriteSizeAndLinkData.GetDataSegment(0, 7);
		if((nextTableEntryToRead == 0) || (nextTableEntryToRead >= spriteAttributeTableSize))
		{
			spriteSearchComplete = true;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DigitalRenderBuildSpriteCellList(const HScanSettings& hscanSettings, const VScanSettings& vscanSettings, unsigned int spriteDisplayCacheIndex, unsigned int spriteTableBaseAddress, bool interlaceMode2Active, bool screenModeRS1Active, bool& spriteDotOverflow, SpriteDisplayCacheEntry& spriteDisplayCacheEntry, unsigned int& spriteCellDisplayCacheEntryCount, std::vector<SpriteCellDisplayCacheEntry>& spriteCellDisplayCache) const
{
	if(!spriteDotOverflow)
	{
		//##TODO## Tidy up this list of constants
		const unsigned int spriteCacheEntrySize = 4;
		const unsigned int spriteTableEntrySize = 8;
		const unsigned int spriteAttributeTableSize = (screenModeRS1Active)? 80: 64;
		const unsigned int spritePosScreenStartH = 0x80;
		const unsigned int spritePosScreenStartV = (interlaceMode2Active)? 0x100: 0x80;
		const unsigned int rowsPerTile = (!interlaceMode2Active)? 8: 16;
		const unsigned int renderSpriteDisplayCacheSize = (screenModeRS1Active)? 20: 16;
		const unsigned int renderSpriteCellDisplayCacheSize = (screenModeRS1Active)? 40: 32;

		//Calculate the address in VRAM of this sprite table entry
		unsigned int spriteTableEntryAddress = spriteTableBaseAddress + (spriteDisplayCacheEntry.spriteTableIndex * spriteTableEntrySize);

		//Read all remaining data for the sprite from the sprite attribute table in VRAM.
		//The first 4 bytes of the sprite attribute entry have already been loaded from
		//the internal sprite cache.
		//##TODO## Perform hardware tests, to confirm if the data from the sprite cache is
		//buffered after it is read the first time while parsing the sprite table to
		//determine the list of sprites on the current line, or if the data is read again
		//from the cache during the mapping decoding.
		Data spriteMappingData(16);
		Data spriteHPosData(16);
		spriteMappingData = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+4) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+5);
		spriteHPosData = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+6) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+7);

		//Load the remaining data into the sprite display cache
		spriteDisplayCacheEntry.mappingData = spriteMappingData;
		spriteDisplayCacheEntry.hpos = spriteHPosData;

		//Calculate the width and height of this sprite in cells
		unsigned int spriteWidthInCells = spriteDisplayCacheEntry.sizeAndLinkData.GetDataSegment(10, 2) + 1;
		unsigned int spriteHeightInCells = spriteDisplayCacheEntry.sizeAndLinkData.GetDataSegment(8, 2) + 1;

		//Extract the vertical flip and horizontal flip flags from the sprite mapping.
		//Unlike vertical and horizontal flip for normal pattern cells in the scroll
		//layers, when horizontal flip or vertical flip is applied to a sprite, the entire
		//sprite is flipped, so that the cell order itself is reversed, as well as the
		//order of the pixels within each cell. We extract the vertical flip and
		//horizontal flip flags here so we can emulate that behaviour.
		bool hflip = spriteMappingData.GetBit(11);
		bool vflip = spriteMappingData.GetBit(12);

		//Calculate the cell row number, and pattern row number within the cell, which
		//fall on the current display line for this sprite.
		unsigned int spriteCellRowNumber = spriteDisplayCacheEntry.spriteRowIndex / rowsPerTile;
		unsigned int spriteCellPatternRowNumber = spriteDisplayCacheEntry.spriteRowIndex % rowsPerTile;

		//Apply vertical flip to the sprite cell number if vertical flip is active. Note
		//that we do not invert the pattern row number here, just the cell row number. All
		//we're doing at this stage of sprite processing is decoding the cells within the
		//sprite that fall on the current display line. Vertical flip is applied to the
		//pattern row within a cell at a later stage of sprite processing.
		if(vflip)
		{
			spriteCellRowNumber = ((spriteHeightInCells - 1) - spriteCellRowNumber);
		}

		//Add details of each cell in this sprite on the current display line to the
		//internal sprite pattern render list.
		for(unsigned int i = 0; i < spriteWidthInCells; ++i)
		{
			//Record sprite boundary information for sprite boxing support if requested
			if(videoEnableSpriteBoxing && (spriteCellDisplayCacheEntryCount < renderSpriteCellDisplayCacheSize))
			{
				boost::mutex::scoped_lock spriteLock(spriteBoundaryMutex[drawingImageBufferPlane]);

				//Calculate the position of this sprite relative to the screen
				const unsigned int cellWidthInPixels = 8;
				Data spritePosH(9, spriteDisplayCacheEntry.hpos.GetData());
				int spriteHeightDivider = (!interlaceMode2Active)? 1: 2;
				int spritePosXInScreenSpace = ((int)spritePosH.GetData() - (int)spritePosScreenStartH) + (int)hscanSettings.leftBorderPixelCount;
				int spritePosYInScreenSpace = (((int)spriteDisplayCacheEntry.vpos.GetData() - (int)spritePosScreenStartV) / spriteHeightDivider) + (int)vscanSettings.topBorderLineCount;

				//If this is the first cell column for the sprite, draw a horizontal line
				//down the left boundary of the sprite.
				if(i == 0)
				{
					SpriteBoundaryLineEntry spriteBoundaryLineEntry;
					spriteBoundaryLineEntry.linePosXStart = spritePosXInScreenSpace;
					spriteBoundaryLineEntry.linePosXEnd = spritePosXInScreenSpace;
					spriteBoundaryLineEntry.linePosYStart = spritePosYInScreenSpace + ((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider);
					spriteBoundaryLineEntry.linePosYEnd = spritePosYInScreenSpace + (((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider) + 1);
					imageBufferSpriteBoundaryLines[drawingImageBufferPlane].push_back(spriteBoundaryLineEntry);
				}

				//If this is the last cell column for the sprite, draw a horizontal line
				//down the right boundary of the sprite.
				if(((i + 1) == spriteWidthInCells) || ((spriteCellDisplayCacheEntryCount + 1) == renderSpriteCellDisplayCacheSize))
				{
					SpriteBoundaryLineEntry spriteBoundaryLineEntry;
					spriteBoundaryLineEntry.linePosXStart = spritePosXInScreenSpace + (int)((i + 1) * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosXEnd = spritePosXInScreenSpace + (int)((i + 1) * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosYStart = spritePosYInScreenSpace + ((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider);
					spriteBoundaryLineEntry.linePosYEnd = spritePosYInScreenSpace + (((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider) + 1);
					imageBufferSpriteBoundaryLines[drawingImageBufferPlane].push_back(spriteBoundaryLineEntry);
				}

				//If this is the first line for the sprite, draw a horizontal line across
				//the top boundary of the sprite.
				if((spriteDisplayCacheEntry.spriteRowIndex == 0) || (interlaceMode2Active && (spriteDisplayCacheEntry.spriteRowIndex == 1)))
				{
					SpriteBoundaryLineEntry spriteBoundaryLineEntry;
					spriteBoundaryLineEntry.linePosXStart = spritePosXInScreenSpace + (int)(i * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosXEnd = spritePosXInScreenSpace + (int)((i + 1) * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosYStart = spritePosYInScreenSpace;
					spriteBoundaryLineEntry.linePosYEnd = spritePosYInScreenSpace;
					imageBufferSpriteBoundaryLines[drawingImageBufferPlane].push_back(spriteBoundaryLineEntry);
				}

				//If this is the last line for the sprite, draw a horizontal line across
				//the bottom boundary of the sprite.
				if(((spriteDisplayCacheEntry.spriteRowIndex + 1) == (spriteHeightInCells * rowsPerTile)) || (interlaceMode2Active && ((spriteDisplayCacheEntry.spriteRowIndex + 2) >= (spriteHeightInCells * rowsPerTile))))
				{
					SpriteBoundaryLineEntry spriteBoundaryLineEntry;
					spriteBoundaryLineEntry.linePosXStart = spritePosXInScreenSpace + (int)(i * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosXEnd = spritePosXInScreenSpace + (int)((i + 1) * cellWidthInPixels);
					spriteBoundaryLineEntry.linePosYStart = spritePosYInScreenSpace + (((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider) + 1);
					spriteBoundaryLineEntry.linePosYEnd = spritePosYInScreenSpace + (((int)spriteDisplayCacheEntry.spriteRowIndex / spriteHeightDivider) + 1);
					imageBufferSpriteBoundaryLines[drawingImageBufferPlane].push_back(spriteBoundaryLineEntry);
				}
			}

			//We perform a check for a sprite dot overflow here. If each sprite is 2 cells
			//wide, and the maximum number of sprites for a line are present, we generate
			//the exact maximum number of sprite dots per line. If sprite widths are 3
			//cells or wider, we can exceed the maximum sprite dot count at this point.
			if(spriteCellDisplayCacheEntryCount < renderSpriteCellDisplayCacheSize)
			{
				//Record the reference from this individual sprite cell back to the sprite
				//display cache entry that holds the mapping data for the cell.
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].spriteDisplayCacheIndex = spriteDisplayCacheIndex;

				//Calculate the cell offset into the sprite pattern data for this cell,
				//taking into account horizontal flip. Note that pattern cells for sprites
				//are ordered in columns, not rows. The pattern data for the top left cell
				//appears first, followed by the pattern data for the second row on the
				//leftmost column, and so forth to the end of that column, followed by the
				//pattern data for the topmost cell in the second column, and so on.
				unsigned int spriteCellHorizontalOffset = (hflip)? (spriteWidthInCells - 1) - i: i;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].patternCellOffset = (spriteCellHorizontalOffset * spriteHeightInCells) + spriteCellRowNumber;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].patternRowOffset = spriteCellPatternRowNumber;
				spriteCellDisplayCache[spriteCellDisplayCacheEntryCount].spriteCellColumnNo = i;

				//Advance to the next available entry in the sprite cell cache
				++spriteCellDisplayCacheEntryCount;
			}
			else
			{
				spriteDotOverflow = true;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
unsigned int S315_5313::DigitalRenderReadPixelIndex(const Data& patternRow, bool horizontalFlip, unsigned int pixelIndex) const
{
	const unsigned int patternDataPixelEntryBitCount = 4;

	if(!horizontalFlip)
	{
		//Pattern data row format (no horizontal flip):
		//---------------------------------------------------------------------------------------------------------------------------------
		//|31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|-------------------------------------------------------------------------------------------------------------------------------|
		//|    Pixel 1    |    Pixel 2    |    Pixel 3    |    Pixel 4    |    Pixel 5    |    Pixel 6    |    Pixel 7    |    Pixel 8    |
		//---------------------------------------------------------------------------------------------------------------------------------
		return patternRow.GetDataSegment(((cellBlockSizeH - 1) - pixelIndex) * patternDataPixelEntryBitCount, 4);
	}
	else
	{
		//Pattern data row format (with horizontal flip):
		//---------------------------------------------------------------------------------------------------------------------------------
		//|31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|-------------------------------------------------------------------------------------------------------------------------------|
		//|    Pixel 8    |    Pixel 7    |    Pixel 6    |    Pixel 5    |    Pixel 4    |    Pixel 3    |    Pixel 2    |    Pixel 1    |
		//---------------------------------------------------------------------------------------------------------------------------------
		return patternRow.GetDataSegment(pixelIndex * patternDataPixelEntryBitCount, 4);
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::CalculateLayerPriorityIndex(unsigned int& layerIndex, bool& shadow, bool& highlight, bool shadowHighlightEnabled, bool spriteIsShadowOperator, bool spriteIsHighlightOperator, bool foundSpritePixel, bool foundLayerAPixel, bool foundLayerBPixel, bool prioritySprite, bool priorityLayerA, bool priorityLayerB) const
{
	//Initialize the shadow/highlight flags
	shadow = false;
	highlight = false;

	//Perform layer priority calculations
	if(!shadowHighlightEnabled)
	{
		//Perform standard layer priority calculations
		if(foundSpritePixel && prioritySprite)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
		}
		else if(foundSpritePixel)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
		}
	}
	else
	{
		//Perform shadow/highlight mode layer priority calculations. Note that some
		//illustrations in the official documentation from Sega demonstrating the
		//behaviour of shadow/highlight mode are incorrect. In particular, the third and
		//fifth illustrations on page 64 of the "Genesis Software Manual", showing layers
		//B and A being shadowed when a shadow sprite operator is at a lower priority, are
		//incorrect. If any layer is above an operator sprite pixel, the sprite operator
		//is ignored, and the higher priority pixel is output without the sprite operator
		//being applied. This has been confirmed through hardware tests. All other
		//illustrations describing the operation of shadow/highlight mode in relation to
		//layer priority settings appear to be correct.
		if(foundSpritePixel && prioritySprite && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
		}
		else if(foundLayerAPixel && priorityLayerA)
		{
			layerIndex = LAYER_LAYERA;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel && priorityLayerB)
		{
			layerIndex = LAYER_LAYERB;
			if(prioritySprite && spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(prioritySprite && spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundSpritePixel && !spriteIsShadowOperator && !spriteIsHighlightOperator)
		{
			layerIndex = LAYER_SPRITE;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
		}
		else if(foundLayerAPixel)
		{
			layerIndex = LAYER_LAYERA;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else if(foundLayerBPixel)
		{
			layerIndex = LAYER_LAYERB;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}
		else
		{
			layerIndex = LAYER_BACKGROUND;
			if(!priorityLayerA && !priorityLayerB)
			{
				shadow = true;
			}
			if(spriteIsShadowOperator)
			{
				shadow = true;
			}
			else if(spriteIsHighlightOperator)
			{
				highlight = true;
			}
		}

		//If shadow and highlight are both set, they cancel each other out. This is why a
		//sprite acting as a highlight operator is unable to highlight layer A, B, or the
		//background, if layers A and B both have their priority bits unset. This has been
		//confirmed on the hardware.
		if(shadow && highlight)
		{
			shadow = false;
			highlight = false;
		}
	}
}

//----------------------------------------------------------------------------------------
//Sprite list debugging functions
//----------------------------------------------------------------------------------------
S315_5313::SpriteMappingTableEntry S315_5313::GetSpriteMappingTableEntry(unsigned int entryNo) const
{
	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Read the sprite table base address register
	unsigned int spriteTableBaseAddress = M5GetNameTableBaseSprite(accessTarget);

	//According to official documentation, if we're in H40 mode, the AT9 bit of the sprite
	//table base address is masked. We emulate that here. Note that the "Traveller's
	//Tales" logo in Sonic 3D on the Mega Drive relies on AT9 being valid in H32 mode.
	//##TODO## Confirm this behaviour through hardware tests
	bool screenModeRS1Active = M5GetRS1(accessTarget);
	if(screenModeRS1Active)
	{
		spriteTableBaseAddress &= 0xFC00;
	}

	//Calculate the address in VRAM of this sprite table entry
	const unsigned int spriteTableEntrySize = 8;
	unsigned int spriteTableEntryAddress = spriteTableBaseAddress + (entryNo * spriteTableEntrySize);
	spriteTableEntryAddress %= vramSize;

	//Read all raw data for the sprite from the sprite attribute table in VRAM
	SpriteMappingTableEntry entry;
	entry.rawDataWord0 = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+0) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+1);
	entry.rawDataWord1 = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+2) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+3);
	entry.rawDataWord2 = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+4) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+5);
	entry.rawDataWord3 = ((unsigned int)vram->ReadCommitted(spriteTableEntryAddress+6) << 8) | (unsigned int)vram->ReadCommitted(spriteTableEntryAddress+7);

	//Decode the sprite mapping data
	//        -----------------------------------------------------------------
	//        |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// Word0  |---------------------------------------------------------------|
	//        |                          Vertical Pos                         |
	//        -----------------------------------------------------------------
	//        -----------------------------------------------------------------
	//        |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// Word1  |---------------------------------------------------------------|
	//        | /   /   /   / | HSize | VSize | / |         Link Data         |
	//        -----------------------------------------------------------------
	//        HSize:     Horizontal size of the sprite
	//        VSize:     Vertical size of the sprite
	//        Link Data: Next sprite entry to read from table during sprite rendering
	//        -----------------------------------------------------------------
	//        |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// Word2  |---------------------------------------------------------------|
	//        |Pri|PalRow |VF |HF |              Pattern Number               |
	//        -----------------------------------------------------------------
	//        Pri:    Priority Bit
	//        PalRow: The palette row number to use when displaying the pattern data
	//        VF:     Vertical Flip
	//        HF:     Horizontal Flip
	//        Mapping (Pattern Name) data format:
	//        -----------------------------------------------------------------
	//        |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// Word3  |---------------------------------------------------------------|
	//        |                         Horizontal Pos                        |
	//        -----------------------------------------------------------------
	entry.ypos = entry.rawDataWord0.GetData();
	entry.width = entry.rawDataWord1.GetDataSegment(10, 2);
	entry.height = entry.rawDataWord1.GetDataSegment(8, 2);
	entry.link = entry.rawDataWord1.GetDataSegment(0, 7);
	entry.priority = entry.rawDataWord2.GetBit(15);
	entry.paletteLine = entry.rawDataWord2.GetDataSegment(13, 2);
	entry.vflip = entry.rawDataWord2.GetBit(12);
	entry.hflip = entry.rawDataWord2.GetBit(11);
	entry.blockNumber = entry.rawDataWord2.GetDataSegment(0, 11);
	entry.xpos = entry.rawDataWord3.GetData();

	return entry;
}

//----------------------------------------------------------------------------------------
void S315_5313::SetSpriteMappingTableEntry(unsigned int entryNo, const SpriteMappingTableEntry& entry, bool useSeparatedData)
{
	AccessTarget accessTarget;
	accessTarget.AccessLatest();

	//Read the sprite table base address register
	unsigned int spriteTableBaseAddress = M5GetNameTableBaseSprite(accessTarget);

	//According to official documentation, if we're in H40 mode, the AT9 bit of
	//the sprite table base address is masked. We emulate that here. Note that the
	//"Traveller's Tales" logo in Sonic 3D on the Mega Drive relies on AT9 being
	//valid in H32 mode.
	bool screenModeRS1Active = M5GetRS1(accessTarget);
	if(screenModeRS1Active)
	{
		spriteTableBaseAddress &= 0xFC00;
	}

	//Select the data to write back to the sprite table
	Data rawDataWord0(entry.rawDataWord0);
	Data rawDataWord1(entry.rawDataWord1);
	Data rawDataWord2(entry.rawDataWord2);
	Data rawDataWord3(entry.rawDataWord3);
	if(useSeparatedData)
	{
		rawDataWord0 = entry.ypos;
		rawDataWord1.SetDataSegment(10, 2, entry.width);
		rawDataWord1.SetDataSegment(8, 2, entry.height);
		rawDataWord1.SetDataSegment(0, 7, entry.link);
		rawDataWord2.SetBit(15, entry.priority);
		rawDataWord2.SetDataSegment(13, 2, entry.paletteLine);
		rawDataWord2.SetBit(12, entry.vflip);
		rawDataWord2.SetBit(11, entry.hflip);
		rawDataWord2.SetDataSegment(0, 11, entry.blockNumber);
		rawDataWord3 = entry.xpos;
	}

	//Calculate the address in VRAM of this sprite table entry
	const unsigned int spriteTableEntrySize = 8;
	unsigned int spriteTableEntryAddress = spriteTableBaseAddress + (entryNo * spriteTableEntrySize);
	spriteTableEntryAddress %= vramSize;

	//Write the raw data for the sprite to the sprite attribute table in VRAM
	vram->WriteLatest(spriteTableEntryAddress+0, (unsigned char)rawDataWord0.GetUpperHalf());
	vram->WriteLatest(spriteTableEntryAddress+1, (unsigned char)rawDataWord0.GetLowerHalf());
	vram->WriteLatest(spriteTableEntryAddress+2, (unsigned char)rawDataWord1.GetUpperHalf());
	vram->WriteLatest(spriteTableEntryAddress+3, (unsigned char)rawDataWord1.GetLowerHalf());
	vram->WriteLatest(spriteTableEntryAddress+4, (unsigned char)rawDataWord2.GetUpperHalf());
	vram->WriteLatest(spriteTableEntryAddress+5, (unsigned char)rawDataWord2.GetLowerHalf());
	vram->WriteLatest(spriteTableEntryAddress+6, (unsigned char)rawDataWord3.GetUpperHalf());
	vram->WriteLatest(spriteTableEntryAddress+7, (unsigned char)rawDataWord3.GetLowerHalf());

	//Calculate the address in the internal sprite cache of the cached portion of this
	//sprite entry
	const unsigned int spriteCacheEntrySize = 4;
	unsigned int spriteCacheTableEntryAddress = spriteTableBaseAddress + (entryNo * spriteCacheEntrySize);
	spriteCacheTableEntryAddress %= spriteCacheSize;

	//Update the sprite cache to make it contain the new data
	spriteCache->WriteLatest(spriteCacheTableEntryAddress+0, (unsigned char)rawDataWord0.GetUpperHalf());
	spriteCache->WriteLatest(spriteCacheTableEntryAddress+1, (unsigned char)rawDataWord0.GetLowerHalf());
	spriteCache->WriteLatest(spriteCacheTableEntryAddress+2, (unsigned char)rawDataWord1.GetUpperHalf());
	spriteCache->WriteLatest(spriteCacheTableEntryAddress+3, (unsigned char)rawDataWord1.GetLowerHalf());
}
