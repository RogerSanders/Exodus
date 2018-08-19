#ifndef __MDBUSARBITER_H__
#define __MDBUSARBITER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "Device/Device.pkg"
#include <mutex>

class MDBusArbiter :public Device
{
public:
	// Constructors
	MDBusArbiter(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);

	// Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	// Reference functions
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IDevice* target);
	virtual bool AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target);
	virtual void RemoveReference(IDevice* target);
	virtual void RemoveReference(IBusInterface* target);

	// Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();

	// Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	bool ReadZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data& m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime);
	bool WriteZ80ToM68000(unsigned int m68kMemoryAccessLocation, Data m68kBusData, IDeviceContext* caller, double accessTime, unsigned int accessContext, IBusInterface::AccessResult& m68kBusAccessResult, double& executionTime);

	// CE line state functions
	virtual unsigned int GetCELineID(const Marshal::In<std::wstring>& lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, void* calculateCELineStateContext) const;

	// Line functions
	virtual unsigned int GetLineID(const Marshal::In<std::wstring>& lineName) const;
	virtual Marshal::Ret<std::wstring> GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	// Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	// Enumerations
	enum class CELineID;
	enum class LineID;
	enum class MemoryInterface;

	// Structures
	struct LineAccess;

private:
	// CE line state functions
	unsigned int BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, bool cartInLineAsserted, IDeviceContext* caller, double accessTime) const;
	unsigned int BuildCELineZ80(unsigned int targetAddress) const;

	// Line functions
	void ApplyLineStateChange(LineID targetLine, const Data& lineData, double accessTime);
	void ApplyPendingLineStateChanges(double accessTime);
	bool AdvanceUntilPendingLineStateChangeApplied(IDeviceContext* caller, double accessTime, unsigned int accessContext, LineID targetLine, Data targetLineState, double& lineStateReachedTime);

private:
	// Device references
	IDevice* _bootROM;

	// Bus interfaces
	IBusInterface* _z80MemoryBus;
	IBusInterface* _m68kMemoryBus;

	// Device settings
	bool _activateTMSS;
	bool _bactivateTMSS;
	bool _activateBootROM;
	bool _bactivateBootROM;

	// TMSS Security settings
	bool _bootROMEnabled;
	bool _bbootROMEnabled;
	bool _vdpLockoutActive;
	bool _bvdpLockoutActive;
	mutable bool _vdpLockoutTripped;
	bool _bvdpLockoutTripped;

	// Z80 to M68K memory bankswitch register settings
	mutable std::mutex _bankswitchAccessMutex;
	Data _z80BankswitchDataCurrent;
	Data _bz80BankswitchDataCurrent;
	Data _z80BankswitchDataNew;
	Data _bz80BankswitchDataNew;
	unsigned int _z80BankswitchBitsWritten;
	unsigned int _bz80BankswitchBitsWritten;

	// CE line masks
	unsigned int _ceLineMaskReadHighWriteLow;
	unsigned int _ceLineMaskUDS;
	unsigned int _ceLineMaskLDS;
	unsigned int _ceLineMaskOE0;
	unsigned int _ceLineMaskCE0;
	unsigned int _ceLineMaskBootROM;
	unsigned int _ceLineMaskROM;
	unsigned int _ceLineMaskASEL;
	unsigned int _ceLineMaskFDC;
	unsigned int _ceLineMaskFDWR;
	unsigned int _ceLineMaskTIME;
	unsigned int _ceLineMaskIO;
	unsigned int _ceLineMaskEOE;
	unsigned int _ceLineMaskNOE;
	unsigned int _ceLineMaskZRAM;
	unsigned int _ceLineMaskSOUND;

	// Line access
	std::mutex _lineMutex;
	mutable double _lastLineCheckTime;
	volatile bool _lineAccessPending;
	double _lastTimesliceLength;
	double _blastTimesliceLength;
	std::list<LineAccess> _lineAccessBuffer;
	std::list<LineAccess> _blineAccessBuffer;

	// Line state
	volatile bool _cartInLineState;
	bool _bcartInLineState;
	volatile bool _z80BusRequestLineState;
	bool _bz80BusRequestLineState;
	volatile bool _z80BusGrantLineState;
	bool _bz80BusGrantLineState;
	volatile bool _z80BusResetLineState;
	bool _bz80BusResetLineState;
	volatile bool _m68kBusRequestLineState;
	bool _bm68kBusRequestLineState;
	volatile bool _m68kBusGrantLineState;
	bool _bm68kBusGrantLineState;
	volatile bool _vresLineState;
	bool _bvresLineState;
	volatile bool _haltLineState;
	bool _bhaltLineState;
	volatile bool _sresLineState;
	bool _bsresLineState;
	volatile bool _wresLineState;
	bool _bwresLineState;

	mutable bool _z80BusRequestLineStateChangeTimeLatchEnable;
	mutable bool _z80BusGrantLineStateChangeTimeLatchEnable;
	mutable bool _z80BusResetLineStateChangeTimeLatchEnable;
	mutable bool _m68kBusRequestLineStateChangeTimeLatchEnable;
	mutable bool _m68kBusGrantLineStateChangeTimeLatchEnable;
	mutable double _z80BusRequestLineStateChangeTimeLatch;
	mutable double _z80BusGrantLineStateChangeTimeLatch;
	mutable double _z80BusResetLineStateChangeTimeLatch;
	mutable double _m68kBusRequestLineStateChangeTimeLatch;
	mutable double _m68kBusGrantLineStateChangeTimeLatch;
};

#include "MDBusArbiter.inl"
#endif
