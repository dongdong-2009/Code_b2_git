/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/VideoOutputGroup.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A VideoOutputGroup is an object to group VideoOutputs together,
  * such as a video wall monitor group, a wallboard, all recording devices or all
  * operator workstations.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/VideoOutputGroup.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"

#include "bus/generic_agent/src/GenericAgent.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoOutputGroup.h"
#include "core/data_access_interface/video_switch_agent/src/VideoOutputGroupAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/naming/src/Naming.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    //
    // VideoOutputGroup
    //
    VideoOutputGroup::VideoOutputGroup( TA_Base_Core::IEntityDataPtr entityData )
        : m_videoOutputGroupData( boost::dynamic_pointer_cast<TA_Base_Core::VideoOutputGroup>(entityData) )
    , m_isListLoaded( false )
    , m_entityKey( entityData->getKey() )
    {
        // Store the database data.

        if ( 0 == m_videoOutputGroupData.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                "The provided entity data is not VideoOutputGroup data." ) );
        }
        

		//chenlei++
		//preload videoOutput setting from database
		m_videoOutputsMap = TA_IRS_Core::VideoOutputGroupAccessFactory::getInstance().getVideoOutputGroup( 
			m_entityKey, false )->getVideoOutputs();

		activateServantAtInit();

        // Don't load the VideoOutputs yet, because they may not have all been constructed yet.
    }

    
    //
    // ~VideoOutputGroup
    //
    VideoOutputGroup::~VideoOutputGroup()
    {
        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.

        try
        {
            // Deactivate the CORBA servant, so it no longer accepts incoming calls.
            // Don't call deactivateAndDelete() here because Generic Agent deletes this object.
			//deactivateServantAtClose();
            deactivateServant();    
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
    // IVideoOutputGroup Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getKey
    //
    unsigned long VideoOutputGroup::getKey()
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return m_entityKey;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }
        

    //
    // getName
    //
    char* VideoOutputGroup::getName()
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return CORBA::string_dup( m_videoOutputGroupData->getName().c_str() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }


    //
    // getGroupType
    //
    char* VideoOutputGroup::getGroupType() 
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return CORBA::string_dup( m_videoOutputGroupData->getGroupType().c_str() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }


    //
    // getTypeSize
    //
    char* VideoOutputGroup::getTypeSize() 
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return CORBA::string_dup( m_videoOutputGroupData->getTypeSize().c_str() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }


    //
    // getPosition
    //
    unsigned short VideoOutputGroup::getPosition()
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return m_videoOutputGroupData->getPosition();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }    
        return 0;
    }


    //
    // getGroupDimensionX
    //
    unsigned short VideoOutputGroup::getGroupDimensionX()
    {
    //    TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return m_videoOutputGroupData->getGroupDimensionX();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }


    //
    // getGroupDimensionY
    //
    unsigned short VideoOutputGroup::getGroupDimensionY()
    {
     //   TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        try
        {
            return m_videoOutputGroupData->getGroupDimensionY();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }    
        return 0;
    }


    //
    // getVideoOutputList
    //
    TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence* VideoOutputGroup::getVideoOutputList()
    {
    //    TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        if ( !m_isListLoaded )
        {
            loadVideoOutputs();
        }
            
        // This object is cleaned up by the ORB.

        TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence* videoOutputs = 
            new TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence();
        if ( 0 == videoOutputs )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Failed to create new VideoOutputSequence object." );
        }
        videoOutputs->length(  m_videoOutputs.size() );

        int i = 0;
        VideoOutputVector::iterator it = m_videoOutputs.begin();
        for( ; it != m_videoOutputs.end(); ++it )
        {
            //Mintao++
            //TD14315
			if( NULL != (*it))
			{
				(*videoOutputs)[i++] = (*it)->_this();

			}
            //Mintao++
            //TD14315
        }

        return videoOutputs;
    }


    //
    // isCapableOfLayouts
    //
    bool VideoOutputGroup::isCapableOfLayouts()
    {
        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        AgentModeMonitor::getInstance().verifyInControlMode( "isCapableOfLayouts" );
        try
        {
            return m_videoOutputGroupData->isCapableOfLayouts();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        return false;
    }


    /**
      * getVideoWallboardAgentName
      *
      * Gets the video wallboard agent name to use the layouts
      *
      * @return     string
      *             The unique video wallboard name.
      *
      * @exception  VideoSwitchAgentException
      *             Thrown if:
      *             1. The database cannot be accessed.
      *             2. The data cannot be retrieved.
      *             3. The retrieved data is invalid.
      *             4. The agent is in Monitor mode.
      *             5. The VideoOutputGroup is no capable of layouts
      */
    char* VideoOutputGroup::getVideoWallboardAgentName()
    {
    //    TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );
        
        //
        // If the VideoOutputGroup is not capable of layouts, then throw an exception
        //
        if ( ! this->isCapableOfLayouts () )
        {
            LOG_EXCEPTION_DETAILS ( SourceInfo, "VideoSwitchAgentException", "Requested Video Wallboard Agent Name, but the VideoOutputGroup is NOT capable of Video Wallboard Layout manipulation" );
            throw TA_Base_Bus::VideoSwitchAgentException( "Requested Video Wallboard Agent Name, but the VideoOutputGroup is NOT capable of Video Wallboard Layout manipulation" );
        }
        else
        {
            try
            {
                return CORBA::string_dup( m_videoOutputGroupData->getVideoWallboardAgentName().c_str() );
            }
            catch( const TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
                throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
            }
            catch( const TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
                throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
            }
        }
        return 0;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntity Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isValid
    //
    bool VideoOutputGroup::isValid()
    {
        return true;
    }


    //
    // start
    //
    void VideoOutputGroup::start() 
    {
        // Do nothing.
    }

    
    //
    // stop
    //
    void VideoOutputGroup::stop() 
    {
        // Deactivate this CORBA servant, so it no longer accepts incoming calls.
        // Don't call deactivateAndDelete() here because Generic Agent deletes this object.

        deactivateServant();   
    }

    
    //
    // update
    //
    void VideoOutputGroup::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        // FUTURE TODO - Update the m_videoOutputs vector if the relevant table changes.
        // FUTURE TODO - Update m_myCorbaName if it changes. 
    }

    
    //
    // remove
    //
    void VideoOutputGroup::remove()
    {
        // Remove the object from the SwitchManager object.

        try
        {
            SwitchManager::getInstance().removeVideoOutputGroup( m_entityKey );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to remove VideoOutputGroup from SwitchManager." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to remove VideoOutputGroup from SwitchManager." );
        }
        
        stop();
    }

    
    //
    // setToControlMode
    //
    void VideoOutputGroup::activateServantAtInit()
    {
        loadVideoOutputs();
        
        // Register this object so it can be resolved by the agent's clients.

        try
        {
            activateServantWithName( m_videoOutputGroupData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "VideoOutputGroup (name unknown) cannot be registered." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "VideoOutputGroup (name unknown) cannot be registered." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "VideoOutputGroup (name unknown) cannot be registered." );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "VideoOutputGroup (name unknown) cannot be registered." );
        }
     }

    
    //
    // setToMonitorMode
    //
    void VideoOutputGroup::deactivateServantAtClose()
    {
        // Deactivate the CORBA servant, so it no longer accepts incoming calls.
        // Don't call deactivateAndDelete() here because Generic Agent deletes this object.
        
        deactivateServant();    
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Helper Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // loadVideoOutputs
    //
    void VideoOutputGroup::loadVideoOutputs()
    {
        // Thread lock this method.

        TA_Base_Core::ThreadGuard guard( m_videoOutputsLock );

        // Check that the data hasn't been loaded while we've been waiting for the lock.

        if ( m_isListLoaded )
        {
            return;
        }

        TA_ASSERT( 0 != m_videoOutputGroupData.get(), "The video output group database object has not been set." );

        m_videoOutputs.clear();

        // Get the expected size of the list from the dimensions of the group.

        unsigned short size = getGroupDimensionX() * getGroupDimensionY();
        if ( 0 == size )
        {
            try
            {
                std::stringstream msg;
                msg << "Given the dimensions, no VideoOutputs are expected in VideoOutputGroup " << getName() << ".";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, msg.str().c_str() );
            }
            catch( const TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Given the dimensions, no VideoOutputs are expected in this VideoOutputGroup" );
            }
            return;
        }

        // Get the positions and entity keys of the VideoOutputs in the group.  

		//chenlei--
		//use the preloaded m_videoOutputsMap member
        //std::map< unsigned short, unsigned long > videoOutputs;
        try
        {
			//chenlei--
			//use the preloaded m_videoOutputsMap member
            //videoOutputs = TA_IRS_Core::VideoOutputGroupAccessFactory::getInstance().getVideoOutputGroup( 
            //    m_entityKey, false )->getVideoOutputs();
            if ( m_videoOutputsMap.empty() )
            {
                try
                {
                    std::stringstream msg;
                    msg << "There are no VideoOutputs associated with VideoOutputGroup " << getName() << ".";
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, msg.str().c_str() );
                }
                catch( const TA_Base_Core::DatabaseException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                    LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "There are no VideoOutputs associated with this VideoOutputGroup" );
                }
                return;
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            try
            {
                std::stringstream error;
                error << "The VideoOutputs associated with VideoOutputGroup " << getName() << " could not be determined.";
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException",  error.str().c_str() );
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                    "The VideoOutputs associated with VideoOutputGroup (name unknown) could not be determined." );
            }
            return;
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            try
            { 
                std::stringstream details;
                details << "The VideoOutputs associated with VideoOutputGroup " << getName() << " could not be determined.";
                LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", details.str().c_str() );
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", 
                    "The VideoOutputs associated with VideoOutputGroup (name unknown) could not be determined." );
            }
            return;
        }

        // Get the objects and put them into the vector to return.

        unsigned short position = 0;
        std::map< unsigned short, unsigned long >::iterator it;
        for( it = m_videoOutputsMap.begin(); it != m_videoOutputsMap.end(); it++ )
        {
            position++;

            // Add empty objects for any positions that have been missed.
            // This may be deliberate due to the layout of the monitors.
            // For example, if the monitor layout is as follows:
            //       XXX
            //      XXXXX
            //      XXXXX
            // Then positions 1 and 5 would be blank.

            if ( position != it->first )
            {
                m_videoOutputs.push_back( 0 ); 
                continue;
            }
            
            // There is a monitor for the current position.

            try
            {
                m_videoOutputs.push_back( &SwitchManager::getInstance().getVideoOutputInternal( it->second ) ); 
            }
            catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
                m_videoOutputs.push_back( 0 ); 
            }
        }

        m_isListLoaded = true;
    }

} // TA_IRS_App

