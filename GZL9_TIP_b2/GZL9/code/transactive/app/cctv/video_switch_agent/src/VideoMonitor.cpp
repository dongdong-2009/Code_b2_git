/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/VideoMonitor.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the specific functions for a video monitor.
  * Implements the IVideoMonitor interface. 
  * Inherits from VideoOutput.
  *
  * Note: There are no protocol specific actions that can be performed on 
  * a VideoMonitor - all protocol specific actions are performed on the 
  * VideoOutput class that this inherits from.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/VideoMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"

#include "bus/generic_agent/src/GenericAgent.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    //
    // Strings representing VideoMonitor sizes.
    //
    const std::string VideoMonitor::SMALL   = "Small";
    const std::string VideoMonitor::MEDIUM  = "Medium";
    const std::string VideoMonitor::LARGE   = "Large";


    //
    // VideoMonitor
    //
    VideoMonitor::VideoMonitor( TA_Base_Core::IEntityDataPtr entityData )
    : VideoOutput( entityData )
    , m_videoMonitorData( boost::dynamic_pointer_cast< TA_Base_Core::VideoMonitor >( entityData ) )
    , m_entityKey( entityData->getKey() )
    {
        if ( 0 == m_videoMonitorData.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                "The provided entity data is not VideoMonitor data." ) );
        }
		activateServantAtInit();
    }


    //
    // ~VideoMonitor
    //
    VideoMonitor::~VideoMonitor()
    {
        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.

        try
        {
			deactivateServantAtClose();
            //stop();

            // Do not delete this object because it is owned by GenericAgent.

            //m_videoMonitorData = 0;
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IVideoMonitor Methods
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // getSize
    //
	TA_Base_Bus_GZL9::VideoMonitorCorbaDef::EVideoMonitorSize VideoMonitor::getSize()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoMonitorData.get(), "The monitor database object has not been set." );

        std::string sizeStr = m_videoMonitorData->getSize();
        TA_Base_Bus_GZL9::VideoMonitorCorbaDef::EVideoMonitorSize size = TA_Base_Bus_GZL9::VideoMonitorCorbaDef::Small;
        if ( MEDIUM == sizeStr )
        {
            size = TA_Base_Bus_GZL9::VideoMonitorCorbaDef::Medium;
        }
        else if ( LARGE == sizeStr )
        {
            size = TA_Base_Bus_GZL9::VideoMonitorCorbaDef::Large;
        }
        return size;
    }

    
    //
    // getConsoleKey
    //
    unsigned long VideoMonitor::getConsoleKey()
    {
    //    TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoMonitorData.get(), "The monitor database object has not been set." );

        return m_videoMonitorData->getConsoleKey();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntity Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // start
    //
    void VideoMonitor::start() 
    {
        // Do nothing.
    }

    
    //
    // stop
    //
    void VideoMonitor::stop() 
    {
        // Deactivate this Corba servant so it no longer accepts incoming requests.
        // Don't call deactivateAndDeleteServant() because Generic Agent deletes this object.

        deactivateServant();
    }

    
    //
    // update
    //
    void VideoMonitor::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        TA_ASSERT( 0 != m_videoMonitorData.get(), "The database object for the VideoMonitor has not been initialised." );
        m_videoMonitorData->invalidate();

        VideoOutput::update( updateEvent );

        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
    }

    
    //
    // remove
    //
    void VideoMonitor::remove()
    {
        // Remove the object from the SwitchManager object.

        try
        {
            SwitchManager::getInstance().removeVideoMonitor( m_entityKey );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to remove VideoMonitor from SwitchManager." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to remove VideoMonitor from SwitchManager." );
        }

        stop();
    }

    
    //
    // setToControlMode
    //
    void VideoMonitor::activateServantAtInit()
    {
        // The state of this object is requested from the hardware after a 
        // hardware connection has been achieved.
            
        // Activate this object so it can be receive incoming requests.

        try
        {
            activateServantWithName( m_videoMonitorData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "VideoMonitor (name unknown) cannot be registered." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "VideoMonitor (name unknown) cannot be registered." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "VideoMonitor (name unknown) cannot be registered." );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "VideoMonitor (name unknown) cannot be registered." );
        }
    }

    
    //
    // setToMonitorMode
    //
    void VideoMonitor::deactivateServantAtClose()
    {
        // Deactivate the CORBA servant, so it no longer accepts incoming calls.
        // Don't call deactivateAndDelete() here because Generic Agent deletes this object.
        
        deactivateServant();    
    }

	int VideoMonitor::getDisplayOrder()
	{
		return m_videoMonitorData->getConsoleKey();
	}

} // TA_IRS_App