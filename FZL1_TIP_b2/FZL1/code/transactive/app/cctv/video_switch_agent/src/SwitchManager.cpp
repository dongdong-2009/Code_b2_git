/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Manages access to the entities (Cameras, VideoMonitors,
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * mplements the SwitchManager interface.
  *
  * Design Note: This used to have access to entities by name.  However, names
  * can be changed, so we use entity keys to identify entities instead.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
#include <algorithm>
#include <sstream>
#include <boost/foreach.hpp>

namespace TA_IRS_App
{
    //
    // getInstance
    //
    SwitchManager& SwitchManager::getInstance()
    {
        return *(ACE_Singleton<SwitchManager, ACE_Recursive_Thread_Mutex>::instance());
    }


    //
    // activate
    //
    void SwitchManager::activate()
    {
        TA_ASSERT(AgentModeMonitor::getInstance().isInControlMode(), "The agent is not in Control mode.");
        // The SwitchManager handles access to the agent, so activate it using the agent's name.
        activateServantWithName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
    }

    void SwitchManager::acknowledgeAlarm(const ::TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail, ::CORBA::ULong consoleId)
    {
		unsigned long alarmEntityKey = pAlarmDetail.assocEntityKey;
		if(m_cctvAlarms.find(alarmEntityKey) == m_cctvAlarms.end())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found alarm entity %lu", alarmEntityKey);
			return;
		}
		TA_Base_Core::DefaultEntity* alarmEntity = m_cctvAlarms[alarmEntityKey];
		try
		{
			unsigned long cameraKey;
			std::stringstream ss;
			ss << alarmEntity->getParameterValue("CameraKey");
			ss >> cameraKey;
			if(m_cameras.find(cameraKey) == m_cameras.end())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found camera %lu", alarmEntityKey);
				return;
			}
			Camera* camera = m_cameras[cameraKey];
			std::string cameraAddress = camera->getAddress();
			std::string parameter = cameraAddress + ";" + "0";
			CORBA::Any any;
			any <<= parameter.c_str();
            std::auto_ptr< TA_Base_Core::CommsMessageSender >
				messageSender(TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::VisualComms::Context));
			messageSender->sendCommsMessage(TA_Base_Core::VisualComms::VideoOutputStateUpdate,
				                            consoleId,
											any,
											pAlarmDetail.subsystemKey,
											pAlarmDetail.locationKey);


			
		}
		catch (...)
		{
		}
    }

    ///////////////////////////////////////////////////////////////////
    //
    // SwitchManagerCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////

    TA_Base_Bus::SwitchManagerCorbaDef::VideoObjectsInfo* SwitchManager::getVideoObjectsInfo()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::SwitchManagerCorbaDef::VideoObjectsInfo_var objectsInfo(new TA_Base_Bus::SwitchManagerCorbaDef::VideoObjectsInfo);
        // Fill up cameras info data
        TA_Base_Bus::SwitchManagerCorbaDef::CameraInfoSequence& camerasInfo = objectsInfo->camerasInfo;
        camerasInfo.length(m_cameras.size());
        int i = 0;
        CameraMap::iterator it = m_cameras.begin();
        for(; it != m_cameras.end(); ++i, ++it)
        {
            Camera& camera = *(it->second);
            camerasInfo[i].key = camera.getKey();
            camerasInfo[i].name = camera.getName();
            camerasInfo[i].address = camera.getAddress();
            camerasInfo[i].description = camera.getDescription();
            camerasInfo[i].groupName = camera.getVideoInputGroupName();
        }
        // Sequence
        TA_Base_Bus::SwitchManagerCorbaDef::SequenceInfoSequence& sequencesInfo = objectsInfo->sequencesInfo;
        sequencesInfo.length(m_sequences.size());
        i = 0;
        SequenceMap::iterator it2 = m_sequences.begin();
        for(; it2 != m_sequences.end(); ++i, ++it2)
        {
            Sequence& sequence = *(it2->second);
            sequencesInfo[i].key = sequence.getKey();
            sequencesInfo[i].name = sequence.getName();
            sequencesInfo[i].description = sequence.getDescription();
            sequencesInfo[i].groupName = sequence.getVideoInputGroupName();
            sequencesInfo[i].sequenceDescription = sequence.getSequenceDescription();
            sequencesInfo[i].isReadOnly = sequence.isReadOnly();
            sequencesInfo[i].sequenceConfig = *(sequence.getSequenceConfig());
        }
        // Monitor
        TA_Base_Bus::SwitchManagerCorbaDef::MonitorInfoSequence& monitorsInfo = objectsInfo->monitorsInfo;
        monitorsInfo.length(m_videoMonitors.size());
        i = 0;
        VideoMonitorMap::iterator it3 = m_videoMonitors.begin();
        for(; it3 != m_videoMonitors.end(); ++i, ++it3)
        {
            VideoMonitor& monitor = *(it3->second);
            monitorsInfo[i].key = monitor.getKey();
            monitorsInfo[i].name = monitor.getName();
            monitorsInfo[i].description = monitor.getDescription();
            monitorsInfo[i].address = monitor.getAddress();
            monitorsInfo[i].displayOrder = monitor.getDisplayOrder();
            monitorsInfo[i].consoleKey = monitor.getConsoleKey();
            monitorsInfo[i].isOverall = monitor.isOverallMonitor();
            monitorsInfo[i].outputState = *(monitor.getVideoOutputState());
        }
        return objectsInfo._retn();
    }

    //
    // getVideoMonitors
    //
    TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getVideoMonitors()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence());
        videoMonitors->length(m_videoMonitors.size());
        unsigned int index(0);
        for(VideoMonitorMap::iterator it = m_videoMonitors.begin(); it != m_videoMonitors.end(); ++it)
        {
            (*videoMonitors)[index++] = it->second->_this();
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return videoMonitors.release();
    }


    //
    // getVideoMonitor
    //
    TA_Base_Bus::VideoMonitorCorbaDef_ptr SwitchManager::getVideoMonitor(unsigned long entityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        VideoMonitorMap::iterator it = m_videoMonitors.find(entityKey);
        if(m_videoMonitors.end() == it)
        {
            std::stringstream error;
            error << "The VideoMonitor with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus::VideoSwitchAgentException(error.str().c_str());
        }
        return it->second->_this();
    }


    //
    // getAllConsoleMonitors
    //
    TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getAllConsoleMonitors()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence());
        unsigned int size(1);
        for(VideoMonitorMap::iterator it = m_videoMonitors.begin(); it != m_videoMonitors.end(); ++it)
        {
            if(0 != it->second->getConsoleKey())      // consoleKey of 0 indicates not console monitor.
            {
                (*videoMonitors).length(size);
                (*videoMonitors)[(size - 1) ] = it->second->_this();
                size++;
            }
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return videoMonitors.release();
    }


    //
    // getConsoleMonitors
    //
    TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getConsoleMonitors(
        unsigned long consoleKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence());
        unsigned int size(1);
        for(VideoMonitorMap::iterator it = m_videoMonitors.begin(); it != m_videoMonitors.end(); ++it)
        {
            if(consoleKey == (*it).second->getConsoleKey())
            {
                videoMonitors->length(size);
                (*videoMonitors)[(size - 1) ] = it->second->_this();
                size++;
            }
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return videoMonitors.release();
    }


    //
    // getCameras
    //
    TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence* SwitchManager::getCameras()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence > cameras =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence());
        cameras->length(m_cameras.size());
        unsigned int index(0);
        for(CameraMap::iterator it = m_cameras.begin(); it != m_cameras.end(); ++it)
        {
            (*cameras)[index++] = it->second->_this();
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return cameras.release();
    }


    //
    // getCamera
    //
    TA_Base_Bus::CameraCorbaDef_ptr SwitchManager::getCamera(unsigned long entityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        CameraMap::iterator it = m_cameras.find(entityKey);
        if(m_cameras.end() == it)
        {
            std::stringstream error;
            error << "The Camera with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus::VideoSwitchAgentException(error.str().c_str());
        }
        return it->second->_this();
    }


    //
    // getSequences
    //
    TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence* SwitchManager::getSequences()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence > sequences =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence());
        sequences->length(m_sequences.size());
        unsigned int index(0);
        for(SequenceMap::iterator it = m_sequences.begin(); it != m_sequences.end(); ++it)
        {
            (*sequences)[index++] = it->second->_this();
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return sequences.release();
    }


    //
    // getSequence
    //
    TA_Base_Bus::SequenceCorbaDef_ptr SwitchManager::getSequence(unsigned long entityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        SequenceMap::iterator it = m_sequences.find(entityKey);
        if(m_sequences.end() == it)
        {
            std::stringstream error;
            error << "The Sequence with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus::VideoSwitchAgentException(error.str().c_str());
        }
        return it->second->_this();
    }


    //
    // getVideoOutputs
    //
    TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence* SwitchManager::getVideoOutputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence > videoOutputs =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence());
        videoOutputs->length(m_videoOutputs.size());
        unsigned int index(0);
        for(VideoOutputMap::iterator it = m_videoOutputs.begin(); it != m_videoOutputs.end(); ++it)
        {
            (*videoOutputs)[index++] = it->second->_this();
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return videoOutputs.release();
    }


    //
    // getVideoOutput
    //
    TA_Base_Bus::VideoOutputCorbaDef_ptr SwitchManager::getVideoOutput(unsigned long entityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        VideoOutputMap::iterator it = m_videoOutputs.find(entityKey);
        if(m_videoOutputs.end() == it)
        {
            std::stringstream error;
            error << "The VideoOutput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus::VideoSwitchAgentException(error.str().c_str());
        }
        return it->second->_this();
    }


    //
    // getVideoInputs
    //
    TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence* SwitchManager::getVideoInputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
        std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence > videoInputs =
            std::auto_ptr< TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence >(
                new TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence());
        videoInputs->length(m_videoInputs.size());
        unsigned int index(0);
        for(VideoInputMap::iterator it = m_videoInputs.begin(); it != m_videoInputs.end(); ++it)
        {
            (*videoInputs)[index++] = it->second->_this();
        }
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.
        return videoInputs.release();
    }


    //
    // getVideoInput
    //
    TA_Base_Bus::VideoInputCorbaDef_ptr SwitchManager::getVideoInput(unsigned long entityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        VideoInputMap::iterator it = m_videoInputs.find(entityKey);
        if(m_videoInputs.end() == it)
        {
            std::stringstream error;
            error << "The VideoInput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus::VideoSwitchAgentException(error.str().c_str());
        }
        return it->second->_this();
    }


    void SwitchManager::clearAllMonitorsWithInput(unsigned long inputEntityKey, const char* sessionId)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* monitors = getVideoMonitors();
        if(monitors != NULL)
        {
            for(unsigned int i = 0; i < monitors->length(); ++i)
            {
                (*monitors)[i]->requestSwitchOfInput(0, sessionId, true);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////
    //
    // Add/Remove Entity Methods
    //
    ///////////////////////////////////////////////////////////////////


    //
    // addVideoMonitor
    //
    void SwitchManager::addVideoMonitor(VideoMonitor& videoMonitor)
    {
        unsigned long entityKey = videoMonitor.getKey();
        // If an object with this entityKey already exists - throw an exception.
        if(m_videoMonitors.end() != m_videoMonitors.find(entityKey))
        {
            std::stringstream error;
            error << "A VideoMonitor with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoMonitor will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        if(m_videoOutputs.end() != m_videoOutputs.find(entityKey))
        {
            std::stringstream error;
            error << "A VideoOutput with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoMonitor will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        // This is the only object with this entityKey, so add it to the map.
        m_videoMonitors[ entityKey ] = &videoMonitor;
        m_videoOutputs[ entityKey ] = &videoMonitor;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added VideoMonitor with entityKey %ul", entityKey);
    }


    //
    // removeVideoMonitor
    //
    void SwitchManager::removeVideoMonitor(unsigned long entityKey)
    {
        m_videoMonitors.erase(entityKey);
        m_videoOutputs.erase(entityKey);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed VideoMonitor with entityKey %ul", entityKey);
    }


    //
    // addCamera
    //
    void SwitchManager::addCamera(Camera& camera)
    {
        unsigned long entityKey = camera.getKey();
        // If an object with this entityKey already exists - throw an exception.
        if(m_cameras.end() != m_cameras.find(entityKey))
        {
            std::stringstream error;
            error << "A Camera with the entityKey " << entityKey << " already exists. ";
            error << "The new Camera will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        if(m_videoInputs.end() != m_videoInputs.find(entityKey))
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new Camera will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        // This is the only object with this entityKey, so add it to the map.
        m_cameras[ entityKey ] = &camera;
        m_videoInputs[ entityKey ] = &camera;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added Camera with entityKey %ul", entityKey);
    }


    //
    // removeCamera
    //
    void SwitchManager::removeCamera(unsigned long entityKey)
    {
        m_cameras.erase(entityKey);
        m_videoInputs.erase(entityKey);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed Camera with entityKey %ul", entityKey);
    }


    //
    // addSequence
    //
    void SwitchManager::addSequence(Sequence& sequence)
    {
        unsigned long entityKey = sequence.getKey();
        // If an object with this entityKey already exists - throw an exception.
        if(m_sequences.end() != m_sequences.find(entityKey))
        {
            std::stringstream error;
            error << "A Sequence with the entityKey " << entityKey << " already exists. ";
            error << "The new Sequence will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        if(m_videoInputs.end() != m_videoInputs.find(entityKey))
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new Sequence will not be added.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS, error.str().c_str()));
        }
        // This is the only object with this entityKey, so add it to the map.
        m_sequences[ entityKey ] = &sequence;
        m_videoInputs[ entityKey ] = &sequence;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added Sequence with entityKey %ul", entityKey);
    }


    //
    // removeSequence
    //
    void SwitchManager::removeSequence(unsigned long entityKey)
    {
        m_sequences.erase(entityKey);
        m_videoInputs.erase(entityKey);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed Sequence with entityKey %ul", entityKey);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Helper Methods
    //
    ///////////////////////////////////////////////////////////////////


    //
    // getVideoOutputInternal
    //
    VideoOutput& SwitchManager::getVideoOutputInternal(unsigned long entityKey)
    {
        VideoOutputMap::iterator it = m_videoOutputs.find(entityKey);
        if(m_videoOutputs.end() == it)
        {
            std::stringstream error;
            error << "The VideoOutput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT, error.str().c_str()));
        }
        return *(it->second);
    }


    //
    // getVideoInputInternal
    //
    VideoInput& SwitchManager::getVideoInputInternal(unsigned long entityKey)
    {
        VideoInputMap::iterator it = m_videoInputs.find(entityKey);
        if(m_videoInputs.end() == it)
        {
            std::stringstream error;
            error << "The VideoInput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT, error.str().c_str()));
        }
        return *(it->second);
    }


    //
    // getCameraInternal
    //
    Camera& SwitchManager::getCameraInternal(unsigned long entityKey)
    {
        CameraMap::iterator it = m_cameras.find(entityKey);
        if(m_cameras.end() == it)
        {
            std::stringstream error;
            error << "The Camera with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT, error.str().c_str()));
        }
        return *(it->second);
    }

    //
    // getSequenceInternal
    //
    Sequence& SwitchManager::getSequenceInternal(unsigned long entityKey)
    {
        SequenceMap::iterator it = m_sequences.find(entityKey);
        if(m_sequences.end() == it)
        {
            std::stringstream error;
            error << "The Sequence with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_INPUT, error.str().c_str()));
        }
        return *(it->second);
    }


    //
    // SwitchManager
    //
    SwitchManager::SwitchManager()
    {
    }


    SwitchManager::~SwitchManager()
    {
    }


	std::vector<Camera*> SwitchManager::getInternalCameras()
	{
		std::vector<Camera*> cameras;
		BOOST_FOREACH(CameraMap::value_type& i, m_cameras)
		{
			cameras.push_back(i.second);
		}
		return cameras;
	}

	std::vector<Sequence*> SwitchManager::getInternalSequences()
	{
		std::vector<Sequence*> seuences;
		BOOST_FOREACH(SequenceMap::value_type& i, m_sequences)
		{
			seuences.push_back(i.second);
		}
		return seuences;
	}

	std::vector<VideoMonitor*> SwitchManager::getInternalMonitors()
	{
		std::vector<VideoMonitor*> monitors;
		BOOST_FOREACH(VideoMonitorMap::value_type& i, m_videoMonitors)
		{
			monitors.push_back(i.second);
		}
		return monitors;
	}

	void SwitchManager::addCctvAlarmEntity(TA_Base_Core::DefaultEntity* cctvAlarmEntity)
	{
		m_cctvAlarms.insert(std::make_pair(cctvAlarmEntity->getKey(), cctvAlarmEntity));
	}
} // TA_IRS_App
