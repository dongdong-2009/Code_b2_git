
#ifndef __GENA_POWER_STATUS_DATA_STORE_H_INCLUDED__
#define __GENA_POWER_STATUS_DATA_STORE_H_INCLUDED__

#include "bus/ats/ats_agent/CommonATS/src/IPowerSectionStatusObserver.h"
#include "bus/protocol/Gena/src/IGENATopicDataStore.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{

	class PowerStatusDataStore : public TA_Base_Bus::IGENATopicDataStore,
									public TA_IRS_Bus::IPowerSectionStatusObserver
	{
	public:

		PowerStatusDataStore();
		~PowerStatusDataStore();

		/* 
		 * getAllData
		 * @purpose: get all available data 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: all the data with topic name equal to the parameter.
					empty list is returned if no data is available.
		 */
		virtual TA_Base_Bus::T_GENATopicDataList getAllData(const std::string& topicName );
		
		/* 
		 * synchronizeBegin
		 * @purpose: an interface called when we start synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeBegin(const std::string& topicName){/*do nothing here*/};

		/*  
		 * synchronizeEnd
		 * @purpose: an interface called when we receive an end synchronization notification 
		 * @param: topicName - name of the Gena topic.
		 *
		 * @return: none
		 */
		virtual void synchronizeEnd(const std::string& topicName){/*do nothing here*/};

		/* 
		 * onPowerStatusUpdate
		 * @purpose: update data store power status
		 * @param:	sectionID - power section ID.
		 *			powerStatus 1-Energized , 2-Deenergized, 3-Unknown Status
		 *
		 * @return: none
		 */
		virtual void onPowerStatusUpdate(const unsigned short& sectionID, const unsigned char& powerStatus);

		

	protected:
		/* 
		 * updateDataImpl
		 * @purpose: updates the data store and sends out notification to notify observers
		 * @param: topicName - name of the Gena topic.
		 * @param: pData - data containing the update
		 *
		 * @return: none
		 */
		virtual void updateDataImpl(const std::string& topicName, TA_Base_Bus::GENATopicDataPtr pData);

	private:
		/* 
		 * convertToGENATopicData
		 * @purpose:Convert to GENATopicData type
		 * @param:	sectionID - power section ID.
		 *			powerStatus 1-Energized , 2-Deenergized, 3-Unknown Status
		 *
		 * @return: none
		 */
		TA_Base_Bus::GENATopicDataPtr convertToGENATopicData(const unsigned short& sectionID, const unsigned char& powerStatus);

		TA_Base_Bus::GENATopicDataPtr convertToGENATopicData(const unsigned short& sectionID, const std::string& powerStatus);

		TA_IRS_Bus::PowerStatusMap m_powerStatusMap;

		TA_Base_Bus::T_GENATopicDataList m_GenaPowerDataList;

	};
}  //TA_IRS_App

#endif //__GENA_POWER_STATUS_DATA_STORE_H_INCLUDED__