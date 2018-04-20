/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/CallBannerListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * List control for displaying call banner items
  * 
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4284 4250)
#endif // _MSC_VER

#include "app\system_control\banner\pages\call_page\src\stdafx.h"
#include "app\system_control\banner\pages\call_page\src\CallBannerListCtrl.h"

namespace TA_IRS_App
{

    CallBannerListCtrl::CallBannerListCtrl(int maxColumnCount)
    : m_calllistSortingInfo(maxColumnCount)
    {
    }
	BEGIN_MESSAGE_MAP(CallBannerListCtrl, ListCtrlSelNoFocus)  //TD16152
	//{{AFX_MSG_MAP(CallBannerListCtrl)
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()  //TD16152


    void CallBannerListCtrl::initialise(int columnIndex, TA_Base_Bus::ISortingCallback* callback)
    {
        m_calllistSortingInfo.activateSort(columnIndex);
        m_calllistSortingInfo.defineSortingSemantic(columnIndex, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);
        m_calllistSortingInfo.setCallback(callback);
    }


    LRESULT CallBannerListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        // Give call list control chance to sort itself
        TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_calllistSortingInfo);    
    
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }

	void CallBannerListCtrl::HideScrollBars()  //TD16152
	{
	
			RECT ierect;
			int cxvs, cyvs;
			GetClientRect(&ierect); //Get client width and height

			cyvs = GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ
			
			 cxvs=0; //Set VERT to zero when choosen HORZ


			//Here we set the position of the window to the clientrect + the size of the scrollbars
			SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);

			//Her we modify the rect so the right part is subbed from the rect.
			ierect.bottom -= ierect.top;

			//Just to be safe that the left/top corner is 0...
			ierect.top = 0;
			ierect.left = 0;
			
			HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!

			//The -2 is probably a border of some kind that we also need to remove. I could not find any good
			//metrics that gave me an 2 as an answer. So insted we makes it static with -2.
		
			iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom-2);
			//After the range has been made we add it...
			SetWindowRgn (iehrgn, TRUE);

	}
	//****************************************************************************************

	void CallBannerListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
	{
		// TODO: Add your message handler code here and/or call default
	
		 ModifyStyle(WS_HSCROLL ,0,0); //Just by modifing the style we remove the scrollbar

		CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
	}

	//****************************************************************************************

}      //TD16152
