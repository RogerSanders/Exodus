#include <windows.h>
#include <iostream>
#include <chrono>
#include "PerformMarshalTest.h"

const bool checkResult = false;
const bool clearResult = false;
const bool columnOutput = false;
const int M = 4000;
const int N = (columnOutput ? 10000 : 100000);

void FillNoMarshalling( double* data, size_t entryCount, std::vector<double>& out )
{
	out.resize(entryCount);
	for(size_t i = 0; i < entryCount; i++)
	{
		out[i] = data[i];
	}
}

void FillNoMarshallingPushBack( double* data, size_t entryCount, std::vector<double>& out )
{
	out.clear();
	out.reserve(entryCount);
	for(size_t i = 0; i < entryCount; i++)
	{
		out.push_back(data[i]);
	}
}

void FillNoMarshallingAssign( double* data, size_t entryCount, std::vector<double>& out )
{
	out.assign(data, (data + entryCount));
}

void FillNoMarshallingConstruct( double* data, size_t entryCount, std::vector<double>& out )
{
	out = std::vector<double>(data, (data + entryCount));
}

void CheckResult(const std::vector< double >& data, const std::vector< double >& result0, const std::vector< double >& result1, const std::vector< double >& result2, const std::vector< double >& result3)
{
	for(int j = 0; j < N; j++)
	{
		if(result0.size() != M)
		{
			std::cout << "ERROR!";
		}
		if(result1.size() != M)
		{
			std::cout << "ERROR!";
		}
		if(result2.size() != M)
		{
			std::cout << "ERROR!";
		}
		if(result3.size() != M)
		{
			std::cout << "ERROR!";
		}
		for(int i = 0; i < M; ++i)
		{
			if(result0[i] != data[(M*(4*j+0))+i])
			{
				std::cout << "ERROR!";
			}
			if(result1[i] != data[(M*(4*j+1))+i])
			{
				std::cout << "ERROR!";
			}
			if(result2[i] != data[(M*(4*j+2))+i])
			{
				std::cout << "ERROR!";
			}
			if(result3[i] != data[(M*(4*j+3))+i])
			{
				std::cout << "ERROR!";
			}
		}
	}
}

void ClearResult(std::vector< double >& result0, std::vector< double >& result1, std::vector< double >& result2, std::vector< double >& result3)
{
	result0.clear();
	result1.clear();
	result2.clear();
	result3.clear();
}

