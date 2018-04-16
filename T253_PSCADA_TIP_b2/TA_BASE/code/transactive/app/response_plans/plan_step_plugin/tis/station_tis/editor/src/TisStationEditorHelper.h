#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"
#include <vector>
#include <map>

struct ListItem;
namespace TA_Base_App
{

class TisStationEditorHelper
{
public:
	static const unsigned int INVALID_ID;
    TisStationEditorHelper();
    ~TisStationEditorHelper();
    std::vector<ListItem> getStationItems();
    std::string getStationById(const unsigned int id);
    unsigned int getIdByStation(const std::string& station);
private:
	UniqueListItemCache m_stations;
};

}
