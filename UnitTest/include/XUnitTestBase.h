#pragma once
#ifndef XUnitTestBase_h__
#define XUnitTestBase_h__

#include <stdlib.h>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

class XTestObject
{
public:
    XTestObject();

    virtual ~XTestObject();

    virtual void initTestFunction() = 0;
};

class XUnitTestFrameBase
{
public:
    XUnitTestFrameBase();
    virtual ~XUnitTestFrameBase();

    void registerTestClass(XTestObject* pObject);

    void unregisterTestClass(XTestObject* pObject);

    void exec();

    void addError(const std::string& strError = "");

	void addTestFunction();

    void addTestCount();

    void clear();

    std::string toString() const;

	std::string result() const;

    bool save(const std::string& strPath = "../log/test.txt");

protected:
    int mErrorCount;
    int mTestCount;
	int m_nTestFunction;
    std::set<std::string> mErrorInfo;
    std::set<XTestObject*> mTestObjects;
};

//DECLARE_SINGLETON_CLASS(XUnitTestFrame, XUnitTestFrameBase, UTILITY_EXPORT)
class XUnitTestFrame : public XUnitTestFrameBase
{
public:

	static XUnitTestFrameBase* getInstancePtr()
	{
		static XUnitTestFrame _instance;
		return &_instance;
	}
};

template <typename T>
class XUnitTestRegisterAgent
{
public:
    XUnitTestRegisterAgent()
        : mObj(NULL)
    {
        mObj = new T();

        XUnitTestFrame::getInstancePtr()->registerTestClass(mObj);
    }

    ~XUnitTestRegisterAgent()
    {
        XUnitTestFrame::getInstancePtr()->unregisterTestClass(mObj);

        if (NULL != mObj)
        {
            delete mObj;
            mObj = NULL;
        }
    }

protected:
    T* mObj;
};

#define  DECLARE_UNIT_TEST() \
    public:\
    virtual void initTestFunction();

#define BEGIN_IMPLEMENT_TEST(className) \
    static XUnitTestRegisterAgent<className> g_Test##className; \
    void className::initTestFunction(){

#define END_IMPLEMENT_TEST() \
    } 

#define ADD_TEST_FUNCTION(funcName) \
	XUnitTestFrame::getInstancePtr()->addTestFunction(); \
	funcName();

#define UNIT_TEST_ASSERT(bValue) XUnitTestFrame::getInstancePtr()->addTestCount(); \
        if(!(bValue))\
        {\
        std::stringstream ss; \
        ss << std::string(__FILE__) << " " << std::string(__FUNCTION__) << " (LINE:" <<  __LINE__ << ")"; \
        XUnitTestFrame::getInstancePtr()->addError(ss.str());} 


#define FAST_WRITE_FILE(path, content, len) \
{\
	std::ofstream _ofs_00; \
	_ofs_00.open(path, std::ios::binary | std::ios::ate | std::ios::out); \
	if (_ofs_00.is_open()) \
	{ \
		_ofs_00.write(content, len); \
		_ofs_00.flush(); \
		_ofs_00.close(); \
	} \
} 

//尝试使用比较工具
const std::string STR_BC_PATH = "\"D:\\Green\\Beyond Compare 3\\";
const std::string STR_BC_COMMAND = STR_BC_PATH + "BCompare.exe\"";

#define EASY_COMPARE(path0, path1) \
{\
	std::stringstream __ss__00__; \
	__ss__00__ << STR_BC_COMMAND << " " << path0 << " " << path1; \
	system(__ss__00__.str().c_str()); \
}

#endif // XUnitTestBase_h__
