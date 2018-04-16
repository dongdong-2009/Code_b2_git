#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/SQLDataProcessor.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/SQLDataProcessor.cpp");

class TestSuite_SQLDataProcessor_cpp_ab76d74d : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_SQLDataProcessor_cpp_ab76d74d);
        CPPTEST_TEST(test_CSQLDataProcessor_1);
        CPPTEST_TEST(test__WriteToFile_1);
        CPPTEST_TEST(test_addDataToFileSQLH_1);
        CPPTEST_TEST(test_addDataToFileSQLMACRODEFH_1);
        CPPTEST_TEST(test_checkDuplicateSqlID_1);
        CPPTEST_TEST(test_getDataForFileTypeCommon_1);
        CPPTEST_TEST(test_getDataForFileTypeOracleMySql_1);
        CPPTEST_TEST(test_getIndexTableName_1);
        CPPTEST_TEST(test_getInstance_1);
        CPPTEST_TEST(test_removeInstance_1);
        CPPTEST_TEST(test_writeToFileSQLH_1);
        CPPTEST_TEST(test_writeToFileSQLMACRODEFH_1);
        CPPTEST_TEST(test_x7eCSQLDataProcessor_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

        void test_CSQLDataProcessor_1();
        void test__WriteToFile_1();
        void test_addDataToFileSQLH_1();
        void test_addDataToFileSQLMACRODEFH_1();
        void test_checkDuplicateSqlID_1();
        void test_getDataForFileTypeCommon_1();
        void test_getDataForFileTypeOracleMySql_1();
        void test_getIndexTableName_1();
        void test_getInstance_1();
        void test_removeInstance_1();
        void test_writeToFileSQLH_1();
        void test_writeToFileSQLMACRODEFH_1();
        void test_x7eCSQLDataProcessor_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_SQLDataProcessor_cpp_ab76d74d);

void TestSuite_SQLDataProcessor_cpp_ab76d74d::setUp()
{
}

void TestSuite_SQLDataProcessor_cpp_ab76d74d::tearDown()
{
}

/* CPPTEST_TEST_CASE_BEGIN test_CSQLDataProcessor_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLDataProcessor::CSQLDataProcessor(void) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_CSQLDataProcessor_1()
{
	CSQLDataProcessor::getInstance();
	CSQLDataProcessor::removeInstance();

}
/* CPPTEST_TEST_CASE_END test_CSQLDataProcessor_1 */

/* CPPTEST_TEST_CASE_BEGIN test__WriteToFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::_WriteToFile(VrtLinesConT &, std::ofstream *) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test__WriteToFile_1()
{
	CSQLDataProcessor::getInstance();
	std::ofstream ofstreamSQLH;
	std::string strDirSqlH = defSQLFileName_Out_SQLH;
	VrtLinesConT vrtLines;
	std::string strLine;
	std::ofstream *pOfStream = NULL;
	int nFunRes = 0;
	
	nFunRes = CSQLDataProcessor::getInstance()._WriteToFile(vrtLines, pOfStream);

	strLine = SQLH_SQLMACRODEFH_FILE_COMMENT_HEADER;
	vrtLines.push_back(strLine);
	
	remove(strDirSqlH.c_str());
	ofstreamSQLH.open(strDirSqlH.c_str());
	if (ofstreamSQLH.is_open())
	{
	   pOfStream = &ofstreamSQLH;
	   CSQLDataProcessor::getInstance()._WriteToFile(vrtLines, pOfStream);
	}
	ofstreamSQLH.close();  	
	CSQLDataProcessor::removeInstance();
	remove(strDirSqlH.c_str());

}
/* CPPTEST_TEST_CASE_END test__WriteToFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test_addDataToFileSQLH_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::addDataToFileSQLH(const std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_addDataToFileSQLH_1()
{
	CSQLDataProcessor::getInstance();
	std::string _strLine;
	int nFunRes = 0; 

	_strLine = SQLH_SQLMACRODEFH_FILE_COMMENT_HEADER;
	nFunRes = CSQLDataProcessor::getInstance().addDataToFileSQLH(_strLine);

	CSQLDataProcessor::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test_addDataToFileSQLH_1 */

