#pragma once
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{

class CachedConfig : boost::noncopyable
{
public:
	static CachedConfig& instance();
	~CachedConfig(void);
	unsigned long getLocationKey();
	void setLocationKey(const unsigned long locationKey);
	int getAtsTrainListPollingInterval();
	void setAtsTrainListPollingInterval(int second);
private:
	CachedConfig(void);

	unsigned long m_locationKey;
	int m_atsTrainListPollingInterval;
};

}
