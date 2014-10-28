#ifndef __MARSHALTEST_H__
#define __MARSHALTEST_H__
#include "IMarshalTest.h"
#include "MarshalTestData.h"
#include "CompareTestData.h"

class MarshalTest :public IMarshalTest
{
public:
	//In tests
	bool InTestPrimitive01(const std::vector<int>& arg) const { return CompareTestData(arg, testData.testPrimitive01); }
	bool InTestPrimitive02(const std::list<int>& arg) const { return CompareTestData(arg, testData.testPrimitive02); }
	bool InTestPrimitive03(const std::deque<int>& arg) const { return CompareTestData(arg, testData.testPrimitive03); }
	bool InTestPrimitive04(const std::set<int>& arg) const { return CompareTestData(arg, testData.testPrimitive04); }
	bool InTestPrimitive05(const std::multiset<int>& arg) const { return CompareTestData(arg, testData.testPrimitive05); }
	bool InTestPrimitive06(const std::map<int, int>& arg) const { return CompareTestData(arg, testData.testPrimitive06); }
	bool InTestPrimitive07(const std::multimap<int, int>& arg) const { return CompareTestData(arg, testData.testPrimitive07); }
	bool InTestPrimitive08(const std::stack<int>& arg) const { return CompareTestData(arg, testData.testPrimitive08); }
	bool InTestPrimitive09(const std::queue<int>& arg) const { return CompareTestData(arg, testData.testPrimitive09); }
	bool InTestPrimitive10(const std::priority_queue<int>& arg) const { return CompareTestData(arg, testData.testPrimitive10); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InTestPrimitive20(const std::array<int, 10>& arg) const { return CompareTestData(arg, testData.testPrimitive20); }
	bool InTestPrimitive21(const std::forward_list<int>& arg) const { return CompareTestData(arg, testData.testPrimitive21); }
	bool InTestPrimitive22(const std::unordered_set<int>& arg) const { return CompareTestData(arg, testData.testPrimitive22); }
	bool InTestPrimitive23(const std::unordered_multiset<int>& arg) const { return CompareTestData(arg, testData.testPrimitive23); }
	bool InTestPrimitive24(const std::unordered_map<int, int>& arg) const { return CompareTestData(arg, testData.testPrimitive24); }
	bool InTestPrimitive25(const std::unordered_multimap<int, int>& arg) const { return CompareTestData(arg, testData.testPrimitive25); }
#endif
	bool InTestPrimitive30(int arg) const { return CompareTestData(arg, testData.testPrimitive30); }
	bool InTestPrimitive31(const std::vector<std::list<std::set<int>>>& arg) const { return CompareTestData(arg, testData.testPrimitive31); }
	bool InTestPrimitive32(const std::map<int, std::multimap<std::string, int>>& arg) const { return CompareTestData(arg, testData.testPrimitive32); }
	bool InTestPrimitive33(const std::string& arg) const { return CompareTestData(arg, testData.testPrimitive33); }
	bool InTestPrimitive34(const std::wstring& arg) const { return CompareTestData(arg, testData.testPrimitive34); }
	bool InTestPrimitive61(const std::vector<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive61); }
	bool InTestPrimitive62(const std::list<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive62); }
	bool InTestPrimitive63(const std::deque<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive63); }
	bool InTestPrimitive64(const std::set<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive64); }
	bool InTestPrimitive65(const std::multiset<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive65); }
	bool InTestPrimitive66(const std::map<int, std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive66); }
	bool InTestPrimitive67(const std::multimap<int, std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive67); }
	bool InTestPrimitive68(const std::stack<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive68); }
	bool InTestPrimitive69(const std::queue<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive69); }
	bool InTestPrimitive70(const std::priority_queue<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive70); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InTestPrimitive80(const std::array<std::vector<int>, 10>& arg) const { return CompareTestData(arg, testData.testPrimitive80); }
	bool InTestPrimitive81(const std::forward_list<std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive81); }
	bool InTestPrimitive82(const std::unordered_set<std::vector<int>, VectorHasher<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive82); }
	bool InTestPrimitive83(const std::unordered_multiset<std::vector<int>, VectorHasher<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive83); }
	bool InTestPrimitive84(const std::unordered_map<int, std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive84); }
	bool InTestPrimitive85(const std::unordered_multimap<int, std::vector<int>>& arg) const { return CompareTestData(arg, testData.testPrimitive85); }
#endif

	bool InTestCustomMarshaller01(const std::vector<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller01); }
	bool InTestCustomMarshaller02(const std::list<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller02); }
	bool InTestCustomMarshaller03(const std::deque<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller03); }
	bool InTestCustomMarshaller04(const std::set<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller04); }
	bool InTestCustomMarshaller05(const std::multiset<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller05); }
	bool InTestCustomMarshaller06(const std::map<int, CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller06); }
	bool InTestCustomMarshaller07(const std::multimap<int, CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller07); }
	bool InTestCustomMarshaller08(const std::stack<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller08); }
	bool InTestCustomMarshaller09(const std::queue<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller09); }
	bool InTestCustomMarshaller10(const std::priority_queue<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller10); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InTestCustomMarshaller20(const std::array<CustomMarshalObjectWithDefaultConstructor, 10>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller20); }
	bool InTestCustomMarshaller21(const std::forward_list<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller21); }
	bool InTestCustomMarshaller22(const std::unordered_set<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller22); }
	bool InTestCustomMarshaller23(const std::unordered_multiset<CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller23); }
	bool InTestCustomMarshaller24(const std::unordered_map<int, CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller24); }
	bool InTestCustomMarshaller25(const std::unordered_multimap<int, CustomMarshalObject>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller25); }
#endif
	bool InTestCustomMarshaller30(const CustomMarshalObject& arg) const { return CompareTestData(arg, testData.testCustomMarshaller30); }
	bool InTestCustomMarshaller31(const std::vector<std::list<std::set<CustomMarshalObject>>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller31); }
	bool InTestCustomMarshaller32(const std::map<int, std::multimap<std::string, CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller32); }
	bool InTestCustomMarshaller61(const std::vector<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller61); }
	bool InTestCustomMarshaller62(const std::list<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller62); }
	bool InTestCustomMarshaller63(const std::deque<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller63); }
	bool InTestCustomMarshaller64(const std::set<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller64); }
	bool InTestCustomMarshaller65(const std::multiset<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller65); }
	bool InTestCustomMarshaller66(const std::map<int, std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller66); }
	bool InTestCustomMarshaller67(const std::multimap<int, std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller67); }
	bool InTestCustomMarshaller68(const std::stack<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller68); }
	bool InTestCustomMarshaller69(const std::queue<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller69); }
	bool InTestCustomMarshaller70(const std::priority_queue<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller70); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InTestCustomMarshaller80(const std::array<std::vector<CustomMarshalObject>, 10>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller80); }
	bool InTestCustomMarshaller81(const std::forward_list<std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller81); }
	bool InTestCustomMarshaller82(const std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller82); }
	bool InTestCustomMarshaller83(const std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller83); }
	bool InTestCustomMarshaller84(const std::unordered_map<int, std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller84); }
	bool InTestCustomMarshaller85(const std::unordered_multimap<int, std::vector<CustomMarshalObject>>& arg) const { return CompareTestData(arg, testData.testCustomMarshaller85); }
#endif

	//InOut tests
	bool InOutTestPrimitive01(std::vector<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive01); arg = testData.testPrimitive01; return result; }
	bool InOutTestPrimitive02(std::list<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive02); arg = testData.testPrimitive02; return result; }
	bool InOutTestPrimitive03(std::deque<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive03); arg = testData.testPrimitive03; return result; }
	bool InOutTestPrimitive04(std::set<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive04); arg = testData.testPrimitive04; return result; }
	bool InOutTestPrimitive05(std::multiset<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive05); arg = testData.testPrimitive05; return result; }
	bool InOutTestPrimitive06(std::map<int, int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive06); arg = testData.testPrimitive06; return result; }
	bool InOutTestPrimitive07(std::multimap<int, int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive07); arg = testData.testPrimitive07; return result; }
	bool InOutTestPrimitive08(std::stack<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive08); arg = testData.testPrimitive08; return result; }
	bool InOutTestPrimitive09(std::queue<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive09); arg = testData.testPrimitive09; return result; }
	bool InOutTestPrimitive10(std::priority_queue<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive10); arg = testData.testPrimitive10; return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InOutTestPrimitive20(std::array<int, 10>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive20); arg = testData.testPrimitive20; return result; }
	bool InOutTestPrimitive21(std::forward_list<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive21); arg = testData.testPrimitive21; return result; }
	bool InOutTestPrimitive22(std::unordered_set<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive22); arg = testData.testPrimitive22; return result; }
	bool InOutTestPrimitive23(std::unordered_multiset<int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive23); arg = testData.testPrimitive23; return result; }
	bool InOutTestPrimitive24(std::unordered_map<int, int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive24); arg = testData.testPrimitive24; return result; }
	bool InOutTestPrimitive25(std::unordered_multimap<int, int>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive25); arg = testData.testPrimitive25; return result; }
