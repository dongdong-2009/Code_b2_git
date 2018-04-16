#if !defined(SCADAHISTORIANAGENT)
#define SCADAHISTORIANAGENT

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/ScadaHistorianAgent.h $
  * @author:   Anita Lee
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include <vector>
#include "bus/generic_agent/src/IGenericAgentUser.h"

//
// Forward declarations
//

namespace TA_Bus
{
    class GenericAgent; 
    class IEntity; 
}

namespace TA_Core
{
    class IEntityData; 
    class ConfigUpdateDetails;
    struct StateUpdateMessageCorbaDef;
}


namespace TA_App
{
    //
	// Forward Declarations
	//
    class ScadaHistorianAgentProcess;
    class HistoryConfig;

    class ScadaHistorianAgent : public virtual TA_Bus::IGenericAgentUser
    {

    public:

        /**
          * ScadaHistorianAgent
		  *
		  * Constructor
          *
          * @param The number of command line arguments passed in 
          * @param The command line
          *
          * @exception GenericAgentException thrown if GenericAgent fails to
          *            initialise.
		  *
          */
        ScadaHistorianAgent(int argc, char* argv[]);

        /**
		  * ~ScadaHistorianAgent
		  *
          * Destructor
		  *
		  */
        virtual ~ScadaHistorianAgent();

        /**
          * agentTerminate
          *
          * This method is called by the GenericAgent when it is time to
          * terminate.
		  *
          */
	    void agentTerminate();

        /**
          * createEntity
          *
          * This method is called by the GenericAgent. It creates an IEntity
		  * for each child entity of the Data Recording Agent.
          *
          * @param Entity data retrieved from the database.
          *
          * @return The constructed entity
		  *
          */
	    TA_Bus::IEntity* createEntity(TA_Core::IEntityData& entityData);

        /**
          * agentSetMonitor
          *
          * This method is called by GenericAgent when the Data Recording Agent 
		  * operating mode is set to Monitor. 
		  *
          */
	    void agentSetMonitor();

        /**
          * agentSetControl
          *
          * This method is called by GenericAgent when the Data Recording Agent 
		  * operating mode is set to Control. 
		  *
          */
	    void agentSetControl();

        /**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether or not they
          * are operating in monitor or control mode.
		  *
          */
	    void registerForStateUpdates();

        /**
          * receiveSpecialisedMessage
          *
          * This method is called whenever the Agent receives a state update or
          * state update request (depending on their mode of operation)
		  *
          */
        void receiveSpecialisedMessage(const TA_Core::StateUpdateMessageCorbaDef& message);

        /**
          * processOnlineUpdate
          *
          * This is called by Generic Agent whenever it gets an online update that it
          * cant handle itself.
          *
          * @param The update event
		  *
          */
        void processOnlineUpdate(const TA_Core::ConfigUpdateDetails& updateEvent);

        /**
          * run
          *
          * This method calls GenericAgent::run() which blocks until the System
          * Controller shuts us down
		  *
          */
        void run();

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
        
    private:

		//
        // Disable copy constructor and assignment operator
		//
        ScadaHistorianAgent( const ScadaHistorianAgent& theScadaHistorianAgent);
        ScadaHistorianAgent& operator=(const ScadaHistorianAgent &);

        TA_Bus::GenericAgent* m_genericAgent;
        
        ScadaHistorianAgentProcess* m_scadaHistorianAgentProcess;

        std::vector<HistoryConfig*> m_dataCollectionConfig;
    };
}

#endif // !defined(SCADAHISTORIANAGENT)
