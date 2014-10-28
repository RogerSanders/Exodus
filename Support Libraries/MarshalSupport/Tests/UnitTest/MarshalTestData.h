#ifndef __MARSHALTESTDATA_H__
#define __MARSHALTESTDATA_H__
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
#include <functional>
#endif
#include "CustomMarshalObject.h"

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
namespace std {
template <>
struct hash<CustomMarshalObject>
{
	std::size_t operator()(const CustomMarshalObject& k) const
	{
		return hash<unsigned int>()(k.sourceAddress) ^ hash<unsigned int>()(k.targetAddress) ^ hash<unsigned int>()(k.returnAddress) ^ hash<std::wstring>()(k.disassembly);
	}
};
}

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

class MarshalTestData
{
public:
	MarshalTestData()
	:testCustomMarshaller30(1)
	{
		//In tests
		testPrimitive01.push_back(1); testPrimitive01.push_back(2); testPrimitive01.push_back(3); testPrimitive01.push_back(4); testPrimitive01.push_back(5);
		testPrimitive02.push_back(1); testPrimitive02.push_back(2); testPrimitive02.push_back(3); testPrimitive02.push_back(4); testPrimitive02.push_back(5);
		testPrimitive03.push_back(1); testPrimitive03.push_back(2); testPrimitive03.push_back(3); testPrimitive03.push_back(4); testPrimitive03.push_back(5);
		testPrimitive04.insert(1); testPrimitive04.insert(2); testPrimitive04.insert(3); testPrimitive04.insert(4); testPrimitive04.insert(5);
		testPrimitive05.insert(1); testPrimitive05.insert(2); testPrimitive05.insert(3); testPrimitive05.insert(4); testPrimitive05.insert(5); testPrimitive05.insert(1); testPrimitive05.insert(2); testPrimitive05.insert(3); testPrimitive05.insert(4); testPrimitive05.insert(5);
		testPrimitive06.insert(std::pair<int, int>(1, 1)); testPrimitive06.insert(std::pair<int, int>(2, 2)); testPrimitive06.insert(std::pair<int, int>(3, 3)); testPrimitive06.insert(std::pair<int, int>(4, 4)); testPrimitive06.insert(std::pair<int, int>(5, 5));
		testPrimitive07.insert(std::pair<int, int>(1, 1)); testPrimitive07.insert(std::pair<int, int>(2, 2)); testPrimitive07.insert(std::pair<int, int>(3, 3)); testPrimitive07.insert(std::pair<int, int>(4, 4)); testPrimitive07.insert(std::pair<int, int>(5, 5)); testPrimitive07.insert(std::pair<int, int>(1, 1)); testPrimitive07.insert(std::pair<int, int>(2, 2)); testPrimitive07.insert(std::pair<int, int>(3, 3)); testPrimitive07.insert(std::pair<int, int>(4, 4)); testPrimitive07.insert(std::pair<int, int>(5, 5));
		testPrimitive08.push(5); testPrimitive08.push(4); testPrimitive08.push(3); testPrimitive08.push(2); testPrimitive08.push(1);
		testPrimitive09.push(1); testPrimitive09.push(2); testPrimitive09.push(3); testPrimitive09.push(4); testPrimitive09.push(5);
		testPrimitive10.push(1); testPrimitive10.push(2); testPrimitive10.push(3); testPrimitive10.push(4); testPrimitive10.push(5);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		testPrimitive20[0] = 1; testPrimitive20[1] = 2; testPrimitive20[2] = 3; testPrimitive20[3] = 4; testPrimitive20[4] = 5; testPrimitive20[5] = 1; testPrimitive20[6] = 2; testPrimitive20[7] = 3; testPrimitive20[8] = 4; testPrimitive20[9] = 5;
		testPrimitive21.push_front(5); testPrimitive21.push_front(4); testPrimitive21.push_front(3); testPrimitive21.push_front(2); testPrimitive21.push_front(1);
		testPrimitive22.insert(1); testPrimitive22.insert(2); testPrimitive22.insert(3); testPrimitive22.insert(4); testPrimitive22.insert(5);
		testPrimitive23.insert(1); testPrimitive23.insert(2); testPrimitive23.insert(3); testPrimitive23.insert(4); testPrimitive23.insert(5); testPrimitive23.insert(1); testPrimitive23.insert(2); testPrimitive23.insert(3); testPrimitive23.insert(4); testPrimitive23.insert(5);
		testPrimitive24.insert(std::pair<int, int>(1, 1)); testPrimitive24.insert(std::pair<int, int>(2, 2)); testPrimitive24.insert(std::pair<int, int>(3, 3)); testPrimitive24.insert(std::pair<int, int>(4, 4)); testPrimitive24.insert(std::pair<int, int>(5, 5));
		testPrimitive25.insert(std::pair<int, int>(1, 1)); testPrimitive25.insert(std::pair<int, int>(2, 2)); testPrimitive25.insert(std::pair<int, int>(3, 3)); testPrimitive25.insert(std::pair<int, int>(4, 4)); testPrimitive25.insert(std::pair<int, int>(5, 5)); testPrimitive25.insert(std::pair<int, int>(1, 1)); testPrimitive25.insert(std::pair<int, int>(2, 2)); testPrimitive25.insert(std::pair<int, int>(3, 3)); testPrimitive25.insert(std::pair<int, int>(4, 4)); testPrimitive25.insert(std::pair<int, int>(5, 5));
#endif
		testPrimitive30 = 12345;
		std::list<std::set<int>> testPrimitive31TempList1;
		testPrimitive31TempList1.push_back(testPrimitive04);
		testPrimitive31TempList1.push_back(testPrimitive04);
		(*testPrimitive31TempList1.rbegin()).insert(42);
		std::list<std::set<int>> testPrimitive31TempList2;
		std::set<int> testPrimitive31TempSet1;
		testPrimitive31TempSet1.insert(1234);
		testPrimitive31TempSet1.insert(5678);
		testPrimitive31TempList2.push_back(testPrimitive31TempSet1);
		testPrimitive31.push_back(testPrimitive31TempList1);
		testPrimitive31.push_back(testPrimitive31TempList2);
		std::multimap<std::string, int> testPrimitive32TempMultiMap1;
		testPrimitive32TempMultiMap1.insert(std::pair<std::string, int>("One", 1));
		testPrimitive32TempMultiMap1.insert(std::pair<std::string, int>("Two", 2));
		testPrimitive32TempMultiMap1.insert(std::pair<std::string, int>("Three", 3));
		testPrimitive32TempMultiMap1.insert(std::pair<std::string, int>("Four", 4));
		testPrimitive32TempMultiMap1.insert(std::pair<std::string, int>("Five", 5));
		std::multimap<std::string, int> testPrimitive32TempMultiMap2;
		testPrimitive32TempMultiMap2.insert(std::pair<std::string, int>("Meaning of Life", 42));
		testPrimitive32TempMultiMap2.insert(std::pair<std::string, int>("Number between 1 and 10", 7));
		testPrimitive32.insert(std::pair<int, std::multimap<std::string, int>>(1234, testPrimitive32TempMultiMap1));
		testPrimitive32.insert(std::pair<int, std::multimap<std::string, int>>(5678, testPrimitive32TempMultiMap2));
		testPrimitive33 = "12345";
		testPrimitive34 = L"12345";

		std::vector<int> vectorElementInt1; vectorElementInt1.push_back(1);
		std::vector<int> vectorElementInt2; vectorElementInt2.push_back(2);
		std::vector<int> vectorElementInt3; vectorElementInt3.push_back(3);
		std::vector<int> vectorElementInt4; vectorElementInt4.push_back(4);
		std::vector<int> vectorElementInt5; vectorElementInt5.push_back(5);
		testPrimitive61.push_back(vectorElementInt1); testPrimitive61.push_back(vectorElementInt2); testPrimitive61.push_back(vectorElementInt3); testPrimitive61.push_back(vectorElementInt4); testPrimitive61.push_back(vectorElementInt5);
		testPrimitive62.push_back(vectorElementInt1); testPrimitive62.push_back(vectorElementInt2); testPrimitive62.push_back(vectorElementInt3); testPrimitive62.push_back(vectorElementInt4); testPrimitive62.push_back(vectorElementInt5);
		testPrimitive63.push_back(vectorElementInt1); testPrimitive63.push_back(vectorElementInt2); testPrimitive63.push_back(vectorElementInt3); testPrimitive63.push_back(vectorElementInt4); testPrimitive63.push_back(vectorElementInt5);
		testPrimitive64.insert(vectorElementInt1); testPrimitive64.insert(vectorElementInt2); testPrimitive64.insert(vectorElementInt3); testPrimitive64.insert(vectorElementInt4); testPrimitive64.insert(vectorElementInt5);
		testPrimitive65.insert(vectorElementInt1); testPrimitive65.insert(vectorElementInt2); testPrimitive65.insert(vectorElementInt3); testPrimitive65.insert(vectorElementInt4); testPrimitive65.insert(vectorElementInt5); testPrimitive65.insert(vectorElementInt1); testPrimitive65.insert(vectorElementInt2); testPrimitive65.insert(vectorElementInt3); testPrimitive65.insert(vectorElementInt4); testPrimitive65.insert(vectorElementInt5);
		testPrimitive66.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive66.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive66.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive66.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive66.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5));
		testPrimitive67.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive67.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive67.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive67.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive67.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5)); testPrimitive67.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive67.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive67.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive67.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive67.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5));
		testPrimitive68.push(vectorElementInt5); testPrimitive68.push(vectorElementInt4); testPrimitive68.push(vectorElementInt3); testPrimitive68.push(vectorElementInt2); testPrimitive68.push(vectorElementInt1);
		testPrimitive69.push(vectorElementInt1); testPrimitive69.push(vectorElementInt2); testPrimitive69.push(vectorElementInt3); testPrimitive69.push(vectorElementInt4); testPrimitive69.push(vectorElementInt5);
		testPrimitive70.push(vectorElementInt1); testPrimitive70.push(vectorElementInt2); testPrimitive70.push(vectorElementInt3); testPrimitive70.push(vectorElementInt4); testPrimitive70.push(vectorElementInt5);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		testPrimitive80[0] = vectorElementInt1; testPrimitive80[1] = vectorElementInt2; testPrimitive80[2] = vectorElementInt3; testPrimitive80[3] = vectorElementInt4; testPrimitive80[4] = vectorElementInt5; testPrimitive80[5] = vectorElementInt1; testPrimitive80[6] = vectorElementInt2; testPrimitive80[7] = vectorElementInt3; testPrimitive80[8] = vectorElementInt4; testPrimitive80[9] = vectorElementInt5;
		testPrimitive81.push_front(vectorElementInt5); testPrimitive81.push_front(vectorElementInt4); testPrimitive81.push_front(vectorElementInt3); testPrimitive81.push_front(vectorElementInt2); testPrimitive81.push_front(vectorElementInt1);
		testPrimitive82.insert(vectorElementInt1); testPrimitive82.insert(vectorElementInt2); testPrimitive82.insert(vectorElementInt3); testPrimitive82.insert(vectorElementInt4); testPrimitive82.insert(vectorElementInt5);
		testPrimitive83.insert(vectorElementInt1); testPrimitive83.insert(vectorElementInt2); testPrimitive83.insert(vectorElementInt3); testPrimitive83.insert(vectorElementInt4); testPrimitive83.insert(vectorElementInt5); testPrimitive83.insert(vectorElementInt1); testPrimitive83.insert(vectorElementInt2); testPrimitive83.insert(vectorElementInt3); testPrimitive83.insert(vectorElementInt4); testPrimitive83.insert(vectorElementInt5);
		testPrimitive84.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive84.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive84.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive84.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive84.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5));
		testPrimitive85.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive85.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive85.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive85.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive85.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5)); testPrimitive85.insert(std::pair<int, std::vector<int>>(1, vectorElementInt1)); testPrimitive85.insert(std::pair<int, std::vector<int>>(2, vectorElementInt2)); testPrimitive85.insert(std::pair<int, std::vector<int>>(3, vectorElementInt3)); testPrimitive85.insert(std::pair<int, std::vector<int>>(4, vectorElementInt4)); testPrimitive85.insert(std::pair<int, std::vector<int>>(5, vectorElementInt5));
