/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * PACorbaMessageSubscriber bridges communications between various PA Manager components
  * and CORBA broadcasts - subscribes to any PA status name change notifications
  * Only a single model can attach to this subscriber at a time, and to ensure the client
  * model is kept up to date, must connect first, then update Status Name list (if we refresh
  * list before subscribing notifications, Status Name list may be out of sync)
  *
  */


#if !defined(PACORBAMESSAGESUBSCRIBER__INCLUDED_)
#define PACORBAMESSAGESUBSCRIBER__INCLUDED_

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"


class PACorbaMessageSubscriber : 
public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{

public:
    /**
     * PACorbaMessageSubscriber (constructor)
     * 
     */
	PACorbaMessageSubscriber( CWnd* pNotifyWnd );

	virtual ~PACorbaMessageSubscriber();

    /**
     * getMessageType (interface implementation)
     *
     * @see GenericMessageSubscriber::getMessageType for more details
     *
     */
    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);

protected:
    void subscribe();
    void unsubscribe();

    void processScheduleBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData& message );;

private:
    // Copy constructor not used so made private 
    PACorbaMessageSubscriber( const PACorbaMessageSubscriber& );
    // Assignment not used so it is made private
    PACorbaMessageSubscriber& operator=( const PACorbaMessageSubscriber& );
	PACorbaMessageSubscriber();

private:
    CWnd* m_pNotifyWnd;
};

#endif // !defined(PASigBroadcastSettingSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
