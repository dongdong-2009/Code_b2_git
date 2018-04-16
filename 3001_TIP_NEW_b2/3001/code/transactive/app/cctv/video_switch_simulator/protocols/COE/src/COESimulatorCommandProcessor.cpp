/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COESimulatorCommandProcessor.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * COESimulatorCommandProcessor implements ISimulationCommandProcessor, for use with the generic simulator.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <iostream>
#include <sstream>

#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulatorCommandProcessor.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h"

using namespace TA_IRS_App;

std::string COESimulatorCommandProcessor::commands[NUM_COMMANDS] =
{
    "status",
    "setSwitchStatus",
    "setSummaryAlarms",
    "setMonitorState",
    "setQuadState",
    "setSequenceState",
	"triggeringEvent",
	"Notifications"
};

std::string COESimulatorCommandProcessor::commandUsage[NUM_COMMANDS] =
{
    "status",
    "setSwitchStatus <active | standby>",
    "setSummaryAlarms <system (0|1)> <transmission (0|1)> <camera (0|1)> <ancillary (0|1)> <dvr (0|1)>",
    "setMonitorState <mon address> <cam | quad | seq | bvs> <item location address/BVS Stage> <item address> ...",
    "setQuadState <quad address> <cam 1 location address> <cam 1 address> ...",
    "setSequenceState <sequence address> <dwell time> <cam 1 location address> <cam 1 address> ...",
	"triggeringEvent <open|close|list> <stationId> <eventId> <monitorId>",
	""
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COESimulatorCommandProcessor::COESimulatorCommandProcessor(COESimulator& parentSimulator)
:       m_parentSimulator(parentSimulator),
        m_errMsg(""),
		m_shouldSendUnsolicitedNotification(true)
{

}

COESimulatorCommandProcessor::~COESimulatorCommandProcessor()
{

}

    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                  ISimulationCommandProcessor methods                     //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

//
//hasCommandName
//
bool COESimulatorCommandProcessor::hasCommandName( const std::string &name )
{
    if (name == "help")
    {
        return true;
    }

    return hasCommand(name);
}


//
//usageString
//
std::string COESimulatorCommandProcessor::usageString(const std::string &name)
{
    return cmdUsage(name);
}


//
//errorMsg
//
std::string COESimulatorCommandProcessor::errorMsg()
{
    return m_errMsg;
}


//
//execute
//
bool COESimulatorCommandProcessor::execute(const std::string &name, const std::string &params)
{
    m_errMsg = "";

    if(name == "help" )
    {
        for (unsigned int i = 0; i < NUM_COMMANDS; i++)
        {
            std::cout << commandUsage[i] << std::endl;
        }
        return true;
    }
    
    else if(name == "status")
    {
        m_parentSimulator.execStatus();
        return true;
    }

    else if(name == "setSwitchStatus")
    {
        char    status[100];

        // Get the host and port 
        int numTokens = sscanf(params.c_str(), "%s", status);
        if ( (numTokens==1) && ( ( status[0] == 'a' ) || ( status[0] == 's') ) )
        {
            bool isActive = ( status[0] == 'a' );
            m_parentSimulator.updateObserversWithSwitchStatus( isActive );
            return true;
        } 
        else
        {
            return false;
        }
    }

    else if(name == "setSummaryAlarms")
    {
        int sys(0);
        int tra(0);
        int cam(0);
        int anc(0);
		int dvr(0);
        // Get the host and port 
        int numTokens = sscanf(params.c_str(), "%d %d %d %d %d", &sys, &tra, &cam, &anc, &dvr );
        if (numTokens==5)
        {
            COEEquipmentState::getInstance().setSummaryAlarm( COEEquipmentState::SYS_SUM, (sys != 0) );
            COEEquipmentState::getInstance().setSummaryAlarm( COEEquipmentState::TRANS_SUM, (tra != 0) );
            COEEquipmentState::getInstance().setSummaryAlarm( COEEquipmentState::CAM_SUM, (cam != 0) );
            COEEquipmentState::getInstance().setSummaryAlarm( COEEquipmentState::ANC_SUM, (anc != 0) );
			COEEquipmentState::getInstance().setSummaryAlarm( COEEquipmentState::DVR_SUM, (dvr != 0) );
            return true;
        } 
        else
        {
            return false;
        }
    }

    else if(name == "setMonitorState")
    {
        int monAddr(0);
        char itemType[50];
        int itemLocationAddr(0);
        int itemAddr(0);
        std::string localParams( params );
        int numTokens = sscanf(localParams.c_str(), "%d %s %d %d", &monAddr, itemType, &itemLocationAddr, &itemAddr );
        if (numTokens==4)
        {
            std::string type( reinterpret_cast< const char* >(itemType) );
            if ( type == "cam" )
            {
                COEEquipmentState::MonitorState state;
                state.monitorId = (monAddr % 0x0100);
				state.assignmentType = COEEquipmentState::CAMERA;
                state.assignedDeviceLocationKey = (itemLocationAddr % 0x0100);
                state.assignedDeviceID = (itemAddr % 0x0100);

                COEEquipmentState::getInstance().setMonitorState( m_parentSimulator.getLocation(), (monAddr % 0x0100), state, m_shouldSendUnsolicitedNotification);
            }
            else if ( type == "quad" )
            {
                COEEquipmentState::MonitorState state;
                state.monitorId = (monAddr % 0x0100);
				state.assignmentType = COEEquipmentState::QUAD;
                state.assignedDeviceLocationKey = 0x00; // not used.
                state.assignedDeviceID = (itemAddr % 0x0100);

                COEEquipmentState::getInstance().setMonitorState( m_parentSimulator.getLocation(), (monAddr % 0x0100), state, m_shouldSendUnsolicitedNotification);
            }
            else if ( type == "seq" )
            {
                COEEquipmentState::MonitorState state;
                state.monitorId = (monAddr % 0x0100);
				state.assignmentType = COEEquipmentState::SEQUENCE;
                state.assignedDeviceLocationKey = 0x00; // not used.
                state.assignedDeviceID = (itemAddr % 0x0100);

                COEEquipmentState::getInstance().setMonitorState( m_parentSimulator.getLocation(), (monAddr % 0x0100), state, m_shouldSendUnsolicitedNotification);
            }
            else if ( type == "bvs" )
            {
                COEEquipmentState::MonitorState state;
                state.monitorId = (monAddr % 0x0100);
				state.assignmentType = COEEquipmentState::TRAIN;
                state.assignedDeviceLocationKey = (itemLocationAddr % 0x0100);
                state.assignedDeviceID = 0x00;//not used

                COEEquipmentState::getInstance().setMonitorState( m_parentSimulator.getLocation(), (monAddr % 0x0100), state, m_shouldSendUnsolicitedNotification);
            }
            else
            {
                return false;
            }
        } 
        else
        {
            return false;
        }
    }

    else if(name == "setQuadState")
    {
        int quadAddr(0);
        int cam1LocationAddr(0);
        int cam1Addr(0);
        int cam2LocationAddr(0);
        int cam2Addr(0);
        int cam3LocationAddr(0);
        int cam3Addr(0);
        int cam4LocationAddr(0);
        int cam4Addr(0);
        int numTokens = sscanf(params.c_str(), "%d %d %d %d %d %d %d %d %d", &quadAddr, &cam1LocationAddr, &cam1Addr,
            &cam2LocationAddr, &cam2Addr, &cam3LocationAddr, &cam3Addr, &cam4LocationAddr, &cam4Addr );
        if (numTokens==9)
        {
            COEEquipmentState::QuadState state( 8, 0x00 );
            state[ 0 ] = (cam1LocationAddr%0x0100);
            state[ 1 ] = (cam1Addr%0x0100);
            state[ 2 ] = (cam2LocationAddr%0x0100);
            state[ 3 ] = (cam2Addr%0x0100);
            state[ 4 ] = (cam3LocationAddr%0x0100);
            state[ 5 ] = (cam3Addr%0x0100);
            state[ 6 ] = (cam4LocationAddr%0x0100);
            state[ 7 ] = (cam4Addr%0x0100);
            COEEquipmentState::getInstance().setQuadState( m_parentSimulator.getLocation(), (quadAddr % 0x0100), state );
        } 
        else
        {
            return false;
        }
    }

    else if(name == "setSequenceState")
    {
        std::string localParams( params );
        int value(0);
        char* token = strtok( const_cast<char*>(localParams.c_str()), " " );
        if (0 != token)
        {
            if ( 0 < sscanf(token, "%d", &value) )
            {
                COEEquipmentState::SequenceState state;
                state.dwellTime = 0;
                unsigned char seqAddr = (value%0x0100);
                token = strtok( 0, " " );
                if (0 != token)
                {
                    if ( 0 < sscanf(token, "%d", &value) )
                    {
                        state.dwellTime = (value%0x0100);
                        unsigned char camLocAddr, camAddr;
                        token = strtok( 0, " " );
                        while (0 != token)
                        {
                            if ( 0 < sscanf(token, "%d", &value) )
                            {
                                camLocAddr = (value%0x0100);
                                token = strtok( 0, " " );
                                if ( (0 != token) && (0 < sscanf(token, "%d", &value)) )
                                {
                                    camAddr = (value%0x0100);
                                    state.orderedCameras.push_back(camLocAddr);
                                    state.orderedCameras.push_back(camAddr);
                                }
                            }
                            token = strtok( 0, " " );
                        }
                    }
                }
                COEEquipmentState::getInstance().setSequenceState( m_parentSimulator.getLocation(), (seqAddr % 0x0100), state );
            }
            else
            {
                return false;
            }

        } 
        else
        {
            return false;
        }
    }
	else if(name=="triggeringEvent")
	{
		int stationId(0);
        int eventId(0);
		int monitorId(0);
		char state[10] = "";
        int numTokens = sscanf(params.c_str(), "%s %d %d %d",&state, &stationId, &eventId, &monitorId);
		
		std::string stateString(state);
		if( stateString == "list" )
		{
			COEEquipmentState::getInstance().listOpenTriggeringEvents(m_parentSimulator.getLocation());
		}
		else
		{
			if(stateString == "open")
			{
				if( numTokens != 4 )
				{
					return false;
				}
				COEEquipmentState::getInstance().openTriggeringEvent(m_parentSimulator.getLocation(), stationId, eventId, monitorId);
			}
			else if(stateString == "close")
			{
				if( numTokens != 3 )
				{
					return false;
				}
				COEEquipmentState::getInstance().closeTriggeringEvent(m_parentSimulator.getLocation(), stationId, eventId);
			}
			else
			{
				return false;
			}
			std::stringstream output;
			output << stateString << ": StationId = " << stationId << ", EventId = " << eventId;
			output << ", MonitorId = " <<monitorId;
			std::cout << output.str() << std::endl;
		}
	}
	else if(name == "Notification")
	{
		m_shouldSendUnsolicitedNotification = !m_shouldSendUnsolicitedNotification;
		std::stringstream output;
		output << "Notifications are now turned ";
		if(m_shouldSendUnsolicitedNotification)
		{
			output << "ON (Normal mode)";
		}
		else
		{
			output << "OFF (testing mode)";
		}
		
		std::cout << output.str() << std::endl;

	}
	return true;
	
}


    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //              CctvSimulatorCommandProcessor specific methods              //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////


//
//hasCommand
//
bool COESimulatorCommandProcessor::hasCommand( const std::string &name )
{
    for (unsigned int i = 0; i < NUM_COMMANDS; i++)
    {
        if (commands[i] == name)
        {
            return true;
        }
    }
    return false;
}


//
//cmdUsage
//
std::string COESimulatorCommandProcessor::cmdUsage(const std::string& name)
{
    for (unsigned int i = 0; i < NUM_COMMANDS; i++)
    {
        if (commands[i] == name)
        {
            return commandUsage[i];
        }
    }
    return "";
}
