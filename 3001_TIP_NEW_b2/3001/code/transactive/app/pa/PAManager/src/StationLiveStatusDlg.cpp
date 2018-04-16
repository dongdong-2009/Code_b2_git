/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationLiveStatusDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/05/16 18:00:29 $
  * Last modified by:  $Author: qi.huang $
  *
  */
// StationLiveStatusDlg.cpp : implementation file
//

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/StationLiveStatusDlg.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "app/pa/PAManager/src/StationLiveStatusModel.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_Core;


const std::string StationLiveStatusDlg::s_pressPttPrompt = 
    "Press PTT on the PA paging console before speaking.";
const std::string StationLiveStatusDlg::s_releasePttPrompt = 
    "Release PTT on the PA paging console to terminate.";
// TD 17342
//const std::string StationLiveStatusDlg::s_broadcastComplete = 
//    "Broadcast complete.";
// TD 17342

/////////////////////////////////////////////////////////////////////////////
// StationLiveStatusDlg dialog
StationLiveStatusDlg::StationLiveStatusDlg( bool isOCC, 
    const std::string& broadcastId, CWnd* pParent ) : 
LiveStatusPage( isOCC,  StationLiveStatusDlg::IDD, pParent ),
m_openingBroadcastId(broadcastId),
m_progressLst(),
m_failuresLst()
{
    //{{AFX_DATA_INIT(StationLiveStatusDlg)
	//}}AFX_DATA_INIT
    m_chimeText = _T("");
	broadcastStateUnknownflag = false;
}


StationLiveStatusDlg::~StationLiveStatusDlg()
{
}


GenericStatusModel* StationLiveStatusDlg::getStatusModelPointer()
{ 
	if ( NULL == m_stationStatusModel )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Status model null" );
	}
    return m_stationStatusModel; 
}


StationLiveStatusModel& StationLiveStatusDlg::getStationLiveStatusModel() 
{ 
    return castPointerToReference<StationLiveStatusModel>(m_stationStatusModel); 
}


void StationLiveStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	LiveStatusPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StationLiveStatusDlg)
	DDX_Control(pDX, IDC_LIVE_STATUS, m_announceStatusControl);
	DDX_Control(pDX, ID_STN_STATUS_RETRY_FAILURES_BTN, m_retryFailuresButton);
	DDX_Control(pDX, ID_STN_STATUS_RETRY_ALL_BTN, m_retryAllButton);
	DDX_Control(pDX, IDC_STN_STATUS_PROGRESS_LST, m_progressLst);
	DDX_Control(pDX, IDC_STN_STATUS_FAILURES_LST, m_failuresLst);
  	DDX_Control(pDX, ID_STN_STATUS_CLOSE_BTN, m_closeButton);
    DDX_Text(pDX, IDC_STATION_STATUS_CHIME_TB, m_chimeText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StationLiveStatusDlg, LiveStatusPage)
	//{{AFX_MSG_MAP(StationLiveStatusDlg)
    ON_BN_CLICKED(ID_STN_STATUS_CLOSE_BTN, OnButtonClose)
    ON_BN_CLICKED(ID_STN_HELP, OnButtonHelp)
    ON_BN_CLICKED(ID_STN_STATUS_RETRY_FAILURES_BTN, OnButtonRetryFailures)
    ON_BN_CLICKED(ID_STN_STATUS_RETRY_ALL_BTN, OnButtonRetryAll)
    ON_WM_DESTROY()
	// jeffrey++ TD10377
	ON_WM_TIMER()
	// ++jeffrey TD10377
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL StationLiveStatusDlg::OnInitDialog() 
{
    m_stationStatusModel = new StationLiveStatusModel(*this, m_openingBroadcastId);

	LiveStatusPage::OnInitDialog();

    try
    {
        std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig> bc =
        m_stationStatusModel->getBroadcastConfig();

        TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig& stationLiveConfig = 
        bc->theBroadcastConfigSpecifics.theStationLiveConfig();

        m_chimeText = stationLiveConfig.hasChime ? "On" : "Off";

        // Dispatch internal variable updates to UI
        UpdateData(FALSE);
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            PaErrorHandler::displayError(PaErrorHandler::BROADCAST_ID_NOT_FOUND);
            break;
        default:
            break;
        }
        return FALSE;
    }
    catch (const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        return FALSE;
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        return FALSE;
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
        return FALSE;
    }
	// jeffrey++ TD10377
	SetTimer(1, 1000, NULL);	// enable and set timer interval to 1 second
	// ++jeffrey TD10377

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void StationLiveStatusDlg::onBroadcastStateChanged(const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState& newState, bool initialisation)
{
    LiveStatusPage::onBroadcastStateChanged(newState, initialisation);

    std::string statusWindowOutput;

    switch (newState)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::Creation:
        //Bohong++ for CL-20692
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Execution:
        statusWindowOutput = s_pressPttPrompt;
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
        // When in runtime stage, PA Agent has acknowledged PTT
        // and began broadcasting
        statusWindowOutput = s_releasePttPrompt;
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Aborting:
    case TA_Base_Bus::IPAAgentCorbaDef::Termination:
    case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
        // Broadcast complete
        // statusWindowOutput = s_broadcastComplete; // TD 17342
        break;
    }

    // Because two states may contribute to the same message (and just
    // in case we skip certain short term states like Termination)
    // We usually get two consecutive notifications for same message -
    // we only respond to the first of any such consecutive request
    if (!m_lastStatusString.hasBeenSetToValue(statusWindowOutput))
    {
        m_lastStatusString.setValue(statusWindowOutput);
        addStringToStatusArea(statusWindowOutput);
    }
}


