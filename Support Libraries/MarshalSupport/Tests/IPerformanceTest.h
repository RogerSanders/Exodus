#ifndef __IPERFORMANCETEST_H__
#define __IPERFORMANCETEST_H__
#include <vector>
#include <string>
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif

class IPerformanceTest
{
public:
	virtual ~IPerformanceTest() = 0 {};

	virtual void StringInNoMarshalling(const char* data, std::string& out) const = 0;
	virtual void StringInNoMarshallingWithSize(const char* data, size_t length, std::string& out) const = 0;
	virtual void StringInWithMarshalling(const Marshal::In<std::string>& data, std::string& out) const = 0;

	virtual void FillVectorNoMarshalling(double* data, size_t entryCount, std::vector<double>& out) const = 0;
	virtual void FillVectorNoMarshallingPushBack(double* data, size_t entryCount, std::vector<double>& out) const = 0;
	virtual void FillVectorNoMarshallingAssign(double* data, size_t entryCount, std::vector<double>& out) const = 0;
	virtual void FillVectorNoMarshallingConstruct(double* data, size_t entryCount, std::vector<double>& out) const = 0;
	virtual void FillVectorWithMarshalling(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const = 0;
	virtual void FillVectorWithMarshallingPushBack(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const = 0;
	virtual void FillVectorWithMarshallingAssign(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const = 0;
	virtual void FillVectorWithMarshallingConstruct(double* data, size_t entryCount, const Marshal::Out<std::vector<double>>& out) const = 0;
};

#endif
