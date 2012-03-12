#ifndef __ILOGENTRY_H__
#define __ILOGENTRY_H__
#include <string>

class ILogEntry
{
public:
	//Enumerations
	enum EventLevel;

	//Constructors
	virtual ~ILogEntry() = 0 {}

	//Getters
	virtual EventLevel GetEventLevel() const = 0;
	inline std::wstring GetText() const;
	inline std::wstring GetSource() const;
	inline std::wstring GetEventLevelString() const;
	inline std::wstring GetTimeString() const;

	//Setters
	inline void OverrideSourceText(const std::wstring& asource) const;

	//Version functions
	virtual unsigned int GetInterfaceVersion() const = 0;

protected:
	//Getters
	virtual const wchar_t* GetTextInternal() const = 0;
	virtual const wchar_t* GetSourceInternal() const = 0;
	virtual const wchar_t* GetEventLevelStringInternal() const = 0;
	virtual const wchar_t* GetTimeStringInternal() const = 0;

	//Setters
	virtual void OverrideSourceTextInternal(const wchar_t* asource) const = 0;
};

#include "ILogEntry.inl"
#endif
