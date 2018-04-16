/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/TisLogViewer.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#if !defined(AFX_TISLOGVIEWER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
#define AFX_TISLOGVIEWER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// TisLogViewerApp:
// See TisLogViewer.cpp for the implementation of this class
//
namespace TA_IRS_App
{
    class TisLogViewerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        TisLogViewerApp();

        unsigned long getRefreshIntervalInSecs();
        void setRefreshIntervalInSecs( unsigned long interval );


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(TisLogViewerApp)
        public:
        // }}AFX_VIRTUAL

    // Implementation

        // {{AFX_MSG(TisLogViewerApp)
            // NOTE - the ClassWizard will add and remove member functions here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        unsigned long m_refreshInterval;

    };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_App

#endif // !defined(AFX_TISLOGVIEWER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
