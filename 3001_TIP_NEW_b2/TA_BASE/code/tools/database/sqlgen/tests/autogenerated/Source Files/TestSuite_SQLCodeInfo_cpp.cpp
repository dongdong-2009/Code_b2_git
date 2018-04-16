#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/SQLCodeInfo.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/SQLCodeInfo.cpp");

class TestSuite_SQLCodeInfo_cpp_c45c945b : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_SQLCodeInfo_cpp_c45c945b);
        CPPTEST_TEST(test_CSQLCodeInfo_1);
        CPPTEST_TEST(test__FreeData_1);
        CPPTEST_TEST(test_analyzeDataToFiles_1);
        CPPTEST_TEST(test_analyzeData_1);
        CPPTEST_TEST(test_err_analyzeData_1);
		CPPTEST_TEST(test_err2_analyzeData_1);

        CPPTEST_TEST(test_checkOracleAndMysqlSqlNum_1);
        CPPTEST_TEST(test_err_checkOracleAndMysqlSqlNum_1);
        
        CPPTEST_TEST(test_setFileName_1);
        CPPTEST_TEST(test_x7eCSQLCodeInfo_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();
        
		CSQLCodeInfo*  _SampleSQLCodeInfo();


        void test_CSQLCodeInfo_1();
        void test__FreeData_1();
        void test_analyzeDataToFiles_1();
        void test_analyzeData_1();
        void test_err_analyzeData_1();
        void test_err2_analyzeData_1();
        void test_checkOracleAndMysqlSqlNum_1();
        void test_err_checkOracleAndMysqlSqlNum_1();
        void test_setFileName_1();
        void test_x7eCSQLCodeInfo_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_SQLCodeInfo_cpp_c45c945b);

void TestSuite_SQLCodeInfo_cpp_c45c945b::setUp()
{
}

void TestSuite_SQLCodeInfo_cpp_c45c945b::tearDown()
{
}

CSQLCodeInfo*  TestSuite_SQLCodeInfo_cpp_c45c945b::_SampleSQLCodeInfo()
{		
	CSQLCodeInfo* pSQLCodeInfo = new CSQLCodeInfo();
	VrtLinesConT vrtAllLines;
	std::string strFileName = defSQLFileName_IN_SQLCODETXT;
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

	strLine = "[plan_type_map]";
	vrtAllLines.push_back(strLine);

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_TYPE_MAP_SELECT_80001=select fk_node_id, fk_plan_type from PLAN_TYPE_MAP"; 
	vrtAllLines.push_back(strLine);
	pSQLCodeInfo->setFileName(strFileName);
	pSQLCodeInfo->analyzeData(vrtAllLines);	
	
	return pSQLCodeInfo;
}


/* CPPTEST_TEST_CASE_BEGIN test_CSQLCodeInfo_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLCodeInfo::CSQLCodeInfo(void) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_CSQLCodeInfo_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	DEF_DELETE(pSQLCodeInfo);	
}
/* CPPTEST_TEST_CASE_END test_CSQLCodeInfo_1 */

