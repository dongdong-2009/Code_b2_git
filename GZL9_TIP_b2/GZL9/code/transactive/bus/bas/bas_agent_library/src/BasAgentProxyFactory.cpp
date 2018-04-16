/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/GZL9/transactive/bus/bas/bas_agent_library/src/BasAgentProxyFactory.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/02/27 17:12:34 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the bas agent.
  */
#include "bus/bas/bas_agent_library/src/BasAgentProxyFactory.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"

namespace TA_IRS_Bus
{
    TA_IRS_Bus::BasAgentProxySmartPtr& BasAgentProxyFactory::getBasAgentProxyByLocationKey( long locationKey )
    {
        FUNCTION_ENTRY("BasAgentProxyFactory::getBasAgentProxyByLocationKey");

        BasAgentProxyMapIterator it;
        {
            TA_THREADGUARD(m_agentMapLock);
            it = m_basAgentProxyMap.find(locationKey);
            if (it != m_basAgentProxyMap.end())
            {
                return it->second;
            }
        }
        
        std::string strAgentEntityName = getAgentEntityName( locationKey );
        TA_IRS_Bus::BasAgentProxySmartPtr agentProxyPtr(new BasAgentProxy(strAgentEntityName, true));
        if (NULL != agentProxyPtr.get())
        {
            TA_THREADGUARD(m_agentMapLock);
            m_basAgentProxyMap[locationKey] = agentProxyPtr;
        } 
        else
        {
            std::string strError = "can not get the BASAgent with entityname:";
            strError += strAgentEntityName;
            TA_THROW(BasNotDeployStationException(strError));
        }

        FUNCTION_EXIT;
        return m_basAgentProxyMap[locationKey];


    }

    std::string BasAgentProxyFactory::getAgentEntityName( unsigned long locationKey )
    {
        FUNCTION_ENTRY("getAgentEntityName");

        TA_Base_Core::IEntityDataList agentList;
        getEntityDataList( locationKey, false, agentList);
        std::string strEntityName = "";
        if (agentList.size() > 0)
        {
            strEntityName = agentList[0]->getName();
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not find the Agent Entity of locatoin : %d", locationKey);
        }

        // delete the data avoid the memory leak
        TA_Base_Core::IEntityDataList::iterator it;
        for (it = agentList.begin(); it != agentList.end(); it++)
        {
            delete (*it);
            (*it) = NULL;
        }

        if (strEntityName.empty())
        {
            TA_THROW(BasNotDeployStationException("Can not found the Agent Entity Data"));
        }
        FUNCTION_EXIT;
        return strEntityName;
    }

    void BasAgentProxyFactory::getEntityDataList( unsigned long locationKey, bool loadParameter, TA_Base_Core::IEntityDataList& dataList )
    {
        FUNCTION_ENTRY("getEntityDataList");
        using namespace TA_Base_Core;
        // get the location key and stationBasAgent type name
        // prepare to get the StationBASAgent entity name
        //unsigned long locationKey = TimeTableManagerUtility::stringToUnsignedLong(RunParams::getInstance().get(RPARAM_LOCATIONKEY));
        std::string strStationBasAgentTypeName = TA_Base_Core::StationBASAgentEntityData::getStaticType();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
            "create BasAgent using locationKey=%d, BasAgentTypeName=%s",
            locationKey, strStationBasAgentTypeName.c_str());

        // get the StationBASAgent entityName
        dataList =	TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(strStationBasAgentTypeName, locationKey, loadParameter);
        FUNCTION_EXIT;
    }

    BasAgentProxyFactory::BasAgentProxyFactory()
    {
        FUNCTION_ENTRY("BasAgentProxyFactory");
        FUNCTION_EXIT;
    }

    BasAgentProxyFactory::~BasAgentProxyFactory()
    {
        FUNCTION_ENTRY("~BasAgentProxyFactory");
        FUNCTION_EXIT;
    }

}