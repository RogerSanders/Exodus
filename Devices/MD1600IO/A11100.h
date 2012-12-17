#ifndef __A11100_H__
#define __A11100_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"
#include <boost/thread/mutex.hpp>
#include <vector>

//Note that the 315-5364 acts as this device in the Mega Drive, as it's the device which
//controls the Z80 RESET(ZRES) and BUSREQ(ZBR) lines.
//##TODO## Delete this device, as it has now been replaced by MDBusArbiter.
class A11100 :public Device
{
public:
	//Constructors
	A11100(const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual UpdateMethod GetUpdateMethod() const;
	virtual void ExecuteTimeslice(double nanoseconds);
	virtual void ExecuteTimesliceTimingPointStep(unsigned int accessContext);
	virtual double GetNextTimingPointInDeviceTime(unsigned int& accessContext) const;

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData);
	void ApplyPendingLineStateChanges(IDeviceContext* caller, double accessTime, unsigned int accessContext);

private:
	//Enumerations
	enum LineID
	{
		LINE_RESET = 1,
		LINE_BUSREQ,
		LINE_BUSACK
	};

	//Structures
	struct LineAccess
	{
		LineAccess(unsigned int alineLD, const Data& astate, double aaccessTime)
		:lineID(alineLD), state(astate), accessTime(aaccessTime)
		{}

		unsigned int lineID;
		Data state;
		double accessTime;
	};

private:
	//Bus interface
	IBusInterface* memoryBus;

	//Internal state
	mutable boost::mutex accessMutex;
	bool initialized;
	bool binitialized;

	//Line access
	boost::mutex lineMutex;
	mutable double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;

	bool busRequested;
	bool bbusRequested;
	bool busGranted;
	bool bbusGranted;
	bool reset;
	bool breset;
};

#endif
