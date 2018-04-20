/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/common_library/src/MmsScadaUtility.h $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/25 18:07:02 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */


#if !defined(AFX_MmsScadaUtility_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_)
#define AFX_MmsScadaUtility_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4503)
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_Base_Bus
{
	class RightsLibrary;
	class IEntity;
    class IEntityDataList;
    class ScadaProxyFactory;

	class MmsScadaUtility : public virtual IEntityUpdateEventProcessor
	{
	public:
	
		MmsScadaUtility();
		MmsScadaUtility(std::string agentName, 
						const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap,
						bool useProxies = true);
		virtual ~MmsScadaUtility();

        //TD16871 
        //Mintao++
        /**
         * processOnlineUpdateOfMaintenanceData
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        void processOnlineUpdateOfMaintenanceData();
        
        void registerLocalEntities(const std::string& agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap);

		/**
		* isDataPointMmsInhibited
		* @return false if datapoint, or subsystem that datapoint is in
		* is MMS Inhibited, or Global MMS inhibit is applied - else return true
		*
		* @throw TA_Base_Core::ScadaCommonException if can not read inhibit state for
		*		 global, datapoint or subsystem
		*/

		bool isDataPointMmsInhibited(unsigned long dataPointKey);

        bool isDataPointMmsInhibited(unsigned long dataPointKey, TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess);
		bool isDataPointMmsInhibited(const std::string& dataPointName);

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
		std::vector<ScadaUtilities::MaintenanceData> getMaintenanceData(unsigned long subsystemKey,
			const std::vector<unsigned int>& periods);


	/**
	  * processEntityUpdateEvent
	  * 
	  * process the entity update event
	  * 
	  * @return virtual void 
	  * @param : unsigned long entityKey
	  * @param : ScadaEntityUpdateType updateType
	  * 
	  * @exception <exceptions> Optional
	  */
		virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);

		bool isMmsGlobalInhibited();

        bool isMmsSubsystemInhibited(unsigned long subsystemkey);

	protected:
		void freeSubSystemNodes();

        bool isMmsDpInhibited(unsigned long dataPointKey, const std::string& agentName);
        
		
		//std::string getMmsReportType(const std::string& objectName);	// CL-21212 reduce DB access
		//int getMmsReportPeriod(const std::string& objectName);		// CL-21212	reduce DB access

		ScadaUtilities::MaintenanceData getMmsMaintData (TA_Base_Core::IEntityData* entitydata, const std::string& reportType);

	private:

        // Mintao++(TD13650)
        void initScadaProxyFactory();
        // Mintao++(TD13650)
		/**
		 * Local Entities
		 */
		std::string m_localAgentName;
		const std::map<unsigned long, TA_Base_Bus::IEntity*> *m_localEntityMap;
		bool m_useProxies;
        bool m_globalInhibitDpExist;
        bool m_globalInhibitProxyCreated;
		DataPointProxySmartPtr m_globalInhibitProxy;
        TA_Base_Core::ReEntrantThreadLockable m_globalInhibitLock;
		TA_Base_Core::ReEntrantThreadLockable m_subsystemNodesLock;
		std::map<unsigned long, DataNodeProxySmartPtr*> m_subsystemNodes;

        /**
         * Cache map to reduce number of database calls                                                  
         */ 
        //std::map<std::string, std::string> m_reportTypeMap;	// CL-21212 this buffer is useless
        //std::map<std::string, int> m_reportPeriodMap;		// CL-21212 this buffer is useless
        std::map<unsigned long, std::map<std::string, TA_Base_Core::IEntityDataList> > m_physicalSubsystemEntitiesMap;
        
	    /**
	     * Synchronize access to the cache map
	     *
	     */
	    TA_Base_Core::ReEntrantThreadLockable m_mapLock;

	    /**
	     * cache the scada proxy factory
	     *
	     */
		TA_Base_Bus::ScadaProxyFactory*			m_proxyFactory;				// Mintao++(TD13650)

    };
}
#endif // !defined(AFX_MmsScadaUtility_H__C39208D3_EB6A_460A_AFA4_F659A6180B35__INCLUDED_)
