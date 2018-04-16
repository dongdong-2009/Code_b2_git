/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.cpp $
* @author:  Robert Young
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/10/01 16:36:38 $
* Last modified by:  $Author: grace.koh $
*
* TODO: <description>
*/

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "app/cctv/video_display_manager/src/stdafx.h"
#include "StationDlgVideoPlayImos.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/QuadControllerDlg.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/StationVideoDisplayManagerDlgIMOS.h"
#include "app/cctv/video_display_manager/src/LocationSelectionDialog.h"
#include "app/cctv/video_display_manager/src/DlgVideoPlayImos.h"
#include "app/cctv/video_display_manager/src/Configuration.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "CCTVCommon.h"
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78
#define MINVALUE    2400
#define NUMSCREENS    3

using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
StationVideoDisplayManagerDlgIMOS::StationVideoDisplayManagerDlgIMOS(TA_Base_Bus::IGUIAccess& controlClass, VideoDisplayManagerPresenter& presenter)
        : TransActiveDialog(controlClass, StationVideoDisplayManagerDlgIMOS::IDD, NULL),
        m_mouseTracking(FALSE),
        m_dragImage(NULL),
        m_dragAdjustPoint(0, 0),
        m_pSequenceEditorDlg(0),
        m_isAssignedBySchematic(false),
        m_overriddenLocation(0),
        m_videoDlg(NULL),
        m_panLeftBtn(G_PAN_LEFT),
        m_panRightBtn(G_PAN_RIGHT),
        m_tiltUpBtn(G_TILT_UP),
        m_tiltDownBtn(G_TILT_DOWN),
        m_ptLeftUpBtn(G_PT_LEFTUP),
        m_ptLeftDownBtn(G_PT_LEFTDOWN),
        m_ptRightUpBtn(G_PT_RIGHTUP),
        m_ptRightDownBtn(G_PT_RIGHTDOWN),
        m_zoomInBtn(G_ZOOM_IN),
        m_zoomOutBtn(G_ZOOM_OUT),
        m_presenter(presenter)
{
    FUNCTION_ENTRY("VideoDisplayManagerDlg");
    m_sequenceTreeCtrl.setVideoDisplayManagerPresenter(&m_presenter);
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    FUNCTION_EXIT;
}


