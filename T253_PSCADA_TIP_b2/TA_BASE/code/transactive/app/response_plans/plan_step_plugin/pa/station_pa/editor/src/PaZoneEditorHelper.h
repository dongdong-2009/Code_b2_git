#pragma once;
#include <vector>
#include <map>

struct ListItem;

namespace TA_Base_Core
{
	class IPaZone;
}

namespace TA_Base_App
{

class PaZoneEditorHelper
{
public:
    PaZoneEditorHelper();
    ~PaZoneEditorHelper();
    std::vector<ListItem> getAllPaZoneItems();
    std::vector<ListItem> getPaZoneItemsByLocation(unsigned long locationKey);
	TA_Base_Core::IPaZone* getPaZoneByKey(unsigned long key);
private:
    typedef std::map<unsigned long, TA_Base_Core::IPaZone*> PaZoneMap;
    PaZoneMap m_paZoneMap;
};

}