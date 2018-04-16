#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"
#include <set>

struct ListItem;
namespace TA_Base_App
{
class TisPriorityEditorHelper
{
public:
    TisPriorityEditorHelper();
    ~TisPriorityEditorHelper();
    std::vector<ListItem> getPriorityItems();
    bool isValidPriority(int priority);
	std::string getPriorityDescription(int priority);
private:
	UniqueListItemCache m_priorities;
};
}