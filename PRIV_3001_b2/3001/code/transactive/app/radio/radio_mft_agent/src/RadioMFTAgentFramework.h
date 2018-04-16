/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentFramework.h $
 * @author:  Glen Kidd
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

#ifndef RADIO_MFT_AGENT_FRAMEWORK_H_INCLUDED
#define RADIO_MFT_AGENT_FRAMEWORK_H_INCLUDED

#include "bus/generic_agent/src/IGenericAgentUser.h"

#include "app/radio/radio_mft_agent/src/RadioMFTAgent.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentSummary.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentCommandServant.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

// forward declarations
namespace TA_Base_Bus
{
    class IEntity;
}

class RadioMFTAgentFramework :  public TA_Base_Bus::IGenericAgentUser
{

public:
	RadioMFTAgentFramework();
	virtual ~RadioMFTAgentFramework();

public:
    /**
     * These functions are not relevant for this MFT-based agent.
     * 
     */
	virtual void agentSetMonitor();
	virtual void agentTerminate();
	virtual void agentSetControl();
    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);
	virtual void notifyGroupOffline(const std::string& group);
	virtual void notifyGroupOnline(const std::string& group);
	virtual void registerForStateUpdates();
	virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
	virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    virtual void run();
    
    RadioMFTAgentCommandServant* getCommandServant() {return m_RadioMFTAgentCommandServant;};

    virtual void checkOperationMode(){};
private:
    //Disabled Methods
    RadioMFTAgentFramework(const RadioMFTAgentFramework&);
    RadioMFTAgentFramework& operator=(const RadioMFTAgentFramework&);

	/**
	  * setup
      * 
      * Initializes the agent
	  */
    void setup();

	/**
	  * cleanup
	  * 
      * Clean up all member variables
	  */
    void cleanup(); 

    unsigned long getLocationKey();
    
private:
	RadioMFTAgent                   m_RadioMFTAgent;
	RadioMFTAgentSummary*           m_RadioMFTAgentSummary;
	RadioMFTAgentCommandServant*    m_RadioMFTAgentCommandServant;
};
#endif // !defined({DED8718A-0569-49d9-872F-4D9D1D9E9415}__INCLUDED_)
