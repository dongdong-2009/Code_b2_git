#if !defined(AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_)
#define AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_

/**
  * HistoryViewerDlg.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/HistoryViewerDlg.h $
  * @author:  A. Lee
  * @version: $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The main dialog class.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include <map>

#include "app/history/history_viewer/src/GraphDataPoint.h"
#include "app/history/history_viewer/src/dlgbars.h"
#include "app/history/history_viewer/src/resource.h"
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/user_settings/src/SettingsMgr.h"

//{{AFX_INCLUDES()
#include "tchart.h"
//}}AFX_INCLUDES

const long secInDay = 86400;		// Number of seconds in a day.
const long sampleWarning = 5000;	// Warning if loading more than this
const long sampleLimit = 20000;		// Maximum number of samples


using TA_Core::IEntityData;


namespace TA_Core
{
    class AuditMessageSender;
}


namespace TA_App
{
    //
    // Forward declarations
    //
    class RealTimeDataUpdateSubscriber;
    class GraphRefreshThread;
    class DataPointOnlineUpdateSubscriber;
    class DataPointLoaderThread;

    //
    // CHistoryViewerDlg
    //
    class CHistoryViewerDlg
        : public TA_Bus::TransActiveDialog,
          public TA_Core::RunParamUser
    {

    public:

        /**
		  * CHistoryViewerDlg
		  *
		  * Constructor.
		  *
		  */
        CHistoryViewerDlg(TA_Bus::IGUIAccess* controlClass);

        /**
		  * ~CHistoryViewerDlg
		  *
		  * Destructor.
		  *
		  */
        virtual ~CHistoryViewerDlg() {}

        /**
		  * onRunParamChange
		  *
		  * Called when there is a change in a Run Param. Implemented to
          * satisfy the RunParamUser interface.
		  *
		  * @param name Run param name
          * @param value New run param value
		  *
		  */
        virtual void onRunParamChange(const std::string& name, const std::string& value);

        /**
		  * DDV_NoRange
		  *
		  * Verify that the range is not zero
		  *
		  * @param pDX Pointer to the dialog data exchange object
          * @param rangeSeconds Range on the X-axis in seconds
		  *
		  */
        static void DDV_NoRange(CDataExchange* pDX, long rangeSeconds);

        /**
		  * DDV_NoRange
		  *
		  * Test for range = 0
		  *
          * @param rangeSeconds Range on the X-axis in seconds
		  *
		  */
        static bool NoRange(const long& rangeSeconds);

        /**
		  * DDV_NoRange
		  *
		  * Verify that the range is not too big
		  *
		  * @param pDX Pointer to the dialog data exchange object
          * @param rangeSeconds Range on the X-axis in seconds
          * @param histConfig Sample type
		  *
		  */
        static void DDV_LargeRange(CDataExchange* pDX, long rangeSeconds, int histConfig);

        /**
		  * DDV_NoRange
		  *
		  * Test for large range
		  *
		  * @param rangeSeconds Range on the X-axis in seconds
          * @param histConfig Sample type
		  *
		  */
        static bool LargeRange(const long& rangeSeconds, int histConfig);

        /**
		  * addDataPoint
		  *
		  * Add a new trend to the graph
		  *
		  * @param dataPointKey Entity key of the data point to be added
          *
		  */
        void addDataPoint(unsigned long dataPointKey);

        /**
		  * updateRealTimePlot
		  *
		  * Updates the graph for a particular trend with a new value
		  *
		  * @param entitykey Entity key of the data point trend to be updated
          * @param timestamp Time when the data point changed value
          * @param status Status of data point
		  *
		  */
        void updateRealTimePlot(unsigned long entitykey, time_t timestamp, double value, unsigned long status);

        /**
		  * refreshGraph
		  *
		  * Refreshes the graph and moves the graph to include the current time. Each
          * trend has a new point added for the current time which is the last value plotted.
		  *
		  * @param timestamp Time of the refresh
          *
		  */
        void refreshGraph(time_t timestamp);

        /**
		  * processUpdateCfgEvent
		  *
		  * Processes an online configuration update of type Update
		  *
		  * @param entityData Pointer to IEntityData object of the updated entity
          *
		  */
        void processUpdateCfgEvent(IEntityData* entityData);

        /**
		  * processCreateCfgEvent
		  *
		  * Processes an online configuration update of type Create
		  *
		  * @param entityData Pointer to IEntityData object of the updated entity
          *
		  */
        void processCreateCfgEvent(unsigned long entitykey, IEntityData* entityData);

        /**
		  * processDeleteCfgEvent
		  *
		  * Processes an online configuration update of type Delete
		  *
		  * @param entityData Pointer to IEntityData object of the updated entity
          *
		  */
        void processDeleteCfgEvent(unsigned long entitykey);

        /**
		  * processEntityParameterUpdates
		  *
		  *
		  *
		  * @param
          *
		  */
        //void processEntityParameterUpdates(const std::string& parameter);

        /**
		  * serverDown
		  *
		  * Called when the server is down. Disables appropriate items and displays a
          * message to indicate the server is down.
		  *
		  */
        void serverDown();

        /**
		  * serverUp
		  *
		  * Called when the server is up. Enabled appropriate items and displays a
          * message to indicate the server is up.
          *
		  */
        void serverUp();

        /**
		  * saveUserSettings
		  *
		  * Saves the user settings. Currently, these are the data points that are currently
          * displayed in the graph for the current operator.
		  *
		  * @param userSettings User settings to be saves
          *
		  */
        void saveUserSettings(std::map<std::string, std::string>& userSettings);

        /**
		  * loadUserSettings
		  *
		  * Loads the saves user settings
		  *
		  * @param settingMgr SettingsMgr object that stores and managers the user settings
          *
		  */
        void loadUserSettings(TA_Bus::SettingsMgr& settingMgr);

        /**
		  * restoreUserSettings
		  *
		  * Displays the loaded user settings in the GUI.
		  *
		  */
        void restoreUserSettings();

    protected:
        // Generated message map functions
        //{{AFX_MSG(CHistoryViewerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnApply();
        afx_msg void OnNowCheck();
        afx_msg void OnToggleNowCheck();
        afx_msg void OnViewCurrentTime();
        afx_msg void OnDataPointAdd();
        afx_msg void OnDataPointDelete();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnZoomGraph();
        afx_msg void OnUndoZoomGraph();
        afx_msg void OnScrollGraph();
        afx_msg void OnPrintGraph();
        afx_msg void OnExport();
        afx_msg void OnClickSeriesPoint(long SeriesIndex, long ValueIndex, WPARAM wParam, LPARAM lParam, long X, long Y);
        afx_msg void OnClose();
        afx_msg void OnOK();
        afx_msg void OnAppAbout();
        afx_msg void OnHelp();
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam = 0L);
        afx_msg LRESULT OnPopMessageString(WPARAM wParam, LPARAM lParam);
        afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
        afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
        afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
        afx_msg void OnViewModify();
        afx_msg void OnDestroy();
        afx_msg void OnOnBeforeDrawAxes();
        afx_msg void OnMarkTipToolGetText(long Tool, BSTR FAR* Text);
	    afx_msg void OnGetSeriesMark(long SeriesIndex, long ValueIndex, BSTR FAR* MarkText);
	afx_msg void OnClickBackgroundTchart(long Button, long Shift, long X, long Y);
	afx_msg void OnClickLegend(long Button, long Shift, long X, long Y);
        DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
        // Loads new samples from the database if the same history type has been updated.
        afx_msg LRESULT OnHistoryUpdate( WPARAM wParam, LPARAM lParam );
        afx_msg LRESULT OnLoadingComplete( WPARAM wParam, LPARAM lParam );
        afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnStatusBarCheck();
        afx_msg void OnToolBarCheck();
        afx_msg void OnUpdateStatusBarMenu(CCmdUI* pCmdUI);
        afx_msg void OnUpdateToolBarMenu(CCmdUI* pCmdUI);
        DECLARE_MESSAGE_MAP()

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CHistoryViewerDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
        //}}AFX_VIRTUAL

        BOOL DestroyWindow();

    private:

        void SetStartDateTime();
        void SetStartDateTime(COleDateTime startDateTime);
        void SetEndDateTime(const COleDateTime& endDateTime);
        void LoadHistoryConfig();
        void LoadDataPoints();
        void ParseParameters();
        void SetupGraph();
        void AddToGraph(std::vector <unsigned long> &dataPointKeyList);
        void DeleteFromGraph(std::vector <unsigned long> &dataPointKeyList);
        void AdjustTimeLabelFormat();
        void TurnOffLiveUpdate();
        void SetValuesFromGraph();
        void SetGraphFromValues();
        long GetNumSamples(const long rangeSeconds);
        COleDateTime GetEndTime();
        void LoadAllData();
        void AdjustSeriesNumbers(const long &seriesNum);
        void AdjustCustomAxes(long axisIndex, long seriesNum);
        void UnzoomNoReset();
        void SetStatusLiveMode();
        void ResizeGraph(int cx, int cy);
        void ResizeGraph();
        void moveGraph(time_t timestamp);
        void SetMenuItemCheck(CString menuString, const UINT& itemID, BOOL bChecked = true);
        void ApplyChanges();
        void displayCurrentTrendDataWarning();
        int getDataTypeDisplayed(int type);
        void calculateRangeSeconds();
        std::string ConvertIntToString(int integer);
        std::string getOperatorName(unsigned long operatorId);
        void sendAuditMessage(const TA_Core::MessageType& messageType, const TA_Core::DescriptionParameters& descriptionParams);
        bool isDataPointAlreadyDisplayed(unsigned long entityKey);

        // Create the toolbar
        // @exception This throws a ToolbarCreationError if it cannot create the toolbar.
        //            and a ButtonNotFound if it cannot find the button.
        void CreateToolbar();
        class ToolbarCreationError {};

        // Adds text the a toolbar button.
        // @exception This throws a ButtonNotFound if it cannot find the button.
        void AddTextToButton( UINT buttonId, CString buttonLabel );
        class ButtonNotFound {};

        // FindMenuItem() will find a menu item from the specified menu and returns
        // its position (0-based) in the specified menu.  It returns -1 if no menu
        // item is found.
        //
        // @exception This throws a ItemNotFoundException if it cannot find the item
        int FindMenuItem( CMenu* Menu, const CString& menuString );
        class ItemNotFoundException{};

    public:

        /**
		  * getCurrentGraphCount
		  *
		  * Get the number of trends displayed in the graph
		  *
		  * @return Number of displayed trends
          *
		  */
        int getCurrentGraphCount()
        {
            return m_GraphDataPointList.size();
        }


    private:

        long m_RangeDaysTemp; // Range Days
        std::vector <GraphDataPoint> m_GraphDataPointList;
        std::vector <RealTimeDataUpdateSubscriber*> m_realTimeDataUpdateSubscriberList;
        GraphRefreshThread* m_graphRefreshThread;
        DataPointOnlineUpdateSubscriber* m_dataPointOnlineUpdateSubscriber;
        TA_Core::AuditMessageSender* m_auditSender;
        std::string m_operatorId;
        std::string m_operatorName;
        bool m_UnzoomReset; // Indicates whether to reset parameters when unzooming
        CDlgStatusBar m_wndStatusBar;
        UINT m_nIDTracking;
        UINT m_nIDLastMessage;
        CDlgToolBar     m_toolBar;
        bool m_serverDown;
        std::string m_sessionId;
        static TA_Core::ReEntrantThreadLockable m_lock;
        static TA_Core::ReEntrantThreadLockable m_toolbarLock;
        time_t m_lastTimeStamp;
        std::string m_dataPointsSettingStr;
        long m_customAxisDisplayed;
        long m_lastSeriesClicked;
        long m_boolCustomAxisIndex;
        long m_rightMarginInitial;

        static const std::string DATAPOINTS_SETTING;

        typedef TA_Core::NamedObject<TA_Bus::DataPointCorbaDef,
 		            TA_Bus::DataPointCorbaDef_ptr,
                    TA_Bus::DataPointCorbaDef_var> DataPointNamedObj;

        DataPointLoaderThread* m_dataPointLoaderThread;

    protected:

        HICON m_hIcon;

    public:

        // Dialog Data
        //{{AFX_DATA(CHistoryViewerDlg)
        enum { IDD = IDD_HISTORYVIEWER_DIALOG };
        CTChart	m_ChartCtrl;
        //}}AFX_DATA
        BOOL m_NowCheck;
        long m_RangeSeconds;
        int	 m_HistoryType;
        COleDateTime m_StartDateTime;
        COleDateTime m_rangeTime;
        int m_rangeDays;

        typedef struct
        {
            time_t timestamp;
            unsigned long status;
            double value;
        } DataPointInfo;

    };

} // TA_App


#endif // !defined(AFX_HISTORYVIEWERDLG_H__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_)





