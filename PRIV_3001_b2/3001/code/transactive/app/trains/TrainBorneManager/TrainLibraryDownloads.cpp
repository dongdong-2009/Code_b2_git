/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/TrainLibraryDownloads.cpp $
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
#include "TrainLibraryDownloads.h"
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


    TrainLibraryDownloads::TrainLibraryDownloads(CWnd* window)
        : AbstractTrainDownloads(window, true)
    {
        FUNCTION_ENTRY( "TrainLibraryDownloads" );
        FUNCTION_EXIT;
    }


    TrainLibraryDownloads::~TrainLibraryDownloads()
    {
        FUNCTION_ENTRY( "~TrainLibraryDownloads" );
        FUNCTION_EXIT;
    }


    bool TrainLibraryDownloads::processRequests(const TA_Base_Core::TrainList& trainList, std::string& errorString)
    {
        FUNCTION_ENTRY( "processRequests" );

        // results for caller
        bool result = true;
        errorString = "Unsuccessful";

        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            // submit the request
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),downloadNextMessageLibrary,( trainList, sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),downloadNextMessageLibrary,( trainList, sessionId.c_str()));
			}
			//++libo
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", "While calling downloadNextMessageLibrary" );

            result = false;           
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "While calling downloadNextMessageLibrary" );

            result = false;
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            result = false;
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            result = false;
        }
        catch( const CORBA::Exception& e )
        {
            std::string exceptionMsg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", exceptionMsg.c_str() );

            result = false;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            result = false;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While calling downloadNextMessageLibrary" );

            result = false;
        }

        FUNCTION_EXIT;
        return result;
    }

	void TrainLibraryDownloads::requestCancelDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& cancelList)
	{
		TA_Base_Core::ThreadGuard guard(m_cancelDownLoadTrainsLock);
		m_cancelDownloadTrains = cancelList;
	}

	void TrainLibraryDownloads::processCancelDownloadRequest()
	{
        std::vector< unsigned int > trainsToSubmit;
        if ( true == m_canSubmitMultiple )
        {
			TA_Base_Core::ThreadGuard guard( m_cancelDownLoadTrainsLock );
			TA_IRS_Bus::CommonTypes::TrainIdList::iterator it = m_cancelDownloadTrains.begin();
			for(; it != m_cancelDownloadTrains.end(); ++it)
			{
				trainsToSubmit.push_back(*it);
			}
			// Clear the set
			m_cancelDownloadTrains.clear();
		}
		else
		{
			TA_Base_Core::ThreadGuard guard( m_cancelDownLoadTrainsLock );
			TA_IRS_Bus::CommonTypes::TrainIdList::iterator it = m_cancelDownloadTrains.begin();
			if(it != m_cancelDownloadTrains.end())
			{
				trainsToSubmit.push_back(*it);
				m_cancelDownloadTrains.erase(it);
			}
		}
        if ( false == trainsToSubmit.empty() )
		{
            // submit all the requests
            TA_Base_Core::TrainList trainList;
            trainList.length( trainsToSubmit.size() );
            unsigned int i = 0;
            for ( std::vector< unsigned int >::iterator iter = trainsToSubmit.begin();
                  iter != trainsToSubmit.end(); ++iter )
            {
                trainList[ i++ ] = *iter;
            }
			std::string errorString = "Unsuccessful";
            bool result = true;
            try
            {
		        try
		        {
		            // get the session Id
		            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Process cancel download request");

		            // submit the request
					try
					{
						CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(), cancelDownloadMessageLibrary, ( trainList, sessionId.c_str()));
					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(), cancelDownloadMessageLibrary, ( trainList, sessionId.c_str()));
					}
		        }
		        catch ( const TA_Base_Core::OperationModeException& )
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", "While calling downloadNextMessageLibrary" );

		            result = false;
		            //errorString = TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN;
		        }
		        catch ( const TA_Base_Core::AccessDeniedException& )
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "While calling downloadNextMessageLibrary" );

		            result = false;
		            //errorString = std::string(TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN ) + "Access Denied";
		        }
		        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

		            result = false;
		            //errorString = std::string(TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN) + e.details.in();
		        }
		        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

		            result = false;
		            //errorString = std::string( TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN ) + e.details.in();
		        }
		        catch( const CORBA::Exception& e )
		        {
		            std::string exceptionMsg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ) );

		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", exceptionMsg.c_str() );

		            result = false;
		            //errorString = TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN + exceptionMsg;
		        }
		        catch ( const TA_Base_Core::TransactiveException& e )
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

		            result = false;
		            //errorString = std::string(TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN) + e.what();
		        }
		        catch (...)
		        {
		            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While calling downloadNextMessageLibrary" );

		            result = false;
		            //errorString = TTISMessageManager::ERROR_SENDING_CANCEL_DOWNLOAD_TRAIN;
		        }
            }
            catch (...)
            {
                // protect against child classes not catching something
            }

            // now the GUI will be updated to in progress as the results of each train are received asynchronously
            // however, if the submission failed, then the trains must be marked failed as there will
            // be no events received.

            if ( false == result )
            {
                TA_THREADGUARD( m_requestLock );

                // update all to failed
                for ( std::vector< unsigned int >::iterator trainIter = trainsToSubmit.begin();
                      trainIter != trainsToSubmit.end(); ++trainIter )
                {
                    processDownloadFinish( *trainIter, false, errorString );
                }
            }
		}
	}

	void TrainLibraryDownloads::run()
	{
        FUNCTION_ENTRY( "run" );

        m_running = true;

        while ( true == m_running )
        {
            // find all items that need to be submitted to the agent
            std::vector< unsigned int > trainsToSubmit;

            if ( true == m_canSubmitMultiple )
            {
				TA_Base_Core::ThreadGuard guard( m_requestLock );
                for ( DownloadInformationList::iterator iter = m_requests.begin();
                      iter != m_requests.end(); ++iter )
                {
                    // anything not in progress needs to be submitted
                    if ( false == iter->inProgress )
                    {
                        trainsToSubmit.push_back( iter->trainId );

                        // increment the retry count
                        ++(iter->retryCount);

                        // set it to in progress so that the item cant be removed from m_requests
                        iter->inProgress = true;
                    }
                }
            }
            else
            {
                // only a single request can be submitted,
                // if no other request is in progress

                bool requestInProgress = false;

				TA_Base_Core::ThreadGuard guard( m_requestLock );

                for ( DownloadInformationList::iterator iter = m_requests.begin();
                      iter != m_requests.end(); ++iter )
                {
                    // anything not in progress needs to be submitted
                    if ( true == iter->inProgress )
                    {
                        requestInProgress = true;

                        // found one in progress, cannot continue
                        break;
                    }
                }

                // if there are none in progress
                if ( false == requestInProgress )
                {
                    // queue the first one
                    if ( false == m_requests.empty() )
                    {
                        trainsToSubmit.push_back( m_requests.front().trainId );

                        // increment the retry count
                        ++(m_requests.front().retryCount);

                        // set it to in progress so that the item cant be removed from m_requests
                        m_requests.front().inProgress = true;
                    }
                }
            }

            // only need to do this if there are actions to perform
            if ( false == trainsToSubmit.empty() )
            {
                // submit all the requests
                TA_Base_Core::TrainList corbaTrainList;
                corbaTrainList.length( trainsToSubmit.size() );

                unsigned int i = 0;
                for ( std::vector< unsigned int >::iterator iter = trainsToSubmit.begin();
                      iter != trainsToSubmit.end(); ++iter )
                {
                    corbaTrainList[ i++ ] = *iter;
                }

                std::string errorString;
                bool success = false;
                try
                {
                    success = processRequests( corbaTrainList, errorString );
                }
                catch (...)
                {
                    // protect against child classes not catching something
                }

                // now the GUI will be updated to in progress as the results of each train are received asynchronously
                // however, if the submission failed, then the trains must be marked failed as there will
                // be no events received.

                if ( false == success )
                {
                    TA_THREADGUARD( m_requestLock );

                    // update all to failed
                    for ( std::vector< unsigned int >::iterator trainIter = trainsToSubmit.begin();
                          trainIter != trainsToSubmit.end(); ++trainIter )
                    {
                        processDownloadFinish( *trainIter, false, errorString );
                    }
                }

            }
			processCancelDownloadRequest();
            // wait for the retry period
            m_semaphore.timedWait( m_requestRetryInterval );
        }

        FUNCTION_EXIT;
	}
} // TA_IRS_App

