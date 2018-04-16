/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainLibraryDownloads.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
        : AbstractTrainDownloads(window, false)
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
        errorString.erase();

        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            // submit the request
            //TISAgentAccessFactory::getInstance().getTTISAgent()->downloadNextMessageLibrary( trainList, sessionId.c_str() );
			//libo++
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
            errorString = TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN;
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "While calling downloadNextMessageLibrary" );

            result = false;
            errorString = std::string(TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN ) + "Access Denied";
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            result = false;
            errorString = std::string(TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN) + e.details.in();
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            result = false;
            errorString = std::string( TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN ) + e.details.in();
        }
        catch( const CORBA::Exception& e )
        {
            std::string exceptionMsg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", exceptionMsg.c_str() );

            result = false;
            errorString = TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN + exceptionMsg;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            result = false;
            errorString = std::string(TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN) + e.what();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While calling downloadNextMessageLibrary" );

            result = false;
            errorString = TTISMessageManager::ERROR_SENDING_DOWNLOAD_TRAIN;
        }

        FUNCTION_EXIT;
        return result;
    }


} // TA_IRS_App

