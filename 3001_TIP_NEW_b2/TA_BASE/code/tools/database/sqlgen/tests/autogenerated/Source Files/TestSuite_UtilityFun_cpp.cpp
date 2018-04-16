#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/UtilityFun.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/UtilityFun.cpp");

class TestSuite_UtilityFun_cpp_8afd5a96 : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_UtilityFun_cpp_8afd5a96);
        CPPTEST_TEST(test_CUtilityFun_1);
        CPPTEST_TEST(test_getInstance_1);
        CPPTEST_TEST(test_int2str_1);
        CPPTEST_TEST(test_ltrim_1);
        CPPTEST_TEST(test_removeInstance_1);
        CPPTEST_TEST(test_removeUnVisableStr_1);
        CPPTEST_TEST(test_rtrim_1);
        CPPTEST_TEST(test_sysStrLwr_1);
        CPPTEST_TEST(test_sysStrUpper_1);
        CPPTEST_TEST(test_sysStricmp_1);
        CPPTEST_TEST(test_trim_1);
        CPPTEST_TEST(test_x7eCUtilityFun_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

        void test_CUtilityFun_1();
        void test_getInstance_1();
        void test_int2str_1();
        void test_ltrim_1();
        void test_removeInstance_1();
        void test_removeUnVisableStr_1();
        void test_rtrim_1();
        void test_sysStrLwr_1();
        void test_sysStrUpper_1();
        void test_sysStricmp_1();
        void test_trim_1();
        void test_x7eCUtilityFun_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_UtilityFun_cpp_8afd5a96);

void TestSuite_UtilityFun_cpp_8afd5a96::setUp()
{
}

void TestSuite_UtilityFun_cpp_8afd5a96::tearDown()
{
}

/* CPPTEST_TEST_CASE_BEGIN test_CUtilityFun_1 */
/* CPPTEST_TEST_CASE_CONTEXT CUtilityFun::CUtilityFun(void) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_CUtilityFun_1()
{
	CUtilityFun::getInstance();
	CUtilityFun::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_CUtilityFun_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT CUtilityFun & CUtilityFun::getInstance(void) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_getInstance_1()
{
	CUtilityFun::getInstance();
	CUtilityFun::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_getInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_int2str_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CUtilityFun::int2str(int) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_int2str_1()
{
	CUtilityFun::getInstance();
	int _nVal  = 5;  
	std::string strNVal;
	
	_nVal  = INT_MIN;  
	strNVal = CUtilityFun::getInstance().int2str(_nVal);
	_nVal  = 5;  
	strNVal = CUtilityFun::getInstance().int2str(_nVal);
	_nVal  = INT_MAX;  
	strNVal = CUtilityFun::getInstance().int2str(_nVal);

	CUtilityFun::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_int2str_1 */

/* CPPTEST_TEST_CASE_BEGIN test_ltrim_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string & CUtilityFun::ltrim(std::string &) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_ltrim_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	std::string strTestRes;	
 
	strTest = "  _TTTT_  _TTT_  ";
	strTestRes = CUtilityFun::getInstance().ltrim(strTest);	

	CUtilityFun::removeInstance(); 
}
/* CPPTEST_TEST_CASE_END test_ltrim_1 */

/* CPPTEST_TEST_CASE_BEGIN test_removeInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CUtilityFun::removeInstance(void) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_removeInstance_1()
{ 
	CUtilityFun::getInstance();
	CUtilityFun::removeInstance(); 
	
}
/* CPPTEST_TEST_CASE_END test_removeInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_removeUnVisableStr_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CUtilityFun::removeUnVisableStr(std::string &) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_removeUnVisableStr_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	int nFunRes = 0;	

	strTest = "  _TTTT_  _TTT_  \r\n";
	nFunRes = CUtilityFun::getInstance().removeUnVisableStr(strTest);	

	CUtilityFun::removeInstance();		
}
/* CPPTEST_TEST_CASE_END test_removeUnVisableStr_1 */

/* CPPTEST_TEST_CASE_BEGIN test_rtrim_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string & CUtilityFun::rtrim(std::string &) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_rtrim_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	std::string strTestRes;	

	strTest = "  _TTTT_  _TTT_  ";
	strTestRes = CUtilityFun::getInstance().rtrim(strTest);	

	CUtilityFun::removeInstance(); 
}
/* CPPTEST_TEST_CASE_END test_rtrim_1 */

/* CPPTEST_TEST_CASE_BEGIN test_sysStrLwr_1 */
/* CPPTEST_TEST_CASE_CONTEXT char * CUtilityFun::sysStrLwr(char *) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_sysStrLwr_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	std::string strTestRes;	

	strTest = "  _TTttT_  _TtT_  ";
	strTestRes = CUtilityFun::getInstance().sysStrLwr((char*)strTest.c_str());	

	CUtilityFun::removeInstance(); 
}
/* CPPTEST_TEST_CASE_END test_sysStrLwr_1 */

/* CPPTEST_TEST_CASE_BEGIN test_sysStrUpper_1 */
/* CPPTEST_TEST_CASE_CONTEXT char * CUtilityFun::sysStrUpper(char *) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_sysStrUpper_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	std::string strTestRes;	

	strTest = "  _TTttT_  _TtT_  ";
	strTestRes = CUtilityFun::getInstance().sysStrUpper((char*)strTest.c_str());	

	CUtilityFun::removeInstance();

}
/* CPPTEST_TEST_CASE_END test_sysStrUpper_1 */

/* CPPTEST_TEST_CASE_BEGIN test_sysStricmp_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CUtilityFun::sysStricmp(const char *, const char *) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_sysStricmp_1()
{ 
	CUtilityFun::getInstance();
	std::string _strLeft;
	std::string _strRight;	
	int _return  = 0;
	
	_strLeft = "Same";
	_strRight = "Same";
	_return  = ::CUtilityFun::sysStricmp(_strLeft.c_str(), _strRight.c_str());

	_strLeft = "Same";
	_strRight = "Diff";
	_return  = ::CUtilityFun::sysStricmp(_strLeft.c_str(), _strRight.c_str());

	CUtilityFun::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_sysStricmp_1 */

/* CPPTEST_TEST_CASE_BEGIN test_trim_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string & CUtilityFun::trim(std::string &) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_trim_1()
{
	CUtilityFun::getInstance();
	std::string strTest;
	std::string strTestRes;	

	strTest = "  _TTttT_  _TtT_  ";
	strTestRes = CUtilityFun::getInstance().trim(strTest);	

	CUtilityFun::removeInstance();

}
/* CPPTEST_TEST_CASE_END test_trim_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCUtilityFun_1 */
/* CPPTEST_TEST_CASE_CONTEXT CUtilityFun::~CUtilityFun(void) */
void TestSuite_UtilityFun_cpp_8afd5a96::test_x7eCUtilityFun_1()
{
	CUtilityFun::getInstance();
	CUtilityFun::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_x7eCUtilityFun_1 */
