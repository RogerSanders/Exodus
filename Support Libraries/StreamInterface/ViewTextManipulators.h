#ifndef __VIEWTEXTMANIPULATORS_H__
#define __VIEWTEXTMANIPULATORS_H__
#include <iomanip>
namespace Stream {
class ViewText;

struct Hex
{
public:
	//Constructors
	inline Hex(unsigned int alength = 0);

	//Character length functions
	inline unsigned int GetLength() const;
	inline void SetLength(unsigned int alength);

	//Stream operators
	friend inline ViewText& operator>>(ViewText& stream, const Hex& object);
	friend inline ViewText& operator<<(ViewText& stream, const Hex& object);

private:
	unsigned int length;
};

} //Close namespace Stream
#include "ViewTextManipulators.inl"
#endif
