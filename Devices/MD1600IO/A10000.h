/*--------------------------------------------------------------------------------------*\
Things to do:
-Name the bits in the version register properly, corresponding to the names of the
physical lines they test.
-Find out what line bit 4 of the version register is connected to
-Provide a way to set the state of the external lines through the system file
-Implement a debug window to allow line and controller port information to be viewed and
edited
-Confirm and implement the line connections to this device properly, so that it responds
to all reads and writes in the correct manner.
-Fix the locking and access time guards on memory and line access, to prevent overlapped
and out of order access. Remember that a memory access could cause an external line
change, which could in tern result in a line access back to the chip from within a lock.

Crazy ideas:
-We will be linking peripheral devices via system addons. That means all our control
devices will simply have emulation cores like any other device. This means you can have
debug windows for your peripherals.
-You should be able to define a serial loopback device just through an addon xml file,
which connects, for example, control ports 1 and 2 together in serial mode.
-Since we're emulating the physical line connections to a peripheral, it may be possible
to connect a real controller to the computer, and allow the real device to drive input.
We can prevent rollbacks causing problems by buffering input and output commands to
peripheral devices in the device which handles the physical interface, and in the case
of a rollback, the repeated outputs are dropped, and the previous inputs are repeated,
which should keep the physical device consistent. In the case of changes to the control
register, the interface device will need to insert a real-time delay, to allow the
physical device enough time to register the change.

References:
-Sega Genesis I/O Chip and Peripherals, Charles MacDonald
-Sega Genesis Hardware Notes, Charles MacDonald, v0.8 03/02/2001
\*--------------------------------------------------------------------------------------*/
#ifndef __A10000_H__
#define __A10000_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Device/Device.pkg"
#include <mutex>
#include <vector>

//Physical port interface:
//Pin 1 - D0
//Pin 2 - D1
//Pin 3 - D2
//Pin 4 - D3
//Pin 5 - +5V
//Pin 6 - TL
//Pin 7 - TH
//Pin 8 - GND
//Pin 9 - TR

class A10000 :public Device
{
public:
	//Constructors
	A10000(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	using Device::AddReference;
	virtual bool AddReference(const std::wstring& referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual bool SendNotifyAfterExecuteCalled() const;
	virtual void NotifyAfterExecuteCalled();
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const std::wstring& lineName) const;
	virtual std::wstring GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentSetLineState(unsigned int targetLine, const Data& lineData);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void AssertCurrentOutputLineState() const;
	virtual void NegateCurrentOutputLineState() const;

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;

private:
	//Enumerations
	enum class Ports;
	enum class PortLine;
	enum class LineID;

	//Structures
	struct InputLineState;
	struct LineAccess;

private:
	//Line functions
	void AssertCurrentOutputLineStateForPort(Ports portNo) const;
	void NegateCurrentOutputLineStateForPort(Ports portNo) const;
	void ApplyLineStateChange(LineID targetLine, const Data& lineData);
	void ApplyPendingLineStateChanges(double currentTimesliceProgress);
	static LineID GetLineIDForPort(Ports portNo, PortLine portLine);
	static inline unsigned int GetPortIndexForPort(Ports portNo);

	//Data register access
	inline Data ReadDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const;
	inline void WriteDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data);