StationVideoDisplayManagerDlgIMOS::~StationVideoDisplayManagerDlgIMOS()
{
    FUNCTION_ENTRY("~VideoDisplayManagerDlg");
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::setDragImage(CImageList* dragImage)
{
    FUNCTION_ENTRY("setDragImage");
    if (m_dragImage != NULL)
    {
        delete m_dragImage;
        m_dragImage = NULL;
    }
    m_dragImage = dragImage;
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::DoDataExchange(CDataExchange* pDX)
{
    TransActiveDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(VideoDisplayManagerDlg)
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_STATION_SEQ_STATION_COMB, m_stationListForSEQ);
    DDX_Control(pDX, IDC_STATION_SEQENCE_TREE, m_sequenceTreeCtrl);
    DDX_Control(pDX, IDC_STATION_SEQ_LST, m_sequencesListBox);
    DDX_Control(pDX, IDC_STATION_SEL_STATION_COMB, m_stationListForCamera);
    DDX_Control(pDX, IDC_STATION_EDIT_SEQ_BTN, m_editSequenceBtn);
    DDX_Control(pDX, IDC_QUAD1, m_quad1);
    DDX_Control(pDX, IDC_QUAD2, m_quad2);
    DDX_Control(pDX, IDC_QUAD3, m_quad3);
    DDX_Control(pDX, IDC_QUAD4, m_quad4);
    DDX_Control(pDX, IDC_LOCAL_MONITOR, m_localMonitor);
    //text
    DDX_Control(pDX, IDC_QUAD1_TEXT, m_quad1_Text);
    DDX_Control(pDX, IDC_QUAD2_TEXT, m_quad2_Text);
    DDX_Control(pDX, IDC_QUAD3_TEXT, m_quad3_Text);
    DDX_Control(pDX, IDC_QUAD4_TEXT, m_quad4_Text);
    DDX_Control(pDX, IDC_LOCAL_MONITOR_TEXT, m_localMonitorText);
    //index label
    DDX_Control(pDX, IDC_QUAD1_INDEX, m_quad1_Index);
    DDX_Control(pDX, IDC_QUAD2_INDEX, m_quad2_Index);
    DDX_Control(pDX, IDC_QUAD3_INDEX, m_quad3_Index);
    DDX_Control(pDX, IDC_QUAD4_INDEX, m_quad4_Index);
    //pan button
    DDX_Control(pDX, IDC_PAN_LEFT_BTN, m_panLeftBtn);
    DDX_Control(pDX, IDC_PAN_RIGHT_BTN, m_panRightBtn);
    DDX_Control(pDX, IDC_TILT_UP_BTN, m_tiltUpBtn);
    DDX_Control(pDX, IDC_TILT_DOWN_BTN, m_tiltDownBtn);
    DDX_Control(pDX, IDC_PT_LEFT_UP_BTN, m_ptLeftUpBtn);
    DDX_Control(pDX, IDC_PT_RIGHT_UP_BTN, m_ptRightUpBtn);
    DDX_Control(pDX, IDC_PT_LEFT_DOWN_BTN, m_ptLeftDownBtn);
    DDX_Control(pDX, IDC_PT_RIGHT_DOWN_BTN, m_ptRightDownBtn);
    DDX_Control(pDX, IDC_ZOOM_OUT_BTN, m_zoomOutBtn);
    DDX_Control(pDX, IDC_ZOOM_IN_BTN, m_zoomInBtn);
    DDX_Control(pDX, IDC_SCHEMATIC_SEL_LIST, m_schematicLevelsList);
}


BEGIN_MESSAGE_MAP(StationVideoDisplayManagerDlgIMOS, TransActiveDialog)
    //{{AFX_MSG_MAP(VideoDisplayManagerDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_APP_HELP, OnHelpDisplaymanagerhelp)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_MONITOR_UPDATED, onMonitorUpdated)
    ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)
    ON_MESSAGE(TA_INIT_CAMERAS, OnInitCameras)
    ON_MESSAGE(TA_INIT_QUADS, OnInitQuads)
    ON_MESSAGE(TA_INIT_SEQUENCES, OnInitSequences)
    ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
    ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
    ON_MESSAGE(TA_RUN_PARAM , onHandleRunParam)
    ON_MESSAGE(TA_INIT_STAGES, OnInitStages)
    ON_MESSAGE(TA_INIT_STATION, OnInitStations)
    ON_CBN_SELCHANGE(IDC_STATION_SEQ_STATION_COMB, onSelectionChangeStationList)
    ON_BN_CLICKED(IDC_STATION_EDIT_SEQ_BTN, onEditSequence)
    ON_WM_TIMER()
    ON_WM_MOVE()
    ON_MESSAGE(REFRESH_SCHEMATIC_MESSAGE, onRefreshSchematic)
    ON_LBN_SELCHANGE(IDC_SCHEMATIC_SEL_LIST, OnLevelSelChange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VideoDisplayManagerDlg message handlers

BOOL StationVideoDisplayManagerDlgIMOS::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog");
    CRect windowSize;
    GetWindowRect(&windowSize);
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = false;
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    properties.maxHeight = properties.minHeight;
    properties.maxWidth = properties.minWidth;
    setResizingProperties(properties);
    TransActiveDialog::OnInitDialog();
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);        // Set big icon
    //
    // Build the main dialog
    //
    // get the location of the GUI
    AfxGetApp()->DoWaitCursor(1);
    EnableEditSeqButton(false);
    m_ptzButtons.push_back(&m_zoomInBtn);
    m_ptzButtons.push_back(&m_zoomOutBtn);
    m_ptzButtons.push_back(&m_panLeftBtn);
    m_ptzButtons.push_back(&m_panRightBtn);
    m_ptzButtons.push_back(&m_tiltUpBtn);
    m_ptzButtons.push_back(&m_tiltDownBtn);
    m_ptzButtons.push_back(&m_ptLeftUpBtn);
    m_ptzButtons.push_back(&m_ptRightUpBtn);
    m_ptzButtons.push_back(&m_ptLeftDownBtn);
    m_ptzButtons.push_back(&m_ptRightDownBtn);

    // disalbe station select
    m_stationListForSEQ.EnableWindow(FALSE);
    m_stationListForCamera.EnableWindow(FALSE);
    // Disalbe all ptz buttons
    enablePTZButton(FALSE);
    DisplayItemManager::getInstance().initialiseDisplayItems(this);
    SetTimer(HALF_MINUTE_EVENT, 30000, NULL);
    FUNCTION_EXIT;
    return TRUE;
}

void StationVideoDisplayManagerDlgIMOS::LoadButtonIcons()
{
    FUNCTION_ENTRY("LoadButtonIcons");
    m_zoomInBtn.setIcons(IDI_ZOOM_IN_ICON);
    m_zoomOutBtn.setIcons(IDI_ZOOM_OUT_ICON);
    m_ptLeftUpBtn.setIcons(IDI_PT_NW_ICON);
    m_ptRightUpBtn.setIcons(IDI_PT_NE_ICON);
    m_ptRightDownBtn.setIcons(IDI_PT_SE_ICON);
    m_ptLeftDownBtn.setIcons(IDI_PT_SW_ICON);
    m_tiltUpBtn.setIcons(IDI_NORTH_ARROW);
    m_panRightBtn.setIcons(IDI_EAST_ARROW);
    m_tiltDownBtn.setIcons(IDI_SOUTH_ARROW);
    m_panLeftBtn.setIcons(IDI_WEST_ARROW);
    // Set the check box icons.
    /*
    ((CComboBox*) GetDlgItem(IDC_COM_PAN_SPEED))->AddString("低速水平运动");
    ((CComboBox*) GetDlgItem(IDC_COM_PAN_SPEED))->AddString("高速水平运动");
    ((CComboBox*) GetDlgItem(IDC_COM_PAN_SPEED))->SetCurSel(0);
    ((CComboBox*) GetDlgItem(IDC_COM_TILT_SPEED))->AddString("低速垂直运动");
    ((CComboBox*) GetDlgItem(IDC_COM_TILT_SPEED))->AddString("高速垂直运动");
    ((CComboBox*) GetDlgItem(IDC_COM_TILT_SPEED))->SetCurSel(0);
    */
    FUNCTION_EXIT;
}

