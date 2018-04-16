/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationDvaStatusDlg.cpp $
  * @author Jade Lee
  * @version $Revision: #10 $
  * Last modification: $DateTime: 2014/09/25 12:21:34 $
  * Last modified by: $Author: qi.huang $
  * 
  * StationDvaStatusDlg handles GUI component of DVA station broadcast status dialog
  */

#include <sstream>

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/StationDvaStatusDlg.h"
#include "app/pa/PAManager/src/DvaMessageChangeSubscriber.h"
#include "app/pa/PAManager/src/StationDvaStatusModel.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

StationDvaStatusDlg::StationDvaStatusDlg( bool isOcc, const std::string& broadcastId, 
    DvaMessageChangeSubscriber& dvaMessageChangeSubscriber, CWnd* pParent ) : 
DvaStatusPage( isOcc, StationDvaStatusDlg::IDD, pParent ),
m_dvaMessageChangeSubscriber(dvaMessageChangeSubscriber),
m_openingBroadcastId(broadcastId),
m_progressLst(),
m_failuresLst()
{
    //{{AFX_DATA_INIT(StationDvaStatusDlg)
	m_chimeText = _T("");
	m_dwellSecs = 0;
	m_periodSecs = _T("");
	m_startTimeText = _T("");
	m_endTimeText = _T("");
}


StationDvaStatusDlg::~StationDvaStatusDlg()
{
}


StationDvaStatusModel& StationDvaStatusDlg::getStationDvaStatusModel() 
{ 
    return castPointerToReference<StationDvaStatusModel>(m_stationStatusModel); 
}


GenericStatusModel* StationDvaStatusDlg::getStatusModelPointer()
{
	if ( NULL == m_stationStatusModel )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Status model null" );
	}  
    return m_stationStatusModel;
}


void StationDvaStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	DvaStatusPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StationDvaStatusDlg)
	DDX_Control(pDX, ID_STN_STATUS_RETRY_FAILURES_BTN, m_retryFailuresButton);
	DDX_Control(pDX, ID_STN_STATUS_RETRY_ALL_BTN, m_retryAllButton);
	DDX_Control(pDX, IDC_STATION_STATUS_BROADCAST_ID_TB, m_currentBroadcastIdTb);
	DDX_Control(pDX, IDC_STATION_STATUS_BROADCAST_ID_TB_NEW, m_newBroadcastIdTb);
	DDX_Control(pDX, IDC_STN_STATUS_PROGRESS_LST, m_progressLst);
	DDX_Control(pDX, IDC_STN_STATUS_FAILURES_LST, m_failuresLst);
    DDX_Control(pDX, ID_STN_STATUS_CLOSE_BTN, m_closeButton);
    DDX_Control(pDX, ID_STN_STATUS_ABORT_BTN, m_abortButton);
    DDX_Control(pDX, IDC_STATUS_CHECK_SUN, m_btnCycSun);
    DDX_Control(pDX, IDC_STATUS_CHECK_MON, m_btnCycMon);
    DDX_Control(pDX, IDC_STATUS_CHECK_TUE, m_btnCycTue);
    DDX_Control(pDX, IDC_STATUS_CHECK_WED, m_btnCycWed);
    DDX_Control(pDX, IDC_STATUS_CHECK_THU, m_btnCycThu);
    DDX_Control(pDX, IDC_STATUS_CHECK_FRI, m_btnCycFri);
    DDX_Control(pDX, IDC_STATUS_CHECK_SAT, m_btnCycSat);
	DDX_Text(pDX, IDC_STATION_STATUS_CHIME_TB, m_chimeText);
	DDX_Text(pDX, IDC_STATION_STATUS_DWELL_TB, m_dwellSecs);
	DDX_Text(pDX, IDC_STATION_STATUS_PERIOD_TB, m_periodSecs);
	DDX_Text(pDX, IDC_STATION_STATUS_START_TB, m_startTimeText);
	DDX_Text(pDX, IDC_STATION_STATUS_STOP_TB, m_endTimeText);
	DDX_Control(pDX, IDC_STATUS_TEXT, m_statusAreaText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StationDvaStatusDlg, DvaStatusPage)
	//{{AFX_MSG_MAP(StationDvaStatusDlg)
    ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
    ON_MESSAGE(WM_PA_DVA_MESSAGE_UPDATE, OnDvaMessageUpdate)
	ON_BN_CLICKED(ID_STN_STATUS_CLOSE_BTN, OnButtonClose)
    ON_BN_CLICKED(ID_STN_HELP, OnButtonHelp)
    ON_BN_CLICKED(ID_STN_STATUS_RETRY_FAILURES_BTN, OnButtonRetryFailures)
    ON_BN_CLICKED(ID_STN_STATUS_RETRY_ALL_BTN, OnButtonRetryAll)
    ON_BN_CLICKED(ID_STN_STATUS_ABORT_BTN, OnButtonAbort)
	ON_EN_CHANGE(IDC_STATION_STATUS_BROADCAST_ID_TB_NEW, OnChangeNewStatusName)
    ON_WM_DESTROY()
    ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL StationDvaStatusDlg::OnInitDialog() 
{
    m_stationStatusModel = new StationDvaStatusModel(*this, m_openingBroadcastId);

    DvaStatusPage::OnInitDialog();

    // Listen out for updates to any ad-hoc messages
    try
    {
        m_dvaMessageChangeSubscriber.addListenerWindow(getHWnd());
    }
    catch (const std::exception& expWhat)
    {
        // The ad-hoc list items will not be updated (nothing major, just can't view up to date info)
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }

    // Fill out contents of the messages to broadcast list once off - must be done AFTER
    // the subscription to ad-hoc updates (above) - to ensure up to date
    // (this means fillOutBroadcastConfiguration needs to be done last)
    fillOutBroadcastConfiguration();

	SetTimer(1, 1000, NULL);	// enable and set timer interval to 1 second

	m_newBroadcastIdTb.SetLimitText(112);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH StationDvaStatusDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    FUNCTION_ENTRY( "OnCtlColor" );

    HBRUSH hbr = DvaStatusPage::OnCtlColor(pDC, pWnd, nCtlColor);

    const int nWndResId = pWnd->GetDlgCtrlID();

    if (( IDC_STATIC_STATUS_CHECK_SUN == nWndResId ) || ( IDC_STATIC_STATUS_CHECK_SAT == nWndResId ))
    {
        if ( pWnd->IsWindowEnabled() )
        {
            pDC->SetTextColor(RGB(255, 0, 0));
        }
    }

    FUNCTION_EXIT;
    return hbr;
}


