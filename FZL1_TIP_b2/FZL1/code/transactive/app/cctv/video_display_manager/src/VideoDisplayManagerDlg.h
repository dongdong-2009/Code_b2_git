/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * TODO: <description>
  */

// VideoDisplayManagerDlg.h : header file
//

#if !defined(AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
#define AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094A9__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/EditNoFocus.h"
#include "app/cctv/video_display_manager/src/PTZButton.h"
#include "app/cctv/video_display_manager/src/Label.h"
#include "app/cctv/video_display_manager/src/ToolTipLabel.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "app/cctv/video_display_manager/src/TransparentImage.h"
#include "app/cctv/video_display_manager/src/VideoSourcesPropertySheet.h"
#include "app/cctv/video_display_manager/src/VideoSourcesTreeCtrl.h"
#include "app/cctv/video_display_manager/src/VideoSourcesListBox.h"
#include "app/cctv/video_display_manager/src/SchematicList.h"
#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_App
{

    class Camera;
    class Sequence;
    class Monitor;
    class DisplayItem;
    class VideoInput;
    class CDlgVideoPlay;
    class VideoDisplayManagerPresenter;
    class SequenceEditorDlg;

    class VideoDisplayManagerDlg : public IGFVideoSwitchDisplayView,
								   public TA_Base_Core::RunParamUser
    {
            // Construction

        public:

            VideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, VideoDisplayManagerPresenter& presenter);  // standard constructor

            virtual ~VideoDisplayManagerDlg();

            void setDragImage(CImageList* dragImage);

            void setDragAdjust(CPoint& dragAdjust)
            {
                m_dragAdjustPoint = dragAdjust;
            }

            // Dialog Data
            //{{AFX_DATA(VideoDisplayManagerDlg)
            enum { IDD = IDD_VIDEODISPLAYMANAGER_DIALOG };

			CPTZButton    m_zoomInBtn;
			CPTZButton    m_zoomOutBtn;

			CPTZButton    m_ptLeftUpBtn;
			CPTZButton    m_ptRightUpBtn;
			CPTZButton    m_ptLeftDownBtn;
			CPTZButton    m_ptRightDownBtn;

			CPTZButton    m_tiltUpBtn; //up
			CPTZButton    m_tiltDownBtn; //down

			CPTZButton    m_panRightBtn; //right
			CPTZButton    m_panLeftBtn;  //left

            CComboBox m_stationListForSEQ; // The combo box to hold all the stations
            //    CComboBox m_stationListForCamera;
            VideoSourcesTreeCtrl    m_sequenceTreeCtrl;
            VideoSourcesListBox    m_sequencesListBox;
            CComboBox m_stationListForCamera;
            CButton                m_editSequenceBtn;
            //   CEditNoFocus    m_detailsBox;
            CStatic m_wallMonitors[NUMBER_WALL_MONITORS];
			CStatic   m_quad1;
			CStatic   m_quad2;
			CStatic   m_quad3;
			CStatic   m_quad4;
            CStatic   m_localMonitor;

			//monitor index
			CLabel     m_quad1_Index;
			CLabel     m_quad2_Index;
			CLabel     m_quad3_Index;
			CLabel     m_quad4_Index;

            //text on the monitor
            ToolTipLabel m_wallMonitorsText[NUMBER_WALL_MONITORS];
			CLabel     m_quad1_Text;
			CLabel     m_quad2_Text;
			CLabel     m_quad3_Text;
			CLabel     m_quad4_Text;
            CLabel     m_localMonitorText;

            CLabel m_wallMonitorsIndex[NUMBER_WALL_MONITORS];
            CBitmap m_wallMonitorBitmap;
            CBitmap m_wallMonitorHighLightBitmap;

            // Resources for single wall monitors
            CStatic m_singleWallMonitors[NUMBER_SINGLE_WALL_MONITORS];
            ToolTipLabel m_singleWallMonitorsText[NUMBER_SINGLE_WALL_MONITORS];
            CLabel m_singleWallMonitorsIndex[NUMBER_SINGLE_WALL_MONITORS];
            CBitmap m_singleWallMonitorBitmap;
            CBitmap m_singleWallMonitorHighLightBitmap;
            //}}AFX_DATA

            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(VideoDisplayManagerDlg)

        public:
            virtual BOOL DestroyWindow();
            virtual BOOL PreTranslateMessage(MSG* pMsg);

        protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            //}}AFX_VIRTUAL

        public :

            //iview
            void DisplaySequences(std::vector<VideoInput*> & sequences) ;
            void DisplaySequence(Sequence* sequence);
            void GetSchematicCRET(CRect& markerRect);
            unsigned long GetStationIdForGwax() ;
            HWND GetWndHandle();
            unsigned long GetSelectedSequenceId();

            unsigned long GetSelectedStation();

            void EnableEditSeqButton(bool enabled);

            MonitorDisplayItem* getMonitorDisplayitem(int index , Monitor* monitor);

            void beginDragImage();

            void paintStandControl(MonitorWnd * item);


            void paintHightedControl(MonitorWnd * item);

            void endDrage();

            void setDragIcon()
            {
                SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CURSOR));
            }

            void handleDragImageOnMouseMove(BOOL begin, CPoint& point);

            void loadSequenceSubMenu(CPoint& point, Monitor * monitor , Sequence* seq, bool bIsLocalMonitor);
            void loadCameraSubMenu(CPoint& point , Monitor * monitor, bool bIsLocalMonitor);

            void displayErrorMessage(CString& message);
			virtual void enablePTZButton(BOOL enabled);

            void displayStaionCameraDiagram(std::string& fullFileName);

            void assignMonitor(MonitorDisplayItem * item);

            void changeWallMontior( MonitorDisplayItem * monitor, bool split);

            void enableStationSelection(BOOL enable);
            void setSelectedStation(unsigned int index);
            void UpdateSequenceTreeCtrl(Sequence* sequence);

            //void refreshIndexLable(int index, bool isQuad);

            // Implementation

        protected:
            HICON m_hIcon;

            // Generated message map functions
            //{{AFX_MSG(VideoDisplayManagerDlg)
            virtual BOOL OnInitDialog();
            afx_msg void OnPaint();
            afx_msg HCURSOR OnQueryDragIcon();
            afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
            afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
            afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
            // afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
            afx_msg void OnAppAbout();
            afx_msg void OnHelpDisplaymanagerhelp();
            afx_msg void OnClose();
            afx_msg void OnDestroy();
            afx_msg LRESULT onMonitorUpdated(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitCameras(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitSequences(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitStages(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitMonitors(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitDisplayManager(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitStations(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT onHandleRunParam(WPARAM wParam, LPARAM lParam);
            afx_msg void onSelectionChangeStationList();
            afx_msg void onSelectionChangeStationDiagramList();
            afx_msg void onEditSequence();
            afx_msg void OnTimer(UINT nEventID);

            afx_msg void OnLevelSelChange();
            afx_msg LRESULT onRefreshSchematic(WPARAM wParam, LPARAM lParam);
            afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
            //}}AFX_MSG
            DECLARE_MESSAGE_MAP()
        private:
			void initPtzBtnsActionHandler();
            // Helper Functions

            /**
              * configureGUILayout
              *
              * Does some dodgy stuff - dynamically hides and resizes based
              * on the location type.
              * I cant think of a better way around this for now.
              *
              * @param locationType
              */
            //void configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType);


            void AddMonitorToConsole(int consoleMonitorId, int labelConsoleMonitorId, int textConsoleMonitorId, int offset);
            void AddSequenceControlToConsole(int consolePreviousElementButton, int consolePlayCheck, int consoleNextElementButton, int consolePauseCheck, int consolePlaybackLabel, int offset);

            /**
              * setupMonitors
              *
              * Sets up the monitor icons, labels, and text overlays
              *
              * @param locationType
              */
            void setupMonitors(TA_Base_Core::ILocation::ELocationType locationType);

			/**
              * LoadButtonIcons
              *
              * loads the icons for the sequence control
              * buttons.
              */
            void LoadButtonIcons();


            /**
              * UpdateDialog
              *
              * Sends a WM_PAINT message to the Vidiwall dialog.
              */
            void updateDialog();


            /**
              * InvalidateControlLabels
              *
              * Invalidates the control labels associated with
              * the previous and currently highlighted monitors.
              */
            void invalidateControlLabels();


            void endDragging();

            /**
              * getMonitor
              *
              * Get the monitor object associated with a given control.
              *
              * @param controlId    The control (CStatic) ID of a monitor
              *
              * @return     The monitor object (NULL if none)
              */
            Monitor* getMonitor(int controlId);
            // maps the other way around
            int getControlId(std::string monitorName);


            /**
              * getSelectedVideoSource
              *
              * Gets the video source that is selected on one
              * of the pages. Used for drag and drop.
              *
              * @return The selected source, NULL if none.
              */
            VideoInput* getSelectedVideoSource();

            /**
              * onRunParamChange
              *
              * Called when a runtime parameter changes.
              * Especially important because this is the routine through which the schematics
              * will request a camera to monitor change if the DisplayManager is already running.
              *
              * @param name    The name of the RunParam that has changed
              * @param value   The new value of this RunParam
              */
            virtual void onRunParamChange(const std::string& name, const std::string& value);

            void clearMonitor(Monitor* monitor);
            void loadSequenceController(Sequence* sequence);

            void createCctvProxy();

            MonitorWnd* createMonitorWnd(CStatic* wnd, HBITMAP standard, HBITMAP highlight);

            // Private Data Members
            BOOL m_mouseTracking;                        // TRUE if mouse tracking & dragging.
            std::vector<  boost::shared_ptr< CStatic > > m_monitors;            // For the monitors.
            std::vector<  boost::shared_ptr< CTransparentImage > > m_recordingDevices;    // For the recording devices.
            std::vector<  boost::shared_ptr< CLabel > > m_labels;            // For the transparent labels.

            short m_hilitedControlID;                    // Resource ID of hilited control.
            BOOL m_hilitingPerformed;
            short m_prvHilitedControlID;                // Resource ID of previous hilited control.
            BOOL m_normalisationPerformed;
            BOOL m_restoring;
            BOOL m_contextActive;
            CImageList* m_dragImage;
            CPoint m_dragAdjustPoint;
            SequenceEditorDlg* m_pSequenceEditorDlg;

            // check box icons.
            HICON m_previousElementIcon;
            HICON m_nextElementIcon;
            HICON m_playIcon;
            HICON m_pauseIcon;

            // these map monitor names to resource id's
            // and resource id's to monitor names
            std::map<std::string, int> m_monitorNameToResourceIdMap;
            std::map<int, std::string> m_resourceIdToMonitorNameMap;

            bool m_isAssignedBySchematic;

            VideoDisplayManagerPresenter& m_presenter;

			std::vector<CPTZButton*> m_ptzButtons;

            SchematicList m_schematicLevesList;

            std::map<CStatic*, MonitorWnd*> m_wndToMonitorWndMap;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