	//Control register access
	inline Data ReadControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const;
	inline void WriteControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data);

	//Serial control register access
	inline Data ReadSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const;
	inline void WriteSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data);

	//TxData register access
	inline Data ReadTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const;
	inline void WriteTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data);

	//RxData register access
	inline Data ReadRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo) const;
	inline void WriteRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, Ports portNo, const Data& data);

	//Raw register functions
	inline Data GetVersionRegister() const;
	inline void SetVersionRegister(const Data& data);
	inline Data GetDataRegister(Ports portNo) const;
	inline void SetDataRegister(Ports portNo, const Data& data);
	inline Data GetControlRegister(Ports portNo) const;
	inline void SetControlRegister(Ports portNo, const Data& data);
	inline Data GetSerialControlRegister(Ports portNo) const;
	inline void SetSerialControlRegister(Ports portNo, const Data& data);
	inline Data GetTxDataRegister(Ports portNo) const;
	inline void SetTxDataRegister(Ports portNo, const Data& data);
	inline Data GetRxDataRegister(Ports portNo) const;
	inline void SetRxDataRegister(Ports portNo, const Data& data);

	//Version register functions
	inline bool GetOverseasFlag() const;
	inline void SetOverseasFlag(bool data);
	inline bool GetPALFlag() const;
	inline void SetPALFlag(bool data);
	inline bool GetNoDiskFlag() const;
	inline void SetNoDiskFlag(bool data);
	inline unsigned int GetHardwareVersion() const;
	inline void SetHardwareVersion(unsigned int data);

	//Data register functions
	inline bool GetDataRegisterHL(Ports portNo) const;
	inline void SetDataRegisterHL(Ports portNo, bool state);
	inline bool GetDataRegisterTH(Ports portNo) const;
	inline void SetDataRegisterTH(Ports portNo, bool state);
	inline bool GetDataRegisterTR(Ports portNo) const;
	inline void SetDataRegisterTR(Ports portNo, bool state);
	inline bool GetDataRegisterTL(Ports portNo) const;
	inline void SetDataRegisterTL(Ports portNo, bool state);
	inline bool GetDataRegisterD3(Ports portNo) const;
	inline void SetDataRegisterD3(Ports portNo, bool state);
	inline bool GetDataRegisterD2(Ports portNo) const;
	inline void SetDataRegisterD2(Ports portNo, bool state);
	inline bool GetDataRegisterD1(Ports portNo) const;
	inline void SetDataRegisterD1(Ports portNo, bool state);
	inline bool GetDataRegisterD0(Ports portNo) const;
	inline void SetDataRegisterD0(Ports portNo, bool state);

	//Control register functions
	inline bool GetControlRegisterHL(Ports portNo) const;
	inline void SetControlRegisterHL(Ports portNo, bool state);
	inline bool GetControlRegisterTH(Ports portNo) const;
	inline void SetControlRegisterTH(Ports portNo, bool state);
	inline bool GetControlRegisterTR(Ports portNo) const;
	inline void SetControlRegisterTR(Ports portNo, bool state);
	inline bool GetControlRegisterTL(Ports portNo) const;
	inline void SetControlRegisterTL(Ports portNo, bool state);
	inline bool GetControlRegisterD3(Ports portNo) const;
	inline void SetControlRegisterD3(Ports portNo, bool state);
	inline bool GetControlRegisterD2(Ports portNo) const;
	inline void SetControlRegisterD2(Ports portNo, bool state);
	inline bool GetControlRegisterD1(Ports portNo) const;
	inline void SetControlRegisterD1(Ports portNo, bool state);
	inline bool GetControlRegisterD0(Ports portNo) const;
	inline void SetControlRegisterD0(Ports portNo, bool state);

	//Serial control register functions
	inline unsigned int GetSerialBaudRate(Ports portNo) const;
	inline void SetSerialBaudRate(Ports portNo, unsigned int state);
	inline bool GetSerialInputEnabled(Ports portNo) const;
	inline void SetSerialInputEnabled(Ports portNo, bool state);
	inline bool GetSerialOutputEnabled(Ports portNo) const;
	inline void SetSerialOutputEnabled(Ports portNo, bool state);
	inline bool GetSerialInterruptEnabled(Ports portNo) const;
	inline void SetSerialInterruptEnabled(Ports portNo, bool state);
	inline bool GetSerialErrorFlag(Ports portNo) const;
	inline void SetSerialErrorFlag(Ports portNo, bool state);
	inline bool GetRxDataBufferFull(Ports portNo) const;
	inline void SetRxDataBufferFull(Ports portNo, bool state);
	inline bool GetTxDataBufferFull(Ports portNo) const;
	inline void SetTxDataBufferFull(Ports portNo, bool state);

	//Interrupt functions
	void UpdateHLInterruptState(IDeviceContext* caller, double accessTime, unsigned int accessContext);

private:
	//Constants
	static const unsigned int controlPortCount = 3;

	//Bus interface
	IBusInterface* memoryBus;
	IBusInterface* controlPortBus;

	//Version register settings
	Data versionRegister;
	Data bversionRegister;
	unsigned int inputHardwareVersion;
	unsigned int binputHardwareVersion;

	//Control port registers
	mutable std::mutex accessMutex;
	std::vector<Data> dataRegisters;
	std::vector<Data> bdataRegisters;
	std::vector<Data> controlRegisters;
	std::vector<Data> bcontrolRegisters;
	std::vector<Data> serialControlRegisters;
	std::vector<Data> bserialControlRegisters;
	std::vector<Data> txDataRegisters;
	std::vector<Data> btxDataRegisters;
	std::vector<Data> rxDataRegisters;
	std::vector<Data> brxDataRegisters;
	bool currentHLLineState;
	bool bcurrentHLLineState;

	//Input line state
	std::vector<InputLineState> inputLineState;
	std::vector<InputLineState> binputLineState;

	//Line access
	std::mutex lineMutex;
	double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;
};

#include "A10000.inl"
#endif
