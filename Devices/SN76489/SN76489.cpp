#include "SN76489.h"
#include <sstream>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::SN76489(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID), reg(channelCount * 2, false, Data(toneRegisterBitCount))
{
	//Initialize the audio output stream
	outputSampleRate = 48000;	//44100;
	outputStream.Open(1, 16, outputSampleRate, outputSampleRate/4, outputSampleRate/20);

	//Initialize the locked register state
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		channelVolumeRegisterLocked[i] = false;
		channelDataRegisterLocked[i] = false;
	}

	//##TODO## Provide a way for these properties to be defined externally, and provide
	//debug windows which can modify them on the fly.
	externalClockRate = 0.0;
	externalClockDivider = 16.0;
	shiftRegisterBitCount = 16;
	shiftRegisterDefaultValue = 0x8000;
	noiseWhiteTappedBitMask = 0x0009;
	noisePeriodicTappedBitMask = 0x0001;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int SN76489::GetISN76489Version() const
{
	return ThisISN76489Version();
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool SN76489::BuildDevice()
{
	//Initialize the wave logging state
	std::wstring captureFolder = GetSystemInterface().GetCapturePath();
	wavLoggingEnabled = false;
	std::wstring wavLoggingFileName = GetDeviceInstanceName() + L".wav";
	wavLoggingPath = PathCombinePaths(captureFolder, wavLoggingFileName);
	for(unsigned int channelNo = 0; channelNo < channelCount; ++channelNo)
	{
		wavLoggingChannelEnabled[channelNo] = false;
		std::wstringstream wavLoggingChannelFileName;
		wavLoggingChannelFileName << GetDeviceInstanceName() << L" - C" << channelNo << L".wav";
		wavLoggingChannelPath[channelNo] = PathCombinePaths(captureFolder, wavLoggingChannelFileName.str());
	}

	//Register each data source with the generic data access base class
	std::wstring audioLogExtensionFilter = L"Wave file|wav";
	std::wstring audioLogDefaultExtension = L"wav";
	GenericAccessDataInfo* dataInfoShiftRegister;
	GenericAccessDataInfo* dataInfoShiftRegisterDefaultValue;
	GenericAccessDataInfo* dataInfoShiftRegisterWhiteNoiseBits;
	GenericAccessDataInfo* dataInfoShiftRegisterPeriodicNoiseBits;
	bool result = true;
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL1_VOLUMEREGISTER, IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<volumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL2_VOLUMEREGISTER, IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<volumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL3_VOLUMEREGISTER, IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<volumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_VOLUMEREGISTER, IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<volumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL1_TONEREGISTER,   IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<toneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL2_TONEREGISTER,   IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<toneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL3_TONEREGISTER,   IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<toneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_TONEREGISTER,   IGenericAccessDataValue::DATATYPE_UINT))->SetLockingSupported(true)->SetUIntMaxValue((1<<toneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_NOISETYPE, IGenericAccessDataValue::DATATYPE_BOOL))->SetLockingSupported(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_NOISEPERIOD, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue(3));
	result &= AddGenericDataInfo(dataInfoShiftRegister = (new GenericAccessDataInfo(DATASOURCE_NOISESHIFTREGISTER, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_LATCHEDCHANNELNO, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue(channelCount-1));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_VOLUMEREGISTERLATCHED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_EXTERNALCLOCKRATE, IGenericAccessDataValue::DATATYPE_DOUBLE))->SetDoubleMinValue(0.0));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_EXTERNALCLOCKDIVIDER, IGenericAccessDataValue::DATATYPE_DOUBLE))->SetDoubleMinValue(1.0));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_SHIFTREGISTERBITCOUNT, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMinValue(1)->SetUIntMaxValue(32)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_DECIMAL));
	result &= AddGenericDataInfo(dataInfoShiftRegisterDefaultValue = (new GenericAccessDataInfo(DATASOURCE_SHIFTREGISTERDEFAULTVALUE, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo(dataInfoShiftRegisterWhiteNoiseBits = (new GenericAccessDataInfo(DATASOURCE_WHITENOISETAPPEDBITMASK, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo(dataInfoShiftRegisterPeriodicNoiseBits = (new GenericAccessDataInfo(DATASOURCE_PERIODICNOISETAPPEDBITMASK, IGenericAccessDataValue::DATATYPE_UINT))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::INTDISPLAYMODE_HEXADECIMAL));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_AUDIOLOGGINGENABLED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_AUDIOLOGGINGPATH, IGenericAccessDataValue::DATATYPE_FILEPATH))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL2_AUDIOLOGGINGENABLED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL3_AUDIOLOGGINGENABLED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_AUDIOLOGGINGENABLED, IGenericAccessDataValue::DATATYPE_BOOL)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH, IGenericAccessDataValue::DATATYPE_FILEPATH))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL2_AUDIOLOGGINGPATH, IGenericAccessDataValue::DATATYPE_FILEPATH))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL3_AUDIOLOGGINGPATH, IGenericAccessDataValue::DATATYPE_FILEPATH))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(DATASOURCE_CHANNEL4_AUDIOLOGGINGPATH, IGenericAccessDataValue::DATATYPE_FILEPATH))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));

	//Save references to the data info structures which contain values that need to have
	//their limits changed when the bitcount for the shift register is changed.
	genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegister);
	genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterDefaultValue);
	genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterWhiteNoiseBits);
	genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterPeriodicNoiseBits);

	//Register page layouts for generic access to this device
	//##TODO## Investigate implementing a central resource system using resource key names
	//of the form "AssemblyName:DictionaryName:KeyName", where each assembly registers its
	//internal resource strings using FindResourceEx, LoadResource, LockResource, and the
	//platform retrieves the most appropriate matching resource based on the current
	//culture. That would remove literal text strings here and open our platform up for
	//easy localization. Note that it must be possible for an assembly to register
	//resource keys against another assembly, as well as register the same resource key
	//against different cultures. I would also suggest we have the concept of a fallback
	//value for a particular resource key, where a device for example could ship with all
	//strings in a particular language, but another assembly could be added later which
	//specifies resource overrides for all strings, including the invariant culture, while
	//the fallback resource values are used in the case that nothing else is defined which
	//is appropriate, IE, as might be the case if the device is updated and the
	//localization isn't yet.
	GenericAccessPage* registersPage = new GenericAccessPage(L"Generic - Registers");
	registersPage->AddEntry((new GenericAccessGroup(L"Channel 1"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL1_VOLUMEREGISTER, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL1_TONEREGISTER, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 2"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL2_VOLUMEREGISTER, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL2_TONEREGISTER, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 3"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL3_VOLUMEREGISTER, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL3_TONEREGISTER, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 4"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL4_VOLUMEREGISTER, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL4_TONEREGISTER, L"Tone"))
	                 ->AddEntry((new GenericAccessGroupSingleSelectionList(DATASOURCE_CHANNEL4_NOISETYPE, L"Noise Type"))->SetAllowNewItemEntry(true)
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"Periodic [0]"), new GenericAccessDataValueBool(false))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"White [1]"), new GenericAccessDataValueBool(true)))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL4_NOISEPERIOD, L"Noise Period"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_NOISESHIFTREGISTER, L"Noise Shift Register")))
	             ->AddEntry((new GenericAccessGroup(L"Control"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_LATCHEDCHANNELNO, L"Latched Channel No"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_VOLUMEREGISTERLATCHED, L"Volume Register Latched")));
	result &= AddGenericAccessPage(registersPage);
	GenericAccessPage* parametersPage = new GenericAccessPage(L"Generic - Parameters");
	parametersPage->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_EXTERNALCLOCKRATE, L"External Clock Rate"))
	              ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_EXTERNALCLOCKDIVIDER, L"External Clock Divider"))
	              ->AddEntry((new GenericAccessGroup(L"Noise Channel"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_SHIFTREGISTERBITCOUNT, L"Shift Register Bit Count"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_SHIFTREGISTERDEFAULTVALUE, L"Shift Register Default Value"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_WHITENOISETAPPEDBITMASK, L"White Noise Tapped Bit Mask"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_PERIODICNOISETAPPEDBITMASK, L"Periodic Noise Tapped Bit Mask")));
	result &= AddGenericAccessPage(parametersPage);
	GenericAccessPage* audioLoggingPage = new GenericAccessPage(L"Generic - Audio Logging");
	audioLoggingPage->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_AUDIOLOGGINGENABLED, L"Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_AUDIOLOGGINGPATH, L"Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED, L"Channel 1 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL2_AUDIOLOGGINGENABLED, L"Channel 2 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL3_AUDIOLOGGINGENABLED, L"Channel 3 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL4_AUDIOLOGGINGENABLED, L"Channel 4 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH, L"Channel 1 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL2_AUDIOLOGGINGPATH, L"Channel 2 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL3_AUDIOLOGGINGPATH, L"Channel 3 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(DATASOURCE_CHANNEL4_AUDIOLOGGINGPATH, L"Channel 4 Log Path"));
	result &= AddGenericAccessPage(audioLoggingPage);

	return result;
}