#endif
	bool InOutTestPrimitive30(int arg) const { bool result = CompareTestData(arg, testData.testPrimitive30); arg = testData.testPrimitive30; return result; }
	bool InOutTestPrimitive31(std::vector<std::list<std::set<int>>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive31); arg = testData.testPrimitive31; return result; }
	bool InOutTestPrimitive32(std::map<int, std::multimap<std::string, int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive32); arg = testData.testPrimitive32; return result; }
	bool InOutTestPrimitive33(std::string& arg) const { bool result = CompareTestData(arg, testData.testPrimitive33); arg = testData.testPrimitive33; return result; }
	bool InOutTestPrimitive34(std::wstring& arg) const { bool result = CompareTestData(arg, testData.testPrimitive34); arg = testData.testPrimitive34; return result; }
	bool InOutTestPrimitive61(std::vector<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive61); arg = testData.testPrimitive61; return result; }
	bool InOutTestPrimitive62(std::list<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive62); arg = testData.testPrimitive62; return result; }
	bool InOutTestPrimitive63(std::deque<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive63); arg = testData.testPrimitive63; return result; }
	bool InOutTestPrimitive64(std::set<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive64); arg = testData.testPrimitive64; return result; }
	bool InOutTestPrimitive65(std::multiset<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive65); arg = testData.testPrimitive65; return result; }
	bool InOutTestPrimitive66(std::map<int, std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive66); arg = testData.testPrimitive66; return result; }
	bool InOutTestPrimitive67(std::multimap<int, std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive67); arg = testData.testPrimitive67; return result; }
	bool InOutTestPrimitive68(std::stack<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive68); arg = testData.testPrimitive68; return result; }
	bool InOutTestPrimitive69(std::queue<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive69); arg = testData.testPrimitive69; return result; }
	bool InOutTestPrimitive70(std::priority_queue<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive70); arg = testData.testPrimitive70; return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InOutTestPrimitive80(std::array<std::vector<int>, 10>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive80); arg = testData.testPrimitive80; return result; }
	bool InOutTestPrimitive81(std::forward_list<std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive81); arg = testData.testPrimitive81; return result; }
	bool InOutTestPrimitive82(std::unordered_set<std::vector<int>, VectorHasher<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive82); arg = testData.testPrimitive82; return result; }
	bool InOutTestPrimitive83(std::unordered_multiset<std::vector<int>, VectorHasher<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive83); arg = testData.testPrimitive83; return result; }
	bool InOutTestPrimitive84(std::unordered_map<int, std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive84); arg = testData.testPrimitive84; return result; }
	bool InOutTestPrimitive85(std::unordered_multimap<int, std::vector<int>>& arg) const { bool result = CompareTestData(arg, testData.testPrimitive85); arg = testData.testPrimitive85; return result; }
#endif

	bool InOutTestCustomMarshaller01(std::vector<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller01); arg = testData.testCustomMarshaller01; return result; }
	bool InOutTestCustomMarshaller02(std::list<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller02); arg = testData.testCustomMarshaller02; return result; }
	bool InOutTestCustomMarshaller03(std::deque<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller03); arg = testData.testCustomMarshaller03; return result; }
	bool InOutTestCustomMarshaller04(std::set<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller04); arg = testData.testCustomMarshaller04; return result; }
	bool InOutTestCustomMarshaller05(std::multiset<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller05); arg = testData.testCustomMarshaller05; return result; }
	bool InOutTestCustomMarshaller06(std::map<int, CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller06); arg = testData.testCustomMarshaller06; return result; }
	bool InOutTestCustomMarshaller07(std::multimap<int, CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller07); arg = testData.testCustomMarshaller07; return result; }
	bool InOutTestCustomMarshaller08(std::stack<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller08); arg = testData.testCustomMarshaller08; return result; }
	bool InOutTestCustomMarshaller09(std::queue<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller09); arg = testData.testCustomMarshaller09; return result; }
	bool InOutTestCustomMarshaller10(std::priority_queue<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller10); arg = testData.testCustomMarshaller10; return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InOutTestCustomMarshaller20(std::array<CustomMarshalObjectWithDefaultConstructor, 10>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller20); arg = testData.testCustomMarshaller20; return result; }
	bool InOutTestCustomMarshaller21(std::forward_list<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller21); arg = testData.testCustomMarshaller21; return result; }
	bool InOutTestCustomMarshaller22(std::unordered_set<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller22); arg = testData.testCustomMarshaller22; return result; }
	bool InOutTestCustomMarshaller23(std::unordered_multiset<CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller23); arg = testData.testCustomMarshaller23; return result; }
	bool InOutTestCustomMarshaller24(std::unordered_map<int, CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller24); arg = testData.testCustomMarshaller24; return result; }
	bool InOutTestCustomMarshaller25(std::unordered_multimap<int, CustomMarshalObject>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller25); arg = testData.testCustomMarshaller25; return result; }
#endif
	bool InOutTestCustomMarshaller30(CustomMarshalObject& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller30); arg = testData.testCustomMarshaller30; return result; }
	bool InOutTestCustomMarshaller31(std::vector<std::list<std::set<CustomMarshalObject>>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller31); arg = testData.testCustomMarshaller31; return result; }
	bool InOutTestCustomMarshaller32(std::map<int, std::multimap<std::string, CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller32); arg = testData.testCustomMarshaller32; return result; }
	bool InOutTestCustomMarshaller61(std::vector<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller61); arg = testData.testCustomMarshaller61; return result; }
	bool InOutTestCustomMarshaller62(std::list<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller62); arg = testData.testCustomMarshaller62; return result; }
	bool InOutTestCustomMarshaller63(std::deque<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller63); arg = testData.testCustomMarshaller63; return result; }
	bool InOutTestCustomMarshaller64(std::set<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller64); arg = testData.testCustomMarshaller64; return result; }
	bool InOutTestCustomMarshaller65(std::multiset<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller65); arg = testData.testCustomMarshaller65; return result; }
	bool InOutTestCustomMarshaller66(std::map<int, std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller66); arg = testData.testCustomMarshaller66; return result; }
	bool InOutTestCustomMarshaller67(std::multimap<int, std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller67); arg = testData.testCustomMarshaller67; return result; }
	bool InOutTestCustomMarshaller68(std::stack<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller68); arg = testData.testCustomMarshaller68; return result; }
	bool InOutTestCustomMarshaller69(std::queue<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller69); arg = testData.testCustomMarshaller69; return result; }
	bool InOutTestCustomMarshaller70(std::priority_queue<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller70); arg = testData.testCustomMarshaller70; return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	bool InOutTestCustomMarshaller80(std::array<std::vector<CustomMarshalObject>, 10>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller80); arg = testData.testCustomMarshaller80; return result; }
	bool InOutTestCustomMarshaller81(std::forward_list<std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller81); arg = testData.testCustomMarshaller81; return result; }
	bool InOutTestCustomMarshaller82(std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller82); arg = testData.testCustomMarshaller82; return result; }
	bool InOutTestCustomMarshaller83(std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller83); arg = testData.testCustomMarshaller83; return result; }
	bool InOutTestCustomMarshaller84(std::unordered_map<int, std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller84); arg = testData.testCustomMarshaller84; return result; }
	bool InOutTestCustomMarshaller85(std::unordered_multimap<int, std::vector<CustomMarshalObject>>& arg) const { bool result = CompareTestData(arg, testData.testCustomMarshaller85); arg = testData.testCustomMarshaller85; return result; }
