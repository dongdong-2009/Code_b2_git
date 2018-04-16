#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/SQLType.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/SQLType.cpp");

class TestSuite_SQLType_cpp_5849352b : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_SQLType_cpp_5849352b);
        CPPTEST_TEST(test_CSQLType_1);
        CPPTEST_TEST(test__FreeData_1);
        CPPTEST_TEST(test_analyzeDataTypeCommonOracleMysql_1);
        CPPTEST_TEST(test_err_analyzeDataTypeCommonOracleMysql_1);
        
        CPPTEST_TEST(test_analyzeDataTypeCommon_1);
        CPPTEST_TEST(test_analyzeDataTypeOracleMysql_1);
        CPPTEST_TEST(test_err_analyzeDataTypeOracleMysql_1);
        
        CPPTEST_TEST(test_analyzeData_1);
        CPPTEST_TEST(test_checkSqlIDMatch_1);
        CPPTEST_TEST(test_getDBName_1);
        CPPTEST_TEST(test_getDataNum_1);
        CPPTEST_TEST(test_x7eCSQLType_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();
        
		CSQLType* _SampleSQLType_common();
		CSQLType* _SampleSQLType_Oracle(); 
		CSQLType* _SampleSQLType_MySql();


        void test_CSQLType_1();
        void test__FreeData_1();
        void test_analyzeDataTypeCommonOracleMysql_1();
        void test_err_analyzeDataTypeCommonOracleMysql_1();
        void test_analyzeDataTypeCommon_1();
        void test_analyzeDataTypeOracleMysql_1();
        void test_err_analyzeDataTypeOracleMysql_1();
        void test_analyzeData_1();
        void test_checkSqlIDMatch_1();
        void test_getDBName_1();
        void test_getDataNum_1();
        void test_x7eCSQLType_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_SQLType_cpp_5849352b);

void TestSuite_SQLType_cpp_5849352b::setUp()
{
}

void TestSuite_SQLType_cpp_5849352b::tearDown()
{
}

CSQLType* TestSuite_SQLType_cpp_5849352b::_SampleSQLType_common()
{
	
	CSQLType* pSQLType = new CSQLType();
	VrtLinesConT vrtAllLines;
	std::string strLine;

	strLine = "[[common]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_INSERT_79006=insert into PLAN_DSS_DP_RULE";
	vrtAllLines.push_back(strLine);

	pSQLType->analyzeData(vrtAllLines);	

	return pSQLType;
}

CSQLType* TestSuite_SQLType_cpp_5849352b::_SampleSQLType_Oracle()
{
	CSQLType* pSQLType = new CSQLType();
	VrtLinesConT vrtAllLines;
	std::string strLine;
	  	
	strLine = "[[Oracle]]";
	vrtAllLines.push_back(strLine);

	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	vrtAllLines.push_back(strLine);

	pSQLType->analyzeData(vrtAllLines);	

	return pSQLType;
}

CSQLType* TestSuite_SQLType_cpp_5849352b::_SampleSQLType_MySql()
{
	CSQLType* pSQLType = new CSQLType();
	VrtLinesConT vrtAllLines;
	std::string strLine;

	strLine = "[[MySQL]]";
	vrtAllLines.push_back(strLine);
	
	strLine = "PLAN_DSS_DP_RULE_SELECT_79001=SELECT func_seq_get_next_number('PLAN_DSS_SEQ')";
	vrtAllLines.push_back(strLine);

	pSQLType->analyzeData(vrtAllLines);	

	return pSQLType;
}


/* CPPTEST_TEST_CASE_BEGIN test_CSQLType_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLType::CSQLType(void) */
void TestSuite_SQLType_cpp_5849352b::test_CSQLType_1()
{
	CSQLDataProcessor::getInstance();
	CSQLType* pSQLType = _SampleSQLType_common();
	DEF_DELETE(pSQLType);
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_CSQLType_1 */

/* CPPTEST_TEST_CASE_BEGIN test__FreeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CSQLType::_FreeData(void) */
void TestSuite_SQLType_cpp_5849352b::test__FreeData_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	CSQLType* pSQLType = _SampleSQLType_common();
	DEF_DELETE(pSQLType);
	CSQLDataProcessor::removeInstance();

}
/* CPPTEST_TEST_CASE_END test__FreeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeDataTypeCommonOracleMysql_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::analyzeDataTypeCommonOracleMysql(const std::string &, CSQLType *, CSQLType *) */
void TestSuite_SQLType_cpp_5849352b::test_analyzeDataTypeCommonOracleMysql_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeCommon = _SampleSQLType_common();
	CSQLType* pSQLTypeOracle = _SampleSQLType_Oracle();
	CSQLType* pSQLTypeMysql = _SampleSQLType_MySql();


	pSQLTypeCommon->analyzeDataTypeCommonOracleMysql(_strIndexTableName, pSQLTypeOracle, pSQLTypeMysql);
	DEF_DELETE(pSQLTypeCommon);
	DEF_DELETE(pSQLTypeOracle);
	DEF_DELETE(pSQLTypeMysql);
	 CSQLDataProcessor::removeInstance();
}

