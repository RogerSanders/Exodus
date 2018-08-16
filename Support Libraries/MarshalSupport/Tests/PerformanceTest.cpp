#include "PerformanceTest.h"

void PerformanceTest::StringInNoMarshalling(const char* data, std::string& out) const
{
	out = data;
}

void PerformanceTest::StringInNoMarshallingWithSize(const char* data, size_t length, std::string& out) const
{
	out.assign(data, length);
}

void PerformanceTest::StringInWithMarshalling(const Marshal::In<std::string>& data, std::string& out) const
{
	data.Get(out);
}

void PerformanceTest::FillVectorNoMarshalling(double* data, size_t entryCount, std::vector<double>& out) const
{
	out.resize(entryCount);
	for(size_t i = 0; i < entryCount; ++i)
	{
		out[i] = data[i];
	}
}

void PerformanceTest::FillVectorNoMarshallingPushBack(double* data, size_t entryCount, std::vector<double>& out) const
{
	out.clear();
	out.reserve(entryCount);
	for(size_t i = 0; i < entryCount; ++i)
	{
		out.push_back(data[i]);
	}
}

void PerformanceTest::FillVectorNoMarshallingAssign(double* data, size_t entryCount, std::vector<double>& out) const
{
	out.assign(data, (data + entryCount));
}

void PerformanceTest::FillVectorNoMarshallingConstruct(double* data, size_t entryCount, std::vector<double>& out) const
{
	out = std::vector<double>(data, (data + entryCount));
}

void PerformanceTest::FillVectorWithMarshalling(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const
{
	out.resize(entryCount, 0);
	for(size_t i = 0; i < entryCount; ++i)
	{
		out.SetElement(i, data[i]);
	}
}

void PerformanceTest::FillVectorWithMarshallingPushBack(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const
{
	out.clear();
	out.reserve(entryCount);
	for(size_t i = 0; i < entryCount; ++i)
	{
		out.push_back(data[i]);
	}
}

void PerformanceTest::FillVectorWithMarshallingAssign(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const
{
	out.AssignFrom(data, entryCount);
}

void PerformanceTest::FillVectorWithMarshallingConstruct(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const
{
	out = std::vector<double>(data, (data + entryCount));
}
