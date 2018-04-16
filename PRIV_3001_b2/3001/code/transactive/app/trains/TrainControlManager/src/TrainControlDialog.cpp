/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "app/trains/TrainControlManager/src/StdAfx.h"
#include "app/trains/TrainControlManager/src/TrainControlDialog.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/public_interfaces/ItaRights.h"

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


#include <sstream>
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

   /* TrainControlDialog::TrainControlDialog( TA_Base_Bus::IGUIAccess& genericGUICallback,
                                            TA_Base_Bus::ItaRights& taRights,
                                            CWnd* pParent )*/
	IMPLEMENT_DYNAMIC(TrainControlDialog, ITrainBorne)
	TrainControlDialog::TrainControlDialog(CWnd* pParent )
        : ITrainBorne(TrainControlDialog::IDD, pParent ),
         // m_rights( taRights ),
          m_canTakeControl( false ),
          m_trainAgentKey( 0 )

    {
        FUNCTION_ENTRY( "TrainControlDialog" );

        //m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		//initialise();

        FUNCTION_EXIT;
    }


    TrainControlDialog::~TrainControlDialog()
    {
        FUNCTION_ENTRY( "~TrainControlDialog" );

        // shared_ptr m_trainInformationProxy and  m_trainOperationModeProxy release automatically;

        // cleaup train details
        m_trainDetailsStore.clear();

        FUNCTION_EXIT;
    }


    void TrainControlDialog::initialise()
    {
		//hongzhi, set corba timeout to 5 seconds to solve train list disappear sometimes
		TA_Base_Core::RunParams::getInstance().set(RPARAM_RESOLVE_TIMEOUT_MS,"5000");

        // register for runparam changes to session ID (RPARAM_SESSIONID)
        TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, RPARAM_SESSIONID );

        // create the proxies
        createProxies();

        // populate the initial map
        getTrainsAndDetails();

        // check for access control
        checkRights();

        EnableWindow( TRUE );
    }


    void TrainControlDialog::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(TrainControlDialog)
       DDX_Control( pDX, IDC_TC_CB_TRAINID, m_trainList );
        DDX_Control( pDX, IDC_TC_ED_TSI2, m_secondaryTSI );
        DDX_Control( pDX, IDC_TC_ED_TSI1, m_primaryTSI );
        DDX_Control( pDX, IDC_TC_ED_STATUS, m_resultDisplay );
         DDX_Control( pDX, IDC_TC_BU_TAKECONTROL, m_takeControl );
        DDX_Control( pDX, IDC_TC_BU_CLOSE, m_closeButton );
		DDX_Control( pDX, IDC_TC_LT_TRAIN_LIST, m_listTrainCtrl);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( TrainControlDialog, CDialog )
        //{{AFX_MSG_MAP(TrainControlDialog)
        ON_BN_CLICKED( IDC_TC_BU_CLOSE, OnClose )
        ON_WM_DESTROY()
        ON_BN_CLICKED( IDC_TC_BU_TAKECONTROL, OnTakeControl )
        ON_CBN_SELENDOK( IDC_TC_CB_TRAINID, OnSelectTrain )
		ON_NOTIFY(NM_CLICK,IDC_TC_LT_TRAIN_LIST,&TrainControlDialog::OnNMTrainSelected)
        ON_MESSAGE( WM_ON_UPDATE_TRAIN, OnUpdateTrain )
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()



    void TrainControlDialog::onRunParamChange( const std::string& name, const std::string& value )
    {
        FUNCTION_ENTRY( "onRunParamChange" );

        if ( name == RPARAM_SESSIONID )
        {
            // check the access control
            checkRights();

            // re-evaluate button states
            enableTakeControlButton();
        }

        FUNCTION_EXIT;
    }


    void TrainControlDialog::processTrainDetails( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& event )
    {
        FUNCTION_ENTRY( "processTrainDetails" );

        TA_THREADGUARD( m_trainDetailsLock );

        // find the entry
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator  findIter = m_trainDetailsStore.find( event.trainId );

        if ( findIter == m_trainDetailsStore.end() )
        {
            // not found, add it
            m_trainDetailsStore.insert( TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::value_type( event.trainId, event ) );
        }
        else
        {
            // found, update it
            findIter->second = event;
        }

        // send a message to indicate the train details were updated
        PostMessage( WM_ON_UPDATE_TRAIN, static_cast<WPARAM>( event.trainId ), 0 );

        FUNCTION_EXIT;
    }


    void TrainControlDialog::processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event )
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );
        FUNCTION_EXIT;
    }

    
    void TrainControlDialog::processTrainSelectionUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& event )
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );
        FUNCTION_EXIT;
    }


    BOOL TrainControlDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        /*CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = windowSize.bottom - windowSize.top;
        properties.maxWidth = windowSize.right - windowSize.left;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        setResizingProperties(properties);*/

       // ITrainBorne::OnInitDialog();
		__super::OnInitDialog();
        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        //SetIcon(m_hIcon, TRUE);            // Set big icon
        //SetIcon(m_hIcon, FALSE);        // Set small icon

        EnableWindow( FALSE );

        // disable the m_takeControlButton
        m_takeControl.EnableWindow( FALSE );

		//set the ClistCtrl column names 
		m_listTrainCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		columnNamesList();
		initialise();
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
    }

	void TrainControlDialog::columnNamesList()
	{
		FUNCTION_ENTRY( "columnNamesList" );
		
		std::map<int,LPSTR> ColumnName;
		ColumnName[0] = "TrainID/Service";
		ColumnName[1] = "RadioID1";
		ColumnName[2] = "RadioID2";

		for (int x=0; x<3;x++)
		{
			m_listTrainCtrl.InsertColumn(x,ColumnName[x],LVCFMT_LEFT,100);
		}
		
		FUNCTION_EXIT;
	}

    void TrainControlDialog::createProxies()
    {
        FUNCTION_ENTRY( "createProxies" );

        // initalise the proxy factory if not already
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().guiInitialise(
            TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );

        m_trainInformationProxy =
            TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy() ;

        m_trainOperationModeProxy =
            TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainOperationModeCorbaProxy();

        // register for train information updates
        m_trainInformationProxy->addObserver( this );

        FUNCTION_EXIT;
    }


    void TrainControlDialog::getTrainsAndDetails()
    {
        FUNCTION_ENTRY( "getTrainsAndDetails" );

        try
        {
            TA_THREADGUARD( m_trainDetailsLock );

            // get the train list
            TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainList = m_trainInformationProxy->getTrainList();

            // populate the map and the drop down list
            for ( TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator trainIter = trainList.begin();
                  trainIter != trainList.end(); ++trainIter )
            {
                m_trainDetailsStore.insert(
                    TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::value_type( trainIter->trainId, *trainIter ) );

                //int position = m_trainList.AddString( trainIter->serviceNumber.c_str() );
                //m_trainList.SetItemData( position, static_cast<DWORD>( trainIter->trainId ) );
				
				//CListCtrl
				int nIndex = m_listTrainCtrl.InsertItem(LVIF_TEXT|LVIF_STATE,trainIter->trainId-1,trainIter->serviceNumber.c_str(),0,0,0,NULL);
				m_listTrainCtrl.SetItemText(nIndex,1,trainIter->primaryTsi.c_str());
				m_listTrainCtrl.SetItemText(nIndex,2,trainIter->secondaryTsi.c_str());
            }
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            // if the agent is not running now, the list will be populated when it starts
        }

        FUNCTION_EXIT;
    }


    void TrainControlDialog::checkRights()
    {
        FUNCTION_ENTRY( "checkRights" );

        if ( 0 == m_trainAgentKey )
        {
            unsigned long locationKey = 0;

            std::istringstream locationKeyStream( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ) );
            locationKeyStream >> locationKey;

            TA_ASSERT( locationKey != 0, "Location Key is not set" );

            TA_Base_Core::IEntityDataList trainAgentEntityList =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                        TA_Base_Core::TrainAgentEntityData::getStaticType(), locationKey );

            TA_ASSERT( trainAgentEntityList.size() == 1, "There must be exactly one train agent at each location" );

            m_trainAgentKey = trainAgentEntityList[0]->getKey();

            delete trainAgentEntityList[0];
            trainAgentEntityList.clear();
        }

        std::string reason;
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        /*m_canTakeControl = m_rights.isActionPermittedOnResource( sessionId,
                                                                 m_trainAgentKey,
                                                                 TA_Base_Bus::aca_TRAIN_MANUAL_TAKE_CONTROL,
                                                                 reason );

        if ( false == m_canTakeControl )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Can not take control of trains, reason: %s",
                         reason.c_str() );
        }*/

        FUNCTION_EXIT;
    }


    void TrainControlDialog::enableTakeControlButton()
    {
        FUNCTION_ENTRY( "enableTakeControlButton" );

        // see if there is something selected
        //int currentSelection = m_trainList.GetCurSel();
		int currentSelection = m_listTrainCtrl.GetSelectionMark();

        // crappy MFC bool
        BOOL enableButton = FALSE;

        if ( ( true == m_canTakeControl ) &&
             ( CB_ERR != currentSelection ) )
        {
            enableButton = TRUE;
        }

        m_takeControl.EnableWindow( enableButton );

        FUNCTION_EXIT;
    }

    
    void TrainControlDialog::updateTrainDetails()
    {
        FUNCTION_ENTRY( "updateTrainDetails" );

        //int currentSelection = m_trainList.GetCurSel();
		int currentSelection = m_listTrainCtrl.GetSelectionMark();

		

        if ( CB_ERR != currentSelection )
        {
            // something is selected
            
            // get the train ID from the item data
            DWORD selectedItemData = m_trainList.GetItemData( currentSelection );
		
			CString str = m_listTrainCtrl.GetItemText(currentSelection,0);
		
			

			//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			//	"upddateTrainDetails: GetItemData : %l ",selectedItemData);

            TA_IRS_Bus::CommonTypes::TrainIdType trainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( selectedItemData );

            // find the train details
            TA_THREADGUARD( m_trainDetailsLock );

            TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetailsStore.find( trainId );

            if ( m_trainDetailsStore.end() != findIter )
            {
                m_primaryTSI.SetWindowText( ( findIter->second.primaryTsi ).c_str() );
                m_secondaryTSI.SetWindowText( ( findIter->second.secondaryTsi ).c_str() );

                FUNCTION_EXIT;
                return;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Cannot find train details for selected item %d, trainId %d",
                             currentSelection, trainId );
            }
        }

        // nothing is selected
        // or there was an error retrieving data
        m_primaryTSI.SetWindowText( "" );
        m_secondaryTSI.SetWindowText( "" );

        FUNCTION_EXIT;
    }

	int TrainControlDialog::getTrainIDSelected(CString str)
	{
		FUNCTION_ENTRY("getTrainIDSelected");

		std::string::size_type uPos = str.Find("/");
		CString strTarget = str.Left(uPos);
		int trainId = _ttoi(strTarget);
		return trainId;

		FUNCTION_EXIT;
	}

    void TrainControlDialog::OnClose()
    {
        FUNCTION_ENTRY( "OnClose" );

        ITrainBorne::OnClose();

        FUNCTION_EXIT;
    }


    void TrainControlDialog::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        // unsubscribe for train list updates
        m_trainInformationProxy->removeObserver( this );

        // deregister for ruparam changes
        TA_Base_Core::RunParams::getInstance().deregisterRunParamUser( this );

        ITrainBorne::OnDestroy();

        FUNCTION_EXIT;
    }


    LRESULT TrainControlDialog::OnUpdateTrain( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "OnUpdateTrain" );

        // wParam contains the train ID that was updated
        TA_IRS_Bus::CommonTypes::TrainIdType updatedTrainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( wParam );

        // get the current list selected item
        TA_IRS_Bus::CommonTypes::TrainIdType selectedTrainId = 0;

        {
            // get the details of the train
            TA_THREADGUARD( m_trainDetailsLock );
        
            TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetailsStore.find( updatedTrainId );

            TA_ASSERT( m_trainDetailsStore.end() != findIter, "This was just added before the message was sent" );

            //int currentSelection = m_trainList.GetCurSel();
			int currentSelection = m_listTrainCtrl.GetSelectionMark();
			CString strTrainId;

            if ( CB_ERR != currentSelection )
            {
                // something is selected
                // get the train ID from the item data
                //DWORD selectedItemData = m_trainList.GetItemData( currentSelection );
				strTrainId = m_listTrainCtrl.GetItemText(currentSelection,0);
				DWORD selectedItemData = getTrainIDSelected(strTrainId);

                selectedTrainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( selectedItemData );
            }

            // find the train in the list box
            int updatedPosition = CB_ERR;
			
            //for ( int i = 0; i < m_trainList.GetCount(); ++i )
			for( int i = 0; i < m_listTrainCtrl.GetItemCount(); ++i)
            {
                // get the train id this item is for
                //DWORD itemData = m_trainList.GetItemData( i );
				CString strTrain = m_listTrainCtrl.GetItemText(i,0);
				DWORD itemData = getTrainIDSelected(strTrain);

                TA_IRS_Bus::CommonTypes::TrainIdType itemTrainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( itemData );

                // if it matches the updated train ID
                if ( itemTrainId == updatedTrainId )
                {
                    updatedPosition = i;

                    // exit the loop
                    break;
                }
            }

            // if an existing item was found, it must be removed and re-added in the same spot
            if ( CB_ERR != updatedPosition )
            {
                //m_trainList.DeleteString( updatedPosition );
				m_listTrainCtrl.DeleteItem(updatedPosition);
                //updatedPosition = m_trainList.InsertString( updatedPosition, findIter->second.serviceNumber.c_str() );
				updatedPosition = m_listTrainCtrl.InsertItem( LVIF_TEXT|LVIF_STATE,updatedPosition, findIter->second.serviceNumber.c_str(),0,0,0,NULL );
                //m_listTrainCtrl.SetItemData( updatedPosition, static_cast<DWORD>( findIter->second.trainId ) );
				m_listTrainCtrl.SetItemText(updatedPosition,1,findIter->second.primaryTsi.c_str());
				m_listTrainCtrl.SetItemText(updatedPosition,2,findIter->second.secondaryTsi.c_str());
            }
            else // it needs to be added at the end
            {
                int position = m_trainList.AddString( findIter->second.serviceNumber.c_str() );
                m_trainList.SetItemData( position, static_cast<DWORD>( findIter->second.trainId ) );
				
				//CListCtrl
				int nIndex = m_listTrainCtrl.InsertItem(LVIF_TEXT|LVIF_STATE,position,findIter->second.serviceNumber.c_str(),0,0,0,NULL);
				m_listTrainCtrl.SetItemText(nIndex,1,findIter->second.primaryTsi.c_str());
				m_listTrainCtrl.SetItemText(nIndex,2,findIter->second.secondaryTsi.c_str());
            }
        }


        // now the current selection must be restored
        if ( 0 != selectedTrainId )
        {
            //for ( int i = 0; i < m_trainList.GetCount(); ++i )
			for ( int i = 0; i < m_listTrainCtrl.GetItemCount(); ++i )
            {
                // get the train id this item is for
				CString strTrainId = m_listTrainCtrl.GetItemText(i,0);
				DWORD itemData = getTrainIDSelected(strTrainId);

                //DWORD itemData = m_trainList.GetItemData( i );
                TA_IRS_Bus::CommonTypes::TrainIdType itemTrainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( itemData );

                // if it matches the selected train ID
                if ( itemTrainId == selectedTrainId )
                {
                    //m_trainList.SetCurSel( i );
					m_listTrainCtrl.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);

                    // exit the loop
                    break;
                }
            }
        }

        // refresh the display
        updateTrainDetails();

        // update the button state
        enableTakeControlButton();

        FUNCTION_EXIT;
        return 0;
    }


    void TrainControlDialog::OnSelectTrain()
    {
        FUNCTION_ENTRY( "OnSelectTrain" );
		
		//TB_DoApply();
        // selection has changed, so update the display
        updateTrainDetails();

        // also update the button state
        enableTakeControlButton();

        FUNCTION_EXIT;
    }

	void TrainControlDialog::OnNMTrainSelected(NMHDR *pNMHDR, LRESULT *pRESULT)
	{
		FUNCTION_ENTRY( "OnNMTrainSelected" );
		
		OnSelectTrain();
		TB_CanApply(true);
		*pRESULT = 0;
		FUNCTION_EXIT;
	}
	
	bool TrainControlDialog::TB_DoApply()
	{
		//ITrainBorne::TB_DoApply();
		OnTakeControl();
		
		return true;
	}
	/*
	void TrainControlDialog::TB_CanApply(bool bCanApplyNow)
	{
		OnTakeControl();
	}*/

    void TrainControlDialog::OnTakeControl()
    {
        FUNCTION_ENTRY( "OnTakeControl" );

        // get the selected item
        //int currentSelection = m_trainList.GetCurSel();
		
		int currentSelection = m_listTrainCtrl.GetSelectionMark();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"(OnTakeControl) Train Selected = %d",currentSelection);

        TA_ASSERT( CB_ERR != currentSelection, "Something has to be selected for this button to be enabled" );

        // get the train ID from the item data
        //DWORD selectedItemData = m_trainList.GetItemData( currentSelection );
		CString strTrainId = m_listTrainCtrl.GetItemText(currentSelection,0);
		DWORD selectedItemData = getTrainIDSelected(strTrainId);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"(OnTakeControl) getTrainIDSelected = %d",selectedItemData );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = static_cast<TA_IRS_Bus::CommonTypes::TrainIdType>( selectedItemData );

        // get the session ID
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // status string for result area
        CString statusString;

        // get the details of the train
        {
            TA_THREADGUARD( m_trainDetailsLock );
        
            TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetailsStore.find( trainId );

            TA_ASSERT( m_trainDetailsStore.end() != findIter, "This must exist if the item is in the list" );

            statusString = "Taking control of train ";
            statusString += findIter->second.serviceNumber.c_str();
            statusString += "... ";
        }

        // set the text to indicate it is in progress
        m_resultDisplay.SetWindowText( statusString );

        try
        {
            // make a wait cursor
            CWaitCursor wait;

            // do the actual operation
            m_trainOperationModeProxy->manuallyTakeTrainControl( trainId, sessionId.c_str() );

            // update the display string
            statusString += "Success.\r\n";
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
            
            // update the error details
            statusString += "Failed.\r\nError Communicating with Train Agent.\r\n";
            statusString += e.what();
            statusString += "\r\n";
        }
        catch( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // update the error details
            statusString += "Failed.\r\nAccess Denied.\r\n";
        }
        catch( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // update the error details
            statusString += "Failed.\r\nAccess Denied.\r\n";
        }
        catch( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // update the error details
            statusString += "Failed.\r\nError sending command to train.\r\n";
        }
        catch( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // update the error details
            statusString += "Failed.\r\nTrain Timeout\r\n";
        }
        catch( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // update the error details
            statusString += "Failed.\r\nOperation Cancelled.\r\n";
        }
        catch( TA_IRS_Bus::MpuChangeoverException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::MpuChangeoverException", e.what() );

            // update the error details
            statusString += "Failed.\r\nMPU changeover.\r\n";
        }
        catch( TA_IRS_Bus::BadCommandException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::BadCommandException", e.what() );

            // update the error details
            statusString += "Failed.\r\nBad Command.\r\n";
            statusString += e.what();
            statusString += "\r\n";
        }

        // update the display
        m_resultDisplay.SetWindowText( statusString );

        FUNCTION_EXIT;
    }

}