void TestSuite_SQLType_cpp_5849352b::test_err_analyzeDataTypeCommonOracleMysql_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeCommon = _SampleSQLType_common();
	CSQLType* pSQLTypeMysql = _SampleSQLType_MySql(); 		   

	{
		CSQLType* pSQLTypeOracle2 = new CSQLType();
		VrtLinesConT vrtAllLines;
		std::string strLine;

		strLine = "[[Oracle]]";
		vrtAllLines.push_back(strLine);

		strLine = "PLAN_DSS_DP_RULE_SELECT_79001_ERROR=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		vrtAllLines.push_back(strLine);

		pSQLTypeOracle2->analyzeData(vrtAllLines);	

		pSQLTypeCommon->analyzeDataTypeCommonOracleMysql(_strIndexTableName, pSQLTypeOracle2, pSQLTypeMysql);

		DEF_DELETE(pSQLTypeOracle2);
		vrtAllLines.clear();
	}
	
	DEF_DELETE(pSQLTypeCommon);
	DEF_DELETE(pSQLTypeMysql);
	
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_analyzeDataTypeCommonOracleMysql_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeDataTypeCommon_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::analyzeDataTypeCommon(const std::string &) */
void TestSuite_SQLType_cpp_5849352b::test_analyzeDataTypeCommon_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeCommon = _SampleSQLType_common();

	pSQLTypeCommon->analyzeDataTypeCommon(_strIndexTableName);
	DEF_DELETE(pSQLTypeCommon);
	CSQLDataProcessor::removeInstance();	 	
}
/* CPPTEST_TEST_CASE_END test_analyzeDataTypeCommon_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeDataTypeOracleMysql_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::analyzeDataTypeOracleMysql(const std::string &, CSQLType *) */
void TestSuite_SQLType_cpp_5849352b::test_analyzeDataTypeOracleMysql_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeOracle = _SampleSQLType_Oracle();
	CSQLType* pSQLTypeMysql = _SampleSQLType_MySql();


	pSQLTypeOracle->analyzeDataTypeOracleMysql(_strIndexTableName, pSQLTypeMysql);
	DEF_DELETE(pSQLTypeOracle);
	DEF_DELETE(pSQLTypeMysql);
	CSQLDataProcessor::removeInstance();
}


void TestSuite_SQLType_cpp_5849352b::test_err_analyzeDataTypeOracleMysql_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeOracle = _SampleSQLType_Oracle();
	CSQLType* pSQLTypeMysql = _SampleSQLType_MySql();


	pSQLTypeOracle->analyzeDataTypeOracleMysql(_strIndexTableName, pSQLTypeMysql);
	pSQLTypeOracle->analyzeDataTypeOracleMysql(_strIndexTableName, pSQLTypeMysql);

	{
		CSQLType* pSQLTypeOracle2 = new CSQLType();
		VrtLinesConT vrtAllLines;
		std::string strLine;

		strLine = "[[Oracle]]";
		vrtAllLines.push_back(strLine);

		strLine = "PLAN_DSS_DP_RULE_SELECT_79001_ERROR=select PLAN_DSS_SEQ.NEXTVAL from DUAL";
		vrtAllLines.push_back(strLine);

		pSQLTypeOracle2->analyzeData(vrtAllLines);	
		pSQLTypeOracle2->analyzeDataTypeOracleMysql(_strIndexTableName, pSQLTypeMysql);
		DEF_DELETE(pSQLTypeOracle2);
		vrtAllLines.clear();
	}						 			 	

	DEF_DELETE(pSQLTypeOracle);
	DEF_DELETE(pSQLTypeMysql);
	CSQLDataProcessor::removeInstance();
}

/* CPPTEST_TEST_CASE_END test_analyzeDataTypeOracleMysql_1 */

/* CPPTEST_TEST_CASE_BEGIN test_analyzeData_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::analyzeData(VrtLinesConT &) */
void TestSuite_SQLType_cpp_5849352b::test_analyzeData_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	CSQLType* pSQLType = _SampleSQLType_common();
	DEF_DELETE(pSQLType);

	{
		CSQLType* pSQLType2 = new CSQLType();
		VrtLinesConT vrtAllLines; 
		pSQLType2->analyzeData(vrtAllLines);	
		DEF_DELETE(pSQLType2); 
	}
	
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_analyzeData_1 */

/* CPPTEST_TEST_CASE_BEGIN test_checkSqlIDMatch_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::checkSqlIDMatch(const std::string &, std::string &) */
void TestSuite_SQLType_cpp_5849352b::test_checkSqlIDMatch_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string _strIndexTableName  = "[PLAN_DSS_DP_RULE]";
	CSQLType* pSQLTypeMysql = _SampleSQLType_MySql();
	std::string strSqlID;
	std::string strOUTMysqlSqlLine;
	
	strSqlID = "PLAN_DSS_DP_RULE_SELECT_79001";

	pSQLTypeMysql->checkSqlIDMatch(strSqlID, strOUTMysqlSqlLine);
	
	strSqlID = "ERRORFORTEST";

	pSQLTypeMysql->checkSqlIDMatch(strSqlID, strOUTMysqlSqlLine);

	DEF_DELETE(pSQLTypeMysql);
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_checkSqlIDMatch_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDBName_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CSQLType::getDBName(void) */
void TestSuite_SQLType_cpp_5849352b::test_getDBName_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	std::string strDBName;
	CSQLType* pSQLType = _SampleSQLType_common();
	strDBName = pSQLType->getDBName();
	DEF_DELETE(pSQLType);
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_getDBName_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDataNum_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLType::getDataNum(void) */
void TestSuite_SQLType_cpp_5849352b::test_getDataNum_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	int nDataNum = 0;
	CSQLType* pSQLType = _SampleSQLType_common();
	nDataNum = pSQLType->getDataNum();
	DEF_DELETE(pSQLType);
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_getDataNum_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCSQLType_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLType::~CSQLType(void) */
void TestSuite_SQLType_cpp_5849352b::test_x7eCSQLType_1()
{
	CSQLDataProcessor::getInstance();
	//CSQLDataProcessor::removeInstance();

	CSQLType* pSQLType = _SampleSQLType_common();
	DEF_DELETE(pSQLType);
	CSQLDataProcessor::removeInstance();
}
/* CPPTEST_TEST_CASE_END test_x7eCSQLType_1 */
