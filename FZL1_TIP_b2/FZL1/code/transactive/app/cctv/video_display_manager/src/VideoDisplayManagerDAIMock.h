#pragma once
#include "IVideoDisplayManagerDAI.h"

namespace TA_IRS_App
{
    class VideoDisplayManagerDAIMock:
        public IVideoDisplayManagerDAI
    {
        public:
            VideoDisplayManagerDAIMock(void);
            TA_Base_Core::ILocation::ELocationType getLocationType(const std::string&  sessionID);
            TA_Base_Core::IEntityDataList getSequences(unsigned long locationKey);
            std::vector<TA_Base_Core::ILocation*> getAllLocations();
            TA_Base_Core::IEntityDataList getMonitors(unsigned long locationKey);
            TA_Base_Core::ILocation*  getLocation(const std::string& sessionID)
            {
                return NULL;
            }
        public:
            ~VideoDisplayManagerDAIMock(void);
    };

}