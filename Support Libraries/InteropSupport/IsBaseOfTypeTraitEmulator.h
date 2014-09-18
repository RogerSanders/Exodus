#ifndef __ISBASEOFTYPETRAITEMULATOR_H__
#define __ISBASEOFTYPETRAITEMULATOR_H__
namespace InteropSupport {

//##TODO## Reformat and comment this. See the following:
//http://stackoverflow.com/questions/2910979/how-is-base-of-works
template<class B, class D>
struct is_base_of_emulator
{
	template<class B, class D>
	struct Host
	{
		operator B*() const;
		operator D*();
	};

	typedef char (&yes)[1];
	typedef char (&no)[2];

	template<class T>
	static yes check(D*, T);
	static no check(B*, int);

	static const bool result = (sizeof(check(Host<B,D>(), int())) == sizeof(yes));
};

} //Close namespace InteropSupport
#endif
