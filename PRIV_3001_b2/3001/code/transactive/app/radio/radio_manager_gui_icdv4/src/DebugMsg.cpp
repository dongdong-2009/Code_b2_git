/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/DebugMsg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureManager.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "DebugMsg.h"

extern FutureManager* gFutureManager;

namespace
{

    static const int IDC_DEBUG_TEXT = 8901;
    static const int IDC_DEBUG_CLEAR = 8902;
    static const int IDC_FUTURE = 8903;

    static const int BUTTON_HEIGHT = 20;

    static HWND gDebugWindow = NULL;
    static TCHAR sDebugMsg[64][512];
    static int sDebugMsgIndex = 0;

    static const TCHAR* szWindowClass = "Radio_DebugEventWnd";

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
	    int wmId, wmEvent;

	    switch (message) 
	    {
            case 10011:
                {
                    HWND eWnd = ::GetDlgItem(hWnd, IDC_DEBUG_TEXT);
                    ::SendMessage(eWnd, EM_SETSEL, 0, 0);
                    ::SendMessage(eWnd, EM_REPLACESEL, 0, (LPARAM) sDebugMsg[lParam]);
                }
                ShowWindow(hWnd, SW_SHOW);
                return 0;

		    case WM_COMMAND:
			    wmId    = LOWORD(wParam); 
			    wmEvent = HIWORD(wParam); 
                switch (wmId)
                {
                case IDC_FUTURE:
                    {
                        HWND eWnd = ::GetDlgItem(hWnd, IDC_DEBUG_TEXT);
                        ::SendMessage(eWnd, EM_SETSEL, 0, 0);
                        ::SendMessage(eWnd, EM_REPLACESEL, 0, (LPARAM) gFutureManager->getFutureList().c_str());
                    }
                    break;

                case IDC_DEBUG_CLEAR:
                    SetWindowText(GetDlgItem(hWnd, IDC_DEBUG_TEXT), "");
                    break;
                }
                return 0;

		    case WM_DESTROY:
			    PostQuitMessage(0);
                gDebugWindow = NULL;
                return 0;

            case WM_CLOSE:
                ShowWindow(hWnd, SW_HIDE);
                return 0;

            case WM_SIZE:
                {
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                    SetWindowPos(GetDlgItem(hWnd, IDC_DEBUG_TEXT), NULL, 0, 0,
                        rc.right, rc.bottom - BUTTON_HEIGHT, 
                        SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

                    SetWindowPos(GetDlgItem(hWnd, IDC_DEBUG_CLEAR), NULL, 0, rc.bottom - BUTTON_HEIGHT,
                        0, 0, 
                        SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

                    SetWindowPos(GetDlgItem(hWnd, IDC_FUTURE), NULL, 85, rc.bottom - BUTTON_HEIGHT,
                        0, 0, 
                        SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
                }
                return 0;
       }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    static ATOM MyRegisterClass(HINSTANCE hInstance)
    {
	    WNDCLASSEX wcex;

	    wcex.cbSize = sizeof(WNDCLASSEX); 

	    wcex.style			= CS_HREDRAW | CS_VREDRAW;
	    wcex.lpfnWndProc	= (WNDPROC)WndProc;
	    wcex.cbClsExtra		= 0;
	    wcex.cbWndExtra		= 0;
	    wcex.hInstance		= hInstance;
	    wcex.hIcon			= NULL;
	    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	    wcex.lpszMenuName	= NULL;
	    wcex.lpszClassName	= szWindowClass;
	    wcex.hIconSm		= NULL;

	    return RegisterClassEx(&wcex);
    }
}



void AddDebugMessage(const std::string& msg)
{
    if (!gDebugWindow)
    {
        // can't create window, just exit
        return;
    }

    static TA_Base_Core::ReEntrantThreadLockable lLock;
    lLock.acquire();
    static int ind = 0;
    int lIndex = sDebugMsgIndex++;
    if (sDebugMsgIndex == 64)
    {
        sDebugMsgIndex = 0;
    }

    wsprintf(sDebugMsg[lIndex], "%03d %s\r\n", ++ind, msg.c_str());
    lLock.release();

    PostMessage(gDebugWindow, 10011, 0, lIndex);
}


void TempLog(const std::string& msg)
{
    static TA_Base_Core::ReEntrantThreadLockable lLock;
    lLock.acquire();
    static int sIndex = 0;
    FILE *logF;
    if (sIndex++ == 0)
    {
        logF = fopen("c:\\templog.txt", "w");
    }
    else
    {
        logF = fopen("c:\\templog.txt", "a");
    }
    if (logF == NULL) return;

    fprintf(logF, "%d %s\n", time(NULL), msg.c_str());

    fclose(logF);

    lLock.release();

}

void InitializeDebugWindow(HINSTANCE hInstance)
{
	MyRegisterClass(hInstance);

   gDebugWindow = CreateWindow(szWindowClass, "DebugEvent", WS_OVERLAPPEDWINDOW,
      0, 0, 800, 200, NULL, NULL, hInstance, NULL);

   if (!gDebugWindow)
   {
      return;
   }

   RECT rc;
   GetClientRect(gDebugWindow, &rc);

   CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
       0, 0, rc.right, rc.bottom - BUTTON_HEIGHT, gDebugWindow,
       (HMENU) IDC_DEBUG_TEXT, hInstance, NULL);

   CreateWindow("BUTTON", "Clear", WS_CHILD | WS_VISIBLE  | 
                BS_PUSHBUTTON,
       0, rc.bottom - BUTTON_HEIGHT, 80, BUTTON_HEIGHT, gDebugWindow,
       (HMENU) IDC_DEBUG_CLEAR, hInstance, NULL);

   CreateWindow("BUTTON", "Show Future", WS_CHILD | WS_VISIBLE  | 
                BS_PUSHBUTTON,
       85, rc.bottom - BUTTON_HEIGHT, 180, BUTTON_HEIGHT, gDebugWindow,
       (HMENU) IDC_FUTURE, hInstance, NULL);
}
