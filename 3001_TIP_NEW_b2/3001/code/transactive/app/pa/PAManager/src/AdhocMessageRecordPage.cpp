/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/AdhocMessageRecordPage.cpp $
  * @author Jade Lee
  * @version $Revision: #12 $
  * Last modification: $DateTime: 2017/09/29 18:05:34 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * Provides a dialog interface for recording of ad-hoc
  *     messages.  While the dialog is active, it is possible
  *     for external sources to modify the type / label of the
  *     ad-hoc message being recorded.  No special actions are
  *     taken when this occurs (ignored, and changes applied when
  *     user closes dialog regardless)
  *
  */

#include "app/pa/PAManager/src/stdafx.h"

#include <time.h>

#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/AdhocMessageRecordPage.h"
#include "app/pa/PAManager/src/OperatorTerminalDvaPlaybackModel.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/RightsChangedWindowNotificationHandler.h"
#include "app/pa/PAManager/src/StationBroadcastModel.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "app/pa/PAManager/src/CachedConfig.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AdhocMessageRecordPage dialog

static const UINT TIMER_ID_TIME_FIELD = 1;
// Time elapsed between updating of elapsed time field (during recording or playback)
static const UINT TIMER_REFRESH_MS = 100;   // 10 updates / second

static const UINT TIMER_COUNTDOWN_START_TIME = 60;

const char* const s_deleteMessageComplete = "Delete message successfully.";
const char* const s_deleteMessageFailed = "Delete message failed.";

AdhocMessageRecordPage::AdhocMessageRecordPage( 
    const TA_IRS_Bus::StationDvaMessageRecord& source,
    OperatorTerminalDvaPlaybackModel& dvaPlaybackObject, 
    StationBroadcastModel& stnBroadcastModel,
    CWnd* pParent /*=NULL*/ )	:
CDialog(AdhocMessageRecordPage::IDD, pParent),
m_recordingModel(NULL),
m_hIcons(),
m_dvaPlaybackObject(dvaPlaybackObject),
m_startTime(),
m_estimatedDuration(),
m_isDeleted(TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == source.usMsgType), 
m_hasMessageTypeOrLabelBeenModified(false),
m_timeFieldTimer(0),
m_rightsChangeHandler( NULL ),
m_isPlayBack( false ),
m_stnBroadcastModel( stnBroadcastModel )
{
    m_recordingModel = new AdhocMessageRecordModel( source.ulKey, *this );
    bool bEmergency = ( source.usMsgType == TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY );
    m_recordingModel->setEmergencyMsg( bEmergency );

    if ( m_isDeleted )
    {
        m_adhocDescription = _T("");
    }
    else
    {
        m_adhocDescription = _T(source.strMsgName.c_str());
    }
	m_time = _T("");
}


AdhocMessageRecordPage::~AdhocMessageRecordPage()
{
    if ( NULL != m_recordingModel )
    {
        delete m_recordingModel;
        m_recordingModel = NULL;
    }
	if ( NULL != m_rightsChangeHandler )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Rights change handler should be deleted in OnDestroy" );
	}
}


void AdhocMessageRecordPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AdhocMessageRecordPage)
	DDX_Control(pDX, IDC_STOP, m_stopButton);
	DDX_Control(pDX, IDC_CHECK_TYPE, m_typeControl);
    DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_descriptionControl);
    DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDC_DELETE, m_deleteButton);
	DDX_Control(pDX, IDC_LISTEN, m_listenButton);
	DDX_Control(pDX, IDC_START, m_startRecordingButton);
    DDX_Control(pDX, IDC_RECORD_STATUS, m_recordStatus);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_adhocDescription);
	DDX_Text(pDX, IDC_EDIT_TIME, m_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AdhocMessageRecordPage, CDialog)
	//{{AFX_MSG_MAP(AdhocMessageRecordPage)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_BUTTON_HLP, OnHelp)
	ON_BN_CLICKED(IDC_LISTEN, OnListen)
	ON_BN_CLICKED(IDC_START, OnStartRecording)
    ON_BN_CLICKED(IDCANCEL, OnCancel)
    ON_MESSAGE(WM_PA_STN_STATUS_REPORT_UPDATE, OnProgressReportUpdate)
    ON_MESSAGE(WM_PA_RIGHTS_CHANGE, OnRightsChanged)
	ON_WM_DESTROY()
    ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT_DESCRIPTION, OnChangeEditDescription)
	ON_BN_CLICKED(IDC_CHECK_TYPE, OnCheckType)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AdhocMessageRecordPage message handlers


