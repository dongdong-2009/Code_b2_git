/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/DataPointHelper.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * Helper for managing Datapoints
  */
#ifndef DATAPOINTHELPER_H
#define DATAPOINTHELPER_H

#include <string>
#include <map>

#include "core/message/types/MessageTypes.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsTwpEntityData.h"  
#include "core/data_access_interface/entity_access/src/AtsPsdEntityData.h"  

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/generic_agent/src/IEntity.h"

namespace TA_Base_Core
{
	class AlarmHelper;
};

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
};

namespace TA_IRS_App
{
    class DataPointHelper : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
    public:
		
		DataPointHelper (TA_Base_Core::AtsAgentEntityDataPtr entityData);
		virtual ~DataPointHelper();

		void			addInputBooleanDataPoint(unsigned long entityKey);
		void			addInputTextDataPoint(unsigned long entityKey);
		void			addOutputBooleanDataPoint(unsigned long entityKey, const TA_Base_Core::MessageType& alarmType);
		void			addOutputBooleanDataPoint(unsigned long entityKey, std::string alarmTypeKey = "");
		bool			isDataPointAdded(unsigned long entityKey);
		bool			getBooleanDataPointValue(unsigned long entityKey);
		std::vector<unsigned char>			
						getCovertedBcdTextDataPointValue(unsigned long entityKey);
		bool			isBooleanDataPointQualityGood(unsigned long entityKey);
		bool			isTextDataPointQualityGood(unsigned long entityKey);
		bool			setBooleanDataPointValue(unsigned long entityKey, bool value);

		void			setAlarmState(std::string alarmTypeKey, bool isInAlarm);
		bool			isInAlarm(std::string alarmTypeKey);

		void			updateIsInControlMode( bool isControlMode ); //wenching++ (TD14526)

		std::string     getTWPDataPointCorbaName(unsigned long entityKey); //changxing++ (TD15264)
		//
		// IEntityUpdateEventProcessor Interface implementation
		//
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
		void  setAtsTwpEntityData(TA_Base_Core::AtsTwpEntityData* atstwpentitydata);//(TD15213)
		void  setAtsPsdEntityData(TA_Base_Core::AtsPsdEntityData* atspsdentitydata);//(TD15213)

	protected:

		// Forward declaration
		struct DataPointDetail;
		class AlarmDetail;

		void		createDataPointProxy(unsigned long entityKey, TA_Base_Bus::DataPointProxySmartPtr &);

		void 								addDataPointDetail(unsigned long entityKey,
																DataPointDetail *dpDetail,
																std::string alarmTypeKey);
		DataPointDetail&					getDataPointDetail(unsigned long entityKey);

		void								addAlarmDetail(const TA_Base_Core::MessageType& alarmType);
		AlarmDetail&						getAlarmDetail(std::string alarmTypeKey);

		std::string							getDataPointType(std::string objectName);
		std::string							getDataPointWriteable(std::string objectName);

	private:
		// Disable default constructor, copy constructor and assignment operator
		DataPointHelper();
        DataPointHelper( const DataPointHelper& dataPointHelper);
        DataPointHelper& operator=(const DataPointHelper&);

	protected:

		struct DataPointDetail
		{
			TA_Base_Bus::DataPointProxySmartPtr	dataPointProxy;
			std::string						alarmTypeKey;
			bool							proxyAvailable;
		};

		class AlarmDetail
		{
		public:
			AlarmDetail(const TA_Base_Core::MessageType& type)
				: alarmType (type) {};

			const TA_Base_Core::MessageType&		alarmType;
			bool							alarmGenerated;
		};

		typedef std::map<unsigned long, DataPointDetail*>	DataPointDetailMap;
		typedef DataPointDetailMap::iterator				DataPointDetailMapIt;

		typedef std::map<std::string, AlarmDetail>			AlarmDetailMap;
		typedef AlarmDetailMap::iterator					AlarmDetailMapIt;

		DataPointDetailMap					m_dataPointDetailMap;
		AlarmDetailMap						m_alarmDetailMap;

		//TD18093, jianghp
		//TA_Base_Core::AlarmHelper&				m_alarmHelper;
		//TD18093, jianghp
		TA_Base_Core::AtsAgentEntityDataPtr		m_entityData;

		bool									m_isInControlMode; //wenching++ (TD14526)
		TA_Base_Core::AtsTwpEntityData*   m_atstwpentitydata;   //TD15213 
		TA_Base_Core::AtsPsdEntityData*   m_atspsdentitydata;   //TD15213 
        TA_Base_Bus::ScadaProxyFactory*   m_proxyFactoryPtr;
	};
}

#endif 
