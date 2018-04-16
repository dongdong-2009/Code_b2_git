/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ProposedMasterModeUpdateSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(ProposedMasterModeUpdateSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define ProposedMasterModeUpdateSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

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
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"

namespace TA_Base_Core
{
    class AuditMessageSender;
}

namespace TA_IRS_App
{
	class MappedECSDataCache;
	class IProposedMasterModeObserver;
}

namespace TA_IRS_App
{
	class ProposedMasterModeUpdateSubscriber 
	: public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{

	public:
		ProposedMasterModeUpdateSubscriber(TA_IRS_App::IProposedMasterModeObserver* observer);
		virtual ~ProposedMasterModeUpdateSubscriber();
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);

	private:
		ProposedMasterModeUpdateSubscriber( const ProposedMasterModeUpdateSubscriber& ProposedMasterModeUpdateSubscriber);

		TA_Base_Core::NonReEntrantThreadLockable     m_lock;
		TA_Base_Core::AuditMessageSender*            m_auditSender;
		TA_IRS_App::IProposedMasterModeObserver*   m_proposedStationModeObserver;
	};
}

#endif // !defined(ProposedMasterModeUpdateSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
