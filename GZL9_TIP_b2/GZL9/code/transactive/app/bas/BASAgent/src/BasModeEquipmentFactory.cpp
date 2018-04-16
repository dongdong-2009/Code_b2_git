/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasModeEquipmentFactory.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#include <string>

#include "app/bas/BASAgent/src/BasModeEquipmentFactory.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"

#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_App
{

    BasModeEquipmentFactory* BasModeEquipmentFactory::m_instance = NULL;
    BasModeEquipmentFactory::~BasModeEquipmentFactory()
    {
        FUNCTION_ENTRY("~BasModeEquipmentFactory");
        FUNCTION_EXIT;
    }

    BasModeEquipmentFactory& BasModeEquipmentFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");
        if (0 == m_instance)
        {
            m_instance = new BasModeEquipmentFactory();
        }
        FUNCTION_EXIT;
        return *m_instance;
    }

    TA_IRS_App::AllBasModeEquipments BasModeEquipmentFactory::getAllBasModeEquipments()
    {
        using namespace TA_Base_Core;

        FUNCTION_ENTRY("getAllBasModeEquipments");
        TA_IRS_App::AllBasModeEquipments ret;

        long locationKey = BasCachedConfig::getInstance().getLocationKey();
        std::string stationName = BasCachedConfig::getInstance().getStationName();

        TA_IRS_Core::AllLocationSubsystems allSubsystems;
        std::vector< TA_IRS_Core::SubSystem_SmartPtr > subsystems;
        if(TA_IRS_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(allSubsystems))
        {
            subsystems = allSubsystems[locationKey];
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found %d Subsystems for location %d, contain the system config items", subsystems.size(), locationKey);
            std::vector< TA_IRS_Core::SubSystem_SmartPtr >::iterator subIter = subsystems.begin();

            for (; subIter != subsystems.end(); ++subIter)
            {
                if (!(*subIter)->getAssetName().empty() && !(*subIter)->getIsMerge() && !(*subIter)->getIsSystem())
                {
                    ret[(*subIter)->getKey()] = new BasModeEquipment(stationName + (*subIter)->getAssetName());
                }
                else
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, 
                        "can not get mode subsystems Asset name from DB!, BA_SUBSYSTEM PKEY: %lu", (*subIter)->getKey());
                }
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "can not get All Bas Mode Equipments!");
            ret.clear();
        }


        FUNCTION_EXIT;
        return ret;
    }

    BasModeEquipmentFactory::BasModeEquipmentFactory()
    {
        FUNCTION_ENTRY("BasModeEquipmentFactory");
        FUNCTION_EXIT;
    }

};