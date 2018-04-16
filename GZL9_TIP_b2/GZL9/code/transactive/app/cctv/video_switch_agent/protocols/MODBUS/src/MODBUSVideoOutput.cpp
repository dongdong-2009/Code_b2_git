/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a VideoOutput
  * using the MODBUS protocol.
  * Inherits from IPDVideoOutput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <sstream>
#include <iostream>
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/SetSequenceConfigCommand.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
/*
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/PTZCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/ActiveVideoInputCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/ControlSequencePlaybackCommand.h"
*/
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/CommandPackage.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
//#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

namespace TA_IRS_App
{
//
// MODBUSVideoOutput
//
namespace
{
static const int MAX_SOURCES_PER_SEQUENCE = 64;
}

MODBUSVideoOutput::MODBUSVideoOutput(TA_Base_Core::VideoOutputPtr videoOutputData)
        : m_videoOutputData(videoOutputData),
        m_currentVideoInputKey(0),
        m_isInAlarmMode(false)
{
	/*
	unsigned long inputKey = 0;
	std::string desc;
	TA_IRS_Core::SequenceHelper helper;
	try
	{
		helper.getRelationShipFromParentKey(m_videoOutputData->getKey(), inputKey, desc);
	}
	catch (...)
	{
	}
	m_currentVideoInputKey = inputKey;
	*/
    MODBUSSwitchManager::getInstance().addVideoOutput(*this);
//        MODBUSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );

    // Commented for TD16066
    /*		TA_IRS_Core::IVideoTriggeringEvents triggeringData = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents();
    		TA_IRS_Core::IVideoTriggeringEvents::iterator itr;
    		for(itr = triggeringData.begin(); itr != triggeringData.end(); itr++)
    		{
    			TA_MODBUS::AlarmState state;
    			state.eventId = (*itr)->getEventId();
    			state.stationId = (*itr)->getStationId();
    			unsigned long pkey = (*itr)->getKey();

    			m_alarmStateToId[state] = pkey;
    			m_idToAlarmState[pkey] = state;
    		}*/
}


//
// getKey
//
unsigned long MODBUSVideoOutput::getKey()
{
    return m_videoOutputData->getKey();
}


int MODBUSVideoOutput::getDisplayOrder()
{
	TA_Base_Core::VideoMonitorPtr p = boost::dynamic_pointer_cast<TA_Base_Core::VideoMonitor>(m_videoOutputData);
    if (p)
    {
        return p->getDisplayOrder();
    }
    return -1;
}

unsigned long MODBUSVideoOutput::getConsoleKey()
{
	TA_Base_Core::VideoMonitorPtr p = boost::dynamic_pointer_cast<TA_Base_Core::VideoMonitor>(m_videoOutputData);
    if (p)
    {
        return p->getConsoleKey();
    }
	return 0;
}

//
// getAddress
//
std::string MODBUSVideoOutput::getAddress()
{
    try
    {
        return m_videoOutputData->getAddress();
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
        LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
        throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
        LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
        throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
    }
}


///////////////////////////////////////////////////////////////////////
//
// State Update Methods
//
///////////////////////////////////////////////////////////////////////


//
// notifyInputSwitchedToOutput
//
void MODBUSVideoOutput::notifyInputSwitchedToOutput(unsigned long videoInputKey)
{
    if (m_isInAlarmMode || m_currentVideoInputKey != videoInputKey)
    {

        m_isInAlarmMode = false;
        m_alarmStack.clear();
        m_currentVideoInputKey = videoInputKey;
        try
        {
            StateUpdateBroadcaster::getInstance().notifyVideoOutputStateUpdate(m_videoOutputData->getKey());
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
        }
        catch (const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
        }
    }
}


/**
  *
  */
void MODBUSVideoOutput::notifyOutputShowingAlarmStack(TA_MODBUS::AlarmStack alarmStack)
{
    if (!m_isInAlarmMode || m_alarmStack != alarmStack)
    {
        m_isInAlarmMode = true;
        m_alarmStack = alarmStack;
        m_currentVideoInputKey = 0;
        try
        {
            StateUpdateBroadcaster::getInstance().notifyVideoOutputStateUpdate(m_videoOutputData->getKey());
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
        }
        catch (const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
        }
    }
}

//
// updateState
//
void MODBUSVideoOutput::updateState(const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state)
{
    m_currentVideoInputKey = state.currentVideoInputKey;
    m_isInAlarmMode = state.isInAlarmMode;

    m_alarmStack = alarmStateSequenceToAlarmStack(state.alarmStack);
}


///////////////////////////////////////////////////////////////////////
//
// IPDVideoOutput Methods
//
///////////////////////////////////////////////////////////////////////


//
// getCurrentVideoInputName
//
unsigned long MODBUSVideoOutput::getCurrentVideoInputKey()
{
    return m_currentVideoInputKey;
}


//
// getVideoOutputState
//
TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* MODBUSVideoOutput::getVideoOutputState()
{
    TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* state = new TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState();
    state->currentVideoInputKey = m_currentVideoInputKey;
    state->alarmStack = alarmStateSequenceFromAlarmStack(m_alarmStack);
    state->isInAlarmMode = m_isInAlarmMode;

    return state;
}

// MODBUS
void MODBUSVideoOutput::requestSingleCamToOutput(unsigned long camKey)
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());

    MODBUSCamera *cam = MODBUSSwitchManager::getInstance().getCamera(camKey);
    TA_ASSERT(cam != NULL, "");
    //unsigned char stationID = TA_IRS_Core::VideoStationIdAccessFactory::getInstance().getVideoStationIdByLocation(cam->getLocationKey())->getStationId();
    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
	unsigned char cameraID = (cameraAddress & 0xffu);
	unsigned char stationID = (cameraAddress >> 8);

    MODBUSSwitchManager::getInstance().getCommunicationsHandler()->assignCameraToMonitor(monitorID, stationID, cameraID);
}

