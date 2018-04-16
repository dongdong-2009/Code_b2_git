/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/AbstractECSAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(AbstractECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define AbstractECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_


#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "app/ecs/ECSAgent/src/EcsGlobals.h"


class DataPointWriteQueueProcessor;
class EcsZoneDataSource;
class ItaEcsAgentSummary;

namespace TA_Base_Bus 
{
	class GenericAgent;
	class AbstractDecisionEngine;
	class DataPoint;
    class GenericAgent;
    class IEntity;
	class DataNode;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
}

namespace TA_Base_Core
{
	class IEntityData;
	class ConfigUpdateDetails;
	struct StateUpdateMessageCorbaDef;
}


class AbstractECSAgent : public virtual TA_Base_Bus::IGenericAgentUser,
						 public virtual TA_Base_Bus::IDataPointAgent, 
						 public virtual TA_Base_Core::IOnlineUpdatable,
						 public virtual TA_Base_Bus::IDataNodeAgent
{
	typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
public:

    AbstractECSAgent(ItaEcsAgentSummary& theSummary);

    virtual ~AbstractECSAgent();
	
	//TD18706,jianghp, handle the operation mode
	virtual void checkOperationMode(){};

    virtual void setupSpecifics();
    /**
      * agentTerminate
      *
      * This method is called by the GenericAgent when it is time to
      * terminate.
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
      * This method is called by the GenericAgent to allow the child entities to be
	  * created.  These entities are then managed by the generic agent.
      *
      * @param Entity data retrieved from the database.
      *
      * @return The constructed entity
      */
	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);
        //throw(TA_Base_Core::InvalidECSConfigurationException);

    TA_Base_Bus::IEntity* createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData );
        //throw(TA_Base_Core::InvalidECSConfigurationException);

    /**
      * agentSetMonitor
      *
      * This method is called by GenericAgent when the Alarm Agent operating mode is
      * set to Monitor.
      */
    virtual void agentSetMonitor();

    virtual void agentSetMonitorSpecifics() = 0;


    /**
      * agentSetControl
      *
      * This method is called by GenericAgent when the Alarm Agent operating mode is
      * set to Control.
      */
    virtual void agentSetControl();

    virtual void agentSetControlSpecifics() = 0;

    /**
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOffline( const std::string& group );

	/**
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOnline( const std::string& group );

    /**
      * registerForStateUpdates
      *
      * This pure virtual method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not they
      * are operating in monitor or control mode.
      */
    virtual void registerForStateUpdates();


    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      *
      */
    virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        // Not used.  Functionality moved to the EcsSynchronisationManager
    }


    /**
      * processOnlineUpdate
      *
      * This is called by Generic Agent whenever it gets an online update that it
      * cant handle itself.
      *
      * @param The update event
      */
    void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    /**
      * run
      *
      * This method calls GenericAgent::run() which blocks until the System
      * Controller shuts us down
      */
    void run();

    void killMeAndRaiseAlarm();

    // Implementing the IDataPointAgent interface
    virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );

	//Mintao++
	virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );
    
    virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
    
	virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );
	
	virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );

	//chenlei++
	//for loading thread to execute so as to perform some procedure in failover
	void failoverSpecific();
protected:


    /**
      * setup
      *
      * This method must be called by the derived classes before any other operations are
      * called in the base class.  It is best to call this method in the constructor of
      * the derived class.  This design is enforced by assertions.
      *
      * Unfortunately, this messiness was necessary because the setup method calls pure
      * virtual functions at specificly ordered points in the method.  If this was called
      * from the base constructor as originally intended, the application asserts because
      * at the time the base constructor is called, the derived class instance does not
      * exist yet.
      *
      */
    void setup(int argc, char* argv[]);
        //throw(TA_Core::GenericAgentException, TA_Core::DatabaseException, TA_Core::DataException);

    /**
      * cleanup
      *
      * This method must be called by the derived classes before the base class destructor
      * is called.  It is best to call this method in the destructor of the derived class.
      * This design is enforced by an assertion in the base class destructor.
      *
      * Unfortunately, this messiness was necessary due to the same reasons described in
      * setup().  If this was called from the base destructor as originally intended, the
      * application asserts because by the time the base destructor is called, the derived
      * class instance would have been destroyed already.
      *
      */
    void cleanup();
    virtual void cleanupSpecifics() = 0;
    virtual void stopSpecificThreads() = 0;

    void processConfiguration();
        //throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException);
    virtual void processSpecificConfiguration() = 0;
        //throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException) = 0;

    void validateRuntimeConfiguration();
        //throw(TA_Base_Core::InvalidECSConfigurationException);
    virtual void validateSpecificRuntimeConfiguration() = 0;
        //throw(TA_Base_Core::InvalidECSConfigurationException) = 0;

        //throw(TA_Core::GenericAgentException, TA_Base_Core::InvalidECSConfigurationException, TA_Base_Core::CachedMappingNotFoundException) = 0;

    // For termination

    void updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

    // liuyu++ start master/station specific threads
    virtual void startSpecificThreads() = 0;
    // ++liuyu start master/station specific threads

    virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


    ItaEcsAgentSummary&                         m_theSummary;
    // Attributes
    TA_Base_Core::ReEntrantThreadLockable    m_lockForOperationModes;

    TA_Base_Bus::GenericAgent*                  m_genericAgent;
    TA_Base_Core::EOperationMode                m_operationMode;


    DataPoints                                  m_dataPoints;
    DataPointWriteQueueProcessor*               m_dataPointWriteQueueProcessor;
	EcsZoneDataSource*                          m_ecsZoneDataSource;


    // This flag is used as a workaround for the limited agentSetControl()
    // and agentSetMonitor() methods.  Ideally, it would be nice to be able
    // to pass in a parameter to inidcate whether the agent is starting up 
    // in control mode or switching to control mode from monitor mode.  
    // Instead, we have to rely on this member to flag the agent startup
    volatile bool                               m_isOnStartup;


    // Deliberately created as a member because it is needed before generic agent is created.
    TA_Base_Bus::DataPointFactory*               m_dataPointFactory;

    std::string                                 m_agentName;
private:

    // Disable copy constructor and assignment operator
    AbstractECSAgent( const AbstractECSAgent& theAgent);
    AbstractECSAgent& operator=(const AbstractECSAgent &);

    // Bulk polling library
	TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
	TA_Base_Bus::SummaryProcessor* m_summaryProcessor;
	TA_Base_Bus::ScadaPersistencePtr m_persistence;
	
	TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_dpBulkPolledInterface;
	TA_Base_Bus::DataPointAccessCorbaDef_Impl * m_dpAccessInterface;
	TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_dnBulkPolledInterface;
	TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_dnAccessInterface;
	
	TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
	TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;
	
	DataNodeList m_dataNodes;
};

class failoverLoadingThread : public TA_Base_Core::Thread
{
public:
	failoverLoadingThread(AbstractECSAgent* pEcsAgent): m_ecsAgent(pEcsAgent) {};
	~failoverLoadingThread(){};

	virtual void run(){ m_ecsAgent->failoverSpecific(); };
	virtual void terminate(){};

private:
	AbstractECSAgent* m_ecsAgent;
};
#endif // !defined(AbstractECSAgent_A7EE4E27_1FF9_4bb8_BBB7_6B1F2C675F52__INCLUDED_)