//----------------------------------------------------------------------------------------
bool SN76489::ValidateDevice()
{
	return (externalClockRate != 0.0);
}

//----------------------------------------------------------------------------------------
void SN76489::Initialize()
{
	//Initialize the render thread properties
	remainingRenderTime = 0;

	//Initialize the render data
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		channelRenderData[i].polarityNegative = false;
		channelRenderData[i].remainingToneCycles = 0;
	}
	noiseShiftRegister = shiftRegisterDefaultValue;
	noiseOutputMasked = true;
	outputBuffer.clear();

	//Initialize the register block, and set the correct register sizes for each entry.
	reg.Initialize();
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		if(i == noiseChannelNo)
		{
			reg.ReferenceCommitted((i*2) + 0).Resize(volumeRegisterBitCount);
			reg.ReferenceCommitted((i*2) + 1).Resize(noiseRegisterBitCount);
		}
		else
		{
			reg.ReferenceCommitted((i*2) + 0).Resize(volumeRegisterBitCount);
			reg.ReferenceCommitted((i*2) + 1).Resize(toneRegisterBitCount);
		}
	}

	//According to SN76489.txt by Maxim, SEGA integrated versions of the PSG have the
	//channels initialized with data all zeros, and volume all ones. This has been
	//verified through hardware tests.
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		if(!channelVolumeRegisterLocked[i])
		{
			SetVolumeRegister(i, Data(volumeRegisterBitCount, ~0u), AccessTarget().AccessLatest());
		}
		if(!channelDataRegisterLocked[i])
		{
			SetToneRegister(i, Data(toneRegisterBitCount, 0), AccessTarget().AccessLatest());
		}
	}

	//Note that hardware tests on a Mega Drive have shown that SEGA integrated versions of
	//the PSG are initialized with the volume register of the second channel latched.
	//##TODO## Make these power-on defaults configurable through the system XML file
	latchedChannel = 1;
	latchedVolume = true;
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int SN76489::GetClockSourceID(const std::wstring& clockSourceName) const
{
	if(clockSourceName == L"CLOCK")
	{
		return CLOCK_CLOCK;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
std::wstring SN76489::GetClockSourceName(unsigned int clockSourceID) const
{
	switch(clockSourceID)
	{
	case CLOCK_CLOCK:
		return L"CLOCK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------
void SN76489::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_CLOCK)
	{
		externalClockRate = clockRate;
	}

	//Since a clock rate change will affect our timing point calculations, trigger a
	//rollback.
	GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------
void SN76489::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	//Apply the input clock rate change
	if(clockInput == CLOCK_CLOCK)
	{
		externalClockRate = clockRate;
	}
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void SN76489::BeginExecution()
{
	//Initialize the worker thread state
	pendingRenderOperationCount = 0;
	renderThreadLagging = false;
	regTimesliceList.clear();

	//Start the render worker thread
	renderThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&SN76489::RenderThread), this));
}

