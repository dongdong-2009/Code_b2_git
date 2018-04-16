/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.cpp $
* @author:  Robert Young
* @version: $Revision: #12 $
*
* Last modification: $DateTime: 2013/11/18 21:38:42 $
* Last modified by:  $Author: huang.wenbo $
*
* TODO: <description>
*/

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "app/cctv/video_display_manager/src/stdafx.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/QuadControllerDlg.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/LocationSelectionDialog.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlgMODBUS.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenter.h"
#include "app/cctv/video_display_manager/src/ModbusMonitorAssigner.h"
#include "app/cctv/video_display_manager/src/ModbusVideoPlayerControl.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78      //zhongjie++
#define MINVALUE                2400    //zhongjie++
#define NUMSCREENS              3       //zhongjie++

using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
const char* NODEVICE = "No item is currently selected";
const unsigned int TRAIN_COMMAND_OFFSET = 1;
/////////////////////////////////////////////////////////////////////////////
// VideoDisplayManagerDlg dialog

VideoDisplayManagerDlgMODBUS::VideoDisplayManagerDlgMODBUS(TA_Base_Bus::IGUIAccess& controlClass, VideoDisplayManagerPresenter& presenter)
        : TransActiveDialog(controlClass, VideoDisplayManagerDlgMODBUS::IDD, NULL),
        m_camerasPage(NULL),
        m_sequencesPage(NULL),
        m_mouseTracking(FALSE),
        m_hilitedControlID(0),
        m_prvHilitedControlID(0),
        m_restoring(FALSE),
        m_contextActive(FALSE),
        m_hilitingPerformed(TRUE),
        m_normalisationPerformed(TRUE),
        m_dragImage(NULL),
        m_dragAdjustPoint(0, 0),
        m_pSequenceEditorDlg(0),
        m_isAssignedBySchematic(false),
        m_overriddenLocation(0),
        m_presenter(presenter),
        m_monitorAssigner(NULL)
{
    FUNCTION_ENTRY("VideoDisplayManagerDlg");
    //{{AFX_DATA_INIT(CGenericGUIDialogTestDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_guiMonitors.fill(NULL);
    m_guiMonitorsDesc.fill(NULL);
    m_sequenceTreeCtrl.setVideoDisplayManagerPresenter(&m_presenter);
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    FUNCTION_EXIT;
}


VideoDisplayManagerDlgMODBUS::~VideoDisplayManagerDlgMODBUS()
{
    FUNCTION_ENTRY("~VideoDisplayManagerDlg");
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::setDragImage(CImageList* dragImage)
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


void VideoDisplayManagerDlgMODBUS::DoDataExchange(CDataExchange* pDX)
{
    TransActiveDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GF_OCC_SEQ_STATION_COMB, m_stationListForSEQ);
    DDX_Control(pDX, IDC_GF_SEQENCE_TREE, m_sequenceTreeCtrl);
    DDX_Control(pDX, IDC_GF_SEQ_LST, m_sequencesListBox);
    DDX_Control(pDX, IDC_GF_OCC_SEL_STATION_COMB, m_stationListForCamera);
    DDX_Control(pDX, IDC_GF_EDIT_SEQ_BTN, m_editSequenceBtn);
    DDX_Control(pDX, IDC_SCHEMATIC_SEL_LIST, m_schematicLevesList);
}


