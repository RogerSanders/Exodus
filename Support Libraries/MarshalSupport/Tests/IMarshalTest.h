#ifndef __IMARSHALTEST_H__
#define __IMARSHALTEST_H__
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include "CustomMarshalObject.h"
#include "MoveTypeNoCopy.h"
#include "MoveTypeConstructOnly.h"

class IMarshalTest
{
public:
	inline virtual unsigned int Version() const { return 0; }

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

	//Vector extension tests
	virtual void OutTestVectorExtensions01(const Marshal::Out<std::vector<int>>& marshaller) const = 0;
	virtual void OutTestVectorExtensions02(const Marshal::Out<std::vector<CustomMarshalObject>>& marshaller) const = 0;
	virtual void OutTestVectorExtensions11(const Marshal::Out<std::vector<int>>& marshaller) const = 0;
	virtual void OutTestVectorExtensions12(const Marshal::Out<std::vector<CustomMarshalObject>>& marshaller) const = 0;

	//std::pair tests
	virtual bool InTestPair01(const Marshal::In<std::pair<std::wstring, int>>& marshaller) const = 0;
	virtual void OutTestPair01(const Marshal::Out<std::pair<std::wstring, int>>& marshaller) const = 0;
	virtual Marshal::Ret<std::pair<std::wstring, int>> RetTestPair01() const = 0;

protected:
	inline ~IMarshalTest() { }
};

#endif
