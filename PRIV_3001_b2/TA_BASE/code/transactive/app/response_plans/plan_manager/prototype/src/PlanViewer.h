/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewer.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// PlanViewer.h : main header file for the PLANVIEWER application
//

#if !defined(AFX_PLANVIEWER_H__C75EC6C4_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_)
#define AFX_PLANVIEWER_H__C75EC6C4_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerApp:
// See PlanViewer.cpp for the implementation of this class
//

class CPlanViewerApp : public CWinApp
{
public:
    CPlanViewerApp();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanViewerApp)
    public:
    virtual BOOL InitInstance();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
    virtual int Run();
    // }}AFX_VIRTUAL

// Implementation

    // {{AFX_MSG(CPlanViewerApp)
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANVIEWER_H__C75EC6C4_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_)
