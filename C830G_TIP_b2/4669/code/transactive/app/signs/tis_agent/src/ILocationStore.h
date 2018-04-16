#ifndef TIS_AGENT_ILOCTIONSTORE_H_INCLUDED
#define TIS_AGENT_ILOCTIONSTORE_H_INCLUDED
/**
  *This class is used to fix CL-21314,reduce database access
  */
#include "core/data_access_interface/src/ILocation.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <map>
namespace TA_IRS_App
{
    class ILocationStore
    {
    public:
        ~ILocationStore();
        static ILocationStore* getInstance();
        TA_Base_Core::ILocation* getLocationByKey( TA_IRS_Bus::StationIdType stationId );
    private:
        ILocationStore();
        static ILocationStore* m_theClassInstance;
        static TA_Base_Core::ReEntrantThreadLockable m_ilocationMapAccessLock;
        std::map<TA_IRS_Bus::StationIdType, TA_Base_Core::ILocation*> m_ilocationMap;
    };
}
#endif