LRESULT StationVideoDisplayManagerDlgIMOS::OnInitStations(WPARAM wParam, LPARAM lParam)
{
    unsigned long localStation = CCTVConfiguration::getinstance().getLocationKey();
    std::vector < TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
    for (std::vector<TA_Base_Core::ILocation*>::iterator it = stations.begin(); it != stations.end(); ++it)
    {
        TA_Base_Core::ILocation* station = *it;
        if (station->getKey() == localStation)
        {
            std::string str = station->getDisplayName();
            int index = m_stationListForCamera.AddString(station->getDisplayName().c_str());
            m_stationListForCamera.SetItemData(index, station->getKey());
            m_stationListForCamera.SetCurSel(index);
            m_schematicLevelsList.changeLocation(GetStationIdForGwax());
            index = m_stationListForSEQ.AddString(station->getDisplayName().c_str());
            m_stationListForSEQ.SetItemData(index, station->getKey());
            m_stationListForSEQ.SetCurSel(index);
            break;
        }
    }
    return 0;
}

LRESULT StationVideoDisplayManagerDlgIMOS::OnInitSequences(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT StationVideoDisplayManagerDlgIMOS::OnInitCameras(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
LRESULT StationVideoDisplayManagerDlgIMOS::OnInitQuads(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
LRESULT StationVideoDisplayManagerDlgIMOS::OnInitStages(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT StationVideoDisplayManagerDlgIMOS::OnInitMonitors(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitMonitors");
    AfxGetApp()->DoWaitCursor(0);
    setupMonitors();
    DisplayItemManager::getInstance().registerForMonitorChanges(this);
    FUNCTION_EXIT;
    return 0;
}


LRESULT StationVideoDisplayManagerDlgIMOS::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitDisplayManager");
    AfxGetApp()->DoWaitCursor(0);
    LoadButtonIcons();
    // Create video play dailog
    m_videoDlg = new StationDlgVideoPlayImos(DisplayItemManager::getInstance().getIMOSSingleMonitor(),
                                             DisplayItemManager::getInstance().getIMOSQuadMonitors(),
                                             this,
                                             this);
    CDialog* dlg = dynamic_cast<CDialog*>(m_videoDlg);
    TA_ASSERT(dlg != NULL, "");
    dlg->Create(DlgVideoPlayImos::IDD, this);
    m_videoDlg->setMonitorAssignChangedObserver(m_presenter);
    //m_videoDlg->ShowWindow(SW_SHOW);
    m_presenter.setMonitorAssigner(m_videoDlg);
    // Set ptz button call back
    IPTZCommandListener* ptzListener = dynamic_cast<IPTZCommandListener*>(m_videoDlg);
    TA_ASSERT(ptzListener != NULL, "");
    m_zoomInBtn.setCommandListener(ptzListener);
    m_zoomOutBtn.setCommandListener(ptzListener);
    m_panLeftBtn.setCommandListener(ptzListener);
    m_panRightBtn.setCommandListener(ptzListener);
    m_tiltUpBtn.setCommandListener(ptzListener);
    m_tiltDownBtn.setCommandListener(ptzListener);
    m_ptLeftUpBtn.setCommandListener(ptzListener);
    m_ptRightUpBtn.setCommandListener(ptzListener);
    m_ptLeftDownBtn.setCommandListener(ptzListener);
    m_ptRightDownBtn.setCommandListener(ptzListener);
    m_presenter.onCompleteInitStation();
    //  Check the commandline parameters for a camera-to-monitor switch request (TD 13455).
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
    {
        //  Display Manager has been started to handle a switch request.
        //  Set the camera-parameter "trigger" and call onRunParamChange() with the other
        //  monitor-parameter "trigger" : when both triggers are set the switch is made.
        onRunParamChange(RPARAM_DISPLAYMONITOR, TA_Base_Core::RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
    }
    //  Register for camera-to-monitor switch requests (eg. coming from the schematic).
    //  This enters these parameter names into the map of runtime parameters, so
    //  ::isSet(RPARAM_DISPLAYMONITOR) will now
    //  always return true, even if they were not in the commandline).
    RunParams::getInstance().registerRunParamUser(this, RPARAM_DISPLAYMONITOR);
    if (RunParams::getInstance().isSet(RPARAM_TRIGGER_ENTITY))
    {
        onRunParamChange(RPARAM_TRIGGER_ENTITY, RunParams::getInstance().get(RPARAM_TRIGGER_ENTITY));
    }
    RunParams::getInstance().registerRunParamUser(this, RPARAM_TRIGGER_ENTITY);
    // Clear monitor runparam
    if (RunParams::getInstance().isSet(RPARAM_CLEAR_MONITOR))
    {
        onRunParamChange(RPARAM_CLEAR_MONITOR, RunParams::getInstance().get(RPARAM_CLEAR_MONITOR));
    }
    RunParams::getInstance().registerRunParamUser(this, RPARAM_CLEAR_MONITOR);
    AfxGetApp()->DoWaitCursor(-1);
    FUNCTION_EXIT;
    return 0;
}

LRESULT StationVideoDisplayManagerDlgIMOS::onHandleRunParam(WPARAM wParam, LPARAM lParam)
{
    RunParamStrut * runparam = (RunParamStrut *) wParam;
    if (runparam != NULL)
    {
        std::string name = runparam->name;
        std::string value = runparam->value;
        delete runparam;
        m_presenter.onRunParamChange(name , value);
    }
    return 0;
}

void StationVideoDisplayManagerDlgIMOS::OnDestroy()
{
    FUNCTION_ENTRY("OnDestroy");
    delete m_videoDlg;
    DisplayItemManager::getInstance().deregisterForMonitorChanges(this);
    DisplayItemManager::removeInstance();
    CDialog::OnDestroy();
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType)
{
    //
    // Set-up dialog for OCR, Station, or Depot
    //
    // If at the OCC or depot - remove one of the console monitors
}

void StationVideoDisplayManagerDlgIMOS::AddSequenceControlToConsole(int consolePreviousElementButton, int consolePlayCheck, int consoleNextElementButton, int consolePauseCheck, int consolePlaybackLabel, int offset)
{
}

void StationVideoDisplayManagerDlgIMOS::setupMonitors()
{
    m_presenter.setUpMonitor();
}


Monitor* StationVideoDisplayManagerDlgIMOS::getMonitor(int controlId)
{
    FUNCTION_ENTRY("getMonitor");
    // find and return the item in the map
    std::map<int, std::string>::iterator findIter = m_resourceIdToMonitorNameMap.find(controlId);
    if (findIter != m_resourceIdToMonitorNameMap.end())
    {
        FUNCTION_EXIT;
        return DisplayItemManager::getInstance().getMonitor(findIter->second);
    }
    // not found
    FUNCTION_EXIT;
    return NULL;
}


int StationVideoDisplayManagerDlgIMOS::getControlId(std::string monitorName)
{
    FUNCTION_ENTRY("getControlId");
    // find and return the item in the map
    std::map<std::string, int>::iterator findIter = m_monitorNameToResourceIdMap.find(monitorName);
    if (findIter != m_monitorNameToResourceIdMap.end())
    {
        FUNCTION_EXIT;
        return findIter->second;
    }
    // not found
    FUNCTION_EXIT;
    return NULL;
}





// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void StationVideoDisplayManagerDlgIMOS::OnPaint()
{
    FUNCTION_ENTRY("OnPaint");
    if (IsIconic())
    {
        CPaintDC dc(this);    // device context for painting
        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    TransActiveDialog::OnPaint();
    // OnPaint should not directly call com method
    PostMessage(REFRESH_SCHEMATIC_MESSAGE);
    FUNCTION_EXIT;
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR StationVideoDisplayManagerDlgIMOS::OnQueryDragIcon()
{
    FUNCTION_ENTRY("OnQueryDragIcon");
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}


BOOL StationVideoDisplayManagerDlgIMOS::DestroyWindow()
{
    FUNCTION_ENTRY("DestroyWindow");
    if (m_dragImage != NULL)
    {
        delete m_dragImage;
    }
    FUNCTION_EXIT;
    return TransActiveDialog::DestroyWindow();
}


// Don't allow an escape key command to be processed.
BOOL StationVideoDisplayManagerDlgIMOS::PreTranslateMessage(MSG* pMsg)
{
    switch (pMsg->message)
    {
        case WM_KEYDOWN:
            switch (pMsg->wParam)
            {
                case VK_ESCAPE:
                    return TRUE;
                case VK_LEFT:
                    return TRUE;
                case VK_RIGHT:
                    return TRUE;
                case VK_UP:
                    return TRUE;
                case VK_TAB:
                    return TRUE;
                default:
                    break;
            }
            break;
        case WM_SYSCOMMAND:
            // otherwise wont respond to Alt/F4 during capture
            if (pMsg->wParam == SC_CLOSE)
            {
                ::ReleaseCapture();
                if (m_mouseTracking && (m_dragImage != NULL))
                {
                    m_dragImage->DragLeave(this);
                    m_dragImage->EndDrag();
                    delete m_dragImage;
                    m_dragImage = NULL;
                    SetCursor(LoadCursor(NULL, IDC_ARROW));
                }
            }
            break;
        default:
            break;
    }
    return TransActiveDialog::PreTranslateMessage(pMsg);
}


void StationVideoDisplayManagerDlgIMOS::OnLButtonDown(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnLButtonDown");
    TransActiveDialog::OnLButtonDown(nFlags, point);
    m_presenter.MainUILButtonDown(point);
    FUNCTION_EXIT;
}

void StationVideoDisplayManagerDlgIMOS:: beginDragImage()
{
    SetCapture();
    m_mouseTracking = TRUE;
    CPoint cursorLocation;
    ::GetCursorPos(&cursorLocation);
    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
    m_dragImage->DragEnter(NULL, cursorLocation);
}


void StationVideoDisplayManagerDlgIMOS::OnLButtonUp(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnLButtonUp");
    TransActiveDialog::OnLButtonUp(nFlags, point);
    m_presenter.MainUILButtonUp(point);
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::OnMouseMove(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnMouseMove");
    m_presenter.MainUIMouseMove(point);
    TransActiveDialog::OnMouseMove(nFlags, point);
    FUNCTION_EXIT;
}


/**
* UpdateDialog
*
* Sends a WM_PAINT message to the Vidiwall dialog.
*/
void StationVideoDisplayManagerDlgIMOS::updateDialog()
{
    FUNCTION_ENTRY("updateDialog");
    SendMessage(WM_PAINT, NULL, NULL);
    FUNCTION_EXIT;
}


/**
* InvalidateControlLabels
*
* Invalidates the control labels associated with
* the previous and currently highlighted monitors.
*/
void StationVideoDisplayManagerDlgIMOS::invalidateControlLabels()
{
}


void StationVideoDisplayManagerDlgIMOS::OnContextMenu(CWnd* pWnd, CPoint point)
{
    FUNCTION_ENTRY("OnContextMenu");
    ScreenToClient(&point);
    m_presenter.MainUIContextMenu(point);
    CDialog::OnContextMenu(pWnd, point);
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::loadQuadController(Quad* quad)
{
}


void StationVideoDisplayManagerDlgIMOS::loadSequenceController(Sequence* sequence)
{
    /*
    FUNCTION_ENTRY("loadSequenceController");
    TA_ASSERT(sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible");
    // launch Sequence Editor
    SequenceEditorDlg sequenceEditorDlg(this, NULL);
    m_pSequenceEditorDlg = &sequenceEditorDlg;
    sequenceEditorDlg.DoModal(sequence, 0);
    m_pSequenceEditorDlg = NULL;
    FUNCTION_EXIT;
    */
}


void StationVideoDisplayManagerDlgIMOS::OnRButtonDown(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnRButtonDown");
    // Invoke the left button down functionality to ensure
    // that the hilited and non hilited monitors are updated.
    if (FALSE == m_contextActive)
    {
        LPARAM lParam = 0;
        lParam = ((point.x) + (point.y << 16));
        SendMessage(WM_LBUTTONDOWN, MK_RBUTTON, lParam);
    }
    TransActiveDialog::OnRButtonDown(nFlags, point);
    FUNCTION_EXIT;
}

VideoInput* StationVideoDisplayManagerDlgIMOS::getSelectedVideoSource()
{
    FUNCTION_ENTRY("getSelectedVideoSource");
    // Hopefully when the back end for train operations is
    // properly implemented - this can all be encapsulated in
    // VideoSourcesPropertySheet
    VideoInput* source = NULL;
    source = m_sequenceTreeCtrl.getSelectedInput();
    return source;
    FUNCTION_EXIT;
    return source;
}


void StationVideoDisplayManagerDlgIMOS::endDragging()
{
    FUNCTION_ENTRY("endDragging");
    m_sequenceTreeCtrl.endDragging();
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::OnHelpDisplaymanagerhelp()
{
    FUNCTION_ENTRY("OnHelpDisplaymanagerhelp");
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    FUNCTION_EXIT;
}


void StationVideoDisplayManagerDlgIMOS::OnAppAbout()
{
    FUNCTION_ENTRY("OnAppAbout");
    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    FUNCTION_EXIT;
}

void StationVideoDisplayManagerDlgIMOS::OnClose()
{
    FUNCTION_ENTRY("OnClose");
    KillTimer(HALF_MINUTE_EVENT);
    try
    {
        m_presenter.closeSchematic();
    }
    catch (...)
    {
    }
    TransActiveDialog::DestroyWindow();
    FUNCTION_EXIT;
}





LRESULT StationVideoDisplayManagerDlgIMOS::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
{
    return 0;
}



void StationVideoDisplayManagerDlgIMOS::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY("onRunParamChange");
    RunParamStrut * runparam = new RunParamStrut();
    runparam->name = name;
    runparam->value = value;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "get runtime param. name is %s value is %s",
                name.c_str() , value.c_str());
    PostMessage(TA_RUN_PARAM , (WPARAM) runparam , 0);
}


LRESULT StationVideoDisplayManagerDlgIMOS::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnSetWindowPosition");
    switch (lParam)
    {
        case TA_Base_Bus::TA_GenericGui::FOCUS:
            ShowWindow(SW_RESTORE);
            SetForegroundWindow();
            return 1;
        default:
            break;
    }
    FUNCTION_EXIT;
    return 0;
}


//resize the window, refer to PAManager
void StationVideoDisplayManagerDlgIMOS::ModifyWindowPos()
{
    int left, top, width, height;
    int screenWidth, currentScreen;
    int numScreens = 1;
    int x_pos;
    CPoint   pt;
    GetCursorPos(&pt);
    RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                    TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
                                                                    pt.x);
    x_pos = pt.x;
    screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    if (screenWidth >= MINVALUE)
        numScreens = NUMSCREENS;
    currentScreen = x_pos / (screenWidth / numScreens);
    left = currentScreen * (screenWidth / numScreens) + 1;
    width = screenWidth / numScreens;
    top  = boundary.top;
    height = boundary.bottom - boundary.top;
    this->MoveWindow(left, top, width, height, NULL);
}

void StationVideoDisplayManagerDlgIMOS::onSelectionChangeStationList()
{
    m_presenter.SelectStation();
}

unsigned long StationVideoDisplayManagerDlgIMOS::GetStationIdForGwax()
{
    int curItemIdx = m_stationListForCamera.GetCurSel();
    unsigned long locationId  = 0;
    if (CB_ERR != curItemIdx)
    {
        locationId = m_stationListForCamera.GetItemData(curItemIdx);
    }
    return locationId;
}

void StationVideoDisplayManagerDlgIMOS::DisplaySequences(std::vector<VideoInput*> & sequences)
{
    m_sequenceTreeCtrl.DeleteAllItems();
    m_sequenceTreeCtrl.insertVideoSources(sequences);
}

void StationVideoDisplayManagerDlgIMOS::DisplaySequence(Sequence* sequence)
{
    m_sequencesListBox.ResetContent();
    // m_sequencesListBox.addSourceString ( sequence->getDescription ().c_str ()  );
    std::ostringstream desc;
    desc << "视频序列" << sequence->getName();
    m_sequencesListBox.addSourceString(desc.str().c_str());
    std::list<Camera*> cameraList = sequence->getCameras();
    std::list<Camera*>::iterator iter = cameraList.begin();
    for (; iter != cameraList.end(); ++iter)
    {
        desc.clear();
        desc.str("");
        desc << "摄像头" << (*iter)->getName();
        m_sequencesListBox.addSourceString(desc.str().c_str());
    }
}

void StationVideoDisplayManagerDlgIMOS::UpdateSequenceTreeCtrl(Sequence* sequence)
{
    m_sequenceTreeCtrl.videoInputUpdated(sequence);
}


void StationVideoDisplayManagerDlgIMOS::GetSchematicCRET(CRect& markerRect)
{
    CWnd * marker = GetDlgItem(IDC_STATION_STATION_PIC);
    marker->GetWindowRect(markerRect);
}

HWND StationVideoDisplayManagerDlgIMOS::GetWndHandle()
{
    return GetSafeHwnd();
}

unsigned long StationVideoDisplayManagerDlgIMOS::GetSelectedSequenceId()
{
    return 0;
}

unsigned long StationVideoDisplayManagerDlgIMOS:: GetSelectedStation()
{
    int curItemIdx = m_stationListForSEQ.GetCurSel();
    unsigned long locationId  = 0;
    if (CB_ERR != curItemIdx)
    {
        locationId = m_stationListForSEQ.GetItemData(curItemIdx);
    }
    return locationId;
}

void StationVideoDisplayManagerDlgIMOS::onEditSequence()
{
    VideoInput * input = m_sequenceTreeCtrl.getSelectedInput();
    if (NULL != input)
    {
        unsigned long seqId = input->getKey();
        m_presenter.editSequence(seqId);
    }
}

void StationVideoDisplayManagerDlgIMOS ::EnableEditSeqButton(bool enabled)
{
    m_editSequenceBtn.EnableWindow(enabled);
}

MonitorDisplayItem* StationVideoDisplayManagerDlgIMOS::getMonitorDisplayitem(int index , Monitor* monitor)
{
    MonitorDisplayItem* item = new MonitorDisplayItem();
    item->setParentWnd(this);
    switch (index)
    {
            //get local monitor
        case 1 :
            item->setIndex(index);
            item->setLable(&m_quad1_Text);
            m_quad1_Text.SetTransparent(TRUE);
            m_quad1_Index.SetTransparent(TRUE);
            item->setMonitorwnd(&m_quad1);
            item->SetMonitor(monitor);
            item->setLocalMonitor(false);
            break;
        case 2:
            item->setIndex(index);
            item->setLable(&m_quad2_Text);
            m_quad2_Text.SetTransparent(TRUE);
            m_quad2_Index.SetTransparent(TRUE);
            item->setMonitorwnd(&m_quad2);
            item->SetMonitor(monitor);
            item->setLocalMonitor(false);
            break;
        case 3 :
            item->setIndex(index);
            item->setLable(&m_quad3_Text);
            m_quad3_Text.SetTransparent(TRUE);
            m_quad3_Index.SetTransparent(TRUE);
            item->setMonitorwnd(&m_quad3);
            item->SetMonitor(monitor);
            item->setLocalMonitor(false);
            break;
        case 4:
            item->setIndex(index);
            item->setLable(&m_quad4_Text);
            m_quad4_Text.SetTransparent(TRUE);
            m_quad4_Index.SetTransparent(TRUE);
            item->setMonitorwnd(&m_quad4);
            item->SetMonitor(monitor);
            item->setLocalMonitor(false);
            break;
        case 0:
            // local monitor
            item->setIndex(index);
            item->setLable(&m_localMonitorText);
            m_localMonitorText.SetTransparent(TRUE);
            item->setMonitorwnd(&m_localMonitor);
            item->SetMonitor(monitor);
            item->setLocalMonitor(true);
            break;
        default:
            delete item;
            item = NULL;
            break;
    }
    return item ;
}

void StationVideoDisplayManagerDlgIMOS::paintStandControl(MonitorDisplayItem * item)
{
    CStatic* imagewnd = (CStatic*) item->getMonitorWnd();
    if (true == item ->isLocalMonitor())
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SINGLEVMON)));
    }
    else
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                         MAKEINTRESOURCE(IDB_SINGLEVMON)));
    }
    imagewnd ->Invalidate();
    TransActiveDialog::OnPaint();
}


