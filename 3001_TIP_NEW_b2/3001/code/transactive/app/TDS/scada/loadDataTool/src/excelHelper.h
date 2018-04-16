
#ifndef _EXCELHELPER_H_
#define _EXCELHELPER_H_

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

//#include <stdio.h>
#include <tchar.h>
//#include <Windows.h>
#include <Objbase.h>
#include <string>
#pragma warning( disable : 4786)

#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
    rename( "RGB", "MSORGB" )

using namespace Office;

#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

#import "C:\\Program Files\\Microsoft Office\\OFFICE12\\EXCEL.EXE" \
    rename( "DialogBox", "ExcelDialogBox" ) \
    rename( "RGB", "ExcelRGB" ) \
    rename( "CopyFile", "ExcelCopyFile" ) \
    rename( "ReplaceText", "ExcelReplaceText")

class ExcelHelper
{
public:
	ExcelHelper();

	~ExcelHelper();
	int WriteTxtFromClipBoard(std::string filePath, std::string sheetName, const char* data);
	Excel::_WorksheetPtr GetSheet(Excel::_WorkbookPtr wbPtr, const char* shtName);	//if not exist, then create it.
	bool SetSheetVisible(std::string filePath, std::string sheetName, bool bValue);

private:
	Excel::_ApplicationPtr m_pApplication;
};

#endif
