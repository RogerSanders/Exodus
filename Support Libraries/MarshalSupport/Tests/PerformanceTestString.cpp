#include <windows.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "PerformMarshalTest.h"

int main()
{
	IPerformanceTest* performanceTestInterface = CreatePerformanceTestInterface();

	auto t0_cpu = std::chrono::high_resolution_clock::now();
	auto t1_cpu = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> secs = t1_cpu - t0_cpu;

	std::cout << "MarshalSupport std::string performance test" << std::endl;
	std::cout << "\t\tchar*\t\tchar* + size\tMarshal" << std::endl;

	std::cout << std::showpoint << std::fixed << std::setprecision(8);

	std::string stringResult0;
	std::string stringResult1;
	std::string stringResult2;
	unsigned int stringLoopCount = 10000000;
	unsigned int stringLength = 98;
	bool enableOutput = false;
	while(true)
	{
		if(stringLength == 0)
		{
			enableOutput = true;
		}

		std::string testString;
		for(unsigned int i = 0; i < stringLength; ++i)
		{
			testString.push_back((unsigned char)((i % 0x40) + 0x20));
		}
		if(enableOutput) std::cout << "Length: " << testString.length() << '\t';

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(unsigned int j = 0; j < stringLoopCount; j++)
		{
			performanceTestInterface->StringInNoMarshalling(testString.c_str(), stringResult0);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		if(enableOutput) std::cout << secs.count() << '\t';

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(unsigned int j = 0; j < stringLoopCount; j++)
		{
			performanceTestInterface->StringInNoMarshallingWithSize(testString.c_str(), testString.length(), stringResult1);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		if(enableOutput) std::cout << secs.count() << '\t';

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(unsigned int j = 0; j < stringLoopCount; j++)
		{
			performanceTestInterface->StringInWithMarshalling(testString, stringResult2);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		if(enableOutput) std::cout << secs.count() << '\n';

		stringLength = (stringLength + 1) % 100;
	}

	DeletePerformanceTestInterface(performanceTestInterface);
	return 0;
}
