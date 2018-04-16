#pragma once;
#include <vector>
#include <map>

struct ListItem;

namespace TA_Base_Core
{
class ILocation;
}

namespace TA_Base_App
{
class LocationEditorHelper
{
public:
    LocationEditorHelper();
    ~LocationEditorHelper();
    std::vector<ListItem> getLocationItems();
    TA_Base_Core::ILocation* getLocationByKey(unsigned long locationKey);
private:
    typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;
    LocationMap m_locationMap;
};
}