#endif

		testCustomMarshaller01.push_back(1); testCustomMarshaller01.push_back(2); testCustomMarshaller01.push_back(3); testCustomMarshaller01.push_back(4); testCustomMarshaller01.push_back(5);
		testCustomMarshaller02.push_back(1); testCustomMarshaller02.push_back(2); testCustomMarshaller02.push_back(3); testCustomMarshaller02.push_back(4); testCustomMarshaller02.push_back(5);
		testCustomMarshaller03.push_back(1); testCustomMarshaller03.push_back(2); testCustomMarshaller03.push_back(3); testCustomMarshaller03.push_back(4); testCustomMarshaller03.push_back(5);
		testCustomMarshaller04.insert(1); testCustomMarshaller04.insert(2); testCustomMarshaller04.insert(3); testCustomMarshaller04.insert(4); testCustomMarshaller04.insert(5);
		testCustomMarshaller05.insert(1); testCustomMarshaller05.insert(2); testCustomMarshaller05.insert(3); testCustomMarshaller05.insert(4); testCustomMarshaller05.insert(5); testCustomMarshaller05.insert(1); testCustomMarshaller05.insert(2); testCustomMarshaller05.insert(3); testCustomMarshaller05.insert(4); testCustomMarshaller05.insert(5);
		testCustomMarshaller06.insert(std::pair<int, int>(1, 1)); testCustomMarshaller06.insert(std::pair<int, int>(2, 2)); testCustomMarshaller06.insert(std::pair<int, int>(3, 3)); testCustomMarshaller06.insert(std::pair<int, int>(4, 4)); testCustomMarshaller06.insert(std::pair<int, int>(5, 5));
		testCustomMarshaller07.insert(std::pair<int, int>(1, 1)); testCustomMarshaller07.insert(std::pair<int, int>(2, 2)); testCustomMarshaller07.insert(std::pair<int, int>(3, 3)); testCustomMarshaller07.insert(std::pair<int, int>(4, 4)); testCustomMarshaller07.insert(std::pair<int, int>(5, 5)); testCustomMarshaller07.insert(std::pair<int, int>(1, 1)); testCustomMarshaller07.insert(std::pair<int, int>(2, 2)); testCustomMarshaller07.insert(std::pair<int, int>(3, 3)); testCustomMarshaller07.insert(std::pair<int, int>(4, 4)); testCustomMarshaller07.insert(std::pair<int, int>(5, 5));
		testCustomMarshaller08.push(5); testCustomMarshaller08.push(4); testCustomMarshaller08.push(3); testCustomMarshaller08.push(2); testCustomMarshaller08.push(1);
		testCustomMarshaller09.push(1); testCustomMarshaller09.push(2); testCustomMarshaller09.push(3); testCustomMarshaller09.push(4); testCustomMarshaller09.push(5);
		testCustomMarshaller10.push(1); testCustomMarshaller10.push(2); testCustomMarshaller10.push(3); testCustomMarshaller10.push(4); testCustomMarshaller10.push(5);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		testCustomMarshaller20[0] = 1; testCustomMarshaller20[1] = 2; testCustomMarshaller20[2] = 3; testCustomMarshaller20[3] = 4; testCustomMarshaller20[4] = 5; testCustomMarshaller20[5] = 1; testCustomMarshaller20[6] = 2; testCustomMarshaller20[7] = 3; testCustomMarshaller20[8] = 4; testCustomMarshaller20[9] = 5;
		testCustomMarshaller21.push_front(5); testCustomMarshaller21.push_front(4); testCustomMarshaller21.push_front(3); testCustomMarshaller21.push_front(2); testCustomMarshaller21.push_front(1);
		testCustomMarshaller22.insert(1); testCustomMarshaller22.insert(2); testCustomMarshaller22.insert(3); testCustomMarshaller22.insert(4); testCustomMarshaller22.insert(5);
		testCustomMarshaller23.insert(1); testCustomMarshaller23.insert(2); testCustomMarshaller23.insert(3); testCustomMarshaller23.insert(4); testCustomMarshaller23.insert(5); testCustomMarshaller23.insert(1); testCustomMarshaller23.insert(2); testCustomMarshaller23.insert(3); testCustomMarshaller23.insert(4); testCustomMarshaller23.insert(5);
		testCustomMarshaller24.insert(std::pair<int, int>(1, 1)); testCustomMarshaller24.insert(std::pair<int, int>(2, 2)); testCustomMarshaller24.insert(std::pair<int, int>(3, 3)); testCustomMarshaller24.insert(std::pair<int, int>(4, 4)); testCustomMarshaller24.insert(std::pair<int, int>(5, 5));
		testCustomMarshaller25.insert(std::pair<int, int>(1, 1)); testCustomMarshaller25.insert(std::pair<int, int>(2, 2)); testCustomMarshaller25.insert(std::pair<int, int>(3, 3)); testCustomMarshaller25.insert(std::pair<int, int>(4, 4)); testCustomMarshaller25.insert(std::pair<int, int>(5, 5)); testCustomMarshaller25.insert(std::pair<int, int>(1, 1)); testCustomMarshaller25.insert(std::pair<int, int>(2, 2)); testCustomMarshaller25.insert(std::pair<int, int>(3, 3)); testCustomMarshaller25.insert(std::pair<int, int>(4, 4)); testCustomMarshaller25.insert(std::pair<int, int>(5, 5));
