#ifndef __VIEWMANIPULATORHEX_H__
#define __VIEWMANIPULATORHEX_H__
#include <iomanip>
namespace Stream {
class ViewText;

struct Hex
{
public:
	//Constructors
	inline Hex(unsigned int length = 0);

	//Character length functions
	inline unsigned int GetLength() const;
	inline void SetLength(unsigned int length);

	//Stream operators
	friend inline ViewText& operator>>(ViewText& stream, const Hex& object);
	friend inline ViewText& operator<<(ViewText& stream, const Hex& object);

private:
	unsigned int _length;
};

} //Close namespace Stream
#include "ViewManipulatorHex.inl"
#endif
