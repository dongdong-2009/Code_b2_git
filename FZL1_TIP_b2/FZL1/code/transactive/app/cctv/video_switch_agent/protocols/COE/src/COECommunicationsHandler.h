#if !defined(COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

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
  * Handles the protocol dependent sections of the operations for a CommunicationsHandler
  * using the COE protocol.
  * This interface is used by the CommunicationsHandler class.
  *
  */

#include "app/cctv/video_switch_agent/protocols/src/IPDCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SynchronisedQueue.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "bus/trains/TrainCommonLibrary/src/AtsZoneUtils.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include <string>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace TA_Base_Core
{
    class AlarmHelper;
    class IEntityData;
}

namespace TA_IRS_App
{
    class IAgentRequestCommand;
    class ReceivedDataProcessor;
}

namespace TA_IRS_App
{
    class COECommunicationsHandler : public TA_Base_Core::Thread,
        public IPDCommunicationsHandler,
		public ICOEChannelStateChangeObserver,
        boost::noncopyable
    {
        public:

            //
            // Have to use reference counting pointers to queue commands so can guarantee they only
            // get cleaned up when all references to it are out of scope.
            //
            typedef boost::shared_ptr< IAgentRequestCommand > RequestCommandSharedPtr;

            /**
              * COECommunicationsHandler
              *
              * Standard constructor.
              *
              * @param      TA_Base_Core::VideoSwitchAgent& videoSwitchEntityData
              *             The configuration data for this CommunicationsHandler.
              *
              * @exception   VideoSwitchAgentInternalException
              *              Thrown if mandatory VideoSwitchAgent parameters or configured VideoSwitch
              *              entity addresses cannot be retrieved from the database.
              */
            COECommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentEntityData);


            /**
              * ~COECommunicationsHandler
              *
              * Standard destructor.
              */
            virtual ~COECommunicationsHandler();


            /**
              * submitRequestCommand
              *
              * Adds the command to the vector of commands ready to be sent to
              * the Video Switch.
              *
              * @param command
              * @param
              *
              */
            void submitRequestCommand(RequestCommandSharedPtr command, bool isClientRequest);


            /**
              * processCommunicationsChannelState
              *
              * description
              *
              * @param channelIndex
              * @param
              *
              */
			virtual void processCommunicationsChannelState(unsigned int channelIndex, COECommunicationsChannel::ECommunicationChannelState state);


            ///////////////////////////////////////////////////////////////////////
            //
            // IPDCommunicationsHandler Methods
            //
            ///////////////////////////////////////////////////////////////////////


            /**
              * connect
              *
              * Connects to the switching hardware. Once connected this will
              * request the state of all devices from the hardware.
              */
            virtual void connect();


            /**
              * disconnect
              *
              * Disconnects from the switching hardware.
              */
            virtual void disconnect();


            virtual unsigned long getEntityKey();

            ///////////////////////////////////////////////////////////////////////
            //
            // Thread Methods
            //
            ///////////////////////////////////////////////////////////////////////


            /**
              * run
              *
              * Continually processes requests from the agent and responses from
              * the hardware. Commands from the received queue have priority over
              * the request queue.
              */
            virtual void run();


            /**
              * terminate
              *
              * Signals the run loop to complete. This method should not be called
              * directly. Clients should call disconnect.
              */
            virtual void terminate();

        private:

            //
            // Private Helper Functions
            //

            /**
             * loadCommunicationsChannels
             *
             * Creates a COECommunicationChannel for each VideoSwitch address and port number from the database.
             *
             * @exception   DatabaseException
             *              Thrown if the address data  cannot be retrieved from the database.
             * @exception   DataException
             *              Thrown if the type for the address data retrieved from the database is invalid.
             */
            void loadCommunicationsChannels();


            /**
              * activateCommunicationsChannels
              *
              * description
              *
              */
            void activateCommunicationsChannels();


            /**
              * deactivateCommuncationsChannels
              *
              * description
              *
              */
            void deactivateCommunicationsChannels();


            /**
              * cleanUp
              *
              * description
              *
              */
            void cleanUp();