#endif

	//Out tests
	virtual void OutTestPrimitive01(const Marshal::Out<std::vector<int>>& arg) const { arg = testData.testPrimitive01; }
	virtual void OutTestPrimitive02(const Marshal::Out<std::list<int>>& arg) const { arg = testData.testPrimitive02; }
	virtual void OutTestPrimitive03(const Marshal::Out<std::deque<int>>& arg) const { arg = testData.testPrimitive03; }
	virtual void OutTestPrimitive04(const Marshal::Out<std::set<int>>& arg) const { arg = testData.testPrimitive04; }
	virtual void OutTestPrimitive05(const Marshal::Out<std::multiset<int>>& arg) const { arg = testData.testPrimitive05; }
	virtual void OutTestPrimitive06(const Marshal::Out<std::map<int, int>>& arg) const { arg = testData.testPrimitive06; }
	virtual void OutTestPrimitive07(const Marshal::Out<std::multimap<int, int>>& arg) const { arg = testData.testPrimitive07; }
	virtual void OutTestPrimitive08(const Marshal::Out<std::stack<int>>& arg) const { arg = testData.testPrimitive08; }
	virtual void OutTestPrimitive09(const Marshal::Out<std::queue<int>>& arg) const { arg = testData.testPrimitive09; }
	virtual void OutTestPrimitive10(const Marshal::Out<std::priority_queue<int>>& arg) const { arg = testData.testPrimitive10; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitive20(const Marshal::Out<std::array<int, 10>>& arg) const { arg = testData.testPrimitive20; }
	virtual void OutTestPrimitive21(const Marshal::Out<std::forward_list<int>>& arg) const { arg = testData.testPrimitive21; }
	virtual void OutTestPrimitive22(const Marshal::Out<std::unordered_set<int>>& arg) const { arg = testData.testPrimitive22; }
	virtual void OutTestPrimitive23(const Marshal::Out<std::unordered_multiset<int>>& arg) const { arg = testData.testPrimitive23; }
	virtual void OutTestPrimitive24(const Marshal::Out<std::unordered_map<int, int>>& arg) const { arg = testData.testPrimitive24; }
	virtual void OutTestPrimitive25(const Marshal::Out<std::unordered_multimap<int, int>>& arg) const { arg = testData.testPrimitive25; }
#endif
	virtual void OutTestPrimitive30(const Marshal::Out<int>& arg) const { arg = testData.testPrimitive30; }
	virtual void OutTestPrimitive31(const Marshal::Out<std::vector<std::list<std::set<int>>>>& arg) const { arg = testData.testPrimitive31; }
	virtual void OutTestPrimitive32(const Marshal::Out<std::map<int, std::multimap<std::string, int>>>& arg) const { arg = testData.testPrimitive32; }
	virtual void OutTestPrimitive33(const Marshal::Out<std::string>& arg) const { arg = testData.testPrimitive33; }
	virtual void OutTestPrimitive34(const Marshal::Out<std::wstring>& arg) const { arg = testData.testPrimitive34; }
	virtual void OutTestPrimitive61(const Marshal::Out<std::vector<std::vector<int>>>& arg) const { arg = testData.testPrimitive61; }
	virtual void OutTestPrimitive62(const Marshal::Out<std::list<std::vector<int>>>& arg) const { arg = testData.testPrimitive62; }
	virtual void OutTestPrimitive63(const Marshal::Out<std::deque<std::vector<int>>>& arg) const { arg = testData.testPrimitive63; }
	virtual void OutTestPrimitive64(const Marshal::Out<std::set<std::vector<int>>>& arg) const { arg = testData.testPrimitive64; }
	virtual void OutTestPrimitive65(const Marshal::Out<std::multiset<std::vector<int>>>& arg) const { arg = testData.testPrimitive65; }
	virtual void OutTestPrimitive66(const Marshal::Out<std::map<int, std::vector<int>>>& arg) const { arg = testData.testPrimitive66; }
	virtual void OutTestPrimitive67(const Marshal::Out<std::multimap<int, std::vector<int>>>& arg) const { arg = testData.testPrimitive67; }
	virtual void OutTestPrimitive68(const Marshal::Out<std::stack<std::vector<int>>>& arg) const { arg = testData.testPrimitive68; }
	virtual void OutTestPrimitive69(const Marshal::Out<std::queue<std::vector<int>>>& arg) const { arg = testData.testPrimitive69; }
	virtual void OutTestPrimitive70(const Marshal::Out<std::priority_queue<std::vector<int>>>& arg) const { arg = testData.testPrimitive70; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitive80(const Marshal::Out<std::array<std::vector<int>, 10>>& arg) const { arg = testData.testPrimitive80; }
	virtual void OutTestPrimitive81(const Marshal::Out<std::forward_list<std::vector<int>>>& arg) const { arg = testData.testPrimitive81; }
	virtual void OutTestPrimitive82(const Marshal::Out<std::unordered_set<std::vector<int>, VectorHasher<int>>>& arg) const { arg = testData.testPrimitive82; }
	virtual void OutTestPrimitive83(const Marshal::Out<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& arg) const { arg = testData.testPrimitive83; }
	virtual void OutTestPrimitive84(const Marshal::Out<std::unordered_map<int, std::vector<int>>>& arg) const { arg = testData.testPrimitive84; }
	virtual void OutTestPrimitive85(const Marshal::Out<std::unordered_multimap<int, std::vector<int>>>& arg) const { arg = testData.testPrimitive85; }
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitiveWithMove01(const Marshal::Out<std::vector<int>>& arg) const { arg = std::move(testData.testPrimitive01); }
	virtual void OutTestPrimitiveWithMove02(const Marshal::Out<std::list<int>>& arg) const { arg = std::move(testData.testPrimitive02); }
	virtual void OutTestPrimitiveWithMove03(const Marshal::Out<std::deque<int>>& arg) const { arg = std::move(testData.testPrimitive03); }
	virtual void OutTestPrimitiveWithMove04(const Marshal::Out<std::set<int>>& arg) const { arg = std::move(testData.testPrimitive04); }
	virtual void OutTestPrimitiveWithMove05(const Marshal::Out<std::multiset<int>>& arg) const { arg = std::move(testData.testPrimitive05); }
	virtual void OutTestPrimitiveWithMove06(const Marshal::Out<std::map<int, int>>& arg) const { arg = std::move(testData.testPrimitive06); }
	virtual void OutTestPrimitiveWithMove07(const Marshal::Out<std::multimap<int, int>>& arg) const { arg = std::move(testData.testPrimitive07); }
	virtual void OutTestPrimitiveWithMove08(const Marshal::Out<std::stack<int>>& arg) const { arg = std::move(testData.testPrimitive08); }
	virtual void OutTestPrimitiveWithMove09(const Marshal::Out<std::queue<int>>& arg) const { arg = std::move(testData.testPrimitive09); }
	virtual void OutTestPrimitiveWithMove10(const Marshal::Out<std::priority_queue<int>>& arg) const { arg = std::move(testData.testPrimitive10); }
	virtual void OutTestPrimitiveWithMove20(const Marshal::Out<std::array<int, 10>>& arg) const { arg = std::move(testData.testPrimitive20); }
	virtual void OutTestPrimitiveWithMove21(const Marshal::Out<std::forward_list<int>>& arg) const { arg = std::move(testData.testPrimitive21); }
	virtual void OutTestPrimitiveWithMove22(const Marshal::Out<std::unordered_set<int>>& arg) const { arg = std::move(testData.testPrimitive22); }
	virtual void OutTestPrimitiveWithMove23(const Marshal::Out<std::unordered_multiset<int>>& arg) const { arg = std::move(testData.testPrimitive23); }
	virtual void OutTestPrimitiveWithMove24(const Marshal::Out<std::unordered_map<int, int>>& arg) const { arg = std::move(testData.testPrimitive24); }
	virtual void OutTestPrimitiveWithMove25(const Marshal::Out<std::unordered_multimap<int, int>>& arg) const { arg = std::move(testData.testPrimitive25); }
	virtual void OutTestPrimitiveWithMove30(const Marshal::Out<int>& arg) const { arg = std::move(testData.testPrimitive30); }
	virtual void OutTestPrimitiveWithMove31(const Marshal::Out<std::vector<std::list<std::set<int>>>>& arg) const { arg = std::move(testData.testPrimitive31); }
	virtual void OutTestPrimitiveWithMove32(const Marshal::Out<std::map<int, std::multimap<std::string, int>>>& arg) const { arg = std::move(testData.testPrimitive32); }
	virtual void OutTestPrimitiveWithMove33(const Marshal::Out<std::string>& arg) const { arg = std::move(testData.testPrimitive33); }
	virtual void OutTestPrimitiveWithMove34(const Marshal::Out<std::wstring>& arg) const { arg = std::move(testData.testPrimitive34); }
	virtual void OutTestPrimitiveWithMove61(const Marshal::Out<std::vector<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive61); }
	virtual void OutTestPrimitiveWithMove62(const Marshal::Out<std::list<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive62); }
	virtual void OutTestPrimitiveWithMove63(const Marshal::Out<std::deque<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive63); }
	virtual void OutTestPrimitiveWithMove64(const Marshal::Out<std::set<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive64); }
	virtual void OutTestPrimitiveWithMove65(const Marshal::Out<std::multiset<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive65); }
	virtual void OutTestPrimitiveWithMove66(const Marshal::Out<std::map<int, std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive66); }
	virtual void OutTestPrimitiveWithMove67(const Marshal::Out<std::multimap<int, std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive67); }
	virtual void OutTestPrimitiveWithMove68(const Marshal::Out<std::stack<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive68); }
	virtual void OutTestPrimitiveWithMove69(const Marshal::Out<std::queue<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive69); }
	virtual void OutTestPrimitiveWithMove70(const Marshal::Out<std::priority_queue<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive70); }
	virtual void OutTestPrimitiveWithMove80(const Marshal::Out<std::array<std::vector<int>, 10>>& arg) const { arg = std::move(testData.testPrimitive80); }
	virtual void OutTestPrimitiveWithMove81(const Marshal::Out<std::forward_list<std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive81); }
	virtual void OutTestPrimitiveWithMove82(const Marshal::Out<std::unordered_set<std::vector<int>, VectorHasher<int>>>& arg) const { arg = std::move(testData.testPrimitive82); }
	virtual void OutTestPrimitiveWithMove83(const Marshal::Out<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& arg) const { arg = std::move(testData.testPrimitive83); }
	virtual void OutTestPrimitiveWithMove84(const Marshal::Out<std::unordered_map<int, std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive84); }
	virtual void OutTestPrimitiveWithMove85(const Marshal::Out<std::unordered_multimap<int, std::vector<int>>>& arg) const { arg = std::move(testData.testPrimitive85); }
