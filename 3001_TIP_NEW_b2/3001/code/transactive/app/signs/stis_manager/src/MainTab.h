/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/MainTab.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This creates and manages each of the three main tabs at the top of the app.
  *
  */

#if !defined(AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_)
#define AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainTab.h : header file
//

#include "DisplayPage.h"
#include "LibraryVersionPage.h"
#include "TimeSchedulePage.h"
//#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"
#include "TisDialogCtrl.h"

class REBProgressManager;
/////////////////////////////////////////////////////////////////////////////
// MainTab window
namespace TA_IRS_App
{

    class MainTab : public CTisDialogCtrl
    {
    public:
	    MainTab();
        virtual ~MainTab();

        void Init();

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(MainTab)
	    public:
	    virtual BOOL PreTranslateMessage(MSG* pMsg);
	    //}}AFX_VIRTUAL

		CDialog* getMainTabPage(UINT iPage);
		DisplayPage& getDisplayPage();		
		LibraryVersionPage& getupgradePage();
		TimeSchedulePage& getTimeSchedulePage();
    protected:
	    //{{AFX_MSG(MainTab)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
	    DisplayPage m_displayPage;
		TimeSchedulePage m_timeSchedulePage;
        LibraryVersionPage m_upgradePage;
		bool m_isOccLocation;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_)
