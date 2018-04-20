/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andrew Del Carlo
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */

//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/common_library/src/MmsScadaUtility.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsGlobalInhibitCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	ScadaUtilities*				ScadaUtilities::m_singleton = NULL;
    TA_Base_Core::ReEntrantThreadLockable ScadaUtilities::m_singletonLock;
	
	ScadaUtilities::ScadaUtilities() 
		: m_localAgentName (""), m_localEntityMap (NULL),
          m_mmsScadaUtility(NULL)
	{
		TA_Base_Bus::RightsLibraryFactory rightsFactory;
        // We don't want to check rights for agent side, only on MFT side.
        // TODO: need to consider for Plans and check if other apps are uing scada utilities to check rights.
		m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
#if 0
		m_rightsMgr = rightsFactory.buildRightsLibrary();
#endif

        m_mmsScadaUtility = new MmsScadaUtility("", NULL, false);
	}

	ScadaUtilities::~ScadaUtilities()
	{
		if (m_rightsMgr != NULL)
		{
			delete m_rightsMgr;
			m_rightsMgr = NULL;
		}

        if (m_mmsScadaUtility != NULL)
        {
            delete m_mmsScadaUtility;
            m_mmsScadaUtility = NULL;
        }
	}

	ScadaUtilities& ScadaUtilities::getInstance()
	{
        if ( m_singleton == NULL )
        {
        	TA_THREADGUARD( m_singletonLock );
			if ( m_singleton == NULL )
			{
           		m_singleton = new ScadaUtilities();
			}
        }
        // increment reference counte
        return *m_singleton;

	}

    void ScadaUtilities::removeInstance()
    {
    	if(m_singleton == NULL)
    	{
    		return;
    	}

        TA_THREADGUARD( m_singletonLock );
         if ( m_singleton != NULL )
         {
               delete m_singleton;
               m_singleton = NULL;
         }
    }

	bool ScadaUtilities::isSetValueActionAllowed(unsigned long entityKey, unsigned long locationKey, unsigned long subSystemKey, std::string sessionId)
	{
		return isActionAllowedWithLocSub(TA_Base_Bus::aca_DATA_POINT_WRITE, entityKey, locationKey, subSystemKey, sessionId);
	}

	/*bool ScadaUtilities::isActionAllowed(unsigned long action, unsigned long entityKey, std::string sessionId)
	{
		bool allowed = false;

		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;

		try
		{
			if (m_rightsMgr != NULL)
			{
				allowed = m_rightsMgr->isActionPermittedOnResource(
										sessionId, 
										entityKey, 
										action, 
										reason, 
										decisionModule); 
			}
		}
		catch (...)
		{
		}

		return allowed;
	}*/
	
    bool ScadaUtilities::isActionAllowedWithLocSub(unsigned long action, unsigned long entityKey, unsigned long locationKey, unsigned long subSystemKey, std::string sessionId)
    {
        bool allowed = false;
        
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;
        
        try
        {
            if (m_rightsMgr != NULL)
            {
                allowed = m_rightsMgr->isActionPermittedOnResourceAndLocSub(
                    sessionId, 
                    entityKey, 
                    locationKey,
                    subSystemKey,
                    action, 
                    reason, 
                    decisionModule); 
            }
        }
        catch (...)
        {
        }
	
        
        return allowed;
    }
    
	void ScadaUtilities::registerLocalEntities(std::string agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap)
	{
		m_localAgentName = agentName;
		m_localEntityMap = entityMap;

        m_mmsScadaUtility->registerLocalEntities(agentName, entityMap);
	}

	bool ScadaUtilities::isDataPointMmsInhibited(std::string dataPointName)
	{
		return m_mmsScadaUtility->isDataPointMmsInhibited(dataPointName);
	}


	bool ScadaUtilities::isDataPointMmsInhibited(unsigned long dataPointKey)
	{	
		return m_mmsScadaUtility->isDataPointMmsInhibited(dataPointKey);
	}

    bool ScadaUtilities::isMmsGlobalInhibited()
    {
        return m_mmsScadaUtility->isMmsGlobalInhibited();
    }

	std::vector<ScadaUtilities::MaintenanceData> ScadaUtilities::getMaintenanceData(unsigned long subsystemKey,
		const std::vector<unsigned int>& periods)
	{
		return m_mmsScadaUtility->getMaintenanceData(subsystemKey, periods);
	}

    //TD16871 
    //Mintao++
    void ScadaUtilities::processOnlineUpdateOfMaintenanceData()
    {
        m_mmsScadaUtility->processOnlineUpdateOfMaintenanceData();
    }
    //TD16871 
    //Mintao++
    
	std::string ScadaUtilities::getAssetName(TA_Base_Core::CorbaName& corbaName)
	{
		std::string type = 
			TA_Base_Core::EntityAccessFactory::getInstance().getTypeOf(corbaName.getObjectName());

		if (type == TA_Base_Core::DataPointEntityData::getStaticType())
		{
			return getAssetNameOfDataPoint(corbaName);
		}
		else if (type == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
			return getAssetNameOfDataPoint(corbaName);
		}

		return "";
	}

	std::string ScadaUtilities::getAssetNameOfDataPoint(TA_Base_Core::CorbaName& dataPointCorbaName)
	{
		TA_Base_Core::CorbaName dnCorbaName = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfParent(dataPointCorbaName.getObjectName());

		return getAssetNameOfDataNode(dnCorbaName);
	}

	std::string ScadaUtilities::getAssetNameOfDataNode(TA_Base_Core::CorbaName& dataNodeCorbaName)
	{
		TA_Base_Core::IData* assetPar = NULL;

		try
		{
			assetPar = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
						dataNodeCorbaName.getObjectName(), 
						TA_Base_Core::DataNodeEntityData::DATANODE_ASSET_NAME );
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "getAssetNameOfDataNode() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "getAssetNameOfDataNode() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
		}

		std::string assetName = "";

		if (assetPar != NULL)
		{
			try
			{
				assetName = assetPar->getStringData(0, "VALUE");
			}
			catch (TA_Base_Core::DataException& e)
			{
				std::string text = "getAssetNameOfDataNode() - " + std::string(e.what());
 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
			}

			delete assetPar;
		}

		return assetName;
	}
}


