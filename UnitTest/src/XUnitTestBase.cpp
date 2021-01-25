#include "XUnitTestBase.h"
#include <sstream>
#include <fstream>
#include <set>

XUnitTestFrameBase::XUnitTestFrameBase()
    : mErrorCount(0)
    , mTestCount(0)
	, m_nTestFunction(0)
{

}

XUnitTestFrameBase::~XUnitTestFrameBase()
{

}

void XUnitTestFrameBase::registerTestClass( XTestObject* pObject )
{
    if (NULL == pObject)
    {
        return;
    }

    mTestObjects.insert(pObject);
}

void XUnitTestFrameBase::unregisterTestClass( XTestObject* pObject )
{
    if (NULL == pObject)
    {
        return;
    }

    mTestObjects.erase(pObject);
}

void XUnitTestFrameBase::exec()
{
    clear();

    for (std::set<XTestObject*>::iterator it = mTestObjects.begin();
        it != mTestObjects.end(); ++it)
    {
        XTestObject* pObject = (*it);
        if (NULL == pObject)
        {
            continue;
        }
        pObject->initTestFunction();
    }
}

void XUnitTestFrameBase::addError( const std::string& strError /*= ""*/ )
{
    ++mErrorCount;
    mErrorInfo.insert(strError);
}

void XUnitTestFrameBase::addTestFunction()
{
	++m_nTestFunction;
}

void XUnitTestFrameBase::clear()
{
    mTestCount = 0;
    mErrorCount = 0;
    mErrorInfo.clear();
}

std::string XUnitTestFrameBase::toString() const
{
    std::stringstream ss;
	for (std::set<std::string>::const_iterator it = mErrorInfo.begin(); 
		it != mErrorInfo.end(); ++it)
	{
		ss << (*it) << "\r\n";
	}

    ss << "total=" << mTestCount << "," << (mTestCount - mErrorCount) << "(OK)," << mErrorCount << "(Error)" << "\r\n";

    return ss.str();
}

std::string XUnitTestFrameBase::result() const
{
	std::stringstream ss;

	ss << "total=" << mTestCount << "," << (mTestCount - mErrorCount) << "(OK)," << mErrorCount << "(Error)" << ".function=" << m_nTestFunction << "\r\n";

	return ss.str();
}

bool XUnitTestFrameBase::save(const std::string &strPath)
{
    std::ofstream ofs;
    ofs.open(strPath.c_str(), std::ios::binary | std::ios::out | std::ios::ate);
    if(!ofs.is_open())
    {
        return false;
    }

    ofs << toString();
    ofs.close();
    return true;
}

void XUnitTestFrameBase::addTestCount()
{
    ++mTestCount;
}

//IMPLEMENT_SINGLETON_CLASS(XUnitTestFrame)

XTestObject::XTestObject()
{
    
}

XTestObject::~XTestObject()
{

}
