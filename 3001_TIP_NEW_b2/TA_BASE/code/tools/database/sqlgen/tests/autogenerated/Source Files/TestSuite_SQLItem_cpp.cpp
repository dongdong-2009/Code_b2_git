#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/SQLItem.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/SQLItem.cpp");

class TestSuite_SQLItem_cpp_f9415170 : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_SQLItem_cpp_f9415170);
        CPPTEST_TEST(test_CSQLItem_1);
        CPPTEST_TEST(test__CheckSqlID_1);
        CPPTEST_TEST(test__FreeData_1);
        CPPTEST_TEST(test_analyzeData_1);
        CPPTEST_TEST(test_checkDuplicateSqlID_1);
        CPPTEST_TEST(test_getDataForFileTypeCommon_1);
        CPPTEST_TEST(test_getDataForFileTypeOracleMySql_1);
        CPPTEST_TEST(test_getSqlID_1);
        CPPTEST_TEST(test_getSqlLine_1);
        CPPTEST_TEST(test_x7eCSQLItem_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

		CSQLItem* _SampleCSQLItem();


        void test_CSQLItem_1();
        void test__CheckSqlID_1();
        void test__FreeData_1();
        void test_analyzeData_1();
        void test_checkDuplicateSqlID_1();
        void test_getDataForFileTypeCommon_1();
        void test_getDataForFileTypeOracleMySql_1();
        void test_getSqlID_1();
        void test_getSqlLine_1();
        void test_x7eCSQLItem_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_SQLItem_cpp_f9415170);

void TestSuite_SQLItem_cpp_f9415170::setUp()
{
}

void TestSuite_SQLItem_cpp_f9415170::tearDown()
{
}

CSQLItem* TestSuite_SQLItem_cpp_f9415170::_SampleCSQLItem()
{
	CSQLItem* pSQLItem = new CSQLItem();
	std::string strLine;
	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
								   	
	pSQLItem->analyzeData(strLine);
	
	return pSQLItem;
}

/* CPPTEST_TEST_CASE_BEGIN test_CSQLItem_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLItem::CSQLItem(void) */
void TestSuite_SQLItem_cpp_f9415170::test_CSQLItem_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();

	DEF_DELETE(pSQLItem);
	
}
/* CPPTEST_TEST_CASE_END test_CSQLItem_1 */

/* CPPTEST_TEST_CASE_BEGIN test__CheckSqlID_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLItem::_CheckSqlID(void) */
void TestSuite_SQLItem_cpp_f9415170::test__CheckSqlID_1()
{	
	CSQLItem* pSQLItem = new CSQLItem();
	std::string strLine;
	
	{
		strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		pSQLItem->analyzeData(strLine);
		pSQLItem->_CheckSqlID();
	}
	
	{
		strLine = "PLAN_DSS_ DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		pSQLItem->analyzeData(strLine);
		pSQLItem->_CheckSqlID();
	}
	
	{
		strLine = "PLAN_DSS_DP_RULE_SELECT_=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		pSQLItem->analyzeData(strLine);
		pSQLItem->_CheckSqlID();
	}	

	DEF_DELETE(pSQLItem);
		
}
/* CPPTEST_TEST_CASE_END test__CheckSqlID_1 */

