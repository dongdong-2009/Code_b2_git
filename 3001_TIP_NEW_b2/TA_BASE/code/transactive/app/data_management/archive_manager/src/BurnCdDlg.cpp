/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/data_management/archive_manager/src/BurnCdDlg.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Dialog box to handle the CD Burn process.
  * This dialog is specific to CD-R and should be replaced with
  * a similar dialog if an alternative media is required.
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4503 4786 ) 
#endif // defined( WIN32 )

#include <sstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/BurnCdDlg.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "cots/Nero/inc/NeroAPIglue.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const std::string ARTIST = "Transactive";
}

namespace TA_Base_App
{
    
    BurnCdDlg::BurnCdDlg( const std::string& volumeLabel, const std::string& cdLabel, 
        const WriteArchiveHelper::FileDetailsVector& files, CWnd* pParent /*=NULL*/)
        : CDialog(BurnCdDlg::IDD, pParent),
          m_volumeLabel( volumeLabel ),
          m_cdLabel( cdLabel ),
          m_files( files )
    {
        //{{AFX_DATA_INIT(BurnCdDlg)
        m_csStage = _T("");
        m_csLog = _T("");
        //}}AFX_DATA_INIT
    }
        

    BurnCdDlg::~BurnCdDlg()
    {
        try
        {
            terminateAndWait();
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in destructor." );
        }
    }


    void BurnCdDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(BurnCdDlg)
	    DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	    DDX_Control(pDX, IDC_STAGE_STATIC, m_stageStatic);
	    DDX_Control(pDX, IDC_LOG_EDIT, m_editLog);
        DDX_Control(pDX, IDC_WRITE_PROGRESS, m_writeProgress);
        DDX_Text(pDX, IDC_STAGE_STATIC, m_csStage);
        DDX_Text(pDX, IDC_LOG_EDIT, m_csLog);
	    //}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(BurnCdDlg, CDialog)
        //{{AFX_MSG_MAP(BurnCdDlg)
	    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	    ON_MESSAGE(WM_CLOSE_DLG, OnCloseDlg)
    END_MESSAGE_MAP()
        

    void BurnCdDlg::run()
    {
        // Make sure no exceptions escape from the run() method.

        try
        {
            // Set the state to PROCESSING here. 
            // The state will be set to IDLE in the OnCloseDlg method.

            m_stateManager.setState( StateManager::PROCESSING );

            //  Display an hour-glass cursor.

            HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

            try
            {
                // Add all the files to be burnt to m_neroIsoItemsList.

                for( WriteArchiveHelper::FileDetailsVector::const_iterator it = m_files.begin(); it != m_files.end(); ++it )
                {
                    addFile( (*it).nameWithPath, (*it).name );
                }
              
                // Check that we do actually have files to write.  No point in setting 
                // up the hardware etc if there are no files to burn.  
        
                if( 0 == m_neroIsoItemsList.GetCount() )
                {
                    CString reason;
                    reason.LoadString( UE050090_REASON_NO_FILES );
                    CString error;
                    error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
                    
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << m_volumeLabel << reason;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091);
                    
                    TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, 
                        std::string( error ) ); 
                    PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                    return;
                }
       
                // Prepare for the write, includes setting up the hardware and the CD configuration.
                // The associated call to neroApiFree() is in OnDestroy(). 

                neroApiInit();

                // If the user has cancelled - don't go any further.

                if ( StateManager::STOPPING == m_stateManager.getState() )
                {
                    PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                    return;
                }

                // Write away...

                doBurn();

                // If the user has cancelled - don't go any further.

                if ( StateManager::STOPPING == m_stateManager.getState() )
                {
                    PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                    return;
                }
            
                // Write successful.

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_volumeLabel;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050046);

