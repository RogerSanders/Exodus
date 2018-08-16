#ifndef __PERFORMMARSHALTEST_H__
#define __PERFORMMARSHALTEST_H__
#include "catch.hpp"
#include "IMarshalTest.h"
#include "CatchWrapper.h"
#include "IPerformanceTest.h"
#include "ISomeInterface.h"

extern "C" __declspec(dllimport) void ConfigureCatchTestFramework(ResultCaptureWrapper* resultCapture, Catch::IRunner* runner, Catch::Ptr<Catch::IConfig const> const& config);

extern "C" __declspec(dllimport) bool MarshalTestConversions();
extern "C" __declspec(dllimport) bool MarshalTestVectorExtensions(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestNoCopyTypes(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestPair(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestTuple(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestUniquePtr(IMarshalTest& marshalTest);

extern "C" __declspec(dllimport) bool MarshalTestInPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestOutPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestOutComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInOutPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInOutComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestRetPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestRetComplex(IMarshalTest& marshalTest);

extern "C" __declspec(dllimport) ISomeInterface* CreateSomeInterface();

extern "C" __declspec(dllimport) IPerformanceTest* CreatePerformanceTestInterface();
extern "C" __declspec(dllimport) void DeletePerformanceTestInterface(IPerformanceTest* target);

#endif
