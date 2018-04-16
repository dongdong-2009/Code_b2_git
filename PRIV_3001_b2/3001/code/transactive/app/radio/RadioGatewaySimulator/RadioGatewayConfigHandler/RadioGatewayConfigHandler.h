#pragma once

#include "IRadioGatewayDataProvider.h"
#include "IRadioOperationStrategy.h"
#include <map>
#include "RadioStratFactory.h"

typedef std::map <EStratType, IRadioOperationStrategy * > StrategyCollectionType;

class CRadioGatewayConfigHandler : public IRadioGatewayDataProvider
{
	public:
		CRadioGatewayConfigHandler(void);
		~CRadioGatewayConfigHandler(void);

		virtual RadioSubscriberData getRadioSubscriberData (unsigned long resourceId);
		virtual ProfileTalkGroupListType getTalkGroupsFromProfile (std::string strProfile);
		virtual ProfileDBUpdateCollectionType getProfileDBUpdate ();
		virtual unsigned long getRadioSystemStatus ();
		virtual unsigned long getDGNARequestStatus ();
		virtual RadioCallForwardingQueryData getQueryCallForwardingData ();

	private:
		
		StrategyCollectionType m_mapStrategyCollection;
		RadioStratFactory * m_radioStrategyFactory;
};
