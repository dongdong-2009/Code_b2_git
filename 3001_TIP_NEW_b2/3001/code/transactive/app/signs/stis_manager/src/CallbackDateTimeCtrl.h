/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/CallbackDateTimeCtrl.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is a Date time control that listens for its own change events
  * and passes them on to the specified interface.
  * Done so that the owning dialog doesnt have to listen for and handle the events.
  */

#if !defined(AFX_CallbackDateTimeCtrl_H__DBFC6652_457D_4F5E_910E_FD94B027CEE3__INCLUDED_)
#define AFX_CallbackDateTimeCtrl_H__DBFC6652_457D_4F5E_910E_FD94B027CEE3__INCLUDED_

#include "IDateTimeListener.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_App
{

    class CallbackDateTimeCtrl : public CDateTimeCtrl
    {

    public:
	    CallbackDateTimeCtrl();
        virtual ~CallbackDateTimeCtrl();

        void setCallback(IDateTimeListener* listener);

    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CallbackDateTimeCtrl)
	    //}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CallbackDateTimeCtrl)
        afx_msg BOOL onDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
        IDateTimeListener* m_listener;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_CallbackDateTimeCtrl_H__DBFC6652_457D_4F5E_910E_FD94B027CEE3__INCLUDED_)
