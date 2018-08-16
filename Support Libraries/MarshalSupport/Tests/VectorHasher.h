#ifndef __VECTORHASHER_H__
#define __VECTORHASHER_H__
#include <vector>
#include <functional>

template<class T>
struct VectorHasher
{
	size_t operator()(const std::vector<T>& target) const
	{
		std::hash<T> hasher;
		return hasher(target[0]);
	}
};

#endif
