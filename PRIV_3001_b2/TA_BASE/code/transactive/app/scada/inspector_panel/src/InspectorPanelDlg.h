/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/inspector_panel/src/InspectorPanelDlg.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides the main dialog used to display the data point information.  
  * It has a tab control with 3 associated dialogs (1 per tab).  In addition it also 
  * has two other dialogs (control details and information details) either of which 
  * can be displayed instead of the main dialog.  
  */

#ifndef INSPECTORPANELDLG_H
#define INSPECTORPANELDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/MainTabControl.h"
#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/DirectControlDialog.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/utilities/src/RunParams.h"

/////////////////////////////////////////////////////////////////////////////
// CInspectorPanelDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
	class CInspectorPanelDlg :	public TA_Base_Bus::TransActiveDialog, 
									public TA_Base_Core::RunParamUser,
									public TA_Base_Bus::IRightsChangeObserver
	{			
	// Construction
	public:

		CInspectorPanelDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor
		virtual ~CInspectorPanelDlg();
		
		//TD18095, jianghp, to fix the performance of showing manager application
		void init();
		//TD18095, jianghp, to fix the performance of showing manager application

		// This will move to the specified page
		// The page names are;
		//   Control
		//   Information
		//   Tag
		//   Maintenance
		//
		void			setPage(std::string pageName);
		void			setupPermissions();
		void			updatePermissions();
		void			setEquipment(std::string equipmentName);
		void			destroyEquipment();

		void			updateMainDialog();
        void            setEquipmentLabelInMainThread(const std::string& label);
        void            setTitleInMainThread(const std::string& label);
        std::string     getEquipmentNameFromDataPointEntity(TA_Base_Core::IEntityData* entity);
        void            loadDataPointsLater()			
        { PostMessage(LOAD_DATAPOINTS_MESSAGE); }
		void			resetList();



		// TD #11930
		// Get rid of these public static methods used to update the status window.
		// This will be done via calls to PostMessage from now on.
		//static void		addStatusLine ( const char* statusText, unsigned long timestamp = 0 );
		//static void		clearStatusMessage();

		// Implementation of RunParamUser interface
		virtual void	onRunParamChange(const std::string& name, const std::string& value);

		//Implementation of IRightsChangeObserver interface
		virtual void 	OnRightsChange(std::string sessionId, TA_Base_Bus::ERightsChangeType changeType, TA_Base_Bus::DutyChanges& changes );

		// Dialog Data
		//{{AFX_DATA(CInspectorPanelDlg)
	enum { IDD = IDD_INSPECTOR_PANEL_DIALOG };
		CEdit	m_statusBox;
		CMainTabControl		m_MainTab;
	//}}AFX_DATA

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInspectorPanelDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL

        /**
         * OnWindowPosChanging
         *
         * This method controls the resizing of the frame.
         * It ensures the frame is not resized to an inappropriate size.
         */
        virtual afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);

		// Implementation
	protected:
	
		// Generated message map functions
		//{{AFX_MSG(CInspectorPanelDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		virtual void OnCancel();
		afx_msg void OnClickMaintab(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnUpdateLabelInMainThread(UINT wParam, LONG lParam);
        afx_msg LRESULT OnProcessRunParamInMainThread(UINT wParam, LONG lParam);
	    afx_msg LRESULT OnUpdateStatusWindow(UINT wParam, LONG lParam);
		afx_msg void OnHelpButton(); //TD15124 azenitha
		afx_msg void OnClose(); //TD18095, jianghp, to fix the performance of showing manager application
		afx_msg void OnDestroy(); //2008-07-23
	//}}AFX_MSG

		LRESULT OnProxyEntityUpdate (WPARAM wParam, LPARAM commandID);
		LRESULT OnLoadDataPoints (WPARAM wParam, LPARAM lParam);

		void resizeIfRequired();

		DECLARE_MESSAGE_MAP()

	protected:
		
		HICON									m_hIcon;
		CEquipment*								m_equipment;
		CMainTabControl::ETabNumbers			m_previousTab;
		CMainTabControl::ETabNumbers			m_requestedTab;
		std::string								m_requestedPage;
		int										m_heightDifference;

		// Resource ID of this gui entity
		unsigned long							m_resourceId;

		// Rights Library Manager to get permissions for actions within this application
		TA_Base_Bus::RightsLibrary*				m_rightsMgr;
        CDirectControlDialog                    m_directControlDlg;
        bool                                    m_isInDirectControl;
		bool									m_isInitialTab;

    private:

        int m_currentScreen;
        CRect m_workingArea;

	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifndef INSPECTORPANELDLG_H
