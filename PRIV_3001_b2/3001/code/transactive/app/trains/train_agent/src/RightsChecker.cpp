/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/RightsChecker.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/exceptions/src/RightsException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
    const std::string RightsChecker::CONTROL_TRAINBORNE_CCTV = "CONTROL_TRAINBORNE_CCTV";
    const std::string RightsChecker::TRAIN_TIS_FREETEXT_MESSAGE = "TRAIN_TIS_FREETEXT_MESSAGE";
    const std::string RightsChecker::TRAIN_TIS_STORED_MESSAGE = "TRAIN_TIS_STORED_MESSAGE";
    const std::string RightsChecker::TRAIN_TIS_RESET_MESSAGE = "TRAIN_TIS_RESET_MESSAGE";
    const std::string RightsChecker::TRAIN_TIS_DOWNLOAD_LIBRARY = "TRAIN_TIS_DOWNLOAD_LIBRARY";
    const std::string RightsChecker::TRAIN_TIS_DOWNLOAD_SCHEDULE = "TRAIN_TIS_DOWNLOAD_SCHEDULE";
    const std::string RightsChecker::TRAIN_PA_PRERECORDED_ANNOUNCEMENT = "TRAIN_PA_PRERECORDED_ANNOUNCEMENT";
    const std::string RightsChecker::TRAIN_PA_CYCLIC_ANNOUNCEMENT = "TRAIN_PA_CYCLIC_ANNOUNCEMENT";
    const std::string RightsChecker::TRAIN_PA_LIVE_ANNOUNCEMENT = "TRAIN_PA_LIVE_ANNOUNCEMENT";
    const std::string RightsChecker::RADIO_MAKE_TRAIN_CALL = "RADIO_MAKE_TRAIN_CALL";
    const std::string RightsChecker::RADIO_ANSWER_PEC_CALL = "RADIO_ANSWER_PEC_CALL";
    const std::string RightsChecker::TRAIN_MANUAL_TAKE_CONTROL = "TRAIN_MANUAL_TAKE_CONTROL";
    const std::string RightsChecker::RADIO_MANAGE_TRAIN_SELECTIONS = "RADIO_MANAGE_TRAIN_SELECTIONS";
}


namespace TA_IRS_App
{

    RightsChecker::RightsChecker( ITrainAgentConfiguration& configuration,
                                  TA_Base_Bus::ItaRights& itaRights,
                                  IOperationModeManager& operationModeManager )
        : m_configuration( configuration ),
          m_taRights( itaRights ),
          m_operationModeManager( operationModeManager )
    {
        FUNCTION_ENTRY( "RightsChecker" );
        FUNCTION_EXIT;
    }


    RightsChecker::~RightsChecker()
    {
        FUNCTION_ENTRY( "~RightsChecker" );
        FUNCTION_EXIT;
    }


    bool RightsChecker::canControlCctv( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canControlCctv" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_CONTROL_TRAINBORNE_CCTV, CONTROL_TRAINBORNE_CCTV );
    }


    bool RightsChecker::canDisplayTtisFreeText( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canDisplayTtisFreeText" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_FREETEXT_MESSAGE, TRAIN_TIS_FREETEXT_MESSAGE );
    }


    bool RightsChecker::canDisplayTtisPredefined( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canDisplayTtisPredefined" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_STORED_MESSAGE, TRAIN_TIS_STORED_MESSAGE );
    }


    bool RightsChecker::canResetTtisMessage( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canResetTtisMessage" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_RESET_MESSAGE, TRAIN_TIS_RESET_MESSAGE );
    }


    bool RightsChecker::canDownloadTtisLibrary( TA_IRS_Bus::CommonTypes::TrainIdType trainId,const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canDownloadTtisLibrary" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_DOWNLOAD_LIBRARY, TRAIN_TIS_DOWNLOAD_LIBRARY,trainId );
    }
	