//----------------------------------------------------------------------------------------
void SN76489::SuspendExecution()
{
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Suspend the render worker thread
	if(renderThreadActive)
	{
		renderThreadActive = false;
		renderThreadUpdate.notify_all();
		renderThreadStopped.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
bool SN76489::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void SN76489::NotifyUpcomingTimeslice(double nanoseconds)
{
	lastAccessTime = 0;

	//Add the new timeslice to all our timed buffers
	reg.AddTimeslice(nanoseconds);

	//Add references to the new timeslice entry from our timed buffers to the uncommitted
	//timeslice lists for the buffers
	regTimesliceListUncommitted.push_back(reg.GetLatestTimeslice());
}

//----------------------------------------------------------------------------------------
Device::UpdateMethod SN76489::GetUpdateMethod() const
{
	return UPDATEMETHOD_TIMESLICE;
}

//----------------------------------------------------------------------------------------
void SN76489::ExecuteTimeslice(double nanoseconds)
{
	//If the render thread is lagging, pause here until it has caught up, so we don't
	//leave the render thread behind with an ever-increasing workload it will never be
	//able to complete.
	if(renderThreadLagging)
	{
		boost::mutex::scoped_lock lock(timesliceMutex);
		while(renderThreadLagging)
		{
			renderThreadLaggingStateChange.wait(lock);
		}
	}
}

//----------------------------------------------------------------------------------------
void SN76489::ExecuteRollback()
{
	//Rollback our timed buffers
	reg.Rollback();

	latchedChannel = blatchedChannel;
	latchedVolume = blatchedVolume;

	//Clear any uncommitted timeslices from our render timeslice buffers
	regTimesliceListUncommitted.clear();
}

//----------------------------------------------------------------------------------------
void SN76489::ExecuteCommit()
{
	//Commit our timed buffers
	reg.Commit();

	blatchedChannel = latchedChannel;
	blatchedVolume = latchedVolume;

	//Ensure that a valid latest timeslice exists in all our buffers. We need this
	//check here, because commits can be triggered by the system at potentially any
	//point in time, whether a timeslice has been issued or not.
	if(!regTimesliceListUncommitted.empty())
	{
		//Obtain a timeslice lock so we can update the data we feed to the render
		//thread
		boost::mutex::scoped_lock lock(timesliceMutex);

		//Add the number of timeslices we are about to commit to the count of pending
		//render operations. This is used to track if the render thread is lagging.
		pendingRenderOperationCount += (unsigned int)regTimesliceListUncommitted.size();

		//Move all timeslices in our uncommitted timeslice lists over to the committed
		//timeslice lists, for processing by the render thread.
		regTimesliceList.splice(regTimesliceList.end(), regTimesliceListUncommitted);

		//Notify the render thread that it's got more work to do
		renderThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------
//Render functions
//----------------------------------------------------------------------------------------
void SN76489::RenderThread()
{
	boost::mutex::scoped_lock lock(renderThreadMutex);

	//Start the render loop
	bool done = false;
	while(!done)
	{
		//Obtain a copy of the latest completed timeslice period
		RandomTimeAccessBuffer<Data, double>::Timeslice regTimesliceCopy;
		bool renderTimesliceObtained = false;
		{
			boost::mutex::scoped_lock timesliceLock(timesliceMutex);

			//If there is at least one render timeslice pending, grab it from the queue.
			if(!regTimesliceList.empty())
			{
				//Update the lagging state for the render thread
				--pendingRenderOperationCount;
				renderThreadLagging = (pendingRenderOperationCount > maxPendingRenderOperationCount);
				renderThreadLaggingStateChange.notify_all();

				//Grab the next completed timeslice from the timeslice list
				regTimesliceCopy = *regTimesliceList.begin();
				regTimesliceList.pop_front();

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

		//Render the audio output
		size_t outputBufferPos = outputBuffer.size();
		double outputFrequency = externalClockRate / externalClockDivider;
		bool moreSamplesRemaining = true;
		while(moreSamplesRemaining)
		{
			//Determine the time of the next write. Note that currently, this may be
			//negative under certain circumstances, in particular when a write occurs past
			//the end of a timeslice. Negative times won't cause writes to be processed at
			//the incorrect time under the current model, but we do need to ensure that
			//remainingRenderTime isn't negative before attempting to generate an output.
			remainingRenderTime += reg.GetNextWriteTime(regTimesliceCopy);

			//Calculate the output sample count. Note that remainingRenderTime may be
			//negative, but we catch that below before using outputSampleCount.
			unsigned int outputSampleCount = (unsigned int)(remainingRenderTime * (outputFrequency / 1000000000.0));

			//If we have one or more output samples to generate before the next settings
			//change or the end of the target timeslice, generate and output the samples.
			if((remainingRenderTime > 0) && (outputSampleCount > 0))
			{
				//Resize the output buffer to fit the samples we're about to add
				outputBuffer.resize(outputBuffer.size() + outputSampleCount);

				//For each channel, calculate the output data for the elapsed time
				std::vector<float> channelBuffer[channelCount];
				for(unsigned int channelNo = 0; channelNo < channelCount; ++channelNo)
				{
					channelBuffer[channelNo].resize(outputSampleCount);
					UpdateChannel(channelNo, outputSampleCount, channelBuffer[channelNo]);

					//Output the channel wave log
					if(wavLoggingChannelEnabled[channelNo])
					{
						boost::mutex::scoped_lock lock(waveLoggingMutex);
						for(unsigned int i = 0; i < channelBuffer[channelNo].size(); ++i)
						{
							short sample = (short)(channelBuffer[channelNo][i] * (32767.0f/channelCount));
							wavLogChannel[channelNo].WriteData(sample);
						}
					}
				}

				//Mix the output from each channel into a combined output buffer
				for(unsigned int sampleNo = 0; sampleNo < outputSampleCount; ++sampleNo)
				{
					float mixedSample = 0.0;
					for(unsigned int channelNo = 0; channelNo < channelCount; ++channelNo)
					{
						mixedSample += channelBuffer[channelNo][sampleNo];
					}
					mixedSample /= channelCount;
					outputBuffer[outputBufferPos++] = (short)(mixedSample * (32767.0f / 6.0f));
				}

				RandomTimeAccessBuffer<Data, double>::WriteInfo writeInfo = reg.GetWriteInfo(0, regTimesliceCopy);
				if(writeInfo.exists)
				{
					//If the noise register is being modified, we need to reset the LFSR
					//to the default for value for the next cycle.
					if(writeInfo.writeAddress == (noiseChannelNo * 2) + 1)
					{
						noiseShiftRegister = shiftRegisterDefaultValue;
					}
				}

				//Adjust the remainingRenderTime variable to remove the time we just
				//consumed generating the output samples.
				remainingRenderTime -= (double)outputSampleCount * (1000000000.0 / outputFrequency);
			}

			//Advance to the next write operation, or the end of the current timeslice.
			moreSamplesRemaining = reg.AdvanceByStep(regTimesliceCopy);
		}

		//Output the mixed channel wave log
		if(wavLoggingEnabled)
		{
			boost::mutex::scoped_lock lock(waveLoggingMutex);
			wavLog.WriteData(outputBuffer);
		}

		//Play the mixed audio stream. Note that we fold samples from successive render
		//operations together, ensuring that we only send data to the output audio stream
		//when we have a significant number of samples to send.
		size_t minimumSamplesToOutput = (size_t)(outputFrequency / 60.0);
		if(outputBuffer.size() >= minimumSamplesToOutput)
		{
			unsigned int internalSampleCount = (unsigned int)outputBuffer.size();
			unsigned int outputSampleCount = (unsigned int)((double)internalSampleCount * ((double)outputSampleRate / outputFrequency));
			AudioStream::AudioBuffer* outputBufferFinal = outputStream.CreateAudioBuffer(outputSampleCount, 1);
			if(outputBufferFinal != 0)
			{
				outputStream.ConvertSampleRate(outputBuffer, internalSampleCount, 1, outputBufferFinal->buffer, outputSampleCount);
				outputStream.PlayBuffer(outputBufferFinal);
			}
			outputBuffer.clear();
			outputBuffer.reserve(minimumSamplesToOutput * 2);
		}

		//Advance past the timeslice we've just rendered from
		{
			boost::mutex::scoped_lock lock(timesliceMutex);
			reg.AdvancePastTimeslice(regTimesliceCopy);
		}
	}
	renderThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void SN76489::UpdateChannel(unsigned int channelNo, unsigned int outputSampleCount, std::vector<float>& outputBuffer)
{
	ChannelRenderData* renderData = &channelRenderData[channelNo];

	//Read current register data, and calculate half-frequency and amplitude
	Data volumeRegisterData(GetVolumeRegister(channelNo, AccessTarget().AccessCommitted()));
	Data toneRegisterData(GetToneRegister(channelNo, AccessTarget().AccessCommitted()));
	float amplitude = 0.0;
	if(volumeRegisterData.GetData() < 0xF)
	{
		//##NOTE## There is an error in SN76489.txt regarding attenuation. The document
		//correctly states that each step of the volume register corresponds with 2db of
		//attenuation (as stated by the SN76489 datasheet), however the math that follows
		//is erroneous. A decibel is a 10th of a bel(0.1 bels), not 10 bels, and a 2db
		//drop corresponds to the ratio 10^(-0.2), not 10^(-0.1).
		//##NOTE## There might be more to this. Remember that we had to divide the
		//attenuation power in the YM2612 core by 2 to get the correct result. Maybe
		//Maxim is right afterall, and you have to halve the attenuation power in the
		//YM2612 core, and this core, in order to get the correct result. Examine the
		//math he provided and do some more research. Maybe we're wrong about our
		//conversion from db to linear.
//		float attenuationInBels = (float)(volumeRegisterData.GetData() * 2) / 10.0f;
		float attenuationInBels = (float)volumeRegisterData.GetData() / 10.0f;
		amplitude = pow(10.0f, -attenuationInBels);
	}

	//If we're updating the noise register, decode the noise register data.
	bool whiteNoiseSelected = false;
	if(channelNo == noiseChannelNo)
	{
		//Noise register format
		//	--------------
		//	| 2  | 1 | 0 |
		//	|------------|
		//	|Type|Period |
		//	==============
		//Type:   Noise type (0 = Periodic, 1 = White)
		//Period: Shift rate (0 = 0x10, 1 = 0x20, 2 = 0x40, 3 = Tone2)
		whiteNoiseSelected = toneRegisterData.GetBit(2);
		switch(toneRegisterData.GetDataSegment(0, 2))
		{
		case 0:
			toneRegisterData = 0x10;
			break;
		case 1:
			toneRegisterData = 0x20;
			break;
		case 2:
			toneRegisterData = 0x40;
			break;
		case 3:
			//Read the current tone register data from channel 2
			toneRegisterData = GetToneRegister(2, AccessTarget().AccessCommitted()).GetData();
			break;
		}
	}

	//If we were partway through a cycle on this channel when we rendered the last step,
	//resume the last cycle.
	unsigned int samplesWritten = 0;
	while((renderData->remainingToneCycles > 0) && (samplesWritten < outputSampleCount))
	{
		//If we're starting the output on a negative cycle, negate the output data.
		float writeData = (renderData->polarityNegative)? -amplitude: amplitude;

		//If we're updating the noise register, calculate the output data based on
		//the shift register.
		if(channelNo == noiseChannelNo)
		{
			//Use the current noise bit to calculate the output data
			writeData = (noiseOutputMasked)? 0: amplitude;
		}

		//Write the sample to the output buffer
		outputBuffer[samplesWritten++] = writeData;
		--renderData->remainingToneCycles;
	}

	//##NOTE## Hardware tests on the SEGA integrated chip have shown that when the tone
	//data is set to 0, it behaves the same as a tone data value of 1. This includes the
	//noise channel when the period is obtained from the channel 2 tone data. This
	//behaviour contradicts the information in SN76489.txt, but our readings are clear.
	//Tone data values of both 0 and 1 produce a square wave that toggles between
	//positive and negative each internal clock cycle. This has been confirmed by
	//sampling the PSG output directly from the chip using an oscilloscope. SN76489.txt
	//states that the channel output is held at +1 when the tone data is set to 0.
	//Perhaps this is true for other implementations of this device, but our observed
	//behaviour is correct for the SEGA embedded version used in the Mega Drive. Note
	//that "After Burner II" uses a tone value of 0 in all PSG channels for its audio,
	//causing distorted sound on the system. The PSG sound emulation for this game is
	//fundamentally inaccurate when we hold the output at +1.
	if(toneRegisterData == 0)
	{
		toneRegisterData = 1;
	}

	//Output repeating oscillations of the wave at the target frequency and amplitude
	while(samplesWritten < outputSampleCount)
	{
		unsigned int samplesToWrite = toneRegisterData.GetData();

		//Invert the polarity of the wave in preparation for the new cycle
		renderData->polarityNegative = !renderData->polarityNegative;

		//If we're starting the output on a negative cycle, negate the output data.
		float writeData = (renderData->polarityNegative)? -amplitude: amplitude;

		//If we're updating the noise register, calculate the output data based on
		//the shift register.
		if(channelNo == noiseChannelNo)
		{
			unsigned int tappedBitMask = whiteNoiseSelected? noiseWhiteTappedBitMask: noisePeriodicTappedBitMask;

			//If the polarity has shifted from -1 to +1, read a new output bit
			//and adjust the shift register.
			if(!renderData->polarityNegative)
			{
				Data shiftRegister(shiftRegisterBitCount, noiseShiftRegister);
				noiseOutputMasked = !shiftRegister.GetBit(0);
				bool newUpperBit = !((shiftRegister & tappedBitMask).ParityEven());
				shiftRegister >>= 1;
				shiftRegister.SetBit(shiftRegister.GetBitCount() - 1, newUpperBit);
				noiseShiftRegister = shiftRegister.GetData();
			}

			//Use the current noise bit to calculate the output data
			writeData = (noiseOutputMasked)? 0: amplitude;
		}

		if(samplesToWrite > (outputSampleCount - samplesWritten))
		{
			//If we don't have enough samples remaining in this step to complete
			//the next cycle, clamp the number of samples to write, and save the
			//number of additional samples we need to complete for the next step.
			renderData->initialToneCycles = samplesToWrite;
			renderData->remainingToneCycles = samplesToWrite - (outputSampleCount - samplesWritten);
			samplesToWrite = (outputSampleCount - samplesWritten);
		}

		//Write a block of samples to the output buffer
		for(unsigned int i = 0; i < samplesToWrite; ++i)
		{
			outputBuffer[samplesWritten++] = writeData;
		}
	}
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult SN76489::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessMutex);
	AccessTarget accessTarget;
	accessTarget.AccessTime(accessTime);

	//Trigger a system rollback if the device has been accessed out of order. This is
	//required in order to ensure that the register latch settings are correct when each
	//write occurs.
	if(accessTime < lastAccessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	lastAccessTime = accessTime;

	//Process the write
	if(data.MSB())
	{
		//Latch/Data byte format
		//----------------------------------
		//| 7 | 6 | 5 | 4  | 3 | 2 | 1 | 0 |
		//|---|-------|----|---------------|
		//| 1 |Channel|Type|Data (low bits)|
		//----------------------------------

		//Process a latch/data byte
		latchedChannel = data.GetDataSegment(5, 2);
		latchedVolume = data.GetBit(4);
		if(latchedVolume)
		{
			Data temp(GetVolumeRegister(latchedChannel, accessTarget));
			temp.SetLowerBits(4, data.GetDataSegment(0, 4));
			if(!channelVolumeRegisterLocked[latchedChannel])
			{
				SetVolumeRegister(latchedChannel, temp, accessTarget);
			}
		}
		else
		{
			Data temp(GetToneRegister(latchedChannel, accessTarget));
			temp.SetLowerBits(4, data.GetDataSegment(0, 4));
			if(!channelDataRegisterLocked[latchedChannel])
			{
				SetToneRegister(latchedChannel, temp, accessTarget);
			}
		}
	}
	else
	{
		//Data byte format
		//---------------------------------
		//| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//|---|---|-----------------------|
		//| 0 | - |   Data (high bits)    |
		//---------------------------------

		//Process a data byte
		if(latchedVolume)
		{
			Data temp(GetVolumeRegister(latchedChannel, accessTarget));
			temp.SetUpperBits(6, data.GetDataSegment(0, 6));
			if(!channelVolumeRegisterLocked[latchedChannel])
			{
				SetVolumeRegister(latchedChannel, temp, accessTarget);
			}
		}
		else
		{
			Data temp(GetToneRegister(latchedChannel, accessTarget));
			temp.SetUpperBits(6, data.GetDataSegment(0, 6));
			if(!channelDataRegisterLocked[latchedChannel])
			{
				SetToneRegister(latchedChannel, temp, accessTarget);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void SN76489::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		//Clock settings
		if((*i)->GetName() == L"ExternalClockRate")
		{
			(*i)->ExtractData(externalClockRate);
		}

		//Register data
		else if((*i)->GetName() == L"Registers")
		{
			reg.LoadState(*(*i));
		}

		//Register latch settings
		else if((*i)->GetName() == L"LatchedChannel")
		{
			(*i)->ExtractData(latchedChannel);
			if(latchedChannel >= channelCount)
			{
				latchedChannel = channelCount - 1;
			}
		}
		else if((*i)->GetName() == L"VolumeRegisterLatched")
		{
			(*i)->ExtractData(latchedVolume);
		}

		//Render thread properties
		else if((*i)->GetName() == L"RemainingRenderTime")
		{
			(*i)->ExtractData(remainingRenderTime);
		}

		//Render data
		else if((*i)->GetName() == L"RenderData")
		{
			unsigned int channelNo;
			if((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < channelCount))
			{
				(*i)->ExtractAttributeHex(L"InitialToneCycles", channelRenderData[channelNo].initialToneCycles);
				(*i)->ExtractAttributeHex(L"RemainingToneCycles", channelRenderData[channelNo].remainingToneCycles);
				(*i)->ExtractAttribute(L"PolarityNegative", channelRenderData[channelNo].polarityNegative);
			}
		}
		else if((*i)->GetName() == L"NoiseShiftRegister")
		{
			(*i)->ExtractHexData(noiseShiftRegister);
		}
		else if((*i)->GetName() == L"NoiseOutputMasked")
		{
			(*i)->ExtractData(noiseOutputMasked);
		}
	}
}

//----------------------------------------------------------------------------------------
void SN76489::SaveState(IHierarchicalStorageNode& node) const
{
	//Clock settings
	node.CreateChild(L"ExternalClockRate").SetData(externalClockRate);

	//Register data
	reg.SaveState(node.CreateChild(L"Registers"), L"", true);

	//Register latch settings
	node.CreateChild(L"LatchedChannel", latchedChannel);
	node.CreateChild(L"VolumeRegisterLatched", latchedVolume);

	//Render thread properties
	node.CreateChild(L"RemainingRenderTime", remainingRenderTime);

	//Render data
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		IHierarchicalStorageNode& renderDataState = node.CreateChild(L"RenderData");
		renderDataState.CreateAttribute(L"ChannelNo", i);
		renderDataState.CreateAttributeHex(L"InitialToneCycles", channelRenderData[i].initialToneCycles, (toneRegisterBitCount+3)/4);
		renderDataState.CreateAttributeHex(L"RemainingToneCycles", channelRenderData[i].remainingToneCycles, (toneRegisterBitCount+3)/4);
		renderDataState.CreateAttribute(L"PolarityNegative", channelRenderData[i].polarityNegative);
	}
	node.CreateChildHex(L"NoiseShiftRegister", noiseShiftRegister, (shiftRegisterBitCount+3)/4);
	node.CreateChild(L"NoiseOutputMasked", noiseOutputMasked);
}

//----------------------------------------------------------------------------------------
void SN76489::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	//Initialize the register locking state
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		channelVolumeRegisterLocked[i] = false;
		channelDataRegisterLocked[i] = false;
	}

	//Load the register locking state
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"LockedVolumeRegister")
		{
			unsigned int channelNo;
			if((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < channelCount))
			{
				channelVolumeRegisterLocked[channelNo] = true;
			}
		}
		else if((*i)->GetName() == L"LockedDataRegister")
		{
			unsigned int channelNo;
			if((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < channelCount))
			{
				channelDataRegisterLocked[channelNo] = true;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void SN76489::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	//Save the register locking state
	for(unsigned int i = 0; i < channelCount; ++i)
	{
		if(channelVolumeRegisterLocked[i])
		{
			node.CreateChild(L"LockedVolumeRegister").CreateAttribute(L"ChannelNo", i);
		}
		if(channelDataRegisterLocked[i])
		{
			node.CreateChild(L"LockedDataRegister").CreateAttribute(L"ChannelNo", i);
		}
	}
}

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
bool SN76489::ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const
{
	ApplyGenericDataValueDisplaySettings(dataID, dataValue);
	switch(dataID)
	{
	case DATASOURCE_CHANNEL1_VOLUMEREGISTER:
		return dataValue.SetValue(GetVolumeRegister(0, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL2_VOLUMEREGISTER:
		return dataValue.SetValue(GetVolumeRegister(1, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL3_VOLUMEREGISTER:
		return dataValue.SetValue(GetVolumeRegister(2, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL4_VOLUMEREGISTER:
		return dataValue.SetValue(GetVolumeRegister(3, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL1_TONEREGISTER:
		return dataValue.SetValue(GetToneRegister(0, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL2_TONEREGISTER:
		return dataValue.SetValue(GetToneRegister(1, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL3_TONEREGISTER:
		return dataValue.SetValue(GetToneRegister(2, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL4_TONEREGISTER:
		return dataValue.SetValue(GetToneRegister(3, AccessTarget().AccessLatest()).GetData());
	case DATASOURCE_CHANNEL4_NOISETYPE:
	case DATASOURCE_CHANNEL4_NOISEPERIOD:
		//##TODO##
		return dataValue.SetValue(GetToneRegister(3, AccessTarget().AccessLatest()).GetData() != 0);
	case DATASOURCE_NOISESHIFTREGISTER:
		return dataValue.SetValue(noiseShiftRegister);
	case DATASOURCE_LATCHEDCHANNELNO:
		return dataValue.SetValue(latchedChannel);
	case DATASOURCE_VOLUMEREGISTERLATCHED:
		return dataValue.SetValue(latchedVolume);
	case DATASOURCE_EXTERNALCLOCKRATE:
		return dataValue.SetValue(externalClockRate);
	case DATASOURCE_EXTERNALCLOCKDIVIDER:
		return dataValue.SetValue(externalClockDivider);
	case DATASOURCE_SHIFTREGISTERBITCOUNT:
		return dataValue.SetValue(shiftRegisterBitCount);
	case DATASOURCE_SHIFTREGISTERDEFAULTVALUE:
		return dataValue.SetValue(shiftRegisterDefaultValue);
	case DATASOURCE_WHITENOISETAPPEDBITMASK:
		return dataValue.SetValue(noiseWhiteTappedBitMask);
	case DATASOURCE_PERIODICNOISETAPPEDBITMASK:
		return dataValue.SetValue(noisePeriodicTappedBitMask);
	case DATASOURCE_AUDIOLOGGINGENABLED:
		return dataValue.SetValue(wavLoggingEnabled);
	case DATASOURCE_AUDIOLOGGINGPATH:
		return dataValue.SetValue(wavLoggingPath);
	case DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED:
		return dataValue.SetValue(wavLoggingChannelEnabled[0]);
	case DATASOURCE_CHANNEL2_AUDIOLOGGINGENABLED:
		return dataValue.SetValue(wavLoggingChannelEnabled[1]);
	case DATASOURCE_CHANNEL3_AUDIOLOGGINGENABLED:
		return dataValue.SetValue(wavLoggingChannelEnabled[2]);
	case DATASOURCE_CHANNEL4_AUDIOLOGGINGENABLED:
		return dataValue.SetValue(wavLoggingChannelEnabled[3]);
	case DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH:
		return dataValue.SetValue(wavLoggingChannelPath[0]);
	case DATASOURCE_CHANNEL2_AUDIOLOGGINGPATH:
		return dataValue.SetValue(wavLoggingChannelPath[1]);
	case DATASOURCE_CHANNEL3_AUDIOLOGGINGPATH:
		return dataValue.SetValue(wavLoggingChannelPath[2]);
	case DATASOURCE_CHANNEL4_AUDIOLOGGINGPATH:
		return dataValue.SetValue(wavLoggingChannelPath[3]);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool SN76489::WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue)
{
	//##TODO## Restructure this to be a flat switch statement as per other devices
	ApplyGenericDataValueLimitSettings(dataID, dataValue);
	IGenericAccessDataValue::DataType dataType = dataValue.GetType();
	if(dataType == IGenericAccessDataValue::DATATYPE_UINT)
	{
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		switch(dataID)
		{
		case DATASOURCE_CHANNEL1_VOLUMEREGISTER:
			SetVolumeRegister(0, Data(volumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL2_VOLUMEREGISTER:
			SetVolumeRegister(1, Data(volumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL3_VOLUMEREGISTER:
			SetVolumeRegister(2, Data(volumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL4_VOLUMEREGISTER:
			SetVolumeRegister(3, Data(volumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL1_TONEREGISTER:
			SetToneRegister(0, Data(toneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL2_TONEREGISTER:
			SetToneRegister(1, Data(toneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL3_TONEREGISTER:
			SetToneRegister(2, Data(toneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_CHANNEL4_TONEREGISTER:
			SetToneRegister(3, Data(toneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_NOISESHIFTREGISTER:
			noiseShiftRegister = Data(GetShiftRegisterBitCount(), dataValueAsUInt.GetValue()).GetData();
			return true;
		case DATASOURCE_LATCHEDCHANNELNO:
			latchedChannel = dataValueAsUInt.GetValue();
			return true;
		case DATASOURCE_SHIFTREGISTERBITCOUNT:{
			shiftRegisterBitCount = dataValueAsUInt.GetValue();
			//Update the maximum allowable values for each data value that depends on the
			//shift register bit count
			unsigned int shiftRegisterBitCountMask = (((1 << (shiftRegisterBitCount - 1)) - 1) << 1) | 1;
			for(std::list<GenericAccessDataInfo*>::const_iterator i = genericDataToUpdateOnShiftRegisterBitCountChange.begin(); i != genericDataToUpdateOnShiftRegisterBitCountChange.end(); ++i)
			{
				(*i)->SetUIntMaxValue(shiftRegisterBitCountMask);
			}
			//Limit the current values of any settings that are affected by the shift
			//register bit count
			shiftRegisterDefaultValue &= shiftRegisterBitCountMask;
			noiseWhiteTappedBitMask &= shiftRegisterBitCountMask;
			noisePeriodicTappedBitMask &= shiftRegisterBitCountMask;
			return true;}
		case DATASOURCE_SHIFTREGISTERDEFAULTVALUE:
			shiftRegisterDefaultValue = dataValueAsUInt.GetValue();
			return true;
		case DATASOURCE_WHITENOISETAPPEDBITMASK:
			noiseWhiteTappedBitMask = dataValueAsUInt.GetValue();
			return true;
		case DATASOURCE_PERIODICNOISETAPPEDBITMASK:
			noisePeriodicTappedBitMask = dataValueAsUInt.GetValue();
			return true;
		}
	}
	else if(dataType == IGenericAccessDataValue::DATATYPE_BOOL)
	{
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		switch(dataID)
		{
		case DATASOURCE_CHANNEL4_NOISETYPE:
		case DATASOURCE_CHANNEL4_NOISEPERIOD:
			//##TODO##
			SetToneRegister(3, Data(toneRegisterBitCount, (unsigned int)dataValueAsBool.GetValue()), AccessTarget().AccessLatest());
			return true;
		case DATASOURCE_VOLUMEREGISTERLATCHED:
			latchedVolume = dataValueAsBool.GetValue();
			return true;
		case DATASOURCE_AUDIOLOGGINGENABLED:
			SetAudioLoggingEnabled(dataValueAsBool.GetValue());
			return true;
		case DATASOURCE_CHANNEL1_AUDIOLOGGINGENABLED:
			SetChannelAudioLoggingEnabled(0, dataValueAsBool.GetValue());
			return true;
		case DATASOURCE_CHANNEL2_AUDIOLOGGINGENABLED:
			SetChannelAudioLoggingEnabled(1, dataValueAsBool.GetValue());
			return true;
		case DATASOURCE_CHANNEL3_AUDIOLOGGINGENABLED:
			SetChannelAudioLoggingEnabled(2, dataValueAsBool.GetValue());
			return true;
		case DATASOURCE_CHANNEL4_AUDIOLOGGINGENABLED:
			SetChannelAudioLoggingEnabled(3, dataValueAsBool.GetValue());
			return true;
		}
	}
	else if(dataType == IGenericAccessDataValue::DATATYPE_DOUBLE)
	{
		IGenericAccessDataValueDouble& dataValueAsDouble = (IGenericAccessDataValueDouble&)dataValue;
		switch(dataID)
		{
		case DATASOURCE_EXTERNALCLOCKRATE:
			externalClockRate = dataValueAsDouble.GetValue();
			return true;
		case DATASOURCE_EXTERNALCLOCKDIVIDER:
			externalClockDivider = dataValueAsDouble.GetValue();
			return true;
		}
	}
	else if(dataType == IGenericAccessDataValue::DATATYPE_FILEPATH)
	{
		IGenericAccessDataValueFilePath& dataValueAsFilePath = (IGenericAccessDataValueFilePath&)dataValue;
		switch(dataID)
		{
		case DATASOURCE_AUDIOLOGGINGPATH:
			wavLoggingPath = dataValueAsFilePath.GetValue();
			return true;
		case DATASOURCE_CHANNEL1_AUDIOLOGGINGPATH:
			wavLoggingChannelPath[0] = dataValueAsFilePath.GetValue();
			return true;
		case DATASOURCE_CHANNEL2_AUDIOLOGGINGPATH:
			wavLoggingChannelPath[1] = dataValueAsFilePath.GetValue();
			return true;
		case DATASOURCE_CHANNEL3_AUDIOLOGGINGPATH:
			wavLoggingChannelPath[2] = dataValueAsFilePath.GetValue();
			return true;
		case DATASOURCE_CHANNEL4_AUDIOLOGGINGPATH:
			wavLoggingChannelPath[3] = dataValueAsFilePath.GetValue();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Data locking functions
//----------------------------------------------------------------------------------------
bool SN76489::GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const
{
	switch(dataID)
	{
	case DATASOURCE_CHANNEL1_VOLUMEREGISTER:
		return channelVolumeRegisterLocked[0];
	case DATASOURCE_CHANNEL2_VOLUMEREGISTER:
		return channelVolumeRegisterLocked[1];
	case DATASOURCE_CHANNEL3_VOLUMEREGISTER:
		return channelVolumeRegisterLocked[2];
	case DATASOURCE_CHANNEL4_VOLUMEREGISTER:
		return channelVolumeRegisterLocked[3];
	case DATASOURCE_CHANNEL1_TONEREGISTER:
		return channelDataRegisterLocked[0];
	case DATASOURCE_CHANNEL2_TONEREGISTER:
		return channelDataRegisterLocked[1];
	case DATASOURCE_CHANNEL3_TONEREGISTER:
		return channelDataRegisterLocked[2];
	case DATASOURCE_CHANNEL4_TONEREGISTER:
		return channelDataRegisterLocked[3];
	//##DEBUG##
	case DATASOURCE_CHANNEL4_NOISETYPE:
		return channelDataRegisterLocked[3];
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool SN76489::SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state)
{
	switch(dataID)
	{
	case DATASOURCE_CHANNEL1_VOLUMEREGISTER:
		channelVolumeRegisterLocked[0] = state;
		return true;
	case DATASOURCE_CHANNEL2_VOLUMEREGISTER:
		channelVolumeRegisterLocked[1] = state;
		return true;
	case DATASOURCE_CHANNEL3_VOLUMEREGISTER:
		channelVolumeRegisterLocked[2] = state;
		return true;
	case DATASOURCE_CHANNEL4_VOLUMEREGISTER:
		channelVolumeRegisterLocked[3] = state;
		return true;
	case DATASOURCE_CHANNEL1_TONEREGISTER:
		channelDataRegisterLocked[0] = state;
		return true;
	case DATASOURCE_CHANNEL2_TONEREGISTER:
		channelDataRegisterLocked[1] = state;
		return true;
	case DATASOURCE_CHANNEL3_TONEREGISTER:
		channelDataRegisterLocked[2] = state;
		return true;
	case DATASOURCE_CHANNEL4_TONEREGISTER:
		channelDataRegisterLocked[3] = state;
		return true;
	//##DEBUG##
	case DATASOURCE_CHANNEL4_NOISETYPE:
		channelDataRegisterLocked[3] = state;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Audio logging functions
//----------------------------------------------------------------------------------------
void SN76489::SetAudioLoggingEnabled(bool state)
{
	boost::mutex::scoped_lock lock(waveLoggingMutex);
	if(wavLoggingEnabled != state)
	{
		if(state)
		{
			wavLog.SetDataFormat(1, 16, outputSampleRate);
			wavLog.Open(wavLoggingPath, Stream::WAVFile::OPENMODE_WRITEONLY, Stream::WAVFile::CREATEMODE_CREATE);
		}
		else
		{
			wavLog.Close();
		}
		wavLoggingEnabled = state;
	}
}

//----------------------------------------------------------------------------------------
void SN76489::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool state)
{
	boost::mutex::scoped_lock lock(waveLoggingMutex);
	if(wavLoggingChannelEnabled[channelNo] != state)
	{
		if(state)
		{
			wavLogChannel[channelNo].SetDataFormat(1, 16, outputSampleRate);
			wavLogChannel[channelNo].Open(wavLoggingChannelPath[channelNo], Stream::WAVFile::OPENMODE_WRITEONLY, Stream::WAVFile::CREATEMODE_CREATE);
		}
		else
		{
			wavLogChannel[channelNo].Close();
		}
		wavLoggingChannelEnabled[channelNo] = state;
	}
}
