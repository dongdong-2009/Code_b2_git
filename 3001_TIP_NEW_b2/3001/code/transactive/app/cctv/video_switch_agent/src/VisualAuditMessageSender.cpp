/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/VisualAuditMessageSender.cpp $
  * @author:   Bruce Polmear
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/29 18:03:06 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Broadcasts messages to the GUIs and any other Video Switch Agents
  * notifying them of state updates.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/VisualAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/alarm/src/AlarmConstants.h"

/*
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "bus/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
*/

namespace TA_IRS_App
{
    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable VisualAuditMessageSender::m_singletonLock;
	VisualAuditMessageSender* VisualAuditMessageSender::m_theClassInstance = NULL;


    //
    // getInstance
    //
    VisualAuditMessageSender& VisualAuditMessageSender::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new VisualAuditMessageSender();
		}

		return *m_theClassInstance;
    }


    //
    // ~VisualAuditMessageSender
    //
    VisualAuditMessageSender::~VisualAuditMessageSender() 
    {
        // Do nothing.
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Audit Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // sendMessage
    //
    void VisualAuditMessageSender::sendMessage( EAuditMessageType messageType,
                                                unsigned long entityKey,
                                                const std::string& sessionId,
                                                const std::string& optionalParameter1,
                                                const std::string& optionalParameter2 )
    {
        const TA_Base_Core::MessageType* type = 0;

        TA_Base_Core::NameValuePair param1( "ConsoleName", getWorkstationForSession( sessionId ) );
        TA_Base_Core::NameValuePair param2( "VideoInput", optionalParameter1 );
        TA_Base_Core::NameValuePair param3( "VideoOutput", optionalParameter2 );

        TA_Base_Core::NameValuePair actionParam1( "Action", optionalParameter1 );
        TA_Base_Core::NameValuePair actionParam2( "EntityName", optionalParameter2 );

		// TD14548
		TA_Base_Core::NameValuePair name_v1( "Operator", optionalParameter1 );
		TA_Base_Core::NameValuePair name_v2( "SequenceID", optionalParameter2 );
		// TD14548
		TA_Base_Core::NameValuePair blankParam("Blank", " ");

        TA_Base_Core::DescriptionParameters params;
        params.push_back( &param1 );
        params.push_back( &param2 );

        switch ( messageType )
        {
            case SwitchInputToOutputAttempt:
                type = &(TA_Base_Core::VisualAudit::VisualSwitchInputToOutputAttempt);
                params.push_back( &param3 );
                break;
            case SwitchInputToOutputSuccess:
                type = &(TA_Base_Core::VisualAudit::VisualSwitchInputToOutputSuccess);
                params.push_back( &param3 );
                break;
            case SwitchInputToOutputFailure:
                type = &(TA_Base_Core::VisualAudit::VisualSwitchInputToOutputFailure);
                params.push_back( &param3 );
                break;
            case LockAttempt:
                type = &(TA_Base_Core::VisualAudit::VisualLockCameraAttempt);
                break;
            case LockSuccess:
                type = &(TA_Base_Core::VisualAudit::VisualLockCameraSuccess);
                break;
            case LockFailure:
                type = &(TA_Base_Core::VisualAudit::VisualLockCameraFailure);
                break;
            case UnlockAttempt:
                type = &(TA_Base_Core::VisualAudit::VisualUnlockCameraAttempt);
                break;
            case UnlockSuccess:
                type = &(TA_Base_Core::VisualAudit::VisualUnlockCameraSuccess);
                break;
            case UnlockFailure:
                type = &(TA_Base_Core::VisualAudit::VisualUnlockCameraFailure);
                break;
            case OverrideLockAttempt:
                type = &(TA_Base_Core::VisualAudit::VisualOverrideCameraLockAttempt);
                break;
            case OverrideLockSuccess:
                type = &(TA_Base_Core::VisualAudit::VisualOverrideCameraLockSuccess);
                break;
            case OverrideLockFailure:
                type = &(TA_Base_Core::VisualAudit::VisualOverrideCameraLockFailure);
                break;
            case ActionAttempt:
                type = &(TA_Base_Core::VisualAudit::VisualOperatorActionAttempt);
                params.clear();
                params.push_back( &actionParam1 );
                params.push_back( &actionParam2 );
                break;
            case ActionSuccess:
                type = &(TA_Base_Core::VisualAudit::VisualOperatorActionSuccess);
                params.clear();
                params.push_back( &actionParam1 );
                params.push_back( &actionParam2 );
                break;
            case ActionFailure:
                type = &(TA_Base_Core::VisualAudit::VisualOperatorActionFailure);
                params.clear();
                params.push_back( &actionParam1 );
                params.push_back( &actionParam2 );
                break;
			case EditSequence: // TD14548
				type = &(TA_Base_Core::VisualAudit::VisualEditSequence);
				params.clear();
				params.push_back( &blankParam);
				params.push_back( &name_v2 );
				break;
            default:
                break;
        }

        // Send the audit message

        if ( type != 0 )
        {
			timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;

			m_auditSender->sendAuditMessage( *type,     // Message Type
		                                     entityKey, // Entity Key
		                                     params,    // Description Parameters
		                                     "",        // Details
		                                     sessionId, // Session ID
		                                     "",        // Alarm ID
		                                     "",        // Incident ID
		                                     "",		// Parent Event ID
											 NULL,		// Filter Data
											 timestamp);// Timestamp Data		
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // VisualAuditMessageSender
    //
    VisualAuditMessageSender::VisualAuditMessageSender()
    : m_auditSender( std::auto_ptr< TA_Base_Core::AuditMessageSender >( 
            TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( 
            TA_Base_Core::VisualAudit::Context ) ) )
    {
		// Note: Publish by context as the MessageSender will be sending multiple MessageTypes. 
    }


    //
    // getWorkstationForSession
    //
    std::string VisualAuditMessageSender::getWorkstationForSession( const std::string& sessionId ) const
    {
        TA_Base_Core::IConsole* console = 0;
        std::string workstation = "";
        try
        {
            console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionId, false );

            if ( console != 0 )
            {
                workstation = console->getName();
                delete console;
                console = 0;
            }
        }
        catch ( const TA_Base_Core::DatabaseException& e )
        {
            std::stringstream error;
            error << "Failed to retrieve the operator name for the sessionId: ";
            error << sessionId;
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", error.str().c_str() );
        }
        catch ( const TA_Base_Core::DataException& e )
        {
            std::stringstream error;
            error << "Failed to retrieve the operator name for the sessionId: ";
            error << sessionId;
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", error.str().c_str() );
        }
        catch ( const TA_Base_Core::EntityTypeException& e )
        {
            std::stringstream error;
            error << "Failed to retrieve the operator name for the sessionId: ";
            error << sessionId;
            LOG_EXCEPTION_CATCH( SourceInfo, "EntityTypeException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "EntityTypeException", error.str().c_str() );
        }
        catch ( ... )
        {
            std::stringstream error;
            error << "Failed to retrieve the operator name for the sessionId: ";
            error << sessionId;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "Unknown Exception", error.str().c_str() );
        }


        return workstation;
    }

} // TA_IRS_App

