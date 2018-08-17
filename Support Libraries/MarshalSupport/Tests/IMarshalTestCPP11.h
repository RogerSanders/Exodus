#ifndef __IMARSHALTESTCPP11_H__
#define __IMARSHALTESTCPP11_H__
#include "IMarshalTest.h"
#include "MarshalSupport/MarshalSupport.pkg"
using namespace MarshalSupport::Operators;
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "VectorHasher.h"
//class CustomMarshalObject;
#include "CustomMarshalObject.h"
#include "MoveTypeNoCopy.h"
#include "MoveTypeConstructOnly.h"
#include "ISomeInterface.h"

class IMarshalTestCPP11 : public IMarshalTest
{
public:
	inline virtual unsigned int Version() const { return 1; }

	//In tests
	virtual bool InTestPrimitive20(const Marshal::In<std::array<int, 10>>& marshaller) const = 0;
	virtual bool InTestPrimitive21(const Marshal::In<std::forward_list<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive22(const Marshal::In<std::unordered_set<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive23(const Marshal::In<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual bool InTestPrimitive24(const Marshal::In<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual bool InTestPrimitive25(const Marshal::In<std::unordered_multimap<int, int>>& marshaller) const = 0;
	virtual bool InTestPrimitive80(const Marshal::In<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual bool InTestPrimitive81(const Marshal::In<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive82(const Marshal::In<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive83(const Marshal::In<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive84(const Marshal::In<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InTestPrimitive85(const Marshal::In<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;

	virtual bool InTestCustomMarshaller20(const Marshal::In<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller21(const Marshal::In<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller22(const Marshal::In<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller23(const Marshal::In<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller24(const Marshal::In<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller25(const Marshal::In<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller80(const Marshal::In<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller81(const Marshal::In<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller82(const Marshal::In<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller83(const Marshal::In<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller84(const Marshal::In<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InTestCustomMarshaller85(const Marshal::In<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;

	//Out tests
	virtual void OutTestPrimitive20(const Marshal::Out<std::array<int, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitive21(const Marshal::Out<std::forward_list<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive22(const Marshal::Out<std::unordered_set<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive23(const Marshal::Out<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual void OutTestPrimitive24(const Marshal::Out<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitive25(const Marshal::Out<std::unordered_multimap<int, int>>& marshaller) const = 0;
	virtual void OutTestPrimitive80(const Marshal::Out<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual void OutTestPrimitive81(const Marshal::Out<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive82(const Marshal::Out<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive83(const Marshal::Out<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive84(const Marshal::Out<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual void OutTestPrimitive85(const Marshal::Out<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;
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

	virtual void OutTestCustomMarshaller20(const Marshal::Out<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller21(const Marshal::Out<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller22(const Marshal::Out<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller23(const Marshal::Out<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller24(const Marshal::Out<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller25(const Marshal::Out<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller80(const Marshal::Out<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller81(const Marshal::Out<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller82(const Marshal::Out<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller83(const Marshal::Out<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller84(const Marshal::Out<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual void OutTestCustomMarshaller85(const Marshal::Out<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
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

	//InOut tests
	virtual bool InOutTestPrimitive20(const Marshal::InOut<std::array<int, 10>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive21(const Marshal::InOut<std::forward_list<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive22(const Marshal::InOut<std::unordered_set<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive23(const Marshal::InOut<std::unordered_multiset<int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive24(const Marshal::InOut<std::unordered_map<int, int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive25(const Marshal::InOut<std::unordered_multimap<int, int>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive80(const Marshal::InOut<std::array<std::vector<int>, 10>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive81(const Marshal::InOut<std::forward_list<std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive82(const Marshal::InOut<std::unordered_set<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive83(const Marshal::InOut<std::unordered_multiset<std::vector<int>, VectorHasher<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive84(const Marshal::InOut<std::unordered_map<int, std::vector<int>>>& marshaller) const = 0;
	virtual bool InOutTestPrimitive85(const Marshal::InOut<std::unordered_multimap<int, std::vector<int>>>& marshaller) const = 0;

	virtual bool InOutTestCustomMarshaller20(const Marshal::InOut<std::array<CustomMarshalObjectWithDefaultConstructor, 10>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller21(const Marshal::InOut<std::forward_list<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller22(const Marshal::InOut<std::unordered_set<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller23(const Marshal::InOut<std::unordered_multiset<CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller24(const Marshal::InOut<std::unordered_map<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller25(const Marshal::InOut<std::unordered_multimap<int, CustomMarshalObject>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller80(const Marshal::InOut<std::array<std::vector<CustomMarshalObject>, 10>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller81(const Marshal::InOut<std::forward_list<std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller82(const Marshal::InOut<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller83(const Marshal::InOut<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller84(const Marshal::InOut<std::unordered_map<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;
	virtual bool InOutTestCustomMarshaller85(const Marshal::InOut<std::unordered_multimap<int, std::vector<CustomMarshalObject>>>& marshaller) const = 0;

	//Ret tests
	virtual Marshal::Ret<std::array<int, 10>> RetTestPrimitive20() const = 0;
	virtual Marshal::Ret<std::forward_list<int>> RetTestPrimitive21() const = 0;
	virtual Marshal::Ret<std::unordered_set<int>> RetTestPrimitive22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<int>> RetTestPrimitive23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, int>> RetTestPrimitive24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, int>> RetTestPrimitive25() const = 0;
	virtual Marshal::Ret<std::array<std::vector<int>, 10>> RetTestPrimitive80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<int>>> RetTestPrimitive81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<int>, VectorHasher<int>>> RetTestPrimitive82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<int>, VectorHasher<int>>> RetTestPrimitive83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<int>>> RetTestPrimitive84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<int>>> RetTestPrimitive85() const = 0;
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

	virtual Marshal::Ret<std::array<CustomMarshalObjectWithDefaultConstructor, 10>> RetTestCustomMarshaller20() const = 0;
	virtual Marshal::Ret<std::forward_list<CustomMarshalObject>> RetTestCustomMarshaller21() const = 0;
	virtual Marshal::Ret<std::unordered_set<CustomMarshalObject>> RetTestCustomMarshaller22() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<CustomMarshalObject>> RetTestCustomMarshaller23() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, CustomMarshalObject>> RetTestCustomMarshaller24() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, CustomMarshalObject>> RetTestCustomMarshaller25() const = 0;
	virtual Marshal::Ret<std::array<std::vector<CustomMarshalObject>, 10>> RetTestCustomMarshaller80() const = 0;
	virtual Marshal::Ret<std::forward_list<std::vector<CustomMarshalObject>>> RetTestCustomMarshaller81() const = 0;
	virtual Marshal::Ret<std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller82() const = 0;
	virtual Marshal::Ret<std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>>> RetTestCustomMarshaller83() const = 0;
	virtual Marshal::Ret<std::unordered_map<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller84() const = 0;
	virtual Marshal::Ret<std::unordered_multimap<int, std::vector<CustomMarshalObject>>> RetTestCustomMarshaller85() const = 0;
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

	//Vector extension tests
	virtual void OutTestVectorExtensions03(const Marshal::Out<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestVectorExtensions04(const Marshal::Out<std::vector<MoveTypeNoCopy>>& marshaller) const = 0;
	virtual void OutTestVectorExtensions14(const Marshal::Out<std::vector<MoveTypeNoCopy>>& marshaller) const = 0;

	//Move only object tests
	virtual bool InTestNoCopy01(const Marshal::In<MoveTypeNoCopy>& marshaller) const = 0;
	virtual bool InTestNoCopy02(const Marshal::In<std::vector<MoveTypeNoCopy>>& marshaller) const = 0;
	virtual bool InTestNoCopy03(const Marshal::In<std::list<MoveTypeNoCopy>>& marshaller) const = 0;
	virtual void OutTestNoCopy01(const Marshal::Out<MoveTypeNoCopy>& marshaller) const = 0;
	virtual void OutTestNoCopy02(const Marshal::Out<std::vector<MoveTypeNoCopy>>& marshaller) const = 0;
	virtual void OutTestNoCopy03(const Marshal::Out<std::list<MoveTypeNoCopy>>& marshaller) const = 0;
	virtual Marshal::Ret<MoveTypeNoCopy> RetTestNoCopy01() const = 0;
	virtual Marshal::Ret<std::vector<MoveTypeNoCopy>> RetTestNoCopy02() const = 0;
	virtual Marshal::Ret<std::list<MoveTypeNoCopy>> RetTestNoCopy03() const = 0;
	virtual bool InTestMoveConstructOnly01(const Marshal::In<MoveTypeConstructOnly>& marshaller) const = 0;
	virtual bool InTestMoveConstructOnly03(const Marshal::In<std::list<MoveTypeConstructOnly>>& marshaller) const = 0;

	//std::pair tests
	virtual bool InTestPair02(const Marshal::In<std::pair<std::wstring, MoveTypeNoCopy>>& marshaller) const = 0;
	virtual void OutTestPair02(const Marshal::Out<std::pair<std::wstring, MoveTypeNoCopy>>& marshaller) const = 0;
	virtual Marshal::Ret<std::pair<std::wstring, MoveTypeNoCopy>> RetTestPair02() const = 0;

	//std::tuple tests
	virtual bool InTestTuple01(const Marshal::In<std::tuple<std::wstring, int, bool, std::string>>& marshaller) const = 0;
	virtual bool InTestTuple02(const Marshal::In<std::tuple<std::string, MoveTypeNoCopy>>& marshaller) const = 0;
	virtual bool InTestTuple03(const Marshal::In<std::tuple<>>& marshaller) const = 0;
	virtual void OutTestTuple01(const Marshal::Out<std::tuple<std::wstring, int, bool, std::string>>& marshaller) const = 0;
	virtual void OutTestTuple02(const Marshal::Out<std::tuple<std::string, MoveTypeNoCopy>>& marshaller) const = 0;
	virtual void OutTestTuple03(const Marshal::Out<std::tuple<>>& marshaller) const = 0;
	virtual Marshal::Ret<std::tuple<std::wstring, int, bool, std::string>> RetTestTuple01() const = 0;
	virtual Marshal::Ret<std::tuple<std::string, MoveTypeNoCopy>> RetTestTuple02() const = 0;
	virtual Marshal::Ret<std::tuple<>> RetTestTuple03() const = 0;

	//STL pointer tests
	virtual bool InTestPointer01(const Marshal::In<std::unique_ptr<int, MarshalSupport::MarshallableDelete<int>>>& marshaller) const = 0;
	virtual bool InTestPointer02(const Marshal::In<std::unique_ptr<ISomeInterface>>& marshaller) const = 0;
	virtual void OutTestPointer01(const Marshal::Out<std::unique_ptr<int, MarshalSupport::MarshallableDelete<int>>>& marshaller) const = 0;
	virtual void OutTestPointer02(const Marshal::Out<std::unique_ptr<ISomeInterface>>& marshaller) const = 0;
	virtual Marshal::Ret<std::unique_ptr<int, MarshalSupport::MarshallableDelete<int>>> RetTestPointer01() const = 0;
	virtual Marshal::Ret<std::unique_ptr<ISomeInterface>> RetTestPointer02() const = 0;
};

#endif
