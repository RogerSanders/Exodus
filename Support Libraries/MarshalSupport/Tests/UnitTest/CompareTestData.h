#ifndef __COMPARETESTDATA_H__
#define __COMPARETESTDATA_H__
#include "MarshalSupport/MarshalSupport.pkg"
using namespace MarshalSupport;
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#endif
#include "catch.hpp"

template<class T>
bool CompareTestData(const T& arg1, const T& arg2)
{
	REQUIRE(arg1 == arg2);
	return (arg1 == arg2);
}

template<class T>
bool CompareTestData(const std::priority_queue<T>& arg1, const std::priority_queue<T>& arg2)
{
	std::priority_queue<T> arg1Copy = arg1;
	std::priority_queue<T> arg2Copy = arg2;

	bool result = true;
	while(!arg1Copy.empty() && !arg2Copy.empty())
	{
		result &= (arg1Copy.top() == arg2Copy.top());
		REQUIRE(arg1Copy.top() == arg2Copy.top());
		arg1Copy.pop();
		arg2Copy.pop();
	}
	REQUIRE(arg1Copy.empty() == arg2Copy.empty());
	return result && (arg1Copy.empty() == arg2Copy.empty());
}

#endif
