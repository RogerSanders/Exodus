#define _HAS_EXCEPTIONS 0
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
	SECTION("Primitives", "")
	{
		REQUIRE(MarshalTestInPrimitive(marshalTest));
	}
	SECTION("Marshalled Types", "")
	{
		REQUIRE(MarshalTestInComplex(marshalTest));
	}
}

TEST_CASE("Marshal::Out", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("Primitives", "")
	{
		REQUIRE(MarshalTestOutPrimitive(marshalTest));
	}
	SECTION("Marshalled Types", "")
	{
		REQUIRE(MarshalTestOutComplex(marshalTest));
	}
}

TEST_CASE("Marshal::InOut", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("Primitives", "")
	{
		REQUIRE(MarshalTestInOutPrimitive(marshalTest));
	}
	SECTION("Marshalled Types", "")
	{
		REQUIRE(MarshalTestInOutComplex(marshalTest));
	}
}

TEST_CASE("Marshal::Ret", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("Primitives", "")
	{
		REQUIRE(MarshalTestRetPrimitive(marshalTest));
	}
	SECTION("Marshalled Types", "")
	{
		REQUIRE(MarshalTestRetComplex(marshalTest));
	}
}

TEST_CASE("Conversions", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	SECTION("Implicit Conversions", "")
	{
		REQUIRE(MarshalTestConversions());
	}
}

TEST_CASE("Extensions", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("Vector Extensions", "")
	{
		REQUIRE(MarshalTestVectorExtensions(marshalTest));
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
TEST_CASE("NoCopyTypes", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("NoCopyTypes", "")
	{
		REQUIRE(MarshalTestNoCopyTypes(marshalTest));
	}
}
#endif

TEST_CASE("Composite types", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("std::pair", "")
	{
		REQUIRE(MarshalTestPair(marshalTest));
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	SECTION("std::tuple", "")
	{
		REQUIRE(MarshalTestTuple(marshalTest));
	}
#endif
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
TEST_CASE("STL pointer types", "")
{
	Catch::IContext& catchContext = Catch::getCurrentContext();
	ResultCaptureWrapper captureWrapper(catchContext.getResultCapture());
	ConfigureCatchTestFramework(&captureWrapper, catchContext.getRunner(), catchContext.getConfig());

	MarshalTest marshalTest;
	SECTION("std::unique_ptr", "")
	{
		REQUIRE(MarshalTestUniquePtr(marshalTest));
	}
}
#endif
