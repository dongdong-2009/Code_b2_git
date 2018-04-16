/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/VideoSwitchResource.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles all the common functions of video switch resources.
  *
  * NOTE: When inheriting from this class, if you implement the TA_Base_Bus::IEntity
  * methods, you need to call the TA_Base_Bus::IEntity methods in this class as part
  * of the overriding methods.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/VideoSwitchResource.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{

    //
    // VideoSwitchResource
    //
    VideoSwitchResource::VideoSwitchResource(TA_Base_Core::IEntityDataPtr entityData)
        : m_videoSwitchResourceData(boost::dynamic_pointer_cast<TA_Base_Core::VideoSwitchResource>(entityData))
    {
        if(0 == m_videoSwitchResourceData.get())
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG, "The provided entity data is not VideoSwitchResource data."));
        }
        getKey();
    }


    //
    // ~VideoSwitchResource
    //
    VideoSwitchResource::~VideoSwitchResource()
    {
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
    }


    //
    // getVideoSwitchResourceData
    //
    TA_Base_Core::VideoSwitchResourcePtr& VideoSwitchResource::getVideoSwitchResourceData()
    {
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The database object for this VideoSwitchResource has not been initialised.");
        return m_videoSwitchResourceData;
    }


    //
    // getSubsystemKey
    //
    unsigned long VideoSwitchResource::getSubsystemKey()
    {
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        return m_videoSwitchResourceData->getSubsystem();
    }


    //
    // getLocationKey
    //
    unsigned long VideoSwitchResource::getLocationKey()
    {
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        return m_videoSwitchResourceData->getLocation();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    //
    // VideoSwitchResourceCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getKey
    //
    unsigned long VideoSwitchResource::getKey()
    {
        //TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        try
        {
            return m_videoSwitchResourceData->getKey();
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        return 0;
    }


    //
    // getName
    //
    char* VideoSwitchResource::getName()
    {
        //TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        try
        {
            return CORBA::string_dup(m_videoSwitchResourceData->getName().c_str());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        return 0;
    }


    //
    // getType
    //
    char* VideoSwitchResource::getType()
    {
        //TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        return CORBA::string_dup(m_videoSwitchResourceData->getType().c_str());           // This doesn't throw any exceptions.
    }


    //
    // getDescription
    //
    char* VideoSwitchResource::getDescription()
    {
        //TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        try
        {
            return CORBA::string_dup(m_videoSwitchResourceData->getDescription().c_str());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        return 0;
    }


    //
    // getAddress
    //
    char* VideoSwitchResource::getAddress()
    {
        //TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The VideoSwitchResource database object has not been set.");
        try
        {
            return CORBA::string_dup(m_videoSwitchResourceData->getAddress().c_str());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
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
    bool VideoSwitchResource::isValid()
    {
        return true;
    }


    //
    // start
    //
    void VideoSwitchResource::start()
    {
    }


    //
    // stop
    //
    void VideoSwitchResource::stop()
    {
    }


    //
    // update
    //
    void VideoSwitchResource::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        TA_ASSERT(0 != m_videoSwitchResourceData.get(), "The database object for the VideoSwitchResource has not been initialised.");
        m_videoSwitchResourceData->invalidate();
        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
        // FUTURE TODO If they no longer support superimposed text control, it should be removed.
    }

    //
    // remove
    //
    void VideoSwitchResource::remove()
    {
        stop();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


} // namespace TA_IRS_App
