/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h $
  * @author:  Robert Young
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2009/04/08 14:05:16 $
  * Last modified by:  $Author: builder $
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

#include "app/cctv/video_display_manager/src/CamerasPage.h"
#include "app/cctv/video_display_manager/src/EditNoFocus.h"
#include "app/cctv/video_display_manager/src/Label.h"
#include "app/cctv/video_display_manager/src/QuadsPage.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/SequencesPage.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/TrainsPage.h"
#include "app/cctv/video_display_manager/src/TransparentImage.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoSourcesPropertySheet.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainCctvUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"

#include "core/utilities/src/RunParams.h"

#include <vector>


// cant think of a better way
#define MONITOR_TEXT_DIFF	(IDC_TEXT_WALL_MONITOR_1 - IDC_WALL_MONITOR_1)
#define MONITOR_LABEL_DIFF	(IDC_LABEL_WALL_MONITOR_1 - IDC_WALL_MONITOR_1)
#define CAMERA_TEXT_LENGTH 14
const char RPARAM_ENTITY[]	= "Entity";
const char RPARAM_DISPLAYMONITOR[]	= "DisplayMonitor";

namespace TA_IRS_App
{

        class QuadControllerDlg;

        class BvsStage;
        class Camera;
        class Quad;
        class Sequence;
        class Monitor;

        class DisplayItem;
        class VideoInput;
        //class VideoOutput;

        class VideoDisplayManagerDlg :  public TA_Base_Bus::TransActiveDialog,
                                        public TA_Base_Core::RunParamUser
        {
        // Construction
        public:

            VideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass); // standard constructor

            ~VideoDisplayManagerDlg();

            void setDragImage(CImageList* dragImage);

            void setDragAdjust(CPoint& dragAdjust) { m_dragAdjustPoint = dragAdjust; }
            //void moveChild(RECT&);

            /**
              * getTrainsPage
              *
              * gets a pointer to the trains page for getting train information
              *
              * @return a pointer, may be null.
              */
            inline TrainsPage* getTrainsPage() { return m_trainsPage;}


            // Dialog Data
            //{{AFX_DATA(VideoDisplayManagerDlg)
            enum { IDD = IDD_VIDEODISPLAYMANAGER_DIALOG };
            CButton    m_consolePreviousElement;
            CButton    m_consolePlay;
            CButton    m_consolePause;
            CButton    m_consoleNextElement;
            CButton    m_commonPreviousElement;
            CButton    m_commonPlay;
            CButton    m_commonPause;
            CButton    m_commonNextElement;
            CEditNoFocus    m_detailsBox;
            //}}AFX_DATA

            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(VideoDisplayManagerDlg)
            public:
            virtual BOOL DestroyWindow();
            virtual BOOL PreTranslateMessage(MSG* pMsg);
            protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            //}}AFX_VIRTUAL

