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


#include "app/trains/train_agent/src/CctvAgentInterface.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"

#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"

#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/exceptions/src/FatalConfigurationException.h"


namespace TA_IRS_App
{

    CctvAgentInterface::CctvAgentInterface( ITrainAgentConfiguration& configuration )
        : m_configuration( configuration ),
          m_switchManager()
    {
        FUNCTION_ENTRY( "CctvAgentInterface" );

        loadAgentObject();

        FUNCTION_EXIT;
    }


    CctvAgentInterface::~CctvAgentInterface()
    {
        FUNCTION_ENTRY( "~CctvAgentInterface" );
        FUNCTION_EXIT;
    }


    void CctvAgentInterface::setStageSwitchMode( unsigned long originatingStage,
                                                 unsigned long destinationStage,
                                                 bool allowSwitch )
    {
        FUNCTION_ENTRY( "setStageSwitchMode" );

        try
        {
            // Just make the call on the remote agent.
            CORBA_CALL( m_switchManager, setStageSwitchMode, ( originatingStage, destinationStage, allowSwitch ) );//limin
        }
        // Catch any exceptions and wrap them in the appropriate C++ exception
        catch ( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::VideoSwitchAgentException", e.what.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what.in() ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    void CctvAgentInterface::loadAgentObject()
    {
        FUNCTION_ENTRY( "loadAgentObject" );

        try
        {
            // load the local VideoSwitchAgent entity from the database, and populate the named object with the details.
            TA_Base_Core::CorbaNameList videoSwitchAgentCorbaName =
                TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( TA_Base_Core::VideoSwitchAgent::getStaticType(),
                                                                                                m_configuration.getLocationKey(),false );

            if ( videoSwitchAgentCorbaName.size() != 1 )
            {
                std::ostringstream error;

                error << "Incorrect configuration, there are " << videoSwitchAgentCorbaName.size()
                      << " video switch agent corba names at this location";

                TA_THROW( TA_Base_Core::FatalConfigurationException( error.str() ) );
            }

            m_switchManager.setCorbaName( videoSwitchAgentCorbaName[0] );
        }
        catch ( ... )
        {
            TA_THROW( TA_Base_Core::FatalConfigurationException( "Configuration Error: Can not locate VideoSwitch Agent" ) );
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App
