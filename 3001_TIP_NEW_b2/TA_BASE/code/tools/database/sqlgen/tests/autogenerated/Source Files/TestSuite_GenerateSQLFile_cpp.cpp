#include "cpptest.h"

CPPTEST_CONTEXT("../../../Source Files/GenerateSQLFile.cpp");
CPPTEST_TEST_SUITE_INCLUDED_TO("../../../Source Files/GenerateSQLFile.cpp");

class TestSuite_GenerateSQLFile_cpp_fe69d5de : public CppTest_TestSuite
{
    public:
        CPPTEST_TEST_SUITE(TestSuite_GenerateSQLFile_cpp_fe69d5de);
        CPPTEST_TEST(test_CGenerateSQLFile_1);
        CPPTEST_TEST(test__CheckFileUpdateTime_1);
		CPPTEST_TEST(test__err_CheckFileUpdateTime_1);
		CPPTEST_TEST(test__err2_CheckFileUpdateTime_1);
		CPPTEST_TEST(test__err3_CheckFileUpdateTime_1);
		CPPTEST_TEST(test__err4_CheckFileUpdateTime_1);			

        CPPTEST_TEST(test__CheckLine_1);
        CPPTEST_TEST(test__ClossAllOutPutFiles_1);
        CPPTEST_TEST(test__ExtractArg_1);
        CPPTEST_TEST(test__err_ExtractArg_1);        
        
        CPPTEST_TEST(test__FormatToWriteFileTimeStr_1);
        CPPTEST_TEST(test__GeneralSQLFiles_1);
		CPPTEST_TEST(test__err_GeneralSQLFiles_1);
		CPPTEST_TEST(test__err2_GeneralSQLFiles_1);
		CPPTEST_TEST(test__err3_GeneralSQLFiles_1);
		CPPTEST_TEST(test__err4_GeneralSQLFiles_1);
		CPPTEST_TEST(test__err5_GeneralSQLFiles_1);
		
		
        CPPTEST_TEST(test__GetFileLastUpdateTime_1);
        CPPTEST_TEST(test__err_GetFileLastUpdateTime_1);
        
        CPPTEST_TEST(test__GetFirstLineOfFile_1);
        CPPTEST_TEST(test__MutiLinesToSingleLine_1);
        CPPTEST_TEST(test__OpenAllOutPutFiles_1);
		CPPTEST_TEST(test__err_OpenAllOutPutFiles_1);
		CPPTEST_TEST(test__err2_OpenAllOutPutFiles_1);

        CPPTEST_TEST(test__ParseCmdLine_1);
        CPPTEST_TEST(test__err_ParseCmdLine_1);
        
        CPPTEST_TEST(test__ReadFile_1);
		CPPTEST_TEST(test__err_ReadFile_1);

        CPPTEST_TEST(test__RemoveBlackLine_1);
        CPPTEST_TEST(test__RemoveFile_1);
        CPPTEST_TEST(test__err_RemoveFile_1);
        
        CPPTEST_TEST(test__RemoveOldSQLFiles_1);
        CPPTEST_TEST(test__err_RemoveOldSQLFiles_1);
        CPPTEST_TEST(test__err2_RemoveOldSQLFiles_1);
        
        
        CPPTEST_TEST(test__TrimLines_1);
        CPPTEST_TEST(test__WriteBeginToFiles_1);
        CPPTEST_TEST(test__WriteDataToFiles_1);
        CPPTEST_TEST(test__err_WriteDataToFiles_1);
        
        CPPTEST_TEST(test__WriteEndToFiles_1);
        CPPTEST_TEST(test__WriteTimeStrToFiles_1);
        CPPTEST_TEST(test_generalFiles_1);
        CPPTEST_TEST(test_err_generalFiles_1);
        
        CPPTEST_TEST(test_setParam_1);
        CPPTEST_TEST(test_x7eCGenerateSQLFile_1);
        CPPTEST_TEST_SUITE_END();
        
        void setUp();
        void tearDown();