BOOL AdhocMessageRecordPage::OnInitDialog() 
{
    m_rightsChangeHandler =  new RightsChangedWindowNotificationHandler(m_hWnd);

	CDialog::OnInitDialog();
	
    // Call to perform initialisation of dialog if in deleted state
    setIsDeleted(m_isDeleted);

    // Display initial time
    updateTimeField();

    updateButtonStates();
    
    // Set up button icons
    CRect rc(0, 0, 32, 32);
	// TD16864
	CRect tempRect(0, 0, 20, 20);
	// TD16864
    m_hIcons[IDI_AHR_RECORD] = LoadImage( AfxGetInstanceHandle(), 
						            MAKEINTRESOURCE( IDI_AHR_RECORD ),
						            IMAGE_ICON, 
						            rc.Width(), 
						            rc.Height(), 
						            LR_DEFAULTCOLOR );
    m_hIcons[IDI_AHR_DELETE] = LoadImage( AfxGetInstanceHandle(), 
						            MAKEINTRESOURCE( IDI_AHR_DELETE ),
						            IMAGE_ICON, 
						            rc.Width(), 
						            rc.Height(), 
						            LR_DEFAULTCOLOR );
    m_hIcons[IDI_PLAY_ICON] = LoadImage( AfxGetInstanceHandle(), 
						            MAKEINTRESOURCE( IDI_PLAY_ICON ),
						            IMAGE_ICON, 
						            rc.Width(), 
						            rc.Height(), 
						            LR_DEFAULTCOLOR );
    m_hIcons[IDI_STOP_ICON] = LoadImage( AfxGetInstanceHandle(), 
						            MAKEINTRESOURCE( IDI_STOP_ICON ),
						            IMAGE_ICON, 
						            tempRect.Width(), //TD16864  
						            tempRect.Height(), //TD16864  
						            LR_DEFAULTCOLOR );


    m_stopButton.SetIcon((HICON)m_hIcons[IDI_STOP_ICON]);
    m_deleteButton.SetIcon((HICON)m_hIcons[IDI_AHR_DELETE]);
	m_listenButton.SetIcon((HICON)m_hIcons[IDI_PLAY_ICON]);
	m_startRecordingButton.SetIcon((HICON)m_hIcons[IDI_AHR_RECORD]);

    m_descriptionControl.SetLimitText( TA_Base_Bus::IPAAgentCorbaDef::MAX_ADHOC_LABEL_LENGTH );

    int nEmergencyCheck = 0;
    if ( NULL != m_recordingModel )
    {
        nEmergencyCheck = m_recordingModel->isEmergencyMsg() ? 1 : 0;
    }
    m_typeControl.SetCheck( nEmergencyCheck );

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void AdhocMessageRecordPage::destroyIcons()
{
    for (std::map<DWORD, HANDLE>::iterator it = m_hIcons.begin();
                it != m_hIcons.end();
                it ++)
    {
        BOOL result = DestroyIcon((HICON)it->second);
        
		if ( false == result )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Icon destruction failed" );
			TA_THROW(TA_Base_Core::TransactiveException("Icon destruction failed"));
		}
    }

    m_hIcons.clear();
}


void AdhocMessageRecordPage::setIsDeleted(bool bDeleted)
{
    // Do not do state change checking here - ie if (m_isDeleted == bDeleted) return;
    // Otherwise startup state will be incorrect
    // We delete an adhoc message by setting the deleted flag, and clearing label   
    m_isDeleted = bDeleted;

    if (m_isDeleted)
    {
		m_recordingModel->setEmergencyMsg(false);
        m_adhocDescription = "";
        m_typeControl.SetCheck( 0 );
        UpdateData(FALSE);

        // Zero out duration (being an empty message)
        m_estimatedDuration.setValue(0);
        updateTimeField();
    }
    
    // The deleted state of a message defines whether the description or type controls enabled or not
    refreshDescriptionTypeEnabledStates();
}