#endif
		std::list<std::set<CustomMarshalObject>> testCustomMarshaller31TempList1;
		testCustomMarshaller31TempList1.push_back(testCustomMarshaller04);
		testCustomMarshaller31TempList1.push_back(testCustomMarshaller04);
		(*testCustomMarshaller31TempList1.rbegin()).insert(42);
		std::list<std::set<CustomMarshalObject>> testCustomMarshaller31TempList2;
		std::set<CustomMarshalObject> testCustomMarshaller31TempSet1;
		testCustomMarshaller31TempSet1.insert(1234);
		testCustomMarshaller31TempSet1.insert(5678);
		testCustomMarshaller31TempList2.push_back(testCustomMarshaller31TempSet1);
		testCustomMarshaller31.push_back(testCustomMarshaller31TempList1);
		testCustomMarshaller31.push_back(testCustomMarshaller31TempList2);
		std::multimap<std::string, CustomMarshalObject> testCustomMarshaller32TempMultiMap1;
		testCustomMarshaller32TempMultiMap1.insert(std::pair<std::string, CustomMarshalObject>("One", 1));
		testCustomMarshaller32TempMultiMap1.insert(std::pair<std::string, CustomMarshalObject>("Two", 2));
		testCustomMarshaller32TempMultiMap1.insert(std::pair<std::string, CustomMarshalObject>("Three", 3));
		testCustomMarshaller32TempMultiMap1.insert(std::pair<std::string, CustomMarshalObject>("Four", 4));
		testCustomMarshaller32TempMultiMap1.insert(std::pair<std::string, CustomMarshalObject>("Five", 5));
		std::multimap<std::string, CustomMarshalObject> testCustomMarshaller32TempMultiMap2;
		testCustomMarshaller32TempMultiMap2.insert(std::pair<std::string, CustomMarshalObject>("Meaning of Life", 42));
		testCustomMarshaller32TempMultiMap2.insert(std::pair<std::string, CustomMarshalObject>("Number between 1 and 10", 7));
		testCustomMarshaller32.insert(std::pair<int, std::multimap<std::string, CustomMarshalObject>>(1234, testCustomMarshaller32TempMultiMap1));
		testCustomMarshaller32.insert(std::pair<int, std::multimap<std::string, CustomMarshalObject>>(5678, testCustomMarshaller32TempMultiMap2));

		std::vector<CustomMarshalObject> vectorElementCustomMarshaller1; vectorElementCustomMarshaller1.push_back(1);
		std::vector<CustomMarshalObject> vectorElementCustomMarshaller2; vectorElementCustomMarshaller2.push_back(2);
		std::vector<CustomMarshalObject> vectorElementCustomMarshaller3; vectorElementCustomMarshaller3.push_back(3);
		std::vector<CustomMarshalObject> vectorElementCustomMarshaller4; vectorElementCustomMarshaller4.push_back(4);
		std::vector<CustomMarshalObject> vectorElementCustomMarshaller5; vectorElementCustomMarshaller5.push_back(5);
		testCustomMarshaller61.push_back(vectorElementCustomMarshaller1); testCustomMarshaller61.push_back(vectorElementCustomMarshaller2); testCustomMarshaller61.push_back(vectorElementCustomMarshaller3); testCustomMarshaller61.push_back(vectorElementCustomMarshaller4); testCustomMarshaller61.push_back(vectorElementCustomMarshaller5);
		testCustomMarshaller62.push_back(vectorElementCustomMarshaller1); testCustomMarshaller62.push_back(vectorElementCustomMarshaller2); testCustomMarshaller62.push_back(vectorElementCustomMarshaller3); testCustomMarshaller62.push_back(vectorElementCustomMarshaller4); testCustomMarshaller62.push_back(vectorElementCustomMarshaller5);
		testCustomMarshaller63.push_back(vectorElementCustomMarshaller1); testCustomMarshaller63.push_back(vectorElementCustomMarshaller2); testCustomMarshaller63.push_back(vectorElementCustomMarshaller3); testCustomMarshaller63.push_back(vectorElementCustomMarshaller4); testCustomMarshaller63.push_back(vectorElementCustomMarshaller5);
		testCustomMarshaller64.insert(vectorElementCustomMarshaller1); testCustomMarshaller64.insert(vectorElementCustomMarshaller2); testCustomMarshaller64.insert(vectorElementCustomMarshaller3); testCustomMarshaller64.insert(vectorElementCustomMarshaller4); testCustomMarshaller64.insert(vectorElementCustomMarshaller5);
		testCustomMarshaller65.insert(vectorElementCustomMarshaller1); testCustomMarshaller65.insert(vectorElementCustomMarshaller2); testCustomMarshaller65.insert(vectorElementCustomMarshaller3); testCustomMarshaller65.insert(vectorElementCustomMarshaller4); testCustomMarshaller65.insert(vectorElementCustomMarshaller5); testCustomMarshaller65.insert(vectorElementCustomMarshaller1); testCustomMarshaller65.insert(vectorElementCustomMarshaller2); testCustomMarshaller65.insert(vectorElementCustomMarshaller3); testCustomMarshaller65.insert(vectorElementCustomMarshaller4); testCustomMarshaller65.insert(vectorElementCustomMarshaller5);
		testCustomMarshaller66.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller66.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller66.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller66.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller66.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5));
		testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller67.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5));
		testCustomMarshaller68.push(vectorElementCustomMarshaller5); testCustomMarshaller68.push(vectorElementCustomMarshaller4); testCustomMarshaller68.push(vectorElementCustomMarshaller3); testCustomMarshaller68.push(vectorElementCustomMarshaller2); testCustomMarshaller68.push(vectorElementCustomMarshaller1);
		testCustomMarshaller69.push(vectorElementCustomMarshaller1); testCustomMarshaller69.push(vectorElementCustomMarshaller2); testCustomMarshaller69.push(vectorElementCustomMarshaller3); testCustomMarshaller69.push(vectorElementCustomMarshaller4); testCustomMarshaller69.push(vectorElementCustomMarshaller5);
		testCustomMarshaller70.push(vectorElementCustomMarshaller1); testCustomMarshaller70.push(vectorElementCustomMarshaller2); testCustomMarshaller70.push(vectorElementCustomMarshaller3); testCustomMarshaller70.push(vectorElementCustomMarshaller4); testCustomMarshaller70.push(vectorElementCustomMarshaller5);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		testCustomMarshaller80[0] = vectorElementCustomMarshaller1; testCustomMarshaller80[1] = vectorElementCustomMarshaller2; testCustomMarshaller80[2] = vectorElementCustomMarshaller3; testCustomMarshaller80[3] = vectorElementCustomMarshaller4; testCustomMarshaller80[4] = vectorElementCustomMarshaller5; testCustomMarshaller80[5] = vectorElementCustomMarshaller1; testCustomMarshaller80[6] = vectorElementCustomMarshaller2; testCustomMarshaller80[7] = vectorElementCustomMarshaller3; testCustomMarshaller80[8] = vectorElementCustomMarshaller4; testCustomMarshaller80[9] = vectorElementCustomMarshaller5;
		testCustomMarshaller81.push_front(vectorElementCustomMarshaller5); testCustomMarshaller81.push_front(vectorElementCustomMarshaller4); testCustomMarshaller81.push_front(vectorElementCustomMarshaller3); testCustomMarshaller81.push_front(vectorElementCustomMarshaller2); testCustomMarshaller81.push_front(vectorElementCustomMarshaller1);
		testCustomMarshaller82.insert(vectorElementCustomMarshaller1); testCustomMarshaller82.insert(vectorElementCustomMarshaller2); testCustomMarshaller82.insert(vectorElementCustomMarshaller3); testCustomMarshaller82.insert(vectorElementCustomMarshaller4); testCustomMarshaller82.insert(vectorElementCustomMarshaller5);
		testCustomMarshaller83.insert(vectorElementCustomMarshaller1); testCustomMarshaller83.insert(vectorElementCustomMarshaller2); testCustomMarshaller83.insert(vectorElementCustomMarshaller3); testCustomMarshaller83.insert(vectorElementCustomMarshaller4); testCustomMarshaller83.insert(vectorElementCustomMarshaller5); testCustomMarshaller83.insert(vectorElementCustomMarshaller1); testCustomMarshaller83.insert(vectorElementCustomMarshaller2); testCustomMarshaller83.insert(vectorElementCustomMarshaller3); testCustomMarshaller83.insert(vectorElementCustomMarshaller4); testCustomMarshaller83.insert(vectorElementCustomMarshaller5);
		testCustomMarshaller84.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller84.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller84.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller84.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller84.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5));
		testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(1, vectorElementCustomMarshaller1)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(2, vectorElementCustomMarshaller2)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(3, vectorElementCustomMarshaller3)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(4, vectorElementCustomMarshaller4)); testCustomMarshaller85.insert(std::pair<int, std::vector<CustomMarshalObject>>(5, vectorElementCustomMarshaller5));
