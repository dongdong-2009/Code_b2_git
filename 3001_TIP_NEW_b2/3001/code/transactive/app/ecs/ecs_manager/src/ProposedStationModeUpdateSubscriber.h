/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ProposedStationModeUpdateSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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


#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"

#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/exceptions/src/ECSAgentExceptions.h"



namespace TA_Base_Core
{
    class AuditMessageSender;
}

namespace TA_IRS_App
{
	class MappedECSDataCache;
	class IProposedStationModeObserver;
}

namespace TA_IRS_App
{
	class ProposedStationModeUpdateSubscriber 
	: public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{

	public:
		ProposedStationModeUpdateSubscriber(TA_IRS_App::IProposedStationModeObserver* observer);
		virtual ~ProposedStationModeUpdateSubscriber();
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);

	private:
		ProposedStationModeUpdateSubscriber( const ProposedStationModeUpdateSubscriber& ProposedStationModeUpdateSubscriber);

		void filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes);

		//zn++ for logging received data
		void logReceivedProposedMode(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
				const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes);
		//++zn

		TA_Base_Core::NonReEntrantThreadLockable     m_lock;
		TA_Base_Core::AuditMessageSender*            m_auditSender;
		TA_IRS_App::IProposedStationModeObserver*   m_proposedStationModeObserver;
		std::auto_ptr< MappedECSDataCache > m_pMappedECSDataCache;

	};
}
#endif // !defined(ProposedMasterModeUpdateSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
