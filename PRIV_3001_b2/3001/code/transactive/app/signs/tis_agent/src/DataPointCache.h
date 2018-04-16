#ifndef DATAPOINTCACHE_H
#define DATAPOINTCACHE_H

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include <map>

// DataPoint search string
#define LINKNORMALALARM_TYPE "LinkNormalAlarm"
#define OCC_SERVERSTATUS_ALARM_TYPE "diiSEVOCC-ServerNormalAlarm" // OCC only
#define STIS_CURRENTLIBRARY_VERSION_TYPE "SEV.aiiTISC-CurrentSTISLibraryVersion"
#define STIS_NEXTLIBRARY_VERSION_TYPE "SEV.aiiTISC-NextSTISLibraryVersion"
#define ISCS_STIS_CURRENTLIBRARY_VERSION_TYPE "ISCS.aiiTISC-CurrentSTISLibraryVersion"
#define ISCS_STIS_NEXTLIBRARY_VERSION_TYPE "ISCS.aiiTISC-NextSTISLibraryVersion"
#define ISCS_CDB_STIS_CURRENTLIBRARY_VERSION_TYPE "aiiTISC-CurrentSTISLibraryVersion-CDB"
#define ISCS_CDB_STIS_NEXTLIBRARY_VERSION_TYPE "aiiTISC-NextSTISLibraryVersion-CDB"
#define STATION_LIBRARIES_SYNCHRONISED_TYPE "diiTISC-StationLibrariesSynchronised"

#define TTIS_CURRENTLIBRARY_VERSION_TYPE "SEV.aiiTISC-CurrentTTISLibraryVersion"
#define TTIS_NEXTLIBRARY_VERSION_TYPE "SEV.aiiTISC-NextTTISLibraryVersion"
#define ISCS_TTIS_CURRENTLIBRARY_VERSION_TYPE "ISCS.aiiTISC-CurrentTTISLibraryVersion"
#define ISCS_TTIS_NEXTLIBRARY_VERSION_TYPE "ISCS.aiiTISC-NextTTISLibraryVersion"
#define ISCS_CDB_TTIS_CURRENTLIBRARY_VERSION_TYPE "aiiTISC-CurrentTTISLibraryVersion-CDB"
#define ISCS_CDB_TTIS_NEXTLIBRARY_VERSION_TYPE "aiiTISC-NextTTISLibraryVersion-CDB"
#define TRAIN_LIBRARIES_SYNCHRONISED_TYPE "diiTISC-TrainLibrariesSynchronised"

#define LCD_PID_NAME_CONTAIN_STR "LCD"
#define LED_PID_NAME_CONTAIN_STR "LED"
#define PID_STATUS_TYPE "Status"
#define PID_ON_TYPE "On"
#define PID_OFF_TYPE "Off"

namespace TA_IRS_App
{
	class DataPointCache
	{
	public:
		DataPointCache(void);
		~DataPointCache(void);
		TA_Base_Bus::DataPoint* getPidDataPoint(const std::string& firstLevelName, const int address, const std::string& pidType);
		TA_Base_Bus::DataPoint* getOccServerNormalAlarmDataPoint();
		TA_Base_Bus::DataPoint* getLinkNormalAlarmDataPoint(const std::string& firstLevelName);
        // Stis library version
		TA_Base_Bus::DataPoint* getIscsCdbCurrentStisLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsCdbNextStisLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsCurrentStisLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsNextStisLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getStisCurrentStisLibraryVersionDataPoint(const std::string& firstLevelName);
		TA_Base_Bus::DataPoint* getStisNextStisLibraryVersionDataPoint(const std::string& firstLevelName);
		TA_Base_Bus::DataPoint* getStisLibrarySynchronisedDatapoint();
        // Train library version
		TA_Base_Bus::DataPoint* getIscsCdbCurrentTrainLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsCdbNextTrainLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsCurrentTrainLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getIscsNextTrainLibraryVersionDatapoint();
		TA_Base_Bus::DataPoint* getStisCurrentTrainLibraryVersionDataPoint();
		TA_Base_Bus::DataPoint* getStisNextTrainLibraryVersionDataPoint();
		TA_Base_Bus::DataPoint* getTrainLibrarySynchronisedDatapoint();

		void addDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void setDataPointsToControlMode();
		void setDataPointsToMonitorMode();
	    void setDataPointsToBadQuality();
		// DataPoint name format is like "OCC_DT17.TIS.PLTBB.LCD02.eiiLCD-Status"
		// getDataPointFirstLevelName return the string "OCC_DT17"
		static std::string getDataPointFirstLevelName(TA_Base_Bus::DataPoint* dataPoint);
	private:
		void addLinkNormalAlarmDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addPidDataPoint(TA_Base_Bus::DataPoint* dataPoint);
        // Stis library version
		void addStisNextStisLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addStisCurrentStisLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addStisLibrarySynchronisedDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsNextStisLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCurrentStisLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCdbNextStisLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCdbCurrentStisLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
        // Train library version
		void addStisNextTrainLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addStisCurrentTrainLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addTrainLibrarySynchronisedDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsNextTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCurrentTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCdbNextTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);
		void addIscsCdbCurrentTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint);

		void addOccServerNormalAlarmDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void setPidDataPointsToBadQuality();
		void setLinkNormalAlarmDataPointsToBadQuality();
		void setOccServerNormalAlarmDataPointToBadQuality();
		static void setDataPointToBadQuality(TA_Base_Bus::DataPoint* dataPoint);
		//////////////////////////////////////////////////////////////////////////
		typedef std::map<std::string,TA_Base_Bus::DataPoint*> PidTypeMap; // Key is On, Off or Status 
		typedef std::map<int,PidTypeMap> PidAddressMap; // Key is pid address
		typedef std::map<std::string, PidAddressMap> PidFirstLevelNameMap; // Key is datapoint first level name
		PidFirstLevelNameMap m_pidDataPointMap;
		TA_Base_Bus::DataPoint* m_occServerNormalAlarmDatapoint;
		std::map<std::string,TA_Base_Bus::DataPoint*> m_linkNormalAlarmDataPointMap;
		TA_Base_Bus::DataPoint* m_iscsCdbCurrentStisLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsCdbNextStisLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsCurrentStisLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsNextStisLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_stisLibrarySynchronisedDataPoint;
		std::map<std::string, TA_Base_Bus::DataPoint*> m_stisCurrentStisLibraryVersionDataPointMap;
		std::map<std::string, TA_Base_Bus::DataPoint*> m_stisNextStisLibraryVersionDataPointMap;
		std::set<TA_Base_Bus::DataPoint*> m_allDataPoints;
		std::set<TA_Base_Bus::DataPoint*> m_linkNormalAlarmDataPoints;
		std::set<TA_Base_Bus::DataPoint*> m_pidDataPoints;
        // Train data point
		TA_Base_Bus::DataPoint* m_iscsCdbCurrentTrainLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsCdbNextTrainLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsCurrentTrainLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_iscsNextTrainLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_trainLibrarySynchronisedDataPoint;
		TA_Base_Bus::DataPoint* m_stisCurrentTrainLibraryVersionDataPoint;
		TA_Base_Bus::DataPoint* m_stisNextTrainLibraryVersionDataPoint;
	};
}

#endif