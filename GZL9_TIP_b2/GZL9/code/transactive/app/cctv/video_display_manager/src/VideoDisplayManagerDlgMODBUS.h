/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h $
  * @author:  Robert Young
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/11/18 21:38:42 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * TODO: <description>
  */

// VideoDisplayManagerDlg.h : header file
//

#pragma once

#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/CamerasPage.h"
#include "app/cctv/video_display_manager/src/EditNoFocus.h"
#include "app/cctv/video_display_manager/src/Label.h"
#include "app/cctv/video_display_manager/src/SequencesPage.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/TransparentImage.h"
#include "app/cctv/video_display_manager/src/VideoSourcesPropertySheet.h"
#include "app/cctv/video_display_manager/src/VideoSourcesTreeCtrl.h"
#include "app/cctv/video_display_manager/src/VideoSourcesListBox.h"
#include "app/cctv/video_display_manager/src/SchematicList.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "core/utilities/src/RunParams.h"
#include <boost/array.hpp>


// cant think of a better way
#define MONITOR_TEXT_DIFF (IDC_TEXT_WALL_MONITOR_1 - IDC_WALL_MONITOR_1)
#define MONITOR_LABEL_DIFF (IDC_LABEL_WALL_MONITOR_1 - IDC_WALL_MONITOR_1)
#define CAMERA_TEXT_LENGTH 14

namespace TA_IRS_App
{

class QuadControllerDlg;
class Camera;
class Quad;
class Sequence;
class Monitor;
class DisplayItem;
class VideoInput;
class DlgVideoPlayImos;
class VideoDisplayManagerPresenter;
class SequenceEditorDlg;
class IMonitorAssigner;

class VideoDisplayManagerDlgMODBUS : public TA_Base_Bus::TransActiveDialog,
            public TA_Base_Core::RunParamUser,
            public IGFVideoSwitchDisplayView
{
    // Construction

public:

    VideoDisplayManagerDlgMODBUS(TA_Base_Bus::IGUIAccess& controlClass, VideoDisplayManagerPresenter& presenter);  // standard constructor

    ~VideoDisplayManagerDlgMODBUS();

    void setDragImage(CImageList* dragImage);

    void setDragAdjust(CPoint& dragAdjust)
    {
        m_dragAdjustPoint = dragAdjust;
    }

    // Dialog Data
    //{{AFX_DATA(VideoDisplayManagerDlg)
    enum { IDD = IDD_VIDEODISPLAYMANAGER_DIALOG_MODBUS };
    CComboBox m_stationListForSEQ; // The combo box to hold all the stations
    //    CComboBox m_stationListForCamera;
    VideoSourcesTreeCtrl    m_sequenceTreeCtrl;
    VideoSourcesListBox    m_sequencesListBox;
    CComboBox m_stationListForCamera;
    CButton                m_editSequenceBtn;
    //   CEditNoFocus    m_detailsBox;


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

    void paintStandControl(MonitorDisplayItem * item);


    void paintHightedControl(MonitorDisplayItem * item);

    void endDrage();

    void setDragIcon()
    {
        SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CURSOR));
    }

    void handleDragImageOnMouseMove(BOOL begin, CPoint& point);

    void loadSequenceSubMenu(CPoint& point, Monitor * monitor , Sequence* seq, bool bIsLocalMonitor);
    void loadCameraSubMenu(CPoint& point , Monitor * monitor, bool bIsLocalMonitor);

    void displayErrorMessage(CString& message);

    void enablePTZButton(BOOL enabled)
    {
    }

    virtual void highlightedSelectedMonitor(CWnd * monitorwnd);


    void normalizeMonitor(CWnd * monitorwnd);


    void displayStaionCameraDiagram(std::string& fullFileName);

    void updateMonitor(MonitorDisplayItem * item);

    void enableStationSelection(BOOL enable);
    void setSelectedStation(unsigned int index);
    void UpdateSequenceTreeCtrl(Sequence* sequence);

    void refreshIndexLable(int index);

    // Implementation

protected:
    HICON m_hIcon;
    CamerasPage*        m_camerasPage;
    SequencesPage*    m_sequencesPage;

    // Generated message map functions
    //{{AFX_MSG(VideoDisplayManagerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
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
    afx_msg void OnMove(int x, int y);
    afx_msg LRESULT onMonitorUpdated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnInitCameras(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnInitQuads(WPARAM wParam, LPARAM lParam);
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

    afx_msg LRESULT onRefreshSchematic(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLevelSelChange();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    // TD 16824
    virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
    // TD 16824

    //iview



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
    //void configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType);


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
    void setupMonitors();

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
    void loadSequenceController(Sequence* sequence);

    /**
      * sendCarrierOff
      *
      * Does the actual carrier off call to a train
      *
      * @param trainId  the train to carrier off
      */
    void sendCarrierOff(unsigned short trainId);


    /**
      * sendSequenceCommand
      *
      * Send a sequence command.
      * Will assert if a monitor with a sequence assigned is not selected
      *
      * @param playbackCmd
      */
    void sendSequenceCommand(TA_IRS_App::SwitchAgentCommunicator::ESequencePlaybackCommandType playbackCmd);

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
    //refresh index lable
    //void refreshIndexLable();

    void setupGUIMonitors();

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
    // TODO. delete quad control
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


    // TD16059
    bool m_isAssignedBySchematic;
    // TD16059

    // for activating delocalised trains
    unsigned long m_overriddenLocation;

    VideoDisplayManagerPresenter& m_presenter;

    IMonitorAssigner* m_monitorAssigner;
    boost::array<CStatic*, 25> m_guiMonitors;
    boost::array<CLabel*, 25> m_guiMonitorsDesc;

    SchematicList m_schematicLevesList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App
