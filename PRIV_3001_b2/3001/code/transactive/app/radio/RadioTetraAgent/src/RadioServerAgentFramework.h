/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentFramework.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * The deployment architecture for this agent is to run it as a library/component
 * withing the Radio Manager GUI. As such, there is no use of Generic Agent,
 * however, the MFT Agent must be written to facilitate subsequent separation from
 * the GUI, if required.
 * 
 * If there is any typically Generic Agent functionality that is required, then
 * this must be separated into a helper class (for maintenance) and implemented
 * within this Agent library.
 * 
 * @note consider possibility of having both a Generic GUI and Generic Agent
 * derived classes in a single application, assuming there aren't unresolvable
 * conflicts in this case.
 */

#ifndef RADIO_SERVER_AGENT_FRAMEWORK_H_INCLUDED
#define RADIO_SERVER_AGENT_FRAMEWORK_H_INCLUDED

#include <string>

//#include "app/radio/RadioTetraAgent/src/RadioServerAgentSummary.h"
#include "app/radio/RadioTetraAgent/src/RadioServerAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSLinkStatusThread.h"



namespace TA_Base_Bus
{
    class IEntity;
    class GenericAgent;
};

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
};

namespace TA_IRS_App
{

struct StateUpdateMessageCorbaDef;
class RadioServerAgent;
//class RadioServerAgentSummary;
class RadioSDS;
class RadioSDSSummary;


class RadioServerAgentFramework : public TA_Base_Bus::IGenericAgentUser,
								  public TA_Base_Bus::IDataNodeAgent,
	                              public TA_Base_Bus::IDataPointAgent
{
	typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
	typedef std::map< unsigned long, TA_Base_Bus::DataPoint* > DatapointList;

public:
    /**
      * Constructor
      *
      * @param The number of command line arguments passed in
      * @param The command line
      *
      * @exception GenericAgentException thrown if GenericAgent fails to initialise.
      * @exception DatabaseException thrown if a database error occurs
      * @exception DataException thrown if data returned from the database is not valid
      */
	RadioServerAgentFramework( int argc, char *argv[]);

	virtual ~RadioServerAgentFramework();

    /**
      * This method calls GenericAgent::run() which blocks until the System
      * Controller shuts us down
      */
    void run();

	void checkOperationMode();
public:

	virtual void agentSetMonitor();

	virtual void agentSetControl();

    /**
     * If the agent's entities implement CORBA interfaces, this method should take
     * care of deactivating them.
     * 
     * @note If the class implementing this interface holds the GenericAgent as a
     * member variable, DO NOT delete it inside this method. GenericAgent waits for
     * this call to return before shutting down the ORB. Deleting the GenericAgent
     * from within this method will cause obvious problems.
     */
	virtual void agentTerminate();
	
    virtual bool createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                        const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities );

    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);

	virtual void notifyGroupOffline(const std::string & group);

	virtual void notifyGroupOnline(const std::string & group);

	virtual void registerForStateUpdates();

	virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

	virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

	// IDataNodeAgent interface functions
	virtual TA_Base_Bus::DataNode* getDataNode ( unsigned long entityKey );
	virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode* >& dataNodeList );

	// IDataPointAgent interface functions
	virtual TA_Base_Bus::DataPoint* getDataPoint ( unsigned long entityKey );
	virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint* >& dataPointList );
	virtual TA_Base_Bus::DataPointFactory* getDataPointFactory ();

	void setSDSLinkStatusDp(IRadioSDSLinkStatus* sdsLinkStatus);


private:
    RadioServerAgentFramework(const RadioServerAgentFramework&);
    void operator=(const RadioServerAgentFramework&);

private:

	//RadioServerAgentSummary *m_radioServerAgentSummary;

	RadioServerAgent *m_radioServerAgent;


    /*
	 * The RadioServerAgent is the entry-point class for the oursourcer, this class
	 * Is responsibel for setting up the outsourced components to enable runtime operations;
	 * It is not sure if there should be GenericAgent functionality;
	 */
	TA_Base_Bus::GenericAgent* m_pGenericAgent;

    /**
	 *	Used for safe acess;
	 */
	TA_Base_Core::ReEntrantThreadLockable m_lock;


    RadioServerAgentCommandServant* m_radioCommandServant;

	RadioSDSSummary* m_radioSDSSummary;

	// for data point
	TA_Base_Bus::DataPointFactory*					m_dataPointFactory;
	TA_Base_Bus::ScadaProxyFactory*					m_scadaProxyFactory;
	TA_Base_Bus::SummaryProcessor*					m_summaryProcessor;
	TA_Base_Bus::ScadaPersistencePtr				m_persistence;

	TA_Base_Bus::DataPointAgentCorbaDef_Impl*		m_dpBulkPolledInterface;
	TA_Base_Bus::DataPointAccessCorbaDef_Impl*		m_dpAccessInterface;
	TA_Base_Bus::DataNodeAgentCorbaDef_Impl*		m_dnBulkPolledInterface;
	TA_Base_Bus::DataNodeAccessCorbaDef_Impl*		m_dnAccessInterface;

	TA_Base_Core::ReEntrantThreadLockable			m_dataNodeListLock;
	TA_Base_Core::ReEntrantThreadLockable			m_dataPointListLock;
	DataNodeList									m_dataNodes;
	DatapointList									m_dataPointList;
	TA_Base_Bus::DataPoint*                         m_radioFallbackDp;
	
	std::string										m_entityName;
	RadioSDSLinkStatusThread*                       m_radioSDSLinkStatus;
};
}
#endif