                PostMessage( WM_CLOSE_DLG, 0, IDOK );
                return;
            }
            catch ( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                //UINT selectedButton = userMsg.showMsgBox(IDS_UE_050130);
                
                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                return;
            }
            catch ( TA_Base_Core::TransactiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
                CString error;
                error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), e.what() );
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_volumeLabel << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091);

                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                return;
            }
            catch ( ... )
            {
                CString reason;
                reason.LoadString( UE050090_REASON_UNKNOWN_ERROR );
                CString error;
                error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
                
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", std::string( error ) );
                
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_volumeLabel << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091);

                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
               return;
            }

            // If the user has cancelled - deal with it.

            if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                return;
            }
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() ); 
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in BurnCdDlg::run()" ); 
        }
    }

    
    void BurnCdDlg::terminate()
    {
        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
        {
            // If we are in the middle of writing we have to wait for an appropriate
            // time before we terminate.

            if ( StateManager::PROCESSING == m_stateManager.getState() )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055);

                m_stateManager.setState( StateManager::STOPPING );
            }
            else if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                // We're still waiting for the current day to finish restoring.

                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055);
            }
        }
        
        // Wait until it has completed.
        // We could put a count in here so we don't get stuck, but how long should we count for?

        while( StateManager::IDLE != m_stateManager.getState() )
        {
            sleep( 1000 );  // 1 sec
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // BurnCdDlg message handlers
    
    BOOL BurnCdDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        //  Nero reports write/burn progress in percent.

        m_writeProgress.SetRange( 0, 100 );
        
        // Start writing.

        start();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    
    void BurnCdDlg::OnDestroy() 
    {
	    CDialog::OnDestroy();
	    
	    neroApiFree();

        while (!m_neroIsoItemsList.IsEmpty())
        {
            delete m_neroIsoItemsList.RemoveTail();
        }   
    }


    void BurnCdDlg::OnCancel() 
    {
        // If we are in the middle of processing, we have to wait until 
        // it is convenient to cancel.

        if ( StateManager::PROCESSING == m_stateManager.getState() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055);
            m_stateManager.setState( StateManager::STOPPING );

            appendStringToLogDisplay( "Cancelling..." );

            // Don't let the operator press Cancel again.

            m_buttonCancel.EnableWindow( FALSE );
        }
        else
        {
            // We're not processing, so we can just cancel.

            CDialog::OnCancel();
        }
    }

    
    LRESULT BurnCdDlg::OnCloseDlg(WPARAM, LPARAM lparam)
    {
        // If the user has cancelled - don't go any further.

        if ( StateManager::STOPPING == m_stateManager.getState() )
        {
            CString message;
            try
            {
                std::string operatorName = TA_ArchiveManager::getOperatorNameFromSessionId();
                std::string workstationName = TA_ArchiveManager::getWorkstationNameFromSessionId();

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << operatorName << workstationName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050030);
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050060);
            }
        }

        m_stateManager.setState( StateManager::IDLE );
        CDialog::EndDialog( lparam );
	    return NULL;	// This line must be included. Everything above is application specific
    }