#endif
	}

	//In tests
	std::vector<int> testPrimitive01;
	std::list<int> testPrimitive02;
	std::deque<int> testPrimitive03;
	std::set<int> testPrimitive04;
	std::multiset<int> testPrimitive05;
	std::map<int, int> testPrimitive06;
	std::multimap<int, int> testPrimitive07;
	std::stack<int> testPrimitive08;
	std::queue<int> testPrimitive09;
	std::priority_queue<int> testPrimitive10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<int, 10> testPrimitive20;
	std::forward_list<int> testPrimitive21;
	std::unordered_set<int> testPrimitive22;
	std::unordered_multiset<int> testPrimitive23;
	std::unordered_map<int, int> testPrimitive24;
	std::unordered_multimap<int, int> testPrimitive25;
#endif
	int testPrimitive30;
	std::vector<std::list<std::set<int>>> testPrimitive31;
	std::map<int, std::multimap<std::string, int>> testPrimitive32;
	std::string testPrimitive33;
	std::wstring testPrimitive34;
	std::vector<std::vector<int>> testPrimitive61;
	std::list<std::vector<int>> testPrimitive62;
	std::deque<std::vector<int>> testPrimitive63;
	std::set<std::vector<int>> testPrimitive64;
	std::multiset<std::vector<int>> testPrimitive65;
	std::map<int, std::vector<int>> testPrimitive66;
	std::multimap<int, std::vector<int>> testPrimitive67;
	std::stack<std::vector<int>> testPrimitive68;
	std::queue<std::vector<int>> testPrimitive69;
	std::priority_queue<std::vector<int>> testPrimitive70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<int>, 10> testPrimitive80;
	std::forward_list<std::vector<int>> testPrimitive81;
	std::unordered_set<std::vector<int>, VectorHasher<int>> testPrimitive82;
	std::unordered_multiset<std::vector<int>, VectorHasher<int>> testPrimitive83;
	std::unordered_map<int, std::vector<int>> testPrimitive84;
	std::unordered_multimap<int, std::vector<int>> testPrimitive85;
