#ifndef __PERFORMANCETEST_H__
#define __PERFORMANCETEST_H__
#include "IPerformanceTest.h"

class PerformanceTest :public IPerformanceTest
{
	virtual void StringInNoMarshalling(const char* data, std::string& out) const;
	virtual void StringInNoMarshallingWithSize(const char* data, size_t length, std::string& out) const;
	virtual void StringInWithMarshalling(const Marshal::In<std::string>& data, std::string& out) const;

	virtual void FillVectorNoMarshalling(double* data, size_t entryCount, std::vector<double>& out) const;
	virtual void FillVectorNoMarshallingPushBack(double* data, size_t entryCount, std::vector<double>& out) const;
	virtual void FillVectorNoMarshallingAssign(double* data, size_t entryCount, std::vector<double>& out) const;
	virtual void FillVectorNoMarshallingConstruct(double* data, size_t entryCount, std::vector<double>& out) const;
	virtual void FillVectorWithMarshalling(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const;
	virtual void FillVectorWithMarshallingPushBack(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const;
	virtual void FillVectorWithMarshallingAssign(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const;
	virtual void FillVectorWithMarshallingConstruct(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const;
};

#endif