void AdhocMessageRecordPage::refreshDescriptionTypeEnabledStates()
{
    // By default, allow modification if not deleted
    BOOL allowModification = TRUE;

    // However, if no right to modify message, prohibit changes
    if (!PaRightsManager::getInstance().hasRight(
                        PaRightsManager::PA_MODIFY_ADHOC_MESSAGE))
    {
        allowModification = FALSE;
    }

    // Update enabled state some of the input controls
    // They only accept inputs to a non deleted message
    m_descriptionControl.EnableWindow(allowModification);
    m_typeControl.EnableWindow(allowModification);
}


const std::string AdhocMessageRecordPage::getMessageDescription()
{
    if (IsWindow(m_hWnd))
    {
        UpdateData(TRUE);  // Fetch up to date data from control
    }
    return std::string(m_adhocDescription.GetBuffer(0));
}


unsigned short AdhocMessageRecordPage::getMessageType()
{
    if (m_isDeleted)
    {
        return TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK;
    }
    else if ( NULL != m_recordingModel )
    {
        // Return emergency or normal as appropriate
        if ( m_recordingModel->isEmergencyMsg() )
        {
            return TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY;
        }
        else
        {
            return TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL;
        }
    }

    return TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK;
}


void AdhocMessageRecordPage::OnDelete() 
{
	TA_Base_Bus::TransActiveMessage userMsg;
	if (IDNO == userMsg.showMsgBox(IDS_UW_610019))
    {
        return;
    }

    if (m_recordingModel->isRecording())
    {
        return;
    }
    if (m_dvaPlaybackObject.isPlayingDvaMessage())
    {
        return;
    }
    
    if (PaRightsManager::getInstance().hasRight(PaRightsManager::PA_MODIFY_ADHOC_MESSAGE))
    {
        if ( m_stnBroadcastModel.postAdhocTypeChange(m_recordingModel->getMessageKey(), TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK) )
        {
            // Set flag to indicate changes made
            m_hasMessageTypeOrLabelBeenModified = true;

            setIsDeleted(true);

            updateButtonStates();

            addStringToStatusWindow(s_deleteMessageComplete);
        }
        else
        {
            addStringToStatusWindow(s_deleteMessageFailed);
        }
    }
    else
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
    }

}


void AdhocMessageRecordPage::OnHelp() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}


void AdhocMessageRecordPage::OnListen() 
{
	// jeffrey++ TES736
	m_isPlayBack = true;
	// ++jeffrey TES736

    if (m_dvaPlaybackObject.isPlayingDvaMessage())
    {
        // Already playing
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid state" );

        return;
    }

    try
    {    
        m_dvaPlaybackObject.playDvaMessage(m_recordingModel->getMessageKey(), *this);	    

        // Play operation successful, refresh dialog state
        updateButtonStates();
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached:
            PaErrorHandler::displayError(PaErrorHandler::MAXIMUM_BROADCAST_COUNT_REACHED);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        default:
            break;
        }
    }
    catch (const TA_Base_Core::CachedMappingNotFoundException& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", expWhat.what() );
    }
    catch (const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }
}


void AdhocMessageRecordPage::OnStartRecording() 
{
    if (!PaRightsManager::getInstance().hasRightToRecordDvaMessage())
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
        return;
    }

    if (m_recordingModel->isRecording())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Already recording" );

        return;
    }
  
    try
    {
        m_recordingModel->beginRecording(*this);

        if (m_recordingModel->isRecording())
        {
            updateButtonStates();
        }

        // Set flag to indicate changes made
        m_hasMessageTypeOrLabelBeenModified = true;
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached:
            PaErrorHandler::displayError(PaErrorHandler::MAXIMUM_BROADCAST_COUNT_REACHED);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            // The PAAgent returned an error code - treat this as meaning
            // perhaps the message we're recording over is in use?
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        default:
            break;
        }
    }
    catch (const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }
}


