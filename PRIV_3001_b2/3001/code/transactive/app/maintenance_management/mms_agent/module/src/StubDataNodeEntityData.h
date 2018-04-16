/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubDataNodeEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBDATANODEENTITYDATA_H_
#define STUBDATANODEENTITYDATA_H_

#include <iostream>
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

namespace TA_IRS_App_Test
{
	class StubDataNodeEntityData : public TA_Base_Core::DataNodeEntityData
	{
		public :
			StubDataNodeEntityData()
            : m_assetName("AssetName"),
              m_label ("Label"),
              m_shortLabel("ShortLabel")
			{
                std::cout << "StubDataNodeEntityData" <<  std::endl;
			}
			
			virtual ~StubDataNodeEntityData()
			{
                std::cout << "~StubDataNodeEntityData" <<  std::endl;
			}
			
            unsigned long getKey()
            {
                std::cout << "StubDataNodeEntityData::getKey" <<  std::endl;
                return 1;
            }
            
			std::string getName()
			{
                std::cout << "StubDataNodeEntityData::getName" <<  std::endl;
				return "OccMmsAgent";
			}
			
			std::string getDescription()
			{
                std::cout << "StubDataNodeEntityData::getDescription" <<  std::endl;
				return "OccMmsAgent";
			}
			
			unsigned long getSubsystem()
			{
                std::cout << "StubDataNodeEntityData::getSubsystem" <<  std::endl;
				return 1;
			}
			
			unsigned long getLocation()
			{
                std::cout << "StubDataNodeEntityData::getLocation" <<  std::endl;
				return 1;
			}
			
			const std::string & getAssetName()
			{
                std::cout << "StubDataNodeEntityData::getAssetName" <<  std::endl;
				return m_assetName;
			}

			const std::string & getLabel()
			{
                std::cout << "StubDataNodeEntityData::getLabel" <<  std::endl;
				return m_label;
			}

			const std::string & getShortLabel()
			{
                std::cout << "StubDataNodeEntityData::getShortLabel" <<  std::endl;
				return m_shortLabel;
			}
			
			int getDisplayOrder()
			{
                std::cout << "StubDataNodeEntityData::getDisplayOrder" <<  std::endl;
				return 1;
			}

			TA_Base_Core::SharedIEntityDataList getChildDataPoints()
			{
                std::cout << "StubDataNodeEntityData::getChildDataPoints" <<  std::endl;
				TA_Base_Core::SharedIEntityDataList list;
				return list;
			}

			TA_Base_Core::SharedIEntityDataList getChildDataNodes()
			{
                std::cout << "StubDataNodeEntityData::getChildDataNodes" <<  std::endl;
				TA_Base_Core::SharedIEntityDataList list;
				return list;
			}
            
        private:
            std::string m_assetName;
            std::string m_label;
            std::string m_shortLabel;
	};
}
#endif /*STUBDATANODEENTITYDATA_H_*/
