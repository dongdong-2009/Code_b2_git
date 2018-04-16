/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioErrorMsg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace
{
    static const int IDC_DEBUG_TEXT = 8901;

    static const int BUTTON_HEIGHT = 20;

    static HWND gErrorWindow = NULL;
    static TCHAR sErrorMsg[64][512];
    static int sErrorMsgIndex = 0;

    static const TCHAR* szWindowClass = "Radio_ErrorWnd";

	static bool messageShown = false;

    static LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
	    int wmId, wmEvent;

	    switch (message) 
	    {
            case 10091:
				if (messageShown) {
					HWND h = FindWindow(NULL,"Radio Manager Error");
					SendMessage(h, WM_CLOSE, 0, 0);
				}
				messageShown = true;
				if (IDOK == MessageBox(gErrorWindow, sErrorMsg[lParam], "Radio Manager Error", MB_OK|MB_ICONERROR)) {
					messageShown = false;
				}


                return 0;

		    case WM_COMMAND:
			    wmId    = LOWORD(wParam); 
			    wmEvent = HIWORD(wParam); 
                switch (wmId)
                {
                case IDOK:
                    SetWindowText(GetDlgItem(hWnd, IDC_DEBUG_TEXT), "");
                    ShowWindow(hWnd, SW_HIDE);
                    break;
                }
                return 0;

		    case WM_DESTROY:
                gErrorWindow = NULL;
                return 0;

            case WM_CLOSE:
                ShowWindow(hWnd, SW_HIDE);
                return 0;

       }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    static ATOM MyRegisterClass(HINSTANCE hInstance)
    {
	    WNDCLASSEX wcex;

	    wcex.cbSize = sizeof(WNDCLASSEX); 

	    wcex.style			= CS_HREDRAW | CS_VREDRAW;
	    wcex.lpfnWndProc	= (WNDPROC)WndProc2;
	    wcex.cbClsExtra		= 0;
	    wcex.cbWndExtra		= 0;
	    wcex.hInstance		= hInstance;
	    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	    wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
	    wcex.lpszMenuName	= NULL;
	    wcex.lpszClassName	= szWindowClass;
	    wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

	    return RegisterClassEx(&wcex);
    }

}



void AddErrorMessage(const std::string& msg)
{
    if (!gErrorWindow)
    {
        // can't create window, just exit
        return;
    }

    static TA_Base_Core::ReEntrantThreadLockable lLock;
    lLock.acquire();
    static int ind = 0;
    int lIndex = sErrorMsgIndex++;
    if (sErrorMsgIndex == 64)
    {
        sErrorMsgIndex = 0;
    }

    wsprintf(sErrorMsg[lIndex], "%s\r\n", msg.c_str());
    lLock.release();

    PostMessage(gErrorWindow, 10091, 0, lIndex);
}

void InitializeErrorMessageWindow(HINSTANCE hInstance, HWND pParent)
{
    MyRegisterClass(hInstance);

   gErrorWindow = CreateWindow(szWindowClass, "Error", WS_POPUPWINDOW | WS_CAPTION,
      400, 400, 400, 150, pParent, NULL, hInstance, NULL);

   if (!gErrorWindow)
   {
      return;
   }

   RECT rc;
   GetClientRect(gErrorWindow, &rc);

   CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | 
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
       0, 0, rc.right, rc.bottom - BUTTON_HEIGHT, gErrorWindow,
       (HMENU) IDC_DEBUG_TEXT, hInstance, NULL);

   CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE  | 
                BS_PUSHBUTTON,
       (rc.right - 80) / 2, rc.bottom - BUTTON_HEIGHT, 80, BUTTON_HEIGHT, gErrorWindow,
       (HMENU) IDOK, hInstance, NULL);

}

