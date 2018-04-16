/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CRadioManagerDlg.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_CRADIOMANAGERDLG_H__D3329E18_7E44_48CD_AAF7_7ED549ADD38B__INCLUDED_)
#define AFX_CRADIOMANAGERDLG_H__D3329E18_7E44_48CD_AAF7_7ED549ADD38B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/radio_manager_gui/src/RadioStatusBar/CRadioManagerDlgStatusBar.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"
#include "app/radio/radio_manager_gui/src/CtaRadioManagerSummary.h"
#include "app/radio/radio_manager_gui/src/CRadioMonitoringSheet.h"
#include "app/radio/radio_manager_gui/src/RadioManagerFramework.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentFramework.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "app\radio\radio_manager_gui\src\RadioTetraAgentCallbackSubscriber.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

#include "RadioInitMsgDlg.h"

#include <list>
#include <string>


/////////////////////////////////////////////////////////////////////////////
// CRadioManagerDlg dialog

class CRadioManagerDlg : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser
{  

	public:

		CRadioManagerDlg::CRadioManagerDlg( TA_Base_Bus::IGUIAccess& genericGUICallback,
											CWnd* pParent = NULL );

		virtual ~CRadioManagerDlg();

		//{{AFX_DATA(CRadioManagerDlg)
		enum { IDD = IDD_RADIOMANAGER_DIALOG };
		// peter.wong, disable toolbar buttons
		//CButton	m_locateBtn;
		//CButton	m_newEntityBtn;
		//CButton	m_inboxBtn;
		//CButton	m_dialpadBtn;
		//CButton	m_emergencyBtn;
		//}}AFX_DATA


		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

		//{{AFX_VIRTUAL(CRadioManagerDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL
		

	public:
        
		void init(); //TD18626 AVteam
		/** 
          * onRunParamChange
          *
          * Called for a runparam change. selection and deselection.
          */
	    virtual void onRunParamChange( const std::string& name, const std::string& value );


		/**
		* accessor methods of CtaRadioManagerSummary
		*/
		CtaRadioManagerSummary* getCtaRadioManagerSummary();

		void setCtaRadioManagerSummary(CtaRadioManagerSummary* pCtaRadioMgrSummary);
		
		/**
		* checkRights
		*
		* method to check permission of operator 
		*/
		bool checkRights(TA_Base_Bus::AccessControlledAction action);

		virtual BOOL DestroyWindow();

	protected:
		HICON m_hIcon;
		virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);

		//{{AFX_MSG(CRadioManagerDlg)
		virtual BOOL OnInitDialog();
        afx_msg void OnDestroy();
		// peter.wong, disable toolbar buttons
		//afx_msg void OnDialPadToolbarButton();
		//afx_msg void OnInboxToolbarButton();
		//afx_msg void OnNewEntity();
		//afx_msg void OnEmergencyModeToolbarButton();
		//afx_msg void OnLocateSpeakerBtn();
		afx_msg LRESULT onNotifiedEditMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedOperationMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedEntitySelectionChanged(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedLaunchPatchDlg(WPARAM wParam = 0, LPARAM lParam = 0); // Fix TD#13911
		afx_msg LRESULT onNotifiedInitialiseMode(WPARAM wParam = 0, LPARAM lParam = 0); // Fix TD#13911
		//afx_msg LRESULT onNotifiedLaunchInboxPage(WPARAM wParam = 0, LPARAM lParam = 0); //TD16926
		afx_msg LRESULT onNotifiedApiFailureMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onRelaunchRadioManager(WPARAM wParam=0, LPARAM lParam=0);
		afx_msg void OnClose();
		afx_msg void OnTimer(UINT nIDEvent);
		
		//}}AFX_MSG	
		DECLARE_MESSAGE_MAP()

	private:

        /** 
          * methods to create Controls/Dialogs
          */
		void createStatusPage();

		void createDirectoryPage();

		void createEntityDetailsArea();

		void createEntityOperationsArea();
		
		void createStatusBar();

		void enableControls(BOOL flag);

		void ModifyWindowPos();

	// Attributes
	private:
			
		CToolBar m_Toolbar;

        std::list<std::string> m_recentDialedList;
        BOOL m_editMode;

		bool m_isLaunched;
		bool m_isAssignedBySchematic;

		// specifies whether to run the app in the background at startup
		// and hide window when it is closed.
		// set to false for debugging, with --ShowWindow
		bool m_runInBackground;

	protected:
		
		CtaRadioManagerSummary* m_CtaRadioManagerSummary;


	public:

		CRadioMonitoringSheet *m_CRadioMonitoringSheet;

		CRadioDirectoryPageDlg *m_CRadioDirectoryPageDlg;

		CEntityDetailsAreaDlg *m_CEntityDetailsAreaDlg;

		CEntityOperationsAreaDlg *m_CEntityOperationsAreaDlg;

		CRadioManagerDlgStatusBar *m_CRadioManagerDlgStatusBar;

		RadioMFTAgentFramework* m_MFTFramework;

		RadioTetraAgentCallbackSubscriber* m_radioCallback;

	private:
		void loadRadioInitDlg (bool bSetSpecDlg, bool bAddDlg = false);
		void unloadRadioInitDlg (bool bSetSpecDlg);

		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


#endif
