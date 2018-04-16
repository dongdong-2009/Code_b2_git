/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/AtsAgentEntityData.h $
  * @author Ross Tucker
  * @version $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/12/23 17:32:14 $
  * Last modified by: $Author: peter.wong $
  * 
  * Implement the IEntityData interface.
  */

#ifndef ATS_AGENT_ENTITY_DATA
#define ATS_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class AtsAgentEntityData : public EntityData
    {

    public:
        AtsAgentEntityData();
        AtsAgentEntityData( unsigned long key );
        virtual ~AtsAgentEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

		enum EAtsLocationType
		{
			Occ,
			Depot,
			MainStation,
			SecondaryStation1,
			SecondaryStation2,
			Unknown
		};
			
		virtual std::string getAtsServer1IpAddress();
		virtual std::string getAtsServer1ServicePort();
		virtual std::string getAtsServer2IpAddress();
		virtual std::string getAtsServer2ServicePort();
		virtual std::string getAssetName();
		virtual EAtsLocationType getAgentAtsLocationType();
		virtual unsigned long getAtsTablesBaseAddress();
		virtual unsigned long getIscsTablesBaseAddress();
		virtual unsigned long getMaxCommsRetries();
		virtual unsigned long getMaxWatchdogInterval();
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table	
		virtual long getOa1Timeout();
		virtual long getAtcTimeout();
		//++zhou yuan
		virtual long getConnectionFailedTimeout();

    protected:

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        AtsAgentEntityData( const AtsAgentEntityData& theAtsPsdEntityData) {};            
		AtsAgentEntityData& operator=(const AtsAgentEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ATS1_IP_ADDRESS;
		static const std::string ATS1_SERVICE_PORT;
		static const std::string ATS2_IP_ADDRESS;
		static const std::string ATS2_SERVICE_PORT;
		static const std::string AGENT_ATS_LOCATION_TYPE;
		static const std::string ATS_TABLES_BASE_ADDRESS;
		static const std::string ISCS_TABLES_BASE_ADDRESS;
		static const std::string MAX_COMMS_RETRIES;
		static const std::string MAX_WATCHDOG_INTERVAL;

		static const std::string ATS_LOC_TYPE_OCC;
		static const std::string ATS_LOC_TYPE_DEPOT;
		static const std::string ATS_LOC_TYPE_MAIN_STATION;		
		static const std::string ATS_LOC_TYPE_SECONDARY_STATION_1;
		static const std::string ATS_LOC_TYPE_SECONDARY_STATION_2;

		static const std::string OA1_TIMEOUT;
		static const std::string ATC_TIMEOUT;
		static const std::string CONNECTION_FAILED_TIMEOUT;
		static const std::string DEFAULT_VALUE;

		static const std::string ASSET_NAME;

        bool m_isValidData;

		std::string m_atsServer1IpAddress;
        std::string m_atsServer1ServicePort;
		std::string m_atsServer2IpAddress;
		std::string m_atsServer2ServicePort;
		std::string m_assetName;
		EAtsLocationType m_agentAtsLocationType;
		unsigned long m_atsTablesBaseAddress;
		unsigned long m_iscsTablesBaseAddress;
		unsigned long m_maxCommsRetries;
		unsigned long m_maxWatchdogInterval;
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table	
		long		 m_oa1Timeout;
		long		 m_atcTimeout;
		//++zhou yuan
		long         m_connectionFailedTimeout;
    };

    typedef boost::shared_ptr<AtsAgentEntityData> AtsAgentEntityDataPtr;
}

#endif // ATS_AGENT_ENTITY_DATA
