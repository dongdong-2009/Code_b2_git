#pragma once
#include "IVideoDisplayManagerDAI.h"
#include <string>
namespace TA_IRS_App
{
    class VideoDisplayManagerDAIImpl : public IVideoDisplayManagerDAI
    {
        public:
            VideoDisplayManagerDAIImpl(void);
            virtual TA_Base_Core::IEntityDataList getSequences(unsigned long locationKey);
            virtual std::vector<TA_Base_Core::ILocation*> getAllLocations();
            TA_Base_Core::IEntityDataList getMonitors(unsigned long locationKey);
            virtual IEntityDataList getCameras(unsigned long locationKey);
            TA_Base_Core::ILocation*  getLocation(const std::string& sessionID) ;
            TA_Base_Core::IConsole*  getConsole(const std::string& sessionID);

        public:
            ~VideoDisplayManagerDAIImpl(void);
    };
}
