/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/mfc_extensions/src/prnScreen/prnScreen.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/19 10:37:48 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// prnScreen.cpp : Defines the entry point for the DLL application.
//

#include "bus/mfc_extensions/src/StdAfx.h"
#include "winspool.h"
#include "commdlg.h"
#include "prnScreen.h"

UINT CALLBACK PrintHookProc(
							HWND hdlg,      // handle to the dialog box window
							UINT uiMsg,     // message identifier
							WPARAM wParam,  // message parameter
							LPARAM lParam   // message parameter
							)
{
	switch(uiMsg)
    {
	case WM_INITDIALOG:
        {
            // save the handle to print dialog window for future use.
            SetWindowPos(hdlg,
				CWnd::wndTopMost,         // handle to window
                0, 0, 0, 0,          // dont-cares since,
				// this window options is set to
				// SWP_NOSIZE
                SWP_NOSIZE | SWP_NOMOVE    // window-positioning options
				);
            break;
        }
    }
    return 0;
}
PRNSCREEN_API void CapturePrintScreen(long lX) 
{
    HDC dc = NULL;
	PRINTDLG pd;
	BOOL bPrintingOK, bDialog = TRUE;

	//Start Capture Screen
	HDC memDC = CreateCompatibleDC(NULL);
	HDC winDC = GetDC(NULL);

	HWND hWnd = GetDesktopWindow();
	RECT rc;
	GetWindowRect(hWnd, &rc);

	//int cx = (rc.right - rc.left)  ;
	//int cy = rc.bottom - rc.top;

	//double numOfScreens = (double)cx/1280;

	//if(numOfScreens != (int)numOfScreens)
	//{
	//	numOfScreens = (double)cx/1024;
	//	if(numOfScreens != (int)numOfScreens)
	//	{
	//		numOfScreens = (double)cx/800;
	//	}
	//}
	//if(numOfScreens != (int)numOfScreens)
	//{
	//	numOfScreens = 1;
	//}

    struct MonitorInfo
    {
        int getScreenWidth()
        {
            return ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        }

        int getScreenHight()
        {
            return ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
        }

        int getMonitorCount()
        {
            int count = 0;

            HDC hdc = ::GetDC(NULL);
            ::EnumDisplayMonitors( hdc, NULL, &MonitorInfo::monitorEnumProc, (LPARAM)(&count) );
            ::ReleaseDC(NULL, hdc);

            return count;
        }

        static BOOL CALLBACK monitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
        {
            int* count = reinterpret_cast<int*>(dwData);
            (*count)++;
            return TRUE;
        }
    };

    MonitorInfo monitorInfo;
    int cx = monitorInfo.getScreenWidth();
    int cy = monitorInfo.getScreenHight();
    double numOfScreens = monitorInfo.getMonitorCount();
	

	HBITMAP memBitmap = CreateCompatibleBitmap(winDC, cx, cy);
	HBITMAP oldBitmap = (HBITMAP) SelectObject(memDC, memBitmap);

	BitBlt(memDC, 0, 0, cx, cy, winDC, 0, 0, SRCCOPY);
	
	memset( &pd, 0, sizeof(PRINTDLG) ) ;
    
	//Fill out PrintDialog structure
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hDC = NULL;
	pd.hwndOwner = NULL;
	pd.hInstance = NULL;
	pd.nMaxPage = 1;
	pd.nMinPage = 1;
	pd.nFromPage = 1;
	pd.nToPage = 1;
	pd.nCopies = 1;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.lpfnPrintHook = PrintHookProc;
	if (bDialog)
	pd.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE | PD_ENABLEPRINTHOOK;
	else
	pd.Flags = PD_RETURNDC | PD_RETURNDEFAULT;

	//Call PrintDialog
	if(PrintDlg(&pd))
    {
		dc = pd.hDC;
    }


//    CString strTitle;                       // Get the application title
//    strTitle.LoadString(AFX_IDS_APP_TITLE);
	char * strTitle;
	strTitle = "PrintScreen";
    DOCINFO di;                             // Initialise print document details
    ::ZeroMemory (&di, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;



//    BOOL bPrintingOK = dc.StartDoc(&di);    // Begin a new print job
	::StartDoc(dc, &di);
	::StartPage(dc);
    // Get the printing extents and store in the m_rectDraw field of a 
 
	int targetTop, targetLeft;
	cx = cx/(int)numOfScreens;
	//::MessageBox(NULL, _T("Memory allocation for dialog template failed."),
	//					   _T("Allocation Failure"), 
	//					   MB_ICONEXCLAMATION | MB_OK);

	
	double xRatio = (GetDeviceCaps(dc, HORZRES)-50) / (double) cx;
	double yRatio = (GetDeviceCaps(dc, VERTRES)-50) / (double) cy;

	if (yRatio < xRatio) xRatio = yRatio;

	targetTop = (int) (GetDeviceCaps(dc, HORZRES) - xRatio * cy)/2;
	targetLeft = (int) (GetDeviceCaps(dc, HORZRES) - xRatio * cx)/2;
	
	StretchBlt(dc, 25, 25, (int)(xRatio * cx), (int) (xRatio * cy), memDC, (int) lX, 0, cx, cy, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
    
//	bPrintingOK = (dc.EndPage() > 0);
	bPrintingOK = (::EndPage(dc)>0);
      
    if (bPrintingOK)
        ::EndDoc(dc);                      // end a print job
    else
        ::AbortDoc(dc);                      // abort job.

    ::DeleteDC(dc);                          // delete the printer DC
}