#endif

	virtual void OutTestCustomMarshaller01(const Marshal::Out<std::vector<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller01; }
	virtual void OutTestCustomMarshaller02(const Marshal::Out<std::list<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller02; }
	virtual void OutTestCustomMarshaller03(const Marshal::Out<std::deque<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller03; }
	virtual void OutTestCustomMarshaller04(const Marshal::Out<std::set<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller04; }
	virtual void OutTestCustomMarshaller05(const Marshal::Out<std::multiset<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller05; }
	virtual void OutTestCustomMarshaller06(const Marshal::Out<std::map<int, CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller06; }
	virtual void OutTestCustomMarshaller07(const Marshal::Out<std::multimap<int, CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller07; }
	virtual void OutTestCustomMarshaller08(const Marshal::Out<std::stack<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller08; }
	virtual void OutTestCustomMarshaller09(const Marshal::Out<std::queue<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller09; }
	virtual void OutTestCustomMarshaller10(const Marshal::Out<std::priority_queue<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller10; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshaller20(const Marshal::Out<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& arg) const { arg = testData.testCustomMarshaller20; }
	virtual void OutTestCustomMarshaller21(const Marshal::Out<std::forward_list<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller21; }
	virtual void OutTestCustomMarshaller22(const Marshal::Out<std::unordered_set<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller22; }
	virtual void OutTestCustomMarshaller23(const Marshal::Out<std::unordered_multiset<CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller23; }
	virtual void OutTestCustomMarshaller24(const Marshal::Out<std::unordered_map<int, CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller24; }
	virtual void OutTestCustomMarshaller25(const Marshal::Out<std::unordered_multimap<int, CustomMarshalObject>>& arg) const { arg = testData.testCustomMarshaller25; }
#endif
	virtual void OutTestCustomMarshaller30(const Marshal::Out<CustomMarshalObject>& arg) const { arg = testData.testCustomMarshaller30; }
	virtual void OutTestCustomMarshaller31(const Marshal::Out<std::vector<std::list<std::set<CustomMarshalObject>>>>& arg) const { arg = testData.testCustomMarshaller31; }
	virtual void OutTestCustomMarshaller32(const Marshal::Out<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller32; }
	virtual void OutTestCustomMarshaller61(const Marshal::Out<std::vector<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller61; }
	virtual void OutTestCustomMarshaller62(const Marshal::Out<std::list<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller62; }
	virtual void OutTestCustomMarshaller63(const Marshal::Out<std::deque<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller63; }
	virtual void OutTestCustomMarshaller64(const Marshal::Out<std::set<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller64; }
	virtual void OutTestCustomMarshaller65(const Marshal::Out<std::multiset<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller65; }
	virtual void OutTestCustomMarshaller66(const Marshal::Out<std::map<int, std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller66; }
	virtual void OutTestCustomMarshaller67(const Marshal::Out<std::multimap<int, std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller67; }
	virtual void OutTestCustomMarshaller68(const Marshal::Out<std::stack<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller68; }
	virtual void OutTestCustomMarshaller69(const Marshal::Out<std::queue<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller69; }
	virtual void OutTestCustomMarshaller70(const Marshal::Out<std::priority_queue<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller70; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshaller80(const Marshal::Out<std::array<std::vector<CustomMarshalObject>, 10>>& arg) const { arg = testData.testCustomMarshaller80; }
	virtual void OutTestCustomMarshaller81(const Marshal::Out<std::forward_list<std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller81; }
	virtual void OutTestCustomMarshaller82(const Marshal::Out<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller82; }
	virtual void OutTestCustomMarshaller83(const Marshal::Out<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller83; }
	virtual void OutTestCustomMarshaller84(const Marshal::Out<std::unordered_map<int, std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller84; }
	virtual void OutTestCustomMarshaller85(const Marshal::Out<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& arg) const { arg = testData.testCustomMarshaller85; }
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshallerWithMove01(const Marshal::Out<std::vector<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller01); }
	virtual void OutTestCustomMarshallerWithMove02(const Marshal::Out<std::list<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller02); }
	virtual void OutTestCustomMarshallerWithMove03(const Marshal::Out<std::deque<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller03); }
	virtual void OutTestCustomMarshallerWithMove04(const Marshal::Out<std::set<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller04); }
	virtual void OutTestCustomMarshallerWithMove05(const Marshal::Out<std::multiset<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller05); }
	virtual void OutTestCustomMarshallerWithMove06(const Marshal::Out<std::map<int, CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller06); }
	virtual void OutTestCustomMarshallerWithMove07(const Marshal::Out<std::multimap<int, CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller07); }
	virtual void OutTestCustomMarshallerWithMove08(const Marshal::Out<std::stack<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller08); }
	virtual void OutTestCustomMarshallerWithMove09(const Marshal::Out<std::queue<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller09); }
	virtual void OutTestCustomMarshallerWithMove10(const Marshal::Out<std::priority_queue<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller10); }
	virtual void OutTestCustomMarshallerWithMove20(const Marshal::Out<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& arg) const { arg = std::move(testData.testCustomMarshaller20); }
	virtual void OutTestCustomMarshallerWithMove21(const Marshal::Out<std::forward_list<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller21); }
	virtual void OutTestCustomMarshallerWithMove22(const Marshal::Out<std::unordered_set<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller22); }
	virtual void OutTestCustomMarshallerWithMove23(const Marshal::Out<std::unordered_multiset<CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller23); }
	virtual void OutTestCustomMarshallerWithMove24(const Marshal::Out<std::unordered_map<int, CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller24); }
	virtual void OutTestCustomMarshallerWithMove25(const Marshal::Out<std::unordered_multimap<int, CustomMarshalObject>>& arg) const { arg = std::move(testData.testCustomMarshaller25); }
	virtual void OutTestCustomMarshallerWithMove30(const Marshal::Out<CustomMarshalObject>& arg) const { arg = std::move(testData.testCustomMarshaller30); }
	virtual void OutTestCustomMarshallerWithMove31(const Marshal::Out<std::vector<std::list<std::set<CustomMarshalObject>>>>& arg) const { arg = std::move(testData.testCustomMarshaller31); }
	virtual void OutTestCustomMarshallerWithMove32(const Marshal::Out<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller32); }
	virtual void OutTestCustomMarshallerWithMove61(const Marshal::Out<std::vector<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller61); }
	virtual void OutTestCustomMarshallerWithMove62(const Marshal::Out<std::list<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller62); }
	virtual void OutTestCustomMarshallerWithMove63(const Marshal::Out<std::deque<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller63); }
	virtual void OutTestCustomMarshallerWithMove64(const Marshal::Out<std::set<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller64); }
	virtual void OutTestCustomMarshallerWithMove65(const Marshal::Out<std::multiset<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller65); }
	virtual void OutTestCustomMarshallerWithMove66(const Marshal::Out<std::map<int, std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller66); }
	virtual void OutTestCustomMarshallerWithMove67(const Marshal::Out<std::multimap<int, std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller67); }
	virtual void OutTestCustomMarshallerWithMove68(const Marshal::Out<std::stack<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller68); }
	virtual void OutTestCustomMarshallerWithMove69(const Marshal::Out<std::queue<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller69); }
	virtual void OutTestCustomMarshallerWithMove70(const Marshal::Out<std::priority_queue<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller70); }
	virtual void OutTestCustomMarshallerWithMove80(const Marshal::Out<std::array<std::vector<CustomMarshalObject>, 10>>& arg) const { arg = std::move(testData.testCustomMarshaller80); }
	virtual void OutTestCustomMarshallerWithMove81(const Marshal::Out<std::forward_list<std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller81); }
	virtual void OutTestCustomMarshallerWithMove82(const Marshal::Out<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller82); }
	virtual void OutTestCustomMarshallerWithMove83(const Marshal::Out<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller83); }
	virtual void OutTestCustomMarshallerWithMove84(const Marshal::Out<std::unordered_map<int, std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller84); }
	virtual void OutTestCustomMarshallerWithMove85(const Marshal::Out<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& arg) const { arg = std::move(testData.testCustomMarshaller85); }