void StationDvaStatusDlg::fillOutBroadcastConfiguration()
{
    try
    {
        std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig> bc = m_stationStatusModel->getBroadcastConfig();

        TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& stationDvaConfig = bc->theBroadcastConfigSpecifics.theStationDvaConfig();
        
        m_chimeText = stationDvaConfig.hasChime ? "On" : "Off";   
        m_dwellSecs = stationDvaConfig.dwellInSecs;

        if ( !stationDvaConfig.bCyclic )
        {
            // Not a cyclical broadcast, no start / stop time defined
            m_startTimeText = "";
            m_endTimeText = "";

            m_btnCycSun.SetCheck( 0 );
            m_btnCycMon.SetCheck( 0 );
            m_btnCycTue.SetCheck( 0 );
            m_btnCycWed.SetCheck( 0 );
            m_btnCycThu.SetCheck( 0 );
            m_btnCycFri.SetCheck( 0 );
            m_btnCycSat.SetCheck( 0 );

            m_periodSecs = "";
        }
        else
        {
            m_startTimeText = GenericStatusModel::formatDateIntoString(stationDvaConfig.stCyclicParams.startTime);
            m_endTimeText = GenericStatusModel::formatDateIntoString(stationDvaConfig.stCyclicParams.stopTime);

            int nChecked = 0;
            nChecked = stationDvaConfig.stCyclicParams.bCycSun ? 1 : 0;
            m_btnCycSun.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycMon ? 1 : 0;
            m_btnCycMon.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycTue ? 1 : 0;
            m_btnCycTue.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycWed ? 1 : 0;
            m_btnCycWed.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycThu ? 1 : 0;
            m_btnCycThu.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycFri ? 1 : 0;
            m_btnCycFri.SetCheck( nChecked );
            nChecked = stationDvaConfig.stCyclicParams.bCycSat ? 1 : 0;
            m_btnCycSat.SetCheck( nChecked );

            m_periodSecs.Format( "%02d", CTime(stationDvaConfig.stCyclicParams.period).GetMinute() );
        }

        m_vecDvaMsgKeys.clear();
        std::stringstream stmMessages;
        for ( int nLoop = 0; nLoop < stationDvaConfig.messages.length(); ++nLoop )
        {
            unsigned long ulDvaMsgKey = stationDvaConfig.messages[nLoop];
            m_vecDvaMsgKeys.push_back( ulDvaMsgKey );
            try
            {
                TA_IRS_Bus::StationDvaMessageRecord stMsg = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( ulDvaMsgKey );
                stmMessages << stMsg.strMsgName << "\r\n";
            }
            catch (const TA_Base_Core::CachedMappingNotFoundException& expWhat)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", expWhat.what() );
            }
        }

        SetDlgItemText( IDC_STATUS_EDIT_DVA_MSG, stmMessages.str().c_str() );

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
        return;
    }
    catch (const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        return;
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        return;
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
        return;
    }
}


