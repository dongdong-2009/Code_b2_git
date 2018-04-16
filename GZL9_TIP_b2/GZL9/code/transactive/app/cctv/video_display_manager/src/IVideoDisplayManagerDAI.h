#pragma once

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include <string>


using TA_Base_Core::IEntityDataList;

namespace TA_IRS_App
{
    class IVideoDisplayManagerDAI
    {
        public:
            IVideoDisplayManagerDAI(void)
            {
            }
            ~IVideoDisplayManagerDAI(void)
            {
            }
            virtual std::vector<TA_Base_Core::ILocation*> getAllLocations() = 0;
            virtual IEntityDataList getMonitors(unsigned long locationKey) = 0;
            virtual IEntityDataList getSequences(unsigned long locationKey) = 0;
            virtual IEntityDataList getCameras(unsigned long locationKey) = 0;
            virtual TA_Base_Core::ILocation*  getLocation(const std::string& sessionID) = 0;
            virtual TA_Base_Core::IConsole*  getConsole(const std::string& sessionID) = 0;
    };
}