/* CPPTEST_TEST_CASE_BEGIN test__FreeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CSQLCodeInfo::_FreeData(void) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test__FreeData_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	DEF_DELETE(pSQLCodeInfo);	

}
/* CPPTEST_TEST_CASE_END test__FreeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeDataToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLCodeInfo::analyzeDataToFiles(void) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_analyzeDataToFiles_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	pSQLCodeInfo->analyzeDataToFiles();
	
	{
		std::string strFileName;
		pSQLCodeInfo->setFileName(strFileName);
		pSQLCodeInfo->analyzeDataToFiles();
	}
	DEF_DELETE(pSQLCodeInfo);		

}
/* CPPTEST_TEST_CASE_END test_analyzeDataToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLCodeInfo::analyzeData(VrtLinesConT &) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_analyzeData_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	DEF_DELETE(pSQLCodeInfo);	
}

void TestSuite_SQLCodeInfo_cpp_c45c945b::test_err_analyzeData_1()
{
	CSQLCodeInfo* pSQLCodeInfo = new CSQLCodeInfo();
	VrtLinesConT vrtAllLines;
	std::string strLine;

	//empty line
	strLine.clear();
	vrtAllLines.push_back(strLine);

	strLine = "[PLAN_DSS_DP_RULE]";
	vrtAllLines.push_back(strLine);

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_INSERT_79006=insert into PLAN_DSS_DP_RULE";
	vrtAllLines.push_back(strLine);

	strLine = "[[Oracle]]";
	vrtAllLines.push_back(strLine);

	//invalid sqlID
	strLine = "PLAN_DSS_DP_RULE_SELECT_79001_ERROR=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=SELECT func_seq_get_next_number('PLAN_DSS_SEQ')";
	vrtAllLines.push_back(strLine);

	strLine = "[plan_type_map]";
	vrtAllLines.push_back(strLine);

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_TYPE_MAP_SELECT_80001=select fk_node_id, fk_plan_type from PLAN_TYPE_MAP"; 
	vrtAllLines.push_back(strLine);

	pSQLCodeInfo->analyzeData(vrtAllLines);	
	
	DEF_DELETE(pSQLCodeInfo);	
}

void TestSuite_SQLCodeInfo_cpp_c45c945b::test_err2_analyzeData_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	DEF_DELETE(pSQLCodeInfo);	
}
/* CPPTEST_TEST_CASE_END test_analyzeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_checkOracleAndMysqlSqlNum_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLCodeInfo::checkOracleAndMysqlSqlNum(void) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_checkOracleAndMysqlSqlNum_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	pSQLCodeInfo->checkOracleAndMysqlSqlNum();
	

	{
		std::string strFileName;
		pSQLCodeInfo->setFileName(strFileName);
		pSQLCodeInfo->checkOracleAndMysqlSqlNum();
	}
	
	DEF_DELETE(pSQLCodeInfo);	
}

void TestSuite_SQLCodeInfo_cpp_c45c945b::test_err_checkOracleAndMysqlSqlNum_1()
{
	CSQLCodeInfo* pSQLCodeInfo = new CSQLCodeInfo();
	VrtLinesConT vrtAllLines;
	std::string strFileName = defSQLFileName_IN_SQLCODETXT;
	std::string strLine;

	//empty line
	strLine.clear();
	vrtAllLines.push_back(strLine);

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

	//invalid sqlID	NUM

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);
	
	strLine = "PLAN_DSS_DP_RULE_SELECT_79002=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	pSQLCodeInfo->analyzeData(vrtAllLines);
	pSQLCodeInfo->setFileName(strFileName);	
	pSQLCodeInfo->checkOracleAndMysqlSqlNum();

	DEF_DELETE(pSQLCodeInfo);	
}
/* CPPTEST_TEST_CASE_END test_checkOracleAndMysqlSqlNum_1 */

/* CPPTEST_TEST_CASE_BEGIN test_setFileName_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLCodeInfo::setFileName(const std::string &) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_setFileName_1()
{
	std::string _strFileName  = "";
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();
	
	_strFileName  = defSQLFileName_IN_SQLCODETXT;
	pSQLCodeInfo->setFileName(_strFileName);
	DEF_DELETE(pSQLCodeInfo);
	
}
/* CPPTEST_TEST_CASE_END test_setFileName_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCSQLCodeInfo_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLCodeInfo::~CSQLCodeInfo(void) */
void TestSuite_SQLCodeInfo_cpp_c45c945b::test_x7eCSQLCodeInfo_1()
{
	CSQLCodeInfo* pSQLCodeInfo = _SampleSQLCodeInfo();

	DEF_DELETE(pSQLCodeInfo);
}
/* CPPTEST_TEST_CASE_END test_x7eCSQLCodeInfo_1 */
