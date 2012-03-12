#ifndef __DEBUGWINDOWPARAMS_H__
#define __DEBUGWINDOWPARAMS_H__
#include "WindowFunctions/WindowFunctions.pkg"
class IDevice;
class System;

class DebugWindowParams
{
public:
	//Constructors
	inline DebugWindowParams();
	inline DebugWindowParams(System* asystem);
	inline DebugWindowParams(IDevice* adevice);
	inline DebugWindowParams(const std::wstring& astructName);
	inline DebugWindowParams(const std::wstring& astructName, System* asystem);
	inline DebugWindowParams(const std::wstring& astructName, IDevice* adevice);
	virtual ~DebugWindowParams();

	//Clone function
	virtual DebugWindowParams* Clone();

public:
	System* system;
	IDevice* baseDevice;
	std::wstring previousText;
	unsigned int currentControlFocus;
	bool initializedDialog;
	std::wstring structName;
	DWORD windowStyle;
	DWORD extendedWindowStyle;
};

#include "DebugWindowParams.inl"
#endif
