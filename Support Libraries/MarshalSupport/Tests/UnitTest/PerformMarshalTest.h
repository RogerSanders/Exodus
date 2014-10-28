#ifndef __PERFORMMARSHALTEST_H__
#define __PERFORMMARSHALTEST_H__
#include "catch.hpp"
#include "IMarshalTest.h"
#include "CatchWrapper.h"

extern "C" __declspec(dllimport) void ConfigureCatchTestFramework(ResultCaptureWrapper* resultCapture, Catch::IRunner* runner, Catch::Ptr<Catch::IConfig const> const& config);

extern "C" __declspec(dllimport) bool MarshalTestConversions();

extern "C" __declspec(dllimport) bool MarshalTestInPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestOutPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestOutComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInOutPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestInOutComplex(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestRetPrimitive(IMarshalTest& marshalTest);
extern "C" __declspec(dllimport) bool MarshalTestRetComplex(IMarshalTest& marshalTest);

#endif