/* CPPTEST_TEST_CASE_BEGIN test_addDataToFileSQLMACRODEFH_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::addDataToFileSQLMACRODEFH(const std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_addDataToFileSQLMACRODEFH_1()
{
	CSQLDataProcessor::getInstance();
	std::string _strLine;
	int nFunRes = 0; 

	_strLine = SQLMACRODEFH_FILE_BEGIN;
	nFunRes = CSQLDataProcessor::getInstance().addDataToFileSQLMACRODEFH(_strLine);

	CSQLDataProcessor::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test_addDataToFileSQLMACRODEFH_1 */

/* CPPTEST_TEST_CASE_BEGIN test_checkDuplicateSqlID_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::checkDuplicateSqlID(const std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_checkDuplicateSqlID_1()
{
	CSQLDataProcessor::getInstance();
	std::string _strSqlIDCheck;
	int nFunRes = 0; 

	nFunRes = CSQLDataProcessor::getInstance().checkDuplicateSqlID(_strSqlIDCheck);

	_strSqlIDCheck  = "PLAN_DSS_DP_RULE_SELECT_79001";
	nFunRes = CSQLDataProcessor::getInstance().checkDuplicateSqlID(_strSqlIDCheck);

	CSQLDataProcessor::removeInstance();
	
}
/* CPPTEST_TEST_CASE_END test_checkDuplicateSqlID_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDataForFileTypeCommon_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::getDataForFileTypeCommon(const std::string &, const std::string &, std::string &, std::string &, std::string &, std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_getDataForFileTypeCommon_1()
{
	std::string _strINSqlID  = "PLAN_DSS_DP_RULE_SELECT_79001";
	std::string _strINSqlLine  = "select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	std::string _strOUTDefineID  = "";
	std::string _strOUTSQLID = "";
	std::string _strOUTCommonSQL  = "";
	std::string _strOUTOracleSQL  = "";
	std::string _strOUTMySqlSQL  = "";
	int _return  = 0;
	
	CSQLDataProcessor::getInstance();
	
	_return  = CSQLDataProcessor::getInstance().getDataForFileTypeCommon(_strINSqlID, _strINSqlLine, _strOUTDefineID, _strOUTSQLID, _strOUTCommonSQL, _strOUTOracleSQL, _strOUTMySqlSQL);
	
	CSQLDataProcessor::removeInstance();  	
	
}
/* CPPTEST_TEST_CASE_END test_getDataForFileTypeCommon_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getDataForFileTypeOracleMySql_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::getDataForFileTypeOracleMySql(const std::string &, const std::string &, const std::string &, std::string &, std::string &, std::string &, std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_getDataForFileTypeOracleMySql_1()
{
	std::string _strINSqlID  = "PLAN_DSS_DP_RULE_SELECT_79001";
	std::string _strINOracleSQLLine  = "select PLAN_DSS_SEQ.NEXTVAL from DUAL";
	std::string _strINMySqlSQLLine  = "select PLAN_DSS_SEQ.NEXTVAL from DUAL";

	std::string _strOUTDefineID  = "";
	std::string _strOUTSQLID = "";
	std::string _strOUTCommonSQL  = "";
	std::string _strOUTOracleSQL  = "";
	std::string _strOUTMySqlSQL  = "";
	int _return  = 0;
	
	CSQLDataProcessor::getInstance();

	_return  = CSQLDataProcessor::getInstance().getDataForFileTypeOracleMySql(_strINSqlID, 
		_strINOracleSQLLine, _strINMySqlSQLLine, _strOUTDefineID, _strOUTSQLID, _strOUTCommonSQL, _strOUTOracleSQL, _strOUTMySqlSQL);
	
	CSQLDataProcessor::removeInstance();  	

}
/* CPPTEST_TEST_CASE_END test_getDataForFileTypeOracleMySql_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getIndexTableName_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CSQLDataProcessor::getIndexTableName(const std::string &) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_getIndexTableName_1()
{
	std::string _strTableName  = "[PLAN_DSS_DP_RULE]";
	std::string strGetIndexTableName;

	CSQLDataProcessor::getInstance();

	strGetIndexTableName  = CSQLDataProcessor::getInstance().getIndexTableName(_strTableName);
	
	CSQLDataProcessor::removeInstance(); 

}
/* CPPTEST_TEST_CASE_END test_getIndexTableName_1 */