        void test_CGenerateSQLFile_1();
        void test__CheckFileUpdateTime_1();
		void test__err_CheckFileUpdateTime_1();
		void test__err2_CheckFileUpdateTime_1();
		void test__err3_CheckFileUpdateTime_1();
		void test__err4_CheckFileUpdateTime_1();

        void test__CheckLine_1();
        void test__ClossAllOutPutFiles_1();
        void test__ExtractArg_1();
        void test__err_ExtractArg_1();
        
        void test__FormatToWriteFileTimeStr_1();
        void test__GeneralSQLFiles_1();
		void test__err_GeneralSQLFiles_1();
		void test__err2_GeneralSQLFiles_1();
		void test__err3_GeneralSQLFiles_1();
		void test__err4_GeneralSQLFiles_1();
		void test__err5_GeneralSQLFiles_1();

        void test__GetFileLastUpdateTime_1();
        void test__err_GetFileLastUpdateTime_1();
        
        void test__GetFirstLineOfFile_1();
        void test__MutiLinesToSingleLine_1();
        void test__OpenAllOutPutFiles_1();
        void test__err_OpenAllOutPutFiles_1();
        void test__err2_OpenAllOutPutFiles_1();
        
        void test__ParseCmdLine_1();
        void test__err_ParseCmdLine_1();
        
        void test__ReadFile_1();
        void test__err_ReadFile_1();
        void test__RemoveBlackLine_1();
        void test__RemoveFile_1();
        void test__err_RemoveFile_1();
        
        void test__RemoveOldSQLFiles_1();
        void test__err_RemoveOldSQLFiles_1();
        void test__err2_RemoveOldSQLFiles_1();
        
        void test__TrimLines_1();
        void test__WriteBeginToFiles_1();
        void test__WriteDataToFiles_1();
        void test__err_WriteDataToFiles_1();
        
        void test__WriteEndToFiles_1();
        void test__WriteTimeStrToFiles_1();
        void test_generalFiles_1();
        void test_err_generalFiles_1();
        void test_setParam_1();
        void test_x7eCGenerateSQLFile_1();
};

CPPTEST_TEST_SUITE_REGISTRATION(TestSuite_GenerateSQLFile_cpp_fe69d5de);

