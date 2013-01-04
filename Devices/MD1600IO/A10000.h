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
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"
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
	A10000(const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual bool SendNotifyUpcomingTimeslice() const;
	virtual void NotifyUpcomingTimeslice(double nanoseconds);
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void RevokeSetLineState(unsigned int targetLine, const Data& lineData, double reportedTime, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;

private:
	//Enumerations
	enum Ports;
	enum PortLine;
	enum LineID;

	//Structures
	struct InputLineState;
	struct LineAccess;

private:
	//Line functions
	void ApplyLineStateChange(unsigned int targetLine, const Data& lineData);
	void ApplyPendingLineStateChanges(double currentTimesliceProgress);
	LineID GetLineIDForPort(unsigned int portNo, PortLine portLine);

	//Data register access
	inline Data ReadDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const;
	inline void WriteDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data);

	//Control register access
	inline Data ReadControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const;
	inline void WriteControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data);

	//Serial control register access
	inline Data ReadSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const;
	inline void WriteSerialControlRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data);

	//TxData register access
	inline Data ReadTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const;
	inline void WriteTxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data);

	//RxData register access
	inline Data ReadRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo) const;
	inline void WriteRxDataRegister(IDeviceContext* caller, double accessTime, unsigned int accessContext, unsigned int portNo, const Data& data);

	//Raw register functions
	inline Data GetVersionRegister() const;
	inline void SetVersionRegister(const Data& data);
	inline Data GetDataRegister(unsigned int portNo) const;
	inline void SetDataRegister(unsigned int portNo, const Data& data);
	inline Data GetControlRegister(unsigned int portNo) const;
	inline void SetControlRegister(unsigned int portNo, const Data& data);
	inline Data GetSerialControlRegister(unsigned int portNo) const;
	inline void SetSerialControlRegister(unsigned int portNo, const Data& data);
	inline Data GetTxDataRegister(unsigned int portNo) const;
	inline void SetTxDataRegister(unsigned int portNo, const Data& data);
	inline Data GetRxDataRegister(unsigned int portNo) const;
	inline void SetRxDataRegister(unsigned int portNo, const Data& data);

	//Version register functions
	inline bool GetRegionFlag() const;
	inline void SetRegionFlag(bool data);
	inline bool GetVideoFlag() const;
	inline void SetVideoFlag(bool data);
	inline bool GetSegaCDFlag() const;
	inline void SetSegaCDFlag(bool data);
	inline unsigned int GetHardwareVersion() const;
	inline void SetHardwareVersion(unsigned int data);

	//Data register functions
	inline bool GetDataRegisterHL(unsigned int portNo);
	inline void SetDataRegisterHL(unsigned int portNo, bool state);
	inline bool GetDataRegisterTH(unsigned int portNo);
	inline void SetDataRegisterTH(unsigned int portNo, bool state);
	inline bool GetDataRegisterTR(unsigned int portNo);
	inline void SetDataRegisterTR(unsigned int portNo, bool state);
	inline bool GetDataRegisterTL(unsigned int portNo);
	inline void SetDataRegisterTL(unsigned int portNo, bool state);
	inline bool GetDataRegisterD3(unsigned int portNo);
	inline void SetDataRegisterD3(unsigned int portNo, bool state);
	inline bool GetDataRegisterD2(unsigned int portNo);
	inline void SetDataRegisterD2(unsigned int portNo, bool state);
	inline bool GetDataRegisterD1(unsigned int portNo);
	inline void SetDataRegisterD1(unsigned int portNo, bool state);
	inline bool GetDataRegisterD0(unsigned int portNo);
	inline void SetDataRegisterD0(unsigned int portNo, bool state);

	//Control register functions
	inline bool GetControlRegisterHL(unsigned int portNo);
	inline void SetControlRegisterHL(unsigned int portNo, bool state);
	inline bool GetControlRegisterTH(unsigned int portNo);
	inline void SetControlRegisterTH(unsigned int portNo, bool state);
	inline bool GetControlRegisterTR(unsigned int portNo);
	inline void SetControlRegisterTR(unsigned int portNo, bool state);
	inline bool GetControlRegisterTL(unsigned int portNo);
	inline void SetControlRegisterTL(unsigned int portNo, bool state);
	inline bool GetControlRegisterD3(unsigned int portNo);
	inline void SetControlRegisterD3(unsigned int portNo, bool state);
	inline bool GetControlRegisterD2(unsigned int portNo);
	inline void SetControlRegisterD2(unsigned int portNo, bool state);
	inline bool GetControlRegisterD1(unsigned int portNo);
	inline void SetControlRegisterD1(unsigned int portNo, bool state);
	inline bool GetControlRegisterD0(unsigned int portNo);
	inline void SetControlRegisterD0(unsigned int portNo, bool state);

	//Serial control register functions
	inline unsigned int GetSerialBaudRate(unsigned int portNo);
	inline void SetSerialBaudRate(unsigned int portNo, unsigned int state);
	inline bool GetSerialInputEnabled(unsigned int portNo);
	inline void SetSerialInputEnabled(unsigned int portNo, bool state);
	inline bool GetSerialOutputEnabled(unsigned int portNo);
	inline void SetSerialOutputEnabled(unsigned int portNo, bool state);
	inline bool GetSerialInterruptEnabled(unsigned int portNo);
	inline void SetSerialInterruptEnabled(unsigned int portNo, bool state);
	inline bool GetSerialErrorFlag(unsigned int portNo);
	inline void SetSerialErrorFlag(unsigned int portNo, bool state);
	inline bool GetRxDataBufferFull(unsigned int portNo);
	inline void SetRxDataBufferFull(unsigned int portNo, bool state);
	inline bool GetTxDataBufferFull(unsigned int portNo);
	inline void SetTxDataBufferFull(unsigned int portNo, bool state);

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

	//Control port registers
	mutable boost::mutex accessMutex;
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
	boost::mutex lineMutex;
	double lastLineCheckTime;
	volatile bool lineAccessPending;
	double lastTimesliceLength;
	double blastTimesliceLength;
	std::list<LineAccess> lineAccessBuffer;
	std::list<LineAccess> blineAccessBuffer;
};

#include "A10000.inl"
#endif
