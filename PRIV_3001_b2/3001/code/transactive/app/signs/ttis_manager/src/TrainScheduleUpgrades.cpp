/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/TrainScheduleUpgrades.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */


#include "stdafx.h"

#include "TTISCommonConfiguration.h"
#include "TTISMessageManager.h"
#include "TrainScheduleUpgrades.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    using TA_Base_Core::ThreadGuard;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Bus::TISAgentAccessFactory;


    TrainScheduleUpgrades::TrainScheduleUpgrades(CWnd* window)
        : AbstractTrainDownloads(window, true)
    {
        FUNCTION_ENTRY( "TrainScheduleUpgrades" );
        FUNCTION_EXIT;
    }


    TrainScheduleUpgrades::~TrainScheduleUpgrades()
    {
        FUNCTION_ENTRY( "~TrainScheduleUpgrades" );
        FUNCTION_EXIT;
    }


    bool TrainScheduleUpgrades::processRequests(const TA_Base_Core::TrainList& trainList, std::string& errorString)
    {
        FUNCTION_ENTRY( "processRequests" );

        // results for caller
        bool result = true;
        errorString.erase();

        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            // submit the request
            //TISAgentAccessFactory::getInstance().getTTISAgent()->upgradeTimeSchedule( trainList, sessionId.c_str() );

			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),upgradeTimeSchedule,(trainList, sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),upgradeTimeSchedule,(trainList, sessionId.c_str()));
			}
			//++libo
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", "While calling downloadNextMessageLibrary" );

            result = false;
            errorString = TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN;
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "While calling downloadNextMessageLibrary" );

            result = false;
            errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + "Access Denied";
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            result = false;
            errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + e.details.in();
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            result = false;
            errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + e.details.in();
        }
        catch( const CORBA::Exception& e )
        {
            std::string exceptionMsg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", exceptionMsg.c_str() );

            result = false;
            errorString = TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN + exceptionMsg;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            result = false;
            errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN) + e.what();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While calling downloadNextMessageLibrary" );

            result = false;
            errorString = TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN;
        }

        FUNCTION_EXIT;
        return result;
    }


} // TA_IRS_App

