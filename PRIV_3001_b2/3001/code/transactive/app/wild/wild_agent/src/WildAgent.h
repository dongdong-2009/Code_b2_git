//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/src/WildAgent.h $
// @author:  Andy Siow
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// Wild Agent definition file. 
//

#if !defined(WILDAGENT)
#define WILDAGENT

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

#include "bus/ats/ats_agent/IDL/src/IAtsWildCorbaDef.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include <vector>

// Forward declarations.
namespace TA_Base_Bus
{
	class DataPointFactory;
    class GenericAgent; 
    class IEntity; 
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
}

namespace TA_Base_Core
{
    class IEntityData; 
    class ConfigUpdateDetails;
	class StateUpdateMessageSender;
    struct StateUpdateMessageCorbaDef; 	
}

// End of forward declarations.

namespace TA_IRS_App
{
	class ATSTrainTableSubscriber;

	class WILDAgent : public virtual TA_Base_Bus::IGenericAgentUser,
					  //public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >,//limin, failover issue
					  public virtual TA_Base_Bus::IDataPointAgent,
					  public virtual TA_Base_Bus::IDataNodeAgent
	{
	typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;

	public:

	   //
	   // WILDAgent
	   // 
	   // The WildAgent constructor.
	   //
	   // @param the number of commandline arguemnts.
       //
	   // @param the command line arguments.
	   //
	   // @exception GenericAgentException thrown if GenericAgent fails to initialise
	   //
		WILDAgent(int argc, char* argv[]);

		//
        // ~WildAgent
        //
		// The Wild Agent destructor. Declared virtual in case it is subclassed. 
		//
        virtual ~WILDAgent();

		//
		// operations required by Thread
		//
		virtual void run();

		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode(){};
		//
		// agentTerminate
		//
		// This method is called by GenericAgent whenever the WILDAgent is required
		// to shut down. This method should stop any threads that may be running, 
		// deactivate any active CORBA servants and any other preparation for deletion.
		//
		void agentTerminate();

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

		//
		// createEntity
		//
		// Generic Agent will take all configuration data for all entities and returns
		// the entity object. There will be approximately 2400 virtual datapoints that  
		// are descendants of the WILD Agent as these datapoints corresponds to the 
		// state of each wheel, on each train (24 X 1000). 
		//
		// @param Entity data retrieved from the database.
		//
		// @return The constructed entity
		//
	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		//
		// agentSetMonitor
		//
		// This method is called by GenericAgent when the Alarm Agent operating mode is
		// set to Monitor mode. 
		//
		void agentSetMonitor();

		//
		// agentSetControl
		//
		// This method is called by GenericAgent when the Alarm Agent operating mode is
		// set to Control mode. 
		//
		void agentSetControl();
		
		//
		// notifyGroupOffline
		//
		// This method informs the WILD Agent that a group of System Controllers is now
		// "OffLine". 
		//
		void notifyGroupOffline( const std::string& group );

		//
		// notifyGroupOnline
		//
		// This method informs the WILD Agent that there is at least one System Controller
		// in the group that is online.
		//
		void notifyGroupOnline( const std::string& group );

		//
		// registerForStateUpdates
		//
		// This method allows the WILD Agent to receive either requests for state updates
		// or state update messages. 
		//
		void registerForStateUpdates();

		//
		// receiveSpecialisedMessage
		//
		// This method is called whenever the WILD Agent receives a state update or
		// state update request (depending on their mode of operation).
		//
		void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		//
		// receiveSpecialisedMessage
		//
		// This method is called whenever the Agent receives WILD Alarm information from the 
		// WILD processor.
		//
		//virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message); //limin--, fail over issue

		//
		// processOnlineUpdate
		//
		// Any changes to the WILD Agent entities or its descendants will have its updates 
		// received and dealt with in this method. 
		//
		void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		//
		// operations required by IDataPointAgent interface
		//
		//get a single datapoint
		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );
		//get all the datapoints
		virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );
		//
		// createDataPointEntity
		//
		// This creates the DataPoint entity
		//
		// @return A pointer to the IEntity object
		//
		// @param The entity data
		//
		TA_Base_Bus::IEntity* createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData );

		//
		// registerForControlMessages
		//
		// This method registers for the messages we expect to receive while in 
		// control mode.
		//
		void registerForControlMessages();

		//
		// registerForMonitorMessages
		//
		// This method registers for the messages we expect to receive while in 
		// monitor mode.
		//
		void registerForMonitorMessages();

		//
		// cleanUp
		//
		// This method will clean up all the Agent's member objects. 
		//
		void cleanUp();
        
        virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
        
		//
		// operations required by IDataNodeAgent interface
		//
		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );

		/**
		* getAllDataNodes 
		* 
		* 
		* @return virtual void 
		* @param : std::map < unsigned long
		* @param : TA_Base_Bus::DataNode *>& dataNodeList
		* 
		* @exception <exceptions> Optional
		*/
        virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );
	private:

		// Disable the copy constructor and assignment operator so the compiler won't create its own version.
		// Reference: Effective C++
		WILDAgent( const WILDAgent& WILDAgent);
		WILDAgent& operator=(const WILDAgent&);

        void agentSetControlInThread();//limin, for failover

		TA_Base_Bus::GenericAgent* m_genericAgent;
		TA_Base_Bus::DataPointFactory* m_dataPointFactory;
		TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_dpBulkPolledInterface;
		TA_Base_Bus::DataPointAccessCorbaDef_Impl* m_dpAccessInterface;

		//////////// Member variables & structures

		//TA_Base_Core::StateUpdateMessageSender* m_stateUpdateRequestSender;//limin--, failover issue

		ATSTrainTableSubscriber*		   m_atsTrainSubscriber;

		TA_Base_Core::EOperationMode m_currentMode;

		std::map<unsigned short, int> m_trainNumbers;

		TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_dnAccessInterface;
		TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
		TA_Base_Bus::SummaryProcessor* m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr m_persistence;
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
		DataNodeList m_dataNodes;
		std::string m_agentName;
		//////////// Private functions

		//
		// initialiseLocalVariables
		//
		// This method initialises the WILD Agent local variables.
		// 
		void initialiseLocalVariables();


		//
		// updateWILDAgentConfiguration
		//
		// Updates the configuration paramters for the WILD Agent in WILDConfiguration 
		// singleton class
		//
		// @param	updateEvent	The configuration update  
		//
		void updateWILDAgentConfiguration(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		//datapoint map lock
		TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;

		//datapoint map
		std::map< unsigned long, TA_Base_Bus::DataPoint * > m_wildDataPointList;
	};

}

#endif // !defined(WILDAGENT)