void StationVideoDisplayManagerDlgIMOS::paintHightedControl(MonitorDisplayItem * item)
{
    CStatic* imagewnd = (CStatic*) item->getMonitorWnd();
    imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                     MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE)));
    imagewnd ->Invalidate();
    TransActiveDialog::OnPaint();
}

void StationVideoDisplayManagerDlgIMOS::updateMonitor(MonitorDisplayItem * item)
{
    if (NULL != item)
    {
        CLabel* labelWnd = item->getLable();
        TA_IRS_App::VideoInput* input = item->getMonitor()->getInput();
        if (NULL != input)
        {
            labelWnd->SetText(input->getName().c_str());
        }
        else
        {
            labelWnd->SetText("");
        }
        labelWnd->SetTransparent(TRUE);
    }
}

void StationVideoDisplayManagerDlgIMOS::endDrage()
{
    ::ReleaseCapture();
    endDragging();
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    m_dragImage->DragLeave(this);
    m_dragImage->EndDrag();
    if (NULL != m_dragImage)
    {
        delete m_dragImage;
        m_dragImage = NULL;
    }
}

void StationVideoDisplayManagerDlgIMOS:: handleDragImageOnMouseMove(BOOL begin , CPoint& point)
{
    if (false == begin)
    {
        m_dragImage->DragLeave(NULL);
        m_dragImage->EndDrag();
    }
    else
    {
        ClientToScreen(&point);
        m_dragImage->BeginDrag(0, m_dragAdjustPoint);
        m_dragImage->DragEnter(NULL, point);
        m_contextActive = FALSE;
        m_dragImage->DragMove(point);
        ScreenToClient(&point);
    }
}

