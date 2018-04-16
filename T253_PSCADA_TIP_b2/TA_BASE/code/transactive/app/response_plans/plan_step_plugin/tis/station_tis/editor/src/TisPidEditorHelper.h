#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"
#include <vector>

struct ListItem;
namespace TA_Base_App
{

class TisPidEditorHelper
{
public:
	static const unsigned int INVALID_ID;
    TisPidEditorHelper();
    ~TisPidEditorHelper();
	std::vector<ListItem> getPidItems(const std::string& station);
    std::string getPidById(const unsigned int id);
	unsigned int getIdByPid(const std::string& pid);
private:
	UniqueListItemCache m_pids;
};

}


