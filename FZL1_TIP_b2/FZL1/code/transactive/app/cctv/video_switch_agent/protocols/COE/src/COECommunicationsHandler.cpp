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
  * Handles the protocol dependent sections of the operations for a COECommunicationsHandler
  * using the COE protocol.
  * This interface is used by the COECommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 4503)
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/AddressHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace TA_IRS_App
{
    const unsigned short COECommunicationsHandler::NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE = 3;
    const std::string COECommunicationsHandler::VIDEO_SWITCH_TYPE = "VideoSwitch";

    //
    // COECommunicationsHandler
    //
    COECommunicationsHandler::COECommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData)
        : m_videoSwitchAgentData(videoSwitchAgentData),
          m_switchConnectTimeout(0),
          m_switchPollInterval(0),
          m_maxSwitchResponseTime(0),
          m_activeSwitchChannelIndex(0),
          m_exceededNoActiveSwitchCycleAllowance(false),
          m_doTerminate(false),
          m_processingToBeDoneSemaphore(0),
          m_receivedDataProcessor(std::auto_ptr< ReceivedDataProcessor >(new ReceivedDataProcessor()))
    {
        try
        {
            // Gather comms related parameters from Agent Entity Config
            m_switchConnectTimeout = m_videoSwitchAgentData->getSwitchConnectTimeout();
            m_maxSwitchResponseTime = m_videoSwitchAgentData->getMaxSwitchResponseTime();
            m_switchPollInterval = m_videoSwitchAgentData->getSwitchPollInterval();
            // Get the VideoSwitch data, so we can get the addresses for the switching hardware.
            unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
            TA_Base_Core::IEntityDataList dataList =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(VIDEO_SWITCH_TYPE, locationKey);
            BOOST_FOREACH(TA_Base_Core::IEntityData* i, dataList)
            {
                m_videoSwitchData.push_back(TA_Base_Core::IEntityDataPtr(i));
            }
            loadCommunicationsChannels();
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            cleanUp();
            throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            cleanUp();
            throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, e.what());
        }
    }


    //
    // ~COECommunicationsHandler
    //
    COECommunicationsHandler::~COECommunicationsHandler()
    {
        FUNCTION_ENTRY("Destructor");
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            // Clean up this thread if still running, and any queued commands.
            disconnect();
            // Clean up heap objects.
            cleanUp();
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
        FUNCTION_EXIT;
    }


    ///
    // submitRequestCommand
    //
    void COECommunicationsHandler::submitRequestCommand(RequestCommandSharedPtr command, bool isClientRequest)
    {
        FUNCTION_ENTRY("submitRequestCommand");
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        //
        // Make sure the communications handler is connected (i.e. thread active), else
        // do not queue the command, otherwise it may never be processed.
        //
        if(!isConnected())
        {
            commandNotToBeProcessed(command);
            FUNCTION_EXIT;
            return;
        }
        //
        // Since we are connected, queue the command as specified by isClientRequest,
        // and post the semaphore to indicate there is work to do.
        //
        if(isClientRequest)
        {
            m_clientRequestCommandQueue.push(command);
        }
        else
        {
            m_internalRequestCommandQueue.push(command);
        }
        m_processingToBeDoneSemaphore.post();
        FUNCTION_EXIT;
    }

    //
    // processCommunicationsChannelState
    //
    void COECommunicationsHandler::processCommunicationsChannelState(unsigned int channelIndex, COECommunicationsChannel::ECommunicationChannelState state)
    {
        FUNCTION_ENTRY("processCommunicationsChannelStateUpdate");
        TA_ASSERT(channelIndex < m_communicationsChannelsInfo.size(), "Invalid channelIndex passed to processCommunicationsChannelStateUpdate!");
        TA_Base_Core::ThreadGuard commsChannelsStateGuard(m_commsChannelsStateLock);
        switch(state)
        {
            case COECommunicationsChannel::COMMUNICATIONS_FAILURE:
                receivedChannelStateCommsFailure(channelIndex);
                receivedChannelStateNotActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = COECommunicationsChannel::COMMUNICATIONS_FAILURE;
                break;
            case COECommunicationsChannel::IDLE_SWITCH_CHANNEL:
                receivedChannelStateNotCommsFailure(channelIndex);
                receivedChannelStateNotActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = COECommunicationsChannel::IDLE_SWITCH_CHANNEL;
                break;
            case COECommunicationsChannel::ACTIVE_SWITCH_CHANNEL:
                receivedChannelStateNotCommsFailure(channelIndex);
                receivedChannelStateActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = COECommunicationsChannel::ACTIVE_SWITCH_CHANNEL;
                break;
            default:
                break;
        }
        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDCommunicationsHandler Methods to be overridden
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // connect
    //
    void COECommunicationsHandler::connect()
    {
        FUNCTION_ENTRY("connect");
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        if(TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "connect() called while already active. Thread will be restarted.");
            terminateAndWait();
        }
        activateCommunicationsChannels();
        start();
        FUNCTION_EXIT;
    }


    //
    // disconnect
    //
    void COECommunicationsHandler::disconnect()
    {
        FUNCTION_ENTRY("disconnect");
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        if(TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
        {
            terminateAndWait();
        }
        // Empty anything left in the queues as these may never be processed now,
        clearCommandQueues();
        // Ensure flag reset so thread will run correctly again when reactivated.
        m_doTerminate = false;
        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Thread Methods
    //
    ///////////////////////////////////////////////////////////////////////


    void COECommunicationsHandler::run()
    {
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this method.
        FUNCTION_ENTRY("run");
        while(!m_doTerminate)
        {
            try //if something roots up we dont want to give up and have the thread die
            {
                m_processingToBeDoneSemaphore.wait();
                if(!m_clientRequestCommandQueue.empty())
                {
                    processRequestCommand(m_clientRequestCommandQueue.front());
                    m_clientRequestCommandQueue.pop();
                }
                else if(!m_internalRequestCommandQueue.empty())
                {
                    processRequestCommand(m_internalRequestCommandQueue.front());
                    m_internalRequestCommandQueue.pop();
                }
            }
            catch(const TA_Base_Core::TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
            }
        } // end while( false == m_doTerminate)
        deactivateCommunicationsChannels();
        FUNCTION_EXIT;
    }


    //
    // terminate
    //
    void COECommunicationsHandler::terminate()
    {
        m_doTerminate = true;
        m_processingToBeDoneSemaphore.post();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // loadCommunicationChannels
    //
    void COECommunicationsHandler::loadCommunicationsChannels()
    {
        FUNCTION_ENTRY("loadCommunicationsChannels");
        // Get the addresses of all the VideoSwitches and create CommsChannel for each.  This may throw database related exceptions which are documented
        // in the method comment.
        CommunicationsChannelInfo defaultChannelInfo;
        defaultChannelInfo.state = COECommunicationsChannel::NOT_CURRENTLY_ESTABLISHED;
        defaultChannelInfo.numberCyclesWithoutActiveChannel = 0;
        BOOST_FOREACH(TA_Base_Core::IEntityDataPtr& entity, m_videoSwitchData)
        {
            std::auto_ptr<TA_Base_Core::IData> entityParam = std::auto_ptr<TA_Base_Core::IData>(TA_Base_Core::EntityAccessFactory::getInstance().getParameter(entity->getName(), "AssetName"));
            std::string assetName;
			if(entityParam.get() != NULL && entityParam->getNumRows() > 0 && !entityParam->isNull(0, "VALUE"))
			{
				assetName = entityParam->getStringData(0, "VALUE");
			}
            AddressHelper::Address switchAddress = AddressHelper::splitAddress(entity->getAddress());
            m_communicationsChannels.push_back(COECommunicationsChannelPtr(new COECommunicationsChannel(m_activeSwitchChannelIndex++,
                                                                           switchAddress.host,
                                                                           switchAddress.port,
                                                                           m_switchConnectTimeout,
                                                                           m_maxSwitchResponseTime,
                                                                           m_switchPollInterval,
                                                                           *this,
                                                                           *m_receivedDataProcessor.get(),
                                                                           entity->getName(),
                                                                           entity->getKey(),
                                                                           entity->getLocation(),
                                                                           entity->getTypeKey(),
                                                                           entity->getSubsystem(),
                                                                           assetName)));
            m_communicationsChannelsInfo.push_back(defaultChannelInfo);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "COECommunications channel created for VideoSwitch at '%s:%s'", switchAddress.host.c_str(), switchAddress.port.c_str());
        }
        FUNCTION_EXIT;
    }


    //
    // activateCommunicationsChannels
    //
    void COECommunicationsHandler::activateCommunicationsChannels()
    {
        FUNCTION_ENTRY("activateCommunicationsChannels");
        BOOST_FOREACH(COECommunicationsChannelPtr& channel, m_communicationsChannels)
        {
            channel->activateChannel();
        }
        FUNCTION_EXIT;
    }


    //
    // deactivateCommuncationsChannels
    //
    void COECommunicationsHandler::deactivateCommunicationsChannels()
    {
        FUNCTION_ENTRY("deactivateCommuncationsChannels");
        TA_ASSERT(m_communicationsChannelsInfo.size() == m_communicationsChannels.size(),
                  "The number of CommunicationsChannels and CommunicationsChannelsInfo structs is not equal!");
        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        CommunicationsChannelsInfo::iterator commsChannelInfoIt = m_communicationsChannelsInfo.begin();
        for(; commsChannelIt != m_communicationsChannels.end(); ++commsChannelIt)
        {
            (*commsChannelIt)->deactivateChannel();
            //
            // Important to only acquire lock while channel not in process of deactivating
            // else potential for deadlock.
            //
            TA_Base_Core::ThreadGuard commsChannelsStateGuard(m_commsChannelsStateLock);
            (*commsChannelInfoIt).state = COECommunicationsChannel::NOT_CURRENTLY_ESTABLISHED;
            (*commsChannelInfoIt).numberCyclesWithoutActiveChannel = 0;
            ++commsChannelInfoIt;
        }
        m_activeSwitchChannelIndex = m_communicationsChannels.size(); // invalid index indicates no current active switch channel
        m_exceededNoActiveSwitchCycleAllowance = false;
        FUNCTION_EXIT;
    }


    //
    // cleanUp
    //
    void COECommunicationsHandler::cleanUp()
    {
        FUNCTION_ENTRY("cleanUpVideoSwitchData");
        m_videoSwitchData.clear();
        m_communicationsChannels.clear();
        m_communicationsChannelsInfo.clear();
        FUNCTION_EXIT;
    }


    //
    // processRequestCommand
    //
    void COECommunicationsHandler::processRequestCommand(RequestCommandSharedPtr command)
    {
        FUNCTION_ENTRY("processRequestCommand");
        //
        // Important that this thread of execution should not acquire the m_commsChannelsStateLock
        // while calling "sendCommand" on the channel. Otherwise deadlock can occur because the
        // channel's thread may be trying to acquire thwe same lock when processing it's polling
        // commands. This is why this method uses "isThereActiveSwitchChannel" to determine this
        // information.
        //
        unsigned int activeChannelIndex = 0;
        if(isThereActiveSwitchChannel(activeChannelIndex))
        {
            TA_ASSERT(0 != command.get(), "Request command shared ptr is null!");
            m_communicationsChannels[ m_activeSwitchChannelIndex ]->sendCommand(*command.get());
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "Request command could not be sent as no connection to active switch!");
            command->signalCouldNotBeSent();
        }
        FUNCTION_EXIT;
    }


    //
    // receivedChannelStateCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateCommsFailure(unsigned int channelIndex)
    {
    }


    //
    // receivedChannelStateNotCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateNotCommsFailure(unsigned int channelIndex)
    {
    }


    //
    // receivedChannelStateActive
    //
    void COECommunicationsHandler::receivedChannelStateActive(unsigned int channelIndex)
    {
        if(m_activeSwitchChannelIndex != channelIndex)
        {
            // TODO Ensure No Active Switch Alarm Closed
            m_activeSwitchChannelIndex = channelIndex;
			BOOST_FOREACH(CommunicationsChannelInfo& i, m_communicationsChannelsInfo)
            {
                i.numberCyclesWithoutActiveChannel = 0;
            }
            m_exceededNoActiveSwitchCycleAllowance = false;
        }
    }


    //
    // receivedChannelStateNotActive
    //
    void COECommunicationsHandler::receivedChannelStateNotActive(unsigned int channelIndex)
    {
        if(m_activeSwitchChannelIndex == channelIndex)
        {
            m_activeSwitchChannelIndex = m_communicationsChannels.size();
        }
        if(m_activeSwitchChannelIndex == m_communicationsChannels.size() && !m_exceededNoActiveSwitchCycleAllowance)
        {
            m_communicationsChannelsInfo[ channelIndex ].numberCyclesWithoutActiveChannel++;
            unsigned short minNumberCyclesWithoutActiveChannel = 0xffff;
			BOOST_FOREACH(CommunicationsChannelInfo& i, m_communicationsChannelsInfo)
			{
				if(i.numberCyclesWithoutActiveChannel < minNumberCyclesWithoutActiveChannel)
				{
					minNumberCyclesWithoutActiveChannel = i.numberCyclesWithoutActiveChannel;
				}
			}
            if(minNumberCyclesWithoutActiveChannel > NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE)
            {
                m_exceededNoActiveSwitchCycleAllowance = true;
            }
        }
    }


    //
    // isConnected
    //
    bool COECommunicationsHandler::isConnected()
    {
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        return (TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState()
                || TA_Base_Core::Thread::THREAD_STATE_NEW == getCurrentState());
    }


    //
    // isThereActiveSwitchChannel
    //
    bool COECommunicationsHandler::isThereActiveSwitchChannel(unsigned int& activeChannelIndex)
    {
        FUNCTION_ENTRY("isThereActiveSwitchChannel");
        TA_Base_Core::ThreadGuard guard(m_commsChannelsStateLock);
        activeChannelIndex = m_activeSwitchChannelIndex;
        FUNCTION_EXIT;
        return m_activeSwitchChannelIndex < m_communicationsChannels.size();
    }


    //
    // commandNotToBeProcessed
    //
    void  COECommunicationsHandler::commandNotToBeProcessed(RequestCommandSharedPtr command)
    {
        FUNCTION_ENTRY("commandNotProcessed");
        //
        // Log the fact the request command was never processed by CommsHandler (as disconnected)
        // and signal that it could not be sent so the process waiting for completion will wake
        // up. This should not happen as all servant bases are deactivated before the comms handler
        // is disconnected, so log as warning.
        //
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Request command could not be processed as COECommunications handler was disconnected.");
        command->signalCouldNotBeSent();
        FUNCTION_EXIT;
    }


    //
    // clearCommandQueues
    //
    void COECommunicationsHandler::clearCommandQueues()
    {
        FUNCTION_ENTRY("clearCommandQueues");
        while(!m_clientRequestCommandQueue.empty())
        {
            commandNotToBeProcessed(m_clientRequestCommandQueue.front());
            m_clientRequestCommandQueue.pop();
        }
        while(!m_internalRequestCommandQueue.empty())
        {
            commandNotToBeProcessed(m_internalRequestCommandQueue.front());
            m_internalRequestCommandQueue.pop();
        }
        // Ensure semaphore reset back to zero as there is no
        // processing to be done now that the queues have been cleared.
        while(m_processingToBeDoneSemaphore.tryWait());
        FUNCTION_EXIT;
    }


    unsigned long COECommunicationsHandler::getEntityKey()
    {
        return m_videoSwitchAgentData->getKey();
    }


} // TA_IRS_App