void StationVideoDisplayManagerDlgIMOS::loadSequenceSubMenu(CPoint& point , Monitor * monitor, Sequence * seq, bool bIsLocalMonitor)
{
    CMenu contextMenu;
    contextMenu.LoadMenu(IDR_MONSEQCONTEXT_MENU);
    /*
    contextMenu.EnableMenuItem(ID_VIDEO_PLAY, (bIsLocalMonitor && !m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
    contextMenu.EnableMenuItem(ID_VIDEO_STOP, (bIsLocalMonitor && m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
    */
    m_contextActive = TRUE;
    CMenu* pContextMenu = contextMenu.GetSubMenu(0);
    ClientToScreen(&point);
    int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                  point.x, point.y, AfxGetMainWnd());
    m_contextActive = FALSE;
    switch (nCmd)
    {
        case ID_CLEAR_MONITOR:
        {
            m_presenter.clearMonitor(monitor);
        }
        break;
        /*
        case ID_NEW_EDIT_SEQUENCE:
            m_presenter.editSequence(seq->getKey());
            break;
        case ID_VIDEO_PLAY:
            m_presenter.playVideo();
            contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_GRAYED);
            contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_ENABLED);
            break;
        case ID_VIDEO_STOP:
            m_presenter.stopVideo();
            contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_ENABLED);
            contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_GRAYED);
            break;
        */
        default:
            break;
    }
}