void MODBUSVideoOutput::requestSequenceToOutput(unsigned long seqKey)
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());
    MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(seqKey);
    TA_ASSERT(seq != NULL, "");
    bool isAutoSwitchSeq = seq->isReadOnly();
    if (isAutoSwitchSeq)
    {
        unsigned char sequenceID = boost::lexical_cast<unsigned short>(seq->getAddress());
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->startAutoSwitch(sequenceID, monitorID);
    }
    else
    {
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig_var seqConfig = seq->getSequenceConfig();
        std::vector<ManualSequenceConfig> manualSeqConfig;
        for (size_t i = 0; i < seqConfig->inputKeySequence.length(); ++i)
        {
            unsigned long camEntityKey = seqConfig->inputKeySequence[i];
            MODBUSCamera* cam = MODBUSSwitchManager::getInstance().getCamera(camEntityKey);
            TA_ASSERT(cam != NULL, "");

		    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
			unsigned char cameraID = (cameraAddress & 0xffu);
			unsigned char stationID = (cameraAddress >> 8);

            manualSeqConfig.push_back(ManualSequenceConfig(cameraID, stationID, seqConfig->dwellTime));
        }
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->startManualSequence(monitorID, manualSeqConfig);
    }

}

void MODBUSVideoOutput::deactiveSequence(unsigned long seqKey)
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());
    MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(seqKey);
    TA_ASSERT(seq != NULL, "");
    bool isAutoSwitchSeq = seq->isReadOnly();
    if (isAutoSwitchSeq)
    {
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->stopAutoSwitchMode(monitorID);
    }
    else
    {
        std::vector<ManualSequenceConfig> manualSeqConfig;
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->stopManualSequence(monitorID);
    }
}

void MODBUSVideoOutput::requestQuadToOutput(unsigned long quadKey)
{
    std::string outPutAddress = m_videoOutputData->getAddress();
    MODBUSQuad* quad = MODBUSSwitchManager::getInstance().getQuad(quadKey);
    if (quad == 0)
    {
        std::stringstream ss;
        ss << "Not find coequad object" << quadKey;
        throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                                                              ss.str());
    }
    std::string quadAddress = quad->getAddress();
    //MODBUSSwitchManager::getInstance().getCommunicationsHandler()->requestQuadToOutput(quadAddress, outPutAddress);
}