void StationDvaStatusDlg::OnButtonClose() 
{
    bool shouldCloseDialog = tryUpdateStatusNameOnClose();

    if (shouldCloseDialog)
    {
        removeAnnouncement();

        // May shut down window as per normal
        CDialog::OnCancel();
    }
}
           

void StationDvaStatusDlg::OnButtonHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}


void StationDvaStatusDlg::OnButtonRetryFailures()
{
    attemptOperation(RETRY_FAILURES);
}


void StationDvaStatusDlg::OnButtonRetryAll()
{
    attemptOperation(RETRY_ALL);
}


void StationDvaStatusDlg::OnButtonAbort()
{
    // Notify the PA Agent to stop playing messages on broadcast ID,
    // This does not exit the system, still must hit close to do that
    attemptOperation(TERMINATE_BROADCAST);
}


void StationDvaStatusDlg::OnChangeNewStatusName() 
{    
    // Just pass on to generic implementation
    DvaStatusPage::OnChangeNewStatusName();
}


void StationDvaStatusDlg::onCurrentStatusNameDeleted()
{
    DvaStatusPage::onCurrentStatusNameDeleted();

    // Disable all but close window
    m_retryAllButton.EnableWindow(FALSE);
    m_retryFailuresButton.EnableWindow(FALSE);
	m_newBroadcastIdTb.EnableWindow(FALSE);
    m_abortButton.EnableWindow(FALSE);
}


void StationDvaStatusDlg::destroyStatusModel()
{
    delete m_stationStatusModel;
    m_stationStatusModel = 0;
}


afx_msg LRESULT StationDvaStatusDlg::OnDvaMessageUpdate(UINT wParam, LONG lParam)
{
    if (!m_dvaMessageChangeSubscriber.isConnectedListenerWindow(getHWnd()))
    {
        // Probably in the closing down stage, receiving last few messages
        // while unsubscribed - do NOT respond to this notification in this case
        return 0;
    }

    try
    {
        const DvaMessageUpdateData updatedKeys = m_dvaMessageChangeSubscriber.getMessageData(wParam, getHWnd());

        bool bUpdated = false;
        for ( std::vector<unsigned long>::iterator itLoop = m_vecDvaMsgKeys.begin(); itLoop != m_vecDvaMsgKeys.end(); ++itLoop )
        {
            if ( updatedKeys.end() != updatedKeys.find( *itLoop ))
            {
                bUpdated = true;
                break;
            }
        }

        std::stringstream stmMessages;
        for ( std::vector<unsigned long>::iterator itKey = m_vecDvaMsgKeys.begin(); itKey != m_vecDvaMsgKeys.end(); ++itKey )
        {
            try
            {
                TA_IRS_Bus::StationDvaMessageRecord stMsg = 
                    TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( *itKey );
                stmMessages << stMsg.strMsgName << "\r\n";
            }
            catch (const TA_Base_Core::CachedMappingNotFoundException& expWhat)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", expWhat.what() );
            }
        }

        SetDlgItemText( IDC_STATUS_EDIT_DVA_MSG, stmMessages.str().c_str() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
    
    return 0;
}


void StationDvaStatusDlg::OnDestroy() 
{
    try
    {
        m_dvaMessageChangeSubscriber.removeListenerWindow(m_hWnd);
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

	DvaStatusPage::OnDestroy();	
}

// Test link status with PAAgent. If agent is down, throw exception.
// Then update broadcast status to "Unknown". Once the agent is back,
// request progress report update from agent.
void StationDvaStatusDlg::OnTimer(UINT nIDEvent) 
{
    static bool broadcastStateUnknownflag = false;	// indicate the link status of agent, true = disconnected.
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
	DvaStatusPage::OnTimer(nIDEvent);
}

LRESULT StationDvaStatusDlg::OnSysCommand( UINT wParam, LONG lParam )
{
    if (SC_CLOSE == wParam)
    {
        OnButtonClose();
    }
    return 0L;
}

void StationDvaStatusDlg::removeAnnouncement()
{
    FUNCTION_ENTRY( "removeAnnouncement" );

    CWaitCursor hourGlassCursor;

    if ( NULL != m_stationStatusModel )
    {
        m_stationStatusModel->removeAnnouncement();
    }

    FUNCTION_EXIT
}



