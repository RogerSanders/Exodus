#include <ostream>
namespace MarshalSupport {
namespace Marshal {

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
std::basic_ostream<ElementType, traits>& operator<<(std::basic_ostream<ElementType, traits>& stream, const In<std::basic_string<ElementType, traits, Alloc>>& object)
{
	return (stream << object.GetWithoutMove());
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
std::basic_string<ElementType, traits, Alloc> operator+(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() + right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
inline std::basic_string<ElementType, traits, Alloc> operator+(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() + right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
inline std::basic_string<ElementType, traits, Alloc> operator+(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType right)
{
	return (left.GetWithoutMove() + right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
inline std::basic_string<ElementType, traits, Alloc> operator+(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	std::basic_string<ElementType, traits, Alloc> result(left);
	result += right.GetWithoutMove();
	return result;
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
inline std::basic_string<ElementType, traits, Alloc> operator+(const ElementType *left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	std::basic_string<ElementType, traits, Alloc> result(left);
	result += right.GetWithoutMove();
	return result;
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
inline std::basic_string<ElementType, traits, Alloc> operator+(const ElementType left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	std::basic_string<ElementType, traits, Alloc> result(1, left);
	result += right.GetWithoutMove();
	return (result);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator==(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() == right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator==(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() == left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator==(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() == right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator==(const ElementType * left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() == left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator!=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() != right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator!=(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() != left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator!=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() !== right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator!=(const ElementType *left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() != left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() < right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() >= left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() < right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<(const ElementType * left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() >= left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() > right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() <= left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() > right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>(const ElementType * left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() <= left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() <= right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<=(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() > left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() <= right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator<=(const ElementType * left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() > left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const std::basic_string<ElementType, traits, Alloc>& right)
{
	return (left.GetWithoutMove() >= right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>=(const std::basic_string<ElementType, traits, Alloc>& left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() < left);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>=(const In<std::basic_string<ElementType, traits, Alloc>>& left, const ElementType *right)
{
	return (left.GetWithoutMove() >= right);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
bool operator>=(const ElementType * left, const In<std::basic_string<ElementType, traits, Alloc>>& right)
{
	return (right.GetWithoutMove() < left);
}

} //Close namespace Marshal
} //Close namespace MarshalSupport