BEGIN_MESSAGE_MAP(VideoDisplayManagerDlgMODBUS, TransActiveDialog)
    //{{AFX_MSG_MAP(VideoDisplayManagerDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONDOWN()
    //      ON_WM_RBUTTONUP()
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
    ON_MESSAGE(TA_INIT_STAGES, OnInitStages)
    ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
    ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
    ON_MESSAGE(TA_RUN_PARAM , onHandleRunParam)
    ON_MESSAGE(TA_INIT_STATION, OnInitStations)
    ON_MESSAGE(REFRESH_SCHEMATIC_MESSAGE, onRefreshSchematic)
    ON_CBN_SELCHANGE(IDC_GF_OCC_SEQ_STATION_COMB, onSelectionChangeStationList)
    ON_CBN_SELCHANGE(IDC_GF_OCC_SEL_STATION_COMB, onSelectionChangeStationDiagramList)
    ON_BN_CLICKED(IDC_GF_EDIT_SEQ_BTN, onEditSequence)
    ON_WM_TIMER()
    ON_WM_MOVE()
    ON_LBN_SELCHANGE(IDC_SCHEMATIC_SEL_LIST, OnLevelSelChange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VideoDisplayManagerDlg message handlers

BOOL VideoDisplayManagerDlgMODBUS::OnInitDialog()
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
    SetIcon(m_hIcon, TRUE);        // Set big icon

    setupGUIMonitors();

    //
    // Build the main dialog
    //
    AfxGetApp()->DoWaitCursor(1);
    // load and initialise display items
    DisplayItemManager::getInstance().initialiseDisplayItems(this);
    EnableEditSeqButton(false);
    SetTimer(HALF_MINUTE_EVENT, 30000, NULL);
    FUNCTION_EXIT;
    return TRUE;  // return TRUE  unless you set the focus to a control
}


LRESULT VideoDisplayManagerDlgMODBUS::OnInitCameras(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void VideoDisplayManagerDlgMODBUS::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MINIMIZE)
    {
    }
    CDialog::OnSysCommand(nID, lParam);
}

LRESULT VideoDisplayManagerDlgMODBUS::OnInitStations(WPARAM wParam, LPARAM lParam)
{
    m_stationListForCamera.AddString("选择车站");
    m_stationListForSEQ.AddString("选择车站");
    std::vector<TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
    for (std::vector<TA_Base_Core::ILocation*>::iterator it = stations.begin(); it != stations.end(); ++it)
    {
        TA_Base_Core::ILocation* station = *it;
        if (station->getKey() == 0 || TA_Base_Core::ILocation::DPT == station->getLocationType())
        {
            continue;
        }
        int index = 0;
        if (TA_Base_Core::ILocation::OCC == station->getLocationType())
        {
            m_stationListForCamera.SetItemData(0, station->getKey());
            m_stationListForSEQ.SetItemData(0, station->getKey());
            // Ensure the occ is first
            index = m_stationListForCamera.InsertString(1, station->getDisplayName().c_str());
            m_stationListForCamera.SetItemData(index, station->getKey());
            index = m_stationListForSEQ.InsertString(1, station->getDisplayName().c_str());
            m_stationListForSEQ.SetItemData(index, station->getKey());
        }
        else
        {
            index = m_stationListForCamera.AddString(station->getDisplayName().c_str());
            m_stationListForCamera.SetItemData(index, station->getKey());
            index = m_stationListForSEQ.AddString(station->getDisplayName().c_str());
            m_stationListForSEQ.SetItemData(index, station->getKey());
        }
    }
    m_stationListForSEQ.SetCurSel(0);
    m_stationListForCamera.SetCurSel(0);
    /*
    int itemToSelect = -1 ;
    int stationID = m_presenter.getLocationId();
    for(int index = 0, listSize = m_stationListForCamera.GetCount(); index < listSize; ++index)
    {
        if(m_stationListForCamera.GetItemData(index)  ==  stationID)
        {
            itemToSelect = index;
            break;
        }
    }
    if((itemToSelect > CB_ERR) &&
            (itemToSelect < stations.size()))
    {
        m_stationListForSEQ.SetCurSel(itemToSelect);
        m_stationListForCamera.SetCurSel(itemToSelect);
    }
    */
    return 0;
}

LRESULT VideoDisplayManagerDlgMODBUS::OnInitQuads(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitQuads");
    FUNCTION_EXIT;
    return 0;
}


LRESULT VideoDisplayManagerDlgMODBUS::OnInitSequences(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitSequences");
    GetDlgItem(IDC_GF_OCC_SEQ_STATION_COMB)->EnableWindow(TRUE);
    GetDlgItem(IDC_GF_OCC_SEL_STATION_COMB)->EnableWindow(TRUE);
    FUNCTION_EXIT;
    return 0;
}


