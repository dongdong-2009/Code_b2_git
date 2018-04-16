/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/scada/common_library/src/ScadaUtilities.h $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #1 $
  *
  * Last modified by:  $Author: lim.cy $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */


#if !defined(AFX_SCADAUTILITIES_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_)
#define AFX_SCADAUTILITIES_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>
#include <sys/timeb.h>
#include "bus/security/rights_library/src/RightsLibrary.h"

namespace TA_Base_Core
{
	struct CorbaName;
}

namespace TA_Base_Bus
{
	class MmsScadaUtility;
	class RightsLibrary;
	class IEntity;

	class ScadaUtilities
	{
	public:
	
		static ScadaUtilities& getInstance();
        static void removeInstance();

        bool isSetValueActionAllowed(unsigned long entityKey, unsigned long locationKey, unsigned long subSystemKey, std::string sessionId);

		//bool isActionAllowed(unsigned long action, unsigned long entityKey, std::string sessionId);

        bool isActionAllowedWithLocSub(unsigned long action, unsigned long entityKey, unsigned long locationKey, unsigned long subSystemKey, std::string sessionId);

		void registerLocalEntities(std::string agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap);

		/**
		* isDataPointMmsInhibited
		* @return false if datapoint, or subsystem that datapoint is in
		* is MMS Inhibited, or Global MMS inhibit is applied - else return true
		*
		* @throw TA_Base_Core::ScadaCommonException if can not read inhibit state for
		*		 global, datapoint or subsystem
		*/

		bool isDataPointMmsInhibited(std::string dataPointName);

		bool isDataPointMmsInhibited(unsigned long dataPointKey);

		bool isDataPointMmsInhibited(const TA_Base_Core::CorbaName& dataPointCorbaName);
        bool isMmsGlobalInhibited();

		struct MaintenanceData
		{
			//taken from the DATAPOINT_MMS_REPORT_TYPE param of the datapoint config
			enum MMSReportType {CPT, VAL};
			MMSReportType reportType;

			enum MMSReportPeroidType
			{
				HOURLY_BASIS = 1,
				DAILY_BASIS = 24
			};
			//taken from the DATAPOINT_MMS_REPORT_PERIOD para of the datapoint config
			MMSReportPeroidType reportPeroidType;

			//taken form the DATAPOINT_MMS_REPORT_TIME para of the datapoint config
			unsigned int reportPeriodTime;

			//the entity key
			unsigned long entityKey;

			//the entity type key
			unsigned long entityTypeKey;

			//the entity agent name
			std::string agentName;

			//the entity static type name
			std::string entityTypeName;

			//the asset name of the parent datanode 
			std::string assetName;

			//the portion of the entity name of the datapoint that follows the last '.' character
			std::string pointName;

			//the description field of the datapoint entity
			std::string dpDescription;
			
			//the description field of the datapoint's parent datanode entity
			std::string dnDescription;

			//indicates that the value could not be read and should not be considered valid
			bool readFailed;

			//a integer representation of the current datapoint value:
			// bool values should map to 0(false) 1(true)
			// decimal values are passed after engineering units applied.
			double	value;

			//DTL-1926 Add timeStamp of the current datapoint update time.
			//Liuy 2016-06-14
			timeb	timeStamp;
			
			MaintenanceData()
			{
			}

			MaintenanceData(const MaintenanceData& source)
				:
			reportType(source.reportType),
			reportPeroidType(source.reportPeroidType),
			reportPeriodTime(source.reportPeriodTime),
			entityKey(source.entityKey),
			entityTypeKey(source.entityTypeKey),
			agentName(source.agentName.c_str()),
			entityTypeName(source.entityTypeName.c_str()),
			assetName(source.assetName.c_str()),
			pointName(source.pointName.c_str()),
			dpDescription(source.dpDescription.c_str()),
			dnDescription(source.dnDescription.c_str()),
			readFailed(source.readFailed),
			value(source.value),
			timeStamp(source.timeStamp)
			{
			}

			MaintenanceData& operator=(const MaintenanceData& source)
			{
				if(this != &source)
				{
					reportType = source.reportType;
					reportPeroidType = source.reportPeroidType;
					reportPeriodTime = source.reportPeriodTime;
					entityKey = source.entityKey;
					entityTypeKey = source.entityTypeKey;
					agentName = source.agentName.c_str();
					entityTypeName = source.entityTypeName.c_str();
					assetName = source.assetName.c_str();
					pointName = source.pointName.c_str();
					dpDescription = source.dpDescription.c_str();
					dnDescription = source.dnDescription.c_str();
					readFailed = source.readFailed;
					value = source.value;
					timeStamp = source.timeStamp;
				}
				return *this;
			}
		};

		/**
        * getMaintenanceDataObject
        *
        * @return MaintenanceDataObject for all datapoints that:
        * 1) are in the speciified location(0, ignore location) and
        * 2) are configured with the MMS_REPORT_TYPE = "CPT" or "VAL"           
        * @param locationKey: the pkey of the location to retrieve the data points(0, ignore location)
        *
        * @throw TA_Base_Core::DatabaseException 
        * @throw TA_Base_Core::DataException
        */
		std::vector<ScadaUtilities::MaintenanceData*> getMaintenanceDataObject(unsigned long locationKey);
           
		/**
        * getMaintenanceDataValue
        *
        * @return :
        * 1) update dataObjects with latest value       
		* @param dataObjects: input and output, data points which need to retrieve latest value.
        * 
        */
		void getMaintenanceDataValue(std::vector<ScadaUtilities::MaintenanceData*>& dataObjects );
                


		std::string getAssetName(TA_Base_Core::CorbaName& dataPointCorbaName);
		std::string getAssetNameOfDataPoint(TA_Base_Core::CorbaName& dataPointCorbaName);
		std::string getAssetNameOfDataNode(TA_Base_Core::CorbaName& dataNodeCorbaName);
        
        //TD16871 
        //Mintao++
	    /**
	      * processOnlineUpdateOfMaintenanceData
	      * 
	      * <description>
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void processOnlineUpdateOfMaintenanceData();

	protected:
		ScadaUtilities();
		virtual ~ScadaUtilities();

	private:

		/**
		 * Local Entities
		 */
		std::string m_localAgentName;
		const std::map<unsigned long, TA_Base_Bus::IEntity*> *m_localEntityMap;
		RightsLibrary* m_rightsMgr;

		static TA_Base_Bus::ScadaUtilities* m_singleton;

        MmsScadaUtility* m_mmsScadaUtility;
	};
}
#endif // !defined(AFX_SCADAUTILITIES_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_)
