/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/TrainAlarms.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 15:59:52 $
  * Last modified by:  $Author: builder $
  *
  */
// TrainAlarms.cpp: implementation of the TrainAlarms class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/alarmlistctrl.h"
#include "bus/alarm/alarm_list_control/src/TrainAlarms.h"
#include "bus/alarm/alarm_list_control/src/LocationSelectionDialog.h"
#include "bus/alarm/alarm_list_control/src/TrainAgentAccess.h"


#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"

#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"

#include "core/utilities/src/RunParams.h"

#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CCTV_ACTIVATE_CATCH_HANDLER()\
catch (const TA_Base_Core::TransactiveException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.what();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Core::OperationModeException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << std::string( "Operation mode error" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Core::AccessDeniedException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << std::string( "Access Denied" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidTrainException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidParameterException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransmissionFailureException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionTimeoutException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionCancelledException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::MpuChangeoverException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::BadCommandException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::BadCommandException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_040042);\
}\
catch ( ... )\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "...", "..." );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_650019);\
}


#define CCTV_DEACTIVATE_CATCH_HANDLER( TrainId )\
catch (const TA_Base_Core::TransactiveException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.what();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Core::OperationModeException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << std::string( "Operation mode error" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Core::AccessDeniedException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", ex.reason.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << std::string( "Access Denied" );\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidTrainException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::InvalidParameterException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransmissionFailureException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionTimeoutException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TransactionCancelledException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::MpuChangeoverException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( TA_Base_Bus::ITrainExceptionsCorba::BadCommandException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::BadCommandException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
    catch ( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException& ex)\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvException", ex.details.in() );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    userMsg << ex.details.in();\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\
catch ( ... )\
{\
    LOG_EXCEPTION_CATCH( SourceInfo, "...", "..." );\
    TA_Base_Bus::TransActiveMessage userMsg;\
    userMsg << TrainId;\
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);\
}\


namespace TA_Base_Bus
{
	TrainAlarms::TrainAlarms()
	{
        FUNCTION_ENTRY( "TrainAlarms" );
        FUNCTION_EXIT;
   	}


	TrainAlarms::~TrainAlarms()
	{
        FUNCTION_ENTRY( "~TrainAlarms" );
        
        delete m_locationSelectionDialog;
        m_locationSelectionDialog  = NULL;

        FUNCTION_EXIT;
	}

	TrainAlarms& TrainAlarms::getInstance()
	{
        FUNCTION_ENTRY( "getInstance" );
        
        return *( TrainAlarmsSingleton::instance() );
        
        FUNCTION_EXIT;
	}

    void TrainAlarms::acknowledgeAlarm( TA_Base_Bus::AlarmDataItem* details )
    {
        FUNCTION_ENTRY( "acknowledgeAlarm" );
        
        // get the camera list from the database
		TA_Base_Core::IEntityData* iData = NULL;
        std::vector<unsigned long> cameraNumbers;
        unsigned char trainId = 0;
        unsigned long locationKey = details->locationKey;

		try
		{
			iData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(details->assocEntityKey);
			TA_Base_Core::TrainCctvCameraEntityData* cameraData = dynamic_cast <TA_Base_Core::TrainCctvCameraEntityData*> (iData);

			if (NULL != cameraData)
			{
				trainId = static_cast<unsigned char>(cameraData->getPhysicalTrainNumber());
				cameraNumbers = cameraData->getCameraNumbers();
			}
			else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Associated Entity type is not a TrainCctvCameraEntity. Entitykey=%d, EntityTypeKey=%d", details->assocEntityKey, details->assocEntityTypeKey);
            
        }
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An error occurred while retrieving Associated Entity for train camera alarm");
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An error occurred while retrieving Associated Entity for train camera alarm");
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An unknown error occurred while retrieving Associated Entity for train camera alarm");
        }
    
        delete iData;
        iData = NULL;
    
        if ( ( 0 == trainId ) || ( true == cameraNumbers.empty() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // build the train agent and switch agent specific camera lists
        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence trainAgentCamList;
        TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence switchAgentCamList;
    
        trainAgentCamList.length( cameraNumbers.size() );
        switchAgentCamList.length( cameraNumbers.size() );
        for ( unsigned int i = 0; i < cameraNumbers.size(); ++i )
        {
            trainAgentCamList[i] = static_cast<TA_Base_Bus::ITrainCctvCorba::ECctvCamera>( cameraNumbers[i] );
            switchAgentCamList[i] = cameraNumbers[i];
        }
    
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // activate the train image using activateTrain
        bool trainActivated = activateTrain( trainId,
                                             trainAgentCamList,
                                             sessionId  );
    
        // if the train was activated using activateTrain
        if ( true == trainActivated )
        {
            // then the monitor must be activated using switchMonitor
            switchMonitor( locationKey,
                           trainId,
                           switchAgentCamList,
                           TrainAgentAccess::getInstance().getConsoleKey() );
        }

        FUNCTION_EXIT;
    }


    bool TrainAlarms::activateTrain( unsigned char trainId,
                                     TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence cameraIds,
                                     std::string sessionId )
    {
        FUNCTION_ENTRY( "activateTrain" );

        try
        {
            // Get the train information from the train information agent
            ITrainInformationCorba::TrainDetails_var trainInfo;
            CORBA_CALL_RETURN( trainInfo, TrainAgentAccess::getInstance().getTrainInformationAgent(), getTrainDetails, ( trainId ) );//limin
    
            bool delocalised = false;
            unsigned long overrideLocation = 0;
    
            // if the ats information is invalid, or the localisation is invalid
            if ( ( false == trainInfo->atsValid ) ||
                 ( false == trainInfo->isLocalisationValid ) )
            {
                // create a m_locationSelectionDialog if it is not created yet
                if ( NULL == m_locationSelectionDialog )
                {
                    m_locationSelectionDialog = new LocationSelectionDialog();
                }
                
                // using the location in the information set the initial location in m_locationSelectionDialog
                m_locationSelectionDialog->setInitialSelection( trainInfo->currentLocation );
            
                // call DoModal
                if ( IDOK != m_locationSelectionDialog->DoModal() )
                {
                    // if the return value is not IDOK, return
                    return false;
                }
    
                // if the return value is IDOK, get the selected location from the dialog
                delocalised = true;
                overrideLocation = m_locationSelectionDialog->getSelectedLocation();
    
                if ( 0 == overrideLocation )
                {
                    return false;
                }
            }
    
            try
            {
				TA_Base_Bus::ITrainCctvCorba::CctvSwitchState SwitchState;
                CORBA_CALL_RETURN( SwitchState, TrainAgentAccess::getInstance().getTrainCctvAgent(), getCctvState, ( trainId ) );//limin

				if ( TA_Base_Bus::ITrainCctvCorba::CctvQuad == SwitchState.cctvState && SwitchState.item4 != 0 )
				{
					TA_Base_Bus::TransActiveMessage userMsg;

					std::ostringstream train;
					train << "Train "
							<< long(trainInfo->trainId);

					std::ostringstream newCameras;
					std::ostringstream oldCameras;

					switch( cameraIds.length() )
					{
					case 1:
						newCameras << "camera ";
						oldCameras << "camera "
								   << long(SwitchState.item4);
						break;
					case 2:
						newCameras << "cameras ";
						oldCameras << "cameras "
								   << long(SwitchState.item4) 
								   << ","
								   << long(SwitchState.item3);
					    break;
					default:
						newCameras << "cameras ";
						oldCameras << "the old cameras";
					    break;
					}

					for( unsigned int i = 0; i < cameraIds.length(); i ++ ) 
					{
						newCameras << cameraIds[i] << " ";
					}

					userMsg << train.str();
					userMsg << oldCameras.str();
					userMsg << newCameras.str();
					
					UINT returnVal = userMsg.showMsgBox( IDS_UW_090026 );
					if( IDYES != returnVal )
					{
						return true;
					}
				}

                // activate the CCTV using the train cctv agent
                // depending on whether the location is to be overridden, and whether a location was selected, call either addAlarmImage or addAlarmImageDelocalisedTrain
                if ( delocalised )
                {
                    CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImageDelocalisedTrain, ( trainId, cameraIds, overrideLocation, sessionId.c_str() ) );//limin
                }
                else
                {
                    CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImage, ( trainId, cameraIds, sessionId.c_str() ) );//limin
                }
    
                return true;
            }
            catch ( ITrainExceptionsCorba::TrainCctvConflictException& ex)
            {
                std::string message = ex.details.in();
                LOG_EXCEPTION_CATCH( SourceInfo, "ITrainExceptionsCorba::TrainCctvConflictException", message.c_str() );
            
                // catch any cctv conflict exceptions and prompt the user to deactivate a train
                TA_Base_Bus::TransActiveMessage userMsg;
                CString error = message.c_str();
                userMsg << error;
                UINT selectedButton = userMsg.showMsgBox( IDS_UE_650018 );
                    
                if( IDOK == selectedButton )
                {
                    // once the train is deactivated, try to activate the camera again

                    std::ostringstream trainIdString;
                    trainIdString << "PV" << std::setfill('0') << std::setw(2) << ex.owningTrain;
                    try
                    {
                        CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), setCarrierOff, ( ex.owningTrain, sessionId.c_str() ) );//limin
                    }
                    CCTV_DEACTIVATE_CATCH_HANDLER( trainIdString.str() );

                
                    if ( delocalised )
                    {
                        CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImageDelocalisedTrain, ( trainId, cameraIds, overrideLocation, sessionId.c_str() ) );//limin
                    }
                    else
                    {
                        CORBA_CALL( TrainAgentAccess::getInstance().getTrainCctvAgent(), addAlarmImage, ( trainId, cameraIds, sessionId.c_str() ) );//limin
                    }
    
                    FUNCTION_EXIT;
                    return true;
                }
            }
        }
        CCTV_ACTIVATE_CATCH_HANDLER();
    
        FUNCTION_EXIT;
        return false;
    }


    void TrainAlarms::switchMonitor(unsigned long location, unsigned char trainId, TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence cameraIds, unsigned long consoleKey)
    {
        FUNCTION_ENTRY( "switchMonitor" );

        // Use VideoSwitchAgentFactory to activate the cameras using acknowledgeTrainCameraAlarm
        // catch the exceptions and display the error message
        try
        {
            CORBA_CALL( VideoSwitchAgentFactory::getInstance().getSwitchManager(), acknowledgeTrainCameraAlarm, (
                    location, trainId, cameraIds, consoleKey ) );//limin
        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
        
            TA_Base_Bus::TransActiveMessage userMsg;
            CString temp = te.what();
            CString description = "switching trainborne image to monitor: " + temp;
            userMsg << description;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
        }
        catch (TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", ex.what.in() );
    
            TA_Base_Bus::TransActiveMessage userMsg;
            CString temp = ex.what.in();
            CString description = "switching trainborne image to monitor: " + temp;
            userMsg << description;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
        }
        catch( const CORBA::Exception& cex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
    
            TA_Base_Bus::TransActiveMessage userMsg;
            CString temp = "switching trainborne image to monitor: ";
            std::string temp2 = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex );
            CString description =  temp + temp2.c_str();
            userMsg << description;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While switching monitor" );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString description = "switching trainborne image to monitor: ";
            userMsg << description;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_632015);
        }

        FUNCTION_EXIT;
    }

}