void MODBUSVideoOutput::requestCamsToQuad(unsigned long topRightCamKey, unsigned long topLeftCamKey, unsigned long botRightCamKey, unsigned long botLeftCamKey)
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());
    unsigned char topRightCamID = 0, topLeftCamID = 0, botRightCamID = 0, botLeftCamID = 0;
    unsigned char topRightCamStationID = 0, topLeftCamStationID = 0, botRightCamStationID = 0, botLeftCamStationID = 0;
    if (topRightCamKey != 0)
    {
        MODBUSCamera *cam = MODBUSSwitchManager::getInstance().getCamera(topRightCamKey);
        TA_ASSERT(cam != NULL, "");
	    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
		topRightCamStationID = (cameraAddress >> 8);
		topRightCamID = (cameraAddress & 0xffu);
    }
    if (topLeftCamKey != 0)
    {
        MODBUSCamera *cam = MODBUSSwitchManager::getInstance().getCamera(topLeftCamKey);
        TA_ASSERT(cam != NULL, "");
		
	    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
		topLeftCamStationID = (cameraAddress >> 8);
		topLeftCamID = (cameraAddress & 0xffu);
    }
    if (botRightCamKey != 0)
    {
        MODBUSCamera *cam = MODBUSSwitchManager::getInstance().getCamera(botRightCamKey);
        TA_ASSERT(cam != NULL, "");

	    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
		botRightCamStationID = (cameraAddress >> 8);
		botRightCamID = (cameraAddress & 0xffu);
    }
    if (botLeftCamKey != 0)
    {
        MODBUSCamera *cam = MODBUSSwitchManager::getInstance().getCamera(botLeftCamKey);
        TA_ASSERT(cam != NULL, "");

	    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
		botLeftCamStationID = (cameraAddress >> 8);
		botLeftCamID = (cameraAddress & 0xffu);
    }
    MODBUSSwitchManager::getInstance().getCommunicationsHandler()->assignCameraToQuad(monitorID,
            topRightCamStationID, topRightCamID,
            topLeftCamStationID, topLeftCamID,
            botRightCamStationID, botRightCamID,
            botLeftCamStationID, botLeftCamID);
}

