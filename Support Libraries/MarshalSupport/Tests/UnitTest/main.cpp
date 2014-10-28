#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "MarshalTest.h"
#include "PerformMarshalTest.h"
#include "CatchWrapper.h"

TEST_CASE("Marshal::In", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	bool result = true;
	SECTION("Primitives", "")
	{
		result &= MarshalTestInPrimitive(marshalTest);
	}
	SECTION("Marshalled Types", "")
	{
		result &= MarshalTestInComplex(marshalTest);
	}
}

TEST_CASE("Marshal::Out", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	bool result = true;
	SECTION("Primitives", "")
	{
		result &= MarshalTestOutPrimitive(marshalTest);
	}
	SECTION("Marshalled Types", "")
	{
		result &= MarshalTestOutComplex(marshalTest);
	}
}

TEST_CASE("Marshal::InOut", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	bool result = true;
	SECTION("Primitives", "")
	{
		result &= MarshalTestInOutPrimitive(marshalTest);
	}
	SECTION("Marshalled Types", "")
	{
		result &= MarshalTestInOutComplex(marshalTest);
	}
}

TEST_CASE("Marshal::Ret", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	bool result = true;
	SECTION("Primitives", "")
	{
		result &= MarshalTestRetPrimitive(marshalTest);
	}
	SECTION("Marshalled Types", "")
	{
		result &= MarshalTestRetComplex(marshalTest);
	}
}

TEST_CASE("Conversions", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	bool result = true;
	SECTION("Implicit Conversions", "")
	{
		result &= MarshalTestConversions();
	}
}