void AdhocMessageRecordPage::OnOK() 
{
    UpdateData(TRUE);

    if ( !m_recordingModel->isRecordingFailed() && !m_isDeleted && 0 == m_adhocDescription.GetLength())
    {
        PaErrorHandler::displayError(PaErrorHandler::EMPTY_DESCRIPTION_NOT_ALLOWED);
        return;
    }

	// Do not commit the name/type changes to the ad-hoc message
    // (the client code should do this)    
	CDialog::OnOK();
}


void AdhocMessageRecordPage::updateTimeField()
{
    updateTimeField(m_estimatedDuration);
}


time_t AdhocMessageRecordPage::getEstimatedDurationMilli()
{
	if ( false == isTimingMessage() )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not currently timing" );
		return 0;
	}
    return (clock() - m_startTime.getValue()) * 1000 / CLOCKS_PER_SEC;
}


void AdhocMessageRecordPage::updateTimeField(TA_Base_Core::PrimitiveWrapper<time_t>& estimatedDuration)
{
    // Save any current data (TD #2903: changes lost when clicked 'Listen' button)
    UpdateData(TRUE);

    if (estimatedDuration.hasBeenSet())
    {
        time_t seconds = estimatedDuration.getValue() / 1000;

        if ( !m_isPlayBack )
        {
            seconds = ( TIMER_COUNTDOWN_START_TIME > seconds ) ? ( TIMER_COUNTDOWN_START_TIME - seconds ) : 0u;
        }
        
        m_time.Format("%02dm:%02ds", seconds / 60, seconds % 60);
    }
    else
    {
        // Unable to determine (this occurs on initialisation of dialog, 
        // because we can't query the hardware to get this value)
        m_time = "N/A";
    }

    UpdateData(FALSE);
}


afx_msg LRESULT AdhocMessageRecordPage::OnProgressReportUpdate(UINT wParam, LONG lParam)
{
    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState> broadcastState;
    bool bStartTiming = false;
    bool isARecordProgressUpdate = false;

    try
    {
        if (lParam == reinterpret_cast<LONG>(&m_dvaPlaybackObject))
        {
            if (m_dvaPlaybackObject.processPlayDvaNotification(wParam, bStartTiming, &broadcastState))
            {
                // Playback complete, can refresh dialog state
                setMessageDurationCheckpoint(false);
                updateButtonStates();

				m_isPlayBack = false;
            }
        }
        else if (lParam == reinterpret_cast<LONG>(m_recordingModel))
        {
            isARecordProgressUpdate = true;

            if (m_recordingModel->processRecordAdhocNotification(wParam, bStartTiming, &broadcastState))
            {        
                onRecordingComplete();
            }
        }

        // Whenever a broadcast enters a runtime state, can begin timing it
        if (broadcastState.hasBeenSet())
        {
            if (TA_Base_Bus::IPAAgentCorbaDef::Runtime == broadcastState.getValue())
            {
                // First time we enter runtime, set timer checkpoint once off
                if (!isTimingMessage())
                {
                    if ( bStartTiming )
                    {
                        setMessageDurationCheckpoint(true);
                    }
                }
                else
                {
                    // Perform continuous update of time display as we go
                    updateTimeFieldWithEstimate();                   
                }
            }
            else if (TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval == broadcastState.getValue())
            {
                updateButtonStates();
            }
        }
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }  

    return 0;
}


