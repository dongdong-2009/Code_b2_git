/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/AtsAgentSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#if !defined(AtsAgentSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define AtsAgentSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

/**
* Obtains the ATSTrainMessage ptr from the CORBA::Any.
* Converts the Fixed block IDs to ECS Zone IDs.  Only ECS Zones inside the
* station's ECS Zone Decision Scope are mapped.
* Adds the equivalent ATSTrainMessage_var references to the StationDecisionEngine
* and StationDataPointUpdater class maps.  The reference count mechanism is
* apparently thread-safe.
*/

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"

#pragma warning ( disable : 4250 )
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"
#pragma warning ( default : 4250 )

namespace TA_IRS_App
{
    class StationBroadcastProxy;

    class AtsAgentSubscriber 
    : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:
	    AtsAgentSubscriber(StationBroadcastProxy& broadcastManager);
        virtual ~AtsAgentSubscriber();
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);
        void subscribe();
        void unsubscribe();

    private:
        AtsAgentSubscriber( const AtsAgentSubscriber& theAtsAgentSubscriber);

        void processMessage( const TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef& message );

	    TA_Base_Core::NonReEntrantThreadLockable    m_lock;  // Mainly used to syncronise destruction
        StationBroadcastProxy&                      m_broadcastManager;
        unsigned long                               m_locationKey;
    };
} // namespace TA_IRS_App

#endif // !defined(AtsAgentSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)

