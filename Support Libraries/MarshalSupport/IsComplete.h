#ifndef __ISCOMPLETE_H__
#define __ISCOMPLETE_H__
#include "MarshalPreprocessorMacros.h"
namespace MarshalSupport {
namespace Internal {

// Based on code from https://stackoverflow.com/questions/1625105/how-to-write-is-complete-template
//##TODO## Test this on older compilers. We don't need this if there's no C++11 support, but check if it works on VS2010
//for example.
template <class T>
struct is_complete
{
private:
	//Typedefs
	typedef char(&yes)[1];
	typedef char(&no)[2];

private:
	//Check function
	template <class U, class = decltype(sizeof(std::declval<U>()))>
	static yes check(U*);
	template <class U>
	static no check(...);

public:
	// Result
	static const bool value = (sizeof(check<T>(nullptr)) == sizeof(yes));
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