        // Implementation
        protected:
            HICON m_hIcon;
            CamerasPage*        m_camerasPage;
            QuadsPage*        m_quadsPage;
            SequencesPage*    m_sequencesPage;
            TrainsPage*    m_trainsPage;
            VideoSourcesPropertySheet* m_videoSourcesPropertySheet;

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
            afx_msg void OnAppAbout();
            afx_msg void OnHelpDisplaymanagerhelp();
            afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
            afx_msg void OnClose();
            afx_msg void OnCommonNextelementButton();
            afx_msg void OnCommonPauseCheck();
            afx_msg void OnCommonPlayCheck();
            afx_msg void OnCommonPreviouselementButton();
            afx_msg void OnConsoleNextelementButton();
            afx_msg void OnConsolePauseCheck();
            afx_msg void OnConsolePlayCheck();
            afx_msg void OnConsolePreviouselementButton();
            afx_msg void OnDestroy();
            afx_msg LRESULT onMonitorUpdated(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitCameras( WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitQuads( WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitSequences( WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitStages( WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitMonitors( WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnInitDisplayManager( WPARAM wParam, LPARAM lParam);
            //}}AFX_MSG
            DECLARE_MESSAGE_MAP()
            // TD 16824
            virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
            // TD 16824

        private:
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
            void configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType);


			// zhiqiang++
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

            /**
              * IsOverMonitor
              *
              * TRUE if the mouse is over a monitor.
              */
            BOOL isOverMonitor( CPoint& pt, bool doCheckForSequence = false );

            /**
              * PaintStandardControl
              *
              * Paints the non-hilited bitmap in the control that lost focus.
              */
            void paintStandardControl();

          /**
            * PaintHilitedControl
            *
            * Paints the hilited bitmap in the control that lost focus.
            */
            void paintHilitedControl();

            void updateDetailsBox();

            void endDragging();

            void buildTrainContextMenu(CMenu* menu, std::vector<VideoOutput::TrainCamera> stages);

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
            VideoInput* getSelectedVideoSource( bool allowDelocalisedTrains = false );

            /**
              * getSelectedTrainId
              *
              * This is only valid if the trains page is active.
              *
              * @return the selected train id
              */
            unsigned short getSelectedTrainId();

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
            void loadQuadController(Quad* quad);
            void loadTrainController(BvsStage* stage);
            void loadTrainController(unsigned short trainId);
            void loadSequenceController(Sequence* sequence);

            /**
              * deactivateTrain
              *
              * If a train is assigned to a monitor - deactivate it
              * TODO - if other monitors in the system are looking at it
              * then prompt whether to deactivate it locally - or on all monitors.
              *
              * @param source   the stage
              * @param monitor  the monitor that is being cleared - if applicable
              *
              */
            void deactivateTrain(BvsStage* source, Monitor* monitor = NULL);
            

            /**
              * sendCarrierOff
              *
              * Does the actual carrier off call to a train
              *
              * @param trainId  the train to carrier off
              */
            void sendCarrierOff( unsigned short trainId );


            /**
              * sendSequenceCommand
              *
              * Send a sequence command.
              * Will assert if a monitor with a sequence assigned is not selected
              *
              * @param playbackCmd
              */
            void sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::ESequencePlaybackCommandType playbackCmd );

            /**
              * enableSequencePlaybackButtons
              *
              * enables/disables the sequence control buttons
              *
              * @param selectedMonitor  the selected monitor - NULL if none
              */
            void enableSequencePlaybackButtons(Monitor* selectedMonitor);

            void createCctvProxy();

			//resize the window
			void ModifyWindowPos();

            // Private Data Members
            BOOL m_mouseTracking;                        // TRUE if mouse tracking & dragging.
            std::vector< boost::shared_ptr< CStatic > > m_monitors;            // For the monitors.
            std::vector< boost::shared_ptr< CTransparentImage > > m_recordingDevices;    // For the recording devices.
            std::vector< boost::shared_ptr< CLabel > > m_labels;            // For the transparent labels.
            short m_hilitedControlID;                    // Resource ID of hilited control.
            BOOL m_hilitingPerformed;
            short m_prvHilitedControlID;                // Resource ID of previous hilited control.
            BOOL m_normalisationPerformed;
            BOOL m_restoring;
            BOOL m_contextActive;
            CImageList* m_dragImage;
            CPoint m_dragAdjustPoint;
            QuadControllerDlg* m_pQuadControllerDlg;
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

            // update these current runparam camera and monitor strings in onRunParamChange()
            // and when both are filled then execute the camera-to-monitor switch
            std::string m_paramCameraAddress;
            std::string m_paramMonitorName;

            // TD16059
            bool m_isLaunched;
            bool m_isAssignedBySchematic;
            // TD16059

            // for activating delocalised trains
            unsigned long m_overriddenLocation;

            TA_IRS_Bus::TrainCctvCorbaProxyPtr m_trainCctvCorbaProxy;

			//for third console monitor status
			bool  m_thirdMonitorInservice;
        };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