LRESULT VideoDisplayManagerDlgMODBUS::OnInitStages(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


LRESULT VideoDisplayManagerDlgMODBUS::OnInitMonitors(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitMonitors");
    AfxGetApp()->DoWaitCursor(0);
    setupMonitors();
    DisplayItemManager::getInstance().registerForMonitorChanges(this);
    FUNCTION_EXIT;
    return 0;
}


LRESULT VideoDisplayManagerDlgMODBUS::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnInitDisplayManager");
    AfxGetApp()->DoWaitCursor(0);
    // Create video play dailog
	launchModbusVideoPlayer();
	startDaemonThread();
    m_monitorAssigner = new ModbusMonitorAssigner(DisplayItemManager::getInstance().getAllMonitorsArray());
    m_monitorAssigner->setMonitorAssignChangedObserver(m_presenter);
    m_presenter.setMonitorAssigner(m_monitorAssigner);
    AfxGetApp()->DoWaitCursor(-1);
    m_presenter.onCompleteInitStation();
    //  Check the commandline parameters for a camera-to-monitor switch request (TD 13455).
    if (RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
    {
        //  Display Manager has been started to handle a switch request.
        //  Set the camera-parameter "trigger" and call onRunParamChange() with the other
        //  monitor-parameter "trigger" : when both triggers are set the switch is made.
        onRunParamChange(RPARAM_DISPLAYMONITOR, RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
        //  ShowWindow( SW_MINIMIZE);
        //  m_isAssignedBySchematic = false;
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
    FUNCTION_EXIT;
    return 0;
}

LRESULT VideoDisplayManagerDlgMODBUS::onHandleRunParam(WPARAM wParam, LPARAM lParam)
{
    RunParamStrut * runparam = (RunParamStrut *) wParam;
    if (runparam == NULL)
    {
        //error
    }
    else
    {
        std::string name = runparam->name;
        std::string value = runparam->value;
        delete runparam;
        m_presenter.onRunParamChange(name , value);
    }
    return 0;
}

LRESULT VideoDisplayManagerDlgMODBUS::onRefreshSchematic(WPARAM wParam, LPARAM lParam)
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

void VideoDisplayManagerDlgMODBUS::OnDestroy()
{
    FUNCTION_ENTRY("OnDestroy");
    delete m_monitorAssigner;
	terminateDaemonThread();
    DisplayItemManager::getInstance().deregisterForMonitorChanges(this);
    DisplayItemManager::removeInstance();
    CDialog::OnDestroy();
    FUNCTION_EXIT;
}

void VideoDisplayManagerDlgMODBUS::AddMonitorToConsole(int consoleMonitorId, int labelConsoleMonitorId, int textConsoleMonitorId, int offset)
{
}

void VideoDisplayManagerDlgMODBUS::AddSequenceControlToConsole(int consolePreviousElementButton, int consolePlayCheck, int consoleNextElementButton, int consolePauseCheck, int consolePlaybackLabel, int offset)
{
}

void VideoDisplayManagerDlgMODBUS::setupMonitors()
{
    m_presenter.setUpMonitor();
}


Monitor* VideoDisplayManagerDlgMODBUS::getMonitor(int controlId)
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


int VideoDisplayManagerDlgMODBUS::getControlId(std::string monitorName)
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
void VideoDisplayManagerDlgMODBUS::OnPaint()
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
HCURSOR VideoDisplayManagerDlgMODBUS::OnQueryDragIcon()
{
    FUNCTION_ENTRY("OnQueryDragIcon");
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}


BOOL VideoDisplayManagerDlgMODBUS::DestroyWindow()
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
BOOL VideoDisplayManagerDlgMODBUS::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY("PreTranslateMessage");
    switch (pMsg->message)
    {
        case WM_KEYDOWN:
        {
            switch (pMsg->wParam)
            {
                case VK_ESCAPE:
                {
                    FUNCTION_EXIT;
                    return TRUE;
                }
                break;
                case VK_LEFT:
                {
                    FUNCTION_EXIT;
                    return TRUE;
                }
                break;
                case VK_RIGHT:
                {
                    FUNCTION_EXIT;
                    return TRUE;
                }
                break;
                case VK_UP:
                {
                    FUNCTION_EXIT;
                    return TRUE;
                }
                break;
                case VK_TAB:
                {
                    FUNCTION_EXIT;
                    return TRUE;
                }
                break;
                default:
                {
                    //do nothing
                }
                break;
            }
        }
        case WM_SYSCOMMAND:
        {
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
        }
        break;
        default:
        {
            //do nothing
        }
        break;
    }
    FUNCTION_EXIT;
    return TransActiveDialog::PreTranslateMessage(pMsg);
}


void VideoDisplayManagerDlgMODBUS::OnLButtonDown(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnLButtonDown");
    TransActiveDialog::OnLButtonDown(nFlags, point);
    m_presenter.MainUILButtonDown(point);
    FUNCTION_EXIT;
}

void VideoDisplayManagerDlgMODBUS:: beginDragImage()
{
    SetCapture();
    m_mouseTracking = TRUE;
    CPoint cursorLocation;
    ::GetCursorPos(&cursorLocation);
    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
    m_dragImage->DragEnter(NULL, cursorLocation);
}


void VideoDisplayManagerDlgMODBUS::OnLButtonUp(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY("OnLButtonUp");
    TransActiveDialog::OnLButtonUp(nFlags, point);
    m_presenter.MainUILButtonUp(point);
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::OnMouseMove(UINT nFlags, CPoint point)
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
void VideoDisplayManagerDlgMODBUS::updateDialog()
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
void VideoDisplayManagerDlgMODBUS::invalidateControlLabels()
{
    FUNCTION_ENTRY("invalidateControlLabels");
    if ((m_normalisationPerformed && m_hilitingPerformed))
    {
        //nothing to do
        FUNCTION_EXIT;
        return;
    }
    // Declare an iterator for the vector.
    std::vector< boost::shared_ptr< CLabel > >::iterator theIterator;
    // Iterate through the vector and invalidate both of the appropriate labels.
    int i = 0;
    for (theIterator = m_labels.begin(); theIterator != m_labels.end(); theIterator++)
    {
        if (m_hilitedControlID == (* theIterator)->GetDlgCtrlID() - MONITOR_TEXT_DIFF)
        {
            if ((FALSE == m_hilitingPerformed))     //|| ( m_mouseTracking ) )
            {
                (*theIterator)->Invalidate();
            }
        }
        if (m_prvHilitedControlID  == (* theIterator)->GetDlgCtrlID() - MONITOR_TEXT_DIFF)
        {
            if ((FALSE == m_normalisationPerformed))     //|| ( m_mouseTracking ) )
            {
                (*theIterator)->Invalidate();
            }
        }
        i++;
    }
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::OnContextMenu(CWnd* pWnd, CPoint point)
{
    FUNCTION_ENTRY("OnContextMenu");
    ScreenToClient(&point);
    m_presenter.MainUIContextMenu(point);
    CDialog::OnContextMenu(pWnd, point);
    // if context menu currently displayed
    FUNCTION_EXIT;
}

void VideoDisplayManagerDlgMODBUS::loadQuadController(Quad* quad)
{
}

void VideoDisplayManagerDlgMODBUS::loadSequenceController(Sequence* sequence)
{
    /*
    FUNCTION_ENTRY ( "loadSequenceController" );
    TA_ASSERT ( sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible" );
    SequenceEditorDlg sequenceEditorDlg ( this, &m_sequencesPage->m_sequenceTreeCtrl );
    m_pSequenceEditorDlg = &sequenceEditorDlg;
    sequenceEditorDlg.DoModal ( sequence, GetSelectedStation() );
    m_pSequenceEditorDlg = NULL;
    FUNCTION_EXIT;
    */
}


void VideoDisplayManagerDlgMODBUS::OnRButtonDown(UINT nFlags, CPoint point)
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

VideoInput* VideoDisplayManagerDlgMODBUS::getSelectedVideoSource()
{
    FUNCTION_ENTRY("getSelectedVideoSource");
    // Hopefully when the back end for train operations is
    // properly implemented - this can all be encapsulated in
    // VideoSourcesPropertySheet
    VideoInput* source = NULL;
    source = m_sequenceTreeCtrl.getSelectedInput();
    FUNCTION_EXIT;
    return source;
}


unsigned short VideoDisplayManagerDlgMODBUS::getSelectedTrainId()
{
    return 0;
}


void VideoDisplayManagerDlgMODBUS::endDragging()
{
    FUNCTION_ENTRY("endDragging");
    m_sequenceTreeCtrl.endDragging();
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::OnHelpDisplaymanagerhelp()
{
    FUNCTION_ENTRY("OnHelpDisplaymanagerhelp");
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::OnAppAbout()
{
    FUNCTION_ENTRY("OnAppAbout");
    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    FUNCTION_EXIT;
}


void VideoDisplayManagerDlgMODBUS::OnClose()
{
    FUNCTION_ENTRY("OnClose");
    KillTimer(HALF_MINUTE_EVENT);
    //  m_presenter.saveMonitorRelationship();
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



LRESULT VideoDisplayManagerDlgMODBUS::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
{
    DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
    Monitor* monitor = dynamic_cast<Monitor*>(item);
    m_presenter.monitorAssignChanged(monitor);
    return 0;
}

void VideoDisplayManagerDlgMODBUS::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY("onRunParamChange");
    RunParamStrut * runparam = new RunParamStrut();
    runparam->name = name;
    runparam->value = value;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "get runtime param. name is %s value is %s",
                name.c_str() , value.c_str());
    PostMessage(TA_RUN_PARAM , (WPARAM) runparam , 0);
    FUNCTION_EXIT;
}


// TD 16824
LRESULT VideoDisplayManagerDlgMODBUS::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnSetWindowPosition");
    switch (lParam)
    {
        case TA_Base_Bus::TA_GenericGui::FOCUS:
            ShowWindow(SW_RESTORE);
            SetForegroundWindow();
            return true;
        default:
            break;
    }
    FUNCTION_EXIT;
    return S_OK;
}


//resize the window, refer to PAManager
void VideoDisplayManagerDlgMODBUS::ModifyWindowPos()
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
    left    = currentScreen * (screenWidth / numScreens) + 1;
    width   = screenWidth / numScreens;
    top     = boundary.top;
    height  = boundary.bottom - boundary.top;
    this->MoveWindow(left, top, width, height, NULL);
}

void VideoDisplayManagerDlgMODBUS::onSelectionChangeStationList()
{
    int sel = m_stationListForSEQ.GetCurSel();
    if (sel > 0)
    {
        m_presenter.SelectStation();
        m_sequencesListBox.ResetContent();
    }
}

void VideoDisplayManagerDlgMODBUS::onSelectionChangeStationDiagramList()
{
    int sel = m_stationListForCamera.GetCurSel();
    if ( sel > 0)
    {
	    m_schematicLevesList.changeLocation(GetStationIdForGwax());
	    // First item as the default schematic
	    m_presenter.loadStationDiagram(m_schematicLevesList.getFirstItemLevel());
    }
}

unsigned long VideoDisplayManagerDlgMODBUS::GetStationIdForGwax()
{
    int curItemIdx = m_stationListForCamera.GetCurSel();
    unsigned long locationId  = 0;
    if (CB_ERR != curItemIdx)
    {
        locationId = m_stationListForCamera.GetItemData(curItemIdx);
    }
    else
    {
        locationId = m_stationListForCamera.GetItemData(0);
    }
    return locationId;
}

void VideoDisplayManagerDlgMODBUS::DisplaySequences(std::vector<VideoInput*> & sequences)
{
    m_sequenceTreeCtrl.DeleteAllItems();
    m_sequenceTreeCtrl.insertVideoSources(sequences);
}

void VideoDisplayManagerDlgMODBUS::DisplaySequence(Sequence* sequence)
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

void VideoDisplayManagerDlgMODBUS::UpdateSequenceTreeCtrl(Sequence* sequence)
{
    m_sequenceTreeCtrl.videoInputUpdated(sequence);
}


void VideoDisplayManagerDlgMODBUS::GetSchematicCRET(CRect& markerRect)
{
    CWnd * marker = GetDlgItem(IDC_GF_OCC_STATION_PIC);
    marker->GetWindowRect(markerRect);
}
HWND VideoDisplayManagerDlgMODBUS::GetWndHandle()
{
    return GetSafeHwnd();
}
unsigned long VideoDisplayManagerDlgMODBUS::GetSelectedSequenceId()
{
    return 0;
}

unsigned long VideoDisplayManagerDlgMODBUS:: GetSelectedStation()
{
    int curItemIdx = m_stationListForSEQ.GetCurSel();
    unsigned long locationId  = 0;
    if (CB_ERR != curItemIdx)
    {
        locationId = m_stationListForSEQ.GetItemData(curItemIdx);
    }
    return locationId;
}

void VideoDisplayManagerDlgMODBUS::onEditSequence()
{
    VideoInput * input = m_sequenceTreeCtrl.getSelectedInput();
    if (input != NULL)
    {
        unsigned long seqId = input->getKey();
        m_presenter.editSequence(seqId);
    }
}

void VideoDisplayManagerDlgMODBUS ::EnableEditSeqButton(bool enabled)
{
    m_editSequenceBtn.EnableWindow(enabled);
}

MonitorDisplayItem* VideoDisplayManagerDlgMODBUS::getMonitorDisplayitem(int index , Monitor* monitor)
{
    MonitorDisplayItem* item = new MonitorDisplayItem();
    item->setParentWnd(this);
    item->setIndex(index);
    item->setLable(m_guiMonitorsDesc[index]);
    item->setMonitorwnd(m_guiMonitors[index]);
    item->SetMonitor(monitor);
    item->setLocalMonitor(index == 0);

    return item ;
}

void VideoDisplayManagerDlgMODBUS::paintStandControl(MonitorDisplayItem * item)
{
    CStatic* imagewnd = (CStatic*) item->getMonitorWnd();
    if (item->isLocalMonitor())
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                         MAKEINTRESOURCE(IDB_SINGLEVMON)));
    }
    else
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                         MAKEINTRESOURCE(IDB_SINGLEVMON)));
    }
    imagewnd ->Invalidate();
    TransActiveDialog::OnPaint();
}