void StationLiveStatusDlg::OnButtonClose() 
{
    removeAnnouncement();

    CDialog::OnCancel();
}
           

void StationLiveStatusDlg::OnButtonHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}


void StationLiveStatusDlg::OnButtonRetryFailures()
{
    attemptOperation(RETRY_FAILURES);
}


void StationLiveStatusDlg::OnButtonRetryAll()
{
    attemptOperation(RETRY_ALL);
}


void StationLiveStatusDlg::OnDestroy() 
{
	LiveStatusPage::OnDestroy();
}


void StationLiveStatusDlg::destroyStatusModel()
{
    delete m_stationStatusModel;
    m_stationStatusModel = 0;
}

// AZ++ TD10377 -- added check for broadcast ID not found
// jeffrey++ TD10377
// Test link status with PAAgent. If agent is down, throw exception.
// Then update broadcast status to "Unknown". Once the agent is back,
// request progress report update from agent.
void StationLiveStatusDlg::OnTimer(UINT nIDEvent) 
{
	try
	{
        CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), agentLinkTest, () );//limin
		if(broadcastStateUnknownflag)
		{
            try
            {
			    getStatusModel().forceProgressReportUpdate(false);
			    broadcastStateUnknownflag = false;
            }
            catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
                switch ( expWhat.eErrorId )
                {
                case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
                    {
                        broadcastStateUnknownflag = false;
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg.showMsgBox(IDS_UE_610019);
                        CDialog::OnCancel();
                        break;
                    }
                default:
                    break;
                }
            }
            catch (...)
            {
            }
		}
	}
	catch(...)
	{
        //if (CachedConfig::getInstance()->isAtOcc())
        //{
        //    for(int i=0;i<m_progressLst.GetItemCount();i++)
        //    {
        //        m_progressLst.SetItemText(i,2, "Unknown");
        //    }
        //} 
        //else 
		{
			for(int i=0;i<m_progressLst.GetItemCount();i++)
			{
				m_progressLst.SetItemText(i,1, "Unknown");
			}
		}
		broadcastStateUnknownflag = true;
	}
	LiveStatusPage::OnTimer(nIDEvent);
}
// ++jeffrey TD10377
// ++AZ

void StationLiveStatusDlg::removeAnnouncement()
{
    FUNCTION_ENTRY( "removeAnnouncement" );

    CWaitCursor hourGlassCursor;

    if ( NULL != m_stationStatusModel )
    {
        m_stationStatusModel->removeAnnouncement();
    }

    FUNCTION_EXIT
}
