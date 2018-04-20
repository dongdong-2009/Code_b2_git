/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/ToolTipComboBox.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Combo Box with tool tips (except doesn't have tool tips at present, because
  * not sure how to get tool tips to work on a dropped down combo box)
  *
  */

#include "StdAfx.h"
#include "ToolTipComboBox.h"

BEGIN_MESSAGE_MAP(ToolTipComboBox, CComboBox)
    //{{AFX_MSG_MAP(ToolTipComboBox)    
    ON_NOTIFY_REFLECT_EX(TTN_NEEDTEXTA, OnToolTipNotify)
    ON_NOTIFY_REFLECT_EX(TTN_NEEDTEXTW, OnToolTipNotify)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void ToolTipComboBox::PreSubclassWindow()
{ 
    CComboBox::PreSubclassWindow(); 

    // Enabling tool tips for control seems to suppress 
    // tool tip functionality for parent window (w.r.t this control)
    // - and doesn't enable tooltips for this control anyway..
    //EnableToolTips(TRUE);
}


CString ToolTipComboBox::getToolTipText()
{
    int curSel = GetCurSel();
    if (curSel >= 0)
    {
        CString rString;
        GetLBText(curSel, rString);
        return rString;
    }
    else
    {    
        return "";
    }
}


LRESULT ToolTipComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
    if (WM_NOTIFY == message)
    {
        NMHDR TTN_NEEDTEXT *tem=(NMHDR FAR *)lParam;
        if (TBN_GETINFOTIP == tem->code)
        {
            AfxMessageBox("ToolTip");          
        }
    }
    */

    return CComboBox::WindowProc(message, wParam, lParam);
}


BOOL ToolTipComboBox::OnToolTipNotify(NMHDR * pNotifyStruct, LRESULT* result)
{
    // NOTE: This never gets called, enabling tool tips doesn't
    // cause required notifications to be passed around

    // Return FALSE in order to have the parent handle this notification
    // (parent handles by calling getToolTipText)
    return FALSE;
}