void MODBUSVideoOutput::requestSeqsToQuad(unsigned long topRightSeqKey, unsigned long topLeftSeqKey, unsigned long botRightSeqKey, unsigned long botLeftSeqKey)
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());
    std::vector<ManualSequenceConfig> manualSeqConfigTopRightSeq;
    std::vector<ManualSequenceConfig> manualSeqConfigTopLeftSeq;
    std::vector<ManualSequenceConfig> manualSeqConfigBotRightSeq;
    std::vector<ManualSequenceConfig> manualSeqConfigBotLeftSeq;

    if (topRightSeqKey != 0)
    {
        MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(topRightSeqKey);
        TA_ASSERT(seq != NULL, "");
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig_var seqConfig = seq->getSequenceConfig();
        for (size_t i = 0; i < seqConfig->inputKeySequence.length(); ++i)
        {
            unsigned long camEntityKey = seqConfig->inputKeySequence[i];
            MODBUSCamera* cam = MODBUSSwitchManager::getInstance().getCamera(camEntityKey);
            TA_ASSERT(cam != NULL, "");
		    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
			unsigned char cameraID = (cameraAddress & 0xffu);
			unsigned char stationID = (cameraAddress >> 8);
            manualSeqConfigTopRightSeq.push_back(ManualSequenceConfig(cameraID, stationID, seqConfig->dwellTime));
        }
    }
    if (topLeftSeqKey != 0)
    {
        MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(topLeftSeqKey);
        TA_ASSERT(seq != NULL, "");
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig_var seqConfig = seq->getSequenceConfig();
        for (size_t i = 0; i < seqConfig->inputKeySequence.length(); ++i)
        {
            unsigned long camEntityKey = seqConfig->inputKeySequence[i];
            MODBUSCamera* cam = MODBUSSwitchManager::getInstance().getCamera(camEntityKey);
            TA_ASSERT(cam != NULL, "");
		    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
			unsigned char cameraID = (cameraAddress & 0xffu);
			unsigned char stationID = (cameraAddress >> 8);
            manualSeqConfigTopLeftSeq.push_back(ManualSequenceConfig(cameraID, stationID, seqConfig->dwellTime));
        }
    }
    if (botRightSeqKey != 0)
    {
        MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(botRightSeqKey);
        TA_ASSERT(seq != NULL, "");
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig_var seqConfig = seq->getSequenceConfig();
        for (size_t i = 0; i < seqConfig->inputKeySequence.length(); ++i)
        {
            unsigned long camEntityKey = seqConfig->inputKeySequence[i];
            MODBUSCamera* cam = MODBUSSwitchManager::getInstance().getCamera(camEntityKey);
            TA_ASSERT(cam != NULL, "");
		    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
			unsigned char cameraID = (cameraAddress & 0xffu);
			unsigned char stationID = (cameraAddress >> 8);
            manualSeqConfigBotRightSeq.push_back(ManualSequenceConfig(cameraID, stationID, seqConfig->dwellTime));
        }
    }
    if (botLeftSeqKey != 0)
    {
        MODBUSSequence *seq = MODBUSSwitchManager::getInstance().getSequence(botLeftSeqKey);
        TA_ASSERT(seq != NULL, "");
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig_var seqConfig = seq->getSequenceConfig();
        for (size_t i = 0; i < seqConfig->inputKeySequence.length(); ++i)
        {
            unsigned long camEntityKey = seqConfig->inputKeySequence[i];
            MODBUSCamera* cam = MODBUSSwitchManager::getInstance().getCamera(camEntityKey);
            TA_ASSERT(cam != NULL, "");
		    unsigned short cameraAddress = boost::lexical_cast<unsigned short>(cam->getAddress());
			unsigned char cameraID = (cameraAddress & 0xffu);
			unsigned char stationID = (cameraAddress >> 8);
            manualSeqConfigBotLeftSeq.push_back(ManualSequenceConfig(cameraID, stationID, seqConfig->dwellTime));
        }
    }

    MODBUSSwitchManager::getInstance().getCommunicationsHandler()->startQuadManualSequence(monitorID, manualSeqConfigTopRightSeq, manualSeqConfigTopLeftSeq,
            manualSeqConfigBotRightSeq, manualSeqConfigBotLeftSeq);
}

