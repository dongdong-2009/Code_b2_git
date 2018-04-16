#ifndef RADIO_MFT_AGENT_ALIVE_HELPER_H_INCLUDED
#define RADIO_MFT_AGENT_ALIVE_HELPER_H_INCLUDED

#include "app/radio/radio_mft_agent/src/IMcdbRefreshCallback.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentRemoteApiEventImpl.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "bus/radio/Radio_Agent/idl/src/RadioServerAgentIDL.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_IRS_App;

class RadioMFTAgentAliveHelper : //public TA_Base_Bus::IEntityUpdateEventProcessor
								public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>{
public:
	RadioMFTAgentAliveHelper(RadioMFTAgentRemoteApiEventImpl* event);
	~RadioMFTAgentAliveHelper();

	void initialize();

	//
	// IEntityUpdateEventProcessor Interface implementation
	//
	//void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	//
	// SpecialisedMessageSubscriber Interface implementation
	//	
	void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	bool getRadioStatus();
	bool getIscsStatus();

private:
	//bool getDpValue();

	RadioMFTAgentRemoteApiEventImpl* m_pRadioEvent;
	TA_Base_Bus::DataPointProxySmartPtr m_dpProxy;
	//bool m_curIscsStatus;
	//bool m_curRadioStatus;

	TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
		TA_IRS_Bus::RadioServerAgentIDL_ptr,
		TA_IRS_Bus::RadioServerAgentIDL_var> m_radioAgent;
	
	// lock for accessing status
	TA_Base_Core::ReEntrantThreadLockable m_statusLock;

	static std::string RADIO_FALLBACK_DATAPOINT_POSTFIX;
};

#endif