/* CPPTEST_TEST_CASE_BEGIN test_getInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLDataProcessor & CSQLDataProcessor::getInstance(void) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_getInstance_1()
{
	CSQLDataProcessor::getInstance();

	CSQLDataProcessor::removeInstance(); 

}
/* CPPTEST_TEST_CASE_END test_getInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_removeInstance_1 */
/* CPPTEST_TEST_CASE_CONTEXT void CSQLDataProcessor::removeInstance(void) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_removeInstance_1()
{
	CSQLDataProcessor::getInstance();

	CSQLDataProcessor::removeInstance(); 
}
/* CPPTEST_TEST_CASE_END test_removeInstance_1 */

/* CPPTEST_TEST_CASE_BEGIN test_writeToFileSQLH_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::writeToFileSQLH(std::ofstream *) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_writeToFileSQLH_1()
{

	CSQLDataProcessor::getInstance();
	std::ofstream ofstreamSQLH;
	std::string strDirSqlH = defSQLFileName_Out_SQLH;
	std::ofstream *pOfStream = NULL;
	int nFunRes = 0;
	 
	remove(strDirSqlH.c_str());
	ofstreamSQLH.open(strDirSqlH.c_str());
	if (ofstreamSQLH.is_open())
	{
		pOfStream = &ofstreamSQLH;
		CSQLDataProcessor::getInstance().writeToFileSQLH(pOfStream);
	}
	ofstreamSQLH.close();  	
	CSQLDataProcessor::removeInstance();
	remove(strDirSqlH.c_str());
	
	
}
/* CPPTEST_TEST_CASE_END test_writeToFileSQLH_1 */

/* CPPTEST_TEST_CASE_BEGIN test_writeToFileSQLMACRODEFH_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CSQLDataProcessor::writeToFileSQLMACRODEFH(std::ofstream *) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_writeToFileSQLMACRODEFH_1()
{	 	
	CSQLDataProcessor::getInstance();
	std::ofstream ofstreamSQLMACRODEFH;
	std::string strDirSQLMACRODEFH = defSQLFileName_Out_SQLMACRODEFH;
	std::ofstream *pOfStream = NULL;
	int nFunRes = 0;

	remove(strDirSQLMACRODEFH.c_str());
	ofstreamSQLMACRODEFH.open(strDirSQLMACRODEFH.c_str());
	if (ofstreamSQLMACRODEFH.is_open())
	{
		pOfStream = &ofstreamSQLMACRODEFH;
		CSQLDataProcessor::getInstance().writeToFileSQLH(pOfStream);
	}
	ofstreamSQLMACRODEFH.close();  	
	CSQLDataProcessor::removeInstance();
	remove(strDirSQLMACRODEFH.c_str());

}
/* CPPTEST_TEST_CASE_END test_writeToFileSQLMACRODEFH_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCSQLDataProcessor_1 */
/* CPPTEST_TEST_CASE_CONTEXT CSQLDataProcessor::~CSQLDataProcessor(void) */
void TestSuite_SQLDataProcessor_cpp_ab76d74d::test_x7eCSQLDataProcessor_1()
{
	CSQLDataProcessor::getInstance();

	CSQLDataProcessor::removeInstance(); 
}
/* CPPTEST_TEST_CASE_END test_x7eCSQLDataProcessor_1 */