void MODBUSVideoOutput::stopMonitor()
{
    unsigned short monitorID = boost::lexical_cast<unsigned short>(m_videoOutputData->getAddress());
	if(m_currentVideoInputKey != 0)
	{
		const MODBUSSwitchManager::EMODBUSInputType inputType = MODBUSSwitchManager::getInstance().getInputType(m_currentVideoInputKey);

		if(inputType == MODBUSSwitchManager::CAMERA)
		{
	        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->assignCameraToMonitor(monitorID, 0, 0);
		}
		else if(inputType == MODBUSSwitchManager::SEQUENCE)
		{
			MODBUSSequence* sequence = MODBUSSwitchManager::getInstance().getSequence(m_currentVideoInputKey);
			TA_ASSERT(sequence != NULL, "");
			if(sequence->isReadOnly())
			{
	            MODBUSSwitchManager::getInstance().getCommunicationsHandler()->stopAutoSwitchMode(monitorID);
			}
			else
			{
                MODBUSSwitchManager::getInstance().getCommunicationsHandler()->stopManualSequence(monitorID);
			}
		}
	}
	else
	{
		const int displayOrder = (getDisplayOrder() / 5) * 5 + 1;
		unsigned long consoleKey = this->getConsoleKey();
		for(int i = displayOrder; i < displayOrder + 4; ++i)
		{
			MODBUSVideoOutput* monitor = MODBUSSwitchManager::getInstance().getVideoOutPutByDisplayOrderAndConsole(displayOrder, consoleKey);
			if(monitor == NULL)
			{
				continue;
			}
			const unsigned long inputKey = monitor->getCurrentVideoInputKey();
			if(inputKey != 0)
			{
				const MODBUSSwitchManager::EMODBUSInputType inputType = MODBUSSwitchManager::getInstance().getInputType(inputKey);

				if(inputType == MODBUSSwitchManager::CAMERA)
				{
                    MODBUSSwitchManager::getInstance().getCommunicationsHandler()->assignCameraToQuad(monitorID, 0, 0, 0, 0, 0, 0, 0, 0);
					break;
				}
				else if(inputType == MODBUSSwitchManager::SEQUENCE)
				{
                    MODBUSSwitchManager::getInstance().getCommunicationsHandler()->stopQuadManualSequence(monitorID);
					break;
				}
			}
		}
	}
}
//
// requestSwitchOfInput
//
void MODBUSVideoOutput::requestSwitchOfInput(unsigned long videoInputEntityKey, bool isDemand)
{
    // the input should not be quad
    if (!m_isInAlarmMode)
    {
        try
        {
            if (videoInputEntityKey == 0) // if zero, clear the monitor
            {
                stopMonitor();
            }
            else
            {
                switch (MODBUSSwitchManager::getInstance().getInputType(videoInputEntityKey))
                {
                    case MODBUSSwitchManager::CAMERA:
                        requestSingleCamToOutput(videoInputEntityKey);
                        break;
                        /*
                        case MODBUSSwitchManager::QUAD:
                        requestQuadToOutput(videoInputEntityKey);
                        break;
                        */
                    case MODBUSSwitchManager::SEQUENCE:
                        requestSequenceToOutput(videoInputEntityKey);
                        break;
                    default:
                        TA_ASSERT(false, "Unknow input type");
                }
            }

            // broadcast the input update message
            notifyInputSwitchedToOutput(videoInputEntityKey);

			/*
            TA_IRS_Core::SequenceHelper sequenceHelper;
            if (videoInputEntityKey != 0)
            {
                sequenceHelper.setRelationShipOfParentKey(this->getKey(), videoInputEntityKey, "", RL_MONITOR_CAMERA_NORMAL);
            }
            else
            {
                sequenceHelper.delRelationShipOfParentKey(this->getKey());
            }
			*/
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Bus::ModbusException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ModbusException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "ModbusException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
    }
    else
    {
        if (videoInputEntityKey == 0)
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090089");
        }
        else
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090090");
        }
    }
}

