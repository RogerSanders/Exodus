#include "MDControl3.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
MDControl3::MDControl3(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Device(implementationName, instanceName, moduleID)
{
	_memoryBus = 0;
	_buttonPressed.resize(buttonCount);
	_bbuttonPressed.resize(buttonCount);
}

//----------------------------------------------------------------------------------------------------------------------
// Initialization functions
//----------------------------------------------------------------------------------------------------------------------
void MDControl3::Initialize()
{
	for (unsigned int i = 0; i < buttonCount; ++i)
	{
		_buttonPressed[i] = false;
	}
	_lineInputStateTH = false;
	_lineAssertedD0 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D0);
	_lineAssertedD1 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D1);
	_lineAssertedD2 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D2);
	_lineAssertedD3 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D3);
	_lineAssertedTL = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TL);
	_lineAssertedTR = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TR);
	_lineAssertedTH = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TH);
	_lastLineAccessTime = 0;
	_currentTimesliceLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool MDControl3::ValidateDevice()
{
	return (_memoryBus != 0);
}

//----------------------------------------------------------------------------------------------------------------------
// Reference functions
//----------------------------------------------------------------------------------------------------------------------
bool MDControl3::AddReference(const Marshal::In<std::wstring>& referenceName, IBusInterface* target)
{
	if (referenceName == L"BusInterface")
	{
		_memoryBus = target;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::RemoveReference(IBusInterface* target)
{
	if (_memoryBus == target)
	{
		_memoryBus = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
bool MDControl3::SendNotifyUpcomingTimeslice() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::NotifyUpcomingTimeslice(double nanoseconds)
{
	_currentTimesliceLength = nanoseconds;
	_lastLineAccessTime = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::ExecuteRollback()
{
	for (unsigned int i = 0; i < buttonCount; ++i)
	{
		_buttonPressed[i] = _bbuttonPressed[i];
	}
	_lineInputStateTH = _blineInputStateTH;
	_lineAssertedD0 = _blineAssertedD0;
	_lineAssertedD1 = _blineAssertedD1;
	_lineAssertedD2 = _blineAssertedD2;
	_lineAssertedD3 = _blineAssertedD3;
	_lineAssertedTL = _blineAssertedTL;
	_lineAssertedTR = _blineAssertedTR;
	_lineAssertedTH = _blineAssertedTH;
	_currentTimesliceLength = _bcurrentTimesliceLength;
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::ExecuteCommit()
{
	for (unsigned int i = 0; i < buttonCount; ++i)
	{
		_bbuttonPressed[i] = _buttonPressed[i];
	}
	_blineInputStateTH = _lineInputStateTH;
	_blineAssertedD0 = _lineAssertedD0;
	_blineAssertedD1 = _lineAssertedD1;
	_blineAssertedD2 = _lineAssertedD2;
	_blineAssertedD3 = _lineAssertedD3;
	_blineAssertedTL = _lineAssertedTL;
	_blineAssertedTR = _lineAssertedTR;
	_blineAssertedTH = _lineAssertedTH;
	_bcurrentTimesliceLength = _currentTimesliceLength;
}

//----------------------------------------------------------------------------------------------------------------------
// Line functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineID(const Marshal::In<std::wstring>& lineName) const
{
	if (lineName == L"D0")
	{
		return (unsigned int)LineID::D0;
	}
	else if (lineName == L"D1")
	{
		return (unsigned int)LineID::D1;
	}
	else if (lineName == L"D2")
	{
		return (unsigned int)LineID::D2;
	}
	else if (lineName == L"D3")
	{
		return (unsigned int)LineID::D3;
	}
	else if (lineName == L"TL")
	{
		return (unsigned int)LineID::TL;
	}
	else if (lineName == L"TR")
	{
		return (unsigned int)LineID::TR;
	}
	else if (lineName == L"TH")
	{
		return (unsigned int)LineID::TH;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> MDControl3::GetLineName(unsigned int lineID) const
{
	switch ((LineID)lineID)
	{
	case LineID::D0:
		return L"D0";
	case LineID::D1:
		return L"D1";
	case LineID::D2:
		return L"D2";
	case LineID::D3:
		return L"D3";
	case LineID::TL:
		return L"TL";
	case LineID::TR:
		return L"TR";
	case LineID::TH:
		return L"TH";
	}
	return L"";
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int MDControl3::GetLineWidth(unsigned int lineID) const
{
	switch ((LineID)lineID)
	{
	case LineID::D0:
	case LineID::D1:
	case LineID::D2:
	case LineID::D3:
	case LineID::TL:
	case LineID::TR:
	case LineID::TH:
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::SetLineState(unsigned int targetLine, const Data& lineData, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_lineMutex);

	// Read the time at which this access is being made, and trigger a rollback if the
	// device has been accessed out of order.
	if (_lastLineAccessTime > accessTime)
	{
		GetSystemInterface().SetSystemRollback(GetDeviceContext(), caller, accessTime, accessContext);
	}
	_lastLineAccessTime = accessTime;

	// If the TH line has been toggled, select the currently enabled bank.
	if (targetLine == LineID::TH)
	{
		_lineInputStateTH = lineData.GetBit(0);
	}

	// We explicitly release our lock on lineMutex here so that we're not blocking access
	// to SetLineState() on this class before we modify the line state for other devices in
	// the code that follows. Adhering to this pattern helps to avoid deadlock cases that
	// could otherwise arise from valid line mappings.
	lock.unlock();

	// If an input line state has changed, re-evaluate the state of the output lines.
	UpdateLineState(caller, accessTime, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::TransparentSetLineState(unsigned int targetLine, const Data& lineData)
{
	SetLineState(targetLine, lineData, 0, _currentTimesliceLength, 0);
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::AssertCurrentOutputLineState() const
{
	if (_memoryBus != 0)
	{
		if (_lineAssertedD0) _memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD1) _memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD2) _memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD3) _memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTL) _memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTR) _memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTH) _memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 1), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::NegateCurrentOutputLineState() const
{
	if (_memoryBus != 0)
	{
		if (_lineAssertedD0) _memoryBus->SetLineState((unsigned int)LineID::D0, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD1) _memoryBus->SetLineState((unsigned int)LineID::D1, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD2) _memoryBus->SetLineState((unsigned int)LineID::D2, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedD3) _memoryBus->SetLineState((unsigned int)LineID::D3, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTL) _memoryBus->SetLineState((unsigned int)LineID::TL, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTR) _memoryBus->SetLineState((unsigned int)LineID::TR, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
		if (_lineAssertedTH) _memoryBus->SetLineState((unsigned int)LineID::TH, Data(1, 0), GetDeviceContext(), GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::UpdateLineState(IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	bool newLineStateD0 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D0);
	if (_lineAssertedD0 != newLineStateD0)
	{
		_lineAssertedD0 = newLineStateD0;
		_memoryBus->SetLineState(LineID::D0, Data(1, (unsigned int)_lineAssertedD0), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD1 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D1);
	if (_lineAssertedD1 != newLineStateD1)
	{
		_lineAssertedD1 = newLineStateD1;
		_memoryBus->SetLineState(LineID::D1, Data(1, (unsigned int)_lineAssertedD1), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD2 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D2);
	if (_lineAssertedD2 != newLineStateD2)
	{
		_lineAssertedD2 = newLineStateD2;
		_memoryBus->SetLineState(LineID::D2, Data(1, (unsigned int)_lineAssertedD2), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateD3 = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::D3);
	if (_lineAssertedD3 != newLineStateD3)
	{
		_lineAssertedD3 = newLineStateD3;
		_memoryBus->SetLineState(LineID::D3, Data(1, (unsigned int)_lineAssertedD3), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTL = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TL);
	if (_lineAssertedTL != newLineStateTL)
	{
		_lineAssertedTL = newLineStateTL;
		_memoryBus->SetLineState(LineID::TL, Data(1, (unsigned int)_lineAssertedTL), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTR = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TR);
	if (_lineAssertedTR != newLineStateTR)
	{
		_lineAssertedTR = newLineStateTR;
		_memoryBus->SetLineState(LineID::TR, Data(1, (unsigned int)_lineAssertedTR), GetDeviceContext(), caller, accessTime, accessContext);
	}
	bool newLineStateTH = GetDesiredLineState(_lineInputStateTH, _buttonPressed, LineID::TH);
	if (_lineAssertedTH != newLineStateTH)
	{
		_lineAssertedTH = newLineStateTH;
		_memoryBus->SetLineState(LineID::TH, Data(1, (unsigned int)_lineAssertedTH), GetDeviceContext(), caller, accessTime, accessContext);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool MDControl3::GetDesiredLineState(unsigned int currentLineInputStateTH, const std::vector<bool>& currentButtonPressedState, LineID lineID)
{
	if (currentLineInputStateTH)
	{
		// This state is selected when TH is configured as an input and set to 1
		// D0 = Up
		// D1 = Down
		// D2 = Left
		// D3 = Right
		// TL = B
		// TR = C
		// TH = Null (+5v)
		switch (lineID)
		{
		case LineID::D0:
			return !currentButtonPressedState[BUTTONINDEX_UP];
		case LineID::D1:
			return !currentButtonPressedState[BUTTONINDEX_DOWN];
		case LineID::D2:
			return !currentButtonPressedState[BUTTONINDEX_LEFT];
		case LineID::D3:
			return !currentButtonPressedState[BUTTONINDEX_RIGHT];
		case LineID::TL:
			return !currentButtonPressedState[BUTTONINDEX_B];
		case LineID::TR:
			return !currentButtonPressedState[BUTTONINDEX_C];
		case LineID::TH:
			return true;
		}
	}
	else
	{
		// This state is selected when TH is configured as an input and set to 0
		// D0 = Up
		// D1 = Down
		// D2 = Null (grounded)
		// D3 = Null (grounded)
		// TL = A
		// TR = Start
		// TH = Null (+5v)
		switch (lineID)
		{
		case LineID::D0:
			return !currentButtonPressedState[BUTTONINDEX_UP];
		case LineID::D1:
			return !currentButtonPressedState[BUTTONINDEX_DOWN];
		case LineID::D2:
			return false;
		case LineID::D3:
			return false;
		case LineID::TL:
			return !currentButtonPressedState[BUTTONINDEX_A];
		case LineID::TR:
			return !currentButtonPressedState[BUTTONINDEX_START];
		case LineID::TH:
			return true;
		}
	}

	//##TODO## Raise an assert if we end up here with an invalid setting for the target
	// line.
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Input functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MDControl3::GetKeyCodeID(const Marshal::In<std::wstring>& keyCodeName) const
{
	if (keyCodeName == L"Up")
	{
		return BUTTONINDEX_UP+1;
	}
	else if (keyCodeName == L"Down")
	{
		return BUTTONINDEX_DOWN+1;
	}
	else if (keyCodeName == L"Left")
	{
		return BUTTONINDEX_LEFT+1;
	}
	else if (keyCodeName == L"Right")
	{
		return BUTTONINDEX_RIGHT+1;
	}
	else if (keyCodeName == L"A")
	{
		return BUTTONINDEX_A+1;
	}
	else if (keyCodeName == L"B")
	{
		return BUTTONINDEX_B+1;
	}
	else if (keyCodeName == L"C")
	{
		return BUTTONINDEX_C+1;
	}
	else if (keyCodeName == L"Start")
	{
		return BUTTONINDEX_START+1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> MDControl3::GetKeyCodeName(unsigned int keyCodeID) const
{
	switch (keyCodeID)
	{
	case BUTTONINDEX_UP+1:
		return L"Up";
	case BUTTONINDEX_DOWN+1:
		return L"Down";
	case BUTTONINDEX_LEFT+1:
		return L"Left";
	case BUTTONINDEX_RIGHT+1:
		return L"Right";
	case BUTTONINDEX_A+1:
		return L"A";
	case BUTTONINDEX_B+1:
		return L"B";
	case BUTTONINDEX_C+1:
		return L"C";
	case BUTTONINDEX_START+1:
		return L"Start";
	}
	return L"";
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyDown(unsigned int keyCodeID)
{
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
	_buttonPressed[keyCode] = true;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::HandleInputKeyUp(unsigned int keyCodeID)
{
	ButtonIndex keyCode = (ButtonIndex)(keyCodeID-1);
	_buttonPressed[keyCode] = false;
	UpdateLineState(GetDeviceContext(), GetCurrentTimesliceProgress(), 0);
}

//----------------------------------------------------------------------------------------------------------------------
// Savestate functions
//----------------------------------------------------------------------------------------------------------------------
void MDControl3::LoadState(IHierarchicalStorageNode& node)
{
	std::list<IHierarchicalStorageNode*> childList = node.GetChildList();
	for (std::list<IHierarchicalStorageNode*>::iterator i = childList.begin(); i != childList.end(); ++i)
	{
		IHierarchicalStorageNode& childNode = *(*i);
		std::wstring nodeName = childNode.GetName();
		if (nodeName == L"LineInputStateTH")
		{
			childNode.ExtractData(_lineInputStateTH);
		}
		else if (nodeName == L"LineAssertedD0")
		{
			childNode.ExtractData(_lineAssertedD0);
		}
		else if (nodeName == L"LineAssertedD1")
		{
			childNode.ExtractData(_lineAssertedD1);
		}
		else if (nodeName == L"LineAssertedD2")
		{
			childNode.ExtractData(_lineAssertedD2);
		}
		else if (nodeName == L"LineAssertedD3")
		{
			childNode.ExtractData(_lineAssertedD3);
		}
		else if (nodeName == L"LineAssertedTL")
		{
			childNode.ExtractData(_lineAssertedTL);
		}
		else if (nodeName == L"LineAssertedTR")
		{
			childNode.ExtractData(_lineAssertedTR);
		}
		else if (nodeName == L"LineAssertedTH")
		{
			childNode.ExtractData(_lineAssertedTH);
		}
		else if (nodeName == L"ButtonPressed")
		{
			IHierarchicalStorageAttribute* buttonNumberAttribute = childNode.GetAttribute(L"ButtonNo");
			if (buttonNumberAttribute != 0)
			{
				unsigned int buttonNo = buttonNumberAttribute->ExtractValue<unsigned int>();
				if (buttonNo < buttonCount)
				{
					bool state;
					childNode.ExtractAttribute(L"Pressed", state);
					_buttonPressed[buttonNo] = state;
				}
			}
		}
		else if (nodeName == L"CurrentTimesliceLength")
		{
			childNode.ExtractData(_currentTimesliceLength);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void MDControl3::SaveState(IHierarchicalStorageNode& node) const
{
	node.CreateChild(L"LineInputStateTH", _lineInputStateTH);
	node.CreateChild(L"LineAssertedD0", _lineAssertedD0);
	node.CreateChild(L"LineAssertedD1", _lineAssertedD1);
	node.CreateChild(L"LineAssertedD2", _lineAssertedD2);
	node.CreateChild(L"LineAssertedD3", _lineAssertedD3);
	node.CreateChild(L"LineAssertedTL", _lineAssertedTL);
	node.CreateChild(L"LineAssertedTR", _lineAssertedTR);
	node.CreateChild(L"LineAssertedTH", _lineAssertedTH);
	for (unsigned int i = 0; i < buttonCount; ++i)
	{
		node.CreateChild(L"ButtonPressed").CreateAttribute(L"ButtonNo", i).CreateAttribute(L"Pressed", _buttonPressed[i]);
	}
	node.CreateChild(L"CurrentTimesliceLength", _currentTimesliceLength);
}
