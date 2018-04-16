/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/4669/transactive/app/ecs/ecs_manager/src/TimetableSelectionDlg.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/05/26 16:20:00 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#if !defined(AFX_TIMETABLESELECTIONDLG_H__23FD578F_5922_483A_A4FD_444E10465824__INCLUDED_)
#define AFX_TIMETABLESELECTIONDLG_H__23FD578F_5922_483A_A4FD_444E10465824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/AuditMessageUtility.h"
//TD18095, jianghp, to fix the performance of showing manager application
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/message/types/ECSAudit_MessageTypes.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
}

namespace TA_IRS_App
{
	class MappedECSDataCache;

    std::string getControlStatusString(TA_Base_Bus::EDataPointControlState state);
    
    class CTimetableSelectionDlg : public TA_Base_Bus::TransActiveDialog, TA_Base_Core::RunParamUser, TA_Base_Bus::IEntityUpdateEventProcessor,
		public virtual IBaseView
	{

	public:		

		CTimetableSelectionDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL);

		virtual inline void init()
		{//TD18095, jianghp, to fix the performance of showing manager application
			//do nothing....
		};		
		/**
		 *	onRunParamChange()
		 *
		 *	Callback which notifies us of a change in a run parameter's value
		 *	See TA_Base_Core::RunParamUser
		 *
		 */
		virtual void onRunParamChange(const std::string& name, const std::string& value);

        void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	protected:

		HICON m_hIcon;
		// Generated message map functions
		//{{AFX_MSG(CTimetableSelectionDlg)
		virtual BOOL OnInitDialog();	
		afx_msg void OnApply();
		afx_msg void OnClose();
		afx_msg void OnDestroy();
		afx_msg void OnAppAbout();
		afx_msg void OnAppHelp();
		afx_msg LRESULT onCheckPermissions(WPARAM wParam, LPARAM lParam);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	    afx_msg void OnSelchangeStation();
	    afx_msg void OnSelchangeNewTimetable();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnHelpButton(); //TD16730
        afx_msg LRESULT OnEntityUpdateMessage(WPARAM wParam, LPARAM lParam);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

    private:
		
		void CheckPermissions();
		void WriteStatusLine(const CString &lineOfText);		
		void LoadStations();
		void SaveChanges();	
		
		//{{AFX_DATA(CTimetableSelectionDlg)
		enum { IDD = IDD_TIMETABLE_CHANGE_DLG };
		CButton	m_ApplyButton;
		CComboBox	m_NewTimetableCombo;
		CEdit	m_CurrentTimetableEdit;
		CComboBox	m_StationCombo;
		//}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTimetableSelectionDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	private:

		typedef std::map<unsigned long, std::string> TimetableStateMap;
		static const int DEFAULT_TIME_TABLE_STATE;

		bool m_isAllowedToSetTimetable;
		bool m_hasChanged;
		std::auto_ptr<MappedECSDataCache> m_pMappedECSDataCache; 
		
        TA_Base_Bus::DataPointProxySmartPtr m_currentTimetableDataPoint;
		TA_Base_Bus::DataPointProxySmartPtr m_selectTimetableDataPoint;		
		TA_Base_Bus::DataPointProxySmartPtr m_stationVisualLockDataPoint;
        
        TimetableStateMap m_selectTimetableStateMap;

        TA_Base_Core::ReEntrantThreadLockable m_mapLock;

		TA_Base_Bus::ScadaProxyFactory*					m_proxyFactory;
	};

} // namespace

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMETABLESELECTIONDLG_H__23FD578F_5922_483A_A4FD_444E10465824__INCLUDED_)
