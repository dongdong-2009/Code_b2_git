#pragma once
#include "IVideoDisplayManagerDAI.h"
#include <string>
namespace TA_IRS_App
{
    class VideoDisplayManagerDAIImpl :
        public IVideoDisplayManagerDAI
    {
        public:
            VideoDisplayManagerDAIImpl(void);
            virtual TA_Base_Core::ILocation::ELocationType getLocationType(const std::string&  sessionID);
            virtual TA_Base_Core::IEntityDataList getSequences(unsigned long locationKey);
            virtual std::vector<TA_Base_Core::ILocation*> getAllLocations();
            TA_Base_Core::IEntityDataList getMonitors(unsigned long locationKey);

            TA_Base_Core::ILocation*  getLocation(const std::string& sessionID) ;

        public:
            ~VideoDisplayManagerDAIImpl(void);
    };
}
