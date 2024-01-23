#include "SN76489.h"
#include <sstream>
#include <thread>
#include <functional>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
SN76489::SN76489(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Device(implementationName, instanceName, moduleID), _reg(ChannelCount * 2, false, Data(ToneRegisterBitCount))
{
	// Initialize the audio output stream
	_outputSampleRate = 48000;	// 44100;
	_outputStream.Open(1, 16, _outputSampleRate, _outputSampleRate/4, _outputSampleRate/20);

	// Initialize the locked register state
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		_channelVolumeRegisterLocked[i] = false;
		_channelDataRegisterLocked[i] = false;
	}
	_noiseChannelTypeLocked = false;
	_noiseChannelPeriodLocked = false;

	//##TODO## Provide a way for these properties to be defined externally, and provide
	// debug windows which can modify them on the fly.
	_externalClockRate = 0.0;
	_externalClockDivider = 16.0;
	_shiftRegisterBitCount = 16;
	_shiftRegisterDefaultValue = 0x8000;
	_noiseWhiteTappedBitMask = 0x0009;
	_noisePeriodicTappedBitMask = 0x0001;
}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int SN76489::GetISN76489Version() const
{
	return ThisISN76489Version();
}

//----------------------------------------------------------------------------------------------------------------------
// Initialization functions
//----------------------------------------------------------------------------------------------------------------------
bool SN76489::BuildDevice()
{
	// Initialize the wave logging state
	std::wstring captureFolder = GetSystemInterface().GetCapturePath();
	_wavLoggingEnabled = false;
	std::wstring wavLoggingFileName = GetDeviceInstanceName() + L".wav";
	_wavLoggingPath = PathCombinePaths(captureFolder, wavLoggingFileName);
	for (unsigned int channelNo = 0; channelNo < ChannelCount; ++channelNo)
	{
		_wavLoggingChannelEnabled[channelNo] = false;
		std::wstringstream wavLoggingChannelFileName;
		wavLoggingChannelFileName << GetDeviceInstanceName() << L" - C" << channelNo << L".wav";
		_wavLoggingChannelPath[channelNo] = PathCombinePaths(captureFolder, wavLoggingChannelFileName.str());
	}

	// Register each data source with the generic data access base class
	std::wstring audioLogExtensionFilter = L"Wave file|wav";
	std::wstring audioLogDefaultExtension = L"wav";
	GenericAccessDataInfo* dataInfoShiftRegister;
	GenericAccessDataInfo* dataInfoShiftRegisterDefaultValue;
	GenericAccessDataInfo* dataInfoShiftRegisterWhiteNoiseBits;
	GenericAccessDataInfo* dataInfoShiftRegisterPeriodicNoiseBits;
	bool result = true;
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel1VolumeRegister, IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<VolumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel2VolumeRegister, IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<VolumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel3VolumeRegister, IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<VolumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4VolumeRegister, IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<VolumeRegisterBitCount)-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel1ToneRegister,   IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<ToneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel2ToneRegister,   IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<ToneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel3ToneRegister,   IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<ToneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4ToneRegister,   IGenericAccessDataValue::DataType::UInt))->SetLockingSupported(true)->SetUIntMaxValue((1<<ToneRegisterBitCount  )-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4NoiseType, IGenericAccessDataValue::DataType::Bool))->SetLockingSupported(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4NoisePeriod, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(3)->SetLockingSupported(true));
	result &= AddGenericDataInfo(dataInfoShiftRegister = (new GenericAccessDataInfo(ISN76489DataSource::NoiseShiftRegister, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::LatchedChannelNo, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue(ChannelCount-1));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::VolumeRegisterLatched, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::ExternalClockRate, IGenericAccessDataValue::DataType::Double))->SetDoubleMinValue(0.0));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::ExternalClockDivider, IGenericAccessDataValue::DataType::Double))->SetDoubleMinValue(1.0));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::ShiftRegisterBitCount, IGenericAccessDataValue::DataType::UInt))->SetUIntMinValue(1)->SetUIntMaxValue(32)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Decimal));
	result &= AddGenericDataInfo(dataInfoShiftRegisterDefaultValue = (new GenericAccessDataInfo(ISN76489DataSource::ShiftRegisterDefaultValue, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo(dataInfoShiftRegisterWhiteNoiseBits = (new GenericAccessDataInfo(ISN76489DataSource::WhiteNoiseTappedBitMask, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo(dataInfoShiftRegisterPeriodicNoiseBits = (new GenericAccessDataInfo(ISN76489DataSource::PeriodicNoiseTappedBitMask, IGenericAccessDataValue::DataType::UInt))->SetUIntMaxValue((1<<GetShiftRegisterBitCount())-1)->SetIntDisplayMode(IGenericAccessDataValue::IntDisplayMode::Hexadecimal));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::AudioLoggingEnabled, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::AudioLoggingPath, IGenericAccessDataValue::DataType::FilePath))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel1AudioLoggingEnabled, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel2AudioLoggingEnabled, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel3AudioLoggingEnabled, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4AudioLoggingEnabled, IGenericAccessDataValue::DataType::Bool)));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel1AudioLoggingPath, IGenericAccessDataValue::DataType::FilePath))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel2AudioLoggingPath, IGenericAccessDataValue::DataType::FilePath))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel3AudioLoggingPath, IGenericAccessDataValue::DataType::FilePath))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));
	result &= AddGenericDataInfo((new GenericAccessDataInfo(ISN76489DataSource::Channel4AudioLoggingPath, IGenericAccessDataValue::DataType::FilePath))->SetFilePathExtensionFilter(audioLogExtensionFilter)->SetFilePathDefaultExtension(audioLogDefaultExtension)->SetFilePathCreatingTarget(true));

	// Save references to the data info structures which contain values that need to have
	// their limits changed when the bitcount for the shift register is changed.
	_genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegister);
	_genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterDefaultValue);
	_genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterWhiteNoiseBits);
	_genericDataToUpdateOnShiftRegisterBitCountChange.push_back(dataInfoShiftRegisterPeriodicNoiseBits);

	// Register page layouts for generic access to this device
	//##TODO## Investigate implementing a central resource system using resource key names
	// of the form "AssemblyName:DictionaryName:KeyName", where each assembly registers its
	// internal resource strings using FindResourceEx, LoadResource, LockResource, and the
	// platform retrieves the most appropriate matching resource based on the current
	// culture. That would remove literal text strings here and open our platform up for
	// easy localization. Note that it must be possible for an assembly to register
	// resource keys against another assembly, as well as register the same resource key
	// against different cultures. I would also suggest we have the concept of a fallback
	// value for a particular resource key, where a device for example could ship with all
	// strings in a particular language, but another assembly could be added later which
	// specifies resource overrides for all strings, including the invariant culture, while
	// the fallback resource values are used in the case that nothing else is defined which
	// is appropriate, IE, as might be the case if the device is updated and the
	// localization isn't yet.
	GenericAccessPage* registersPage = new GenericAccessPage(L"Registers", L"Registers");
	registersPage->AddEntry((new GenericAccessGroup(L"Channel 1"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel1VolumeRegister, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel1ToneRegister, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 2"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel2VolumeRegister, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel2ToneRegister, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 3"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel3VolumeRegister, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel3ToneRegister, L"Tone")))
	             ->AddEntry((new GenericAccessGroup(L"Channel 4"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel4VolumeRegister, L"Volume"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel4ToneRegister, L"Tone"))
	                 ->AddEntry((new GenericAccessGroupSingleSelectionList(ISN76489DataSource::Channel4NoiseType, L"Noise Type"))->SetAllowNewItemEntry(true)
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"Periodic [0]"), new GenericAccessDataValueBool(false))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"White [1]"), new GenericAccessDataValueBool(true)))
	                 ->AddEntry((new GenericAccessGroupSingleSelectionList(ISN76489DataSource::Channel4NoisePeriod, L"Noise Period"))->SetAllowNewItemEntry(true)
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"Low"), new GenericAccessDataValueUInt(0x0))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"Medium"), new GenericAccessDataValueUInt(0x01))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"High"), new GenericAccessDataValueUInt(0x02))
	                     ->AddSelectionListEntry(new GenericAccessDataValueString(L"Channel 3 Tone"), new GenericAccessDataValueUInt(0x03)))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::NoiseShiftRegister, L"Noise Shift Register")))
	             ->AddEntry((new GenericAccessGroup(L"Control"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::LatchedChannelNo, L"Latched Channel No"))
	                 ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::VolumeRegisterLatched, L"Volume Register Latched")));
	result &= AddGenericAccessPage(registersPage);
	GenericAccessPage* parametersPage = new GenericAccessPage(L"Parameters", L"Parameters");
	parametersPage->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::ExternalClockRate, L"External Clock Rate"))
	              ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::ExternalClockDivider, L"External Clock Divider"))
	              ->AddEntry((new GenericAccessGroup(L"Noise Channel"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::ShiftRegisterBitCount, L"Shift Register Bit Count"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::ShiftRegisterDefaultValue, L"Shift Register Default Value"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::WhiteNoiseTappedBitMask, L"White Noise Tapped Bit Mask"))
	                  ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::PeriodicNoiseTappedBitMask, L"Periodic Noise Tapped Bit Mask")));
	result &= AddGenericAccessPage(parametersPage);
	GenericAccessPage* audioLoggingPage = new GenericAccessPage(L"Audio Logging", L"Audio Logging");
	audioLoggingPage->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::AudioLoggingEnabled, L"Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::AudioLoggingPath, L"Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel1AudioLoggingEnabled, L"Channel 1 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel2AudioLoggingEnabled, L"Channel 2 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel3AudioLoggingEnabled, L"Channel 3 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel4AudioLoggingEnabled, L"Channel 4 Log Enabled"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel1AudioLoggingPath, L"Channel 1 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel2AudioLoggingPath, L"Channel 2 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel3AudioLoggingPath, L"Channel 3 Log Path"))
	                ->AddEntry(new GenericAccessGroupDataEntry(ISN76489DataSource::Channel4AudioLoggingPath, L"Channel 4 Log Path"));
	result &= AddGenericAccessPage(audioLoggingPage);

	return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SN76489::ValidateDevice()
{
	return (_externalClockRate != 0.0);
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::Initialize()
{
	// Initialize the render thread properties
	_remainingRenderTime = 0;

	// Initialize the render data
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		_channelRenderData[i].polarityNegative = false;
		_channelRenderData[i].remainingToneCycles = 0;
	}
	_noiseShiftRegister = _shiftRegisterDefaultValue;
	_noiseOutputMasked = true;
	_outputBuffer.clear();

	// Initialize the register block, and set the correct register sizes for each entry.
	_reg.Initialize();
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		if (i == NoiseChannelNo)
		{
			_reg.ReferenceCommitted((i*2) + 0).Resize(VolumeRegisterBitCount);
			_reg.ReferenceCommitted((i*2) + 1).Resize(NoiseRegisterBitCount);
		}
		else
		{
			_reg.ReferenceCommitted((i*2) + 0).Resize(VolumeRegisterBitCount);
			_reg.ReferenceCommitted((i*2) + 1).Resize(ToneRegisterBitCount);
		}
	}

	// According to SN76489.txt by Maxim, SEGA integrated versions of the PSG have the
	// channels initialized with data all zeros, and volume all ones. This has been
	// verified through hardware tests.
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		if (!_channelVolumeRegisterLocked[i])
		{
			SetVolumeRegister(i, Data(VolumeRegisterBitCount, ~0u), AccessTarget().AccessLatest());
		}
		if (!_channelDataRegisterLocked[i])
		{
			SetToneRegister(i, Data(ToneRegisterBitCount, 0), AccessTarget().AccessLatest());
		}
	}

	// Note that hardware tests on a Mega Drive have shown that SEGA integrated versions of
	// the PSG are initialized with the volume register of the second channel latched.
	//##TODO## Make these power-on defaults configurable through the system XML file
	_latchedChannel = 1;
	_latchedVolume = true;
}

