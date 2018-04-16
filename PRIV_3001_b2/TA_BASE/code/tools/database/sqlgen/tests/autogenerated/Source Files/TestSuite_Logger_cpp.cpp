#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/Logger.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/Logger.cpp");

class TestSuite_Logger_cpp_aa4c6eb7 : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_Logger_cpp_aa4c6eb7);
        CPPTEST_TEST(test_CLogger_1);
        CPPTEST_TEST(test__ConstructMessage_1);
        CPPTEST_TEST(test__ConstructMsgBody_1);
        CPPTEST_TEST(test__ConstructMsgHeader_1);
        CPPTEST_TEST(test__GetStringDebugLevel_1);
        CPPTEST_TEST(test_deletelogFile_1);
        CPPTEST_TEST(test_getInstance_1);
        CPPTEST_TEST(test_log_1);
        CPPTEST_TEST(test_removeInstance_1);
        CPPTEST_TEST(test_setlogFile_1);
        CPPTEST_TEST(test_err_setlogFile_1);
        
        CPPTEST_TEST(test_setlogLevel_1);
        CPPTEST_TEST(test_x7eCLogger_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

        void test_CLogger_1();
        void test__ConstructMessage_1();
        void test__ConstructMsgBody_1();
        void test__ConstructMsgHeader_1();
        void test__GetStringDebugLevel_1();
        void test_deletelogFile_1();
        void test_getInstance_1();
        void test_log_1();
        void test_removeInstance_1();
        void test_setlogFile_1();
        void test_err_setlogFile_1();
        void test_setlogLevel_1();
        void test_x7eCLogger_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_Logger_cpp_aa4c6eb7);

void TestSuite_Logger_cpp_aa4c6eb7::setUp()
{
}

void TestSuite_Logger_cpp_aa4c6eb7::tearDown()
{
}

/* CPPTEST_TEST_CASE_BEGIN test_CLogger_1 */
/* CPPTEST_TEST_CASE_CONTEXT CLogger::CLogger(void) */
void TestSuite_Logger_cpp_aa4c6eb7::test_CLogger_1()
{
	CLogger::getInstance();
	CLogger::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_CLogger_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ConstructMessage_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CLogger::_ConstructMessage(int, const std::string &, int, std::string &, va_list) */
void TestSuite_Logger_cpp_aa4c6eb7::test__ConstructMessage_1()
{
	std::string strLogFileName;
	
	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);
	
	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();

	CLogger::removeInstance();

}
/* CPPTEST_TEST_CASE_END test__ConstructMessage_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ConstructMsgBody_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CLogger::_ConstructMsgBody(int, va_list) */
void TestSuite_Logger_cpp_aa4c6eb7::test__ConstructMsgBody_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();

	CLogger::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test__ConstructMsgBody_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ConstructMsgHeader_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CLogger::_ConstructMsgHeader(int, const std::string &, int, std::string &) */
void TestSuite_Logger_cpp_aa4c6eb7::test__ConstructMsgHeader_1()
{

	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();

	CLogger::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test__ConstructMsgHeader_1 */

/* CPPTEST_TEST_CASE_BEGIN test__GetStringDebugLevel_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CLogger::_GetStringDebugLevel(EDebugLevel) */
void TestSuite_Logger_cpp_aa4c6eb7::test__GetStringDebugLevel_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();

	CLogger::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test__GetStringDebugLevel_1 */

/* CPPTEST_TEST_CASE_BEGIN test_deletelogFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CLogger::deletelogFile(void) */
void TestSuite_Logger_cpp_aa4c6eb7::test_deletelogFile_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();


}
/* CPPTEST_TEST_CASE_END test_deletelogFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT CLogger & CLogger::getInstance(void) */
void TestSuite_Logger_cpp_aa4c6eb7::test_getInstance_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	_SysLog(SourceFLInfo, DebugDebug, "Test log string: %s", "stringValue");

	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test_getInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_log_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CLogger::log(const std::string &, int, EDebugLevel, ...) */
void TestSuite_Logger_cpp_aa4c6eb7::test_log_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();

}
/* CPPTEST_TEST_CASE_END test_log_1 */

/* CPPTEST_TEST_CASE_BEGIN test_removeInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CLogger::removeInstance(void) */
void TestSuite_Logger_cpp_aa4c6eb7::test_removeInstance_1()
{

	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test_removeInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_setlogFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CLogger::setlogFile(const std::string &) */
void TestSuite_Logger_cpp_aa4c6eb7::test_setlogFile_1()
{	 
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);
	CLogger::getInstance().setlogFile(strLogFileName);							
													
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();


}

void TestSuite_Logger_cpp_aa4c6eb7::test_err_setlogFile_1()
{	 
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);
	CLogger::getInstance().setlogFile(strLogFileName);							

	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();


}
/* CPPTEST_TEST_CASE_END test_setlogFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test_setlogLevel_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CLogger::setlogLevel(EDebugLevel) */
void TestSuite_Logger_cpp_aa4c6eb7::test_setlogLevel_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();

}
/* CPPTEST_TEST_CASE_END test_setlogLevel_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCLogger_1 */
/* CPPTEST_TEST_CASE_CONTEXT CLogger::~CLogger(void) */
void TestSuite_Logger_cpp_aa4c6eb7::test_x7eCLogger_1()
{
	std::string strLogFileName;

	CLogger::getInstance();
	strLogFileName = defSQLFilename_OUT_ERRORREPORT;
	CLogger::getInstance().setlogFile(strLogFileName);															
	CLogger::getInstance().setlogLevel(DebugError);

	_SysLog(SourceFLInfo, DebugError, "Test log string: %s", "stringValue");
	CLogger::getInstance().deletelogFile();
	CLogger::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_x7eCLogger_1 */
