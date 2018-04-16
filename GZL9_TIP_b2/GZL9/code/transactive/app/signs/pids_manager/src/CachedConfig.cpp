#include "app/signs/pids_manager/src/CachedConfig.h"
#include "app/signs/pids_manager/src/ConstantDefs.h"

namespace TA_IRS_App
{

CachedConfig::CachedConfig(void) :
        m_locationKey(DEFAULT_LOCATIONKEY),
        m_atsTrainListPollingInterval(DEFAULT_ATS_POLLING_INTERVALE)
{
}

CachedConfig::~CachedConfig(void)
{
}

CachedConfig& CachedConfig::instance()
{
    static CachedConfig _cachedConfig;
    return _cachedConfig;
}

unsigned long CachedConfig::getLocationKey()
{
    return m_locationKey;
}

void CachedConfig::setLocationKey(const unsigned long locationKey)
{
    m_locationKey = locationKey;
}

int CachedConfig::getAtsTrainListPollingInterval()
{
    return m_atsTrainListPollingInterval;
}

void CachedConfig::setAtsTrainListPollingInterval(int second)
{
    m_atsTrainListPollingInterval = second;
}

}
