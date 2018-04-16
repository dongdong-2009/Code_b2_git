#include "excelHelper.h"
#include <stdio.h>
#include <iostream>

int set_Clipboard(const char* data)
{
	if(OpenClipboard(NULL)) 
	{ 
		HGLOBAL clipbuffer; 
		char * buffer; 
		EmptyClipboard(); 
		int nBytes = strlen(data);
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, nBytes+1); 
		buffer = (char*)GlobalLock(clipbuffer); 
		strcpy(buffer, data); 
		GlobalUnlock(clipbuffer); 
		SetClipboardData(CF_TEXT,clipbuffer); 
		CloseClipboard(); 
		printf("set clip board %d bytes ok\n", nBytes);
		return 0;
	}
	else
	{
		printf("set clip board failed.\n");
	}
	return 1;
}

void get_Clipboard()
{
	char * buffer = NULL; 
	//´ò¿ª¼ôÌù°å 
	//CString fromClipboard; 
	if ( OpenClipboard(NULL) ) 
	{ 
		HANDLE hData = GetClipboardData(CF_TEXT); 
		char * buffer = (char*)GlobalLock(hData); 
		//fromClipboard = buffer; 
		printf("buf len is: %d\n", strlen(buffer));
		GlobalUnlock(hData); 
		CloseClipboard(); 
	}
	else
	{
		printf("get clip board failed.\n");
	}
}


ExcelHelper::ExcelHelper()
{
	HRESULT h = CoInitialize(NULL);
	if (FAILED(h))
	{
		printf("failed: CoInitialize: %d\n", h);
		return ;
	}

	if ( FAILED( m_pApplication.CreateInstance( _T("Excel.Application") ) ) )
	{
		printf( "Failed to initialize Excel::_Application!\n");
		return ;
	}
	m_pApplication->PutVisible(0, VARIANT_TRUE);
}

ExcelHelper::~ExcelHelper()
{
	m_pApplication->Quit();
}

int ExcelHelper::WriteTxtFromClipBoard(std::string filePath, std::string sheetName, const char* data)
{    
	if (0 != set_Clipboard(data))
	{
		return 1;
	}
	get_Clipboard();

	Excel::_WorkbookPtr wbPtr;
	wbPtr = m_pApplication->Workbooks->_Open(filePath.c_str());
	wbPtr->Activate();
	Excel::_WorksheetPtr sheetPtr = GetSheet(wbPtr, sheetName.c_str());
	
	Excel::RangePtr rg;
	sheetPtr->Columns->NumberFormat = "@";
	rg = sheetPtr->GetRange("a1", "a1");
	rg->Select();
	sheetPtr->Paste();
	//rg->PasteSpecial(Excel::xlPasteValues, Excel::xlPasteSpecialOperationNone);
	//rg->PutItem(1, 1, "hi\tabcd\nsdfsdf\tfdsjkfdjk\tjsdkfjk\nsdfdskf");	// from 1;

	try
	{
		_variant_t twbn((long)Excel::xlWorkbookNormal);
		_variant_t tlsc((long)Excel::xlLocalSessionChanges);
		// wbPtr->SaveAs("E:\\test_code\\read_excel\\debug\\testxx.xls", twbn, "", "", false, false, Excel::xlNoChange, tlsc, false);
		wbPtr->Save();
	}
	catch (...)
	{
		printf("exception happened when save workbook\n");
	}
	wbPtr->Close();
	return 0;
}

Excel::_WorksheetPtr ExcelHelper::GetSheet(Excel::_WorkbookPtr wbPtr, const char* shtName)
{
	_bstr_t nameTemp = shtName;
	_variant_t var;
	int i;
	long shts = wbPtr->Sheets->Count;
	Excel::_WorksheetPtr sheetPtr;
	bool nf = true;
	for (i = 1; i <= shts && nf; ++i)
	{
		var = (long)i;
		sheetPtr = wbPtr->Sheets->GetItem(var);
		if (sheetPtr->Name == nameTemp)
		{
			nf = false;
			m_pApplication->DisplayAlerts[0] = false;
			sheetPtr->Delete();
			m_pApplication->DisplayAlerts[0] = true;
		}
	}
	//if (nf)
	{
		sheetPtr = 	wbPtr->Sheets->Add();
		sheetPtr->Name = nameTemp;
	}
	return sheetPtr;
}

bool ExcelHelper::SetSheetVisible(std::string filePath, std::string sheetName, bool bValue)
{
	Excel::_WorkbookPtr wbPtr;
	wbPtr = m_pApplication->Workbooks->_Open(filePath.c_str());
	
	_bstr_t nameTemp = sheetName.c_str();
	_variant_t var;
	int i;
	long shts = wbPtr->Sheets->Count;
	Excel::_WorksheetPtr sheetPtr;
	bool nf = true;
	for (i = 1; i <= shts && nf; ++i)
	{
		var = (long)i;
		sheetPtr = wbPtr->Sheets->GetItem(var);
		if (sheetPtr->Name == nameTemp)
		{
			nf = false;
			m_pApplication->DisplayAlerts[0] = false;
			if (false == bValue)
			{
				sheetPtr->Visible[0] = Excel::xlSheetVeryHidden;
			}
			else
			{
				sheetPtr->Visible[0] = Excel::xlSheetVisible;
			}
			m_pApplication->DisplayAlerts[0] = true;
		}
	}
	try
	{
		wbPtr->Save();
	}
	catch (...)
	{
		printf("exception happened when save workbook, after SetSheetVisible\n");
	}
	wbPtr->Close();
	return true != nf;
}
