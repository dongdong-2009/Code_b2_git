#if !defined(AFX_OTHERSESSUBDUTYPROPERTYPAGE_H__4AEFA01A_EE31_48A6_8590_A517DCA410ED__INCLUDED_)
#define AFX_OTHERSESSUBDUTYPROPERTYPAGE_H__4AEFA01A_EE31_48A6_8590_A517DCA410ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherSesSubDutyPropertyPage.h : header file
//
#include "app/security/DutyManager/src/DutyManager.h"
#include "bus/mfc_extensions/src/grid_control/GridCtrlS.h"
/////////////////////////////////////////////////////////////////////////////
// OtherSesSubDutyPropertyPage dialog
using TA_Base_App::CellData;
using TA_Base_App::PSSA;
using TA_Base_App::ProfilesWithDuties;

namespace TA_Base_App
{
	class SelectSesSubDutyDlg;
	class OtherSesSubDutyPropertyPage : public CPropertyPage,
										public TA_Base_Bus::ISessionObserver,
										public TA_Base_Bus::ISubsystemDutyObserver
	{
		DECLARE_DYNCREATE(OtherSesSubDutyPropertyPage)
			
			// Construction
	public:
		OtherSesSubDutyPropertyPage();
		virtual ~OtherSesSubDutyPropertyPage();

		void initialiseOhterSessionDutyGrid();
		
		virtual void sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate);

		// ISubsystemDutyObserver interface
        virtual void dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate);
		// Dialog Data
		//{{AFX_DATA(OtherSesSubDutyPropertyPage)
	enum { IDD = IDD_OTHER_SES_SUB_DLG };
	CEdit	m_selectedSessionEdit;
	CButton	m_showAllSubsystemsOtherSesBtn;
	CButton	m_ReloadDataBtn;
	CButton	m_SelectSesBtn;
	//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(OtherSesSubDutyPropertyPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		void addSubsystem(
			TA_Base_Bus::SubsystemKey subsystemKey, 
			TA_Base_Bus::ProfileKeyList profileKeys,
			TA_Base_Bus::RegionKeyList regionKeys); 

		void removeSubsystem(TA_Base_Bus::SubsystemKey subsystemKey);
        void updateSubsystems(TA_Base_Bus::SubsystemKeyList oldSubsystemKeys,
			TA_Base_Bus::SubsystemKeyList newSubsystemKeys, TA_Base_Bus::ProfileKeyList profileKeys); 

		void updateOtherSesSubDrid(TA_Base_Bus::SessionInfoEx sesInfEx);

		void updateOtherSessionInfoEdit(TA_Base_Bus::SessionInfoEx sesInfEx);

	protected:
		// Generated message map functions
		//{{AFX_MSG(OtherSesSubDutyPropertyPage)
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg LRESULT OnUpdateOtherSesSubDutyMessage(WPARAM, LPARAM);
		afx_msg LRESULT OnReceiveSessionUpdate(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveOhterSesSubDutyUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelectSessionBtn();
	afx_msg void OnReloadSesSubBtn();
	afx_msg void OnShowAllSubsCheckBtn();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	
	private:
		COLORREF convertColourStrToColorref( const std::string& colourStr, const COLORREF& defaultColour ) const;
		
		bool isRegionAccessible(
			const TA_Base_Bus::RegionKey regionKey,
			const TA_Base_Bus::RegionKeyList regionKeys);
		bool isSubsystemAccessible(
			const TA_Base_Bus::ProfileKeyList profileKeys,
			const TA_Base_Bus::SubsystemKey subsystemKey);
		
		TA_Base_App::CellData generateCellData(
			const TA_Base_Bus::SubsystemKey subsystemKey, 
			const TA_Base_Bus::RegionKey regionKey,
			const TA_Base_Bus::ESubsystemState subsystemState);
		
		std::string getActionGroupNameWithErrorChecking(
			const TA_Base_Bus::ActionGroupKey actionGroupKey);

		TA_Base_Bus::ActionGroupKey getActionGroupDB(
			const TA_Base_Bus::ProfileKey profileKey,
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::ESubsystemState subsystemState);
		TA_Base_App::ProfilesWithDuties getProfilesWithDuties(
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::RegionKey regionKey);
		
		COLORREF determineCellBkColour( const bool inRegion, const TA_Base_Bus::ESubsystemState state ) const;

		void sessionUpdateRegionDuty(boost::shared_ptr<TA_Base_Bus::SessionUpdate> sessionUpdate);
		void sessionUpdateSubsystemDuty(boost::shared_ptr<TA_Base_Bus::SessionUpdate> sessionUpdate);

		void enableButtonsAndSessionInfoEdit(bool isable);
		
		TA_Base_Bus::RegionKeyList getLocations();

		TA_Base_Bus::ProfileKeyList getProfileKeyList(
			const TA_Base_Bus::SubsystemKey subsystemKey,
			const TA_Base_Bus::RegionKey regionKey);
		
		CGridCtrlS m_sesSubDutyGrid;
		
		TA_Base_Bus::DutyAgentAccess m_dutyAgent;
        TA_Base_Bus::DutyRights m_dutyRights;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
		TA_Base_Bus::RegionDutyMatrix& m_regionDutyMatrix;
        TA_Base_Bus::SubsystemDutyMatrix& m_subsystemDutyMatrix;
        TA_Base_Bus::SessionInfoEx m_currentSessionInfo;
		TA_Base_Bus::SessionInfoEx m_selectSessionInfo;

		TA_Base_Bus::SessionId m_selectSessionId;
		
		COLORREF m_normalDutyColour;
        COLORREF m_delegatedDutyColour;
        COLORREF m_degradedDutyColour;
        COLORREF m_unallocatedSubsystemColour;
		
		std::vector<TA_Base_App::PSSA> m_actionGroupsDB;

		SelectSesSubDutyDlg* m_selectSesSubDutyDlg;

		bool m_isShowAllSubsystems;
	};
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERSESSUBDUTYPROPERTYPAGE_H__4AEFA01A_EE31_48A6_8590_A517DCA410ED__INCLUDED_)