//----------------------------------------------------------------------------------------------------------------------
// Clock source functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int SN76489::GetClockSourceID(const Marshal::In<std::wstring>& clockSourceName) const
{
	if (clockSourceName == L"CLOCK")
	{
		return (unsigned int)ClockID::Clock;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> SN76489::GetClockSourceName(unsigned int clockSourceID) const
{
	switch ((ClockID)clockSourceID)
	{
	case ClockID::Clock:
		return L"CLOCK";
	}
	return L"";
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::SetClockSourceRate(unsigned int clockInput, double clockRate, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	// Apply the input clock rate change
	if ((ClockID)clockInput == ClockID::Clock)
	{
		_externalClockRate = clockRate;
	}

	// Since a clock rate change will affect our timing point calculations, trigger a
	// rollback.
	GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::TransparentSetClockSourceRate(unsigned int clockInput, double clockRate)
{
	// Apply the input clock rate change
	if ((ClockID)clockInput == ClockID::Clock)
	{
		_externalClockRate = clockRate;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
void SN76489::BeginExecution()
{
	// Initialize the worker thread state
	_pendingRenderOperationCount = 0;
	_renderThreadLagging = false;
	_regTimesliceList.clear();

	// Start the render worker thread
	_renderThreadActive = true;
	std::thread workerThread(std::bind(std::mem_fn(&SN76489::RenderThread), this));
	workerThread.detach();
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::SuspendExecution()
{
	std::unique_lock<std::mutex> lock(_renderThreadMutex);

	// Suspend the render worker thread
	if (_renderThreadActive)
	{
		_renderThreadActive = false;
		_renderThreadUpdate.notify_all();
		_renderThreadStopped.wait(lock);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool SN76489::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::NotifyUpcomingTimeslice(double nanoseconds)
{
	_lastAccessTime = 0;

	// Add the new timeslice to all our timed buffers
	_reg.AddTimeslice(nanoseconds);

	// Add references to the new timeslice entry from our timed buffers to the uncommitted
	// timeslice lists for the buffers
	_regTimesliceListUncommitted.push_back(_reg.GetLatestTimeslice());
}

//----------------------------------------------------------------------------------------------------------------------
Device::UpdateMethod SN76489::GetUpdateMethod() const
{
	return UpdateMethod::Timeslice;
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::ExecuteTimeslice(double nanoseconds)
{
	// If the render thread is lagging, pause here until it has caught up, so we don't
	// leave the render thread behind with an ever-increasing workload it will never be
	// able to complete.
	if (_renderThreadLagging)
	{
		std::unique_lock<std::mutex> lock(_timesliceMutex);
		while (_renderThreadLagging)
		{
			_renderThreadLaggingStateChange.wait(lock);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::ExecuteRollback()
{
	// Rollback our timed buffers
	_reg.Rollback();

	_latchedChannel = _blatchedChannel;
	_latchedVolume = _blatchedVolume;

	// Clear any uncommitted timeslices from our render timeslice buffers
	_regTimesliceListUncommitted.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::ExecuteCommit()
{
	// Commit our timed buffers
	_reg.Commit();

	_blatchedChannel = _latchedChannel;
	_blatchedVolume = _latchedVolume;

	// Ensure that a valid latest timeslice exists in all our buffers. We need this check
	// here, because commits can be triggered by the system at potentially any point in
	// time, whether a timeslice has been issued or not.
	if (!_regTimesliceListUncommitted.empty())
	{
		// Obtain a timeslice lock so we can update the data we feed to the render thread
		std::unique_lock<std::mutex> lock(_timesliceMutex);

		// Add the number of timeslices we are about to commit to the count of pending
		// render operations. This is used to track if the render thread is lagging.
		_pendingRenderOperationCount += (unsigned int)_regTimesliceListUncommitted.size();

		// Move all timeslices in our uncommitted timeslice lists over to the committed
		// timeslice lists, for processing by the render thread.
		_regTimesliceList.splice(_regTimesliceList.end(), _regTimesliceListUncommitted);

		// Notify the render thread that it's got more work to do
		_renderThreadUpdate.notify_all();
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Render functions
//----------------------------------------------------------------------------------------------------------------------
void SN76489::RenderThread()
{
	std::unique_lock<std::mutex> lock(_renderThreadMutex);

	// Start the render loop
	bool done = false;
	while (!done)
	{
		// Obtain a copy of the latest completed timeslice period
		RandomTimeAccessBuffer<Data, double>::Timeslice regTimesliceCopy;
		bool renderTimesliceObtained = false;
		{
			std::unique_lock<std::mutex> timesliceLock(_timesliceMutex);

			// If there is at least one render timeslice pending, grab it from the queue.
			if (!_regTimesliceList.empty())
			{
				// Update the lagging state for the render thread
				--_pendingRenderOperationCount;
				_renderThreadLagging = (_pendingRenderOperationCount > maxPendingRenderOperationCount);
				_renderThreadLaggingStateChange.notify_all();

				// Grab the next completed timeslice from the timeslice list
				regTimesliceCopy = *_regTimesliceList.begin();
				_regTimesliceList.pop_front();

				// Flag that we managed to obtain a render timeslice
				renderTimesliceObtained = true;
			}
		}

		// If no render timeslice was available, we need to wait for a thread suspension
		// request or a new timeslice to be received, then begin the loop again.
		if (!renderTimesliceObtained)
		{
			// If the render thread has not already been instructed to stop, suspend this
			// thread until a timeslice becomes available or this thread is instructed to
			// stop.
			if (_renderThreadActive)
			{
				_renderThreadUpdate.wait(lock);
			}

			// If the render thread has been suspended, flag that we need to exit this
			// render loop.
			done = !_renderThreadActive;

			// Begin the loop again
			continue;
		}

		// Render the audio output
		size_t outputBufferPos = _outputBuffer.size();
		double outputFrequency = _externalClockRate / _externalClockDivider;
		bool moreSamplesRemaining = true;
		while (moreSamplesRemaining)
		{
			// Determine the time of the next write. Note that currently, this may be
			// negative under certain circumstances, in particular when a write occurs past
			// the end of a timeslice. Negative times won't cause writes to be processed at
			// the incorrect time under the current model, but we do need to ensure that
			// remainingRenderTime isn't negative before attempting to generate an output.
			_remainingRenderTime += _reg.GetNextWriteTime(regTimesliceCopy);

			// Calculate the output sample count. Note that remainingRenderTime may be
			// negative, but we catch that below before using outputSampleCount.
			unsigned int outputSampleCount = (unsigned int)(_remainingRenderTime * (outputFrequency / 1000000000.0));

			// If we have one or more output samples to generate before the next settings
			// change or the end of the target timeslice, generate and output the samples.
			if ((_remainingRenderTime > 0) && (outputSampleCount > 0))
			{
				// Resize the output buffer to fit the samples we're about to add
				_outputBuffer.resize(_outputBuffer.size() + outputSampleCount);

				// For each channel, calculate the output data for the elapsed time
				std::vector<float> channelBuffer[ChannelCount];
				for (unsigned int channelNo = 0; channelNo < ChannelCount; ++channelNo)
				{
					channelBuffer[channelNo].resize(outputSampleCount);
					UpdateChannel(channelNo, outputSampleCount, channelBuffer[channelNo]);

					// Output the channel wave log
					if (_wavLoggingChannelEnabled[channelNo])
					{
						std::unique_lock<std::mutex> waveLoggingLock(_waveLoggingMutex);
						for (unsigned int i = 0; i < channelBuffer[channelNo].size(); ++i)
						{
							short sample = (short)(channelBuffer[channelNo][i] * (32767.0f/ChannelCount));
							_wavLogChannel[channelNo].WriteData(sample);
						}
					}
				}

				// Mix the output from each channel into a combined output buffer
				for (unsigned int sampleNo = 0; sampleNo < outputSampleCount; ++sampleNo)
				{
					float mixedSample = 0.0;
					for (unsigned int channelNo = 0; channelNo < ChannelCount; ++channelNo)
					{
						mixedSample += channelBuffer[channelNo][sampleNo];
					}
					mixedSample /= ChannelCount;
					_outputBuffer[outputBufferPos++] = (short)(mixedSample * (32767.0f / 6.0f));
				}

				RandomTimeAccessBuffer<Data, double>::WriteInfo writeInfo = _reg.GetWriteInfo(0, regTimesliceCopy);
				if (writeInfo.exists)
				{
					// If the noise register is being modified, we need to reset the LFSR
					// to the default for value for the next cycle.
					if (writeInfo.writeAddress == (NoiseChannelNo * 2) + 1)
					{
						_noiseShiftRegister = _shiftRegisterDefaultValue;
					}
				}

				// Adjust the remainingRenderTime variable to remove the time we just
				// consumed generating the output samples.
				_remainingRenderTime -= (double)outputSampleCount * (1000000000.0 / outputFrequency);
			}

			// Advance to the next write operation, or the end of the current timeslice.
			moreSamplesRemaining = _reg.AdvanceByStep(regTimesliceCopy);
		}

		// Output the mixed channel wave log
		if (_wavLoggingEnabled)
		{
			std::unique_lock<std::mutex> waveLoggingLock(_waveLoggingMutex);
			_wavLog.WriteData(_outputBuffer);
		}

		// Play the mixed audio stream. Note that we fold samples from successive render
		// operations together, ensuring that we only send data to the output audio stream
		// when we have a significant number of samples to send.
		size_t minimumSamplesToOutput = (size_t)(outputFrequency / 60.0);
		if (_outputBuffer.size() >= minimumSamplesToOutput)
		{
			unsigned int internalSampleCount = (unsigned int)_outputBuffer.size();
			unsigned int outputSampleCount = (unsigned int)((double)internalSampleCount * ((double)_outputSampleRate / outputFrequency));
			AudioStream::AudioBuffer* outputBufferFinal = _outputStream.CreateAudioBuffer(outputSampleCount, 1);
			if (outputBufferFinal != 0)
			{
				_outputStream.ConvertSampleRate(_outputBuffer, internalSampleCount, 1, outputBufferFinal->buffer, outputSampleCount);
				_outputStream.PlayBuffer(outputBufferFinal);
			}
			_outputBuffer.clear();
			_outputBuffer.reserve(minimumSamplesToOutput * 2);
		}

		// Advance past the timeslice we've just rendered from
		{
			std::unique_lock<std::mutex> timesliceLock(_timesliceMutex);
			_reg.AdvancePastTimeslice(regTimesliceCopy);
		}
	}
	_renderThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::UpdateChannel(unsigned int channelNo, unsigned int outputSampleCount, std::vector<float>& outputBuffer)
{
	ChannelRenderData* renderData = &_channelRenderData[channelNo];

	// Read current register data, and calculate half-frequency and amplitude
	Data volumeRegisterData(GetVolumeRegister(channelNo, AccessTarget().AccessCommitted()));
	Data toneRegisterData(GetToneRegister(channelNo, AccessTarget().AccessCommitted()));
	float amplitude = 0.0;
	if (volumeRegisterData.GetData() < 0xF)
	{
		//##NOTE## There is an error in SN76489.txt regarding attenuation. The document
		// correctly states that each step of the volume register corresponds with 2db of
		// attenuation (as stated by the SN76489 datasheet), however the math that follows
		// is erroneous. A decibel is a 10th of a bel(0.1 bels), not 10 bels, and a 2db
		// drop corresponds to the ratio 10^(-0.2), not 10^(-0.1).
		//##NOTE## There might be more to this. Remember that we had to divide the
		// attenuation power in the YM2612 core by 2 to get the correct result. Maybe
		// Maxim is right afterall, and you have to halve the attenuation power in the
		// YM2612 core, and this core, in order to get the correct result. Examine the
		// math he provided and do some more research. Maybe we're wrong about our
		// conversion from db to linear.
//		float attenuationInBels = (float)(volumeRegisterData.GetData() * 2) / 10.0f;
		float attenuationInBels = (float)volumeRegisterData.GetData() / 10.0f;
		amplitude = pow(10.0f, -attenuationInBels);
	}

	// If we're updating the noise register, decode the noise register data.
	bool whiteNoiseSelected = false;
	if (channelNo == NoiseChannelNo)
	{
		// Noise register format
		//	--------------
		//	| 2  | 1 | 0 |
		//	|------------|
		//	|Type|Period |
		//	==============
		// Type:   Noise type (0 = Periodic, 1 = White)
		// Period: Shift rate (0 = 0x10, 1 = 0x20, 2 = 0x40, 3 = Tone2)
		whiteNoiseSelected = toneRegisterData.GetBit(2);
		switch (toneRegisterData.GetDataSegment(0, 2))
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
			// Read the current tone register data from channel 2
			toneRegisterData = GetToneRegister(2, AccessTarget().AccessCommitted()).GetData();
			break;
		}
	}

	// If we were partway through a cycle on this channel when we rendered the last step,
	// resume the last cycle.
	unsigned int samplesWritten = 0;
	while ((renderData->remainingToneCycles > 0) && (samplesWritten < outputSampleCount))
	{
		// If we're starting the output on a negative cycle, negate the output data.
		float writeData = (renderData->polarityNegative)? -amplitude: amplitude;

		// If we're updating the noise register, calculate the output data based on
		// the shift register.
		if (channelNo == NoiseChannelNo)
		{
			// Use the current noise bit to calculate the output data
			writeData = (_noiseOutputMasked)? 0: amplitude;
		}

		// Write the sample to the output buffer
		outputBuffer[samplesWritten++] = writeData;
		--renderData->remainingToneCycles;
	}

	//##NOTE## Hardware tests on the SEGA integrated chip have shown that when the tone
	// data is set to 0, it behaves the same as a tone data value of 1. This includes the
	// noise channel when the period is obtained from the channel 2 tone data. This
	// behaviour contradicts the information in SN76489.txt, but our readings are clear.
	// Tone data values of both 0 and 1 produce a square wave that toggles between
	// positive and negative each internal clock cycle. This has been confirmed by
	// sampling the PSG output directly from the chip using an oscilloscope. SN76489.txt
	// states that the channel output is held at +1 when the tone data is set to 0.
	// Perhaps this is true for other implementations of this device, but our observed
	// behaviour is correct for the SEGA embedded version used in the Mega Drive. Note
	// that "After Burner II" uses a tone value of 0 in all PSG channels for its audio,
	// causing distorted sound on the system. The PSG sound emulation for this game is
	// fundamentally inaccurate when we hold the output at +1.
	if (toneRegisterData == 0)
	{
		toneRegisterData = 1;
	}

	// Output repeating oscillations of the wave at the target frequency and amplitude
	while (samplesWritten < outputSampleCount)
	{
		unsigned int samplesToWrite = toneRegisterData.GetData();

		// Invert the polarity of the wave in preparation for the new cycle
		renderData->polarityNegative = !renderData->polarityNegative;

		// If we're starting the output on a negative cycle, negate the output data.
		float writeData = (renderData->polarityNegative)? -amplitude: amplitude;

		// If we're updating the noise register, calculate the output data based on
		// the shift register.
		if (channelNo == NoiseChannelNo)
		{
			unsigned int tappedBitMask = whiteNoiseSelected? _noiseWhiteTappedBitMask: _noisePeriodicTappedBitMask;

			// If the polarity has shifted from -1 to +1, read a new output bit
			// and adjust the shift register.
			if (!renderData->polarityNegative)
			{
				Data shiftRegister(_shiftRegisterBitCount, _noiseShiftRegister);
				_noiseOutputMasked = !shiftRegister.GetBit(0);
				bool newUpperBit = !((shiftRegister & tappedBitMask).ParityEven());
				shiftRegister >>= 1;
				shiftRegister.SetBit(shiftRegister.GetBitCount() - 1, newUpperBit);
				_noiseShiftRegister = shiftRegister.GetData();
			}

			// Use the current noise bit to calculate the output data
			writeData = (_noiseOutputMasked)? 0: amplitude;
		}

		if (samplesToWrite > (outputSampleCount - samplesWritten))
		{
			// If we don't have enough samples remaining in this step to complete
			// the next cycle, clamp the number of samples to write, and save the
			// number of additional samples we need to complete for the next step.
			renderData->initialToneCycles = samplesToWrite;
			renderData->remainingToneCycles = samplesToWrite - (outputSampleCount - samplesWritten);
			samplesToWrite = (outputSampleCount - samplesWritten);
		}

		// Write a block of samples to the output buffer
		for (unsigned int i = 0; i < samplesToWrite; ++i)
		{
			outputBuffer[samplesWritten++] = writeData;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult SN76489::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_accessMutex);
	AccessTarget accessTarget;
	accessTarget.AccessTime(accessTime);

	// Trigger a system rollback if the device has been accessed out of order. This is
	// required in order to ensure that the register latch settings are correct when each
	// write occurs.
	if (accessTime < _lastAccessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, _lastAccessTime, accessContext);
	}
	_lastAccessTime = accessTime;

	// Process the write
	if (data.MSB())
	{
		// Latch/Data byte format
		// ----------------------------------
		// | 7 | 6 | 5 | 4  | 3 | 2 | 1 | 0 |
		// |---|-------|----|---------------|
		// | 1 |Channel|Type|Data (low bits)|
		// ----------------------------------

		// Process a latch/data byte
		_latchedChannel = data.GetDataSegment(5, 2);
		_latchedVolume = data.GetBit(4);
		if (_latchedVolume)
		{
			Data temp(GetVolumeRegister(_latchedChannel, accessTarget));
			temp.SetLowerBits(4, data.GetDataSegment(0, 4));
			if (!_channelVolumeRegisterLocked[_latchedChannel])
			{
				SetVolumeRegister(_latchedChannel, temp, accessTarget);
			}
		}
		else
		{
			Data temp(GetToneRegister(_latchedChannel, accessTarget));
			temp.SetLowerBits(4, data.GetDataSegment(0, 4));
			if (_latchedChannel == NoiseChannelNo)
			{
				if (_noiseChannelTypeLocked)
				{
					temp.SetBit(2, GetToneRegister(_latchedChannel, accessTarget).GetBit(2));
				}
				if (_noiseChannelPeriodLocked)
				{
					temp.SetDataSegment(0, 2, GetToneRegister(_latchedChannel, accessTarget).GetDataSegment(0, 2));
				}
			}
			if (!_channelDataRegisterLocked[_latchedChannel])
			{
				SetToneRegister(_latchedChannel, temp, accessTarget);
			}
		}
	}
	else
	{
		// Data byte format
		// ---------------------------------
		// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		// |---|---|-----------------------|
		// | 0 | - |   Data (high bits)    |
		// ---------------------------------

		// Process a data byte
		if (_latchedVolume)
		{
			Data temp(GetVolumeRegister(_latchedChannel, accessTarget));
			temp.SetUpperBits(6, data.GetDataSegment(0, 6));
			if (!_channelVolumeRegisterLocked[_latchedChannel])
			{
				SetVolumeRegister(_latchedChannel, temp, accessTarget);
			}
		}
		else
		{
			Data temp(GetToneRegister(_latchedChannel, accessTarget));
			temp.SetUpperBits(6, data.GetDataSegment(0, 6));
			if (_latchedChannel == NoiseChannelNo)
			{
				if (_noiseChannelTypeLocked)
				{
					temp.SetBit(2, GetToneRegister(_latchedChannel, accessTarget).GetBit(2));
				}
				if (_noiseChannelPeriodLocked)
				{
					temp.SetDataSegment(0, 2, GetToneRegister(_latchedChannel, accessTarget).GetDataSegment(0, 2));
				}
			}
			if (!_channelDataRegisterLocked[_latchedChannel])
			{
				SetToneRegister(_latchedChannel, temp, accessTarget);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Savestate functions
//----------------------------------------------------------------------------------------------------------------------
void SN76489::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for (std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		// Clock settings
		if ((*i)->GetName() == L"ExternalClockRate")
		{
			(*i)->ExtractData(_externalClockRate);
		}

		// Register data
		else if ((*i)->GetName() == L"Registers")
		{
			_reg.LoadState(*(*i));
		}

		// Register latch settings
		else if ((*i)->GetName() == L"LatchedChannel")
		{
			(*i)->ExtractData(_latchedChannel);
			if (_latchedChannel >= ChannelCount)
			{
				_latchedChannel = ChannelCount - 1;
			}
		}
		else if ((*i)->GetName() == L"VolumeRegisterLatched")
		{
			(*i)->ExtractData(_latchedVolume);
		}

		// Render thread properties
		else if ((*i)->GetName() == L"RemainingRenderTime")
		{
			(*i)->ExtractData(_remainingRenderTime);
		}

		// Render data
		else if ((*i)->GetName() == L"RenderData")
		{
			unsigned int channelNo;
			if ((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < ChannelCount))
			{
				(*i)->ExtractAttributeHex(L"InitialToneCycles", _channelRenderData[channelNo].initialToneCycles);
				(*i)->ExtractAttributeHex(L"RemainingToneCycles", _channelRenderData[channelNo].remainingToneCycles);
				(*i)->ExtractAttribute(L"PolarityNegative", _channelRenderData[channelNo].polarityNegative);
			}
		}
		else if ((*i)->GetName() == L"NoiseShiftRegister")
		{
			(*i)->ExtractHexData(_noiseShiftRegister);
		}
		else if ((*i)->GetName() == L"NoiseOutputMasked")
		{
			(*i)->ExtractData(_noiseOutputMasked);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::SaveState(IHierarchicalStorageNode& node) const
{
	// Clock settings
	node.CreateChild(L"ExternalClockRate").SetData(_externalClockRate);

	// Register data
	_reg.SaveState(node.CreateChild(L"Registers"), L"", true);

	// Register latch settings
	node.CreateChild(L"LatchedChannel", _latchedChannel);
	node.CreateChild(L"VolumeRegisterLatched", _latchedVolume);

	// Render thread properties
	node.CreateChild(L"RemainingRenderTime", _remainingRenderTime);

	// Render data
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		IHierarchicalStorageNode& renderDataState = node.CreateChild(L"RenderData");
		renderDataState.CreateAttribute(L"ChannelNo", i);
		renderDataState.CreateAttributeHex(L"InitialToneCycles", _channelRenderData[i].initialToneCycles, (ToneRegisterBitCount+3)/4);
		renderDataState.CreateAttributeHex(L"RemainingToneCycles", _channelRenderData[i].remainingToneCycles, (ToneRegisterBitCount+3)/4);
		renderDataState.CreateAttribute(L"PolarityNegative", _channelRenderData[i].polarityNegative);
	}
	node.CreateChildHex(L"NoiseShiftRegister", _noiseShiftRegister, (_shiftRegisterBitCount+3)/4);
	node.CreateChild(L"NoiseOutputMasked", _noiseOutputMasked);
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	// Initialize the register locking state
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		_channelVolumeRegisterLocked[i] = false;
		_channelDataRegisterLocked[i] = false;
	}

	// Load the register locking state
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for (std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if ((*i)->GetName() == L"LockedVolumeRegister")
		{
			unsigned int channelNo;
			if ((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < ChannelCount))
			{
				_channelVolumeRegisterLocked[channelNo] = true;
			}
		}
		else if ((*i)->GetName() == L"LockedDataRegister")
		{
			unsigned int channelNo;
			if ((*i)->ExtractAttribute(L"ChannelNo", channelNo) && (channelNo < ChannelCount))
			{
				_channelDataRegisterLocked[channelNo] = true;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	// Save the register locking state
	for (unsigned int i = 0; i < ChannelCount; ++i)
	{
		if (_channelVolumeRegisterLocked[i])
		{
			node.CreateChild(L"LockedVolumeRegister").CreateAttribute(L"ChannelNo", i);
		}
		if (_channelDataRegisterLocked[i])
		{
			node.CreateChild(L"LockedDataRegister").CreateAttribute(L"ChannelNo", i);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Data read/write functions
//----------------------------------------------------------------------------------------------------------------------
bool SN76489::ReadGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue) const
{
	ApplyGenericDataValueDisplaySettings(dataID, dataValue);
	switch ((ISN76489DataSource)dataID)
	{
	case ISN76489DataSource::Channel1VolumeRegister:
		return dataValue.SetValue(GetVolumeRegister(0, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel2VolumeRegister:
		return dataValue.SetValue(GetVolumeRegister(1, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel3VolumeRegister:
		return dataValue.SetValue(GetVolumeRegister(2, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel4VolumeRegister:
		return dataValue.SetValue(GetVolumeRegister(3, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel1ToneRegister:
		return dataValue.SetValue(GetToneRegister(0, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel2ToneRegister:
		return dataValue.SetValue(GetToneRegister(1, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel3ToneRegister:
		return dataValue.SetValue(GetToneRegister(2, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel4ToneRegister:
		return dataValue.SetValue(GetToneRegister(3, AccessTarget().AccessLatest()).GetData());
	case ISN76489DataSource::Channel4NoiseType:
		return dataValue.SetValue(GetToneRegister(3, AccessTarget().AccessLatest()).GetBit(2));
	case ISN76489DataSource::Channel4NoisePeriod:
		return dataValue.SetValue(GetToneRegister(3, AccessTarget().AccessLatest()).GetDataSegment(0, 2));
	case ISN76489DataSource::NoiseShiftRegister:
		return dataValue.SetValue(_noiseShiftRegister);
	case ISN76489DataSource::LatchedChannelNo:
		return dataValue.SetValue(_latchedChannel);
	case ISN76489DataSource::VolumeRegisterLatched:
		return dataValue.SetValue(_latchedVolume);
	case ISN76489DataSource::ExternalClockRate:
		return dataValue.SetValue(_externalClockRate);
	case ISN76489DataSource::ExternalClockDivider:
		return dataValue.SetValue(_externalClockDivider);
	case ISN76489DataSource::ShiftRegisterBitCount:
		return dataValue.SetValue(_shiftRegisterBitCount);
	case ISN76489DataSource::ShiftRegisterDefaultValue:
		return dataValue.SetValue(_shiftRegisterDefaultValue);
	case ISN76489DataSource::WhiteNoiseTappedBitMask:
		return dataValue.SetValue(_noiseWhiteTappedBitMask);
	case ISN76489DataSource::PeriodicNoiseTappedBitMask:
		return dataValue.SetValue(_noisePeriodicTappedBitMask);
	case ISN76489DataSource::AudioLoggingEnabled:
		return dataValue.SetValue(_wavLoggingEnabled);
	case ISN76489DataSource::AudioLoggingPath:
		return dataValue.SetValue(_wavLoggingPath);
	case ISN76489DataSource::Channel1AudioLoggingEnabled:
		return dataValue.SetValue(_wavLoggingChannelEnabled[0]);
	case ISN76489DataSource::Channel2AudioLoggingEnabled:
		return dataValue.SetValue(_wavLoggingChannelEnabled[1]);
	case ISN76489DataSource::Channel3AudioLoggingEnabled:
		return dataValue.SetValue(_wavLoggingChannelEnabled[2]);
	case ISN76489DataSource::Channel4AudioLoggingEnabled:
		return dataValue.SetValue(_wavLoggingChannelEnabled[3]);
	case ISN76489DataSource::Channel1AudioLoggingPath:
		return dataValue.SetValue(_wavLoggingChannelPath[0]);
	case ISN76489DataSource::Channel2AudioLoggingPath:
		return dataValue.SetValue(_wavLoggingChannelPath[1]);
	case ISN76489DataSource::Channel3AudioLoggingPath:
		return dataValue.SetValue(_wavLoggingChannelPath[2]);
	case ISN76489DataSource::Channel4AudioLoggingPath:
		return dataValue.SetValue(_wavLoggingChannelPath[3]);
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SN76489::WriteGenericData(unsigned int dataID, const DataContext* dataContext, IGenericAccessDataValue& dataValue)
{
	//##TODO## Restructure this to be a flat switch statement as per other devices
	ApplyGenericDataValueLimitSettings(dataID, dataValue);
	IGenericAccessDataValue::DataType dataType = dataValue.GetType();
	if (dataType == IGenericAccessDataValue::DataType::UInt)
	{
		IGenericAccessDataValueUInt& dataValueAsUInt = (IGenericAccessDataValueUInt&)dataValue;
		switch ((ISN76489DataSource)dataID)
		{
		case ISN76489DataSource::Channel1VolumeRegister:
			SetVolumeRegister(0, Data(VolumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel2VolumeRegister:
			SetVolumeRegister(1, Data(VolumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel3VolumeRegister:
			SetVolumeRegister(2, Data(VolumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel4VolumeRegister:
			SetVolumeRegister(3, Data(VolumeRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel1ToneRegister:
			SetToneRegister(0, Data(ToneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel2ToneRegister:
			SetToneRegister(1, Data(ToneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel3ToneRegister:
			SetToneRegister(2, Data(ToneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel4ToneRegister:
			SetToneRegister(3, Data(ToneRegisterBitCount, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::Channel4NoisePeriod:
			SetToneRegister(3, GetToneRegister(3, AccessTarget().AccessLatest()).SetDataSegment(0, 2, dataValueAsUInt.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::NoiseShiftRegister:
			_noiseShiftRegister = Data(GetShiftRegisterBitCount(), dataValueAsUInt.GetValue()).GetData();
			return true;
		case ISN76489DataSource::LatchedChannelNo:
			_latchedChannel = dataValueAsUInt.GetValue();
			return true;
		case ISN76489DataSource::ShiftRegisterBitCount:{
			_shiftRegisterBitCount = dataValueAsUInt.GetValue();
			// Update the maximum allowable values for each data value that depends on the
			// shift register bit count
			unsigned int shiftRegisterBitCountMask = (((1 << (_shiftRegisterBitCount - 1)) - 1) << 1) | 1;
			for (std::list<GenericAccessDataInfo*>::const_iterator i = _genericDataToUpdateOnShiftRegisterBitCountChange.begin(); i != _genericDataToUpdateOnShiftRegisterBitCountChange.end(); ++i)
			{
				(*i)->SetUIntMaxValue(shiftRegisterBitCountMask);
			}
			// Limit the current values of any settings that are affected by the shift
			// register bit count
			_shiftRegisterDefaultValue &= shiftRegisterBitCountMask;
			_noiseWhiteTappedBitMask &= shiftRegisterBitCountMask;
			_noisePeriodicTappedBitMask &= shiftRegisterBitCountMask;
			return true;}
		case ISN76489DataSource::ShiftRegisterDefaultValue:
			_shiftRegisterDefaultValue = dataValueAsUInt.GetValue();
			return true;
		case ISN76489DataSource::WhiteNoiseTappedBitMask:
			_noiseWhiteTappedBitMask = dataValueAsUInt.GetValue();
			return true;
		case ISN76489DataSource::PeriodicNoiseTappedBitMask:
			_noisePeriodicTappedBitMask = dataValueAsUInt.GetValue();
			return true;
		}
	}
	else if (dataType == IGenericAccessDataValue::DataType::Bool)
	{
		IGenericAccessDataValueBool& dataValueAsBool = (IGenericAccessDataValueBool&)dataValue;
		switch ((ISN76489DataSource)dataID)
		{
		case ISN76489DataSource::Channel4NoiseType:
			SetToneRegister(3, GetToneRegister(3, AccessTarget().AccessLatest()).SetBit(2, dataValueAsBool.GetValue()), AccessTarget().AccessLatest());
			return true;
		case ISN76489DataSource::VolumeRegisterLatched:
			_latchedVolume = dataValueAsBool.GetValue();
			return true;
		case ISN76489DataSource::AudioLoggingEnabled:
			SetAudioLoggingEnabled(dataValueAsBool.GetValue());
			return true;
		case ISN76489DataSource::Channel1AudioLoggingEnabled:
			SetChannelAudioLoggingEnabled(0, dataValueAsBool.GetValue());
			return true;
		case ISN76489DataSource::Channel2AudioLoggingEnabled:
			SetChannelAudioLoggingEnabled(1, dataValueAsBool.GetValue());
			return true;
		case ISN76489DataSource::Channel3AudioLoggingEnabled:
			SetChannelAudioLoggingEnabled(2, dataValueAsBool.GetValue());
			return true;
		case ISN76489DataSource::Channel4AudioLoggingEnabled:
			SetChannelAudioLoggingEnabled(3, dataValueAsBool.GetValue());
			return true;
		}
	}
	else if (dataType == IGenericAccessDataValue::DataType::Double)
	{
		IGenericAccessDataValueDouble& dataValueAsDouble = (IGenericAccessDataValueDouble&)dataValue;
		switch ((ISN76489DataSource)dataID)
		{
		case ISN76489DataSource::ExternalClockRate:
			_externalClockRate = dataValueAsDouble.GetValue();
			return true;
		case ISN76489DataSource::ExternalClockDivider:
			_externalClockDivider = dataValueAsDouble.GetValue();
			return true;
		}
	}
	else if (dataType == IGenericAccessDataValue::DataType::FilePath)
	{
		IGenericAccessDataValueFilePath& dataValueAsFilePath = (IGenericAccessDataValueFilePath&)dataValue;
		switch ((ISN76489DataSource)dataID)
		{
		case ISN76489DataSource::AudioLoggingPath:
			_wavLoggingPath = dataValueAsFilePath.GetValue();
			return true;
		case ISN76489DataSource::Channel1AudioLoggingPath:
			_wavLoggingChannelPath[0] = dataValueAsFilePath.GetValue();
			return true;
		case ISN76489DataSource::Channel2AudioLoggingPath:
			_wavLoggingChannelPath[1] = dataValueAsFilePath.GetValue();
			return true;
		case ISN76489DataSource::Channel3AudioLoggingPath:
			_wavLoggingChannelPath[2] = dataValueAsFilePath.GetValue();
			return true;
		case ISN76489DataSource::Channel4AudioLoggingPath:
			_wavLoggingChannelPath[3] = dataValueAsFilePath.GetValue();
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Data locking functions
//----------------------------------------------------------------------------------------------------------------------
bool SN76489::GetGenericDataLocked(unsigned int dataID, const DataContext* dataContext) const
{
	switch ((ISN76489DataSource)dataID)
	{
	case ISN76489DataSource::Channel1VolumeRegister:
		return _channelVolumeRegisterLocked[0];
	case ISN76489DataSource::Channel2VolumeRegister:
		return _channelVolumeRegisterLocked[1];
	case ISN76489DataSource::Channel3VolumeRegister:
		return _channelVolumeRegisterLocked[2];
	case ISN76489DataSource::Channel4VolumeRegister:
		return _channelVolumeRegisterLocked[3];
	case ISN76489DataSource::Channel1ToneRegister:
		return _channelDataRegisterLocked[0];
	case ISN76489DataSource::Channel2ToneRegister:
		return _channelDataRegisterLocked[1];
	case ISN76489DataSource::Channel3ToneRegister:
		return _channelDataRegisterLocked[2];
	case ISN76489DataSource::Channel4ToneRegister:
		return _channelDataRegisterLocked[3];
	case ISN76489DataSource::Channel4NoiseType:
		return _channelDataRegisterLocked[3] || _noiseChannelTypeLocked;
	case ISN76489DataSource::Channel4NoisePeriod:
		return _channelDataRegisterLocked[3] || _noiseChannelPeriodLocked;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SN76489::SetGenericDataLocked(unsigned int dataID, const DataContext* dataContext, bool state)
{
	switch ((ISN76489DataSource)dataID)
	{
	case ISN76489DataSource::Channel1VolumeRegister:
		_channelVolumeRegisterLocked[0] = state;
		return true;
	case ISN76489DataSource::Channel2VolumeRegister:
		_channelVolumeRegisterLocked[1] = state;
		return true;
	case ISN76489DataSource::Channel3VolumeRegister:
		_channelVolumeRegisterLocked[2] = state;
		return true;
	case ISN76489DataSource::Channel4VolumeRegister:
		_channelVolumeRegisterLocked[3] = state;
		return true;
	case ISN76489DataSource::Channel1ToneRegister:
		_channelDataRegisterLocked[0] = state;
		return true;
	case ISN76489DataSource::Channel2ToneRegister:
		_channelDataRegisterLocked[1] = state;
		return true;
	case ISN76489DataSource::Channel3ToneRegister:
		_channelDataRegisterLocked[2] = state;
		return true;
	case ISN76489DataSource::Channel4ToneRegister:
		_channelDataRegisterLocked[3] = state;
		return true;
	case ISN76489DataSource::Channel4NoiseType:
		_noiseChannelTypeLocked = state;
		_channelDataRegisterLocked[3] = false;
		return true;
	case ISN76489DataSource::Channel4NoisePeriod:
		_noiseChannelPeriodLocked = state;
		_channelDataRegisterLocked[3] = false;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Audio logging functions
//----------------------------------------------------------------------------------------------------------------------
void SN76489::SetAudioLoggingEnabled(bool state)
{
	std::unique_lock<std::mutex> lock(_waveLoggingMutex);
	if (_wavLoggingEnabled != state)
	{
		if (state)
		{
			double outputFrequency = _externalClockRate / _externalClockDivider;
			_wavLog.SetDataFormat(1, 16, (unsigned int)outputFrequency);
			_wavLog.Open(_wavLoggingPath, Stream::WAVFile::OpenMode::WriteOnly, Stream::WAVFile::CreateMode::Create);
		}
		else
		{
			_wavLog.Close();
		}
		_wavLoggingEnabled = state;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void SN76489::SetChannelAudioLoggingEnabled(unsigned int channelNo, bool state)
{
	std::unique_lock<std::mutex> lock(_waveLoggingMutex);
	if (_wavLoggingChannelEnabled[channelNo] != state)
	{
		if (state)
		{
			double outputFrequency = _externalClockRate / _externalClockDivider;
			_wavLogChannel[channelNo].SetDataFormat(1, 16, (unsigned int)outputFrequency);
			_wavLogChannel[channelNo].Open(_wavLoggingChannelPath[channelNo], Stream::WAVFile::OpenMode::WriteOnly, Stream::WAVFile::CreateMode::Create);
		}
		else
		{
			_wavLogChannel[channelNo].Close();
		}
		_wavLoggingChannelEnabled[channelNo] = state;
	}
}
