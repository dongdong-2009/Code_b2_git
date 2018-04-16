/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoEnquirer.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Light-weight singleton class to communicate with the VideoSwitchAgent
  * for selected enquiry functions only :
  *     - get camera name
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 )
#endif // defined( WIN32 )

#include "stdafx.h"

#include <memory>
#include "VideoEnquirer.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#ifdef _MSC_VER
    #pragma warning(disable : 4786)
#endif

namespace TA_Base_App
{
    std::string VideoEnquirer::getCameraName( unsigned long cameraEntityKey )
    {
        FUNCTION_ENTRY( "getCameraName" );

        std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject > camera;
        std::string name = "";
        try
        {
            camera = std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject >(
                TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera( cameraEntityKey ) );
            if ( 0 != camera.get() )
            {
                CORBA_CALL_RETURN( name, (*camera), getName, () );//limin
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get Camera." ));
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Camera name" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return name;
    }

    std::string VideoEnquirer::getMonitorName( unsigned long monitorEntityKey )
    {
        FUNCTION_ENTRY( "getMonitorName" );

        std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject > monitor;
        std::string name = "";
        try
        {
            monitor = std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject >(
                TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoMonitorNamedObject( monitorEntityKey ) );
            if ( 0 != monitor.get() )
            {
                CORBA_CALL_RETURN( name, (*monitor), getName, () );//limin
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Monitor object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get Monitor." ));
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Monitor name" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return name;
    }
}
