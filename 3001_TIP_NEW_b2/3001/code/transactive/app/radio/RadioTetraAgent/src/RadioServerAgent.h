/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgent.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * The RadioServerAgent is the entry-point class for the outsourcer.  This class
 * is responsible for setting up the outsourced components to enable runtime
 * operations.
 * 
 * The deployment architecture for this agent is to run it as a typical
 * transActive Agent on a Solaris Server.
 * 
 */

#ifndef RADIO_SERVER_AGENT_H_INCLUDED
#define RADIO_SERVER_AGENT_H_INCLUDED

#include <string>

#include "app/radio/RadioTetraAgent/src/RadioServerAgentCommandServant.h"

//Forward class declarations;

namespace TA_Base_Core
{
	class IEntityData;
}

namespace TA_IRS_App
{
//class RadioServerAgentSummary;

class RadioServerAgent
						
{
public:
    /**
	  * Constructor
      */
	RadioServerAgent();

    /**
	  * Destructor
      */
	virtual ~RadioServerAgent();

    //void setSummary(RadioServerAgentSummary *summary);

    void setCommandServant(RadioServerAgentCommandServant* servant);
    
	virtual void onTerminate();

    virtual void onMonitorMode();

    virtual void onControlMode();


private:
    /**
     * Disabled methods
     */
    RadioServerAgent(const RadioServerAgent&);
    void operator=(const RadioServerAgent&);


private:

    /**
	 *	Interface to rights library,runtime params, audit info & alarms;
	 */
    //RadioServerAgentSummary* m_pServerAgentSummary;
    
    RadioServerAgentCommandServant* m_commandServant;
    
    bool m_inControlMode;


};
}
#endif
