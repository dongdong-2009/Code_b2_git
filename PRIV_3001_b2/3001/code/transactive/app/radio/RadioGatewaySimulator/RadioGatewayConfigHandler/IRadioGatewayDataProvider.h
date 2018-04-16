


#pragma once

#include "RadioCommonTypes.h"

class IRadioGatewayDataProvider
{
	public:

		~IRadioGatewayDataProvider () 
		{

		}

		virtual RadioSubscriberData getRadioSubscriberData (unsigned long resourceId) = 0;
 		virtual ProfileTalkGroupListType getTalkGroupsFromProfile (std::string strProfile) = 0;
		virtual ProfileDBUpdateCollectionType getProfileDBUpdate () = 0;
		virtual unsigned long getRadioSystemStatus () = 0;
		virtual unsigned long getDGNARequestStatus () = 0;
		virtual RadioCallForwardingQueryData getQueryCallForwardingData () = 0;
};