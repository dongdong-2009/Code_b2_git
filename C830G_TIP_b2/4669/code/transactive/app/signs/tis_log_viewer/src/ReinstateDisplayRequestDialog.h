/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/ReinstateDisplayRequestDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_REINSTATEDISPLAYREQUESTDIALOG_H__A689C946_4AAB_4F63_BC2B_D330A8634891__INCLUDED_)
#define AFX_REINSTATEDISPLAYREQUESTDIALOG_H__A689C946_4AAB_4F63_BC2B_D330A8634891__INCLUDED_

#include "app/signs/tis_log_viewer/src/DisplayRequestSender.h"
#include "app/signs/tis_log_viewer/src/resource.h"

#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReinstateDisplayRequestDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ReinstateDisplayRequestDialog dialog

namespace TA_IRS_App
{
    class DisplayRequestSender;
}

namespace TA_IRS_App
{

    class ReinstateDisplayRequestDialog : public CDialog
    {
    // Construction
    public:
	    ReinstateDisplayRequestDialog( std::vector<TA_IRS_Core::IDisplayRequest*> requests,
                                       TA_IRS_Core::IDisplayRequest* lastSelectedItem,
                                       DisplayRequestSender& displayRequestSender,
                                       CWnd* pParent = NULL);   // standard constructor

        void reinstateDisplayRequest();

    // Dialog Data
	    //{{AFX_DATA(ReinstateDisplayRequestDialog)
	    enum { IDD = IDD_REINSTATE_DIALOG };
	    CButton	m_adhocToPredefinedCheck;
	    CEdit	m_textEntry;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ReinstateDisplayRequestDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ReinstateDisplayRequestDialog)
	    virtual void OnOK();
	    virtual BOOL OnInitDialog();
	    afx_msg void OnConvertToAdHoc();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // Stolen from STIS Manager ... no time to move code!!!!
        static std::string getOperatorNameFromSession( const std::string& currentSessionID );

        // The display that we may be reinstating.

	    //the all records selected
        std::vector<TA_IRS_Core::IDisplayRequest*> m_requests;

	    //the last selected record
	    TA_IRS_Core::IDisplayRequest* m_lastSelectedItem;

        // The object used to send the requests
        DisplayRequestSender& m_displayRequestSender;
    };


} // namespace TA_IRS_App

#endif // !defined(AFX_REINSTATEDISPLAYREQUESTDIALOG_H__A689C946_4AAB_4F63_BC2B_D330A8634891__INCLUDED_)


