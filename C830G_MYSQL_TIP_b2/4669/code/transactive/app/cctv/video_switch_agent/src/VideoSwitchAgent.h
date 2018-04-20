#if !defined VIDEO_SWITCH_AGENT_H
#define VIDEO_SWITCH_AGENT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/VideoSwitchAgent.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implements the IGenericAgentUser interface.  Handles Agent specific
  * tasks required on start up and shutdown, as well as process 
  * management messages and instructions originating from the System Controller.
  */

#include <memory>
#include <string>

#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/CommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/StateUpdateSubscriber.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

// Forward declarations

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
}
namespace TA_Base_Bus
{
    class GenericAgent;
    class IEntity;
    class DataPoint;
    class DataPointFactory;	
	class DataNode;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;

}
namespace TA_IRS_App
{
    class CommunicationsHandler;
}

namespace TA_IRS_App
{
    class VideoSwitchAgent : public virtual TA_Base_Bus::IGenericAgentUser, 
							 public virtual TA_Base_Bus::IDataPointAgent,
							 public virtual TA_Base_Bus::IDataNodeAgent 
    {
		typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
    public:
        /**
          * VideoSwitchAgent
          * 
          * Constructor that creates the object that handles communication
          * with the switching hardware and the factory and manager that
          * handle the object monitored by this agent.
          *
          * @param      int argc
          *             The number of command line arguments passed in
          * @param      char* argv[]
          *             The command line
          *
          * @exception  GenericAgentException 
          *             Thrown if GenericAgent fails to initialise.
          */

        VideoSwitchAgent( int argc, char* argv[] );

        /**
          * ~VideoSwitchAgent
          *
          * Standard destructor.
          */

        virtual ~VideoSwitchAgent();

		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode(){};
        /**
          * agentRun
          *
          * Calls GenericAgent::run() which blocks until the System
          * Controller shuts this Agent down.
          */

        void agentRun();

        /**
          * agentTerminate
          *
          * Called by the GenericAgent when it is time to terminate.
          */

        virtual void agentTerminate();

        /** 
          * createAllEntities
          *
          * [optional]
          *
          * This allows for bulk entity creation.
          * It is called first (before any calls to createEntity) to allow agents to
          * optimise entity creation. If true is returned, the entities are already created
          * if false is returned, then createEntity is called one by one for each entity.
          * Also entityCreationComplete will be called at the end of the creation
          *
          * Note that even if using this method, createEntity should still be implemented
          * as it is used for entity updates
          *
          * @param agentEntityData the agent's entity data
          * @param entityDataList a list of shared pointers to all entities
          * @param createdEntities the entities created to return to generic agent
          *
          * @return true if entity creation was done, false if createEntity should be used
          *
          */
        virtual bool createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                        const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities );

        /** 
          * entityCreationComplete
          *
          * [optional]
          *
          * this is called back after 
          * createEntity has been called for all entities.
          * The client can do any clean up of temporary data.
          */
        virtual void entityCreationComplete();

        /**
          * createEntity
          *
          * Called by the GenericAgent. 
          *
          * @return     The constructed entity.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             Entity data retrieved from the database.
          */

	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

        /**
          * agentSetMonitor
          *
          * Called by GenericAgent when the Video Switch Agent 
          * operating mode is set to Monitor.
          */

        virtual void agentSetMonitor();

        /**
          * agentSetControl
          *
          * Called by GenericAgent when the Video Switch Agent 
          * operating mode is set to Control.
          */

        virtual void agentSetControl();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
          *
          * @param      const std::string& group
          *             The group name.
		  */

        virtual void notifyGroupOffline( const std::string& group );

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOnline from the Process Monitor.
          *
          * @param      const std::string& group
          *             The group name.
		  */

        virtual void notifyGroupOnline( const std::string& group );

        /**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether 
          * or not they are operating in Monitor or Control mode.
          */

        virtual void registerForStateUpdates();

        /**
          * receiveSpecialisedMessage
          *
          * Called whenever the Agent receives a state update or
          * state update request (depending on its mode of operation).
          *
          * @param      const TA_Base_Core::StateUpdateMessageCorbaDef& message
          *             The update message.
          */

        virtual void receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message );

        /**
          * processOnlineUpdate
          *
          * Called by Generic Agent whenever it gets an online update 
          * that it cannot handle itself.
          *
          * @param      const TA_Base_Core::ConfigUpdateDetails& updateEvent
          *             The update message.
          */

        virtual void processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

		/**
		 * getAgentKey()
		 *
		 * @return the entity pkey for the Video Switch Agent 
		 *
		 */
		TA_Base_Core::VideoSwitchAgentPtr& getAgentConfig();


        //TD16761 Mintao++
    	//
		// operations required by IDataPointAgent interface
		//


		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );


        virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );

		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );

        virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );

        virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();

        //TD16761 Mintao++
    private:

        //
        // Disable copy constructor and assignment operator.
        //

        VideoSwitchAgent( const VideoSwitchAgent& theVideoSwitchAgent );
        VideoSwitchAgent& operator=( const VideoSwitchAgent & );

        /**
          * registerForControlMessages
          *
          * Registers for the messages the Agent expects to receive 
          * while in Control mode.
          */

        void registerForControlMessages();

        /**
          * registerForMonitorMessages
          *
          * Registers for the messages the Agent expects to receive 
          * while in Monitor mode.
          */

        void registerForMonitorMessages();

        /** 
          * setChildrenToControl
          *
          * description
          */

        void setChildrenToControl();

        /** 
          * setChildrenToMonitor
          *
          * description
          */

        void setChildrenToMonitor();
        
        /** 
          * cleanUp
          *
          * description
          *
          */

        void cleanUp();

        //
        // The Generic Agent.
        //

        TA_Base_Bus::GenericAgent* m_genericAgent;
        
        //
        // The object that handles communication with the hardware.
        //

        std::auto_ptr< CommunicationsHandler > m_communicationsHandler;

        //
        // The object that handles the state update and state update request messages.
        //
        
        StateUpdateSubscriber m_stateUpdateSubscriber;
		bool m_running;

		TA_Base_Core::VideoSwitchAgentPtr	m_agentConfig;
        TA_Base_Bus::DataPointFactory* m_dataPointFactory;

        //TD16761 Mintao++
        //added by Mintao for bulk pull interface 
		TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
		TA_Base_Bus::SummaryProcessor* m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr m_persistence;
		
		TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_dpBulkPolledInterface;
        TA_Base_Bus::DataPointAccessCorbaDef_Impl * m_dpAccessInterface;
		TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_dnAccessInterface;
        
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
		
		DataNodeList m_dataNodes;
		
		std::string m_agentName;
    };

} // TA_IRS_App

#endif // !defined VIDEO_SWITCH_AGENT_H