void StationVideoDisplayManagerDlgIMOS::loadCameraSubMenu(CPoint& point , Monitor * monitor, bool bIsLcoalMonitor)
{
    CMenu contextMenu;
    contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
    CMenu* pContextMenu = contextMenu.GetSubMenu(0);
    /*
    contextMenu.EnableMenuItem(ID_VIDEO_PLAY, (bIsLcoalMonitor && !m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
    contextMenu.EnableMenuItem(ID_VIDEO_STOP, (bIsLcoalMonitor && m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
    */
    m_contextActive = TRUE;
    ClientToScreen(&point);
    int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                  point.x, point.y, AfxGetMainWnd());
    // Process the appropriate context menu command.
    m_contextActive = FALSE;
    switch (nCmd)
    {
        case ID_CLEAR_MONITOR:
        {
            m_presenter.clearMonitor(monitor);
        }
        break;
        /*
        case ID_VIDEO_PLAY:
            m_presenter.playVideo();
            contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_GRAYED);
            contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_ENABLED);
            break;
        case ID_VIDEO_STOP:
            m_presenter.stopVideo();
            contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_ENABLED);
            contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_GRAYED);
            break;
        */
        default:
            break;
    }
}

void StationVideoDisplayManagerDlgIMOS::displayErrorMessage(CString& message)
{
    AfxMessageBox(message , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
}

void StationVideoDisplayManagerDlgIMOS::enablePTZButton(BOOL enabled)
{
    if (m_ptzButtons.size() == 0) return;
    std::vector <CPTZButton*>::iterator it = m_ptzButtons.begin();
    for (; it != m_ptzButtons.end(); ++it)
    {
        CPTZButton* item = *it;
        item->EnableWindow(enabled);
    }
}

void StationVideoDisplayManagerDlgIMOS::highlightedSelectedMonitor(CWnd * monitorwnd)
{
    if (monitorwnd == NULL) return;
    CStatic* imageWnd = (CStatic *) monitorwnd;
    imageWnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                     MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE)));
}