void VideoDisplayManagerDlgMODBUS::refreshIndexLable(int index)
{
    /*
    switch(index)
    {
        case 1:
            m_monitorIndex_01.Invalidate();
        case 2:
            m_monitorIndex_02.Invalidate();
            break;
        case 3:
            m_monitorIndex_03.Invalidate();
            break;
        case 4:
            m_monitorIndex_04.Invalidate();
            break;
        case 5:
            m_monitorIndex_05.Invalidate();
            break;
        case 6:
            m_monitorIndex_06.Invalidate();
            break;
        default:
            break;
    }
    */
}


void VideoDisplayManagerDlgMODBUS::paintHightedControl(MonitorDisplayItem * item)
{
    CStatic* imagewnd = (CStatic*) item->getMonitorWnd();
    if (item->isLocalMonitor())
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                         MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE)));
    }
    else
    {
        imagewnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                         MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE)));
    }
    imagewnd ->Invalidate();
    TransActiveDialog::OnPaint();
}

void VideoDisplayManagerDlgMODBUS::updateMonitor(MonitorDisplayItem * item)
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


void VideoDisplayManagerDlgMODBUS::endDrage()
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

void VideoDisplayManagerDlgMODBUS:: handleDragImageOnMouseMove(BOOL begin , CPoint& point)
{
    if (! begin)
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

void VideoDisplayManagerDlgMODBUS::loadSequenceSubMenu(CPoint& point , Monitor * monitor, Sequence* seq, bool bIsLocalMonitor)
{
    CMenu contextMenu;
    contextMenu.LoadMenu(IDR_MONSEQCONTEXT_MENU);
    /*
    contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, ( bIsLocalMonitor && !m_videoPlaying ) ? MF_ENABLED : MF_GRAYED );
    contextMenu.EnableMenuItem ( ID_VIDEO_STOP, ( bIsLocalMonitor && m_videoPlaying ) ? MF_ENABLED : MF_GRAYED );
    */
    m_contextActive = TRUE;
    CMenu* pContextMenu = contextMenu.GetSubMenu(0);
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
        case ID_NEW_EDIT_SEQUENCE:
            m_presenter.editSequence ( seq->getKey () );
            break;
        case ID_VIDEO_PLAY:
            m_presenter.playVideo();
            contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_GRAYED );
            contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_ENABLED );
            break;
        case ID_VIDEO_STOP:
            m_presenter.stopVideo();
            contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_ENABLED );
            contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_GRAYED );
            break;
        */
        default:
            break;
    }
}
void VideoDisplayManagerDlgMODBUS::loadCameraSubMenu(CPoint& point , Monitor* monitor, bool bIsLocalMonitor)
{
    CMenu contextMenu;
    contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
    /*
    contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, ( bIsLocalMonitor && !m_videoPlaying ) ? MF_ENABLED : MF_GRAYED );
    contextMenu.EnableMenuItem ( ID_VIDEO_STOP, ( bIsLocalMonitor && m_videoPlaying ) ? MF_ENABLED : MF_GRAYED );
    */
    CMenu* pContextMenu = contextMenu.GetSubMenu(0);
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
            break;
        }
        /*
        case ID_NEW_EDIT_SEQUENCE:
            loadSequenceController(seq);
            break;
        case ID_VIDEO_PLAY:
            m_presenter.playVideo();
            contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_GRAYED );
            contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_ENABLED );
            break;
        case ID_VIDEO_STOP:
            m_presenter.stopVideo();
            contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_ENABLED );
            contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_GRAYED );
            break;
        */
        default:
            break;
    }
}

