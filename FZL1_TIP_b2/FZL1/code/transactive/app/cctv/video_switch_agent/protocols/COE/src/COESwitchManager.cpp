/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Rob Young
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Manages access to the entities (Cameras, VideoMonitors,
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <sstream>

namespace TA_IRS_App
{
    // Constructor
    //
    COESwitchManager::COESwitchManager()
        : m_commsHandler(0)
    {
    }



    COESwitchManager::~COESwitchManager()
    {
    }


    //
    // getInstance
    //
    COESwitchManager& COESwitchManager::getInstance()
    {
        return *(ACE_Singleton<COESwitchManager, ACE_Recursive_Thread_Mutex>::instance());
    }



    //
    // getCamera
    //
    COECamera* COESwitchManager::getCamera(unsigned long entityKey)
    {
        COECamera* cam = NULL;
        CameraMap::iterator it = m_cameras.find(entityKey);
        if(m_cameras.end() != it)
        {
            cam = it->second;
        }
        return cam;
    }


    //
    // getSequence
    //
    COESequence* COESwitchManager::getSequence(unsigned long entityKey)
    {
        COESequence* seq = NULL;
        SequenceMap::iterator it = m_sequences.find(entityKey);
        if(m_sequences.end() != it)
        {
            seq = it->second;
        }
        return seq;
    }


    //
    // getVideoOutput
    //
    COEVideoOutput* COESwitchManager::getVideoOutput(unsigned long entityKey)
    {
        COEVideoOutput* output = NULL;
        VideoOutputMap::iterator it = m_videoOutputs.find(entityKey);
        if(m_videoOutputs.end() != it)
        {
            output = it->second;
        }
        return output;
    }


    //
    // getAllVideoOutputsWithAssignment
    //
    std::vector<COEVideoOutput*> COESwitchManager::getAllVideoOutputsWithAssignment(unsigned long entityKey)
    {
        std::vector<COEVideoOutput*> videoOutputs;
        for(VideoOutputMap::iterator it = m_videoOutputs.begin(); it != m_videoOutputs.end(); ++it)
        {
            COEVideoOutput* output = (*it).second;
            if(output->getCurrentVideoInputKey() == entityKey)
            {
                videoOutputs.push_back(output);
            }
        }
        return videoOutputs;
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Add/Remove Entity Methods
    //
    ///////////////////////////////////////////////////////////////////


    // addCamera
    //
    void COESwitchManager::addCamera(COECamera& camera)
    {
        unsigned long entityKey = camera.getKey();
        // If an object with this entityKey already exists - throw an exception.
        if(m_cameras.end() == m_cameras.find(entityKey))
        {
            std::pair< unsigned short, unsigned short > camAddr = camera.getParsedAddress();
            m_cameras[ entityKey ] = &camera;
            m_inputTypes[ entityKey ] = CAMERA;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added COECamera with entityKey %ul", entityKey);
        }
        // This is the only object with this entityKey, so add it to the maps.
    }


    //
    // removeCamera
    //
    void COESwitchManager::removeCamera(unsigned long entityKey)
    {
        CameraMap::iterator it = m_cameras.find(entityKey);
        if(m_cameras.end() != it)
        {
            m_cameras.erase(it);
        }
        m_inputTypes.erase(entityKey);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed COECamera with entityKey %ul", entityKey);
    }


    //
    // addSequence
    //
    void COESwitchManager::addSequence(COESequence& sequence)
    {
        unsigned long entityKey = sequence.getKey();
        if(m_sequences.end() == m_sequences.find(entityKey))
        {
            unsigned short sequenceId = TA_COE::getIdFromAddress(sequence.getAddress());
            m_sequences[ entityKey ] = &sequence;
            m_inputTypes[ entityKey ] = SEQUENCE;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added COESequence with entityKey %ul", entityKey);
        }
    }


    //
    // removeSequence
    //
    void COESwitchManager::removeSequence(unsigned long entityKey)
    {
        SequenceMap::iterator it = m_sequences.find(entityKey);
        if(m_sequences.end() != it)
        {
            m_sequences.erase(it);
        }
        m_inputTypes.erase(entityKey);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed COESequence with entityKey %ul", entityKey);
    }


    //
    // addVideoOutput
    //
    void COESwitchManager::addVideoOutput(COEVideoOutput& videoOutput)
    {
        unsigned long entityKey = videoOutput.getKey();
        if(m_videoOutputs.end() == m_videoOutputs.find(entityKey))
        {
            unsigned short videoOutputId = TA_COE::getIdFromAddress(videoOutput.getAddress());
            m_videoOutputs[ entityKey ] = &videoOutput;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added VideoOutput with entityKey %ul", entityKey);
        }
    }


    //
    // removeVideoOutput
    //
    void COESwitchManager::removeVideoOutput(unsigned long entityKey)
    {
        VideoOutputMap::iterator it = m_videoOutputs.find(entityKey);
        if(m_videoOutputs.end() != it)
        {
            m_videoOutputs.erase(it);
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed VideoOutput with entityKey %ul", entityKey);
    }


    COESwitchManager::ECOEInputType COESwitchManager::getInputType(unsigned long entityKey)
    {
        InputTypeMap::iterator it = m_inputTypes.find(entityKey);
        if(it == m_inputTypes.end())
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The InputType does not exist. "));
        }
        return (*it).second;
    }

    //
    // getCommunicationsHandler
    //
    COECommunicationsHandler* COESwitchManager::getCommunicationsHandler()
    {
        if(0 == m_commsHandler)
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The COECommunicationsHandler does not exist. "));
        }
        return m_commsHandler;
    }


    //
    // addCommunicationsHander
    //
    void COESwitchManager::addCommunicationsHander(COECommunicationsHandler& commsHandler)
    {
        TA_ASSERT(m_commsHandler == 0, "The COECommunicationsHandler already exists");
        m_commsHandler = &commsHandler;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added COECommunicationsHandler");
    }


    //
    // removeCommunicationsHander
    //
    void COESwitchManager::removeCommunicationsHander()
    {
        m_commsHandler = 0;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed COECommunicationsHandler.");
    }
}

