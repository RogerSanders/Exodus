#ifndef __ISBASEOFEMULATOR_H__
#define __ISBASEOFEMULATOR_H__
namespace MarshalSupport {
namespace Internal {

template<class B, class D>
struct is_base_of_emulator
{
private:
	//Typedefs
	typedef char(&yes)[1];
	typedef char(&no)[2];

	//Nested Types
	template<class B, class D>
	struct Host
	{
		operator B*() const;
		operator D*();
	};

private:
	//Check function
	static no check(B*, int);
	template<class T>
	static yes check(D*, T);

public:
	//Result
	static const bool value = (sizeof(check(Host<B, D>(), int())) == sizeof(yes));
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
