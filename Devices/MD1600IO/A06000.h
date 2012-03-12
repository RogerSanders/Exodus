#ifndef __A06000_H__
#define __A06000_H__
#include "SystemInterface/SystemInterface.pkg"
#include "Device/Device.pkg"

class A06000 :public Device
{
public:
	//Constructors
	A06000(const std::wstring& ainstanceName, unsigned int amoduleID);

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
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime);

	//Memory map functions
	void SetMappingPos(unsigned int mappingPos);

private:
	IBusInterface* z80MemoryBus;
	IBusInterface* m68kMemoryBus;

	unsigned int currentMappingPos;
	unsigned int newMappingPos;
	unsigned int bitsWritten;

	unsigned int bcurrentMappingPos;
	unsigned int bnewMappingPos;
	unsigned int bbitsWritten;
};

#endif