int main()
{
	IPerformanceTest* performanceTestInterface = CreatePerformanceTestInterface();

	auto t0_cpu = std::chrono::high_resolution_clock::now();
	auto t1_cpu = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> secs = t1_cpu - t0_cpu;

	std::cout << "MarshalSupport std::vector performance test" << std::endl;

	std::vector<double> data(4 * M * N, 7);
	std::vector<double> result0;
	std::vector<double> result1;
	std::vector<double> result2;
	std::vector<double> result3;

	if(columnOutput)
	{
		std::cout << "\tPushBck\t\t\tResize\t\t\tConstr\t\t\tAssign" << std::endl;
		std::cout << "Direct\tNative\tMarshal\tDirect\tNative\tMarshal\tDirect\tNative\tMarshal\tDirect\tNative\tMarshal" << std::endl;
		std::cout << std::showpoint << std::fixed << std::setprecision(5);
	}
	else
	{
		std::cout << "\tPushBck\t\tResize\t\tConstr\t\tAssign" << std::endl;
		std::cout << std::showpoint << std::fixed << std::setprecision(8);
	}
	std::chrono::duration<float> durations[4][3];

	while(true)
	{
		if(!columnOutput) std::cout << "Direct";
		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			FillNoMarshallingPushBack(&data[M*(4*j+0)], M, result0);
			FillNoMarshallingPushBack(&data[M*(4*j+1)], M, result1);
			FillNoMarshallingPushBack(&data[M*(4*j+2)], M, result2);
			FillNoMarshallingPushBack(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[0][0] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count();
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			FillNoMarshalling(&data[M*(4*j+0)], M, result0);
			FillNoMarshalling(&data[M*(4*j+1)], M, result1);
			FillNoMarshalling(&data[M*(4*j+2)], M, result2);
			FillNoMarshalling(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[1][0] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count();
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			FillNoMarshallingConstruct(&data[M*(4*j+0)], M, result0);
			FillNoMarshallingConstruct(&data[M*(4*j+1)], M, result1);
			FillNoMarshallingConstruct(&data[M*(4*j+2)], M, result2);
			FillNoMarshallingConstruct(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[2][0] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count();
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			FillNoMarshallingAssign(&data[M*(4*j+0)], M, result0);
			FillNoMarshallingAssign(&data[M*(4*j+1)], M, result1);
			FillNoMarshallingAssign(&data[M*(4*j+2)], M, result2);
			FillNoMarshallingAssign(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[3][0] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count();
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		if(!columnOutput) std::cout << std::endl << "Native";
		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorNoMarshallingPushBack(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorNoMarshallingPushBack(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorNoMarshallingPushBack(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorNoMarshallingPushBack(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[0][1] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[0][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorNoMarshalling(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorNoMarshalling(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorNoMarshalling(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorNoMarshalling(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[1][1] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[1][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorNoMarshallingConstruct(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorNoMarshallingConstruct(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorNoMarshallingConstruct(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorNoMarshallingConstruct(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[2][1] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[2][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorNoMarshallingAssign(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorNoMarshallingAssign(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorNoMarshallingAssign(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorNoMarshallingAssign(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[3][1] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[3][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		if(!columnOutput) std::cout << std::endl << "Marshal";
		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorWithMarshallingPushBack(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorWithMarshallingPushBack(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorWithMarshallingPushBack(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorWithMarshallingPushBack(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[0][2] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[0][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorWithMarshalling(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorWithMarshalling(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorWithMarshalling(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorWithMarshalling(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[1][2] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[1][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorWithMarshallingConstruct(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorWithMarshallingConstruct(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorWithMarshallingConstruct(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorWithMarshallingConstruct(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[2][2] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[2][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		t0_cpu = std::chrono::high_resolution_clock::now();
		for(int j = 0; j < N; j++)
		{
			performanceTestInterface->FillVectorWithMarshallingAssign(&data[M*(4*j+0)], M, result0);
			performanceTestInterface->FillVectorWithMarshallingAssign(&data[M*(4*j+1)], M, result1);
			performanceTestInterface->FillVectorWithMarshallingAssign(&data[M*(4*j+2)], M, result2);
			performanceTestInterface->FillVectorWithMarshallingAssign(&data[M*(4*j+3)], M, result3);
		}
		t1_cpu = std::chrono::high_resolution_clock::now();
		secs = t1_cpu - t0_cpu;
		durations[3][2] = secs;
		if(!columnOutput) std::cout << "\t" << secs.count() << " " << (int)((secs.count() / durations[3][0].count()) * 100) << "%";
		if(checkResult) CheckResult(data, result0, result1, result2, result3);
		if(clearResult) ClearResult(result0, result1, result2, result3);

		if(columnOutput)
		{
			std::cout << durations[0][0].count() << "\t" << durations[0][1].count() << "\t" << durations[0][2].count() << "\t"
			          << durations[1][0].count() << "\t" << durations[1][1].count() << "\t" << durations[1][2].count() << "\t"
			          << durations[2][0].count() << "\t" << durations[2][1].count() << "\t" << durations[2][2].count() << "\t"
			          << durations[3][0].count() << "\t" << durations[3][1].count() << "\t" << durations[3][2].count() << "\n";
		}
		else
		{
			std::cout << std::endl;
		}
	}

	DeletePerformanceTestInterface(performanceTestInterface);

	return 0;
}
