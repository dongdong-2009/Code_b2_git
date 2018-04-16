/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/TrainLibraryUpgrades.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */


#include "stdafx.h"

#include "TTISCommonConfiguration.h"
#include "../TrainBorne_PIDS/src/TTISMessageManager.h"
#include "TrainLibraryUpgrades.h"

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


    TrainLibraryUpgrades::TrainLibraryUpgrades(CWnd* window)
        : AbstractTrainDownloads(window, true)
    {
        FUNCTION_ENTRY( "TrainLibraryUpgrades" );
        FUNCTION_EXIT;
    }


    TrainLibraryUpgrades::~TrainLibraryUpgrades()
    {
        FUNCTION_ENTRY( "~TrainLibraryUpgrades" );
        FUNCTION_EXIT;
    }


	void TrainLibraryUpgrades::startDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool autoRetry)
	{
		FUNCTION_ENTRY( "startDownload" );

		TA_THREADGUARD( m_requestLock );

		for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator iter = trainList.begin();
			iter != trainList.end(); ++iter  )
		{

			// before inserting, see if another the same is already there
			DownloadInformationList::iterator requestIter;
			for ( requestIter = m_requests.begin(); m_requests.end() != requestIter; ++requestIter )
			{
				if ( *iter == requestIter->trainId )
				{
					// found it
					break;
				}
			}

			// if there was another in the list already
			if ( requestIter != m_requests.end() )
			{
				LOG_GENERIC ( SourceInfo, DebugUtil::DebugDebug,
					"Found a an existing request for train %d, type %d",
					*iter, getDownloadType() );

				// just update the retry field if necessary
				if ( true == autoRetry )
				{
					requestIter->autoRetry = autoRetry;
				}

				
				//notifyDownloadInProgress( *requestIter );
			
			}
			// if there wasnt one in progress
			else
			{
				LOG_GENERIC ( SourceInfo, DebugUtil::DebugDebug,
					"This is a new request for train %d, type %d",
					*iter, getDownloadType() );

				DownloadInformation newRequest;
				newRequest.trainId = *iter;
				newRequest.autoRetry = autoRetry;
				newRequest.retryCount = 0;
				newRequest.lastError = "";
				newRequest.inProgress = false;

				// insert new request
				m_requests.push_back( newRequest );

				// uodate the gui with the latest changes
				notifyDownloadInProgress( m_requests.back() );
			}
		}

		FUNCTION_EXIT;
	}

    bool TrainLibraryUpgrades::processRequests(const TA_Base_Core::TrainList& trainList, std::string& errorString)
    {
        FUNCTION_ENTRY( "processRequests" );

        // results for caller
        bool result = true;
		errorString = "In progress";

        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            // submit the request
            //TISAgentAccessFactory::getInstance().getTTISAgent()->upgradeMessageLibrary( trainList, sessionId.c_str() );
			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),upgradeMessageLibrary,(trainList, sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),upgradeMessageLibrary,(trainList, sessionId.c_str()));
			}
			//++libo
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", "While calling downloadNextMessageLibrary" );

            result = false;
            //errorString = TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN;
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "While calling downloadNextMessageLibrary" );

            result = false;
            //errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + "Access Denied";
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            result = false;
            //errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + e.details.in();
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            result = false;
            //errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + e.details.in();
        }
        catch( const CORBA::Exception& e )
        {
            std::string exceptionMsg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", exceptionMsg.c_str() );

            result = false;
            //errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + exceptionMsg;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            result = false;
            //errorString = std::string( TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN ) + e.what();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While calling downloadNextMessageLibrary" );

            result = false;
            //errorString = TTISMessageManager::ERROR_SENDING_UPGRADE_TRAIN;
        }

        FUNCTION_EXIT;
        return result;
    }


} // TA_IRS_App