            /**
              * processRequestCommand
              *
              * description
              *
              * @param
              */
            void processRequestCommand(RequestCommandSharedPtr command);


            /**
              * receivedChannelStateCommsFailure
              *
              * description
              *
              */
            void receivedChannelStateCommsFailure(unsigned int channelIndex);


            /**
              * receivedChannelStateNotCommsFailure
              *
              * description
              *
              */
            void receivedChannelStateNotCommsFailure(unsigned int channelIndex);


            /**
              * receivedChannelStateActive
              *
              * description
              *
              * @param
              */
            void receivedChannelStateActive(unsigned int channelIndex);


            /**
              * receivedChannelStateNotActive
              *
              * description
              *
              * @param
              */
            void receivedChannelStateNotActive(unsigned int channelIndex);


            /**
              * isConnected
              *
              * description
              *
              *
              * @return
              */
            bool isConnected();


            /**
              * isThereActiveSwitchChannel
              *
              * description
              *
              * @param
              *
              * @return
              */
            bool isThereActiveSwitchChannel(unsigned int& activeChannelIndex);


            /**
              * commandNotToBeProcessed
              *
              * description
              *
              * @param
              */
            void commandNotToBeProcessed(RequestCommandSharedPtr command);


            /**
              * clearCommandQueues
              *
              * description
              */
            void clearCommandQueues();

            //
            // Private Types
            //

            struct CommunicationsChannelInfo
            {
                COECommunicationsChannel::ECommunicationChannelState state;
                unsigned short numberCyclesWithoutActiveChannel;
            };

			typedef boost::shared_ptr<COECommunicationsChannel> COECommunicationsChannelPtr;
            typedef SynchronisedQueue< RequestCommandSharedPtr > RequestCommandQueue;
            typedef std::vector< TA_Base_Core::IEntityDataPtr > VideoSwitchData;
            typedef std::vector< COECommunicationsChannelPtr > CommunicationsChannels;
            typedef std::vector< CommunicationsChannelInfo > CommunicationsChannelsInfo;

            //
            // Private member variables
            //

            static const std::string VIDEO_SWITCH_TYPE; // The type to use when retrieving the VideoSwitch data.
            static const unsigned short NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE; // Num cycles before alarm that no switch
            // has nominated itself as the Active one.
            TA_Base_Core::VideoSwitchAgentPtr m_videoSwitchAgentData;
            TA_Base_Core::ILocation::ELocationType m_locationType;
            unsigned long m_switchConnectTimeout;
            unsigned long m_switchPollInterval;
            unsigned long m_maxSwitchResponseTime;
            VideoSwitchData m_videoSwitchData; // The VideoSwitch database objects providing address information for each comms channel to be
            // created.
            CommunicationsChannels m_communicationsChannels;
            CommunicationsChannelsInfo m_communicationsChannelsInfo;
            unsigned int m_activeSwitchChannelIndex;
            bool m_exceededNoActiveSwitchCycleAllowance;
            volatile bool m_doTerminate; // Flag indicating if when thread method (run) should terminate.
            TA_Base_Core::ReEntrantThreadLockable m_commsChannelsStateLock; // Synchronise access to methods effecting or requiring knowledge of
            TA_Base_Core::ReEntrantThreadLockable m_threadStateLock; // Synchronise access to methods effecting or requiring knowledge of
            RequestCommandQueue m_clientRequestCommandQueue; // Client Request Commands to be processed
            RequestCommandQueue m_internalRequestCommandQueue; // Internal Request Commands to be processed
            TA_Base_Core::Semaphore m_processingToBeDoneSemaphore; // Semaphore indicating commands are there to be processed
            std::auto_ptr< ReceivedDataProcessor > m_receivedDataProcessor;
            typedef std::vector<TA_IRS_Core::IVideoTriggeringEvent*> VideoTriggeringEventsList;
            std::map<unsigned long, unsigned long>                              m_consoleKeyToMonitorIdMap;
    }; // COECommunicationsHandler

} // TA_IRS_App

#endif // !defined(COECommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
