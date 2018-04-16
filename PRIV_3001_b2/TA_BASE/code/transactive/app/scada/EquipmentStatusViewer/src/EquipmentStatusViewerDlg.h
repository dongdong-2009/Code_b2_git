/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerDlg.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Dialog class of the main dialog responsible for displaying the equipment status list,
 * location, report names, toolbar, status bar etc
 *
 */

#if !defined(AFX_EQUIPMENTSTATUSVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_EQUIPMENTSTATUSVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/scada/EquipmentStatusViewer/src/DnUpdateProcessor.h"
#include "app/scada/EquipmentStatusViewer/src/dlgbars.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusListCtrl.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusListItem.h"
#include "app/scada/EquipmentStatusViewer/src/progressbar.h"
#include "app/scada/EquipmentStatusViewer/src/ConfigurationDialog.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_App
{
    class DataNodeAgentProxy;

	class CEquipmentStatusViewerDlg : public TA_Base_Bus::TransActiveDialog, 
									  public TA_Base_Core::RunParamUser,
									  public DnUpdateProcessCallBack
	{

	public:
	
		CEquipmentStatusViewerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback);

		virtual ~CEquipmentStatusViewerDlg();

		virtual void init();

		virtual void onRunParamChange(const std::string& name, const std::string& value);

		void processorCallBack(boost::shared_ptr<DnUpdateRequest> newRequest);

		//void resetDnUpdateProcessor();

        //EquipmentStatusListItemSharedPtr getReportItem( unsigned long entityKey) const;
		
		void notifyDnUpdate(std::vector<unsigned long> & dnKeys);

		//{{AFX_DATA(CEquipmentStatusViewerDlg)
	    enum { IDD = IDD_ESV_DIALOG };
	    CButton	m_StopButton;
	    //}}AFX_DATA

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEquipmentStatusViewerDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

	protected:
		// Generated message map functions
		//{{AFX_MSG(CEquipmentStatusViewerDlg)
		virtual BOOL OnInitDialog();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnViewStatusbar();
		afx_msg void OnViewToolbar();
		//afx_msg void OnEditConfigurereport();
		afx_msg void OnExport();
		//afx_msg void OnFilter();
		afx_msg void OnPrint();
		//afx_msg void OnNewreport();
		//afx_msg void OnEditReport();
		afx_msg void OnColumnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
		//afx_msg void OnFileDeletereport();
		afx_msg void OnHelpEquipmentstatusviewerhelp();
		afx_msg void OnToolbarHelp();
		afx_msg void OnSelchangeLocationCombo();
		//afx_msg void OnSelchangeReportCombo();
		//afx_msg void OnEditConfigurefilter();
		afx_msg void OnExit();
		afx_msg void OnHelpAboutequipmentstatusviewer();
		//afx_msg void OnEditInspect();
		//afx_msg void OnToolbarInspect();
		afx_msg void OnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg LRESULT onLaunchActive(WPARAM wParam, LPARAM lParam);
    	afx_msg LRESULT OnListItemUpdate(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnSessionIdChange(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnSortListCtrl(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnClose();
	    //afx_msg void OnStop();
	    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg void OnRdblclkEquipmentList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
		DECLARE_MESSAGE_MAP();

	private:
		/**
		 *	CreateToolbar()
		 *
		 *	Creates tollbar for the window
		 *
		 */
		void CreateToolbar();

		/**
		 *	AddTextToButton()
		 *
		 *	Add text to the button of a toolbar
		 *
		 *	@param	buttonId	Id of the button to set the text for
		 *	@param	buttonLabel Text of the button
		 *
		 */
		void AddTextToButton(UINT buttonId, CString buttonLabel );

        /** 
          * UpdateStatusBar
          *
          * Redraw the text in the status bar to reflect the current filtering.
          *
          */
		void UpdateStatusBar();

        /** 
          * Resize
          *
          * Make the list control fill the available space.
          *
          */
        void Resize();

        /** 
          * AdjustGUIWhileLoading
          *
          * Disable or enable all the GUI components while loading the report
          *
          * @param aEnableAllGUIComponents  True = enable the components
          *                                 False = disable (grey out) the components
          */
		void AdjustGUIWhileLoading(bool aEnableAllGUIComponents);

        /** 
          * ClearCurrentFiltersForActive
          *
          * Prepares the current filters to match the MMS/Inhibit/Override/Tagged
          * flags required for the Active Profile reports.
          */
		void ClearCurrentFiltersForActive();

        /** 
          * IsFiltered
          *
          * Are any filters in use?
          * Note that this function updates the status bar display.
          *
          * @return     True if any filters are in use.
          */
		bool IsFiltered();

		/**
		 *	CombineFilters()
		 *
		 *	Clears current and reads db filters and combines them both and stores them in 
		 *  m_currentFilters.
		 *
		 *	@param	aReportKey	Primary key of the report for which filters must be read in
		 *
		 */
        void CombineFilters(std::string& aReportKey);

		/**
		 *	ClearCurrentFilters()
		 *
		 *	Clears the current filters by setting them to false
		 *
		 */
		void ClearCurrentFilters();

		/**
		 *	UnloadFiltersFromDB()
		 *
		 *	Unloads filters from the database by emptying the collection
		 *
		 */
		void UnloadFiltersFromDB();

		/**
		 *	LoadFiltersFromDB()
		 *
		 *	Loads filters from the database into the collection
		 *
		 *	@param	aReportKey	Primary key of the report for which to read filters
		 *	@return				Flag to signify success (true) or failure (false)
		 *
		 */
        bool LoadFiltersFromDB(std::string& aReportKey); 

		/**
		 *	checkPermissions()
		 *
		 *	Checks permission by calling the Access Control library to identify what the 
		 *	current operator is allowed to perform within this application
		 *
		 */
		void checkPermissions();

		/**
		 *	AdjustGUI()
		 *
		 *	Changes the state of GUI controls based on the current state of the 
		 *	dialog, enables/disable buttons etc.
		 *
		 */
		void AdjustGUI();

		/**
		 *	Allow SetStatusBarText of the named class to become a friend of this main
		 *  dialog class and set its status bar text message
		 *
  		 */
		friend void CConfigurationDialog::SetStatusBarText(const CString& message);

		/**
  		 *  SetStatusBarText()
  		 *
		 *	Sets the message in the status bar control
		 *
		 *	@param	message		Message to display
		 *
  		 */
		void SetStatusBarText(int aIndex, const CString& message);

		/**
		 *	CalculateToolBarStatusBarHeights()
		 *
		 *	Calculates heights of status bar and toolbar controls for the window
		 *
		 */
		void CalculateToolBarStatusBarHeights();

		/**
		 *	CreateStatus()
		 *
		 *	Creates statusbar for the window
		 *
		 */
		void CreateStatusBar();

		/**
		 *	PopulateLocations()
		 *
		 *	Populates the combo box with all available locations
		 *
		 */
		void PopulateLocations();

		/**
		 *	PopulateReports()
		 *
		 *	Populates the combo box with all available reports for a particular location
		 *
		 *	@param	aLocationKey	Primary key of the location for which to read available reports
		 *
		 */
		//void PopulateReports(const unsigned long aLocationKey);

		/**
		 *	PopulateEquipmentStatusList()
		 *
		 *	Populates the equipment status list with equipment/datapoint information corresponding
		 *	to the selected report.
		 *
		 *	@param	aReportKey		Primary key of the report to use in populating the list
		 *  @param	aInitProxies	To initialise (true) proxies for equipment/datapoints or not (false)
		 *
		 */
        //void PopulateEquipmentStatusList(std::string& aReportKey, bool aInitProxies = true);
		
		void PopulateEquipmentStatusList(unsigned long locationKey);

		/**
		 *	SelectReportByKey()
		 *
		 *	Selects the requested report in the Report name combobox by searching the combobox items' data
		 *	for the required report key.
		 *
		 *	@param	aReportKey		Primary key of the report to select
		 *
		 */
        //void SelectReportByKey(std::string& reportKey);

		/**
		 *	CleanUpResource()
		 *
		 *	call before process exit, only call one time
		 *
		 */
		void CleanUpResource();

	private:

		bool							m_isAllowedToPrintOrExport;

		bool							m_isToolTip;
	
		bool							m_isAllowedToConfigure;

		bool							m_isAllowedToViewAllReports;

		bool							m_needSortListCtrl;

		bool							m_hasCleanUp;

		CComboBox						m_Location;

		CComboBox						m_ReportName;

		CEquipmentStatusListCtrl		m_EquipmentStatusList;

		CStatusBar						m_WndStatusBar;

		CDlgToolBar						m_WndToolBar;

		/**
		 *	Toolbar height
		 *
		 */
		UINT							m_ToolBarHeight;

		UINT							m_StatusBarHeight;

        unsigned long					m_guiEntityKey;

		/**
		 *	Flag which indicates if filters are currently applied
		 *
		 */
        unsigned long					m_isFiltered;

		/**
		 *	Collection of current filter flags and strings
		 *
		 */
        FilterFlagsAndStrings			m_currentFilters;

		/**
		 *	Collection of filter flags and strings stored in the database
		 *
		 */
        FilterFlagsAndStrings			m_storedFilters;

		/**
		 *	Signifies if this is an active report or database originated report
		 *
		 */
		bool							m_isActiveReport;

		unsigned long					m_selectedLocationKey;

		std::string						m_selectedReportKey;

		std::map< Active_Profile, CString>				m_activeProfileNameConstants;

		TA_Base_Bus::AuthenticationLibrary*				m_authenticationLibrary;

		TA_Base_Core::ProfileAccessFactory&				m_profileAccessFactory;
 		
		EquipmentStatusListItemMap						m_reportItems;

		TA_Base_Core::NonReEntrantThreadLockable		m_reportItemsLock;
      
		std::vector< EquipmentReportEntity>				m_unprocessedEntities;

        unsigned int									m_populatorTimer;

        CProgressBar									m_progressBar;
	
		std::map<int, std::string>						m_reportKeyStringToIdMap;

		std::map< std::string, DataNodeAgentProxy* >	m_DatanodeAgentProxy;

		TA_Base_Bus::IGUIAccess&						m_genericGUICallback;	

		TA_Base_Bus::ScadaProxyFactory&					m_scadaProxyFactory; 

		std::map<unsigned long, boost::shared_ptr<TA_Base_Core::DataNodeEntityData> > m_dnEntityDataPtrMap;
		
		unsigned long									m_curLoginLocation;

		DnUpdateProcessor *								m_DnUpdateProcessor;

		static const unsigned long						OCC_LOCATION_KEY_IN_DATABASE;

		static const std::string						PARENT_ENTITY_NAME;

		std::string										m_previousSessionId;
    };

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPMENTSTATUSVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