//*********************************** NERO

    void BurnCdDlg::neroApiInit()
    {
        appendStringToLogDisplay("Opening NeroAPI.DLL");
        if ( !NeroAPIGlueConnect(NULL) )
        {
            CString reason;
            reason.LoadString( UE050090_REASON_DLL_OPEN_FAILED );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        appendStringToLogDisplay("Retrieving version information.");
        WORD majhi, majlo, minhi, minlo;
        NeroGetAPIVersionEx(&majhi, &majlo, &minhi, &minlo, NULL);
        std::stringstream strVersion;
        strVersion << "Nero API version " << majhi << "." << majlo << "." << minhi << "." << minlo;
        appendStringToLogDisplay( strVersion.str() );
              
        appendStringToLogDisplay("Filling NERO_SETTINGS structure");
        memset(&m_neroSettings, 0, sizeof(m_neroSettings));
        m_neroSettings.nstIdle.ncCallbackFunction = neroIdleCallback;
        m_neroSettings.nstIdle.ncUserData = this;
        m_neroSettings.nstUserDialog.ncCallbackFunction = neroUserDialog;
        m_neroSettings.nstUserDialog.ncUserData = this;
        m_neroSettings.nstNeroFilesPath = strdup( "NeroFiles");
        m_neroSettings.nstVendor = strdup( "ahead");
        m_neroSettings.nstSoftware = strdup( "Nero - Burning Rom");
        m_neroSettings.nstLanguageFile = strdup( "Nero.txt");
        
        memset(&m_neroProgress , 0, sizeof(m_neroProgress ));
        m_neroProgress.npAbortedCallback = neroAbortedCallback;
        m_neroProgress.npAddLogLineCallback = neroAddLogLine;
        m_neroProgress.npDisableAbortCallback = NULL;
        m_neroProgress.npProgressCallback = neroProgressCallback;
        m_neroProgress.npSetPhaseCallback = neroSetPhaseCallback;
        m_neroProgress.npSetMajorPhaseCallback=neroSetMajorPhaseCallback; // NULL;
        m_neroProgress.npSubTaskProgressCallback=NULL;
        m_neroProgress.npUserData = this;
        m_neroDeviceInfos = NULL;
        
        NEROAPI_INIT_ERROR initErr = NeroInit (&m_neroSettings, NULL);
        if (NEROAPI_INIT_OK != initErr)
        {
            CString reason;
            switch (initErr)
            {
            case NEROAPI_INIT_INVALID_ARGS:
                reason.LoadString( UE050090_REASON_INVALID_ARGS );
                break;
            case NEROAPI_INIT_INVALID_SERIAL_NUM:
                reason.LoadString( UE050090_REASON_INVALID_SER_NO );
                break;
            case NEROAPI_INIT_DEMOVERSION_EXPIRED:
                reason.LoadString( UE050090_REASON_EXPIRED );
                break;
            case NEROAPI_INIT_ALREADY_INITIALISED:
                reason.LoadString( UE050090_REASON_RUNNING );
                break;
            case NEROAPI_INIT_CANNOT_LOCK:
                reason.LoadString( UE050090_REASON_UNLOCKABLE );
                break;
            default:
                reason.FormatMessage( UE050090_REASON_UNKNOWN, initErr );
                break;
            }
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        
        appendStringToLogDisplay("Initialization of the NeroAPI successful.");
        
        m_neroDeviceInfos = NeroGetAvailableDrivesEx (MEDIA_CD, NULL);
        if (!m_neroDeviceInfos || (0 >= m_neroDeviceInfos->nsdisNumDevInfos) )
        {
            CString reason;
            reason.FormatMessage( UE050090_REASON_NO_DRIVES, "CD-RW" );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        
        appendStringToLogDisplay("Found the following devices:");
        std::stringstream stemp;
        NERO_SCSI_DEVICE_INFO* pNsdiWritable = 0;
        for (DWORD dDeviceCounter = 0; dDeviceCounter < m_neroDeviceInfos->nsdisNumDevInfos; dDeviceCounter++)
        {
            NERO_SCSI_DEVICE_INFO nsdi = m_neroDeviceInfos->nsdisDevInfos[dDeviceCounter];
            stemp.str( "" );
            stemp << nsdi.nsdiDriveLetter << ": " << nsdi.nsdiDeviceName << ", " << nsdi.nsdiHostAdapterName;
            stemp << " on " << nsdi.nsdiHostAdapterNo << ":" << nsdi.nsdiDeviceID << ", type ";
            stemp << nsdi.nsdiCapabilities << "(0x%X)";
            appendStringToLogDisplay( stemp.str() );
            if ( (NEA_SCSI_DEVTYPE_WORM == nsdi.nsdiDevType) &&
                 ( (int) nsdi.nsdiHostAdapterNo >= 0) )
            {
                //  Use this CDRW drive
                //  Note: the Nero ImageDrive (virtual CDRW drive) has adapterNo == -1
                pNsdiWritable = &m_neroDeviceInfos->nsdisDevInfos[dDeviceCounter];
                //  Could 'break' here if we didn't want to list the rest of the devices
            }
        }

        if ( 0 == pNsdiWritable )
        {
            CString reason;
            reason.FormatMessage( UE050090_REASON_NO_DRIVES, "CD-RW" );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        // Open a connection to the CD writing device.

        m_neroDeviceHandle = NeroOpenDevice( pNsdiWritable);
        if ( 0 == m_neroDeviceHandle )
        {
            CString reason;
            reason.FormatMessage( UE050090_REASON_OPEN_FAILED, pNsdiWritable->nsdiDeviceName );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
    }


    void BurnCdDlg::neroApiFree()
    {
        if (m_neroDeviceInfos)
        {
            NeroFreeMem(m_neroDeviceInfos);
        }
        NeroClearErrors();
        NeroDone();
        NeroAPIGlueDone();
    }


    void BurnCdDlg::doBurn()
    {
        appendStringToLogDisplay("Preparing to write CD...");
        if (m_neroIsoItemsList.IsEmpty())
        {
            CString reason;
            reason.LoadString( UE050090_REASON_NO_FILES );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }

        //  Build ISO item tree

        NERO_WRITE_CD neroWriteCD;
        ZeroMemory( &neroWriteCD, sizeof( NERO_WRITE_CD));
        neroWriteCD.nwcdMediaType = MEDIA_CD;
		// liborm 08/07/2004 - fix for TD defect #3707
		
        neroWriteCD.nwcdIsoTrack = NeroCreateIsoTrackEx( (NERO_ISO_ITEM*)m_neroIsoItemsList.GetHead(), 
            m_volumeLabel.c_str(), NCITEF_CREATE_ISO_FS | NCITEF_USE_JOLIET);
        neroWriteCD.nwcdArtist = ARTIST.c_str();

        // KT 10 Mar 04: PW #3136 Added reformatted volume label for CD (CDLabel).

        neroWriteCD.nwcdTitle = m_cdLabel.c_str();

        // Burn the CD
        // NBF_SIMULATE = Simulate writing before actually doing it.  This reduces the chances of errors that waste CDs.
        // NBF_WRITE = Actually do the write after simulating.
        // NBF_BUF_UNDERRUN_PROT = Underrun protection enables a safer burn mode.
        // NBF_DETECT_NON_EMPTY_CDRW = Detects a non empty CDRW and calls neroUserDialog with DLG_NON_EMPTY_CDRW.
        // NBF_VERIFY = Verifies the filesystem after writing (only works for ISO).

        int iRes = NeroBurn( m_neroDeviceHandle, NERO_ISO_AUDIO_CD, &neroWriteCD, 
            NBF_SIMULATE | NBF_WRITE | NBF_BUF_UNDERRUN_PROT | NBF_DETECT_NON_EMPTY_CDRW | NBF_VERIFY, 
            0, &m_neroProgress);

        // Clean up

        NeroFreeIsoTrack( neroWriteCD.nwcdIsoTrack );
        NeroCloseDevice( m_neroDeviceHandle);

        // If the user has cancelled don't tell them what
        // happened during the write.

        if ( StateManager::STOPPING == m_stateManager.getState() )
        {
            m_buttonCancel.SetWindowText( "Close" );
            return;
        }
        
        // Don't let the user cancel from here on.  Otherwise they
        // might not get the message about labelling the CD.

        m_buttonCancel.EnableWindow( FALSE );

        // Report what happened
        
        appendStringToLogDisplay( NeroGetErrorLog() );

        // Tell the user how the burn process was finished

        CString reason( "" );
        switch(iRes)
        {
        case NEROAPI_BURN_OK:
            reason = "Write successful";
            break;
        case NEROAPI_BURN_UNKNOWN_CD_FORMAT:
            reason.LoadString( UE050090_REASON_UNKNOWN_CD_FORMAT );
            break;
        case NEROAPI_BURN_INVALID_DRIVE:
            reason.LoadString( UE050090_REASON_INVALID_DRIVE );
            break;
        case NEROAPI_BURN_FAILED:
            reason.LoadString( UE050090_REASON_WRITE_FAILED );
            break;
        case NEROAPI_BURN_FUNCTION_NOT_ALLOWED:
            reason.LoadString( UE050090_REASON_FN_NOT_ALLOWED );
            break;
        case NEROAPI_BURN_DRIVE_NOT_ALLOWED:
            reason.LoadString( UE050090_REASON_DRIVES_NOT_ALLOWED );
            break;
        default:
            reason.LoadString( UE050090_REASON_UNKNOWN_ERROR );
            break;
        }
        appendStringToLogDisplay ( std::string( reason ) );

        m_buttonCancel.SetWindowText( "Close" );
        m_buttonCancel.EnableWindow( TRUE );

        if ( NEROAPI_BURN_OK == iRes )
        {
            m_stageStatic.SetWindowText( "CD Write Successful" );
        }
        else
        {
            m_stageStatic.SetWindowText( "CD Write Failed" );
            CString error;
            error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
    }


    void NERO_CALLBACK_ATTR BurnCdDlg::neroAddLogLine(void *pUserData, NERO_TEXT_TYPE type, const char *text)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process Nero add log line callback." );
        }
        else
        {
            std::string log( "Log: " + std::string( text ) );
            dlg->appendStringToLogDisplay( log );
        }
    }


    void NERO_CALLBACK_ATTR BurnCdDlg::neroSetPhaseCallback(void *pUserData, const char *text)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process Nero set phase callback." );
            return;
        }
        else
        {
            dlg->m_stageStatic.SetWindowText( text );
        }
    }


    void NERO_CALLBACK_ATTR BurnCdDlg::neroSetMajorPhaseCallback(void *pUserData, NERO_MAJOR_PHASE phase,void *reserved)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process Nero set major phase callback." );
            return;
        }
        else
        {
            std::string log( "Major phase: ");
            switch( phase )
            {
                case NERO_PHASE_UNSPECIFIED:
                    log += "Unspecified";
                    break;
                case NERO_PHASE_START_CACHE:
                    log += "Start cache";
                    break;
                case NERO_PHASE_DONE_CACHE:
                    log += "Done cache";
                    break;
                case NERO_PHASE_START_TEST:
                    log += "Start test";
                    break;
                case NERO_PHASE_DONE_TEST:
                    log += "Done test";
                    break;
                case NERO_PHASE_START_SIMULATE:
                    log += "Start simulate";
                    break;
                case NERO_PHASE_DONE_SIMULATE:
                    log += "Done simulate";
                    break;
                case NERO_PHASE_START_WRITE:
                    log += "Start write";
                    break;
                case NERO_PHASE_DONE_WRITE:
                    log += "Done write";
                    break;
                case NERO_PHASE_START_SIMULATE_NOSPD:
                    log += "Start simulate nospd";
                    break;
                case NERO_PHASE_DONE_SIMULATE_NOSPD:
                    log += "Done simulate nospd";
                    break;
                case NERO_PHASE_START_WRITE_NOSPD:
                    log += "Start write nospd";
                    break;
                case NERO_PHASE_DONE_WRITE_NOSPD:
                    log += "Done write nospd";
                    break;
                case NERO_PHASE_ENCODE_VIDEO:
                    log += "Encode video";
                    break;
                case NERO_PHASE_SEAMLESSLINK_ACTIVATED:
                    log += "Seamless link activated";
                    break;
                case NERO_PHASE_BUP_ACTIVATED:
                    log += "BUP activated";
                    break;
                case NERO_PHASE_CONTINUE_FORMATTING:
                    log += "Continue formatting";
                    break;
                case NERO_PHASE_FORMATTING_SUCCESSFUL:
                    log += "Formatting successful";
                    break;
                case NERO_PHASE_DVDVIDEO_DETECTED:
                    log += "DVD video detected";
                    break;
                case NERO_PHASE_DVDVIDEO_REALLOC_STARTED:
                    log += "DVD video reallocation started";
                    break;
                case NERO_PHASE_DVDVIDEO_REALLOC_COMPLETED:
                    log += "DVD video reallocation completed";
                    break;
                case NERO_PHASE_DVDVIDEO_REALLOC_NOTNEEDED:
                    log += "DVD video reallocation not needed";
                    break;
                case NERO_PHASE_DVDVIDEO_REALLOC_FAILED:
                    log += "DVD video reallocation failed";
                    break;
                default:
                    log += "Unknown";
                    break;
            }
            dlg->appendStringToLogDisplay( log );
        }
    }


    NeroUserDlgInOut NERO_CALLBACK_ATTR BurnCdDlg::neroUserDialog(void *pUserData, NeroUserDlgInOut type, void *data)
    {
        switch (type)
        {
            case DLG_AUTO_INSERT:
            case DLG_DISCONNECT:
            case DLG_SETTINGS_RESTART:

                return DLG_RETURN_CONTINUE;
                break;

            case DLG_DISCONNECT_RESTART:

                return DLG_RETURN_ON_RESTART;
                break;

            case DLG_OVERBURN:

                return DLG_RETURN_TRUE;
                break;

           
            case DLG_FILESEL_IMAGE:
                {
                    static char BASED_CODE szFilter[]( "Image Files (*.nrg)|*.nrg|All Files (*.*)|*.*||" );
                    TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
                    if ( dlg == 0 )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                            "Failed to cast void* to BurnCdDlg*. Can't process Nero user dialog callback." );
                    }
                    else
                    {
                       CFileDialog dlgOpen(TRUE, NULL, "test.nrg", OFN_OVERWRITEPROMPT, szFilter, dlg);
                        if (dlgOpen.DoModal() == IDOK)
                        {
                            strcpy((char*)data,dlgOpen.GetPathName());
                            return DLG_RETURN_TRUE;
                        }
                    }
                    return DLG_BURNIMAGE_CANCEL;
                    break;
                }
            case DLG_WAITCD:
            case DLG_NON_EMPTY_CDRW:
                {
					static bool flag = true;
                    TA_Base_Bus::TransActiveMessage userMsg;     
					if (flag && IDCANCEL == userMsg.showMsgBox(IDS_UE_050081) )
                    {
                        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
                        if ( dlg == 0 )
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                                "Failed to cast void* to BurnCdDlg*. Can't process Nero user dialog callback." );
                        }
                        else
                        {
                            dlg->m_stateManager.setState( StateManager::STOPPING );
                        }
                    }
					else
					{
						flag ^= true;
					}
                    return DLG_RETURN_EXIT;
                    break;
                }
            case DLG_WAITCD_REMINDER:
            case DLG_AUTO_INSERT_RESTART:
            case DLG_RESTART:
            case DLG_AUDIO_PROBLEMS:
            case DLG_WAITCD_DONE:
            default:

                return DLG_RETURN_EXIT;
                break;
        }
        return DLG_RETURN_EXIT;
    }


    BOOL NERO_CALLBACK_ATTR BurnCdDlg::neroIdleCallback(void *pUserData)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process BurnCdDlg UI messages." );
            return false;  // Assume we're not aborting.
        }
         
        bool isAborting = false;
        if ( StateManager::STOPPING == dlg->m_stateManager.getState() )
        {
            isAborting = true;
        }
        static MSG msg;
        while ( ::PeekMessage(&msg,NULL,NULL,NULL,PM_NOREMOVE) && !isAborting )
        {
            if (!AfxGetThread()->PumpMessage())
            {
                break;
            }
        }
        return isAborting;
    }


    BOOL NERO_CALLBACK_ATTR BurnCdDlg::neroProgressCallback(void *pUserData, DWORD dwProgressInPercent)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process Nero progress callback." );
            return false;  // Assume we're not aborting.
        }
         
        dlg->m_writeProgress.SetPos( dwProgressInPercent );
        if ( StateManager::STOPPING == dlg->m_stateManager.getState() )
        {
            return true;
        }
        return false;
    }


    BOOL NERO_CALLBACK_ATTR BurnCdDlg::neroAbortedCallback(void *pUserData)
    {
        TA_Base_App::BurnCdDlg* dlg = static_cast< TA_Base_App::BurnCdDlg* >( pUserData );
        if ( dlg == 0 )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to cast void* to BurnCdDlg*. Can't process Nero aborted callback." );
            return false;  // Assume we're not aborting.
        }
        if ( StateManager::STOPPING == dlg->m_stateManager.getState() )
        {
            return true;
        }
        return false;
    }


    void BurnCdDlg::addFile( const std::string& fileNameWithPath, const std::string& fileName, bool isDirectory )
    {
        //  Call addFile() for every file that will be burnt to CD
        //  Limitations:
        //  -   only writes a single directory layer (NERO_ISO_ITEM.subDirFirstItem not used)
        //  -   only writes to CD's root directory
        //  -   writes in the order you call this routine
        //
        //  If fixing these later, the ISO item tree needs to be set up as:
        //
        //  Ptr     Sourcename          Destname            nextItem    subDirFirstItem   
        //
        //  <1>     c:\dirOne\FileA.1   \dirOne\FileA.1     <2>         NULL
        //  <2>     c:\dirOne\FileB.2   \dirOne\FileB.2        
        //  <3>     c:\dirOne\FileC.3   \dirOne\FileC.3        
        //  <4>     c:\dirTwo\FileD.4   \dirTwo\FileD.4        

        // First check that the file/directory exists.

        if ( isDirectory )
        {
            if ( !TA_ArchiveManager::isValidDirectory( fileNameWithPath ) )
            {
                CString reason;
                reason.LoadString( UW050004_REASON_INVALID_DIR );
                CString error;
                error.FormatMessage( UW050004_ADD_DIR_FAILED, fileNameWithPath.c_str(), reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }
        else // We're expecting a file.
        {
            if ( !TA_ArchiveManager::isValidFile( fileNameWithPath ) )
            {
                CString reason;
                reason.LoadString( UW050016_REASON_INVALID_FILE );
                CString error;
                error.FormatMessage( UW050016_ADD_FILE_FAILED, fileNameWithPath.c_str(), reason );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }

        // We're not using these because they don't work:
        // pNii = NeroCreateIsoItem();
        // pNii = NeroCreateIsoItemOfSize( sizeof( NERO_ISO_ITEM) );

        NERO_ISO_ITEM* pNii = new NERO_ISO_ITEM;
        if ( pNii == 0 )
        {
            CString reason;
            reason.LoadString( UW050004_UW050016_REASON_MEMORY );
            CString error;
            if ( isDirectory )
            {
                error.FormatMessage( UW050004_ADD_DIR_FAILED, fileNameWithPath.c_str(), reason );
            }
            else
            {
                error.FormatMessage( UW050016_ADD_FILE_FAILED, fileNameWithPath.c_str(), reason );
            }
            TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
        }
        ZeroMemory( pNii, sizeof( NERO_ISO_ITEM ) );
        pNii->itemSize = sizeof( NERO_ISO_ITEM );
        pNii->isReference = false;
        pNii->isDirectory = isDirectory;
        strcpy( pNii->sourceFilePath, fileNameWithPath.c_str() );
        strcpy( pNii->fileName, fileName.c_str() );

        std::string s( "File: " + std::string( pNii->fileName ));
        appendStringToLogDisplay( s.c_str() );

        NERO_ISO_ITEM* pPrevNii = 0;
        if ( !m_neroIsoItemsList.IsEmpty() )
        {
            pPrevNii = (NERO_ISO_ITEM*) m_neroIsoItemsList.GetTail();
        }
        m_neroIsoItemsList.AddTail( pNii);

        if ( 0 != pPrevNii )
        {
            pPrevNii->nextItem = pNii;
        }
    }
    

    //*************************** END NERO


    void BurnCdDlg::appendStringToLogDisplay( const std::string& sAdd )
    {
        //  Add the string to the active Windows Edit control if it is usable, otherwise
        //  just add it to the local string that will be DDX'd into the edit control.

        if ( ::IsWindow( m_editLog.m_hWnd) )
        {
            CString sBuffer;
            m_editLog.GetWindowText( sBuffer );     // This method requires a CString.
            if (!sBuffer.IsEmpty())
            {
                sBuffer+= "\r\n";
            }
            sBuffer+= sAdd.c_str();
            m_editLog.SetWindowText( sBuffer );
            m_editLog.LineScroll( m_editLog.GetLineCount(),0 );
        }
        else
        {
            if (!m_csLog.IsEmpty() )
            {
                m_csLog+= "\r\n";
            }
            m_csLog+= sAdd.c_str();
        }
    }

} // TA_Base_App
