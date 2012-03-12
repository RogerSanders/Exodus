#ifndef __A11100_H__
#define __A11100_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"
#include <vector>

//Note that the 315-5364 acts as this device in the Mega Drive, as it's the device which
//controls the Z80 RESET(ZRES) and BUSREQ(ZBR) lines.

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
	virtual UpdateMethod GetUpdateMethod() const;
	virtual void ExecuteTimeslice(double nanoseconds);

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime);

	//Line functions
	virtual unsigned int GetLineID(const wchar_t* lineName) const;
	virtual const wchar_t* GetLineName(unsigned int lineID) const;
	virtual unsigned int GetLineWidth(unsigned int lineID) const;

private:
	enum LineID
	{
		LINE_RESET = 1,
		LINE_BUSREQ
	};

	//Bus interface
	IBusInterface* memoryBus;

	bool busLocked;
	bool reset;

	bool bbusLocked;
	bool breset;
	bool initialized;
};

#endif