#endif

	//Ret tests
	virtual Marshal::Ret<std::vector<int>> RetTestPrimitive01() const { return testData.testPrimitive01; }
	virtual Marshal::Ret<std::list<int>> RetTestPrimitive02() const { return testData.testPrimitive02; }
	virtual Marshal::Ret<std::deque<int>> RetTestPrimitive03() const { return testData.testPrimitive03; }
	virtual Marshal::Ret<std::set<int>> RetTestPrimitive04() const { return testData.testPrimitive04; }
	virtual Marshal::Ret<std::multiset<int>> RetTestPrimitive05() const { return testData.testPrimitive05; }
	virtual Marshal::Ret<std::map<int, int>> RetTestPrimitive06() const { return testData.testPrimitive06; }
	virtual Marshal::Ret<std::multimap<int, int>> RetTestPrimitive07() const { return testData.testPrimitive07; }
	virtual Marshal::Ret<std::stack<int>> RetTestPrimitive08() const { return testData.testPrimitive08; }
	virtual Marshal::Ret<std::queue<int>> RetTestPrimitive09() const { return testData.testPrimitive09; }
	virtual Marshal::Ret<std::priority_queue<int>> RetTestPrimitive10() const { return testData.testPrimitive10; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<int, 10>> RetTestPrimitive20() const { return testData.testPrimitive20; }
	virtual Marshal::Ret<std::forward_list<int>> RetTestPrimitive21() const { return testData.testPrimitive21; }
	virtual Marshal::Ret<std::unordered_set<int>> RetTestPrimitive22() const { return testData.testPrimitive22; }
	virtual Marshal::Ret<std::unordered_multiset<int>> RetTestPrimitive23() const { return testData.testPrimitive23; }
	virtual Marshal::Ret<std::unordered_map<int, int>> RetTestPrimitive24() const { return testData.testPrimitive24; }
	virtual Marshal::Ret<std::unordered_multimap<int, int>> RetTestPrimitive25() const { return testData.testPrimitive25; }
#endif
	virtual Marshal::Ret<int> RetTestPrimitive30() const { return testData.testPrimitive30; }
	virtual Marshal::Ret<std::vector<std::list<std::set<int>>>> RetTestPrimitive31() const { return testData.testPrimitive31; }
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, int>>> RetTestPrimitive32() const { return testData.testPrimitive32; }
	virtual Marshal::Ret<std::string> RetTestPrimitive33() const { return testData.testPrimitive33; }
	virtual Marshal::Ret<std::wstring> RetTestPrimitive34() const { return testData.testPrimitive34; }
	virtual Marshal::Ret<std::vector<std::vector<int>>> RetTestPrimitive61() const { return testData.testPrimitive61; }
	virtual Marshal::Ret<std::list<std::vector<int>>> RetTestPrimitive62() const { return testData.testPrimitive62; }
	virtual Marshal::Ret<std::deque<std::vector<int>>> RetTestPrimitive63() const { return testData.testPrimitive63; }
	virtual Marshal::Ret<std::set<std::vector<int>>> RetTestPrimitive64() const { return testData.testPrimitive64; }
	virtual Marshal::Ret<std::multiset<std::vector<int>>> RetTestPrimitive65() const { return testData.testPrimitive65; }
	virtual Marshal::Ret<std::map<int, std::vector<int>>> RetTestPrimitive66() const { return testData.testPrimitive66; }
	virtual Marshal::Ret<std::multimap<int, std::vector<int>>> RetTestPrimitive67() const { return testData.testPrimitive67; }
	virtual Marshal::Ret<std::stack<std::vector<int>>> RetTestPrimitive68() const { return testData.testPrimitive68; }
	virtual Marshal::Ret<std::queue<std::vector<int>>> RetTestPrimitive69() const { return testData.testPrimitive69; }
	virtual Marshal::Ret<std::priority_queue<std::vector<int>>> RetTestPrimitive70() const { return testData.testPrimitive70; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<std::vector<int>, 10>> RetTestPrimitive80() const { return testData.testPrimitive80; }
	virtual Marshal::Ret<std::forward_list<std::vector<int>>> RetTestPrimitive81() const { return testData.testPrimitive81; }
	virtual Marshal::Ret<std::unordered_set<std::vector<int>, VectorHasher<int>>> RetTestPrimitive82() const { return testData.testPrimitive82; }
	virtual Marshal::Ret<std::unordered_multiset<std::vector<int>, VectorHasher<int>>> RetTestPrimitive83() const { return testData.testPrimitive83; }
	virtual Marshal::Ret<std::unordered_map<int, std::vector<int>>> RetTestPrimitive84() const { return testData.testPrimitive84; }
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<int>>> RetTestPrimitive85() const { return testData.testPrimitive85; }
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::vector<int>> RetTestPrimitiveWithMove01() const { auto tempData = testData.testPrimitive01; return std::move(tempData); }
	virtual Marshal::Ret<std::list<int>> RetTestPrimitiveWithMove02() const { auto tempData = testData.testPrimitive02; return std::move(tempData); }
	virtual Marshal::Ret<std::deque<int>> RetTestPrimitiveWithMove03() const { auto tempData = testData.testPrimitive03; return std::move(tempData); }
	virtual Marshal::Ret<std::set<int>> RetTestPrimitiveWithMove04() const { auto tempData = testData.testPrimitive04; return std::move(tempData); }
	virtual Marshal::Ret<std::multiset<int>> RetTestPrimitiveWithMove05() const { auto tempData = testData.testPrimitive05; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, int>> RetTestPrimitiveWithMove06() const { auto tempData = testData.testPrimitive06; return std::move(tempData); }
	virtual Marshal::Ret<std::multimap<int, int>> RetTestPrimitiveWithMove07() const { auto tempData = testData.testPrimitive07; return std::move(tempData); }
	virtual Marshal::Ret<std::stack<int>> RetTestPrimitiveWithMove08() const { auto tempData = testData.testPrimitive08; return std::move(tempData); }
	virtual Marshal::Ret<std::queue<int>> RetTestPrimitiveWithMove09() const { auto tempData = testData.testPrimitive09; return std::move(tempData); }
	virtual Marshal::Ret<std::priority_queue<int>> RetTestPrimitiveWithMove10() const { auto tempData = testData.testPrimitive10; return std::move(tempData); }
	virtual Marshal::Ret<std::array<int, 10>> RetTestPrimitiveWithMove20() const { auto tempData = testData.testPrimitive20; return std::move(tempData); }
	virtual Marshal::Ret<std::forward_list<int>> RetTestPrimitiveWithMove21() const { auto tempData = testData.testPrimitive21; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_set<int>> RetTestPrimitiveWithMove22() const { auto tempData = testData.testPrimitive22; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multiset<int>> RetTestPrimitiveWithMove23() const { auto tempData = testData.testPrimitive23; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_map<int, int>> RetTestPrimitiveWithMove24() const { auto tempData = testData.testPrimitive24; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multimap<int, int>> RetTestPrimitiveWithMove25() const { auto tempData = testData.testPrimitive25; return std::move(tempData); }
	virtual Marshal::Ret<int> RetTestPrimitiveWithMove30() const { auto tempData = testData.testPrimitive30; return std::move(tempData); }
	virtual Marshal::Ret<std::vector<std::list<std::set<int>>>> RetTestPrimitiveWithMove31() const { auto tempData = testData.testPrimitive31; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, int>>> RetTestPrimitiveWithMove32() const { auto tempData = testData.testPrimitive32; return std::move(tempData); }
	virtual Marshal::Ret<std::string> RetTestPrimitiveWithMove33() const { auto tempData = testData.testPrimitive33; return std::move(tempData); }
	virtual Marshal::Ret<std::wstring> RetTestPrimitiveWithMove34() const { auto tempData = testData.testPrimitive34; return std::move(tempData); }
	virtual Marshal::Ret<std::vector<std::vector<int>>> RetTestPrimitiveWithMove61() const { auto tempData = testData.testPrimitive61; return std::move(tempData); }
	virtual Marshal::Ret<std::list<std::vector<int>>> RetTestPrimitiveWithMove62() const { auto tempData = testData.testPrimitive62; return std::move(tempData); }
	virtual Marshal::Ret<std::deque<std::vector<int>>> RetTestPrimitiveWithMove63() const { auto tempData = testData.testPrimitive63; return std::move(tempData); }
	virtual Marshal::Ret<std::set<std::vector<int>>> RetTestPrimitiveWithMove64() const { auto tempData = testData.testPrimitive64; return std::move(tempData); }
	virtual Marshal::Ret<std::multiset<std::vector<int>>> RetTestPrimitiveWithMove65() const { auto tempData = testData.testPrimitive65; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, std::vector<int>>> RetTestPrimitiveWithMove66() const { auto tempData = testData.testPrimitive66; return std::move(tempData); }
	virtual Marshal::Ret<std::multimap<int, std::vector<int>>> RetTestPrimitiveWithMove67() const { auto tempData = testData.testPrimitive67; return std::move(tempData); }
	virtual Marshal::Ret<std::stack<std::vector<int>>> RetTestPrimitiveWithMove68() const { auto tempData = testData.testPrimitive68; return std::move(tempData); }
	virtual Marshal::Ret<std::queue<std::vector<int>>> RetTestPrimitiveWithMove69() const { auto tempData = testData.testPrimitive69; return std::move(tempData); }
	virtual Marshal::Ret<std::priority_queue<std::vector<int>>> RetTestPrimitiveWithMove70() const { auto tempData = testData.testPrimitive70; return std::move(tempData); }
	virtual Marshal::Ret<std::array<std::vector<int>, 10>> RetTestPrimitiveWithMove80() const { auto tempData = testData.testPrimitive80; return std::move(tempData); }
	virtual Marshal::Ret<std::forward_list<std::vector<int>>> RetTestPrimitiveWithMove81() const { auto tempData = testData.testPrimitive81; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_set<std::vector<int>, VectorHasher<int>>> RetTestPrimitiveWithMove82() const { auto tempData = testData.testPrimitive82; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multiset<std::vector<int>, VectorHasher<int>>> RetTestPrimitiveWithMove83() const { auto tempData = testData.testPrimitive83; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_map<int, std::vector<int>>> RetTestPrimitiveWithMove84() const { auto tempData = testData.testPrimitive84; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<int>>> RetTestPrimitiveWithMove85() const { auto tempData = testData.testPrimitive85; return std::move(tempData); }
#endif

	virtual Marshal::Ret<std::vector<CustomMarshalObject>> RetTestCustomMarshaller01() const  { return testData.testCustomMarshaller01; }
	virtual Marshal::Ret<std::list<CustomMarshalObject>> RetTestCustomMarshaller02() const  { return testData.testCustomMarshaller02; }
	virtual Marshal::Ret<std::deque<CustomMarshalObject>> RetTestCustomMarshaller03() const  { return testData.testCustomMarshaller03; }
	virtual Marshal::Ret<std::set<CustomMarshalObject>> RetTestCustomMarshaller04() const  { return testData.testCustomMarshaller04; }
	virtual Marshal::Ret<std::multiset<CustomMarshalObject>> RetTestCustomMarshaller05() const  { return testData.testCustomMarshaller05; }
	virtual Marshal::Ret<std::map<int, CustomMarshalObject>> RetTestCustomMarshaller06() const  { return testData.testCustomMarshaller06; }
	virtual Marshal::Ret<std::multimap<int, CustomMarshalObject>> RetTestCustomMarshaller07() const  { return testData.testCustomMarshaller07; }
	virtual Marshal::Ret<std::stack<CustomMarshalObject>> RetTestCustomMarshaller08() const  { return testData.testCustomMarshaller08; }
	virtual Marshal::Ret<std::queue<CustomMarshalObject>> RetTestCustomMarshaller09() const  { return testData.testCustomMarshaller09; }
	virtual Marshal::Ret<std::priority_queue<CustomMarshalObject>> RetTestCustomMarshaller10() const  { return testData.testCustomMarshaller10; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<CustomMarshalObjectWithDefaultConstructor, 10>> RetTestCustomMarshaller20() const  { return testData.testCustomMarshaller20; }
	virtual Marshal::Ret<std::forward_list<CustomMarshalObject>> RetTestCustomMarshaller21() const  { return testData.testCustomMarshaller21; }
	virtual Marshal::Ret<std::unordered_set<CustomMarshalObject>> RetTestCustomMarshaller22() const  { return testData.testCustomMarshaller22; }
	virtual Marshal::Ret<std::unordered_multiset<CustomMarshalObject>> RetTestCustomMarshaller23() const  { return testData.testCustomMarshaller23; }
	virtual Marshal::Ret<std::unordered_map<int, CustomMarshalObject>> RetTestCustomMarshaller24() const  { return testData.testCustomMarshaller24; }
	virtual Marshal::Ret<std::unordered_multimap<int, CustomMarshalObject>> RetTestCustomMarshaller25() const  { return testData.testCustomMarshaller25; }
#endif
	virtual Marshal::Ret<CustomMarshalObject> RetTestCustomMarshaller30() const  { return testData.testCustomMarshaller30; }
	virtual Marshal::Ret<std::vector<std::list<std::set<CustomMarshalObject>>>> RetTestCustomMarshaller31() const  { return testData.testCustomMarshaller31; }
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, CustomMarshalObject>>> RetTestCustomMarshaller32() const  { return testData.testCustomMarshaller32; }
	virtual Marshal::Ret<std::vector<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller61() const  { return testData.testCustomMarshaller61; }
	virtual Marshal::Ret<std::list<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller62() const  { return testData.testCustomMarshaller62; }
	virtual Marshal::Ret<std::deque<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller63() const  { return testData.testCustomMarshaller63; }
	virtual Marshal::Ret<std::set<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller64() const  { return testData.testCustomMarshaller64; }
	virtual Marshal::Ret<std::multiset<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller65() const  { return testData.testCustomMarshaller65; }
	virtual Marshal::Ret<std::map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller66() const  { return testData.testCustomMarshaller66; }
	virtual Marshal::Ret<std::multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller67() const  { return testData.testCustomMarshaller67; }
	virtual Marshal::Ret<std::stack<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller68() const  { return testData.testCustomMarshaller68; }
	virtual Marshal::Ret<std::queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller69() const  { return testData.testCustomMarshaller69; }
	virtual Marshal::Ret<std::priority_queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller70() const  { return testData.testCustomMarshaller70; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<std::vector<CustomMarshalObject>, 10>> RetTestCustomMarshaller80() const  { return testData.testCustomMarshaller80; }
	virtual Marshal::Ret<std::forward_list<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller81() const  { return testData.testCustomMarshaller81; }
	virtual Marshal::Ret<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller82() const  { return testData.testCustomMarshaller82; }
	virtual Marshal::Ret<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller83() const  { return testData.testCustomMarshaller83; }
	virtual Marshal::Ret<std::unordered_map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller84() const  { return testData.testCustomMarshaller84; }
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller85() const  { return testData.testCustomMarshaller85; }
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::vector<CustomMarshalObject>> RetTestCustomMarshallerWithMove01() const { auto tempData = testData.testCustomMarshaller01; return std::move(tempData); }
	virtual Marshal::Ret<std::list<CustomMarshalObject>> RetTestCustomMarshallerWithMove02() const { auto tempData = testData.testCustomMarshaller02; return std::move(tempData); }
	virtual Marshal::Ret<std::deque<CustomMarshalObject>> RetTestCustomMarshallerWithMove03() const { auto tempData = testData.testCustomMarshaller03; return std::move(tempData); }
	virtual Marshal::Ret<std::set<CustomMarshalObject>> RetTestCustomMarshallerWithMove04() const { auto tempData = testData.testCustomMarshaller04; return std::move(tempData); }
	virtual Marshal::Ret<std::multiset<CustomMarshalObject>> RetTestCustomMarshallerWithMove05() const { auto tempData = testData.testCustomMarshaller05; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove06() const { auto tempData = testData.testCustomMarshaller06; return std::move(tempData); }
	virtual Marshal::Ret<std::multimap<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove07() const { auto tempData = testData.testCustomMarshaller07; return std::move(tempData); }
	virtual Marshal::Ret<std::stack<CustomMarshalObject>> RetTestCustomMarshallerWithMove08() const { auto tempData = testData.testCustomMarshaller08; return std::move(tempData); }
	virtual Marshal::Ret<std::queue<CustomMarshalObject>> RetTestCustomMarshallerWithMove09() const { auto tempData = testData.testCustomMarshaller09; return std::move(tempData); }
	virtual Marshal::Ret<std::priority_queue<CustomMarshalObject>> RetTestCustomMarshallerWithMove10() const { auto tempData = testData.testCustomMarshaller10; return std::move(tempData); }
	virtual Marshal::Ret<std::array<CustomMarshalObjectWithDefaultConstructor, 10>> RetTestCustomMarshallerWithMove20() const { auto tempData = testData.testCustomMarshaller20; return std::move(tempData); }
	virtual Marshal::Ret<std::forward_list<CustomMarshalObject>> RetTestCustomMarshallerWithMove21() const { auto tempData = testData.testCustomMarshaller21; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_set<CustomMarshalObject>> RetTestCustomMarshallerWithMove22() const { auto tempData = testData.testCustomMarshaller22; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multiset<CustomMarshalObject>> RetTestCustomMarshallerWithMove23() const { auto tempData = testData.testCustomMarshaller23; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_map<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove24() const { auto tempData = testData.testCustomMarshaller24; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multimap<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove25() const { auto tempData = testData.testCustomMarshaller25; return std::move(tempData); }
	virtual Marshal::Ret<CustomMarshalObject> RetTestCustomMarshallerWithMove30() const { auto tempData = testData.testCustomMarshaller30; return std::move(tempData); }
	virtual Marshal::Ret<std::vector<std::list<std::set<CustomMarshalObject>>>> RetTestCustomMarshallerWithMove31() const { auto tempData = testData.testCustomMarshaller31; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, CustomMarshalObject>>> RetTestCustomMarshallerWithMove32() const { auto tempData = testData.testCustomMarshaller32; return std::move(tempData); }
	virtual Marshal::Ret<std::vector<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove61() const { auto tempData = testData.testCustomMarshaller61; return std::move(tempData); }
	virtual Marshal::Ret<std::list<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove62() const { auto tempData = testData.testCustomMarshaller62; return std::move(tempData); }
	virtual Marshal::Ret<std::deque<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove63() const { auto tempData = testData.testCustomMarshaller63; return std::move(tempData); }
	virtual Marshal::Ret<std::set<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove64() const { auto tempData = testData.testCustomMarshaller64; return std::move(tempData); }
	virtual Marshal::Ret<std::multiset<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove65() const { auto tempData = testData.testCustomMarshaller65; return std::move(tempData); }
	virtual Marshal::Ret<std::map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove66() const { auto tempData = testData.testCustomMarshaller66; return std::move(tempData); }
	virtual Marshal::Ret<std::multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove67() const { auto tempData = testData.testCustomMarshaller67; return std::move(tempData); }
	virtual Marshal::Ret<std::stack<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove68() const { auto tempData = testData.testCustomMarshaller68; return std::move(tempData); }
	virtual Marshal::Ret<std::queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove69() const { auto tempData = testData.testCustomMarshaller69; return std::move(tempData); }
	virtual Marshal::Ret<std::priority_queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove70() const { auto tempData = testData.testCustomMarshaller70; return std::move(tempData); }
	virtual Marshal::Ret<std::array<std::vector<CustomMarshalObject>, 10>> RetTestCustomMarshallerWithMove80() const { auto tempData = testData.testCustomMarshaller80; return std::move(tempData); }
	virtual Marshal::Ret<std::forward_list<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove81() const { auto tempData = testData.testCustomMarshaller81; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshallerWithMove82() const { auto tempData = testData.testCustomMarshaller82; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshallerWithMove83() const { auto tempData = testData.testCustomMarshaller83; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove84() const { auto tempData = testData.testCustomMarshaller84; return std::move(tempData); }
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove85() const { auto tempData = testData.testCustomMarshaller85; return std::move(tempData); }
#endif

protected:
	//In tests
	virtual bool InTestPrimitive01(const Marshal::In<std::vector<int>>& marshaller) const { return InTestPrimitive01(marshaller.Get()); }
	virtual bool InTestPrimitive02(const Marshal::In<std::list<int>>& marshaller) const { return InTestPrimitive02(marshaller.Get()); }
	virtual bool InTestPrimitive03(const Marshal::In<std::deque<int>>& marshaller) const { return InTestPrimitive03(marshaller.Get()); }
	virtual bool InTestPrimitive04(const Marshal::In<std::set<int>>& marshaller) const { return InTestPrimitive04(marshaller.Get()); }
	virtual bool InTestPrimitive05(const Marshal::In<std::multiset<int>>& marshaller) const { return InTestPrimitive05(marshaller.Get()); }
	virtual bool InTestPrimitive06(const Marshal::In<std::map<int, int>>& marshaller) const { return InTestPrimitive06(marshaller.Get()); }
	virtual bool InTestPrimitive07(const Marshal::In<std::multimap<int, int>>& marshaller) const { return InTestPrimitive07(marshaller.Get()); }
	virtual bool InTestPrimitive08(const Marshal::In<std::stack<int>>& marshaller) const { return InTestPrimitive08(marshaller.Get()); }
	virtual bool InTestPrimitive09(const Marshal::In<std::queue<int>>& marshaller) const { return InTestPrimitive09(marshaller.Get()); }
	virtual bool InTestPrimitive10(const Marshal::In<std::priority_queue<int>>& marshaller) const { return InTestPrimitive10(marshaller.Get()); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestPrimitive20(const Marshal::In<std::array<int, 10>>& marshaller) const { return InTestPrimitive20(marshaller.Get()); }
	virtual bool InTestPrimitive21(const Marshal::In<std::forward_list<int>>& marshaller) const { return InTestPrimitive21(marshaller.Get()); }
	virtual bool InTestPrimitive22(const Marshal::In<std::unordered_set<int>>& marshaller) const { return InTestPrimitive22(marshaller.Get()); }
	virtual bool InTestPrimitive23(const Marshal::In<std::unordered_multiset<int>>& marshaller) const { return InTestPrimitive23(marshaller.Get()); }
	virtual bool InTestPrimitive24(const Marshal::In<std::unordered_map<int, int>>& marshaller) const { return InTestPrimitive24(marshaller.Get()); }
	virtual bool InTestPrimitive25(const Marshal::In<std::unordered_multimap<int, int>>& marshaller) const { return InTestPrimitive25(marshaller.Get()); }
#endif
	virtual bool InTestPrimitive30(const Marshal::In<int>& marshaller) const { return InTestPrimitive30(marshaller.Get()); }
	virtual bool InTestPrimitive31(const Marshal::In<std::vector<std::list<std::set<int>>>>& marshaller) const { return InTestPrimitive31(marshaller.Get()); }
	virtual bool InTestPrimitive32(const Marshal::In<std::map<int, std::multimap<std::string, int>>>& marshaller) const { return InTestPrimitive32(marshaller.Get()); }
	virtual bool InTestPrimitive33(const Marshal::In<std::string>& marshaller) const { return InTestPrimitive33(marshaller.Get()); }
	virtual bool InTestPrimitive34(const Marshal::In<std::wstring>& marshaller) const { return InTestPrimitive34(marshaller.Get()); }
	virtual bool InTestPrimitive61(const Marshal::In<std::vector<std::vector<int>>>& marshaller) const { return InTestPrimitive61(marshaller.Get()); }
	virtual bool InTestPrimitive62(const Marshal::In<std::list<std::vector<int>>>& marshaller) const { return InTestPrimitive62(marshaller.Get()); }
	virtual bool InTestPrimitive63(const Marshal::In<std::deque<std::vector<int>>>& marshaller) const { return InTestPrimitive63(marshaller.Get()); }
	virtual bool InTestPrimitive64(const Marshal::In<std::set<std::vector<int>>>& marshaller) const { return InTestPrimitive64(marshaller.Get()); }
	virtual bool InTestPrimitive65(const Marshal::In<std::multiset<std::vector<int>>>& marshaller) const { return InTestPrimitive65(marshaller.Get()); }
	virtual bool InTestPrimitive66(const Marshal::In<std::map<int, std::vector<int>>>& marshaller) const { return InTestPrimitive66(marshaller.Get()); }
	virtual bool InTestPrimitive67(const Marshal::In<std::multimap<int, std::vector<int>>>& marshaller) const { return InTestPrimitive67(marshaller.Get()); }
	virtual bool InTestPrimitive68(const Marshal::In<std::stack<std::vector<int>>>& marshaller) const { return InTestPrimitive68(marshaller.Get()); }
	virtual bool InTestPrimitive69(const Marshal::In<std::queue<std::vector<int>>>& marshaller) const { return InTestPrimitive69(marshaller.Get()); }
	virtual bool InTestPrimitive70(const Marshal::In<std::priority_queue<std::vector<int>>>& marshaller) const { return InTestPrimitive70(marshaller.Get()); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestPrimitive80(const Marshal::In<std::array<std::vector<int>, 10>>& marshaller) const { return InTestPrimitive80(marshaller.Get()); }
	virtual bool InTestPrimitive81(const Marshal::In<std::forward_list<std::vector<int>>>& marshaller) const { return InTestPrimitive81(marshaller.Get()); }
	virtual bool InTestPrimitive82(const Marshal::In<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const { return InTestPrimitive82(marshaller.Get()); }
	virtual bool InTestPrimitive83(const Marshal::In<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const { return InTestPrimitive83(marshaller.Get()); }
	virtual bool InTestPrimitive84(const Marshal::In<std::unordered_map<int, std::vector<int>>>& marshaller) const { return InTestPrimitive84(marshaller.Get()); }
	virtual bool InTestPrimitive85(const Marshal::In<std::unordered_multimap<int, std::vector<int>>>& marshaller) const { return InTestPrimitive85(marshaller.Get()); }
#endif

	virtual bool InTestCustomMarshaller01(const Marshal::In<std::vector<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller01(marshaller.Get()); }
	virtual bool InTestCustomMarshaller02(const Marshal::In<std::list<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller02(marshaller.Get()); }
	virtual bool InTestCustomMarshaller03(const Marshal::In<std::deque<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller03(marshaller.Get()); }
	virtual bool InTestCustomMarshaller04(const Marshal::In<std::set<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller04(marshaller.Get()); }
	virtual bool InTestCustomMarshaller05(const Marshal::In<std::multiset<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller05(marshaller.Get()); }
	virtual bool InTestCustomMarshaller06(const Marshal::In<std::map<int, CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller06(marshaller.Get()); }
	virtual bool InTestCustomMarshaller07(const Marshal::In<std::multimap<int, CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller07(marshaller.Get()); }
	virtual bool InTestCustomMarshaller08(const Marshal::In<std::stack<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller08(marshaller.Get()); }
	virtual bool InTestCustomMarshaller09(const Marshal::In<std::queue<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller09(marshaller.Get()); }
	virtual bool InTestCustomMarshaller10(const Marshal::In<std::priority_queue<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller10(marshaller.Get()); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestCustomMarshaller20(const Marshal::In<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const { return InTestCustomMarshaller20(marshaller.Get()); }
	virtual bool InTestCustomMarshaller21(const Marshal::In<std::forward_list<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller21(marshaller.Get()); }
	virtual bool InTestCustomMarshaller22(const Marshal::In<std::unordered_set<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller22(marshaller.Get()); }
	virtual bool InTestCustomMarshaller23(const Marshal::In<std::unordered_multiset<CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller23(marshaller.Get()); }
	virtual bool InTestCustomMarshaller24(const Marshal::In<std::unordered_map<int, CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller24(marshaller.Get()); }
	virtual bool InTestCustomMarshaller25(const Marshal::In<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const { return InTestCustomMarshaller25(marshaller.Get()); }
#endif
	virtual bool InTestCustomMarshaller30(const Marshal::In<CustomMarshalObject>& marshaller) const { return InTestCustomMarshaller30(marshaller.Get()); }
	virtual bool InTestCustomMarshaller31(const Marshal::In<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const { return InTestCustomMarshaller31(marshaller.Get()); }
	virtual bool InTestCustomMarshaller32(const Marshal::In<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller32(marshaller.Get()); }
	virtual bool InTestCustomMarshaller61(const Marshal::In<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller61(marshaller.Get()); }
	virtual bool InTestCustomMarshaller62(const Marshal::In<std::list<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller62(marshaller.Get()); }
	virtual bool InTestCustomMarshaller63(const Marshal::In<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller63(marshaller.Get()); }
	virtual bool InTestCustomMarshaller64(const Marshal::In<std::set<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller64(marshaller.Get()); }
	virtual bool InTestCustomMarshaller65(const Marshal::In<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller65(marshaller.Get()); }
	virtual bool InTestCustomMarshaller66(const Marshal::In<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller66(marshaller.Get()); }
	virtual bool InTestCustomMarshaller67(const Marshal::In<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller67(marshaller.Get()); }
	virtual bool InTestCustomMarshaller68(const Marshal::In<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller68(marshaller.Get()); }
	virtual bool InTestCustomMarshaller69(const Marshal::In<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller69(marshaller.Get()); }
	virtual bool InTestCustomMarshaller70(const Marshal::In<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller70(marshaller.Get()); }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestCustomMarshaller80(const Marshal::In<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const { return InTestCustomMarshaller80(marshaller.Get()); }
	virtual bool InTestCustomMarshaller81(const Marshal::In<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller81(marshaller.Get()); }
	virtual bool InTestCustomMarshaller82(const Marshal::In<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller82(marshaller.Get()); }
	virtual bool InTestCustomMarshaller83(const Marshal::In<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller83(marshaller.Get()); }
	virtual bool InTestCustomMarshaller84(const Marshal::In<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller84(marshaller.Get()); }
	virtual bool InTestCustomMarshaller85(const Marshal::In<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const { return InTestCustomMarshaller85(marshaller.Get()); }
#endif

	//InOut tests
	virtual bool InOutTestPrimitive01(const Marshal::InOut<std::vector<int>>& marshaller) const { std::vector<int> data = marshaller.Get(); bool result = InOutTestPrimitive01(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive02(const Marshal::InOut<std::list<int>>& marshaller) const { std::list<int> data = marshaller.Get(); bool result = InOutTestPrimitive02(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive03(const Marshal::InOut<std::deque<int>>& marshaller) const { std::deque<int> data = marshaller.Get(); bool result = InOutTestPrimitive03(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive04(const Marshal::InOut<std::set<int>>& marshaller) const { std::set<int> data = marshaller.Get(); bool result = InOutTestPrimitive04(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive05(const Marshal::InOut<std::multiset<int>>& marshaller) const { std::multiset<int> data = marshaller.Get(); bool result = InOutTestPrimitive05(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive06(const Marshal::InOut<std::map<int, int>>& marshaller) const { std::map<int, int> data = marshaller.Get(); bool result = InOutTestPrimitive06(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive07(const Marshal::InOut<std::multimap<int, int>>& marshaller) const { std::multimap<int, int> data = marshaller.Get(); bool result = InOutTestPrimitive07(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive08(const Marshal::InOut<std::stack<int>>& marshaller) const { std::stack<int> data = marshaller.Get(); bool result = InOutTestPrimitive08(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive09(const Marshal::InOut<std::queue<int>>& marshaller) const { std::queue<int> data = marshaller.Get(); bool result = InOutTestPrimitive09(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive10(const Marshal::InOut<std::priority_queue<int>>& marshaller) const { std::priority_queue<int> data = marshaller.Get(); bool result = InOutTestPrimitive10(data); marshaller.Set(data); return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestPrimitive20(const Marshal::InOut<std::array<int, 10>>& marshaller) const { std::array<int, 10> data = marshaller.Get(); bool result = InOutTestPrimitive20(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive21(const Marshal::InOut<std::forward_list<int>>& marshaller) const { std::forward_list<int> data = marshaller.Get(); bool result = InOutTestPrimitive21(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive22(const Marshal::InOut<std::unordered_set<int>>& marshaller) const { std::unordered_set<int> data = marshaller.Get(); bool result = InOutTestPrimitive22(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive23(const Marshal::InOut<std::unordered_multiset<int>>& marshaller) const { std::unordered_multiset<int> data = marshaller.Get(); bool result = InOutTestPrimitive23(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive24(const Marshal::InOut<std::unordered_map<int, int>>& marshaller) const { std::unordered_map<int, int> data = marshaller.Get(); bool result = InOutTestPrimitive24(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive25(const Marshal::InOut<std::unordered_multimap<int, int>>& marshaller) const { std::unordered_multimap<int, int> data = marshaller.Get(); bool result = InOutTestPrimitive25(data); marshaller.Set(data); return result; }
#endif
	virtual bool InOutTestPrimitive30(const Marshal::InOut<int>& marshaller) const { int data = marshaller.Get(); bool result = InOutTestPrimitive30(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive31(const Marshal::InOut<std::vector<std::list<std::set<int>>>>& marshaller) const { std::vector<std::list<std::set<int>>> data = marshaller.Get(); bool result = InOutTestPrimitive31(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive32(const Marshal::InOut<std::map<int, std::multimap<std::string, int>>>& marshaller) const { std::map<int, std::multimap<std::string, int>> data = marshaller.Get(); bool result = InOutTestPrimitive32(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive33(const Marshal::InOut<std::string>& marshaller) const { std::string data = marshaller.Get(); bool result = InOutTestPrimitive33(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive34(const Marshal::InOut<std::wstring>& marshaller) const { std::wstring data = marshaller.Get(); bool result = InOutTestPrimitive34(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive61(const Marshal::InOut<std::vector<std::vector<int>>>& marshaller) const { std::vector<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive61(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive62(const Marshal::InOut<std::list<std::vector<int>>>& marshaller) const { std::list<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive62(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive63(const Marshal::InOut<std::deque<std::vector<int>>>& marshaller) const { std::deque<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive63(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive64(const Marshal::InOut<std::set<std::vector<int>>>& marshaller) const { std::set<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive64(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive65(const Marshal::InOut<std::multiset<std::vector<int>>>& marshaller) const { std::multiset<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive65(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive66(const Marshal::InOut<std::map<int, std::vector<int>>>& marshaller) const { std::map<int, std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive66(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive67(const Marshal::InOut<std::multimap<int, std::vector<int>>>& marshaller) const { std::multimap<int, std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive67(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive68(const Marshal::InOut<std::stack<std::vector<int>>>& marshaller) const { std::stack<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive68(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive69(const Marshal::InOut<std::queue<std::vector<int>>>& marshaller) const { std::queue<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive69(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive70(const Marshal::InOut<std::priority_queue<std::vector<int>>>& marshaller) const { std::priority_queue<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive70(data); marshaller.Set(data); return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestPrimitive80(const Marshal::InOut<std::array<std::vector<int>, 10>>& marshaller) const { std::array<std::vector<int>, 10> data = marshaller.Get(); bool result = InOutTestPrimitive80(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive81(const Marshal::InOut<std::forward_list<std::vector<int>>>& marshaller) const { std::forward_list<std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive81(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive82(const Marshal::InOut<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const { std::unordered_set<std::vector<int>, VectorHasher<int>> data = marshaller.Get(); bool result = InOutTestPrimitive82(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive83(const Marshal::InOut<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const { std::unordered_multiset<std::vector<int>, VectorHasher<int>> data = marshaller.Get(); bool result = InOutTestPrimitive83(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive84(const Marshal::InOut<std::unordered_map<int, std::vector<int>>>& marshaller) const { std::unordered_map<int, std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive84(data); marshaller.Set(data); return result; }
	virtual bool InOutTestPrimitive85(const Marshal::InOut<std::unordered_multimap<int, std::vector<int>>>& marshaller) const { std::unordered_multimap<int, std::vector<int>> data = marshaller.Get(); bool result = InOutTestPrimitive85(data); marshaller.Set(data); return result; }
#endif

	virtual bool InOutTestCustomMarshaller01(const Marshal::InOut<std::vector<CustomMarshalObject>>& marshaller) const { std::vector<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller01(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller02(const Marshal::InOut<std::list<CustomMarshalObject>>& marshaller) const { std::list<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller02(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller03(const Marshal::InOut<std::deque<CustomMarshalObject>>& marshaller) const { std::deque<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller03(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller04(const Marshal::InOut<std::set<CustomMarshalObject>>& marshaller) const { std::set<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller04(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller05(const Marshal::InOut<std::multiset<CustomMarshalObject>>& marshaller) const { std::multiset<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller05(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller06(const Marshal::InOut<std::map<int, CustomMarshalObject>>& marshaller) const { std::map<int, CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller06(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller07(const Marshal::InOut<std::multimap<int, CustomMarshalObject>>& marshaller) const { std::multimap<int, CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller07(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller08(const Marshal::InOut<std::stack<CustomMarshalObject>>& marshaller) const { std::stack<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller08(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller09(const Marshal::InOut<std::queue<CustomMarshalObject>>& marshaller) const { std::queue<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller09(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller10(const Marshal::InOut<std::priority_queue<CustomMarshalObject>>& marshaller) const { std::priority_queue<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller10(data); marshaller.Set(data); return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestCustomMarshaller20(const Marshal::InOut<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const { std::array<CustomMarshalObjectWithDefaultConstructor, 10> data = marshaller.Get(); bool result = InOutTestCustomMarshaller20(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller21(const Marshal::InOut<std::forward_list<CustomMarshalObject>>& marshaller) const { std::forward_list<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller21(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller22(const Marshal::InOut<std::unordered_set<CustomMarshalObject>>& marshaller) const { std::unordered_set<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller22(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller23(const Marshal::InOut<std::unordered_multiset<CustomMarshalObject>>& marshaller) const { std::unordered_multiset<CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller23(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller24(const Marshal::InOut<std::unordered_map<int, CustomMarshalObject>>& marshaller) const { std::unordered_map<int, CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller24(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller25(const Marshal::InOut<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const { std::unordered_multimap<int, CustomMarshalObject> data = marshaller.Get(); bool result = InOutTestCustomMarshaller25(data); marshaller.Set(data); return result; }
#endif
	virtual bool InOutTestCustomMarshaller30(const Marshal::InOut<CustomMarshalObject>& marshaller) const { CustomMarshalObject data = marshaller.Get(); bool result = InOutTestCustomMarshaller30(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller31(const Marshal::InOut<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const { std::vector<std::list<std::set<CustomMarshalObject>>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller31(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller32(const Marshal::InOut<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const { std::map<int, std::multimap<std::string, CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller32(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller61(const Marshal::InOut<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const { std::vector<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller61(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller62(const Marshal::InOut<std::list<std::vector<CustomMarshalObject>>>& marshaller) const { std::list<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller62(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller63(const Marshal::InOut<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const { std::deque<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller63(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller64(const Marshal::InOut<std::set<std::vector<CustomMarshalObject>>>& marshaller) const { std::set<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller64(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller65(const Marshal::InOut<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const { std::multiset<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller65(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller66(const Marshal::InOut<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const { std::map<int, std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller66(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller67(const Marshal::InOut<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const { std::multimap<int, std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller67(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller68(const Marshal::InOut<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const { std::stack<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller68(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller69(const Marshal::InOut<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const { std::queue<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller69(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller70(const Marshal::InOut<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const { std::priority_queue<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller70(data); marshaller.Set(data); return result; }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestCustomMarshaller80(const Marshal::InOut<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const { std::array<std::vector<CustomMarshalObject>, 10> data = marshaller.Get(); bool result = InOutTestCustomMarshaller80(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller81(const Marshal::InOut<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const { std::forward_list<std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller81(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller82(const Marshal::InOut<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const { std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller82(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller83(const Marshal::InOut<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const { std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller83(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller84(const Marshal::InOut<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const { std::unordered_map<int, std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller84(data); marshaller.Set(data); return result; }
	virtual bool InOutTestCustomMarshaller85(const Marshal::InOut<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const { std::unordered_multimap<int, std::vector<CustomMarshalObject>> data = marshaller.Get(); bool result = InOutTestCustomMarshaller85(data); marshaller.Set(data); return result; }
#endif

private:
	MarshalTestData testData;
};

#endif
