/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andrew Del Carlo
  * @version: $Revision$
  *
  * Last modified by:  $Author$
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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

	class ScadaUtilities{
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
			//taken from the MMS_REPORT_TYPE param of the datapoint config
			enum MessageType {CPT, VAL};
			MessageType type;

			//the asset name of the parent datanode 
			std::string assetName;

			//the portion of the entity name of the datapoint that follows the last '.' character
			std::string pointName;

			//the description field of the datapoint entity
			std::string description;

			//indicates that the value could not be read and should not be considered valid
			bool readFailed;

			//a integer representation of the current datapoint value:
			// bool values should map to 0(false) 1(true)
			// decimal values are passed after engineering units applied.
			double value;

		};

		/**
		* getMaintenanceData
		*
		* @return MaintenanceData for all datapoints that:
		* 1) are in the speciified physical subsystem, and
		* 2) are configured with the MMS_REPORT_TYPE = "CPT" or "VAL", and
		* 3) are configured with MMS_REPORT_PERIOD = one of the values in the "periods" param
		*
		* @param subsystemKey: the pkey of the physical subsystem to retrieve the data points
		* from
		*
		* @param periods: a vector of uints to be compared to the MMS_REPORT_PERIOD
		*
		* @throw TA_Base_Core::BadParameterException if param dataPointKey is not
		* the pkey of a configured entity of type datapoint
		*/
		std::vector<MaintenanceData> getMaintenanceData(unsigned long subsystemKey,
			const std::vector<unsigned int>& periods);

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
        static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;

        MmsScadaUtility* m_mmsScadaUtility;
	};
}
#endif // !defined(AFX_SCADAUTILITIES_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_)
