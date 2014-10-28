#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "MarshalSupport/MarshalSupport.pkg"
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
#include <utility>
#endif
#include "MarshalTestData.h"
#include "IMarshalTest.h"
#include "CompareTestData.h"
#include "CatchWrapper.h"
using namespace MarshalSupport;

ResultCaptureStub* captureStub;

//----------------------------------------------------------------------------------------
Marshal::Ret<int> ReturnLiteralParam()
{
	return 42;
}
Marshal::Ret<int> ReturnInParam(const Marshal::In<int>& testData)
{
	return testData;
}
Marshal::Ret<int> ReturnInOutParam(const Marshal::InOut<int>& testData)
{
	return testData;
}
Marshal::Ret<int> ReturnRetParam()
{
	return ReturnLiteralParam();
}
bool ForwardRetParam()
{
	return CompareTestData(42, ReturnInParam(ReturnRetParam()).Get());
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestConversions()
{
	int inoutParam = 1234;
	bool result = true;
	result &= CompareTestData(42, ReturnLiteralParam().Get());
	result &= CompareTestData(42, ReturnInParam(42).Get());
	result &= CompareTestData(inoutParam, ReturnInOutParam(inoutParam).Get());
	result &= CompareTestData(42, ReturnRetParam().Get());
	result &= ForwardRetParam();
	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) void ConfigureCatchTestFramework(ResultCaptureWrapper* resultCapture, Catch::IRunner* runner, Catch::Ptr<Catch::IConfig const> const& config)
{
	captureStub = new ResultCaptureStub(resultCapture);

	Catch::IMutableContext& catchContext = static_cast<Catch::Context&>(Catch::getCurrentMutableContext());
	catchContext.setResultCapture(captureStub);
	catchContext.setRunner(runner);
	catchContext.setConfig(config);
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestInPrimitive(IMarshalTest& marshalTest)
{
	MarshalTestData testData;

	//In tests
	std::vector<int> testPrimitive01 = testData.testPrimitive01;
	std::list<int> testPrimitive02 = testData.testPrimitive02;
	std::deque<int> testPrimitive03 = testData.testPrimitive03;
	std::set<int> testPrimitive04 = testData.testPrimitive04;
	std::multiset<int> testPrimitive05 = testData.testPrimitive05;
	std::map<int, int> testPrimitive06 = testData.testPrimitive06;
	std::multimap<int, int> testPrimitive07 = testData.testPrimitive07;
	std::stack<int> testPrimitive08 = testData.testPrimitive08;
	std::queue<int> testPrimitive09 = testData.testPrimitive09;
	std::priority_queue<int> testPrimitive10 = testData.testPrimitive10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<int, 10> testPrimitive20 = testData.testPrimitive20;
	std::forward_list<int> testPrimitive21 = testData.testPrimitive21;
	std::unordered_set<int> testPrimitive22 = testData.testPrimitive22;
	std::unordered_multiset<int> testPrimitive23 = testData.testPrimitive23;
	std::unordered_map<int, int> testPrimitive24 = testData.testPrimitive24;
	std::unordered_multimap<int, int> testPrimitive25 = testData.testPrimitive25;
#endif
	int testPrimitive30 = testData.testPrimitive30;
	std::vector<std::list<std::set<int>>> testPrimitive31 = testData.testPrimitive31;
	std::map<int, std::multimap<std::string, int>> testPrimitive32 = testData.testPrimitive32;
	std::string testPrimitive33 = testData.testPrimitive33;
	std::wstring testPrimitive34 = testData.testPrimitive34;
	std::vector<std::vector<int>> testPrimitive61 = testData.testPrimitive61;
	std::list<std::vector<int>> testPrimitive62 = testData.testPrimitive62;
	std::deque<std::vector<int>> testPrimitive63 = testData.testPrimitive63;
	std::set<std::vector<int>> testPrimitive64 = testData.testPrimitive64;
	std::multiset<std::vector<int>> testPrimitive65 = testData.testPrimitive65;
	std::map<int, std::vector<int>> testPrimitive66 = testData.testPrimitive66;
	std::multimap<int, std::vector<int>> testPrimitive67 = testData.testPrimitive67;
	std::stack<std::vector<int>> testPrimitive68 = testData.testPrimitive68;
	std::queue<std::vector<int>> testPrimitive69 = testData.testPrimitive69;
	std::priority_queue<std::vector<int>> testPrimitive70 = testData.testPrimitive70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<int>, 10> testPrimitive80 = testData.testPrimitive80;
	std::forward_list<std::vector<int>> testPrimitive81 = testData.testPrimitive81;
	std::unordered_set<std::vector<int>, VectorHasher<int>> testPrimitive82 = testData.testPrimitive82;
	std::unordered_multiset<std::vector<int>, VectorHasher<int>> testPrimitive83 = testData.testPrimitive83;
	std::unordered_map<int, std::vector<int>> testPrimitive84 = testData.testPrimitive84;
	std::unordered_multimap<int, std::vector<int>> testPrimitive85 = testData.testPrimitive85;
#endif

	bool result = true;
	result &= marshalTest.InTestPrimitive01(testPrimitive01);
	result &= marshalTest.InTestPrimitive02(testPrimitive02);
	result &= marshalTest.InTestPrimitive03(testPrimitive03);
	result &= marshalTest.InTestPrimitive04(testPrimitive04);
	result &= marshalTest.InTestPrimitive05(testPrimitive05);
	result &= marshalTest.InTestPrimitive06(testPrimitive06);
	result &= marshalTest.InTestPrimitive07(testPrimitive07);
	result &= marshalTest.InTestPrimitive08(testPrimitive08);
	result &= marshalTest.InTestPrimitive09(testPrimitive09);
	result &= marshalTest.InTestPrimitive10(testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestPrimitive20(testPrimitive20);
	result &= marshalTest.InTestPrimitive21(testPrimitive21);
	result &= marshalTest.InTestPrimitive22(testPrimitive22);
	result &= marshalTest.InTestPrimitive23(testPrimitive23);
	result &= marshalTest.InTestPrimitive24(testPrimitive24);
	result &= marshalTest.InTestPrimitive25(testPrimitive25);
#endif
	result &= marshalTest.InTestPrimitive30(testPrimitive30);
	result &= marshalTest.InTestPrimitive31(testPrimitive31);
	result &= marshalTest.InTestPrimitive32(testPrimitive32);
	result &= marshalTest.InTestPrimitive33(testPrimitive33);
	result &= marshalTest.InTestPrimitive34(testPrimitive34);
	result &= marshalTest.InTestPrimitive61(testPrimitive61);
	result &= marshalTest.InTestPrimitive62(testPrimitive62);
	result &= marshalTest.InTestPrimitive63(testPrimitive63);
	result &= marshalTest.InTestPrimitive64(testPrimitive64);
	result &= marshalTest.InTestPrimitive65(testPrimitive65);
	result &= marshalTest.InTestPrimitive66(testPrimitive66);
	result &= marshalTest.InTestPrimitive67(testPrimitive67);
	result &= marshalTest.InTestPrimitive68(testPrimitive68);
	result &= marshalTest.InTestPrimitive69(testPrimitive69);
	result &= marshalTest.InTestPrimitive70(testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestPrimitive80(testPrimitive80);
	result &= marshalTest.InTestPrimitive81(testPrimitive81);
	result &= marshalTest.InTestPrimitive82(testPrimitive82);
	result &= marshalTest.InTestPrimitive83(testPrimitive83);
	result &= marshalTest.InTestPrimitive84(testPrimitive84);
	result &= marshalTest.InTestPrimitive85(testPrimitive85);
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestPrimitive01(std::move(testPrimitive01));
	result &= marshalTest.InTestPrimitive02(std::move(testPrimitive02));
	result &= marshalTest.InTestPrimitive03(std::move(testPrimitive03));
	result &= marshalTest.InTestPrimitive04(std::move(testPrimitive04));
	result &= marshalTest.InTestPrimitive05(std::move(testPrimitive05));
	result &= marshalTest.InTestPrimitive06(std::move(testPrimitive06));
	result &= marshalTest.InTestPrimitive07(std::move(testPrimitive07));
	result &= marshalTest.InTestPrimitive08(std::move(testPrimitive08));
	result &= marshalTest.InTestPrimitive09(std::move(testPrimitive09));
	result &= marshalTest.InTestPrimitive10(std::move(testPrimitive10));
	result &= marshalTest.InTestPrimitive20(std::move(testPrimitive20));
	result &= marshalTest.InTestPrimitive21(std::move(testPrimitive21));
	result &= marshalTest.InTestPrimitive22(std::move(testPrimitive22));
	result &= marshalTest.InTestPrimitive23(std::move(testPrimitive23));
	result &= marshalTest.InTestPrimitive24(std::move(testPrimitive24));
	result &= marshalTest.InTestPrimitive25(std::move(testPrimitive25));
	result &= marshalTest.InTestPrimitive30(std::move(testPrimitive30));
	result &= marshalTest.InTestPrimitive31(std::move(testPrimitive31));
	result &= marshalTest.InTestPrimitive32(std::move(testPrimitive32));
	result &= marshalTest.InTestPrimitive33(std::move(testPrimitive33));
	result &= marshalTest.InTestPrimitive34(std::move(testPrimitive34));
	result &= marshalTest.InTestPrimitive61(std::move(testPrimitive61));
	result &= marshalTest.InTestPrimitive62(std::move(testPrimitive62));
	result &= marshalTest.InTestPrimitive63(std::move(testPrimitive63));
	result &= marshalTest.InTestPrimitive64(std::move(testPrimitive64));
	result &= marshalTest.InTestPrimitive65(std::move(testPrimitive65));
	result &= marshalTest.InTestPrimitive66(std::move(testPrimitive66));
	result &= marshalTest.InTestPrimitive67(std::move(testPrimitive67));
	result &= marshalTest.InTestPrimitive68(std::move(testPrimitive68));
	result &= marshalTest.InTestPrimitive69(std::move(testPrimitive69));
	result &= marshalTest.InTestPrimitive70(std::move(testPrimitive70));
	result &= marshalTest.InTestPrimitive80(std::move(testPrimitive80));
	result &= marshalTest.InTestPrimitive81(std::move(testPrimitive81));
	result &= marshalTest.InTestPrimitive82(std::move(testPrimitive82));
	result &= marshalTest.InTestPrimitive83(std::move(testPrimitive83));
	result &= marshalTest.InTestPrimitive84(std::move(testPrimitive84));
	result &= marshalTest.InTestPrimitive85(std::move(testPrimitive85));
#endif
	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestInComplex(IMarshalTest& marshalTest)
{
	MarshalTestData testData;

	std::vector<CustomMarshalObject> testCustomMarshaller01 = testData.testCustomMarshaller01;
	std::list<CustomMarshalObject> testCustomMarshaller02 = testData.testCustomMarshaller02;
	std::deque<CustomMarshalObject> testCustomMarshaller03 = testData.testCustomMarshaller03;
	std::set<CustomMarshalObject> testCustomMarshaller04 = testData.testCustomMarshaller04;
	std::multiset<CustomMarshalObject> testCustomMarshaller05 = testData.testCustomMarshaller05;
	std::map<int, CustomMarshalObject> testCustomMarshaller06 = testData.testCustomMarshaller06;
	std::multimap<int, CustomMarshalObject> testCustomMarshaller07 = testData.testCustomMarshaller07;
	std::stack<CustomMarshalObject> testCustomMarshaller08 = testData.testCustomMarshaller08;
	std::queue<CustomMarshalObject> testCustomMarshaller09 = testData.testCustomMarshaller09;
	std::priority_queue<CustomMarshalObject> testCustomMarshaller10 = testData.testCustomMarshaller10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<CustomMarshalObjectWithDefaultConstructor, 10> testCustomMarshaller20 = testData.testCustomMarshaller20;
	std::forward_list<CustomMarshalObject> testCustomMarshaller21 = testData.testCustomMarshaller21;
	std::unordered_set<CustomMarshalObject> testCustomMarshaller22 = testData.testCustomMarshaller22;
	std::unordered_multiset<CustomMarshalObject> testCustomMarshaller23 = testData.testCustomMarshaller23;
	std::unordered_map<int, CustomMarshalObject> testCustomMarshaller24 = testData.testCustomMarshaller24;
	std::unordered_multimap<int, CustomMarshalObject> testCustomMarshaller25 = testData.testCustomMarshaller25;
#endif
	CustomMarshalObject testCustomMarshaller30 = testData.testCustomMarshaller30;
	std::vector<std::list<std::set<CustomMarshalObject>>> testCustomMarshaller31 = testData.testCustomMarshaller31;
	std::map<int, std::multimap<std::string, CustomMarshalObject>> testCustomMarshaller32 = testData.testCustomMarshaller32;
	std::vector<std::vector<CustomMarshalObject>> testCustomMarshaller61 = testData.testCustomMarshaller61;
	std::list<std::vector<CustomMarshalObject>> testCustomMarshaller62 = testData.testCustomMarshaller62;
	std::deque<std::vector<CustomMarshalObject>> testCustomMarshaller63 = testData.testCustomMarshaller63;
	std::set<std::vector<CustomMarshalObject>> testCustomMarshaller64 = testData.testCustomMarshaller64;
	std::multiset<std::vector<CustomMarshalObject>> testCustomMarshaller65 = testData.testCustomMarshaller65;
	std::map<int, std::vector<CustomMarshalObject>> testCustomMarshaller66 = testData.testCustomMarshaller66;
	std::multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller67 = testData.testCustomMarshaller67;
	std::stack<std::vector<CustomMarshalObject>> testCustomMarshaller68 = testData.testCustomMarshaller68;
	std::queue<std::vector<CustomMarshalObject>> testCustomMarshaller69 = testData.testCustomMarshaller69;
	std::priority_queue<std::vector<CustomMarshalObject>> testCustomMarshaller70 = testData.testCustomMarshaller70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<CustomMarshalObject>, 10> testCustomMarshaller80 = testData.testCustomMarshaller80;
	std::forward_list<std::vector<CustomMarshalObject>> testCustomMarshaller81 = testData.testCustomMarshaller81;
	std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller82 = testData.testCustomMarshaller82;
	std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller83 = testData.testCustomMarshaller83;
	std::unordered_map<int, std::vector<CustomMarshalObject>> testCustomMarshaller84 = testData.testCustomMarshaller84;
	std::unordered_multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller85 = testData.testCustomMarshaller85;
#endif

	bool result = true;
	result &= marshalTest.InTestCustomMarshaller01(testCustomMarshaller01);
	result &= marshalTest.InTestCustomMarshaller02(testCustomMarshaller02);
	result &= marshalTest.InTestCustomMarshaller03(testCustomMarshaller03);
	result &= marshalTest.InTestCustomMarshaller04(testCustomMarshaller04);
	result &= marshalTest.InTestCustomMarshaller05(testCustomMarshaller05);
	result &= marshalTest.InTestCustomMarshaller06(testCustomMarshaller06);
	result &= marshalTest.InTestCustomMarshaller07(testCustomMarshaller07);
	result &= marshalTest.InTestCustomMarshaller08(testCustomMarshaller08);
	result &= marshalTest.InTestCustomMarshaller09(testCustomMarshaller09);
	result &= marshalTest.InTestCustomMarshaller10(testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestCustomMarshaller20(testCustomMarshaller20);
	result &= marshalTest.InTestCustomMarshaller21(testCustomMarshaller21);
	result &= marshalTest.InTestCustomMarshaller22(testCustomMarshaller22);
	result &= marshalTest.InTestCustomMarshaller23(testCustomMarshaller23);
	result &= marshalTest.InTestCustomMarshaller24(testCustomMarshaller24);
	result &= marshalTest.InTestCustomMarshaller25(testCustomMarshaller25);
#endif
	result &= marshalTest.InTestCustomMarshaller30(testCustomMarshaller30);
	result &= marshalTest.InTestCustomMarshaller31(testCustomMarshaller31);
	result &= marshalTest.InTestCustomMarshaller32(testCustomMarshaller32);
	result &= marshalTest.InTestCustomMarshaller61(testCustomMarshaller61);
	result &= marshalTest.InTestCustomMarshaller62(testCustomMarshaller62);
	result &= marshalTest.InTestCustomMarshaller63(testCustomMarshaller63);
	result &= marshalTest.InTestCustomMarshaller64(testCustomMarshaller64);
	result &= marshalTest.InTestCustomMarshaller65(testCustomMarshaller65);
	result &= marshalTest.InTestCustomMarshaller66(testCustomMarshaller66);
	result &= marshalTest.InTestCustomMarshaller67(testCustomMarshaller67);
	result &= marshalTest.InTestCustomMarshaller68(testCustomMarshaller68);
	result &= marshalTest.InTestCustomMarshaller69(testCustomMarshaller69);
	result &= marshalTest.InTestCustomMarshaller70(testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestCustomMarshaller80(testCustomMarshaller80);
	result &= marshalTest.InTestCustomMarshaller81(testCustomMarshaller81);
	result &= marshalTest.InTestCustomMarshaller82(testCustomMarshaller82);
	result &= marshalTest.InTestCustomMarshaller83(testCustomMarshaller83);
	result &= marshalTest.InTestCustomMarshaller84(testCustomMarshaller84);
	result &= marshalTest.InTestCustomMarshaller85(testCustomMarshaller85);
#endif
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InTestCustomMarshaller01(std::move(testCustomMarshaller01));
	result &= marshalTest.InTestCustomMarshaller02(std::move(testCustomMarshaller02));
	result &= marshalTest.InTestCustomMarshaller03(std::move(testCustomMarshaller03));
	result &= marshalTest.InTestCustomMarshaller04(std::move(testCustomMarshaller04));
	result &= marshalTest.InTestCustomMarshaller05(std::move(testCustomMarshaller05));
	result &= marshalTest.InTestCustomMarshaller06(std::move(testCustomMarshaller06));
	result &= marshalTest.InTestCustomMarshaller07(std::move(testCustomMarshaller07));
	result &= marshalTest.InTestCustomMarshaller08(std::move(testCustomMarshaller08));
	result &= marshalTest.InTestCustomMarshaller09(std::move(testCustomMarshaller09));
	result &= marshalTest.InTestCustomMarshaller10(std::move(testCustomMarshaller10));
	result &= marshalTest.InTestCustomMarshaller20(std::move(testCustomMarshaller20));
	result &= marshalTest.InTestCustomMarshaller21(std::move(testCustomMarshaller21));
	result &= marshalTest.InTestCustomMarshaller22(std::move(testCustomMarshaller22));
	result &= marshalTest.InTestCustomMarshaller23(std::move(testCustomMarshaller23));
	result &= marshalTest.InTestCustomMarshaller24(std::move(testCustomMarshaller24));
	result &= marshalTest.InTestCustomMarshaller25(std::move(testCustomMarshaller25));
	result &= marshalTest.InTestCustomMarshaller30(std::move(testCustomMarshaller30));
	result &= marshalTest.InTestCustomMarshaller31(std::move(testCustomMarshaller31));
	result &= marshalTest.InTestCustomMarshaller32(std::move(testCustomMarshaller32));
	result &= marshalTest.InTestCustomMarshaller61(std::move(testCustomMarshaller61));
	result &= marshalTest.InTestCustomMarshaller62(std::move(testCustomMarshaller62));
	result &= marshalTest.InTestCustomMarshaller63(std::move(testCustomMarshaller63));
	result &= marshalTest.InTestCustomMarshaller64(std::move(testCustomMarshaller64));
	result &= marshalTest.InTestCustomMarshaller65(std::move(testCustomMarshaller65));
	result &= marshalTest.InTestCustomMarshaller66(std::move(testCustomMarshaller66));
	result &= marshalTest.InTestCustomMarshaller67(std::move(testCustomMarshaller67));
	result &= marshalTest.InTestCustomMarshaller68(std::move(testCustomMarshaller68));
	result &= marshalTest.InTestCustomMarshaller69(std::move(testCustomMarshaller69));
	result &= marshalTest.InTestCustomMarshaller70(std::move(testCustomMarshaller70));
	result &= marshalTest.InTestCustomMarshaller80(std::move(testCustomMarshaller80));
	result &= marshalTest.InTestCustomMarshaller81(std::move(testCustomMarshaller81));
	result &= marshalTest.InTestCustomMarshaller82(std::move(testCustomMarshaller82));
	result &= marshalTest.InTestCustomMarshaller83(std::move(testCustomMarshaller83));
	result &= marshalTest.InTestCustomMarshaller84(std::move(testCustomMarshaller84));
	result &= marshalTest.InTestCustomMarshaller85(std::move(testCustomMarshaller85));
#endif
	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestOutPrimitive(IMarshalTest& marshalTest)
{
	MarshalTestData testData;
	bool result = true;

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

	marshalTest.OutTestPrimitive01(testPrimitive01);
	marshalTest.OutTestPrimitive02(testPrimitive02);
	marshalTest.OutTestPrimitive03(testPrimitive03);
	marshalTest.OutTestPrimitive04(testPrimitive04);
	marshalTest.OutTestPrimitive05(testPrimitive05);
	marshalTest.OutTestPrimitive06(testPrimitive06);
	marshalTest.OutTestPrimitive07(testPrimitive07);
	marshalTest.OutTestPrimitive08(testPrimitive08);
	marshalTest.OutTestPrimitive09(testPrimitive09);
	marshalTest.OutTestPrimitive10(testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestPrimitive20(testPrimitive20);
	marshalTest.OutTestPrimitive21(testPrimitive21);
	marshalTest.OutTestPrimitive22(testPrimitive22);
	marshalTest.OutTestPrimitive23(testPrimitive23);
	marshalTest.OutTestPrimitive24(testPrimitive24);
	marshalTest.OutTestPrimitive25(testPrimitive25);
#endif
	marshalTest.OutTestPrimitive30(testPrimitive30);
	marshalTest.OutTestPrimitive31(testPrimitive31);
	marshalTest.OutTestPrimitive32(testPrimitive32);
	marshalTest.OutTestPrimitive33(testPrimitive33);
	marshalTest.OutTestPrimitive34(testPrimitive34);
	marshalTest.OutTestPrimitive61(testPrimitive61);
	marshalTest.OutTestPrimitive62(testPrimitive62);
	marshalTest.OutTestPrimitive63(testPrimitive63);
	marshalTest.OutTestPrimitive64(testPrimitive64);
	marshalTest.OutTestPrimitive65(testPrimitive65);
	marshalTest.OutTestPrimitive66(testPrimitive66);
	marshalTest.OutTestPrimitive67(testPrimitive67);
	marshalTest.OutTestPrimitive68(testPrimitive68);
	marshalTest.OutTestPrimitive69(testPrimitive69);
	marshalTest.OutTestPrimitive70(testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestPrimitive80(testPrimitive80);
	marshalTest.OutTestPrimitive81(testPrimitive81);
	marshalTest.OutTestPrimitive82(testPrimitive82);
	marshalTest.OutTestPrimitive83(testPrimitive83);
	marshalTest.OutTestPrimitive84(testPrimitive84);
	marshalTest.OutTestPrimitive85(testPrimitive85);
#endif

	result &= CompareTestData(testPrimitive01, testData.testPrimitive01);
	result &= CompareTestData(testPrimitive02, testData.testPrimitive02);
	result &= CompareTestData(testPrimitive03, testData.testPrimitive03);
	result &= CompareTestData(testPrimitive04, testData.testPrimitive04);
	result &= CompareTestData(testPrimitive05, testData.testPrimitive05);
	result &= CompareTestData(testPrimitive06, testData.testPrimitive06);
	result &= CompareTestData(testPrimitive07, testData.testPrimitive07);
	result &= CompareTestData(testPrimitive08, testData.testPrimitive08);
	result &= CompareTestData(testPrimitive09, testData.testPrimitive09);
	result &= CompareTestData(testPrimitive10, testData.testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive20, testData.testPrimitive20);
	result &= CompareTestData(testPrimitive21, testData.testPrimitive21);
	result &= CompareTestData(testPrimitive22, testData.testPrimitive22);
	result &= CompareTestData(testPrimitive23, testData.testPrimitive23);
	result &= CompareTestData(testPrimitive24, testData.testPrimitive24);
	result &= CompareTestData(testPrimitive25, testData.testPrimitive25);
#endif
	result &= CompareTestData(testPrimitive30, testData.testPrimitive30);
	result &= CompareTestData(testPrimitive31, testData.testPrimitive31);
	result &= CompareTestData(testPrimitive32, testData.testPrimitive32);
	result &= CompareTestData(testPrimitive33, testData.testPrimitive33);
	result &= CompareTestData(testPrimitive34, testData.testPrimitive34);
	result &= CompareTestData(testPrimitive61, testData.testPrimitive61);
	result &= CompareTestData(testPrimitive62, testData.testPrimitive62);
	result &= CompareTestData(testPrimitive63, testData.testPrimitive63);
	result &= CompareTestData(testPrimitive64, testData.testPrimitive64);
	result &= CompareTestData(testPrimitive65, testData.testPrimitive65);
	result &= CompareTestData(testPrimitive66, testData.testPrimitive66);
	result &= CompareTestData(testPrimitive67, testData.testPrimitive67);
	result &= CompareTestData(testPrimitive68, testData.testPrimitive68);
	result &= CompareTestData(testPrimitive69, testData.testPrimitive69);
	result &= CompareTestData(testPrimitive70, testData.testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive80, testData.testPrimitive80);
	result &= CompareTestData(testPrimitive81, testData.testPrimitive81);
	result &= CompareTestData(testPrimitive82, testData.testPrimitive82);
	result &= CompareTestData(testPrimitive83, testData.testPrimitive83);
	result &= CompareTestData(testPrimitive84, testData.testPrimitive84);
	result &= CompareTestData(testPrimitive85, testData.testPrimitive85);
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestPrimitiveWithMove01(testPrimitive01);
	marshalTest.OutTestPrimitiveWithMove02(testPrimitive02);
	marshalTest.OutTestPrimitiveWithMove03(testPrimitive03);
	marshalTest.OutTestPrimitiveWithMove04(testPrimitive04);
	marshalTest.OutTestPrimitiveWithMove05(testPrimitive05);
	marshalTest.OutTestPrimitiveWithMove06(testPrimitive06);
	marshalTest.OutTestPrimitiveWithMove07(testPrimitive07);
	marshalTest.OutTestPrimitiveWithMove08(testPrimitive08);
	marshalTest.OutTestPrimitiveWithMove09(testPrimitive09);
	marshalTest.OutTestPrimitiveWithMove10(testPrimitive10);
	marshalTest.OutTestPrimitiveWithMove20(testPrimitive20);
	marshalTest.OutTestPrimitiveWithMove21(testPrimitive21);
	marshalTest.OutTestPrimitiveWithMove22(testPrimitive22);
	marshalTest.OutTestPrimitiveWithMove23(testPrimitive23);
	marshalTest.OutTestPrimitiveWithMove24(testPrimitive24);
	marshalTest.OutTestPrimitiveWithMove25(testPrimitive25);
	marshalTest.OutTestPrimitiveWithMove30(testPrimitive30);
	marshalTest.OutTestPrimitiveWithMove31(testPrimitive31);
	marshalTest.OutTestPrimitiveWithMove32(testPrimitive32);
	marshalTest.OutTestPrimitiveWithMove33(testPrimitive33);
	marshalTest.OutTestPrimitiveWithMove34(testPrimitive34);
	marshalTest.OutTestPrimitiveWithMove61(testPrimitive61);
	marshalTest.OutTestPrimitiveWithMove62(testPrimitive62);
	marshalTest.OutTestPrimitiveWithMove63(testPrimitive63);
	marshalTest.OutTestPrimitiveWithMove64(testPrimitive64);
	marshalTest.OutTestPrimitiveWithMove65(testPrimitive65);
	marshalTest.OutTestPrimitiveWithMove66(testPrimitive66);
	marshalTest.OutTestPrimitiveWithMove67(testPrimitive67);
	marshalTest.OutTestPrimitiveWithMove68(testPrimitive68);
	marshalTest.OutTestPrimitiveWithMove69(testPrimitive69);
	marshalTest.OutTestPrimitiveWithMove70(testPrimitive70);
	marshalTest.OutTestPrimitiveWithMove80(testPrimitive80);
	marshalTest.OutTestPrimitiveWithMove81(testPrimitive81);
	marshalTest.OutTestPrimitiveWithMove82(testPrimitive82);
	marshalTest.OutTestPrimitiveWithMove83(testPrimitive83);
	marshalTest.OutTestPrimitiveWithMove84(testPrimitive84);
	marshalTest.OutTestPrimitiveWithMove85(testPrimitive85);

	result &= CompareTestData(testPrimitive01, testData.testPrimitive01);
	result &= CompareTestData(testPrimitive02, testData.testPrimitive02);
	result &= CompareTestData(testPrimitive03, testData.testPrimitive03);
	result &= CompareTestData(testPrimitive04, testData.testPrimitive04);
	result &= CompareTestData(testPrimitive05, testData.testPrimitive05);
	result &= CompareTestData(testPrimitive06, testData.testPrimitive06);
	result &= CompareTestData(testPrimitive07, testData.testPrimitive07);
	result &= CompareTestData(testPrimitive08, testData.testPrimitive08);
	result &= CompareTestData(testPrimitive09, testData.testPrimitive09);
	result &= CompareTestData(testPrimitive10, testData.testPrimitive10);
	result &= CompareTestData(testPrimitive20, testData.testPrimitive20);
	result &= CompareTestData(testPrimitive21, testData.testPrimitive21);
	result &= CompareTestData(testPrimitive22, testData.testPrimitive22);
	result &= CompareTestData(testPrimitive23, testData.testPrimitive23);
	result &= CompareTestData(testPrimitive24, testData.testPrimitive24);
	result &= CompareTestData(testPrimitive25, testData.testPrimitive25);
	result &= CompareTestData(testPrimitive30, testData.testPrimitive30);
	result &= CompareTestData(testPrimitive31, testData.testPrimitive31);
	result &= CompareTestData(testPrimitive32, testData.testPrimitive32);
	result &= CompareTestData(testPrimitive33, testData.testPrimitive33);
	result &= CompareTestData(testPrimitive34, testData.testPrimitive34);
	result &= CompareTestData(testPrimitive61, testData.testPrimitive61);
	result &= CompareTestData(testPrimitive62, testData.testPrimitive62);
	result &= CompareTestData(testPrimitive63, testData.testPrimitive63);
	result &= CompareTestData(testPrimitive64, testData.testPrimitive64);
	result &= CompareTestData(testPrimitive65, testData.testPrimitive65);
	result &= CompareTestData(testPrimitive66, testData.testPrimitive66);
	result &= CompareTestData(testPrimitive67, testData.testPrimitive67);
	result &= CompareTestData(testPrimitive68, testData.testPrimitive68);
	result &= CompareTestData(testPrimitive69, testData.testPrimitive69);
	result &= CompareTestData(testPrimitive70, testData.testPrimitive70);
	result &= CompareTestData(testPrimitive80, testData.testPrimitive80);
	result &= CompareTestData(testPrimitive81, testData.testPrimitive81);
	result &= CompareTestData(testPrimitive82, testData.testPrimitive82);
	result &= CompareTestData(testPrimitive83, testData.testPrimitive83);
	result &= CompareTestData(testPrimitive84, testData.testPrimitive84);
	result &= CompareTestData(testPrimitive85, testData.testPrimitive85);
#endif

	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestOutComplex(IMarshalTest& marshalTest)
{
	MarshalTestData testData;
	bool result = true;

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
	CustomMarshalObject testCustomMarshaller30(42);
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
	std::array<std::vector<CustomMarshalObject>, 10> testCustomMarshaller80;
	std::forward_list<std::vector<CustomMarshalObject>> testCustomMarshaller81;
	std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller82;
	std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller83;
	std::unordered_map<int, std::vector<CustomMarshalObject>> testCustomMarshaller84;
	std::unordered_multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller85;
#endif

	marshalTest.OutTestCustomMarshaller01(testCustomMarshaller01);
	marshalTest.OutTestCustomMarshaller02(testCustomMarshaller02);
	marshalTest.OutTestCustomMarshaller03(testCustomMarshaller03);
	marshalTest.OutTestCustomMarshaller04(testCustomMarshaller04);
	marshalTest.OutTestCustomMarshaller05(testCustomMarshaller05);
	marshalTest.OutTestCustomMarshaller06(testCustomMarshaller06);
	marshalTest.OutTestCustomMarshaller07(testCustomMarshaller07);
	marshalTest.OutTestCustomMarshaller08(testCustomMarshaller08);
	marshalTest.OutTestCustomMarshaller09(testCustomMarshaller09);
	marshalTest.OutTestCustomMarshaller10(testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestCustomMarshaller20(testCustomMarshaller20);
	marshalTest.OutTestCustomMarshaller21(testCustomMarshaller21);
	marshalTest.OutTestCustomMarshaller22(testCustomMarshaller22);
	marshalTest.OutTestCustomMarshaller23(testCustomMarshaller23);
	marshalTest.OutTestCustomMarshaller24(testCustomMarshaller24);
	marshalTest.OutTestCustomMarshaller25(testCustomMarshaller25);
#endif
	marshalTest.OutTestCustomMarshaller30(testCustomMarshaller30);
	marshalTest.OutTestCustomMarshaller31(testCustomMarshaller31);
	marshalTest.OutTestCustomMarshaller32(testCustomMarshaller32);
	marshalTest.OutTestCustomMarshaller61(testCustomMarshaller61);
	marshalTest.OutTestCustomMarshaller62(testCustomMarshaller62);
	marshalTest.OutTestCustomMarshaller63(testCustomMarshaller63);
	marshalTest.OutTestCustomMarshaller64(testCustomMarshaller64);
	marshalTest.OutTestCustomMarshaller65(testCustomMarshaller65);
	marshalTest.OutTestCustomMarshaller66(testCustomMarshaller66);
	marshalTest.OutTestCustomMarshaller67(testCustomMarshaller67);
	marshalTest.OutTestCustomMarshaller68(testCustomMarshaller68);
	marshalTest.OutTestCustomMarshaller69(testCustomMarshaller69);
	marshalTest.OutTestCustomMarshaller70(testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestCustomMarshaller80(testCustomMarshaller80);
	marshalTest.OutTestCustomMarshaller81(testCustomMarshaller81);
	marshalTest.OutTestCustomMarshaller82(testCustomMarshaller82);
	marshalTest.OutTestCustomMarshaller83(testCustomMarshaller83);
	marshalTest.OutTestCustomMarshaller84(testCustomMarshaller84);
	marshalTest.OutTestCustomMarshaller85(testCustomMarshaller85);
#endif

	result &= CompareTestData(testCustomMarshaller01, testData.testCustomMarshaller01);
	result &= CompareTestData(testCustomMarshaller02, testData.testCustomMarshaller02);
	result &= CompareTestData(testCustomMarshaller03, testData.testCustomMarshaller03);
	result &= CompareTestData(testCustomMarshaller04, testData.testCustomMarshaller04);
	result &= CompareTestData(testCustomMarshaller05, testData.testCustomMarshaller05);
	result &= CompareTestData(testCustomMarshaller06, testData.testCustomMarshaller06);
	result &= CompareTestData(testCustomMarshaller07, testData.testCustomMarshaller07);
	result &= CompareTestData(testCustomMarshaller08, testData.testCustomMarshaller08);
	result &= CompareTestData(testCustomMarshaller09, testData.testCustomMarshaller09);
	result &= CompareTestData(testCustomMarshaller10, testData.testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller20, testData.testCustomMarshaller20);
	result &= CompareTestData(testCustomMarshaller21, testData.testCustomMarshaller21);
	result &= CompareTestData(testCustomMarshaller22, testData.testCustomMarshaller22);
	result &= CompareTestData(testCustomMarshaller23, testData.testCustomMarshaller23);
	result &= CompareTestData(testCustomMarshaller24, testData.testCustomMarshaller24);
	result &= CompareTestData(testCustomMarshaller25, testData.testCustomMarshaller25);
#endif
	result &= CompareTestData(testCustomMarshaller30, testData.testCustomMarshaller30);
	result &= CompareTestData(testCustomMarshaller31, testData.testCustomMarshaller31);
	result &= CompareTestData(testCustomMarshaller32, testData.testCustomMarshaller32);
	result &= CompareTestData(testCustomMarshaller61, testData.testCustomMarshaller61);
	result &= CompareTestData(testCustomMarshaller62, testData.testCustomMarshaller62);
	result &= CompareTestData(testCustomMarshaller63, testData.testCustomMarshaller63);
	result &= CompareTestData(testCustomMarshaller64, testData.testCustomMarshaller64);
	result &= CompareTestData(testCustomMarshaller65, testData.testCustomMarshaller65);
	result &= CompareTestData(testCustomMarshaller66, testData.testCustomMarshaller66);
	result &= CompareTestData(testCustomMarshaller67, testData.testCustomMarshaller67);
	result &= CompareTestData(testCustomMarshaller68, testData.testCustomMarshaller68);
	result &= CompareTestData(testCustomMarshaller69, testData.testCustomMarshaller69);
	result &= CompareTestData(testCustomMarshaller70, testData.testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller80, testData.testCustomMarshaller80);
	result &= CompareTestData(testCustomMarshaller81, testData.testCustomMarshaller81);
	result &= CompareTestData(testCustomMarshaller82, testData.testCustomMarshaller82);
	result &= CompareTestData(testCustomMarshaller83, testData.testCustomMarshaller83);
	result &= CompareTestData(testCustomMarshaller84, testData.testCustomMarshaller84);
	result &= CompareTestData(testCustomMarshaller85, testData.testCustomMarshaller85);
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	marshalTest.OutTestCustomMarshallerWithMove01(testCustomMarshaller01);
	marshalTest.OutTestCustomMarshallerWithMove02(testCustomMarshaller02);
	marshalTest.OutTestCustomMarshallerWithMove03(testCustomMarshaller03);
	marshalTest.OutTestCustomMarshallerWithMove04(testCustomMarshaller04);
	marshalTest.OutTestCustomMarshallerWithMove05(testCustomMarshaller05);
	marshalTest.OutTestCustomMarshallerWithMove06(testCustomMarshaller06);
	marshalTest.OutTestCustomMarshallerWithMove07(testCustomMarshaller07);
	marshalTest.OutTestCustomMarshallerWithMove08(testCustomMarshaller08);
	marshalTest.OutTestCustomMarshallerWithMove09(testCustomMarshaller09);
	marshalTest.OutTestCustomMarshallerWithMove10(testCustomMarshaller10);
	marshalTest.OutTestCustomMarshallerWithMove20(testCustomMarshaller20);
	marshalTest.OutTestCustomMarshallerWithMove21(testCustomMarshaller21);
	marshalTest.OutTestCustomMarshallerWithMove22(testCustomMarshaller22);
	marshalTest.OutTestCustomMarshallerWithMove23(testCustomMarshaller23);
	marshalTest.OutTestCustomMarshallerWithMove24(testCustomMarshaller24);
	marshalTest.OutTestCustomMarshallerWithMove25(testCustomMarshaller25);
	marshalTest.OutTestCustomMarshallerWithMove30(testCustomMarshaller30);
	marshalTest.OutTestCustomMarshallerWithMove31(testCustomMarshaller31);
	marshalTest.OutTestCustomMarshallerWithMove32(testCustomMarshaller32);
	marshalTest.OutTestCustomMarshallerWithMove61(testCustomMarshaller61);
	marshalTest.OutTestCustomMarshallerWithMove62(testCustomMarshaller62);
	marshalTest.OutTestCustomMarshallerWithMove63(testCustomMarshaller63);
	marshalTest.OutTestCustomMarshallerWithMove64(testCustomMarshaller64);
	marshalTest.OutTestCustomMarshallerWithMove65(testCustomMarshaller65);
	marshalTest.OutTestCustomMarshallerWithMove66(testCustomMarshaller66);
	marshalTest.OutTestCustomMarshallerWithMove67(testCustomMarshaller67);
	marshalTest.OutTestCustomMarshallerWithMove68(testCustomMarshaller68);
	marshalTest.OutTestCustomMarshallerWithMove69(testCustomMarshaller69);
	marshalTest.OutTestCustomMarshallerWithMove70(testCustomMarshaller70);
	marshalTest.OutTestCustomMarshallerWithMove80(testCustomMarshaller80);
	marshalTest.OutTestCustomMarshallerWithMove81(testCustomMarshaller81);
	marshalTest.OutTestCustomMarshallerWithMove82(testCustomMarshaller82);
	marshalTest.OutTestCustomMarshallerWithMove83(testCustomMarshaller83);
	marshalTest.OutTestCustomMarshallerWithMove84(testCustomMarshaller84);
	marshalTest.OutTestCustomMarshallerWithMove85(testCustomMarshaller85);

	result &= CompareTestData(testCustomMarshaller01, testData.testCustomMarshaller01);
	result &= CompareTestData(testCustomMarshaller02, testData.testCustomMarshaller02);
	result &= CompareTestData(testCustomMarshaller03, testData.testCustomMarshaller03);
	result &= CompareTestData(testCustomMarshaller04, testData.testCustomMarshaller04);
	result &= CompareTestData(testCustomMarshaller05, testData.testCustomMarshaller05);
	result &= CompareTestData(testCustomMarshaller06, testData.testCustomMarshaller06);
	result &= CompareTestData(testCustomMarshaller07, testData.testCustomMarshaller07);
	result &= CompareTestData(testCustomMarshaller08, testData.testCustomMarshaller08);
	result &= CompareTestData(testCustomMarshaller09, testData.testCustomMarshaller09);
	result &= CompareTestData(testCustomMarshaller10, testData.testCustomMarshaller10);
	result &= CompareTestData(testCustomMarshaller20, testData.testCustomMarshaller20);
	result &= CompareTestData(testCustomMarshaller21, testData.testCustomMarshaller21);
	result &= CompareTestData(testCustomMarshaller22, testData.testCustomMarshaller22);
	result &= CompareTestData(testCustomMarshaller23, testData.testCustomMarshaller23);
	result &= CompareTestData(testCustomMarshaller24, testData.testCustomMarshaller24);
	result &= CompareTestData(testCustomMarshaller25, testData.testCustomMarshaller25);
	result &= CompareTestData(testCustomMarshaller30, testData.testCustomMarshaller30);
	result &= CompareTestData(testCustomMarshaller31, testData.testCustomMarshaller31);
	result &= CompareTestData(testCustomMarshaller32, testData.testCustomMarshaller32);
	result &= CompareTestData(testCustomMarshaller61, testData.testCustomMarshaller61);
	result &= CompareTestData(testCustomMarshaller62, testData.testCustomMarshaller62);
	result &= CompareTestData(testCustomMarshaller63, testData.testCustomMarshaller63);
	result &= CompareTestData(testCustomMarshaller64, testData.testCustomMarshaller64);
	result &= CompareTestData(testCustomMarshaller65, testData.testCustomMarshaller65);
	result &= CompareTestData(testCustomMarshaller66, testData.testCustomMarshaller66);
	result &= CompareTestData(testCustomMarshaller67, testData.testCustomMarshaller67);
	result &= CompareTestData(testCustomMarshaller68, testData.testCustomMarshaller68);
	result &= CompareTestData(testCustomMarshaller69, testData.testCustomMarshaller69);
	result &= CompareTestData(testCustomMarshaller70, testData.testCustomMarshaller70);
	result &= CompareTestData(testCustomMarshaller80, testData.testCustomMarshaller80);
	result &= CompareTestData(testCustomMarshaller81, testData.testCustomMarshaller81);
	result &= CompareTestData(testCustomMarshaller82, testData.testCustomMarshaller82);
	result &= CompareTestData(testCustomMarshaller83, testData.testCustomMarshaller83);
	result &= CompareTestData(testCustomMarshaller84, testData.testCustomMarshaller84);
	result &= CompareTestData(testCustomMarshaller85, testData.testCustomMarshaller85);
#endif

	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestInOutPrimitive(IMarshalTest& marshalTest)
{
	MarshalTestData testData;

	//InOut tests
	std::vector<int> testPrimitive01 = testData.testPrimitive01;
	std::list<int> testPrimitive02 = testData.testPrimitive02;
	std::deque<int> testPrimitive03 = testData.testPrimitive03;
	std::set<int> testPrimitive04 = testData.testPrimitive04;
	std::multiset<int> testPrimitive05 = testData.testPrimitive05;
	std::map<int, int> testPrimitive06 = testData.testPrimitive06;
	std::multimap<int, int> testPrimitive07 = testData.testPrimitive07;
	std::stack<int> testPrimitive08 = testData.testPrimitive08;
	std::queue<int> testPrimitive09 = testData.testPrimitive09;
	std::priority_queue<int> testPrimitive10 = testData.testPrimitive10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<int, 10> testPrimitive20 = testData.testPrimitive20;
	std::forward_list<int> testPrimitive21 = testData.testPrimitive21;
	std::unordered_set<int> testPrimitive22 = testData.testPrimitive22;
	std::unordered_multiset<int> testPrimitive23 = testData.testPrimitive23;
	std::unordered_map<int, int> testPrimitive24 = testData.testPrimitive24;
	std::unordered_multimap<int, int> testPrimitive25 = testData.testPrimitive25;
#endif
	int testPrimitive30 = testData.testPrimitive30;
	std::vector<std::list<std::set<int>>> testPrimitive31 = testData.testPrimitive31;
	std::map<int, std::multimap<std::string, int>> testPrimitive32 = testData.testPrimitive32;
	std::string testPrimitive33 = testData.testPrimitive33;
	std::wstring testPrimitive34 = testData.testPrimitive34;
	std::vector<std::vector<int>> testPrimitive61 = testData.testPrimitive61;
	std::list<std::vector<int>> testPrimitive62 = testData.testPrimitive62;
	std::deque<std::vector<int>> testPrimitive63 = testData.testPrimitive63;
	std::set<std::vector<int>> testPrimitive64 = testData.testPrimitive64;
	std::multiset<std::vector<int>> testPrimitive65 = testData.testPrimitive65;
	std::map<int, std::vector<int>> testPrimitive66 = testData.testPrimitive66;
	std::multimap<int, std::vector<int>> testPrimitive67 = testData.testPrimitive67;
	std::stack<std::vector<int>> testPrimitive68 = testData.testPrimitive68;
	std::queue<std::vector<int>> testPrimitive69 = testData.testPrimitive69;
	std::priority_queue<std::vector<int>> testPrimitive70 = testData.testPrimitive70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<int>, 10> testPrimitive80 = testData.testPrimitive80;
	std::forward_list<std::vector<int>> testPrimitive81 = testData.testPrimitive81;
	std::unordered_set<std::vector<int>, VectorHasher<int>> testPrimitive82 = testData.testPrimitive82;
	std::unordered_multiset<std::vector<int>, VectorHasher<int>> testPrimitive83 = testData.testPrimitive83;
	std::unordered_map<int, std::vector<int>> testPrimitive84 = testData.testPrimitive84;
	std::unordered_multimap<int, std::vector<int>> testPrimitive85 = testData.testPrimitive85;
#endif

	bool result = true;
	result &= marshalTest.InOutTestPrimitive01(testPrimitive01);
	result &= marshalTest.InOutTestPrimitive02(testPrimitive02);
	result &= marshalTest.InOutTestPrimitive03(testPrimitive03);
	result &= marshalTest.InOutTestPrimitive04(testPrimitive04);
	result &= marshalTest.InOutTestPrimitive05(testPrimitive05);
	result &= marshalTest.InOutTestPrimitive06(testPrimitive06);
	result &= marshalTest.InOutTestPrimitive07(testPrimitive07);
	result &= marshalTest.InOutTestPrimitive08(testPrimitive08);
	result &= marshalTest.InOutTestPrimitive09(testPrimitive09);
	result &= marshalTest.InOutTestPrimitive10(testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InOutTestPrimitive20(testPrimitive20);
	result &= marshalTest.InOutTestPrimitive21(testPrimitive21);
	result &= marshalTest.InOutTestPrimitive22(testPrimitive22);
	result &= marshalTest.InOutTestPrimitive23(testPrimitive23);
	result &= marshalTest.InOutTestPrimitive24(testPrimitive24);
	result &= marshalTest.InOutTestPrimitive25(testPrimitive25);
#endif
	result &= marshalTest.InOutTestPrimitive30(testPrimitive30);
	result &= marshalTest.InOutTestPrimitive31(testPrimitive31);
	result &= marshalTest.InOutTestPrimitive32(testPrimitive32);
	result &= marshalTest.InOutTestPrimitive33(testPrimitive33);
	result &= marshalTest.InOutTestPrimitive34(testPrimitive34);
	result &= marshalTest.InOutTestPrimitive61(testPrimitive61);
	result &= marshalTest.InOutTestPrimitive62(testPrimitive62);
	result &= marshalTest.InOutTestPrimitive63(testPrimitive63);
	result &= marshalTest.InOutTestPrimitive64(testPrimitive64);
	result &= marshalTest.InOutTestPrimitive65(testPrimitive65);
	result &= marshalTest.InOutTestPrimitive66(testPrimitive66);
	result &= marshalTest.InOutTestPrimitive67(testPrimitive67);
	result &= marshalTest.InOutTestPrimitive68(testPrimitive68);
	result &= marshalTest.InOutTestPrimitive69(testPrimitive69);
	result &= marshalTest.InOutTestPrimitive70(testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InOutTestPrimitive80(testPrimitive80);
	result &= marshalTest.InOutTestPrimitive81(testPrimitive81);
	result &= marshalTest.InOutTestPrimitive82(testPrimitive82);
	result &= marshalTest.InOutTestPrimitive83(testPrimitive83);
	result &= marshalTest.InOutTestPrimitive84(testPrimitive84);
	result &= marshalTest.InOutTestPrimitive85(testPrimitive85);
#endif

	result &= CompareTestData(testPrimitive01, testData.testPrimitive01);
	result &= CompareTestData(testPrimitive02, testData.testPrimitive02);
	result &= CompareTestData(testPrimitive03, testData.testPrimitive03);
	result &= CompareTestData(testPrimitive04, testData.testPrimitive04);
	result &= CompareTestData(testPrimitive05, testData.testPrimitive05);
	result &= CompareTestData(testPrimitive06, testData.testPrimitive06);
	result &= CompareTestData(testPrimitive07, testData.testPrimitive07);
	result &= CompareTestData(testPrimitive08, testData.testPrimitive08);
	result &= CompareTestData(testPrimitive09, testData.testPrimitive09);
	result &= CompareTestData(testPrimitive10, testData.testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive20, testData.testPrimitive20);
	result &= CompareTestData(testPrimitive21, testData.testPrimitive21);
	result &= CompareTestData(testPrimitive22, testData.testPrimitive22);
	result &= CompareTestData(testPrimitive23, testData.testPrimitive23);
	result &= CompareTestData(testPrimitive24, testData.testPrimitive24);
	result &= CompareTestData(testPrimitive25, testData.testPrimitive25);
#endif
	result &= CompareTestData(testPrimitive30, testData.testPrimitive30);
	result &= CompareTestData(testPrimitive31, testData.testPrimitive31);
	result &= CompareTestData(testPrimitive32, testData.testPrimitive32);
	result &= CompareTestData(testPrimitive33, testData.testPrimitive33);
	result &= CompareTestData(testPrimitive34, testData.testPrimitive34);
	result &= CompareTestData(testPrimitive61, testData.testPrimitive61);
	result &= CompareTestData(testPrimitive62, testData.testPrimitive62);
	result &= CompareTestData(testPrimitive63, testData.testPrimitive63);
	result &= CompareTestData(testPrimitive64, testData.testPrimitive64);
	result &= CompareTestData(testPrimitive65, testData.testPrimitive65);
	result &= CompareTestData(testPrimitive66, testData.testPrimitive66);
	result &= CompareTestData(testPrimitive67, testData.testPrimitive67);
	result &= CompareTestData(testPrimitive68, testData.testPrimitive68);
	result &= CompareTestData(testPrimitive69, testData.testPrimitive69);
	result &= CompareTestData(testPrimitive70, testData.testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive80, testData.testPrimitive80);
	result &= CompareTestData(testPrimitive81, testData.testPrimitive81);
	result &= CompareTestData(testPrimitive82, testData.testPrimitive82);
	result &= CompareTestData(testPrimitive83, testData.testPrimitive83);
	result &= CompareTestData(testPrimitive84, testData.testPrimitive84);
	result &= CompareTestData(testPrimitive85, testData.testPrimitive85);
#endif

	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestInOutComplex(IMarshalTest& marshalTest)
{
	MarshalTestData testData;

	std::vector<CustomMarshalObject> testCustomMarshaller01 = testData.testCustomMarshaller01;
	std::list<CustomMarshalObject> testCustomMarshaller02 = testData.testCustomMarshaller02;
	std::deque<CustomMarshalObject> testCustomMarshaller03 = testData.testCustomMarshaller03;
	std::set<CustomMarshalObject> testCustomMarshaller04 = testData.testCustomMarshaller04;
	std::multiset<CustomMarshalObject> testCustomMarshaller05 = testData.testCustomMarshaller05;
	std::map<int, CustomMarshalObject> testCustomMarshaller06 = testData.testCustomMarshaller06;
	std::multimap<int, CustomMarshalObject> testCustomMarshaller07 = testData.testCustomMarshaller07;
	std::stack<CustomMarshalObject> testCustomMarshaller08 = testData.testCustomMarshaller08;
	std::queue<CustomMarshalObject> testCustomMarshaller09 = testData.testCustomMarshaller09;
	std::priority_queue<CustomMarshalObject> testCustomMarshaller10 = testData.testCustomMarshaller10;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<CustomMarshalObjectWithDefaultConstructor, 10> testCustomMarshaller20 = testData.testCustomMarshaller20;
	std::forward_list<CustomMarshalObject> testCustomMarshaller21 = testData.testCustomMarshaller21;
	std::unordered_set<CustomMarshalObject> testCustomMarshaller22 = testData.testCustomMarshaller22;
	std::unordered_multiset<CustomMarshalObject> testCustomMarshaller23 = testData.testCustomMarshaller23;
	std::unordered_map<int, CustomMarshalObject> testCustomMarshaller24 = testData.testCustomMarshaller24;
	std::unordered_multimap<int, CustomMarshalObject> testCustomMarshaller25 = testData.testCustomMarshaller25;
#endif
	CustomMarshalObject testCustomMarshaller30 = testData.testCustomMarshaller30;
	std::vector<std::list<std::set<CustomMarshalObject>>> testCustomMarshaller31 = testData.testCustomMarshaller31;
	std::map<int, std::multimap<std::string, CustomMarshalObject>> testCustomMarshaller32 = testData.testCustomMarshaller32;
	std::vector<std::vector<CustomMarshalObject>> testCustomMarshaller61 = testData.testCustomMarshaller61;
	std::list<std::vector<CustomMarshalObject>> testCustomMarshaller62 = testData.testCustomMarshaller62;
	std::deque<std::vector<CustomMarshalObject>> testCustomMarshaller63 = testData.testCustomMarshaller63;
	std::set<std::vector<CustomMarshalObject>> testCustomMarshaller64 = testData.testCustomMarshaller64;
	std::multiset<std::vector<CustomMarshalObject>> testCustomMarshaller65 = testData.testCustomMarshaller65;
	std::map<int, std::vector<CustomMarshalObject>> testCustomMarshaller66 = testData.testCustomMarshaller66;
	std::multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller67 = testData.testCustomMarshaller67;
	std::stack<std::vector<CustomMarshalObject>> testCustomMarshaller68 = testData.testCustomMarshaller68;
	std::queue<std::vector<CustomMarshalObject>> testCustomMarshaller69 = testData.testCustomMarshaller69;
	std::priority_queue<std::vector<CustomMarshalObject>> testCustomMarshaller70 = testData.testCustomMarshaller70;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	std::array<std::vector<CustomMarshalObject>, 10> testCustomMarshaller80 = testData.testCustomMarshaller80;
	std::forward_list<std::vector<CustomMarshalObject>> testCustomMarshaller81 = testData.testCustomMarshaller81;
	std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller82 = testData.testCustomMarshaller82;
	std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller83 = testData.testCustomMarshaller83;
	std::unordered_map<int, std::vector<CustomMarshalObject>> testCustomMarshaller84 = testData.testCustomMarshaller84;
	std::unordered_multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller85 = testData.testCustomMarshaller85;
#endif

	bool result = true;
	result &= marshalTest.InOutTestCustomMarshaller01(testCustomMarshaller01);
	result &= marshalTest.InOutTestCustomMarshaller02(testCustomMarshaller02);
	result &= marshalTest.InOutTestCustomMarshaller03(testCustomMarshaller03);
	result &= marshalTest.InOutTestCustomMarshaller04(testCustomMarshaller04);
	result &= marshalTest.InOutTestCustomMarshaller05(testCustomMarshaller05);
	result &= marshalTest.InOutTestCustomMarshaller06(testCustomMarshaller06);
	result &= marshalTest.InOutTestCustomMarshaller07(testCustomMarshaller07);
	result &= marshalTest.InOutTestCustomMarshaller08(testCustomMarshaller08);
	result &= marshalTest.InOutTestCustomMarshaller09(testCustomMarshaller09);
	result &= marshalTest.InOutTestCustomMarshaller10(testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InOutTestCustomMarshaller20(testCustomMarshaller20);
	result &= marshalTest.InOutTestCustomMarshaller21(testCustomMarshaller21);
	result &= marshalTest.InOutTestCustomMarshaller22(testCustomMarshaller22);
	result &= marshalTest.InOutTestCustomMarshaller23(testCustomMarshaller23);
	result &= marshalTest.InOutTestCustomMarshaller24(testCustomMarshaller24);
	result &= marshalTest.InOutTestCustomMarshaller25(testCustomMarshaller25);
#endif
	result &= marshalTest.InOutTestCustomMarshaller30(testCustomMarshaller30);
	result &= marshalTest.InOutTestCustomMarshaller31(testCustomMarshaller31);
	result &= marshalTest.InOutTestCustomMarshaller32(testCustomMarshaller32);
	result &= marshalTest.InOutTestCustomMarshaller61(testCustomMarshaller61);
	result &= marshalTest.InOutTestCustomMarshaller62(testCustomMarshaller62);
	result &= marshalTest.InOutTestCustomMarshaller63(testCustomMarshaller63);
	result &= marshalTest.InOutTestCustomMarshaller64(testCustomMarshaller64);
	result &= marshalTest.InOutTestCustomMarshaller65(testCustomMarshaller65);
	result &= marshalTest.InOutTestCustomMarshaller66(testCustomMarshaller66);
	result &= marshalTest.InOutTestCustomMarshaller67(testCustomMarshaller67);
	result &= marshalTest.InOutTestCustomMarshaller68(testCustomMarshaller68);
	result &= marshalTest.InOutTestCustomMarshaller69(testCustomMarshaller69);
	result &= marshalTest.InOutTestCustomMarshaller70(testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= marshalTest.InOutTestCustomMarshaller80(testCustomMarshaller80);
	result &= marshalTest.InOutTestCustomMarshaller81(testCustomMarshaller81);
	result &= marshalTest.InOutTestCustomMarshaller82(testCustomMarshaller82);
	result &= marshalTest.InOutTestCustomMarshaller83(testCustomMarshaller83);
	result &= marshalTest.InOutTestCustomMarshaller84(testCustomMarshaller84);
	result &= marshalTest.InOutTestCustomMarshaller85(testCustomMarshaller85);
#endif

	result &= CompareTestData(testCustomMarshaller01, testData.testCustomMarshaller01);
	result &= CompareTestData(testCustomMarshaller02, testData.testCustomMarshaller02);
	result &= CompareTestData(testCustomMarshaller03, testData.testCustomMarshaller03);
	result &= CompareTestData(testCustomMarshaller04, testData.testCustomMarshaller04);
	result &= CompareTestData(testCustomMarshaller05, testData.testCustomMarshaller05);
	result &= CompareTestData(testCustomMarshaller06, testData.testCustomMarshaller06);
	result &= CompareTestData(testCustomMarshaller07, testData.testCustomMarshaller07);
	result &= CompareTestData(testCustomMarshaller08, testData.testCustomMarshaller08);
	result &= CompareTestData(testCustomMarshaller09, testData.testCustomMarshaller09);
	result &= CompareTestData(testCustomMarshaller10, testData.testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller20, testData.testCustomMarshaller20);
	result &= CompareTestData(testCustomMarshaller21, testData.testCustomMarshaller21);
	result &= CompareTestData(testCustomMarshaller22, testData.testCustomMarshaller22);
	result &= CompareTestData(testCustomMarshaller23, testData.testCustomMarshaller23);
	result &= CompareTestData(testCustomMarshaller24, testData.testCustomMarshaller24);
	result &= CompareTestData(testCustomMarshaller25, testData.testCustomMarshaller25);
#endif
	result &= CompareTestData(testCustomMarshaller30, testData.testCustomMarshaller30);
	result &= CompareTestData(testCustomMarshaller31, testData.testCustomMarshaller31);
	result &= CompareTestData(testCustomMarshaller32, testData.testCustomMarshaller32);
	result &= CompareTestData(testCustomMarshaller61, testData.testCustomMarshaller61);
	result &= CompareTestData(testCustomMarshaller62, testData.testCustomMarshaller62);
	result &= CompareTestData(testCustomMarshaller63, testData.testCustomMarshaller63);
	result &= CompareTestData(testCustomMarshaller64, testData.testCustomMarshaller64);
	result &= CompareTestData(testCustomMarshaller65, testData.testCustomMarshaller65);
	result &= CompareTestData(testCustomMarshaller66, testData.testCustomMarshaller66);
	result &= CompareTestData(testCustomMarshaller67, testData.testCustomMarshaller67);
	result &= CompareTestData(testCustomMarshaller68, testData.testCustomMarshaller68);
	result &= CompareTestData(testCustomMarshaller69, testData.testCustomMarshaller69);
	result &= CompareTestData(testCustomMarshaller70, testData.testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller80, testData.testCustomMarshaller80);
	result &= CompareTestData(testCustomMarshaller81, testData.testCustomMarshaller81);
	result &= CompareTestData(testCustomMarshaller82, testData.testCustomMarshaller82);
	result &= CompareTestData(testCustomMarshaller83, testData.testCustomMarshaller83);
	result &= CompareTestData(testCustomMarshaller84, testData.testCustomMarshaller84);
	result &= CompareTestData(testCustomMarshaller85, testData.testCustomMarshaller85);
#endif

	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestRetPrimitive(IMarshalTest& marshalTest)
{
	MarshalTestData testData;
	bool result = true;

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

	testPrimitive01 = marshalTest.RetTestPrimitive01();
	testPrimitive02 = marshalTest.RetTestPrimitive02();
	testPrimitive03 = marshalTest.RetTestPrimitive03();
	testPrimitive04 = marshalTest.RetTestPrimitive04();
	testPrimitive05 = marshalTest.RetTestPrimitive05();
	testPrimitive06 = marshalTest.RetTestPrimitive06();
	testPrimitive07 = marshalTest.RetTestPrimitive07();
	testPrimitive08 = marshalTest.RetTestPrimitive08();
	testPrimitive09 = marshalTest.RetTestPrimitive09();
	testPrimitive10 = marshalTest.RetTestPrimitive10();
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testPrimitive20 = marshalTest.RetTestPrimitive20();
	testPrimitive21 = marshalTest.RetTestPrimitive21();
	testPrimitive22 = marshalTest.RetTestPrimitive22();
	testPrimitive23 = marshalTest.RetTestPrimitive23();
	testPrimitive24 = marshalTest.RetTestPrimitive24();
	testPrimitive25 = marshalTest.RetTestPrimitive25();
#endif
	testPrimitive30 = marshalTest.RetTestPrimitive30();
	testPrimitive31 = marshalTest.RetTestPrimitive31();
	testPrimitive32 = marshalTest.RetTestPrimitive32();
	testPrimitive33 = marshalTest.RetTestPrimitive33();
	testPrimitive34 = marshalTest.RetTestPrimitive34();
	testPrimitive61 = marshalTest.RetTestPrimitive61();
	testPrimitive62 = marshalTest.RetTestPrimitive62();
	testPrimitive63 = marshalTest.RetTestPrimitive63();
	testPrimitive64 = marshalTest.RetTestPrimitive64();
	testPrimitive65 = marshalTest.RetTestPrimitive65();
	testPrimitive66 = marshalTest.RetTestPrimitive66();
	testPrimitive67 = marshalTest.RetTestPrimitive67();
	testPrimitive68 = marshalTest.RetTestPrimitive68();
	testPrimitive69 = marshalTest.RetTestPrimitive69();
	testPrimitive70 = marshalTest.RetTestPrimitive70();
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testPrimitive80 = marshalTest.RetTestPrimitive80();
	testPrimitive81 = marshalTest.RetTestPrimitive81();
	testPrimitive82 = marshalTest.RetTestPrimitive82();
	testPrimitive83 = marshalTest.RetTestPrimitive83();
	testPrimitive84 = marshalTest.RetTestPrimitive84();
	testPrimitive85 = marshalTest.RetTestPrimitive85();
#endif

	result &= CompareTestData(testPrimitive01, testData.testPrimitive01);
	result &= CompareTestData(testPrimitive02, testData.testPrimitive02);
	result &= CompareTestData(testPrimitive03, testData.testPrimitive03);
	result &= CompareTestData(testPrimitive04, testData.testPrimitive04);
	result &= CompareTestData(testPrimitive05, testData.testPrimitive05);
	result &= CompareTestData(testPrimitive06, testData.testPrimitive06);
	result &= CompareTestData(testPrimitive07, testData.testPrimitive07);
	result &= CompareTestData(testPrimitive08, testData.testPrimitive08);
	result &= CompareTestData(testPrimitive09, testData.testPrimitive09);
	result &= CompareTestData(testPrimitive10, testData.testPrimitive10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive20, testData.testPrimitive20);
	result &= CompareTestData(testPrimitive21, testData.testPrimitive21);
	result &= CompareTestData(testPrimitive22, testData.testPrimitive22);
	result &= CompareTestData(testPrimitive23, testData.testPrimitive23);
	result &= CompareTestData(testPrimitive24, testData.testPrimitive24);
	result &= CompareTestData(testPrimitive25, testData.testPrimitive25);
#endif
	result &= CompareTestData(testPrimitive30, testData.testPrimitive30);
	result &= CompareTestData(testPrimitive31, testData.testPrimitive31);
	result &= CompareTestData(testPrimitive32, testData.testPrimitive32);
	result &= CompareTestData(testPrimitive33, testData.testPrimitive33);
	result &= CompareTestData(testPrimitive34, testData.testPrimitive34);
	result &= CompareTestData(testPrimitive61, testData.testPrimitive61);
	result &= CompareTestData(testPrimitive62, testData.testPrimitive62);
	result &= CompareTestData(testPrimitive63, testData.testPrimitive63);
	result &= CompareTestData(testPrimitive64, testData.testPrimitive64);
	result &= CompareTestData(testPrimitive65, testData.testPrimitive65);
	result &= CompareTestData(testPrimitive66, testData.testPrimitive66);
	result &= CompareTestData(testPrimitive67, testData.testPrimitive67);
	result &= CompareTestData(testPrimitive68, testData.testPrimitive68);
	result &= CompareTestData(testPrimitive69, testData.testPrimitive69);
	result &= CompareTestData(testPrimitive70, testData.testPrimitive70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testPrimitive80, testData.testPrimitive80);
	result &= CompareTestData(testPrimitive81, testData.testPrimitive81);
	result &= CompareTestData(testPrimitive82, testData.testPrimitive82);
	result &= CompareTestData(testPrimitive83, testData.testPrimitive83);
	result &= CompareTestData(testPrimitive84, testData.testPrimitive84);
	result &= CompareTestData(testPrimitive85, testData.testPrimitive85);
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testPrimitive01 = marshalTest.RetTestPrimitiveWithMove01();
	testPrimitive02 = marshalTest.RetTestPrimitiveWithMove02();
	testPrimitive03 = marshalTest.RetTestPrimitiveWithMove03();
	testPrimitive04 = marshalTest.RetTestPrimitiveWithMove04();
	testPrimitive05 = marshalTest.RetTestPrimitiveWithMove05();
	testPrimitive06 = marshalTest.RetTestPrimitiveWithMove06();
	testPrimitive07 = marshalTest.RetTestPrimitiveWithMove07();
	testPrimitive08 = marshalTest.RetTestPrimitiveWithMove08();
	testPrimitive09 = marshalTest.RetTestPrimitiveWithMove09();
	testPrimitive10 = marshalTest.RetTestPrimitiveWithMove10();
	testPrimitive20 = marshalTest.RetTestPrimitiveWithMove20();
	testPrimitive21 = marshalTest.RetTestPrimitiveWithMove21();
	testPrimitive22 = marshalTest.RetTestPrimitiveWithMove22();
	testPrimitive23 = marshalTest.RetTestPrimitiveWithMove23();
	testPrimitive24 = marshalTest.RetTestPrimitiveWithMove24();
	testPrimitive25 = marshalTest.RetTestPrimitiveWithMove25();
	testPrimitive30 = marshalTest.RetTestPrimitiveWithMove30();
	testPrimitive31 = marshalTest.RetTestPrimitiveWithMove31();
	testPrimitive32 = marshalTest.RetTestPrimitiveWithMove32();
	testPrimitive33 = marshalTest.RetTestPrimitiveWithMove33();
	testPrimitive34 = marshalTest.RetTestPrimitiveWithMove34();
	testPrimitive61 = marshalTest.RetTestPrimitiveWithMove61();
	testPrimitive62 = marshalTest.RetTestPrimitiveWithMove62();
	testPrimitive63 = marshalTest.RetTestPrimitiveWithMove63();
	testPrimitive64 = marshalTest.RetTestPrimitiveWithMove64();
	testPrimitive65 = marshalTest.RetTestPrimitiveWithMove65();
	testPrimitive66 = marshalTest.RetTestPrimitiveWithMove66();
	testPrimitive67 = marshalTest.RetTestPrimitiveWithMove67();
	testPrimitive68 = marshalTest.RetTestPrimitiveWithMove68();
	testPrimitive69 = marshalTest.RetTestPrimitiveWithMove69();
	testPrimitive70 = marshalTest.RetTestPrimitiveWithMove70();
	testPrimitive80 = marshalTest.RetTestPrimitiveWithMove80();
	testPrimitive81 = marshalTest.RetTestPrimitiveWithMove81();
	testPrimitive82 = marshalTest.RetTestPrimitiveWithMove82();
	testPrimitive83 = marshalTest.RetTestPrimitiveWithMove83();
	testPrimitive84 = marshalTest.RetTestPrimitiveWithMove84();
	testPrimitive85 = marshalTest.RetTestPrimitiveWithMove85();

	result &= CompareTestData(testPrimitive01, testData.testPrimitive01);
	result &= CompareTestData(testPrimitive02, testData.testPrimitive02);
	result &= CompareTestData(testPrimitive03, testData.testPrimitive03);
	result &= CompareTestData(testPrimitive04, testData.testPrimitive04);
	result &= CompareTestData(testPrimitive05, testData.testPrimitive05);
	result &= CompareTestData(testPrimitive06, testData.testPrimitive06);
	result &= CompareTestData(testPrimitive07, testData.testPrimitive07);
	result &= CompareTestData(testPrimitive08, testData.testPrimitive08);
	result &= CompareTestData(testPrimitive09, testData.testPrimitive09);
	result &= CompareTestData(testPrimitive10, testData.testPrimitive10);
	result &= CompareTestData(testPrimitive20, testData.testPrimitive20);
	result &= CompareTestData(testPrimitive21, testData.testPrimitive21);
	result &= CompareTestData(testPrimitive22, testData.testPrimitive22);
	result &= CompareTestData(testPrimitive23, testData.testPrimitive23);
	result &= CompareTestData(testPrimitive24, testData.testPrimitive24);
	result &= CompareTestData(testPrimitive25, testData.testPrimitive25);
	result &= CompareTestData(testPrimitive30, testData.testPrimitive30);
	result &= CompareTestData(testPrimitive31, testData.testPrimitive31);
	result &= CompareTestData(testPrimitive32, testData.testPrimitive32);
	result &= CompareTestData(testPrimitive33, testData.testPrimitive33);
	result &= CompareTestData(testPrimitive34, testData.testPrimitive34);
	result &= CompareTestData(testPrimitive61, testData.testPrimitive61);
	result &= CompareTestData(testPrimitive62, testData.testPrimitive62);
	result &= CompareTestData(testPrimitive63, testData.testPrimitive63);
	result &= CompareTestData(testPrimitive64, testData.testPrimitive64);
	result &= CompareTestData(testPrimitive65, testData.testPrimitive65);
	result &= CompareTestData(testPrimitive66, testData.testPrimitive66);
	result &= CompareTestData(testPrimitive67, testData.testPrimitive67);
	result &= CompareTestData(testPrimitive68, testData.testPrimitive68);
	result &= CompareTestData(testPrimitive69, testData.testPrimitive69);
	result &= CompareTestData(testPrimitive70, testData.testPrimitive70);
	result &= CompareTestData(testPrimitive80, testData.testPrimitive80);
	result &= CompareTestData(testPrimitive81, testData.testPrimitive81);
	result &= CompareTestData(testPrimitive82, testData.testPrimitive82);
	result &= CompareTestData(testPrimitive83, testData.testPrimitive83);
	result &= CompareTestData(testPrimitive84, testData.testPrimitive84);
	result &= CompareTestData(testPrimitive85, testData.testPrimitive85);
#endif

	return result;
}

//----------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) bool MarshalTestRetComplex(IMarshalTest& marshalTest)
{
	MarshalTestData testData;
	bool result = true;

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
	CustomMarshalObject testCustomMarshaller30(42);
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
	std::array<std::vector<CustomMarshalObject>, 10> testCustomMarshaller80;
	std::forward_list<std::vector<CustomMarshalObject>> testCustomMarshaller81;
	std::unordered_set<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller82;
	std::unordered_multiset<std::vector<CustomMarshalObject>, VectorHasher<CustomMarshalObject>> testCustomMarshaller83;
	std::unordered_map<int, std::vector<CustomMarshalObject>> testCustomMarshaller84;
	std::unordered_multimap<int, std::vector<CustomMarshalObject>> testCustomMarshaller85;
#endif

	testCustomMarshaller01 = marshalTest.RetTestCustomMarshaller01();
	testCustomMarshaller02 = marshalTest.RetTestCustomMarshaller02();
	testCustomMarshaller03 = marshalTest.RetTestCustomMarshaller03();
	testCustomMarshaller04 = marshalTest.RetTestCustomMarshaller04();
	testCustomMarshaller05 = marshalTest.RetTestCustomMarshaller05();
	testCustomMarshaller06 = marshalTest.RetTestCustomMarshaller06();
	testCustomMarshaller07 = marshalTest.RetTestCustomMarshaller07();
	testCustomMarshaller08 = marshalTest.RetTestCustomMarshaller08();
	testCustomMarshaller09 = marshalTest.RetTestCustomMarshaller09();
	testCustomMarshaller10 = marshalTest.RetTestCustomMarshaller10();
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testCustomMarshaller20 = marshalTest.RetTestCustomMarshaller20();
	testCustomMarshaller21 = marshalTest.RetTestCustomMarshaller21();
	testCustomMarshaller22 = marshalTest.RetTestCustomMarshaller22();
	testCustomMarshaller23 = marshalTest.RetTestCustomMarshaller23();
	testCustomMarshaller24 = marshalTest.RetTestCustomMarshaller24();
	testCustomMarshaller25 = marshalTest.RetTestCustomMarshaller25();
#endif
	testCustomMarshaller30 = marshalTest.RetTestCustomMarshaller30();
	testCustomMarshaller31 = marshalTest.RetTestCustomMarshaller31();
	testCustomMarshaller32 = marshalTest.RetTestCustomMarshaller32();
	testCustomMarshaller61 = marshalTest.RetTestCustomMarshaller61();
	testCustomMarshaller62 = marshalTest.RetTestCustomMarshaller62();
	testCustomMarshaller63 = marshalTest.RetTestCustomMarshaller63();
	testCustomMarshaller64 = marshalTest.RetTestCustomMarshaller64();
	testCustomMarshaller65 = marshalTest.RetTestCustomMarshaller65();
	testCustomMarshaller66 = marshalTest.RetTestCustomMarshaller66();
	testCustomMarshaller67 = marshalTest.RetTestCustomMarshaller67();
	testCustomMarshaller68 = marshalTest.RetTestCustomMarshaller68();
	testCustomMarshaller69 = marshalTest.RetTestCustomMarshaller69();
	testCustomMarshaller70 = marshalTest.RetTestCustomMarshaller70();
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testCustomMarshaller80 = marshalTest.RetTestCustomMarshaller80();
	testCustomMarshaller81 = marshalTest.RetTestCustomMarshaller81();
	testCustomMarshaller82 = marshalTest.RetTestCustomMarshaller82();
	testCustomMarshaller83 = marshalTest.RetTestCustomMarshaller83();
	testCustomMarshaller84 = marshalTest.RetTestCustomMarshaller84();
	testCustomMarshaller85 = marshalTest.RetTestCustomMarshaller85();
#endif

	result &= CompareTestData(testCustomMarshaller01, testData.testCustomMarshaller01);
	result &= CompareTestData(testCustomMarshaller02, testData.testCustomMarshaller02);
	result &= CompareTestData(testCustomMarshaller03, testData.testCustomMarshaller03);
	result &= CompareTestData(testCustomMarshaller04, testData.testCustomMarshaller04);
	result &= CompareTestData(testCustomMarshaller05, testData.testCustomMarshaller05);
	result &= CompareTestData(testCustomMarshaller06, testData.testCustomMarshaller06);
	result &= CompareTestData(testCustomMarshaller07, testData.testCustomMarshaller07);
	result &= CompareTestData(testCustomMarshaller08, testData.testCustomMarshaller08);
	result &= CompareTestData(testCustomMarshaller09, testData.testCustomMarshaller09);
	result &= CompareTestData(testCustomMarshaller10, testData.testCustomMarshaller10);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller20, testData.testCustomMarshaller20);
	result &= CompareTestData(testCustomMarshaller21, testData.testCustomMarshaller21);
	result &= CompareTestData(testCustomMarshaller22, testData.testCustomMarshaller22);
	result &= CompareTestData(testCustomMarshaller23, testData.testCustomMarshaller23);
	result &= CompareTestData(testCustomMarshaller24, testData.testCustomMarshaller24);
	result &= CompareTestData(testCustomMarshaller25, testData.testCustomMarshaller25);
#endif
	result &= CompareTestData(testCustomMarshaller30, testData.testCustomMarshaller30);
	result &= CompareTestData(testCustomMarshaller31, testData.testCustomMarshaller31);
	result &= CompareTestData(testCustomMarshaller32, testData.testCustomMarshaller32);
	result &= CompareTestData(testCustomMarshaller61, testData.testCustomMarshaller61);
	result &= CompareTestData(testCustomMarshaller62, testData.testCustomMarshaller62);
	result &= CompareTestData(testCustomMarshaller63, testData.testCustomMarshaller63);
	result &= CompareTestData(testCustomMarshaller64, testData.testCustomMarshaller64);
	result &= CompareTestData(testCustomMarshaller65, testData.testCustomMarshaller65);
	result &= CompareTestData(testCustomMarshaller66, testData.testCustomMarshaller66);
	result &= CompareTestData(testCustomMarshaller67, testData.testCustomMarshaller67);
	result &= CompareTestData(testCustomMarshaller68, testData.testCustomMarshaller68);
	result &= CompareTestData(testCustomMarshaller69, testData.testCustomMarshaller69);
	result &= CompareTestData(testCustomMarshaller70, testData.testCustomMarshaller70);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	result &= CompareTestData(testCustomMarshaller80, testData.testCustomMarshaller80);
	result &= CompareTestData(testCustomMarshaller81, testData.testCustomMarshaller81);
	result &= CompareTestData(testCustomMarshaller82, testData.testCustomMarshaller82);
	result &= CompareTestData(testCustomMarshaller83, testData.testCustomMarshaller83);
	result &= CompareTestData(testCustomMarshaller84, testData.testCustomMarshaller84);
	result &= CompareTestData(testCustomMarshaller85, testData.testCustomMarshaller85);
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	testCustomMarshaller01 = marshalTest.RetTestCustomMarshallerWithMove01();
	testCustomMarshaller02 = marshalTest.RetTestCustomMarshallerWithMove02();
	testCustomMarshaller03 = marshalTest.RetTestCustomMarshallerWithMove03();
	testCustomMarshaller04 = marshalTest.RetTestCustomMarshallerWithMove04();
	testCustomMarshaller05 = marshalTest.RetTestCustomMarshallerWithMove05();
	testCustomMarshaller06 = marshalTest.RetTestCustomMarshallerWithMove06();
	testCustomMarshaller07 = marshalTest.RetTestCustomMarshallerWithMove07();
	testCustomMarshaller08 = marshalTest.RetTestCustomMarshallerWithMove08();
	testCustomMarshaller09 = marshalTest.RetTestCustomMarshallerWithMove09();
	testCustomMarshaller10 = marshalTest.RetTestCustomMarshallerWithMove10();
	testCustomMarshaller20 = marshalTest.RetTestCustomMarshallerWithMove20();
	testCustomMarshaller21 = marshalTest.RetTestCustomMarshallerWithMove21();
	testCustomMarshaller22 = marshalTest.RetTestCustomMarshallerWithMove22();
	testCustomMarshaller23 = marshalTest.RetTestCustomMarshallerWithMove23();
	testCustomMarshaller24 = marshalTest.RetTestCustomMarshallerWithMove24();
	testCustomMarshaller25 = marshalTest.RetTestCustomMarshallerWithMove25();
	testCustomMarshaller30 = marshalTest.RetTestCustomMarshallerWithMove30();
	testCustomMarshaller31 = marshalTest.RetTestCustomMarshallerWithMove31();
	testCustomMarshaller32 = marshalTest.RetTestCustomMarshallerWithMove32();
	testCustomMarshaller61 = marshalTest.RetTestCustomMarshallerWithMove61();
	testCustomMarshaller62 = marshalTest.RetTestCustomMarshallerWithMove62();
	testCustomMarshaller63 = marshalTest.RetTestCustomMarshallerWithMove63();
	testCustomMarshaller64 = marshalTest.RetTestCustomMarshallerWithMove64();
	testCustomMarshaller65 = marshalTest.RetTestCustomMarshallerWithMove65();
	testCustomMarshaller66 = marshalTest.RetTestCustomMarshallerWithMove66();
	testCustomMarshaller67 = marshalTest.RetTestCustomMarshallerWithMove67();
	testCustomMarshaller68 = marshalTest.RetTestCustomMarshallerWithMove68();
	testCustomMarshaller69 = marshalTest.RetTestCustomMarshallerWithMove69();
	testCustomMarshaller70 = marshalTest.RetTestCustomMarshallerWithMove70();
	testCustomMarshaller80 = marshalTest.RetTestCustomMarshallerWithMove80();
	testCustomMarshaller81 = marshalTest.RetTestCustomMarshallerWithMove81();
	testCustomMarshaller82 = marshalTest.RetTestCustomMarshallerWithMove82();
	testCustomMarshaller83 = marshalTest.RetTestCustomMarshallerWithMove83();
	testCustomMarshaller84 = marshalTest.RetTestCustomMarshallerWithMove84();
	testCustomMarshaller85 = marshalTest.RetTestCustomMarshallerWithMove85();

	result &= CompareTestData(testCustomMarshaller01, testData.testCustomMarshaller01);
	result &= CompareTestData(testCustomMarshaller02, testData.testCustomMarshaller02);
	result &= CompareTestData(testCustomMarshaller03, testData.testCustomMarshaller03);
	result &= CompareTestData(testCustomMarshaller04, testData.testCustomMarshaller04);
	result &= CompareTestData(testCustomMarshaller05, testData.testCustomMarshaller05);
	result &= CompareTestData(testCustomMarshaller06, testData.testCustomMarshaller06);
	result &= CompareTestData(testCustomMarshaller07, testData.testCustomMarshaller07);
	result &= CompareTestData(testCustomMarshaller08, testData.testCustomMarshaller08);
	result &= CompareTestData(testCustomMarshaller09, testData.testCustomMarshaller09);
	result &= CompareTestData(testCustomMarshaller10, testData.testCustomMarshaller10);
	result &= CompareTestData(testCustomMarshaller20, testData.testCustomMarshaller20);
	result &= CompareTestData(testCustomMarshaller21, testData.testCustomMarshaller21);
	result &= CompareTestData(testCustomMarshaller22, testData.testCustomMarshaller22);
	result &= CompareTestData(testCustomMarshaller23, testData.testCustomMarshaller23);
	result &= CompareTestData(testCustomMarshaller24, testData.testCustomMarshaller24);
	result &= CompareTestData(testCustomMarshaller25, testData.testCustomMarshaller25);
	result &= CompareTestData(testCustomMarshaller30, testData.testCustomMarshaller30);
	result &= CompareTestData(testCustomMarshaller31, testData.testCustomMarshaller31);
	result &= CompareTestData(testCustomMarshaller32, testData.testCustomMarshaller32);
	result &= CompareTestData(testCustomMarshaller61, testData.testCustomMarshaller61);
	result &= CompareTestData(testCustomMarshaller62, testData.testCustomMarshaller62);
	result &= CompareTestData(testCustomMarshaller63, testData.testCustomMarshaller63);
	result &= CompareTestData(testCustomMarshaller64, testData.testCustomMarshaller64);
	result &= CompareTestData(testCustomMarshaller65, testData.testCustomMarshaller65);
	result &= CompareTestData(testCustomMarshaller66, testData.testCustomMarshaller66);
	result &= CompareTestData(testCustomMarshaller67, testData.testCustomMarshaller67);
	result &= CompareTestData(testCustomMarshaller68, testData.testCustomMarshaller68);
	result &= CompareTestData(testCustomMarshaller69, testData.testCustomMarshaller69);
	result &= CompareTestData(testCustomMarshaller70, testData.testCustomMarshaller70);
	result &= CompareTestData(testCustomMarshaller80, testData.testCustomMarshaller80);
	result &= CompareTestData(testCustomMarshaller81, testData.testCustomMarshaller81);
	result &= CompareTestData(testCustomMarshaller82, testData.testCustomMarshaller82);
	result &= CompareTestData(testCustomMarshaller83, testData.testCustomMarshaller83);
	result &= CompareTestData(testCustomMarshaller84, testData.testCustomMarshaller84);
	result &= CompareTestData(testCustomMarshaller85, testData.testCustomMarshaller85);
#endif

	return result;
}
