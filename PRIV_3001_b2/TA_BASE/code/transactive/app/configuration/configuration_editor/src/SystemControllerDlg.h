/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/configuration/configuration_editor/src/SystemControllerDlg.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the dialog used to allow the user to enter the System Controller host name and
  * port number.
  */

#if !defined(AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_)
#define AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/configuration_editor/src/resource.h"

namespace TA_Base_App
{
    class SystemControllerDlg : public CDialog
    {
    public:
        /**
         * Constructor
         */
	    SystemControllerDlg(CWnd* pParent = NULL);   // standard constructor


        /**
         * Destructor
         */
        virtual ~SystemControllerDlg(){ };


    // Dialog Data
	    //{{AFX_DATA(SystemControllerDlg)
	    enum { IDD = IDD_SYSTEMCONTROLLER_DIALOG };
	    CEdit	m_portNumber;
	    CEdit	m_hostName;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SystemControllerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    	HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(SystemControllerDlg)
	    afx_msg void onOK();
	    afx_msg void onChangeHostName();
	    afx_msg void onChangePortNumber();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:

        /**
         * refresh
         *
         * This checks the status of the controls and enables or disables the OK button.
         */
        void refresh();
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_)