/* CPPTEST_TEST_CASE_BEGIN test__FreeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CSQLItem::_FreeData(void) */
void TestSuite_SQLItem_cpp_f9415170::test__FreeData_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();


	DEF_DELETE(pSQLItem); 
	
}
/* CPPTEST_TEST_CASE_END test__FreeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLItem::analyzeData(const std::string &) */
void TestSuite_SQLItem_cpp_f9415170::test_analyzeData_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();
	std::string strLine;
	
	strLine.clear();
	pSQLItem->analyzeData(strLine);
	
	strLine = "PLAN_DSS_DP_RULE_SELECT_79001select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	pSQLItem->analyzeData(strLine);	 


	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	pSQLItem->analyzeData(strLine);	 

	DEF_DELETE(pSQLItem); 
	
}
/* CPPTEST_TEST_CASE_END test_analyzeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_checkDuplicateSqlID_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLItem::checkDuplicateSqlID(void) */
void TestSuite_SQLItem_cpp_f9415170::test_checkDuplicateSqlID_1()
{
	CSQLItem* pSQLItem = new CSQLItem();
	std::string strLine;
	int nFunRes = 0;

	{
		strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		pSQLItem->analyzeData(strLine);
		nFunRes = pSQLItem->checkDuplicateSqlID();
	}
	
	{
		strLine = "PLAN_DSS_ DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		pSQLItem->analyzeData(strLine);
		nFunRes = pSQLItem->checkDuplicateSqlID();
	}
	 
	DEF_DELETE(pSQLItem);
	   	
}
/* CPPTEST_TEST_CASE_END test_checkDuplicateSqlID_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDataForFileTypeCommon_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLItem::getDataForFileTypeCommon(std::string &, std::string &, std::string &, std::string &) */
void TestSuite_SQLItem_cpp_f9415170::test_getDataForFileTypeCommon_1()
{

	CSQLItem* pSQLItem = _SampleCSQLItem();
	
    std::string _strOUTDefineID  = "";
 	std::string _strOUTSQLID = "";
    std::string _strOUTCommonSQL  = "";
    std::string _strOUTOracleSQL  = "";
    std::string _strOUTMySqlSQL  = "";
    int _return  = pSQLItem->getDataForFileTypeCommon(_strOUTDefineID, 
					_strOUTSQLID, _strOUTCommonSQL, _strOUTOracleSQL, _strOUTMySqlSQL);


	DEF_DELETE(pSQLItem); 

}
/* CPPTEST_TEST_CASE_END test_getDataForFileTypeCommon_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDataForFileTypeOracleMySql_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLItem::getDataForFileTypeOracleMySql(const std::string &, std::string &, std::string &, std::string &, std::string &) */
void TestSuite_SQLItem_cpp_f9415170::test_getDataForFileTypeOracleMySql_1()
{	
	CSQLItem* pSQLItem = _SampleCSQLItem();

	std::string _strINMySqlLine  = "select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	std::string _strOUTDefineID  = "";   
	std::string _strOUTSQLID = "";
	std::string _strOUTCommonSQL  = "";
	std::string _strOUTOracleSQL  = "";
	std::string _strOUTMySqlSQL  = "";
		
	int _return  = pSQLItem->getDataForFileTypeOracleMySql(_strINMySqlLine, 
		_strOUTDefineID, _strOUTSQLID, _strOUTCommonSQL, _strOUTOracleSQL, _strOUTMySqlSQL);
						  
	DEF_DELETE(pSQLItem);	
 
}
/* CPPTEST_TEST_CASE_END test_getDataForFileTypeOracleMySql_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getSqlID_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CSQLItem::getSqlID(void) */
void TestSuite_SQLItem_cpp_f9415170::test_getSqlID_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();
	std::string strSqlID;

	strSqlID = pSQLItem->getSqlID();

	DEF_DELETE(pSQLItem); 
	
}
/* CPPTEST_TEST_CASE_END test_getSqlID_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getSqlLine_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CSQLItem::getSqlLine(void) */
void TestSuite_SQLItem_cpp_f9415170::test_getSqlLine_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();
	std::string strSqlLine;

	strSqlLine = pSQLItem->getSqlLine();

	DEF_DELETE(pSQLItem); 
	
}
/* CPPTEST_TEST_CASE_END test_getSqlLine_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCSQLItem_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLItem::~CSQLItem(void) */
void TestSuite_SQLItem_cpp_f9415170::test_x7eCSQLItem_1()
{
	CSQLItem* pSQLItem = _SampleCSQLItem();

	DEF_DELETE(pSQLItem); 
}
/* CPPTEST_TEST_CASE_END test_x7eCSQLItem_1 */
