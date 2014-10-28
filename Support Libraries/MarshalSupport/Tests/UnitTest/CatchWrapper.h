#ifndef __CATCHWRAPPER_H__
#define __CATCHWRAPPER_H__
#include "catch.hpp"

class AssertionResultAccessor :public Catch::AssertionResult
{
public:
	AssertionResultAccessor(const Catch::AssertionResult& source)
	:Catch::AssertionResult(source)
	{}

	const Catch::AssertionInfo& GetAssertionInfo() const
	{
		return m_info;
	}
	const Catch::AssertionResultData& GetResultData() const
	{
		return m_resultData;
	}
};

class IAssertionResultWrapper
{
public:
	inline virtual ~IAssertionResultWrapper() = 0;

	virtual const char* GetMacroName() const = 0;
	virtual const char* GetLineInfoFile() const = 0;
	virtual size_t GetLineInfoLineNo() const = 0;
	virtual const char* GetCapturedExpression() const = 0;
	virtual Catch::ResultDisposition::Flags GetResultDisposition() const = 0;
	virtual const char* GetReconstructedExpression() const = 0;
	virtual const char* GetResultMessage() const = 0;
	virtual Catch::ResultWas::OfType GetResultType() const = 0;
};
IAssertionResultWrapper::~IAssertionResultWrapper() {}

class AssertionResultWrapper :public IAssertionResultWrapper
{
public:
	AssertionResultWrapper(const AssertionResultAccessor& atarget)
	:target(atarget)
	{}

	virtual const char* GetMacroName() const
	{
		return target.GetAssertionInfo().macroName.c_str();
	}
	virtual const char* GetLineInfoFile() const
	{
		return target.GetAssertionInfo().lineInfo.file.c_str();
	}
	virtual size_t GetLineInfoLineNo() const
	{
		return target.GetAssertionInfo().lineInfo.line;
	}
	virtual const char* GetCapturedExpression() const
	{
		return target.GetAssertionInfo().capturedExpression.c_str();
	}
	virtual Catch::ResultDisposition::Flags GetResultDisposition() const
	{
		return target.GetAssertionInfo().resultDisposition;
	}
	virtual const char* GetReconstructedExpression() const
	{
		return target.GetResultData().reconstructedExpression.c_str();
	}
	virtual const char* GetResultMessage() const
	{
		return target.GetResultData().message.c_str();
	}
	virtual Catch::ResultWas::OfType GetResultType() const
	{
		return target.GetResultData().resultType;
	}

private:
	AssertionResultAccessor target;
};

class IResultCaptureWrapper
{
public:
	virtual void assertionEnded(const IAssertionResultWrapper& result) = 0;
	virtual bool sectionStarted(Catch::SectionInfo const& sectionInfo, Catch::Counts& assertions ) = 0;
	virtual void sectionEnded(Catch::SectionInfo const& name, Catch::Counts const& assertions, double _durationInSeconds ) = 0;
	virtual void pushScopedMessage(Catch::MessageInfo const& message ) = 0;
	virtual void popScopedMessage(Catch::MessageInfo const& message ) = 0;
	virtual const char* getCurrentTestName() const = 0;
	virtual const Catch::AssertionResult* getLastResult() const = 0;
};

//----------------------------------------------------------------------------------------
class ResultCaptureWrapper :public IResultCaptureWrapper
{
public:
	ResultCaptureWrapper(Catch::IResultCapture* atarget)
	:target(atarget)
	{};

	virtual void assertionEnded(const IAssertionResultWrapper& result)
	{
		Catch::SourceLineInfo sourceLineInfo(result.GetLineInfoFile(), result.GetLineInfoLineNo());
		Catch::AssertionInfo assertionInfo(result.GetMacroName(), sourceLineInfo, result.GetCapturedExpression(), result.GetResultDisposition());
		Catch::AssertionResultData assertionResultData;
		assertionResultData.resultType = result.GetResultType();
		assertionResultData.message = result.GetResultMessage();
		assertionResultData.reconstructedExpression = result.GetReconstructedExpression();
		Catch::AssertionResult newResult(assertionInfo, assertionResultData);
		target->assertionEnded(newResult);
	}

	virtual bool sectionStarted(Catch::SectionInfo const& sectionInfo, Catch::Counts& assertions)
	{
		return target->sectionStarted(sectionInfo, assertions);
	}

	virtual void sectionEnded(Catch::SectionInfo const& name, Catch::Counts const& assertions, double _durationInSeconds)
	{
		return target->sectionEnded(name, assertions, _durationInSeconds);
	}

	virtual void pushScopedMessage(Catch::MessageInfo const& message)
	{
		target->pushScopedMessage(message);
	}

	virtual void popScopedMessage(Catch::MessageInfo const& message)
	{
		target->popScopedMessage(message);
	}

	virtual const char* getCurrentTestName() const
	{
		return target->getCurrentTestName().c_str();
	}

	virtual const Catch::AssertionResult* getLastResult() const
	{
		return target->getLastResult();
	}

private:
	Catch::IResultCapture* target;
};

//----------------------------------------------------------------------------------------
class ResultCaptureStub :public Catch::IResultCapture
{
public:
	ResultCaptureStub(IResultCaptureWrapper* atarget)
	:target(atarget)
	{};

	virtual void assertionEnded(Catch::AssertionResult const& result)
	{
		AssertionResultWrapper wrapper(result);
		target->assertionEnded(wrapper);
	}

	virtual bool sectionStarted(Catch::SectionInfo const& sectionInfo, Catch::Counts& assertions)
	{
		return target->sectionStarted(sectionInfo, assertions);
	}

	virtual void sectionEnded(Catch::SectionInfo const& name, Catch::Counts const& assertions, double _durationInSeconds)
	{
		return target->sectionEnded(name, assertions, _durationInSeconds);
	}

	virtual void pushScopedMessage(Catch::MessageInfo const& message)
	{
		target->pushScopedMessage(message);
	}

	virtual void popScopedMessage(Catch::MessageInfo const& message)
	{
		target->popScopedMessage(message);
	}

	virtual std::string getCurrentTestName() const
	{
		return target->getCurrentTestName();
	}

	virtual const Catch::AssertionResult* getLastResult() const
	{
		return target->getLastResult();
	}

private:
	IResultCaptureWrapper* target;
};

#endif
