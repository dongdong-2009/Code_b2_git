#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_CommonDefine)
#define Transactive_3002_GF_App_Bas_TimeTableManager_CommonDefine
#if _MSC_VER > 1000
#pragma once
#endif


// include the common headers
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"

// some constant
const unsigned int Dlg_Top_Blank_Size = 7;
const unsigned int Dlg_Left_Blank_Size = 7;
const unsigned int Dlg_Right_Blank_Size = 7;
const unsigned int Dlg_Bottom_Blank_Size = 7;

const unsigned int gMFT_Screen_Width = 1440;
const unsigned int gMFT_Screen_Height = 900;

#define COLOR_YELLOW RGB(255,255,0)
#define COLOR_GREEN RGB(0,255,0)
#define COLOR_RED RGB(255,0,0)

#define  RPARAM_TIMETABLE_TYPE "TimeTableType"
#define  RPARAM_TIMETABLE_TYPE_VALUE_OCC "OccTimeTable"
#define  RPARAM_TIMETABLE_TYPE_VALUE_STATION "StationTimeTable"

#define MaxModesSendPerLocation 30

#define MessageBoxCaption "时间表管理器"

// BasMangaer Action enum
typedef enum _TimeTableManagerAction_ 
{
	TimeTableManagerAction_Initialize = 0,
	TimeTableManagerAction_LoadTimeTable
}TimeTableManagerAction;


// some macro define 
#define BAS_TRY \
std::string strExceptionString = ""; \
try

#define BAS_CATCH \
catch(const TA_Base_Core::TransactiveException& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what()); \
} \
catch(const std::exception& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what()); \
} \
catch(...) \
{ \
	strExceptionString = " 未知错误 "; \
	LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur."); \
}

#define BAS_GetExceptionString strExceptionString.c_str()

#define BAS_WhetherHaveError (!strExceptionString.empty())


#define BAS_IsWindow(ctrl_ID) (NULL != GetDlgItem(ctrl_ID) && IsWindow(GetDlgItem(ctrl_ID)->m_hWnd))

#define BAS_MoveFixedSizeCtrl(ctrl_ID, cx, cy) \
{ \
	CRect ctrlRect;	\
	GetDlgItem(ctrl_ID)->GetWindowRect(ctrlRect); \
	ctrlRect.MoveToXY(cx, cy); \
	GetDlgItem(ctrl_ID)->MoveWindow(ctrlRect, TRUE); \
}

#define If_Not_Valid_Window_Return_Void(pCWnd) \
{ \
	if (!IsWindow(pCWnd->GetSafeHwnd())) \
	{ \
		return; \
	} \
}

// For the namespace 
#define Namespace_TA_IRS_APP_Begin \
namespace TA_IRS_App \
{ 

#define Namespace_TA_IRS_APP_End \
}


// Following is the user defined message
#define UserDefinedMessageBase (WM_USER + 100)
// use it when dialog start up, it should be used once
#define Msg_InitializeDialogData (UserDefinedMessageBase + 1)

// This struct used to set the initialize dialog size 
typedef struct  DlgInitializeSize
{
	DlgInitializeSize(int width, int height)
		:nWidth(width),nHeight(height)
	{
	}
	int nWidth;
	int nHeight;
}Dlg_Initialize_Size;

// The application busy indicator
class ApplicationBusyIndicator
{
public:
	ApplicationBusyIndicator()
	{
		m_oldCursor = ::GetCursor();
		HCURSOR cursor = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
		SetCursor(cursor);
	}
	~ApplicationBusyIndicator()
	{
		HCURSOR cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		SetCursor(cursor);
	}
private:
	HCURSOR m_oldCursor;
};
#endif //Transactive_3002_GF_App_Bas_TimeTableManager_CommonDefine