void VideoDisplayManagerDlgMODBUS::displayErrorMessage(CString& message)
{
    AfxMessageBox(message , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
}

void VideoDisplayManagerDlgMODBUS::displayStaionCameraDiagram(std::string& fullFileName)
{
}

void VideoDisplayManagerDlgMODBUS::highlightedSelectedMonitor(CWnd * monitorwnd)
{
    if (monitorwnd == NULL) return;
    //  CStatic* imageWnd =(CStatic *) ::GetDlgItem ( this->GetSafeHwnd() , IDC_WALL_MONITOR_4 );
    CStatic* imageWnd = (CStatic *) monitorwnd;
    imageWnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                     MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE)));
    TransActiveDialog::OnPaint();
}

void VideoDisplayManagerDlgMODBUS::normalizeMonitor(CWnd * monitorwnd)
{
    if (monitorwnd == NULL) return;
    CStatic* imageWnd = (CStatic *) monitorwnd;
    imageWnd->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(),
                                     MAKEINTRESOURCE(IDB_SINGLEVMON)));
    TransActiveDialog::OnPaint();
}

void VideoDisplayManagerDlgMODBUS::enableStationSelection(BOOL enable)
{
    m_stationListForSEQ.EnableWindow(enable);
    m_stationListForCamera.EnableWindow(enable);
}

