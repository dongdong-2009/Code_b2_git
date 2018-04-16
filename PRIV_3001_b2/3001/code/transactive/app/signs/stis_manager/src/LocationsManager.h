#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

class LocationsManager : public boost::noncopyable
{
public:
	static LocationsManager& instance();
	std::vector<TA_Base_Core::ILocation*> getAllLocations();
	TA_Base_Core::ILocation* getLocationByKey(unsigned long locationKey);
	TA_Base_Core::ILocation* getLocationByName(const std::string& locationName);
private:
	LocationsManager();
	static TA_Base_Core::ReEntrantThreadLockable m_lock;
	static LocationsManager* m_instance;
	std::vector<TA_Base_Core::ILocation*> m_allLocations;
	std::map<unsigned long, TA_Base_Core::ILocation*> m_locationKeyMap;
	std::map<std::string, TA_Base_Core::ILocation*> m_locationNameMap;
};