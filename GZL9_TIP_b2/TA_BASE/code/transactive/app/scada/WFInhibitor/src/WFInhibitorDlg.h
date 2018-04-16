/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/scada/wf_inhibitor/src/WFInhibitorDlg.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides the main dialog used to display the data point information.  
  * It has a tab control with 3 associated dialogs (1 per tab).  In addition it also 
  * has two other dialogs (control details and information details) either of which 
  * can be displayed instead of the main dialog.  
  */

#ifndef WF_INHIBITOR_DLG_H
#define WF_INHIBITOR_DLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

#include "app/scada/WFInhibitor/src/resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
/////////////////////////////////////////////////////////////////////////////
// CWFInhibitorDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
	class CWFInhibitorDlg : public TA_Base_Bus::TransActiveDialog, 
							public TA_Base_Core::ITimeoutCallback
	{			
	// Construction
	public:

		CWFInhibitorDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor
		virtual ~CWFInhibitorDlg();

		virtual void timerExpired(long timerId, void* userData);		
		// Implementation of RunParamUser interface
		//virtual void	onRunParamChange(const std::string& name, const std::string& value);
		
		enum { IDD = IDD_WF_INHIBITOR_DIALOG };
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

		std::string getCurrentTimeStr();
		void 	displayResultInfo(CString strResult);
	protected:
		HICON 	m_hIcon;
		CEdit	m_statusBox;

		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnBnClickedButtonWfInhibit();
		afx_msg void OnBnClickedCancel();
		
		bool	m_wfInhibit;
		volatile bool 	m_globalInhibitStatus;
        TA_Base_Core::SingletonTimerUtil& m_timerUtility;

		DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifndef WF_INHIBITOR_DLG_H
