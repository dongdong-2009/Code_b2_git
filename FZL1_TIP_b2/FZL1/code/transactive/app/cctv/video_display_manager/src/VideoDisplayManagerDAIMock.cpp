#include "VideoDisplayManagerDAIMock.h"

namespace TA_IRS_App
{
    VideoDisplayManagerDAIMock::VideoDisplayManagerDAIMock(void)
    {
    }

    VideoDisplayManagerDAIMock::~VideoDisplayManagerDAIMock(void)
    {
    }

    TA_Base_Core::ILocation::ELocationType VideoDisplayManagerDAIMock::getLocationType(const std::string& sessionID)
    {
        return TA_Base_Core::ILocation::OCC;
    }

    std::vector<TA_Base_Core::ILocation*> VideoDisplayManagerDAIMock:: getAllLocations()
    {
        std::vector<TA_Base_Core::ILocation*> locations;
        return locations;
    }

    TA_Base_Core::IEntityDataList VideoDisplayManagerDAIMock::getMonitors(unsigned long locationKey)
    {
        TA_Base_Core::IEntityDataList monitors;
        return monitors;
    }

    TA_Base_Core::IEntityDataList VideoDisplayManagerDAIMock::getSequences(unsigned long locationKey)
    {
        TA_Base_Core::IEntityDataList squences ;
        return squences ;
    }
}



