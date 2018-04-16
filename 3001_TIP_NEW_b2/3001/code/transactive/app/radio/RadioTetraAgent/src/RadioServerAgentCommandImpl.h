/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentCommandImpl.h $
 * @author:  Anggiono
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_SERVER_AGENT_COMMAND_IMPL_H_INCLUDED
#define RADIO_SERVER_AGENT_COMMAND_IMPL_H_INCLUDED


#include <string>
#include <vector>
#include "bus/radio/radio_bus/src/radiotypes.h"

#include "app/radio/RadioTetraAgent/src/IRadioServerAgentCommand.h"
#include "app/radio/RadioTetraAgent/src/RadioServerAgentSummary.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIEventType.h"

#include "core/message/src/CommsMessageSender.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class MessageType;
};
namespace TA_IRS_App
{
class RadioServerAgentCommandImpl : virtual public IRadioServerAgentCommand,
                                    virtual public TA_IRS_App::CADIEventObserver
{

public:
	RadioServerAgentCommandImpl(TA_Base_Core::IEntityDataPtr EntityData);
	virtual ~RadioServerAgentCommandImpl();

    void setSummary(RadioServerAgentSummary& summary);

    /**
     * initialize the various API's
     */
    bool initializeCADI();


    /**
     * clean up the API's
     */
    void cleanupCADI();

    /**
     * Used only by the MFT Agent to notify the Server Agent that the operator's
     * console has received a radio status message.
     * 
     * Internally, the Server Agent shall translate this into a Text Message and use
     * existing Text Message capabilities to store the message and raise a
     * notification to interested parties.
     */
	void radioStatusMsg(unsigned long sourceSSI, unsigned long statusMsg);

	void connectDynamicRegroup(RadioResource resource,unsigned long talkgroupID);

	void releaseDynamicRegroup(RadioResource resource,unsigned long talkgroupID);


    /**
	  * This function is inherited from CADIEventObserver::notify();
      * This method should be called when CADIEvent happened;
	  * It is called from within CADIEventSubject::notifyObservers();
      */
	virtual void notify( TA_IRS_App::CADIEvent &event );


    /**
      * This method should be called when CADIEvent happened;
	  * Has yet to know who will register RadioServerAgent;
	  * it seems this callback is hidden inside notify( TA_IRS_App::CADIEvent &event );
	  * and may not be defined & used here;
      */
	void CADIEventCallback(enum CADIEventType, void *);

private:
    // disabled methods
    RadioServerAgentCommandImpl(const RadioServerAgentCommandImpl&);
    void operator=(const RadioServerAgentCommandImpl&);


    void submitAlarm(const TA_Base_Core::MessageType& pAlarm);
    void closeAlarm(const TA_Base_Core::MessageType& pAlarm);


private:
	/**
	*	Used for safe acess;
	*/
	TA_Base_Core::ReEntrantThreadLockable m_lock;


    RadioServerAgentSummary* m_summary;

    bool m_isCADISessionOkay;



    //	To send notifications of incoming messages to Manager 
    TA_Base_Core::CommsMessageSender* m_commsSender;

    unsigned long m_entityKey;
    unsigned long m_entityLocation;
    unsigned long m_entitySubsystem;
    unsigned long m_entityTypeKey;
    std::string m_entityName;
	
	std::string m_session;
	
};
}
#endif
