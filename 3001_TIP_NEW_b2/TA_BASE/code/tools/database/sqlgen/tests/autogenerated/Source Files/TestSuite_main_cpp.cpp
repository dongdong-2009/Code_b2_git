#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/main.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/main.cpp");

class TestSuite_main_cpp_82f66db2 : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_main_cpp_82f66db2);
        CPPTEST_TEST(test_main_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

        void test_main_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_main_cpp_82f66db2);

void TestSuite_main_cpp_82f66db2::setUp()
{
}

void TestSuite_main_cpp_82f66db2::tearDown()
{
}

/* CPPTEST_TEST_CASE_BEGIN test_main_1 */
/* CPPTEST_TEST_CASE_CONTEXT int main(int, char **) */
void TestSuite_main_cpp_82f66db2::test_main_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);

	//defSQLFileName_Out_SQLH					"sql.h"			 /*OUT*/
	//defSQLFileName_Out_SQLMACRODEFH			"sqlmacrodef.h"	 /*OUT*/

	int _return  = ::main(_argc, _argv);
}
/* CPPTEST_TEST_CASE_END test_main_1 */
