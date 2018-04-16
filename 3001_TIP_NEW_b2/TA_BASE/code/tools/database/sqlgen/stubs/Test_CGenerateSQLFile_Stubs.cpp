#include "cpptest.h"

/**
 * This file contains user stub definitions.
 *
 * To create a user stub:
 * 1. Open stub template list:
 *    - Eclipse IDE: type 'stub' and press <Ctrl> <Space>
 *    - Microsoft Visual Studio IDE: use context menu C++test->Insert Snippet...
 * 2. Choose the stub template from the list.
 * 3. #include the header file where the function you want to stub is originally 
 *    declared as well as any supporting header files as necessary.
 * 4. Fill out the stub signature to match that of the original function, 
 *    while keeping the CppTest_Stub prefix when it applies.
 *    Note: for C++ operators, use appropriate stub names - e.g.:
 *        CppTest_Stub_operator_new    for operator new
 *        CppTest_Stub_operator_delete for operator delete
 *        CppTest_Stub_operator_plus   for operator +
 *    Refer to C++test User's Guide for a complete list of stub names for operators. 
 * 5. Fill out the body of the stub according to intent.
 *
 * Available C++test API functions (see C++test Users Guide for details):
 *     void CppTest_Assert(bool test, const char * message)
 *     void CppTest_Break()
 *     const char* CppTest_GetCurrentTestCaseName()
 *     const char* CppTest_GetCurrentTestSuiteName()
 *     bool CppTest_IsCurrentTestCase(const char* testCaseName)
 */

/** 
 * Header files where the stubbed functions are originally declared.
 * Verify #include directives and add any additional header files as necessary.
 */
#include "src\GenerateSQLFile.h"

/** User stub definition for function: int CGenerateSQLFile::_RemoveFile(const std::string &) */
int (::CGenerateSQLFile::CppTest_Stub__RemoveFile) (const ::std::string & strFileName) 
{
	if (CppTest_IsCurrentTestCase("test__err_RemoveOldSQLFiles_1"))
	{
		std::string strDirSqlH = defSQLFileName_Out_SQLH;
		std::string strDirSqlMacrodefH = defSQLFileName_Out_SQLMACRODEFH;
		if (0 == strDirSqlH.compare(strFileName))
		{
			return -1;
		} 			
	}
	else if (CppTest_IsCurrentTestCase("test__err2_RemoveOldSQLFiles_1"))
	{
		std::string strDirSqlH = defSQLFileName_Out_SQLH;
		std::string strDirSqlMacrodefH = defSQLFileName_Out_SQLMACRODEFH;
		if (0 == strDirSqlMacrodefH.compare(strFileName))
		{
			return -1;
		} 			
	}
	
	return _RemoveFile(strFileName);
}

/** User stub definition for function: int CGenerateSQLFile::_RemoveOldSQLFiles(void) */
int (::CGenerateSQLFile::CppTest_Stub__RemoveOldSQLFiles) (void) 
{
	if (CppTest_IsCurrentTestCase("test__err_GeneralSQLFiles_1"))
	{
		return -1;	
	}
	 	
	return _RemoveOldSQLFiles();
}

/** User stub definition for function: int CGenerateSQLFile::_ReadFile(const std::string &, VrtLinesConT &) */
int (::CGenerateSQLFile::CppTest_Stub__ReadFile) (const ::std::string & strFileName, ::VrtLinesConT & vrtAllLines) 
{
	if (CppTest_IsCurrentTestCase("test__err2_GeneralSQLFiles_1"))
	{
		return -1;	
	}
	  
	return _ReadFile(strFileName, vrtAllLines);
}

/** User stub definition for function: int CGenerateSQLFile::_CheckLine(VrtLinesConT &) */
int (::CGenerateSQLFile::CppTest_Stub__CheckLine) (::VrtLinesConT & vrtAllLines) 
{
	if (CppTest_IsCurrentTestCase("test__err3_GeneralSQLFiles_1"))
	{
		return -1;	
	}

	return _CheckLine( vrtAllLines);
}

/** User stub definition for function: std::string CGenerateSQLFile::_GetFirstLineOfFile(const std::string &) */
::std::string (::CGenerateSQLFile::CppTest_Stub__GetFirstLineOfFile) (const ::std::string & strFileName) 
{
	if (CppTest_IsCurrentTestCase("test__err2_CheckFileUpdateTime_1"))
	{
		std::string strDirSqlMacrodefH = defSQLFileName_Out_SQLMACRODEFH;
		if (0 == strDirSqlMacrodefH.compare(strFileName))
		{
			std::string strRes;
			return strRes;	
		}
	}

    return _GetFirstLineOfFile(strFileName);
}

/** User stub definition for function: std::string CGenerateSQLFile::_GetFileLastUpdateTime(const std::string &) */
::std::string (::CGenerateSQLFile::CppTest_Stub__GetFileLastUpdateTime) (const ::std::string & strFilename) 
{
	if (CppTest_IsCurrentTestCase("test__err3_CheckFileUpdateTime_1"))
	{
		std::string strRes;
		return strRes;	
	}
    return _GetFileLastUpdateTime(strFilename);
}

/** User stub definition for function: int CGenerateSQLFile::_OpenAllOutPutFiles(void) */
int (::CGenerateSQLFile::CppTest_Stub__OpenAllOutPutFiles) (void) 
{
	if (CppTest_IsCurrentTestCase("test__err_WriteDataToFiles_1"))
	{
		return -1;	
	}
	return _OpenAllOutPutFiles();
}