void VideoDisplayManagerDlgMODBUS::setSelectedStation(unsigned int index)
{
    m_stationListForSEQ.SetCurSel(index);
    m_stationListForCamera.SetCurSel(index);
}

void VideoDisplayManagerDlgMODBUS::OnTimer(UINT nEventID)
{
    m_presenter.OnTimer(nEventID, &m_sequenceTreeCtrl);
}

void VideoDisplayManagerDlgMODBUS::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);
}

void VideoDisplayManagerDlgMODBUS::setupGUIMonitors()
{
    CWnd* groupWnds[] = {
        GetDlgItem(IDC_MON_GROUP_LOCAL),
        GetDlgItem(IDC_MON_GROUP_OPS1),
        GetDlgItem(IDC_MON_GROUP_OPS2),
        GetDlgItem(IDC_MON_GROUP_OPS3),
        GetDlgItem(IDC_MON_GROUP_OPS4)
    };

    CBitmap monBm;
    BITMAP bm;
    monBm.LoadBitmap(IDB_BITMAP1);
    monBm.GetObject(sizeof(bm), &bm);

    const int quadMargin = 3;
    const int topMargin = 36;
    const int botMargin = 10;

    for (int i = 0; i < 25; ++i)
    {
        CWnd* groupWnd = groupWnds[i / 5];
        RECT rect;
        groupWnd->GetClientRect(&rect);
        const int widthMarginQaud = (rect.right - rect.left - quadMargin - bm.bmWidth * 2) / 2;
        const int widthMarginSingle = (rect.right - rect.left - bm.bmWidth) / 2;

        CRect monRect;

        switch (i % 5)
        {
            case 0: // single screen
                monRect = CRect(widthMarginSingle, rect.bottom - botMargin - bm.bmHeight, widthMarginSingle + bm.bmWidth, rect.bottom - botMargin);
                break;
            case 1: // quad, top left
                monRect = CRect(widthMarginQaud, topMargin, widthMarginQaud + bm.bmWidth, topMargin + bm.bmHeight);
                break;
            case 2: // quad, top right
                monRect = CRect(widthMarginQaud + quadMargin + bm.bmWidth, topMargin, widthMarginQaud + bm.bmWidth * 2 + quadMargin, topMargin + bm.bmHeight);
                break;
            case 3: // quad, bot left
                monRect = CRect(widthMarginQaud, topMargin + bm.bmHeight + quadMargin, widthMarginQaud + bm.bmWidth, topMargin + bm.bmHeight * 2 + quadMargin);
                break;
            case 4: // quad, bot right
                monRect = CRect(widthMarginQaud + quadMargin + bm.bmWidth, topMargin + bm.bmHeight + quadMargin, widthMarginQaud + bm.bmWidth * 2 + quadMargin, topMargin + bm.bmHeight * 2 + quadMargin);
                break;
        }

        groupWnd->ClientToScreen(monRect);
        this->ScreenToClient(monRect);

        CStatic* s = new CStatic();
        s->Create("", SS_BITMAP | WS_CHILD | WS_VISIBLE, monRect, this);
        s->SetBitmap(monBm);

        CLabel* l = new CLabel();
        CRect labelRect(monRect.left + 2, monRect.top + (monRect.Height() - 16) / 2, monRect.right - 2, monRect.top + (monRect.Height() - 16) / 2 + 16);
        HWND t = CreateWindow(
                     "STATIC",  // Predefined class; Unicode assumed
                     "",      // Button text
                     WS_VISIBLE | WS_CHILD | WS_EX_TRANSPARENT | SS_CENTER,  // Styles
                     labelRect.left,         // x position
                     labelRect.top,         // y position
                     labelRect.Width(),        // Button width
                     labelRect.Height(),        // Button height
                     this->m_hWnd,     // Parent window
                     NULL,       // No menu.
                     (HINSTANCE)GetWindowLong(this->m_hWnd, GWL_HINSTANCE),
                     NULL);      // Pointer not needed.
        ::SendMessage(t, WM_SETFONT, (WPARAM)(GetFont()->operator HFONT()), TRUE);

        l->SubclassWindow(t);
        l->SetTransparent(TRUE);

        m_guiMonitors[i] = s;
        m_guiMonitorsDesc[i] = l;
    }

    monBm.Detach();

}
void VideoDisplayManagerDlgMODBUS::OnLevelSelChange()
{
    m_presenter.loadStationDiagram(m_schematicLevesList.getCurrentSelLevel());
}

} // TA_IRS_App
