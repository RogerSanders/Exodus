#ifndef __IMARSHALTEST_H__
#define __IMARSHALTEST_H__
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
#include "CustomMarshalObject.h"
#include "MarshalTestData.h"

class IMarshalTest
{
public:
	inline virtual ~IMarshalTest() = 0;

	//In tests
	virtual bool InTestPrimitive01(const Marshal::In<std::vector<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive02(const Marshal::In<std::list<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive03(const Marshal::In<std::deque<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive04(const Marshal::In<std::set<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive05(const Marshal::In<std::multiset<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive06(const Marshal::In<std::map<int, int>>& marshaller) const = 0;
	virtual bool InTestPrimitive07(const Marshal::In<std::multimap<int, int>>& marshaller) const = 0;
	virtual bool InTestPrimitive08(const Marshal::In<std::stack<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive09(const Marshal::In<std::queue<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive10(const Marshal::In<std::priority_queue<int>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestPrimitive20(const Marshal::In<std::array<int, 10>>& marshaller) const = 0;
	virtual bool InTestPrimitive21(const Marshal::In<std::forward_list<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive22(const Marshal::In<std::unordered_set<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive23(const Marshal::In<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive24(const Marshal::In<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual bool InTestPrimitive25(const Marshal::In<std::unordered_multimap<int, int>>& marshaller) const = 0;
#endif
	virtual bool InTestPrimitive30(const Marshal::In<int>& marshaller) const = 0;
	virtual bool InTestPrimitive31(const Marshal::In<std::vector<std::list<std::set<int>>>>& marshaller) const = 0;
	virtual bool InTestPrimitive32(const Marshal::In<std::map<int, std::multimap<std::string, int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive33(const Marshal::In<std::string>& marshaller) const = 0;
	virtual bool InTestPrimitive34(const Marshal::In<std::wstring>& marshaller) const = 0;
	virtual bool InTestPrimitive61(const Marshal::In<std::vector<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive62(const Marshal::In<std::list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive63(const Marshal::In<std::deque<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive64(const Marshal::In<std::set<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive65(const Marshal::In<std::multiset<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive66(const Marshal::In<std::map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive67(const Marshal::In<std::multimap<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive68(const Marshal::In<std::stack<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive69(const Marshal::In<std::queue<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive70(const Marshal::In<std::priority_queue<std::vector<int>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestPrimitive80(const Marshal::In<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual bool InTestPrimitive81(const Marshal::In<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive82(const Marshal::In<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive83(const Marshal::In<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive84(const Marshal::In<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive85(const Marshal::In<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;
#endif

	virtual bool InTestCustomMarshaller01(const Marshal::In<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller02(const Marshal::In<std::list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller03(const Marshal::In<std::deque<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller04(const Marshal::In<std::set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller05(const Marshal::In<std::multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller06(const Marshal::In<std::map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller07(const Marshal::In<std::multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller08(const Marshal::In<std::stack<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller09(const Marshal::In<std::queue<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller10(const Marshal::In<std::priority_queue<CustomMarshalObject>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestCustomMarshaller20(const Marshal::In<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller21(const Marshal::In<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller22(const Marshal::In<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller23(const Marshal::In<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller24(const Marshal::In<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller25(const Marshal::In<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
#endif
	virtual bool InTestCustomMarshaller30(const Marshal::In<CustomMarshalObject>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller31(const Marshal::In<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller32(const Marshal::In<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller61(const Marshal::In<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller62(const Marshal::In<std::list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller63(const Marshal::In<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller64(const Marshal::In<std::set<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller65(const Marshal::In<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller66(const Marshal::In<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller67(const Marshal::In<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller68(const Marshal::In<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller69(const Marshal::In<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller70(const Marshal::In<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InTestCustomMarshaller80(const Marshal::In<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller81(const Marshal::In<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller82(const Marshal::In<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller83(const Marshal::In<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller84(const Marshal::In<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller85(const Marshal::In<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#endif

	//Out tests
	virtual void OutTestPrimitive01(const Marshal::Out<std::vector<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive02(const Marshal::Out<std::list<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive03(const Marshal::Out<std::deque<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive04(const Marshal::Out<std::set<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive05(const Marshal::Out<std::multiset<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive06(const Marshal::Out<std::map<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitive07(const Marshal::Out<std::multimap<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitive08(const Marshal::Out<std::stack<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive09(const Marshal::Out<std::queue<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive10(const Marshal::Out<std::priority_queue<int>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitive20(const Marshal::Out<std::array<int, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitive21(const Marshal::Out<std::forward_list<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive22(const Marshal::Out<std::unordered_set<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive23(const Marshal::Out<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive24(const Marshal::Out<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitive25(const Marshal::Out<std::unordered_multimap<int, int>>& marshaller) const = 0;
#endif
	virtual void OutTestPrimitive30(const Marshal::Out<int>& marshaller) const = 0;
	virtual void OutTestPrimitive31(const Marshal::Out<std::vector<std::list<std::set<int>>>>& marshaller) const = 0;
	virtual void OutTestPrimitive32(const Marshal::Out<std::map<int, std::multimap<std::string, int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive33(const Marshal::Out<std::string>& marshaller) const = 0;
	virtual void OutTestPrimitive34(const Marshal::Out<std::wstring>& marshaller) const = 0;
	virtual void OutTestPrimitive61(const Marshal::Out<std::vector<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive62(const Marshal::Out<std::list<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive63(const Marshal::Out<std::deque<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive64(const Marshal::Out<std::set<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive65(const Marshal::Out<std::multiset<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive66(const Marshal::Out<std::map<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive67(const Marshal::Out<std::multimap<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive68(const Marshal::Out<std::stack<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive69(const Marshal::Out<std::queue<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive70(const Marshal::Out<std::priority_queue<std::vector<int>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitive80(const Marshal::Out<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitive81(const Marshal::Out<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive82(const Marshal::Out<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive83(const Marshal::Out<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive84(const Marshal::Out<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive85(const Marshal::Out<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestPrimitiveWithMove01(const Marshal::Out<std::vector<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove02(const Marshal::Out<std::list<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove03(const Marshal::Out<std::deque<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove04(const Marshal::Out<std::set<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove05(const Marshal::Out<std::multiset<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove06(const Marshal::Out<std::map<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove07(const Marshal::Out<std::multimap<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove08(const Marshal::Out<std::stack<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove09(const Marshal::Out<std::queue<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove10(const Marshal::Out<std::priority_queue<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove20(const Marshal::Out<std::array<int, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove21(const Marshal::Out<std::forward_list<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove22(const Marshal::Out<std::unordered_set<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove23(const Marshal::Out<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove24(const Marshal::Out<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove25(const Marshal::Out<std::unordered_multimap<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove30(const Marshal::Out<int>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove31(const Marshal::Out<std::vector<std::list<std::set<int>>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove32(const Marshal::Out<std::map<int, std::multimap<std::string, int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove33(const Marshal::Out<std::string>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove34(const Marshal::Out<std::wstring>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove61(const Marshal::Out<std::vector<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove62(const Marshal::Out<std::list<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove63(const Marshal::Out<std::deque<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove64(const Marshal::Out<std::set<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove65(const Marshal::Out<std::multiset<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove66(const Marshal::Out<std::map<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove67(const Marshal::Out<std::multimap<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove68(const Marshal::Out<std::stack<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove69(const Marshal::Out<std::queue<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove70(const Marshal::Out<std::priority_queue<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove80(const Marshal::Out<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove81(const Marshal::Out<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove82(const Marshal::Out<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove83(const Marshal::Out<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove84(const Marshal::Out<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitiveWithMove85(const Marshal::Out<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;
#endif

	virtual void OutTestCustomMarshaller01(const Marshal::Out<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller02(const Marshal::Out<std::list<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller03(const Marshal::Out<std::deque<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller04(const Marshal::Out<std::set<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller05(const Marshal::Out<std::multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller06(const Marshal::Out<std::map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller07(const Marshal::Out<std::multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller08(const Marshal::Out<std::stack<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller09(const Marshal::Out<std::queue<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller10(const Marshal::Out<std::priority_queue<CustomMarshalObject>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshaller20(const Marshal::Out<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller21(const Marshal::Out<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller22(const Marshal::Out<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller23(const Marshal::Out<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller24(const Marshal::Out<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller25(const Marshal::Out<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
#endif
	virtual void OutTestCustomMarshaller30(const Marshal::Out<CustomMarshalObject>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller31(const Marshal::Out<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller32(const Marshal::Out<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller61(const Marshal::Out<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller62(const Marshal::Out<std::list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller63(const Marshal::Out<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller64(const Marshal::Out<std::set<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller65(const Marshal::Out<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller66(const Marshal::Out<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller67(const Marshal::Out<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller68(const Marshal::Out<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller69(const Marshal::Out<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller70(const Marshal::Out<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshaller80(const Marshal::Out<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller81(const Marshal::Out<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller82(const Marshal::Out<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller83(const Marshal::Out<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller84(const Marshal::Out<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller85(const Marshal::Out<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void OutTestCustomMarshallerWithMove01(const Marshal::Out<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove02(const Marshal::Out<std::list<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove03(const Marshal::Out<std::deque<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove04(const Marshal::Out<std::set<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove05(const Marshal::Out<std::multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove06(const Marshal::Out<std::map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove07(const Marshal::Out<std::multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove08(const Marshal::Out<std::stack<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove09(const Marshal::Out<std::queue<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove10(const Marshal::Out<std::priority_queue<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove20(const Marshal::Out<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove21(const Marshal::Out<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove22(const Marshal::Out<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove23(const Marshal::Out<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove24(const Marshal::Out<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove25(const Marshal::Out<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove30(const Marshal::Out<CustomMarshalObject>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove31(const Marshal::Out<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove32(const Marshal::Out<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove61(const Marshal::Out<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove62(const Marshal::Out<std::list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove63(const Marshal::Out<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove64(const Marshal::Out<std::set<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove65(const Marshal::Out<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove66(const Marshal::Out<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove67(const Marshal::Out<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove68(const Marshal::Out<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove69(const Marshal::Out<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove70(const Marshal::Out<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove80(const Marshal::Out<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove81(const Marshal::Out<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove82(const Marshal::Out<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove83(const Marshal::Out<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove84(const Marshal::Out<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshallerWithMove85(const Marshal::Out<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#endif

	//InOut tests
	virtual bool InOutTestPrimitive01(const Marshal::InOut<std::vector<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive02(const Marshal::InOut<std::list<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive03(const Marshal::InOut<std::deque<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive04(const Marshal::InOut<std::set<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive05(const Marshal::InOut<std::multiset<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive06(const Marshal::InOut<std::map<int, int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive07(const Marshal::InOut<std::multimap<int, int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive08(const Marshal::InOut<std::stack<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive09(const Marshal::InOut<std::queue<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive10(const Marshal::InOut<std::priority_queue<int>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestPrimitive20(const Marshal::InOut<std::array<int, 10>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive21(const Marshal::InOut<std::forward_list<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive22(const Marshal::InOut<std::unordered_set<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive23(const Marshal::InOut<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive24(const Marshal::InOut<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive25(const Marshal::InOut<std::unordered_multimap<int, int>>& marshaller) const = 0;
#endif
	virtual bool InOutTestPrimitive30(const Marshal::InOut<int>& marshaller) const = 0;
	virtual bool InOutTestPrimitive31(const Marshal::InOut<std::vector<std::list<std::set<int>>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive32(const Marshal::InOut<std::map<int, std::multimap<std::string, int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive33(const Marshal::InOut<std::string>& marshaller) const = 0;
	virtual bool InOutTestPrimitive34(const Marshal::InOut<std::wstring>& marshaller) const = 0;
	virtual bool InOutTestPrimitive61(const Marshal::InOut<std::vector<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive62(const Marshal::InOut<std::list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive63(const Marshal::InOut<std::deque<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive64(const Marshal::InOut<std::set<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive65(const Marshal::InOut<std::multiset<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive66(const Marshal::InOut<std::map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive67(const Marshal::InOut<std::multimap<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive68(const Marshal::InOut<std::stack<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive69(const Marshal::InOut<std::queue<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive70(const Marshal::InOut<std::priority_queue<std::vector<int>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestPrimitive80(const Marshal::InOut<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive81(const Marshal::InOut<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive82(const Marshal::InOut<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive83(const Marshal::InOut<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive84(const Marshal::InOut<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive85(const Marshal::InOut<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;
#endif

	virtual bool InOutTestCustomMarshaller01(const Marshal::InOut<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller02(const Marshal::InOut<std::list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller03(const Marshal::InOut<std::deque<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller04(const Marshal::InOut<std::set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller05(const Marshal::InOut<std::multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller06(const Marshal::InOut<std::map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller07(const Marshal::InOut<std::multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller08(const Marshal::InOut<std::stack<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller09(const Marshal::InOut<std::queue<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller10(const Marshal::InOut<std::priority_queue<CustomMarshalObject>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestCustomMarshaller20(const Marshal::InOut<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller21(const Marshal::InOut<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller22(const Marshal::InOut<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller23(const Marshal::InOut<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller24(const Marshal::InOut<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller25(const Marshal::InOut<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
#endif
	virtual bool InOutTestCustomMarshaller30(const Marshal::InOut<CustomMarshalObject>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller31(const Marshal::InOut<std::vector<std::list<std::set<CustomMarshalObject>>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller32(const Marshal::InOut<std::map<int, std::multimap<std::string, CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller61(const Marshal::InOut<std::vector<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller62(const Marshal::InOut<std::list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller63(const Marshal::InOut<std::deque<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller64(const Marshal::InOut<std::set<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller65(const Marshal::InOut<std::multiset<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller66(const Marshal::InOut<std::map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller67(const Marshal::InOut<std::multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller68(const Marshal::InOut<std::stack<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller69(const Marshal::InOut<std::queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller70(const Marshal::InOut<std::priority_queue<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual bool InOutTestCustomMarshaller80(const Marshal::InOut<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller81(const Marshal::InOut<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller82(const Marshal::InOut<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller83(const Marshal::InOut<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller84(const Marshal::InOut<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller85(const Marshal::InOut<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
#endif

	//Ret tests
	virtual Marshal::Ret<std::vector<int>> RetTestPrimitive01() const = 0;
	virtual Marshal::Ret<std::list<int>> RetTestPrimitive02() const = 0;
	virtual Marshal::Ret<std::deque<int>> RetTestPrimitive03() const = 0;
	virtual Marshal::Ret<std::set<int>> RetTestPrimitive04() const = 0;
	virtual Marshal::Ret<std::multiset<int>> RetTestPrimitive05() const = 0;
	virtual Marshal::Ret<std::map<int, int>> RetTestPrimitive06() const = 0;
	virtual Marshal::Ret<std::multimap<int, int>> RetTestPrimitive07() const = 0;
	virtual Marshal::Ret<std::stack<int>> RetTestPrimitive08() const = 0;
	virtual Marshal::Ret<std::queue<int>> RetTestPrimitive09() const = 0;
	virtual Marshal::Ret<std::priority_queue<int>> RetTestPrimitive10() const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<int, 10>> RetTestPrimitive20() const = 0;
	virtual Marshal::Ret<std::forward_list<int>> RetTestPrimitive21() const = 0;
	virtual Marshal::Ret<std::unordered_set<int>> RetTestPrimitive22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<int>> RetTestPrimitive23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, int>> RetTestPrimitive24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, int>> RetTestPrimitive25() const = 0;
#endif
	virtual Marshal::Ret<int> RetTestPrimitive30() const = 0;
	virtual Marshal::Ret<std::vector<std::list<std::set<int>>>> RetTestPrimitive31() const = 0;
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, int>>> RetTestPrimitive32() const = 0;
	virtual Marshal::Ret<std::string> RetTestPrimitive33() const = 0;
	virtual Marshal::Ret<std::wstring> RetTestPrimitive34() const = 0;
	virtual Marshal::Ret<std::vector<std::vector<int>>> RetTestPrimitive61() const = 0;
	virtual Marshal::Ret<std::list<std::vector<int>>> RetTestPrimitive62() const = 0;
	virtual Marshal::Ret<std::deque<std::vector<int>>> RetTestPrimitive63() const = 0;
	virtual Marshal::Ret<std::set<std::vector<int>>> RetTestPrimitive64() const = 0;
	virtual Marshal::Ret<std::multiset<std::vector<int>>> RetTestPrimitive65() const = 0;
	virtual Marshal::Ret<std::map<int, std::vector<int>>> RetTestPrimitive66() const = 0;
	virtual Marshal::Ret<std::multimap<int, std::vector<int>>> RetTestPrimitive67() const = 0;
	virtual Marshal::Ret<std::stack<std::vector<int>>> RetTestPrimitive68() const = 0;
	virtual Marshal::Ret<std::queue<std::vector<int>>> RetTestPrimitive69() const = 0;
	virtual Marshal::Ret<std::priority_queue<std::vector<int>>> RetTestPrimitive70() const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<std::vector<int>, 10>> RetTestPrimitive80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<int>>> RetTestPrimitive81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<int>, VectorHasher<int>>> RetTestPrimitive82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<int>, VectorHasher<int>>> RetTestPrimitive83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<int>>> RetTestPrimitive84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<int>>> RetTestPrimitive85() const = 0;
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::vector<int>> RetTestPrimitiveWithMove01() const = 0;
	virtual Marshal::Ret<std::list<int>> RetTestPrimitiveWithMove02() const = 0;
	virtual Marshal::Ret<std::deque<int>> RetTestPrimitiveWithMove03() const = 0;
	virtual Marshal::Ret<std::set<int>> RetTestPrimitiveWithMove04() const = 0;
	virtual Marshal::Ret<std::multiset<int>> RetTestPrimitiveWithMove05() const = 0;
	virtual Marshal::Ret<std::map<int, int>> RetTestPrimitiveWithMove06() const = 0;
	virtual Marshal::Ret<std::multimap<int, int>> RetTestPrimitiveWithMove07() const = 0;
	virtual Marshal::Ret<std::stack<int>> RetTestPrimitiveWithMove08() const = 0;
	virtual Marshal::Ret<std::queue<int>> RetTestPrimitiveWithMove09() const = 0;
	virtual Marshal::Ret<std::priority_queue<int>> RetTestPrimitiveWithMove10() const = 0;
	virtual Marshal::Ret<std::array<int, 10>> RetTestPrimitiveWithMove20() const = 0;
	virtual Marshal::Ret<std::forward_list<int>> RetTestPrimitiveWithMove21() const = 0;
	virtual Marshal::Ret<std::unordered_set<int>> RetTestPrimitiveWithMove22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<int>> RetTestPrimitiveWithMove23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, int>> RetTestPrimitiveWithMove24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, int>> RetTestPrimitiveWithMove25() const = 0;
	virtual Marshal::Ret<int> RetTestPrimitiveWithMove30() const = 0;
	virtual Marshal::Ret<std::vector<std::list<std::set<int>>>> RetTestPrimitiveWithMove31() const = 0;
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, int>>> RetTestPrimitiveWithMove32() const = 0;
	virtual Marshal::Ret<std::string> RetTestPrimitiveWithMove33() const = 0;
	virtual Marshal::Ret<std::wstring> RetTestPrimitiveWithMove34() const = 0;
	virtual Marshal::Ret<std::vector<std::vector<int>>> RetTestPrimitiveWithMove61() const = 0;
	virtual Marshal::Ret<std::list<std::vector<int>>> RetTestPrimitiveWithMove62() const = 0;
	virtual Marshal::Ret<std::deque<std::vector<int>>> RetTestPrimitiveWithMove63() const = 0;
	virtual Marshal::Ret<std::set<std::vector<int>>> RetTestPrimitiveWithMove64() const = 0;
	virtual Marshal::Ret<std::multiset<std::vector<int>>> RetTestPrimitiveWithMove65() const = 0;
	virtual Marshal::Ret<std::map<int, std::vector<int>>> RetTestPrimitiveWithMove66() const = 0;
	virtual Marshal::Ret<std::multimap<int, std::vector<int>>> RetTestPrimitiveWithMove67() const = 0;
	virtual Marshal::Ret<std::stack<std::vector<int>>> RetTestPrimitiveWithMove68() const = 0;
	virtual Marshal::Ret<std::queue<std::vector<int>>> RetTestPrimitiveWithMove69() const = 0;
	virtual Marshal::Ret<std::priority_queue<std::vector<int>>> RetTestPrimitiveWithMove70() const = 0;
	virtual Marshal::Ret<std::array<std::vector<int>, 10>> RetTestPrimitiveWithMove80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<int>>> RetTestPrimitiveWithMove81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<int>, VectorHasher<int>>> RetTestPrimitiveWithMove82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<int>, VectorHasher<int>>> RetTestPrimitiveWithMove83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<int>>> RetTestPrimitiveWithMove84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<int>>> RetTestPrimitiveWithMove85() const = 0;
#endif

	virtual Marshal::Ret<std::vector<CustomMarshalObject>> RetTestCustomMarshaller01() const = 0;
	virtual Marshal::Ret<std::list<CustomMarshalObject>> RetTestCustomMarshaller02() const = 0;
	virtual Marshal::Ret<std::deque<CustomMarshalObject>> RetTestCustomMarshaller03() const = 0;
	virtual Marshal::Ret<std::set<CustomMarshalObject>> RetTestCustomMarshaller04() const = 0;
	virtual Marshal::Ret<std::multiset<CustomMarshalObject>> RetTestCustomMarshaller05() const = 0;
	virtual Marshal::Ret<std::map<int, CustomMarshalObject>> RetTestCustomMarshaller06() const = 0;
	virtual Marshal::Ret<std::multimap<int, CustomMarshalObject>> RetTestCustomMarshaller07() const = 0;
	virtual Marshal::Ret<std::stack<CustomMarshalObject>> RetTestCustomMarshaller08() const = 0;
	virtual Marshal::Ret<std::queue<CustomMarshalObject>> RetTestCustomMarshaller09() const = 0;
	virtual Marshal::Ret<std::priority_queue<CustomMarshalObject>> RetTestCustomMarshaller10() const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<CustomMarshalObjectWithDefaultConstructor, 10>> RetTestCustomMarshaller20() const = 0;
	virtual Marshal::Ret<std::forward_list<CustomMarshalObject>> RetTestCustomMarshaller21() const = 0;
	virtual Marshal::Ret<std::unordered_set<CustomMarshalObject>> RetTestCustomMarshaller22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<CustomMarshalObject>> RetTestCustomMarshaller23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, CustomMarshalObject>> RetTestCustomMarshaller24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, CustomMarshalObject>> RetTestCustomMarshaller25() const = 0;
#endif
	virtual Marshal::Ret<CustomMarshalObject> RetTestCustomMarshaller30() const = 0;
	virtual Marshal::Ret<std::vector<std::list<std::set<CustomMarshalObject>>>> RetTestCustomMarshaller31() const = 0;
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, CustomMarshalObject>>> RetTestCustomMarshaller32() const = 0;
	virtual Marshal::Ret<std::vector<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller61() const = 0;
	virtual Marshal::Ret<std::list<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller62() const = 0;
	virtual Marshal::Ret<std::deque<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller63() const = 0;
	virtual Marshal::Ret<std::set<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller64() const = 0;
	virtual Marshal::Ret<std::multiset<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller65() const = 0;
	virtual Marshal::Ret<std::map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller66() const = 0;
	virtual Marshal::Ret<std::multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller67() const = 0;
	virtual Marshal::Ret<std::stack<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller68() const = 0;
	virtual Marshal::Ret<std::queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller69() const = 0;
	virtual Marshal::Ret<std::priority_queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller70() const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::array<std::vector<CustomMarshalObject>, 10>> RetTestCustomMarshaller80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller85() const = 0;
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual Marshal::Ret<std::vector<CustomMarshalObject>> RetTestCustomMarshallerWithMove01() const = 0;
	virtual Marshal::Ret<std::list<CustomMarshalObject>> RetTestCustomMarshallerWithMove02() const = 0;
	virtual Marshal::Ret<std::deque<CustomMarshalObject>> RetTestCustomMarshallerWithMove03() const = 0;
	virtual Marshal::Ret<std::set<CustomMarshalObject>> RetTestCustomMarshallerWithMove04() const = 0;
	virtual Marshal::Ret<std::multiset<CustomMarshalObject>> RetTestCustomMarshallerWithMove05() const = 0;
	virtual Marshal::Ret<std::map<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove06() const = 0;
	virtual Marshal::Ret<std::multimap<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove07() const = 0;
	virtual Marshal::Ret<std::stack<CustomMarshalObject>> RetTestCustomMarshallerWithMove08() const = 0;
	virtual Marshal::Ret<std::queue<CustomMarshalObject>> RetTestCustomMarshallerWithMove09() const = 0;
	virtual Marshal::Ret<std::priority_queue<CustomMarshalObject>> RetTestCustomMarshallerWithMove10() const = 0;
	virtual Marshal::Ret<std::array<CustomMarshalObjectWithDefaultConstructor, 10>> RetTestCustomMarshallerWithMove20() const = 0;
	virtual Marshal::Ret<std::forward_list<CustomMarshalObject>> RetTestCustomMarshallerWithMove21() const = 0;
	virtual Marshal::Ret<std::unordered_set<CustomMarshalObject>> RetTestCustomMarshallerWithMove22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<CustomMarshalObject>> RetTestCustomMarshallerWithMove23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, CustomMarshalObject>> RetTestCustomMarshallerWithMove25() const = 0;
	virtual Marshal::Ret<CustomMarshalObject> RetTestCustomMarshallerWithMove30() const = 0;
	virtual Marshal::Ret<std::vector<std::list<std::set<CustomMarshalObject>>>> RetTestCustomMarshallerWithMove31() const = 0;
	virtual Marshal::Ret<std::map<int, std::multimap<std::string, CustomMarshalObject>>> RetTestCustomMarshallerWithMove32() const = 0;
	virtual Marshal::Ret<std::vector<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove61() const = 0;
	virtual Marshal::Ret<std::list<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove62() const = 0;
	virtual Marshal::Ret<std::deque<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove63() const = 0;
	virtual Marshal::Ret<std::set<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove64() const = 0;
	virtual Marshal::Ret<std::multiset<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove65() const = 0;
	virtual Marshal::Ret<std::map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove66() const = 0;
	virtual Marshal::Ret<std::multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove67() const = 0;
	virtual Marshal::Ret<std::stack<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove68() const = 0;
	virtual Marshal::Ret<std::queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove69() const = 0;
	virtual Marshal::Ret<std::priority_queue<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove70() const = 0;
	virtual Marshal::Ret<std::array<std::vector<CustomMarshalObject>, 10>> RetTestCustomMarshallerWithMove80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshallerWithMove82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshallerWithMove83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshallerWithMove85() const = 0;
#endif
};
IMarshalTest::~IMarshalTest() {}

#endif