void AdhocMessageRecordPage::onRecordingComplete()
{
    bool recordingAborted = false;

    if (!isTimingMessage())
    {
        // If the record timer hasn't been started yet, user must have
        // aborted before message recording begun
        recordingAborted = true;
    }

    // If the current state used to be deleted, it won't be
    // any more after having recorded a message (this is
    // the only way to achieve an undeleted state, after the
    // user hits delete)
    if ( m_recordingModel->isPttActivated() && m_isDeleted)
    {
        setIsDeleted(false);
    }

    setMessageDurationCheckpoint(false);

    updateButtonStates();

    // Indicate to user that recording is complete
    if ( m_recordingModel->isRecordingFailed() )
    {
        addStringToStatusWindow(s_recordingFailed);
    }
    else if (recordingAborted)
    {
        addStringToStatusWindow(s_recordingAborted);
    }
    else
    {
        addStringToStatusWindow(s_recordingComplete);
    }
}


bool AdhocMessageRecordPage::isTimingMessage()
{
    return m_startTime.hasBeenSet();
}


void AdhocMessageRecordPage::setMessageDurationCheckpoint(bool startTiming)
{
    if (startTiming)
    {
		if ( true == isTimingMessage() )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Already started timing" );
			return;
		}

        m_startTime.setValue(clock());
        // Clear old value
        m_estimatedDuration.resetValue();

        // Kick off the timed message (to keep time field updating)
        m_timeFieldTimer = SetTimer(TIMER_ID_TIME_FIELD, TIMER_REFRESH_MS, NULL);
    }
    else
    {
        if (isTimingMessage())
        {
            // Set estimated duration to time differential from last setpoint
            m_estimatedDuration.setValue(getEstimatedDurationMilli());

            // If the timer object had been created, release it upon termination
            if (0 != m_timeFieldTimer)
            {
                // Kill off the 'time field' timer
                BOOL result = KillTimer(m_timeFieldTimer);
                
                m_timeFieldTimer = 0;

                // If failure, means the timer wasn't started..
                ASSERT(result);
            }
        }
        else
        {
            // Issue raised by Sean, app used to ASSERT on this condition, but
            // if DVA playback ceases immediately after play, we won't actually
            // start timing (so allow for this condition)
            m_estimatedDuration.setValue(0);
        }

        // Reset m_startTime value so isTimingMessage() will return false again (be uninitialised)
        m_startTime.resetValue();

        // Refresh display
        updateTimeField();        
    }
}
       

void AdhocMessageRecordPage::OnDestroy() 
{
	try
	{
		destroyIcons();
	}
	catch ( ... )
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Icon destruction failed");
	}

    // Delete the rights change notification object before window handle becomes invalid    
    delete m_rightsChangeHandler;
    m_rightsChangeHandler = 0;
	
    // If user clicks OK while playing - that is fine, just ensure
    // playback is stopped in orderly manner
    stopDvaPlaybackIfActive();

    // As per TD #2316, user may abort while recording active (terminates it early)
    abortRecordingIfActive();

  	CDialog::OnDestroy();
}


void AdhocMessageRecordPage::stopDvaPlaybackIfActive()
{
    if (m_dvaPlaybackObject.isPlayingDvaMessage())
    {
        m_dvaPlaybackObject.stopDvaPlayback();
    }    
}


void AdhocMessageRecordPage::abortRecordingIfActive()
{
    if (m_recordingModel->isRecording())
    {
        // Only stop if currently recording
        m_recordingModel->stopRecording();
    }
}


void AdhocMessageRecordPage::updateButtonStates()
{
    bool enableStartRecording = true;
    bool enableListen = true;
    bool enableDelete = true;
    bool enableOk = true;
    bool enableCancel = true;
    bool enableStop = false;    // Only enabled during playback/recording

    if (m_dvaPlaybackObject.isPlayingDvaMessage())
    {
        // While a DVA playback is in progress, only
        // option user has is to close dialog
        enableStartRecording = false;
        enableListen = false;
        enableDelete = false;     
        enableStop = true;   
    }
    else if (m_recordingModel->isRecording())
    {
        // While recording, must wait until recording complete 
        // (ie user can do nothing)
        enableStartRecording = false;
        enableListen = false;
        enableDelete = false;
        enableOk = false;
        enableCancel = false;
        enableStop = true;
    }
    else
    {
        // Cannot listen or delete an already deleted message
        enableListen = !m_isDeleted;
        enableDelete = !m_isDeleted;
        enableStartRecording = m_isDeleted;
    }

    if ( m_recordingModel->isRecordingFailed())
    {
        //m_descriptionControl.EnableWindow(false);
        //m_typeControl.EnableWindow(false);
    }

    if (!PaRightsManager::getInstance().hasRightToRecordDvaMessage())
    {
        enableStartRecording = false;
    }

    if (!PaRightsManager::getInstance().hasRight( PaRightsManager::PA_MODIFY_ADHOC_MESSAGE ))
    {
        // Without the right to modify ad-hoc messages, disallow delete operation
        enableDelete = false;
    }

    m_startRecordingButton.EnableWindow(enableStartRecording);
    m_listenButton.EnableWindow(enableListen); 
    m_deleteButton.EnableWindow(enableDelete);
    m_okButton.EnableWindow(enableOk);
    m_cancelButton.EnableWindow(enableCancel);
    m_stopButton.EnableWindow(enableStop);
}