void StationVideoDisplayManagerDlgIMOS::normalizeMonitor(CWnd * monitorwnd)
{
    if (monitorwnd == NULL) return;
    CStatic* imageWnd = (CStatic *) monitorwnd;
    imageWnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                     MAKEINTRESOURCE(IDB_SINGLEVMON)));
}

void StationVideoDisplayManagerDlgIMOS::displayStaionCameraDiagram(std::string& fullFileName)
{
}

void StationVideoDisplayManagerDlgIMOS::refreshIndexLable(int index)
{
    switch (index)
    {
        case 1:
            m_quad1_Index.Invalidate();
            break;
        case 2:
            m_quad2_Index.Invalidate();
            break;
        case 3:
            m_quad3_Index.Invalidate();
            break;
        case 4:
            m_quad4_Index.Invalidate();
            break;
        default:
            break;
    }
}

unsigned short StationVideoDisplayManagerDlgIMOS::getPanSpeed()
{
    unsigned short speed = 0;
    speed = ((CComboBox*) GetDlgItem(IDC_COM_PAN_SPEED))->GetCurSel() + 1;
    return speed;
}

unsigned short StationVideoDisplayManagerDlgIMOS::getTiltSpeed()
{
    unsigned short speed = 0;
    speed = ((CComboBox*) GetDlgItem(IDC_COM_TILT_SPEED))->GetCurSel() + 1;
    return speed;
}

void StationVideoDisplayManagerDlgIMOS::enableStationSelection(BOOL enable)
{
    m_stationListForSEQ.EnableWindow(enable);
    m_stationListForCamera.EnableWindow(enable);
}

void StationVideoDisplayManagerDlgIMOS::setSelectedStation(unsigned int index)
{
    m_stationListForSEQ.SetCurSel(index);
    m_stationListForCamera.SetCurSel(index);
}

void StationVideoDisplayManagerDlgIMOS::OnTimer(UINT nEventID)
{
    m_presenter.OnTimer(nEventID, &m_sequenceTreeCtrl);
}

void StationVideoDisplayManagerDlgIMOS::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);
}

LRESULT StationVideoDisplayManagerDlgIMOS::onRefreshSchematic(WPARAM wParam, LPARAM lParam)
{
    try
    {
        m_presenter.refreshSchematic();
    }
    catch (...)
    {
    }
    return 0;
}

void StationVideoDisplayManagerDlgIMOS::OnLevelSelChange()
{
    m_presenter.loadStationDiagram(m_schematicLevelsList.getCurrentSelLevel());
}

} // TA_IRS_App
