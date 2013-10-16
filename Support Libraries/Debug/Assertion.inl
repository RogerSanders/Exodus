#include <assert.h>
#include <iostream>
#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------
//Assert functions
//----------------------------------------------------------------------------------------
void Assert(bool expression, unsigned int line, const wchar_t* file, const wchar_t* message)
{
	if(!expression)
	{
		std::wcout << L"Assert failed:\n"
		           << L"-Expression:\t" << message << '\n'
				   << L"-File:\t" << file << '\n'
				   << L"-Line:\t" << std::dec << line << '\n';
		//##TODO## Implement our own message box, and eliminate use of the CRT assert
		//function.
		assert(expression);
		RaiseException(ASSERTFAILUREEXCEPTIONCODE, 0, 0, NULL);
	}
}
