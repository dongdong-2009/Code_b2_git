#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/TableInfo.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/TableInfo.cpp");

class TestSuite_TableInfo_cpp_778d795e : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_TableInfo_cpp_778d795e);
        CPPTEST_TEST(test_CTableInfo_1);
        CPPTEST_TEST(test__FreeData_1);
        CPPTEST_TEST(test_analyzeDataToFiles_1);
        
        CPPTEST_TEST(test_analyzeData_1);
		CPPTEST_TEST(test_err_analyzeData_1);

        CPPTEST_TEST(test_checkOracleAndMysqlSqlNum_1);
		CPPTEST_TEST(test_err_checkOracleAndMysqlSqlNum_1);

        CPPTEST_TEST(test_getTableName_1);
        CPPTEST_TEST(test_x7eCTableInfo_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

		CTableInfo* _SampleTableInfo();


        void test_CTableInfo_1();
        void test__FreeData_1();
        void test_analyzeDataToFiles_1();
        void test_analyzeData_1();
        void test_err_analyzeData_1();
        
        void test_checkOracleAndMysqlSqlNum_1();
        void test_err_checkOracleAndMysqlSqlNum_1();
        
        void test_getTableName_1();
        void test_x7eCTableInfo_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_TableInfo_cpp_778d795e);

void TestSuite_TableInfo_cpp_778d795e::setUp()
{
}

void TestSuite_TableInfo_cpp_778d795e::tearDown()
{
}


CTableInfo* TestSuite_TableInfo_cpp_778d795e::_SampleTableInfo()
{
	CTableInfo* pTableInfo = new CTableInfo();
	VrtLinesConT vrtAllLines;
	std::string strLine;

	strLine = "[PLAN_DSS_DP_RULE]";
	vrtAllLines.push_back(strLine);
	
	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_INSERT_79006=insert into PLAN_DSS_DP_RULE";
	vrtAllLines.push_back(strLine);

	strLine = "[[Oracle]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=SELECT func_seq_get_next_number('PLAN_DSS_SEQ')";
	vrtAllLines.push_back(strLine);

	pTableInfo->analyzeData(vrtAllLines);	

	
	return pTableInfo;
}

/* CPPTEST_TEST_CASE_BEGIN test_CTableInfo_1 */
/* CPPTEST_TEST_CASE_CONTEXT CTableInfo::CTableInfo(void) */
void TestSuite_TableInfo_cpp_778d795e::test_CTableInfo_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();
	
	DEF_DELETE(pTableInfo);
}
/* CPPTEST_TEST_CASE_END test_CTableInfo_1 */

/* CPPTEST_TEST_CASE_BEGIN test__FreeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CTableInfo::_FreeData(void) */
void TestSuite_TableInfo_cpp_778d795e::test__FreeData_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();

	DEF_DELETE(pTableInfo);

}
/* CPPTEST_TEST_CASE_END test__FreeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeDataToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CTableInfo::analyzeDataToFiles(void) */
void TestSuite_TableInfo_cpp_778d795e::test_analyzeDataToFiles_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();

	pTableInfo->analyzeDataToFiles();
	DEF_DELETE(pTableInfo);

}





/* CPPTEST_TEST_CASE_END test_analyzeDataToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CTableInfo::analyzeData(VrtLinesConT &) */
void TestSuite_TableInfo_cpp_778d795e::test_analyzeData_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();

	DEF_DELETE(pTableInfo);

}

void TestSuite_TableInfo_cpp_778d795e::test_err_analyzeData_1()
{
	CTableInfo* pTableInfo = new CTableInfo();
	VrtLinesConT vrtAllLines;
	std::string strLine;
	int nFunRes = 0;
	
	nFunRes = pTableInfo->analyzeData(vrtAllLines);


	strLine = "[PLAN_DSS_DP_RULE]";
	vrtAllLines.push_back(strLine);

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_INSERT_79006=insert into PLAN_DSS_DP_RULE";
	vrtAllLines.push_back(strLine);

	strLine = "[[Oracle]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);

	strLine.clear();
	vrtAllLines.push_back(strLine);

	nFunRes = pTableInfo->analyzeData(vrtAllLines);


	DEF_DELETE(pTableInfo);


}
/* CPPTEST_TEST_CASE_END test_analyzeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_checkOracleAndMysqlSqlNum_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CTableInfo::checkOracleAndMysqlSqlNum(void) */
void TestSuite_TableInfo_cpp_778d795e::test_checkOracleAndMysqlSqlNum_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();

	pTableInfo->checkOracleAndMysqlSqlNum();
	DEF_DELETE(pTableInfo);

}

void TestSuite_TableInfo_cpp_778d795e::test_err_checkOracleAndMysqlSqlNum_1()
{
	CTableInfo* pTableInfo = new CTableInfo();
	VrtLinesConT vrtAllLines;
	std::string strLine;

	strLine = "[PLAN_DSS_DP_RULE]";
	vrtAllLines.push_back(strLine);

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_INSERT_79006=insert into PLAN_DSS_DP_RULE";
	vrtAllLines.push_back(strLine);

	strLine = "[[Oracle]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=SELECT func_seq_get_next_number('PLAN_DSS_SEQ')";
	vrtAllLines.push_back(strLine);
	
	strLine = "PLAN_DSS_DP_RULE_SELECT_79002=SELECT func_seq_get_next_number('PLAN_DSS_SEQ')";
	vrtAllLines.push_back(strLine);

	pTableInfo->analyzeData(vrtAllLines);	
					
	pTableInfo->checkOracleAndMysqlSqlNum();
	DEF_DELETE(pTableInfo);

}
/* CPPTEST_TEST_CASE_END test_checkOracleAndMysqlSqlNum_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getTableName_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CTableInfo::getTableName(void) */
void TestSuite_TableInfo_cpp_778d795e::test_getTableName_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();
	std::string strTableName;
	
	strTableName = pTableInfo->getTableName();
	DEF_DELETE(pTableInfo);

}
/* CPPTEST_TEST_CASE_END test_getTableName_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCTableInfo_1 */
/* CPPTEST_TEST_CASE_CONTEXT CTableInfo::~CTableInfo(void) */
void TestSuite_TableInfo_cpp_778d795e::test_x7eCTableInfo_1()
{
	CTableInfo* pTableInfo = _SampleTableInfo();

	DEF_DELETE(pTableInfo);
}
/* CPPTEST_TEST_CASE_END test_x7eCTableInfo_1 */