	bool RightsChecker::canDownloadTtisLibrary( const std::string& sessionId )
	{
		FUNCTION_ENTRY( "canDownloadTtisLibrary" );
		FUNCTION_EXIT;
		return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_DOWNLOAD_LIBRARY, TRAIN_TIS_DOWNLOAD_LIBRARY );
	}


    bool RightsChecker::canDownloadTtisSchedule( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canDownloadTtisSchedule" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_TIS_DOWNLOAD_SCHEDULE, TRAIN_TIS_DOWNLOAD_SCHEDULE );
    }


    bool RightsChecker::canBroadcastPaLive( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canBroadcastPaLive" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_PA_LIVE_ANNOUNCEMENT, TRAIN_PA_LIVE_ANNOUNCEMENT );
    }


    bool RightsChecker::canBroadcastPaDva( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canBroadcastPaDva" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_PA_PRERECORDED_ANNOUNCEMENT, TRAIN_PA_PRERECORDED_ANNOUNCEMENT );
    }


    bool RightsChecker::canBroadcastPaCyclicDva( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canBroadcastPaCyclicDva" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_PA_CYCLIC_ANNOUNCEMENT, TRAIN_PA_CYCLIC_ANNOUNCEMENT );
    }


    bool RightsChecker::canCallTrain( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canCallTrain" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL, RADIO_MAKE_TRAIN_CALL );
    }


    bool RightsChecker::canAnswerPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canAnswerPec" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_RADIO_ANSWER_PEC_CALL, RADIO_ANSWER_PEC_CALL, trainId );
    }


    bool RightsChecker::canModifySelectionList( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canModifySelectionList" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_RADIO_MANAGE_TRAIN_SELECTIONS, RADIO_MANAGE_TRAIN_SELECTIONS );
    }


    bool RightsChecker::canForceTrainControl( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "canForceTrainControl" );
        FUNCTION_EXIT;
        return isActionPermitted( sessionId, TA_Base_Bus::aca_TRAIN_MANUAL_TAKE_CONTROL, TRAIN_MANUAL_TAKE_CONTROL );
    }


    bool RightsChecker::isActionPermitted( const std::string& sessionId,
                                           unsigned long actionKey,
                                           const std::string& actionString,
                                           TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "isActionPermitted" );
        bool result = false;
		LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"isActionPermitted Entered,result=%d,trainId=%d",result,trainId);

        // if the train ID is not 0, check the operation mode manager to see if we are in control of this train:
        if ( 0 != trainId )
        {
			LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"trainId!=0,result=%d",result);
            if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
            {
                // if we are not in control of the train, then log an error message of the form:
                // Access Denied [Operator=Session ID][action=action key : action string][trainId=train id][result=false]
                // and return false
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugError,
                             "Access Denied [Operator=%s][action=%d : %s][trainId=%d][result=false]",
                             sessionId.c_str(),
                             actionKey,
                             actionString.c_str(),
                             trainId );

                FUNCTION_EXIT;
                return false;
            }
			else
			{
				result = true;
				LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"This train is in current Control, result=%d",result);
			}
        }
		else
		{
			result = true;
		}

		LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"checking completed,result=%d",result);
/*
        // reasonString is a local variable passed by ref
        std::string reasonString;

        try
        {
            // check the access rights
            result = m_taRights.isActionPermittedOnResourceAndLocSub( sessionId,
                                                                      m_configuration.getEntityKey(),
                                                                      m_configuration.getLocationKey(),
                                                                      m_configuration.getRightsSubsystem(),
                                                                      actionKey,
                                                                      reasonString );
			LOG_GENERIC( SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"result=%d, isActionPermittedOnResourceAndLocSub( [sessionId=%s][resource=%d][location=%d][subsystem=%d][action=%d][reasonString=%s]",
				result,
				sessionId.c_str(),
				m_configuration.getEntityKey(),
				m_configuration.getLocationKey(),
				m_configuration.getRightsSubsystem(),
				actionKey,
				reasonString.c_str() );

        }
        catch ( TA_Base_Core::RightsException& e )
        {
            // if there was an error checking rights (RightsException) then log the error and set the result = true
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::RightsException", e.what() );

            result = true;
        }
*/
        // log the result of the rights check
        if ( true == result )
        {
            // Access Granted [Operator=Session ID][action=action key : action string][trainId=train id]
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugInfo,
                         "Access Granted [Operator=%s][action=%d : %s][trainId=%d][Entity: %d][Location: %d][Subsystem: %d]",
                         sessionId.c_str(),
                         actionKey,
                         actionString.c_str(),
                         trainId,
                         m_configuration.getEntityKey(),
                         m_configuration.getLocationKey(),
                         m_configuration.getRightsSubsystem() );
        }
        else
        {
            // Access Denied [Operator=Session ID][action=action key : action string][trainId=train id][reasonString=reasonString string][DM=decision module]
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugInfo,
                         "Access Denied [Operator=%s][action=%d : %s][trainId=%d][Entity: %d][Location: %d][Subsystem: %d]",//[reasonString=%s]",
                         sessionId.c_str(),
                         actionKey,
                         actionString.c_str(),
                         trainId,
                         m_configuration.getEntityKey(),
                         m_configuration.getLocationKey(),
                         m_configuration.getRightsSubsystem());
//                         reasonString.c_str() );
        }
		LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"isActionPermitted Exited");

        FUNCTION_EXIT;
        return result;
    }


} // TA_IRS_App
