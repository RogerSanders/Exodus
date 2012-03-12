#ifndef __MDBUSARBITER_H__
#define __MDBUSARBITER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"
#include <boost/thread/mutex.hpp>

class MDBusArbiter :public Device
{
public:
	//Constructors
	MDBusArbiter(const std::wstring& ainstanceName, unsigned int amoduleID);

	//Initialization functions
	virtual bool ValidateDevice();
	virtual void Initialize();

	//Reference functions
	virtual bool AddReference(const wchar_t* referenceName, IBusInterface* target);
	virtual bool RemoveReference(IBusInterface* target);

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime);

	//CE line state functions
	virtual unsigned int GetCELineID(const wchar_t* lineName, bool inputLine) const;
	virtual void SetCELineInput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual void SetCELineOutput(unsigned int lineID, bool lineMapped, unsigned int lineStartBitNumber);
	virtual unsigned int CalculateCELineStateMemory(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller, double accessTime) const;
	virtual unsigned int CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, unsigned int currentCELineState, const IBusInterface* sourceBusInterface, IDeviceContext* caller) const;

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;
	virtual void SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime);

private:
	//Enumerations
	enum CELineID
	{
		CELINE_RW = 1,
		CELINE_UDS,
		CELINE_LDS,
		CELINE_OE0,
		CELINE_CE0,
		CELINE_ROM,
		CELINE_ASEL,
		CELINE_FDC,
		CELINE_FDWR,
		CELINE_TIME,
		CELINE_IO,
		CELINE_EOE,
		CELINE_NOE,
		CELINE_ZRAM,
		CELINE_SOUND
	};

private:
	//CE line state functions
	unsigned int BuildCELineM68K(unsigned int targetAddress, bool write, bool ceLineUDS, bool ceLineLDS, bool ceLineOE0, bool cartInLineAsserted) const;
	unsigned int BuildCELineZ80(unsigned int targetAddress) const;

private:
	//Bus interfaces
	IBusInterface* z80MemoryBus;
	IBusInterface* m68kMemoryBus;

	//Line access
	enum LineID
	{
		LINE_CARTIN = 1,
		LINE_VPA,
		LINE_INTAK
	};
	boost::mutex lineMutex;
	double lastLineCheckTime;

	//Line state
	bool cartIn;
	bool bcartIn;

	//CE line masks
	unsigned int ceLineMaskReadHighWriteLow;
	unsigned int ceLineMaskUDS;
	unsigned int ceLineMaskLDS;
	unsigned int ceLineMaskOE0;
	unsigned int ceLineMaskCE0;
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
};

#endif
