/*--------------------------------------------------------------------------------------*\
Description:
This core emulates the SN76489 Programmable Sound Generator (PSG), and is designed to
produce a sample-accurate output. Each channel is generated and mixed at the correct
internal sample rate, and all known aspects and behaviour of this device are faithfully
emulated.

Things to do:
-Get a final answer on the conversion from attenuation to linear power, and document it
within your core.
-Make it possible to define the clock and noise register settings externally

Output filtering:
-Note that there is real "decay-like" behaviour, which comes from the audio output
circuitry in the Mega Drive. The same circuit affects the output of the YM2612 in the
same way. If we want to emulate this behaviour, we need to emulate it using a form of
audio filter which can be applied to any sound core, just like we need to emulate analog
video distortion using output filters. These filters need to be separate from the device
cores, and be modular and chainable. Note that we should also use output "filters" to
balance output levels between sound devices.
-Measure the final audio output levels from the embedded SN76489 in the Mega Drive, both
directly from the chip and after the mixing circuit. According to SN76489.txt, several of
the minimum attenuation levels may in fact be clipped, which will greatly affect the
overall volume of the PSG, and the relative volume levels of the different register
settings on the system.

Debug output modifications:
-Add the calculated frequency, and attenuation in db, for each channel, on the register
debug window.
-Display the noise register using its actual separate register values, named correctly,
on the register debug window.

References:
-SN76489 Notes (SN76489.txt), Maxim, 2005
-Genesis Sound Software Manual, Sega Enterprises, 1992, scans by antime
-SN76489 Datasheet, Texas Instruments
-SN76494 Datasheet, Texas Instruments
-SN76489 sound chip details, John Kortink, http://web.inter.nl.net/users/J.Kortink/home/articles/sn76489/index.htm
\*--------------------------------------------------------------------------------------*/
#ifndef __SN76489_H__
#define __SN76489_H__
#include "SystemInterface/SystemInterface.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include <vector>
#include <map>
#include <set>
#include <list>
#include "Device/Device.pkg"
#include "AudioStream/AudioStream.pkg"
#include "Stream/Stream.pkg"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>

class SN76489 :public Device
{
public:
	//Constructors
	SN76489(const std::wstring& ainstanceName, unsigned int amoduleID);
	~SN76489();

	//Initialization functions
	virtual bool BuildDevice();
	virtual void Initialize();

	//Execute functions
	virtual void BeginExecution();
	virtual void SuspendExecution();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual UpdateMethod GetUpdateMethod() const;
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory interface functions
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void GetState(IHeirarchicalStorageNode& node) const;

	//Window functions
	virtual void AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher);
	virtual void RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher);
	virtual void OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher);

private:
	//Enumerations
	enum LockedRegisterKey;
	enum DebugWindow;

	//Structures
	struct ChannelRenderData
	{
		unsigned int initialToneCycles;
		unsigned int remainingToneCycles;
		bool polarityNegative;
	};

	//View and menu classes
	class DebugMenuHandler;
	class RegistersViewModel;
	class PropertiesViewModel;
	class LoggingViewModel;
	class RegistersView;
	class PropertiesView;
	class LoggingView;
	friend class RegistersViewModel;
	friend class PropertiesViewModel;
	friend class LoggingViewModel;
	friend class RegistersView;
	friend class PropertiesView;
	friend class LoggingView;

	//Typedefs
	typedef RandomTimeAccessBuffer<Data, double>::AccessTarget AccessTarget;

	//Render functions
	void RenderThread();
	void UpdateChannel(unsigned int channelNo, unsigned int outputSampleCount, std::vector<float>& outputBuffer);

	//Raw register functions
	inline Data GetVolumeRegister(unsigned int channelNo, const AccessTarget& accessTarget);
	inline void SetVolumeRegister(unsigned int channelNo, const Data& adata, const AccessTarget& accessTarget);
	inline Data GetToneRegister(unsigned int channelNo, const AccessTarget& accessTarget);
	inline void SetToneRegister(unsigned int channelNo, const Data& adata, const AccessTarget& accessTarget);

	//Register locking
	void LockRegister(LockedRegisterKey key, const boost::function<void()>& function);
	void UnlockRegister(LockedRegisterKey key);
	bool IsRegisterLocked(LockedRegisterKey key);

private:
	//Constants
	static const unsigned int channelCount = 4;
	static const unsigned int noiseChannelNo = 3;
	static const unsigned int toneRegisterBitCount = 10;
	static const unsigned int volumeRegisterBitCount = 4;
	static const unsigned int noiseRegisterBitCount = 3;

	//Menu handling
	DebugMenuHandler* menuHandler;

	//Registers
	mutable boost::mutex accessMutex;
	double lastAccessTime;
	RandomTimeAccessBuffer<Data, double> reg;
	unsigned int latchedChannel;
	unsigned int blatchedChannel;
	bool latchedVolume;
	bool blatchedVolume;

	//Render thread properties
	mutable boost::mutex renderThreadMutex;
	mutable boost::mutex timesliceMutex;
	boost::condition renderThreadUpdate;
	boost::condition renderThreadStopped;
	bool renderThreadActive;
	bool pendingRenderOperation;
	static const unsigned int maxPendingRenderOperationCount = 2;
	bool renderThreadLagging;
	boost::condition renderThreadLaggingStateChange;
	unsigned int pendingRenderOperationCount;
	double pendingRenderTimesliceLength;
	double bpendingRenderTimesliceLength;
	std::list<RandomTimeAccessBuffer<Data, double>::Timeslice> regTimesliceList;
	double remainingRenderTime;
	unsigned int outputSampleRate;
	AudioStream outputStream;

	//Render data
	ChannelRenderData channelRenderData[channelCount];
	unsigned int noiseShiftRegister;
	bool noiseOutputMasked;

	//Device Properties
	double externalClockRate;
	double externalClockDivider;
	unsigned int shiftRegisterBitCount;
	unsigned int shiftRegisterDefaultValue;
	unsigned int noiseWhiteTappedBitMask;
	unsigned int noisePeriodicTappedBitMask;

	//Register locking
	mutable boost::mutex registerLockMutex;
	typedef std::pair<LockedRegisterKey, boost::function<void()> > LockedRegisterListEntry;
	typedef std::map<LockedRegisterKey, boost::function<void()> > LockedRegisterList;
	LockedRegisterList lockedRegisters;
	std::set<LockedRegisterKey> lockedRegisterIndex;

	//Wave logging
	mutable boost::mutex waveLoggingMutex;
	bool wavLoggingEnabled;
	bool wavLoggingChannelEnabled[channelCount];
	std::wstring wavLoggingPath;
	std::wstring wavLoggingChannelPath[channelCount];
	Stream::WAVFile wavLog;
	Stream::WAVFile wavLogChannel[channelCount];
};

#include "SN76489.inl"
#endif