void AdhocMessageRecordPage::addStringToStatusWindow(const std::string& str)
{
    PaLayoutHelper::appendStringToStatusWindow(m_recordStatus, str);
}


void AdhocMessageRecordPage::OnCancel() 
{
    if ( m_isDeleted )
    {
        CDialog::OnCancel();
        return;
    }

    if ( m_hasMessageTypeOrLabelBeenModified )
    {
        // Confirm they want to ignore any changes
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UW_610020);
		if (IDNO == selectedButton)
        {         
            return;
        }
    }
	
    CDialog::OnCancel();
}


void AdhocMessageRecordPage::OnChangeEditDescription() 
{
    // Set flag to indicate changes made
    m_hasMessageTypeOrLabelBeenModified = true;	
}


void AdhocMessageRecordPage::OnCheckType() 
{
    bool bEmergency = ( 0 != m_typeControl.GetCheck() );

    if ( NULL != m_recordingModel )
    {
        m_recordingModel->setEmergencyMsg( bEmergency );
    }

    // Set flag to indicate changes made
    m_hasMessageTypeOrLabelBeenModified = true;	
}


bool AdhocMessageRecordPage::hasMessageTypeOrLabelBeenModified() 
{ 
    return m_hasMessageTypeOrLabelBeenModified; 
}

void AdhocMessageRecordPage::OnStop() 
{
    // The stop button won't elicit an immediate response on the dialog - it
    // will just terminate the broadcast (and the termination should come back
    // through as a progress report)
    try
    {
        if (m_dvaPlaybackObject.isPlayingDvaMessage())
        {
            m_dvaPlaybackObject.requestTermination();
        }
        if (m_recordingModel->isRecording())
        {
            m_recordingModel->requestTermination();
        }
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            PaErrorHandler::displayError(PaErrorHandler::BROADCAST_ID_NOT_FOUND);
            CDialog::OnCancel();
            break;
        default:
            break;
        }
    }
    catch(const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        // If there is an error, show error - shouldn't be able to get into this state
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }    
}


void AdhocMessageRecordPage::OnTimer( UINT nIDEvent )
{
    switch (nIDEvent)
    {
    case TIMER_ID_TIME_FIELD:
        // Update the time field (only if currently timing)
        if (isTimingMessage())
        {
            updateTimeFieldWithEstimate();
        }
        break;
    default:
        // Unrecognized timer message
        ASSERT(FALSE);
    }
}


void AdhocMessageRecordPage::updateTimeFieldWithEstimate()
{
    // Cannot give estimate if not timing
	if ( false == isTimingMessage() )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not currently timing" );
        return;
	}

    updateTimeField(TA_Base_Core::PrimitiveWrapper<time_t>(getEstimatedDurationMilli()) );
}


afx_msg LRESULT AdhocMessageRecordPage::OnRightsChanged(UINT wParam, LONG lParam)
{
    try
    {
        // Refresh the edit controls in response to right change
        refreshDescriptionTypeEnabledStates();
    
        // May need to restrict buttons upon a rights change (disable / re-enable etc)
        updateButtonStates();
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }   

    return 0;
}
