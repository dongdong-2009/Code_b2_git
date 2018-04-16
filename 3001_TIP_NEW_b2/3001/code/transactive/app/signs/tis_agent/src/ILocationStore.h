#ifndef TIS_AGENT_ILOCTIONSTORE_H_INCLUDED
#define TIS_AGENT_ILOCTIONSTORE_H_INCLUDED
/**
  *This class is used to fix CL-21314,reduce database access
  */
#include "core/data_access_interface/src/ILocation.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <map>

namespace TA_IRS_App
{
    class ILocationStore
    {
        public:
            ~ILocationStore();
            static ILocationStore* getInstance();
            TA_Base_Core::ILocation* getLocationByKey(unsigned long locationKey);
            TA_Base_Core::ILocation* getLocationByName(const std::string& locationName);
        private:
            ILocationStore();
            static ILocationStore* m_theClassInstance;
            static TA_Base_Core::ReEntrantThreadLockable m_ilocationMapAccessLock;
            std::map<unsigned long, TA_Base_Core::ILocation*> m_locationKeyMap;
            std::map<std::string, TA_Base_Core::ILocation*> m_locationNameMap;
    };
}
#endif