void TestSuite_GenerateSQLFile_cpp_fe69d5de::setUp()
{
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::tearDown()
{
}

/* CPPTEST_TEST_CASE_BEGIN test_CGenerateSQLFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT CGenerateSQLFile::CGenerateSQLFile(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test_CGenerateSQLFile_1()
{	  
	CGenerateSQLFile *pGenerateSQLFile = NULL;

	pGenerateSQLFile = new CGenerateSQLFile();
	DEF_DELETE(pGenerateSQLFile);
    
}
/* CPPTEST_TEST_CASE_END test_CGenerateSQLFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test__CheckFileUpdateTime_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_CheckFileUpdateTime(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__CheckFileUpdateTime_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int nFunRes  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
	
	pGenerateSQLFile->_RemoveOldSQLFiles();
	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->m_strFileTimeSqlcodeTXT.clear();
	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();			   
	
	nFunRes  = pGenerateSQLFile->_CheckFileUpdateTime();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);	
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_CheckFileUpdateTime_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int nFunRes  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	pGenerateSQLFile->_RemoveOldSQLFiles();
	//no sql.h
	nFunRes  = pGenerateSQLFile->_CheckFileUpdateTime();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err2_CheckFileUpdateTime_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int nFunRes  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	pGenerateSQLFile->_RemoveOldSQLFiles();
	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->m_strFileTimeSqlcodeTXT.clear();
	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();			   

	nFunRes  = pGenerateSQLFile->_CheckFileUpdateTime();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
}


void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err3_CheckFileUpdateTime_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int nFunRes  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	pGenerateSQLFile->_RemoveOldSQLFiles();
	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->m_strFileTimeSqlcodeTXT.clear();
	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();			   

	nFunRes  = pGenerateSQLFile->_CheckFileUpdateTime();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err4_CheckFileUpdateTime_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int nFunRes  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	pGenerateSQLFile->_RemoveOldSQLFiles();
	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->m_strFileTimeSqlcodeTXT = "ERRORFORTEST";
	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();			   

	nFunRes  = pGenerateSQLFile->_CheckFileUpdateTime();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
}
/* CPPTEST_TEST_CASE_END test__CheckFileUpdateTime_1 */

/* CPPTEST_TEST_CASE_BEGIN test__CheckLine_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_CheckLine(VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__CheckLine_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string  strValid;
	std::string  strInValid;
	VrtLinesConT _vrtAllLines;
	
	
	strValid = "ALARM_001=select * from alarm";
	strInValid = "ALARM_001 select * from alarm";
	_vrtAllLines.push_back(strValid);
	_vrtAllLines.push_back(strInValid);

	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_CheckLine(_vrtAllLines);
	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__CheckLine_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ClossAllOutPutFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_ClossAllOutPutFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__ClossAllOutPutFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
	
	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles();
	
	DEF_DELETE(pGenerateSQLFile);
	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);

}
/* CPPTEST_TEST_CASE_END test__ClossAllOutPutFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ExtractArg_1 */
/* CPPTEST_TEST_CASE_CONTEXT bool CGenerateSQLFile::_ExtractArg(const char *, std::string &, std::string &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__ExtractArg_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	//remove(defSQLFileName_Out_SQLH);
	//remove(defSQLFileName_Out_SQLMACRODEFH);
				   
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
									  
	_return = pGenerateSQLFile->setParam(_argc, _argv);

	DEF_DELETE(pGenerateSQLFile);													 
	
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_ExtractArg_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	std::string strParam = "  -2Inputfile=./sqlcode.txt"; 
	std::string strGetParamName;
	std::string strGetParamValue;
	pGenerateSQLFile->_ExtractArg(strParam.c_str(), strGetParamName, strGetParamValue);

	strParam = "--I--nputfile=./sqlcode.txt"; 
	pGenerateSQLFile->_ExtractArg(strParam.c_str(), strGetParamName, strGetParamValue);

	DEF_DELETE(pGenerateSQLFile);													 

}
/* CPPTEST_TEST_CASE_END test__ExtractArg_1 */

/* CPPTEST_TEST_CASE_BEGIN test__FormatToWriteFileTimeStr_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_FormatToWriteFileTimeStr(const std::string &, std::string &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__FormatToWriteFileTimeStr_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strDirSqlCodeTxt;
	std::string strFileTimeSqlcodeTXT;
	std::string strFormatRes;
	
	pGenerateSQLFile = new CGenerateSQLFile();
	strDirSqlCodeTxt = defSQLFileName_IN_SQLCODETXT;
	strFileTimeSqlcodeTXT = pGenerateSQLFile->_GetFileLastUpdateTime(strDirSqlCodeTxt);
	pGenerateSQLFile->_FormatToWriteFileTimeStr(strFileTimeSqlcodeTXT, strFormatRes);
	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__FormatToWriteFileTimeStr_1 */

/* CPPTEST_TEST_CASE_BEGIN test__GeneralSQLFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_GeneralSQLFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);  
	pGenerateSQLFile = new CGenerateSQLFile(); 
	_return = pGenerateSQLFile->_GeneralSQLFiles();
	DEF_DELETE(pGenerateSQLFile);	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);
}
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);  
	pGenerateSQLFile = new CGenerateSQLFile(); 
	_return = pGenerateSQLFile->_GeneralSQLFiles();
	DEF_DELETE(pGenerateSQLFile);	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);
}
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err2_GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);  
	pGenerateSQLFile = new CGenerateSQLFile(); 
	_return = pGenerateSQLFile->_GeneralSQLFiles();
	DEF_DELETE(pGenerateSQLFile);	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);
}
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err3_GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);  
	pGenerateSQLFile = new CGenerateSQLFile(); 
	_return = pGenerateSQLFile->_GeneralSQLFiles();
	DEF_DELETE(pGenerateSQLFile);	
	remove(defSQLFileName_Out_SQLH);
	remove(defSQLFileName_Out_SQLMACRODEFH);
}
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err4_GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	

	pGenerateSQLFile = new CGenerateSQLFile(); 
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	_return = pGenerateSQLFile->_GeneralSQLFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);	
}
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err5_GeneralSQLFiles_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;  	
	pGenerateSQLFile = new CGenerateSQLFile(); 
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	_return = pGenerateSQLFile->_GeneralSQLFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);	

}
/* CPPTEST_TEST_CASE_END test__GeneralSQLFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__GetFileLastUpdateTime_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CGenerateSQLFile::_GetFileLastUpdateTime(const std::string &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__GetFileLastUpdateTime_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strDirSqlCodeTxt;
	std::string strFileTimeSqlcodeTXT;
	std::string strFormatRes;

	pGenerateSQLFile = new CGenerateSQLFile();
	strDirSqlCodeTxt = defSQLFileName_IN_SQLCODETXT;
	strFileTimeSqlcodeTXT = pGenerateSQLFile->_GetFileLastUpdateTime(strDirSqlCodeTxt);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);


}


void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_GetFileLastUpdateTime_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strDirSqlCodeTxt;
	std::string strFileTimeSqlcodeTXT;
	std::string strFormatRes;

	pGenerateSQLFile = new CGenerateSQLFile();
	strDirSqlCodeTxt = defSQLFileName_IN_SQLCODETXT;
	strFileTimeSqlcodeTXT = pGenerateSQLFile->_GetFileLastUpdateTime(strDirSqlCodeTxt);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);


}
/* CPPTEST_TEST_CASE_END test__GetFileLastUpdateTime_1 */

/* CPPTEST_TEST_CASE_BEGIN test__GetFirstLineOfFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT std::string CGenerateSQLFile::_GetFirstLineOfFile(const std::string &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__GetFirstLineOfFile_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	std::string _strFileName;
	std::string strGetLine;

	pGenerateSQLFile = new CGenerateSQLFile();
	_strFileName = defSQLFileName_IN_SQLCODETXT;
	strGetLine = pGenerateSQLFile->_GetFileLastUpdateTime(_strFileName);
	_strFileName = defSQLFileName_Out_SQLH;
	strGetLine = pGenerateSQLFile->_GetFileLastUpdateTime(_strFileName);
	pGenerateSQLFile->_RemoveOldSQLFiles();  
		 	
	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__GetFirstLineOfFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test__MutiLinesToSingleLine_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_MutiLinesToSingleLine(VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__MutiLinesToSingleLine_1()
{	  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string  strLineOne;
	std::string  strLineTwo;
	VrtLinesConT _vrtAllLines;


	strLineOne = "ALARM_001=select * from ";
	strLineTwo = "+ alarm";
	_vrtAllLines.push_back(strLineOne);
	_vrtAllLines.push_back(strLineTwo);

	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_MutiLinesToSingleLine(_vrtAllLines);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
		
}
/* CPPTEST_TEST_CASE_END test__MutiLinesToSingleLine_1 */

/* CPPTEST_TEST_CASE_BEGIN test__OpenAllOutPutFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_OpenAllOutPutFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__OpenAllOutPutFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;

	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_OpenAllOutPutFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::ifstream IfstreamSrcFile;
	BOOL bSetRes = FALSE; 	
	std::string strFileName;

	pGenerateSQLFile = new CGenerateSQLFile();
	
	_return  = pGenerateSQLFile->_OpenAllOutPutFiles();
	_return  = pGenerateSQLFile->_ClossAllOutPutFiles();

	//set readonly then open to write data failed
	strFileName = pGenerateSQLFile->m_strDirSqlH;
	IfstreamSrcFile.open(strFileName.c_str());
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)strFileName.c_str(), FILE_ATTRIBUTE_READONLY);			
	}

	_return  = pGenerateSQLFile->_OpenAllOutPutFiles();
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);			
	}
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err2_OpenAllOutPutFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::ifstream IfstreamSrcFile;
	BOOL bSetRes = FALSE; 	
	std::string strFileName;

	pGenerateSQLFile = new CGenerateSQLFile();
	pGenerateSQLFile->_RemoveOldSQLFiles();
	
	_return  = pGenerateSQLFile->_OpenAllOutPutFiles();
	_return  = pGenerateSQLFile->_ClossAllOutPutFiles();

	//set readonly then open to write data failed
	strFileName = pGenerateSQLFile->m_strDirSqlMacrodefH;
	IfstreamSrcFile.open(strFileName.c_str());
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)strFileName.c_str(), FILE_ATTRIBUTE_READONLY);			
	}

	_return  = pGenerateSQLFile->_OpenAllOutPutFiles();
	IfstreamSrcFile.open(strFileName.c_str());
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);			
	}
	pGenerateSQLFile->_RemoveOldSQLFiles();

	DEF_DELETE(pGenerateSQLFile);
}
/* CPPTEST_TEST_CASE_END test__OpenAllOutPutFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ParseCmdLine_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_ParseCmdLine(const VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__ParseCmdLine_1()
{
	VrtLinesConT vecArgs;
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strArg;

	pGenerateSQLFile = new CGenerateSQLFile();
	
	strArg = "sqlgen.exe";
	vecArgs.push_back(strArg);
	strArg = "--Inputfile=./sqlcode.txt";
	vecArgs.push_back(strArg);
	strArg = "--Outpath=./";
	vecArgs.push_back(strArg);

	_return  = pGenerateSQLFile->_ParseCmdLine(vecArgs);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
}


void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_ParseCmdLine_1()
{
	VrtLinesConT vecArgs;
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strArg;

	pGenerateSQLFile = new CGenerateSQLFile();

	strArg = "sqlgen.exe";
	vecArgs.push_back(strArg);
	//error param
	strArg = "-Inputfile=./sqlcode.txt";
	vecArgs.push_back(strArg);
	strArg = "--Outpath=./";
	vecArgs.push_back(strArg);

	_return  = pGenerateSQLFile->_ParseCmdLine(vecArgs);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
}


/* CPPTEST_TEST_CASE_END test__ParseCmdLine_1 */

/* CPPTEST_TEST_CASE_BEGIN test__ReadFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_ReadFile(const std::string &, VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__ReadFile_1()
{

	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	VrtLinesConT vrtAllLines;
	std::string strFileName;
	strFileName = defSQLFileName_IN_SQLCODETXT;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_ReadFile(strFileName, vrtAllLines);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile); 

}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_ReadFile_1()
{  
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	VrtLinesConT vrtAllLines;
	std::string strFileName;
	strFileName = defSQLFileName_IN_SQLCODETXT;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_ReadFile(strFileName, vrtAllLines);
	
	{
		strFileName.clear();
		_return  = pGenerateSQLFile->_ReadFile(strFileName, vrtAllLines);
	}
	
	{
		strFileName = "../../../../../../../../../../../sqlcode.txt";
		_return  = pGenerateSQLFile->_ReadFile(strFileName, vrtAllLines);
	}
	
	
	DEF_DELETE(pGenerateSQLFile); 
}


/* CPPTEST_TEST_CASE_END test__ReadFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test__RemoveBlackLine_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_RemoveBlackLine(VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__RemoveBlackLine_1()
{
	VrtLinesConT _vrtAllLines;
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strLine;

	pGenerateSQLFile = new CGenerateSQLFile();

	strLine = "ALARM_001=select * from alarm";
	_vrtAllLines.push_back(strLine);
	strLine = "";
	_vrtAllLines.push_back(strLine);
	strLine = "ALARM_001=select * from alarm";
	_vrtAllLines.push_back(strLine);

	_return  = pGenerateSQLFile->_RemoveBlackLine(_vrtAllLines);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
	
}
/* CPPTEST_TEST_CASE_END test__RemoveBlackLine_1 */

/* CPPTEST_TEST_CASE_BEGIN test__RemoveFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_RemoveFile(const std::string &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__RemoveFile_1()
{	 
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string _strFileName;
	_strFileName = defSQLFileName_Out_SQLH;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_RemoveFile(_strFileName);

	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles();
	
	_strFileName = defSQLFileName_Out_SQLH;
	_return  = pGenerateSQLFile->_RemoveFile(_strFileName);
	
	_strFileName = defSQLFileName_Out_SQLMACRODEFH;
	_return  = pGenerateSQLFile->_RemoveFile(_strFileName);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile); 
	
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_RemoveFile_1()
{	 
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string _strFileName;
	_strFileName = defSQLFileName_Out_SQLH;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_RemoveFile(_strFileName);

	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles();

	//set sql.h read only then remove error
	_strFileName = defSQLFileName_Out_SQLH;
	BOOL bSetRes = FALSE;
	std::ifstream IfstreamSrcFile; 	
	IfstreamSrcFile.open(_strFileName.c_str());
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)_strFileName.c_str(), FILE_ATTRIBUTE_READONLY);// FILE_ATTRIBUTE_NORMAL			
	} 

	_return  = pGenerateSQLFile->_RemoveFile(_strFileName);

	IfstreamSrcFile.open(_strFileName.c_str());
	if (IfstreamSrcFile.is_open())
	{
		IfstreamSrcFile.close();
		bSetRes = ::SetFileAttributesA((LPCSTR)_strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);// FILE_ATTRIBUTE_NORMAL			
	}
	
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile); 

}

/* CPPTEST_TEST_CASE_END test__RemoveFile_1 */

/* CPPTEST_TEST_CASE_BEGIN test__RemoveOldSQLFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_RemoveOldSQLFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__RemoveOldSQLFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();

	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles();

	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();

	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);
	

}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_RemoveOldSQLFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();
	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles(); 
	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile); 
}

void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err2_RemoveOldSQLFiles_1()
{
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();
	pGenerateSQLFile->_OpenAllOutPutFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles(); 
	_return  = pGenerateSQLFile->_RemoveOldSQLFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile); 
}



/* CPPTEST_TEST_CASE_END test__RemoveOldSQLFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__TrimLines_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_TrimLines(VrtLinesConT &) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__TrimLines_1()
{
	VrtLinesConT _vrtAllLines;
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	std::string strLine;

	pGenerateSQLFile = new CGenerateSQLFile();

	strLine = "  ALARM_001=select * from alarm  ";
	_vrtAllLines.push_back(strLine);
	strLine = "";
	_vrtAllLines.push_back(strLine);
	strLine = "ALARM_001=select * from alarm";
	_vrtAllLines.push_back(strLine);

	_return  = pGenerateSQLFile->_TrimLines(_vrtAllLines);
	pGenerateSQLFile->_RemoveOldSQLFiles();  

	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__TrimLines_1 */

/* CPPTEST_TEST_CASE_BEGIN test__WriteBeginToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_WriteBeginToFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__WriteBeginToFiles_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	int _return  = 0;

	pGenerateSQLFile = new CGenerateSQLFile();

	pGenerateSQLFile->_OpenAllOutPutFiles();
	_return  = pGenerateSQLFile->_WriteBeginToFiles();
	pGenerateSQLFile->_ClossAllOutPutFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();  
	
	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__WriteBeginToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__WriteDataToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_WriteDataToFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__WriteDataToFiles_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	int _return  = 0;  
	pGenerateSQLFile = new CGenerateSQLFile();
	_return  = pGenerateSQLFile->_WriteDataToFiles(); 
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
 	
}


void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__err_WriteDataToFiles_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	int _return  = 0;  
	pGenerateSQLFile = new CGenerateSQLFile();
	_return  = pGenerateSQLFile->_WriteDataToFiles(); 
	pGenerateSQLFile->_RemoveOldSQLFiles();
	DEF_DELETE(pGenerateSQLFile);
		
}


/* CPPTEST_TEST_CASE_END test__WriteDataToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__WriteEndToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_WriteEndToFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__WriteEndToFiles_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	int nFunRes  = 0;

	pGenerateSQLFile = new CGenerateSQLFile(); 	
	
	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();

	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_WriteBeginToFiles();	

	nFunRes = pGenerateSQLFile->_WriteEndToFiles();
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();
							 
	DEF_DELETE(pGenerateSQLFile);

	
}
/* CPPTEST_TEST_CASE_END test__WriteEndToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test__WriteTimeStrToFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::_WriteTimeStrToFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test__WriteTimeStrToFiles_1()
{
	CGenerateSQLFile* pGenerateSQLFile = NULL;
	int nFunRes  = 0;

	pGenerateSQLFile = new CGenerateSQLFile(); 	

	nFunRes = pGenerateSQLFile->_OpenAllOutPutFiles();

	nFunRes = pGenerateSQLFile->_WriteTimeStrToFiles(); 	
	nFunRes = pGenerateSQLFile->_WriteBeginToFiles();	

	nFunRes = pGenerateSQLFile->_WriteEndToFiles();
	nFunRes = pGenerateSQLFile->_ClossAllOutPutFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();
					
	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test__WriteTimeStrToFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test_generalFiles_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::generalFiles(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test_generalFiles_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	//remove(defSQLFileName_Out_SQLH);
	//remove(defSQLFileName_Out_SQLMACRODEFH);

	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();
	pGenerateSQLFile->_RemoveFile(defSQLFileName_Out_SQLH);
	pGenerateSQLFile->_RemoveFile(defSQLFileName_Out_SQLMACRODEFH);

	_return = pGenerateSQLFile->setParam(_argc, _argv);
	pGenerateSQLFile->generalFiles();
	pGenerateSQLFile->generalFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();

	DEF_DELETE(pGenerateSQLFile);
	
}


void TestSuite_GenerateSQLFile_cpp_fe69d5de::test_err_generalFiles_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;

	pGenerateSQLFile = new CGenerateSQLFile();
	pGenerateSQLFile->_RemoveFile(defSQLFileName_Out_SQLH);
	pGenerateSQLFile->_RemoveFile(defSQLFileName_Out_SQLMACRODEFH);

	_return = pGenerateSQLFile->setParam(_argc, _argv);
	
	{
		pGenerateSQLFile->m_strDirErrorReport = "MM://ERRORSTRINGFORTEST.log";
		pGenerateSQLFile->m_strDirErrorReportReserve = defSQLFilename_OUT_ERRORREPORT; 
		pGenerateSQLFile->generalFiles(); 
	}

	pGenerateSQLFile->_RemoveOldSQLFiles();

	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test_generalFiles_1 */

/* CPPTEST_TEST_CASE_BEGIN test_setParam_1 */
/* CPPTEST_TEST_CASE_CONTEXT int CGenerateSQLFile::setParam(int, char **) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test_setParam_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	//remove(defSQLFileName_Out_SQLH);
	//remove(defSQLFileName_Out_SQLMACRODEFH);

	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	_return = pGenerateSQLFile->setParam(_argc, _argv);
	
	{
		_argc = 99;
		_return = pGenerateSQLFile->setParam(_argc, _argv);	
	}

	//pGenerateSQLFile->generalFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();

	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test_setParam_1 */

/* CPPTEST_TEST_CASE_BEGIN test_x7eCGenerateSQLFile_1 */
/* CPPTEST_TEST_CASE_CONTEXT CGenerateSQLFile::~CGenerateSQLFile(void) */
void TestSuite_GenerateSQLFile_cpp_fe69d5de::test_x7eCGenerateSQLFile_1()
{
	int _argc  = 3;
	char *runParams[] = {"sqlgen.exe", "--Inputfile=./sqlcode.txt", "--Outpath=./"};
	char **_argv = runParams;
	//remove(defSQLFileName_Out_SQLH);
	//remove(defSQLFileName_Out_SQLMACRODEFH);

	CGenerateSQLFile *pGenerateSQLFile = NULL;
	int _return  = 0;
	pGenerateSQLFile = new CGenerateSQLFile();

	//_return = pGenerateSQLFile->setParam(_argc, _argv);
	//pGenerateSQLFile->generalFiles();
	pGenerateSQLFile->_RemoveOldSQLFiles();

	DEF_DELETE(pGenerateSQLFile);

}
/* CPPTEST_TEST_CASE_END test_x7eCGenerateSQLFile_1 */