#endif

	std::vector<CustomMarshalObject> testCustomMarshaller01;
	std::list<CustomMarshalObject> testCustomMarshaller02;
	std::deque<CustomMarshalObject> testCustomMarshaller03;
	std::set<CustomMarshalObject> testCustomMarshaller04;
	std::multiset<CustomMarshalObject> testCustomMarshaller05;
	std::map<int, CustomMarshalObject> testCustomMarshaller06;
	std::multimap<int, CustomMarshalObject> testCustomMarshaller07;
	std::stack<CustomMarshalObject> testCustomMarshaller08;
	std::queue<CustomMarshalObject> testCustomMarshaller09;
	std::priority_queue<CustomMarshalObject> testCustomMarshaller10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<CustomMarshalObjectWithDefaultConstructor, 10> testCustomMarshaller20;
	std::forward_list<CustomMarshalObject> testCustomMarshaller21;
	std::unordered_set<CustomMarshalObject> testCustomMarshaller22;
	std::unordered_multiset<CustomMarshalObject> testCustomMarshaller23;
	std::unordered_map<int, CustomMarshalObject> testCustomMarshaller24;
	std::unordered_multimap<int, CustomMarshalObject> testCustomMarshaller25;
#endif
	CustomMarshalObject testCustomMarshaller30;
	std::vector<std::list<std::set<CustomMarshalObject>>> testCustomMarshaller31;
	std::map<int, std::multimap<std::string, CustomMarshalObject>> testCustomMarshaller32;
	std::vector<std::vector<CustomMarshalObject>> testCustomMarshaller61;
	std::list<std::vector<CustomMarshalObject>> testCustomMarshaller62;
	std::deque<std::vector<CustomMarshalObject>> testCustomMarshaller63;
	std::set<std::vector<CustomMarshalObject>> testCustomMarshaller64;
	std::multiset<std::vector<CustomMarshalObject>> testCustomMarshaller65;
	std::map<int, std::vector<CustomMarshalObject>> testCustomMarshaller66;
	std::multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller67;
	std::stack<std::vector<CustomMarshalObject>> testCustomMarshaller68;
	std::queue<std::vector<CustomMarshalObject>> testCustomMarshaller69;
	std::priority_queue<std::vector<CustomMarshalObject>> testCustomMarshaller70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<CustomMarshalObject>, 10> testCustomMarshaller80 = {};
	std::forward_list<std::vector<CustomMarshalObject>> testCustomMarshaller81;
	std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller82;
	std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller83;
	std::unordered_map<int, std::vector<CustomMarshalObject>> testCustomMarshaller84;
	std::unordered_multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller85;
#endif
};

#endif
