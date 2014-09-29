#ifndef __MDBUSARBITER_H__
#define __MDBUSARBITER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Device/Device.pkg"
#include <mutex>

class MDBusArbiter :public Device
{
public:
	//Constructors
	MDBusArbiter(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IDevice* target);
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IDevice* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	bool ReadZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data& m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime);
	bool WriteZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime);

	//CE line state functions
	virtual unsigned int GetCELineID(const std::wstring& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Enumerations
	enum class CELineID;
	enum class LineID;
	enum class MemoryInterface;

	//Structures
	struct LineAccess;

private:
	//CE line state functions
	unsigned int BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, bool cartInLineAsserted, IDeviceContext* caller, double accessTime) const;
	unsigned int BuildCELineZ80(unsigned int targetAddress) const;

	//Line functions
	void ApplyLineStateChange(LineID targetLine, const Data& lineData, double accessTime);
	void ApplyPendingLineStateChanges(double accessTime);
	bool AdvanceUntilPendingLineStateChangeApplied(IDeviceContext* caller, double accessTime, unsigned int accessContext, LineID targetLine, Data targetLineState, double& lineStateReachedTime);

private:
	//Device references
	IDevice* bootROM;

	//Bus interfaces
	IBusInterface* z80MemoryBus;
	IBusInterface* m68kMemoryBus;

	//Device settings
	bool activateTMSS;
	bool bactivateTMSS;
	bool activateBootROM;
	bool bactivateBootROM;

	//TMSS Security settings
	bool bootROMEnabled;
	bool bbootROMEnabled;
	bool vdpLockoutActive;
	bool bvdpLockoutActive;
	mutable bool vdpLockoutTripped;
	bool bvdpLockoutTripped;

	//Z80 to M68K memory bankswitch register settings
	mutable std::mutex bankswitchAccessMutex;
	Data z80BankswitchDataCurrent;
	Data bz80BankswitchDataCurrent;
	Data z80BankswitchDataNew;
	Data bz80BankswitchDataNew;
	unsigned int z80BankswitchBitsWritten;
	unsigned int bz80BankswitchBitsWritten;

	//CE line masks
	unsigned int ceLineMaskReadHighWriteLow;
	unsigned int ceLineMaskUDS;
	unsigned int ceLineMaskLDS;
	unsigned int ceLineMaskOE0;
	unsigned int ceLineMaskCE0;
	unsigned int ceLineMaskBootROM;
	unsigned int ceLineMaskROM;
	unsigned int ceLineMaskASEL;
	unsigned int ceLineMaskFDC;
	unsigned int ceLineMaskFDWR;
	unsigned int ceLineMaskTIME;
	unsigned int ceLineMaskIO;
	unsigned int ceLineMaskEOE;
	unsigned int ceLineMaskNOE;
	unsigned int ceLineMaskZRAM;
	unsigned int ceLineMaskSOUND;

	//Line access
	std::mutex lineMutex;
	mutable double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;

	//Line state
	volatile bool cartInLineState;
	bool bcartInLineState;
	volatile bool z80BusRequestLineState;
	bool bz80BusRequestLineState;
	volatile bool z80BusGrantLineState;
	bool bz80BusGrantLineState;
	volatile bool z80BusResetLineState;
	bool bz80BusResetLineState;
	volatile bool m68kBusRequestLineState;
	bool bm68kBusRequestLineState;
	volatile bool m68kBusGrantLineState;
	bool bm68kBusGrantLineState;
	volatile bool vresLineState;
	bool bvresLineState;
	volatile bool haltLineState;
	bool bhaltLineState;
	volatile bool sresLineState;
	bool bsresLineState;
	volatile bool wresLineState;
	bool bwresLineState;

	mutable bool z80BusRequestLineStateChangeTimeLatchEnable;
	mutable bool z80BusGrantLineStateChangeTimeLatchEnable;
	mutable bool z80BusResetLineStateChangeTimeLatchEnable;
	mutable bool m68kBusRequestLineStateChangeTimeLatchEnable;
	mutable bool m68kBusGrantLineStateChangeTimeLatchEnable;
	mutable double z80BusRequestLineStateChangeTimeLatch;
	mutable double z80BusGrantLineStateChangeTimeLatch;
	mutable double z80BusResetLineStateChangeTimeLatch;
	mutable double m68kBusRequestLineStateChangeTimeLatch;
	mutable double m68kBusGrantLineStateChangeTimeLatch;
};

#include "MDBusArbiter.inl"
#endif