void MODBUSVideoOutput::requestSwitchOfInputs(unsigned long topRightVideoInputEntityKey, unsigned long topLeftVideoInputEntityKey, unsigned long botRightVideoInputEntityKey, unsigned long botLeftVideoInputEntityKey, unsigned long consoleKey, bool isDemand)
{
    // the input should not be quad
    if (!m_isInAlarmMode)
    {
		int displayOrder = (getDisplayOrder() / 5) * 5;
		if(displayOrder != 0)
		{
			consoleKey = 0;
		}
        MODBUSVideoOutput* topLeft = MODBUSSwitchManager::getInstance().getVideoOutPutByDisplayOrderAndConsole(displayOrder + 1, consoleKey);
        MODBUSVideoOutput* topRight = MODBUSSwitchManager::getInstance().getVideoOutPutByDisplayOrderAndConsole(displayOrder + 2, consoleKey);
        MODBUSVideoOutput* botLeft = MODBUSSwitchManager::getInstance().getVideoOutPutByDisplayOrderAndConsole(displayOrder + 3, consoleKey);
        MODBUSVideoOutput* botRight = MODBUSSwitchManager::getInstance().getVideoOutPutByDisplayOrderAndConsole(displayOrder + 4, consoleKey);

		/*
        if ((topLeft || topRight || botRight || botLeft)
                && (!topLeft || topLeft->getCurrentVideoInputKey() == topLeftVideoInputEntityKey)
                && (!topRight || topRight->getCurrentVideoInputKey() == topRightVideoInputEntityKey)
                && (!botLeft || botLeft->getCurrentVideoInputKey() == botLeftVideoInputEntityKey)
                && (!botRight || botRight->getCurrentVideoInputKey() == botRightVideoInputEntityKey))
        {
            return;
        }
		*/

        try
        {
            if (!topRightVideoInputEntityKey && !topLeftVideoInputEntityKey && !botRightVideoInputEntityKey && !botLeftVideoInputEntityKey)
            {
                stopMonitor();
            }
            else
            {
                std::vector<MODBUSSwitchManager::EMODBUSInputType> types;
                std::vector<unsigned long> inputs;
                if (topRightVideoInputEntityKey != 0)
                {
                    types.push_back(MODBUSSwitchManager::getInstance().getInputType(topRightVideoInputEntityKey));
                    inputs.push_back(topRightVideoInputEntityKey);
                }
                if (topLeftVideoInputEntityKey != 0)
                {
                    types.push_back(MODBUSSwitchManager::getInstance().getInputType(topLeftVideoInputEntityKey));
                    inputs.push_back(topLeftVideoInputEntityKey);
                }
                if (botRightVideoInputEntityKey != 0)
                {
                    types.push_back(MODBUSSwitchManager::getInstance().getInputType(botRightVideoInputEntityKey));
                    inputs.push_back(botRightVideoInputEntityKey);
                }
                if (botLeftVideoInputEntityKey != 0)
                {
                    types.push_back(MODBUSSwitchManager::getInstance().getInputType(botLeftVideoInputEntityKey));
                    inputs.push_back(botLeftVideoInputEntityKey);
                }

                MODBUSSwitchManager::EMODBUSInputType type = *(types.begin());

                for (std::vector<MODBUSSwitchManager::EMODBUSInputType>::iterator it = types.begin() + 1; it != types.end(); ++it)
                {
                    if (type != (*it))
                    {
                        throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090095");
                    }
                }

                if (type == MODBUSSwitchManager::SEQUENCE)
                {
                    for (std::vector<unsigned long>::iterator it = inputs.begin(); it != inputs.end(); ++it)
                    {
                        MODBUSSequence* seq = MODBUSSwitchManager::getInstance().getSequence(*(inputs.begin()));
                        if (seq == NULL || seq->isReadOnly())
                        {
                            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090095");
                        }
                    }
                }

                switch (type)
                {
                    case MODBUSSwitchManager::CAMERA:
                        requestCamsToQuad(topRightVideoInputEntityKey, topLeftVideoInputEntityKey, botRightVideoInputEntityKey, botLeftVideoInputEntityKey);
                        break;
                        /*
                        case MODBUSSwitchManager::QUAD:
                        requestQuadToOutput(videoInputEntityKey);
                        break;
                        */
                    case MODBUSSwitchManager::SEQUENCE:
                        requestSeqsToQuad(topRightVideoInputEntityKey, topLeftVideoInputEntityKey, botRightVideoInputEntityKey, botLeftVideoInputEntityKey);
                        break;
                    default:
                        TA_ASSERT(false, "Unknow input type");
                }
            }

            // broadcast the input update message
            notifyInputSwitchedToOutput(0);
            //TA_IRS_Core::SequenceHelper sequenceHelper;
            //sequenceHelper.delRelationShipOfParentKey(getKey());

			MODBUSVideoOutput* outPuts[4] = {topLeft, topRight, botLeft, botRight};
			unsigned long inputkeys[4] = {topLeftVideoInputEntityKey, topRightVideoInputEntityKey, botLeftVideoInputEntityKey, botRightVideoInputEntityKey};

			for(int i = 0; i < 4; ++i)
			{
	            MODBUSVideoOutput* p = outPuts[i];
				
	            if (p != NULL && p->getCurrentVideoInputKey() != inputkeys[i])
	            {
					p->notifyInputSwitchedToOutput(inputkeys[i]);
					/*
					if(inputkeys[i] == 0)
					{
						sequenceHelper.delRelationShipOfParentKey(p->getKey());
					}
					else
					{
						sequenceHelper.setRelationShipOfParentKey(p->getKey(), inputkeys[i], "");
					}
					*/
	            }
			}


        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Bus::ModbusException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ModbusException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "ModbusException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
        catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException(e.what());
        }
    }
    else
    {
        if (!topRightVideoInputEntityKey && !topLeftVideoInputEntityKey && !botRightVideoInputEntityKey && !botLeftVideoInputEntityKey)
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090089");
        }
        else
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("IDS_UE_090090");
        }
    }
}

