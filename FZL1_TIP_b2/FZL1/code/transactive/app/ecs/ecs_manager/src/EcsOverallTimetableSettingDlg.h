/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/EcsOverallTimetableSettingDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_OVERALLECSTIMETABLESETTINGDLG_H__D440A8C3_40C1_4B34_A679_DBE449BA2D94__INCLUDED_)
#define AFX_OVERALLECSTIMETABLESETTINGDLG_H__D440A8C3_40C1_4B34_A679_DBE449BA2D94__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/resource.h"
#include "app/ecs/ecs_manager/src/ECSOverallTimetableListCtrl.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/AuditMessageUtility.h"
//TD18095, jianghp, to fix the performance of showing manager application
#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/message/types/ECSAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"

/////////////////////////////////////////////////////////////////////////////
// COverallEcsTimetableSettingDlg dialog
namespace TA_IRS_App
{
	/////////////////////////////////////////////////////////////////////
	//Launch Manager modified by: zhangjunsheng 2008/09/11 11:00:00

	//	const int TIMER_INTERVAL = 500;
	const int TIMER_ID = 5;
	const int TIMER_INTERVAL = 100000;
	
	//Launch Manager/////////////////////////////////////////////////////

	class COverallEcsTimetableSettingDlg : public TA_Base_Bus::TransActiveDialog, 
										   TA_Base_Core::RunParamUser, 
										   TA_Base_Bus::IEntityUpdateEventProcessor,
										   public virtual IBaseView
	{
	 public:

		COverallEcsTimetableSettingDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL);   // standard constructor

		//TD18095, jianghp, to fix the performance of showing manager application
		virtual void init();

	    /**
		 *	onRunParamChange()
		 *
		 *	Callback which notifies us of a change in a run parameter's value
		 *	See TA_Base_Core::RunParamUser
		 *
		 */
		virtual void onRunParamChange(const std::string& name, const std::string& value);

		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
		void LoadStations();
		void WriteStatusLine(const CString &lineOfText);
		void InitializeListControl();
		void SaveChanges();
		void initialiseDataPointsProxy();
		void setupOffsets();

		//{{AFX_DATA(COverallEcsTimetableSettingDlg)
		enum { IDD = IDD_ECSOVERALLTIMETABLE_SELECTION_DIALOG };
		COverallECSTimetableListCtrl	m_OverallECSTimetableList;
		CEdit							m_statusEditControl;
		CButton m_closeButton;
		//}}AFX_DATA
		CImageList m_StationSelectionImageList;

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(COverallEcsTimetableSettingDlg)
	protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //}}AFX_VIRTUAL
		
		// Overrides		
		//{{AFX_VIRTUAL(COverallEcsTimetableSettingDlg)


	protected:

		HICON m_hIcon;
		//{{AFX_MSG(COverallEcsTimetableSettingDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnColumnclickOverallEcsTimetableList(NMHDR* pNMHDR, LRESULT* pResult);	
		afx_msg void OnClose();
		afx_msg void OnApply();
		afx_msg void OnCancel();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnHelpButton();
		afx_msg void onInitialise();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg LRESULT onEnableCloseButton(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSessionChanged(WPARAM, LPARAM); // CL-20847
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	public:

		std::auto_ptr<MappedECSDataCache> m_pMappedECSDataCache; 
        std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_currentTimetableDataPoints;
        std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_selectTimetableDataPoints;
		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_stationVisualLockDataPoints;

	 private:


		CRect	m_SelectionRect;
		BOOL	m_SelectionFlag;
		CPoint  m_Point;
		int		m_Rows;
		int     m_Columns;
		int		m_PrevItem;
		int     m_iCurRow;
		int     m_iCurCol;		
        int     m_rightRelativeOffset;                          // Offset to maintain dialog geometry during resize.
        int     m_bottomRelativeOffset;                         // Offset to maintain dialog geometry during resize.
		bool	m_areOffsetsSetup;
		bool	m_isInitialise;

		TA_Base_Bus::ScadaProxyFactory*					m_proxyFactory;
	private:
		static  DWORD WINAPI ThreadFunc( LPVOID lpThreadParameter );			//Launch manager ESC jiangshengguang
		void	backgroundInitialiseDataPointsProxy();									//Launch manager ESC jiangshengguang

		void	createDataPointsProxies( std::vector<unsigned long>& currentLocations, TA_Base_Core::IEntityDataList& sourceList, std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>& resultMap );

		bool	CheckPermissions();		// CL-20847
	 
	};
}



#endif // !defined(AFX_OVERALLECSTIMETABLESETTINGDLG_H__D440A8C3_40C1_4B34_A679_DBE449BA2D94__INCLUDED_)
