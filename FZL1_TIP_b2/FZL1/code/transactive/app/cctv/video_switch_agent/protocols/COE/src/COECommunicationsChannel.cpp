/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/CommandCreateHelper.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Timer.h"
#include <sstream>
#include <iomanip>

namespace TA_IRS_App
{
    COECommunicationsChannel::ConnectionLostAlarmRaiser::ConnectionLostAlarmRaiser(const std::string& entityname,
                                                                                   unsigned long entityKey,
                                                                                   unsigned long locationKey,
                                                                                   unsigned long entityTypeKey,
                                                                                   unsigned long subsystemKey,
                                                                                   std::string& assetName)
        : m_entityName(entityname),
          m_entityKey(entityKey),
          m_locationKey(locationKey),
          m_entityTypeKey(entityTypeKey),
          m_subsystemKey(subsystemKey),
          m_assetName(assetName)
    {
    }

    void COECommunicationsChannel::ConnectionLostAlarmRaiser::raiseAlarm(const std::string& videoSwitchAddress)
    {
        TA_Base_Core::DescriptionParameters dp;
        TA_Base_Core::NameValuePair pair1("VideoSwitchAddress", videoSwitchAddress);
        dp.push_back(&pair1);
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::VisualAlarms::VisualConnectionLost,
                m_entityKey,
                m_entityTypeKey,
                dp,
                m_entityName,
                m_locationKey,
                m_subsystemKey,
                m_assetName,
                TA_Base_Core::AlarmConstants::defaultTime);
    }

    void COECommunicationsChannel::ConnectionLostAlarmRaiser::closeAlarm()
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::VisualAlarms::VisualConnectionLost,
                m_entityKey,
                m_locationKey);
    }
    //
    // COECommunicationsChannel
    //
    COECommunicationsChannel::COECommunicationsChannel(unsigned int commsChannelIndex,
                                                       const std::string& address,
                                                       const std::string& port,
                                                       unsigned long connectTimeout,
                                                       unsigned long maxResponseTime,
                                                       unsigned long pollInterval,
                                                       ICOEChannelStateChangeObserver& commsHandler,
                                                       ReceivedDataProcessor& receivedDataProcessor,
                                                       std::string entityName,
                                                       unsigned long entityKey,
                                                       unsigned long locationKey,
                                                       unsigned long entityType,
                                                       unsigned long subSystem,
                                                       std::string assetName)
        : m_running(false),
          m_commsChannelIndex(commsChannelIndex),
          m_address(address),
          m_port(port),
          m_connectTimeout(connectTimeout),
          m_socket(new TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >(m_address, m_port, UINT_MAX, m_connectTimeout)),
          m_maxResponseTime(maxResponseTime),
          m_pollInterval(pollInterval),
          m_commsHandler(commsHandler),
          m_receivedDataProcessor(receivedDataProcessor),
          m_commandInProgress(0),
          m_currentReceivedPacketLength(0),
          m_commandInProgressCondition(),
          m_alarmRaiser(entityName, entityKey, locationKey, entityType, subSystem, assetName)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    //
    // ~COECommunicationsChannel
    //
    COECommunicationsChannel::~COECommunicationsChannel()
    {
        FUNCTION_ENTRY("Destructor");
        FUNCTION_EXIT;
    }


    //
    // sendCommand
    //
    void COECommunicationsChannel::sendCommand(IAgentRequestCommand& command)
    {
        FUNCTION_ENTRY("sendCommand");
        TA_Base_Core::ThreadGuard guard(m_socketLock);
        if(!m_socket->stillConnected())
        {
            command.signalCouldNotBeSent();
        }
        else
        {
            TA_Base_Core::ThreadGuard guard(m_commandInProgressCondition.getLinkedThreadLockable());
            TA_ASSERT(m_commandInProgress == 0, "No command should be in progressing");
            m_commandInProgress = &command;
            std::string dataToSend(command.getDataToSend());
            m_socket->write(dataToSend.c_str(), dataToSend.size());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Send out command:[%s]", TA_COE::getPacketLogFormatStr(dataToSend).c_str());
            if(0 == m_commandInProgressCondition.timedWait(m_maxResponseTime))
            {
                // One last check to ensure received data thread didn't acquire lock inbetween
                // the condition time-out and this thread re-acquiring lock.
                if(0 != m_commandInProgress)
                {
                    m_commandInProgress->signalTimedOut();
                    m_commandInProgress = 0;
                }
            }
        }
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::activateChannel()
    {
        FUNCTION_ENTRY("activateChannel");
        m_socket->start();
        start();
        m_socket->addObserver(*this);
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::deactivateChannel()
    {
        FUNCTION_ENTRY("deactivateChannel");
        m_socket->removeObserver(*this);
        terminateAndWait();
        m_socket->terminateAndWait();
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::run()
    {
        FUNCTION_ENTRY("run");
        m_running = true;
        while(m_running)
        {
            try
            {
                sleep(m_pollInterval);
                sendSummaryPollingCommand();
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
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::terminate()
    {
        m_running = false;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // ITcpSocketObserver Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // processReceivedData
    //
    void COECommunicationsChannel::processReceivedData(std::vector< unsigned char >& pData, TA_Base_Core::ISocket* pSocket)
    {
        FUNCTION_ENTRY("processReceivedData");
        while(!pData.empty())
        {
            m_currentReceivedPacket.push_back(*(pData.begin()));
            pData.erase(pData.begin());
            if(0 == m_currentReceivedPacketLength)
            {
                if(m_currentReceivedPacket.size() >= TA_COE::MIN_VALID_RECEIVED_LENGTH)
                {
					m_currentReceivedPacketLength = TA_COE::NUM_LENGTH_BYTES + TA_COE::getPacketLength(m_currentReceivedPacket[ TA_COE::LSB_LENGTH_BYTE ],
                                                                            m_currentReceivedPacket[ TA_COE::MSB_LENGTH_BYTE ]);
                    bool isValidPacketHeader = m_receivedDataProcessor.validatePacketHeader(m_currentReceivedPacketLength,
                                                                                            m_currentReceivedPacket[ TA_COE::COMMAND_BYTE ],
                                                                                            m_currentReceivedPacket[ TA_COE::ERROR_BYTE ]);
                    if(!isValidPacketHeader)
                    {
                        invalidPacketHeaderReceived();
                    }
                }
            }
            if(0 != m_currentReceivedPacketLength)
            {
                if(m_currentReceivedPacket.size() == m_currentReceivedPacketLength)
                {
                    validPacketReceived();
                }
            }
        }
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::writeFailed()
    {
        FUNCTION_ENTRY("writeFailed");
        TA_Base_Core::ThreadGuard commandInProgressGuard(m_commandInProgressCondition.getLinkedThreadLockable());
        if(0 != m_commandInProgress)
        {
            m_commandInProgress->signalCouldNotBeSent();
            m_commandInProgress = 0;
            m_commandInProgressCondition.broadcast();
        }
        FUNCTION_EXIT;
    }


    bool COECommunicationsChannel::connectionLost()
    {
        FUNCTION_ENTRY("connectionLost");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Connection to the switch has been lost.");
        m_alarmRaiser.raiseAlarm(m_address);
        FUNCTION_EXIT;
        return true;
    }


    void COECommunicationsChannel::connectionEstablished()
    {
        FUNCTION_ENTRY("connectionEstablished");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection to the switch has been established");
        m_alarmRaiser.closeAlarm();
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::sendSummaryPollingCommand()
    {
        FUNCTION_ENTRY("sendSummaryPollingCommand");
        std::auto_ptr< IAgentRequestCommand > pollingCommand(TA_COE::createSummaryPollingCommand());
        TA_ASSERT(0 != pollingCommand.get(), "");
        sendCommand(*pollingCommand.get());
        pollingCommand->waitForCompletion();
		ECommunicationChannelState state =
			(IAgentRequestCommand::SUCCEEDED != pollingCommand->getCompletionState() ? COMMUNICATIONS_FAILURE : ACTIVE_SWITCH_CHANNEL);
        m_commsHandler.processCommunicationsChannelState(m_commsChannelIndex, state);
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::invalidPacketHeaderReceived()
    {
        FUNCTION_ENTRY("invalidPacketHeaderPacketReceived");
        std::stringstream badResponseHeaderMessage;
        badResponseHeaderMessage << "Invalid packet header received:[%s]";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, badResponseHeaderMessage.str().c_str(), TA_COE::getPacketLogFormatStr(m_currentReceivedPacket).c_str());
        m_currentReceivedPacket.erase(m_currentReceivedPacket.begin());
        m_currentReceivedPacketLength = 0;
        FUNCTION_EXIT;
    }


    void COECommunicationsChannel::validPacketReceived()
    {
        FUNCTION_ENTRY("validPacketReceived");
        TA_Base_Core::ThreadGuard commandInProgressGuard(m_commandInProgressCondition.getLinkedThreadLockable());
        bool isCommandInProgressResponse = false;
        if(0 != m_commandInProgress)
        {
            try
            {
                isCommandInProgressResponse = m_commandInProgress->processResponse(m_currentReceivedPacket);
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unexpected exception catch when processing response:[%s]", TA_COE::getPacketLogFormatStr(m_currentReceivedPacket).c_str());
                isCommandInProgressResponse = true;
            }
            if(isCommandInProgressResponse)
            {
                m_commandInProgress->signalResponse();
                m_commandInProgress = 0;
                m_commandInProgressCondition.broadcast();
            }
        }
        if(!isCommandInProgressResponse)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Valid but unexpected packet received [%s]", TA_COE::getPacketLogFormatStr(m_currentReceivedPacket).c_str());
        }
        m_currentReceivedPacket.clear();
        m_currentReceivedPacketLength = 0;
        FUNCTION_EXIT;
    }


}