//
// connectSequence
//
void MODBUSVideoOutput::connectSequence(const TA_Base_Bus_GZL9::VideoOutputCorbaDef::UserDefinedSequenceConfig& newConfig)
{
}

//
// connectSequence
//
void MODBUSVideoOutput::disconnectSequence()
{
}

//
// cycleToNextVideoInput
//
void MODBUSVideoOutput::cycleToNextVideoInput()
{
}


//
// cycleToPreviousVideoInput
//
void MODBUSVideoOutput::cycleToPreviousVideoInput()
{
}


//
// pause
//
void MODBUSVideoOutput::pause()
{
}


//
// play
//
void MODBUSVideoOutput::play()
{
}


//
// stop
//
void MODBUSVideoOutput::stop()
{
}

///////////////////////////////////////////////////////////////////////
//
// IStateValidationItem Methods
//
///////////////////////////////////////////////////////////////////////


//
// validateState
//
void MODBUSVideoOutput::validateState()
{
}

TA_MODBUS::AlarmStack MODBUSVideoOutput::alarmStateSequenceToAlarmStack(TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack alarmStateSequence)
{
    unsigned long length = alarmStateSequence.length();
    TA_MODBUS::AlarmStack alarmStack;
    for (unsigned long i = 0; i < length; i++)
    {
        TA_MODBUS::AlarmState state;

        state.eventId = alarmStateSequence[i].eventId;
        if (state.eventId == 0)
        {
            state.stationId = 0;
        }
        else
        {
            state.stationId = alarmStateSequence[i].stationId;
        }
        alarmStack.push_back(state);
    }
    return alarmStack;
}

TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack MODBUSVideoOutput::alarmStateSequenceFromAlarmStack(TA_MODBUS::AlarmStack alarmStack)
{
    TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack stateSequence;
    stateSequence.length(alarmStack.size());
    TA_MODBUS::AlarmStack::iterator itr;
    unsigned long i = 0;
    for (itr = alarmStack.begin(); itr != alarmStack.end() && i < stateSequence.length(); itr++, i++)
    {
        stateSequence[i].eventId = (*itr).eventId;
        stateSequence[i].stationId = (*itr).stationId;
    }
    return stateSequence;
}

bool MODBUSVideoOutput::isInAlarmMode()
{
    return m_isInAlarmMode;
}

void MODBUSVideoOutput::activeVideoInput(::CORBA::ULong videoInputEntityKey, ::CORBA::Boolean active,  ::CORBA::Boolean isDemand)
{
    /*
    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"CMODBUSVideoOutput::activeVideoInput be called.--(%d)(%d)",videoInputEntityKey,active);
    try
    {
    	if(MODBUSSwitchManager::SEQUENCE != MODBUSSwitchManager::getInstance().getInputType( videoInputEntityKey ))
    	{
    		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "activeVideoInput only valid to sequence");
    		return;
    	}
    	if(active)
    	{
    		requestSequenceToOutput(videoInputEntityKey);
    	}
    	else
    	{
    		deactiveSequence(videoInputEntityKey);
    	}

    	// broadcast the input update message
    	notifyInputSwitchedToOutput(videoInputEntityKey);

    	TA_IRS_Core::SequenceHelper sequenceHelper;
    	if(active != 0)
    	{
    		sequenceHelper.setRelationShipOfParentKey( this->getKey(),videoInputEntityKey,RL_MONITOR_CAMERA_NORMAL);
    	}
    	else
    	{
    		sequenceHelper.delRelationShipOfParentKey(this->getKey());
    	}

    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
    	LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
    	LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
    	throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
    }
    catch (const TA_Base_Core::DataException& e)
    {
    	LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
    	LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
    	throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
    }
    catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
    {
    	LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
    	LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
    	throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
    }
    */
}
} // TA_IRS_App

