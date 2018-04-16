/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/SwitchAgentCommunicator.cpp $
  * @author:  Robert Young
  * @version: $Revision: #10 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  *
  * Singleton to provide access to Agent data and functions
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/GlobalConstants.h"

#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#include "core/alarm/idl/src/AlarmMessageCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/exceptions/src/TransActiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

using CORBA::String_var;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Bus::VideoSwitchAgentFactory;
typedef TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject VideoMonitorNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject CameraNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject SequenceNamedObject;

namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    SwitchAgentCommunicator* SwitchAgentCommunicator::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable SwitchAgentCommunicator::m_singletonLock;

    //
    // getInstance
    //
    SwitchAgentCommunicator& SwitchAgentCommunicator::getInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        if(m_theInstance == 0)
        {
            m_theInstance = new SwitchAgentCommunicator();
        }
        return *m_theInstance;
    }


    //
    // removeInstance
    //
    void SwitchAgentCommunicator::removeInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        if(m_theInstance != 0)
        {
            delete m_theInstance;
            m_theInstance = 0;
        }
    }


    //
    // Constructor
    //
    SwitchAgentCommunicator::SwitchAgentCommunicator()
    {
    }


    SwitchAgentCommunicator::~SwitchAgentCommunicator()
    {
        for(CamKeyToNamedObject::iterator it = m_camKeyToNamedObject.begin(); it != m_camKeyToNamedObject.end(); ++it)
        {
            delete (*it).second;
        }
        for(SequenceKeyToNamedObject::iterator it = m_seqKeyToNamedObject.begin(); it != m_seqKeyToNamedObject.end(); ++it)
		{
			delete (*it).second;
		}
        for(MonitorKeyToNamedObject::iterator it = m_monKeyToNamedObject.begin(); it != m_monKeyToNamedObject.end(); ++it)
        {
            delete(*it).second;
        }
    }



    void SwitchAgentCommunicator::assignMonitor(Monitor* monitor, VideoInput* source)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
        TA_ASSERT(source != NULL, "Video Input is NULL");
        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        VideoMonitorNamedObject* monObj = NULL;
        monObj = getMonObjByKey(monitor->getKey());
        if(NULL == monObj)
        {
            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fail to assignMonitor(%ld,%ld),named object is null", monitor->getKey(), source->getKey());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
        try
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignMonitor(%ld,%ld)", monitor->getKey(), source->getKey());
            CORBA_CALL((*monObj), requestSwitchOfInput, (source->getKey(), session.c_str(), true));
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while assigning monitor: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        // update the state objects
        monitor->assignInput(source);
        // tell the GUI to update its display
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
    }


    void SwitchAgentCommunicator::assignQuad(Monitor* monitor, Camera* topLeftCam, Camera* topRightCam,
                                             Camera* bottomLeftCam, Camera* bottomRightCam)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        VideoMonitorNamedObject* monObj = NULL;
        monObj = getMonObjByKey(monitor->getKey());
        if(NULL == monObj)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fail to assignQuad(%ld),named object is null", monitor->getKey());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
        // if the camera is not null - get the key, if it is, then set the key to 0
        unsigned long topLeft = (topLeftCam != NULL) ? topLeftCam->getKey() : 0;
        unsigned long topRight = (topRightCam != NULL) ? topRightCam->getKey() : 0;
        unsigned long bottomLeft = (bottomLeftCam != NULL) ? bottomLeftCam->getKey() : 0;
        unsigned long bottomRight = (bottomRightCam != NULL) ? bottomRightCam->getKey() : 0;
        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            char chCameras[1024] = {0};
            sprintf(chCameras, "(%ld,%ld,%ld,%ld)", topLeft, topRight, bottomLeft, bottomRight);
            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignQuad(%ld,%s)", monitor->getKey(), chCameras);
            CORBA_CALL((*monObj), assignQuad, (topLeft, topRight, bottomLeft, bottomRight, session.c_str(), true));
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while assigning quad: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        monitor->assignQuad(topLeftCam, topRightCam, bottomLeftCam, bottomRightCam);
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
    }


    void SwitchAgentCommunicator::setSequenceConfig(Sequence* sequence,
                                                    unsigned short dwellTime,
                                                    const std::list< Camera* > cameraInputs)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        TA_ASSERT(sequence != NULL, "Destination Sequence is NULL");
        // get the quad corba object
        SequenceNamedObject* seqObj = NULL;
        seqObj = getSeqObjByKey(sequence->getKey());
        if(NULL == seqObj)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fail to setSequenceConfig(%ld),named object is null", sequence->getKey());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
        std::string strKeys;
        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_Base_Bus::SequenceCorbaDef::SequenceConfig sequenceConfig;
            sequenceConfig.dwellTime = dwellTime;
            sequenceConfig.inputKeySequence.length(cameraInputs.size());
            int index = 0;
            for(std::list< Camera* >::const_iterator iter = cameraInputs.begin();
                    iter != cameraInputs.end(); iter++, index++)
            {
                TA_ASSERT(*iter != NULL, "New Camera list contains a NULL camera");
                sequenceConfig.inputKeySequence[index] = (*iter)->getKey();
                char chKey[32] = {0};
                sprintf(chKey, "%ld", (*iter)->getKey());
                strKeys.append(chKey);
                strKeys.append(" . ");
            }
            LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSequenceConfig(%ld,%d,%s)", sequence->getKey() , dwellTime, strKeys.c_str());
            CORBA_CALL((*seqObj), setSequenceConfig, (sequenceConfig, session.c_str()));
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while updating sequence: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        // update the local state
        sequence->setDwellTime(dwellTime);
        sequence->setCameras(cameraInputs);
        // tell the GUI to update
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(sequence);
    }


    void SwitchAgentCommunicator::setSequenceDescription(Sequence* sequence,
                                                         const std::string& newDescription)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        TA_ASSERT(sequence != NULL, "Destination Sequence is NULL");
        // get the quad corba object
        SequenceNamedObject* seqObj = NULL;
        seqObj = getSeqObjByKey(sequence->getKey());
        if(NULL == seqObj)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fail to setSequenceDescription(%ld),named object is null", sequence->getKey());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSequenceDescription(%ld,%s)", sequence->getKey() , newDescription.c_str());
            CORBA_CALL((*seqObj), setSequenceDescription, (newDescription.c_str(), session.c_str()));    
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while updating sequence description: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        // update the local state
        sequence->setSequenceDescription(newDescription);
        // tell the GUI to update
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(sequence);
    }


    void SwitchAgentCommunicator::clearMonitor(Monitor* monitor)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_singletonLock);
        TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            VideoMonitorNamedObject* monObj = NULL;
            monObj = getMonObjByKey(monitor->getKey());
            if(NULL == monObj)
            {
                LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fail to clearMonitor(%ld),named object is null", monitor->getKey());
				throw EXCEPTION_CANNOT_CONTACT_AGENT;
            }
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearMonitor(%ld)", monitor->getKey());
            CORBA_CALL((*monObj), requestSwitchOfInput, (0, session.c_str(), true));      
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while clearing monitor: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        // update the state objects
        monitor->assignInput(NULL);
        monitor->clearAlarmStack();
        monitor->setShowingAckAlarm(false);
        // tell the GUI to update its display
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
    }

	void SwitchAgentCommunicator::acknowledgeAlarm( unsigned long entityKey)
	{
        ThreadGuard guard(m_singletonLock);
        try
        {
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
            unsigned long localConsoleKey = console->getKey();
            TA_Base_Core::AlarmDetailCorbaDef alarm;
            alarm.alarmID = CORBA::string_dup("nouse");
            alarm.assocEntityKey = entityKey;
            alarm.mmsState = TA_Base_Core::MMS_ALM_SENT;
            alarm.decisionSupportState = TA_Base_Core::NoRelatedPlan;
            alarm.state = TA_Base_Core::AlarmOpen;
            TA_Base_Bus::VideoMonitorCorbaDef_var videoMonitor;
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "acknowledgeAlarm(%d)", entityKey);
            CORBA_CALL(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(localConsoleKey), acknowledgeAlarm, (alarm, localConsoleKey));
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
			throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while acknowledgeAlarm: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
        }
	}

    void SwitchAgentCommunicator::updateSequence(Sequence* sequence)
    {
		TA_ASSERT(NULL != sequence, "");
		SequenceNamedObject* seqObj = getSeqObjByKey(sequence->getKey());
		if(NULL == seqObj)
		{
			return;
		}
        try
        {
            // the only 2 things that can change are the description and the read only flag
            String_var description;
            CORBA_CALL_RETURN(description, (*seqObj), getSequenceDescription, ());
            sequence->setSequenceDescription(description.in());
            CORBA::Boolean ret;
            CORBA_CALL_RETURN(ret, (*seqObj), isReadOnly, ());
            sequence->setReadOnly(ret);
        }
        catch(TA_Base_Bus::VideoSwitchAgentException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in());
            throw TransactiveException(ex.what.in());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while updating sequence: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
        }
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			throw EXCEPTION_CANNOT_CONTACT_AGENT;
		}
		catch(...)
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown");
			throw EXCEPTION_PERFORMING_REQUEST;
		}
    }

    VideoMonitorNamedObject* SwitchAgentCommunicator::getMonObjByKey(const unsigned long key)
    {
		VideoMonitorNamedObject* monObj = NULL;
        MonitorKeyToNamedObject::iterator it = m_monKeyToNamedObject.find(key);
        if(it != m_monKeyToNamedObject.end())
        {
			monObj = it->second;
        }
		else
		{
			try
			{
				monObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoMonitorNamedObject(key);
				m_monKeyToNamedObject.insert(std::make_pair(key, monObj));
			}
			catch (...)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception when loading monitor name object.");
			}
		}
		return monObj;
    }


    CameraNamedObject* SwitchAgentCommunicator::getCamObjByKey(const unsigned long key)
    {
		CameraNamedObject* camObj = NULL;
		CamKeyToNamedObject::iterator it = m_camKeyToNamedObject.find(key);
        if(it != m_camKeyToNamedObject.end())
        {
			camObj = it->second;
        }
		else
		{
			try
			{
				camObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera(key);
				m_camKeyToNamedObject.insert(std::make_pair(key, camObj));
			}
			catch (...)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception when loading cam name object.");
			}
		}
		return camObj;
    }


    SequenceNamedObject* SwitchAgentCommunicator::getSeqObjByKey(const unsigned long key)
    {
		SequenceNamedObject* seqObj = NULL;
		SequenceKeyToNamedObject::iterator it = m_seqKeyToNamedObject.find(key);
		if(it != m_seqKeyToNamedObject.end())
		{
			seqObj = it->second;
		}
		else
		{
			try
			{
				seqObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSequence(key);
				m_seqKeyToNamedObject.insert(std::make_pair(key, seqObj));
			}
			catch (...)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception when loading sequence name object.");
			}
		}
        return seqObj;
    }

} // namespace TA_IRS_App