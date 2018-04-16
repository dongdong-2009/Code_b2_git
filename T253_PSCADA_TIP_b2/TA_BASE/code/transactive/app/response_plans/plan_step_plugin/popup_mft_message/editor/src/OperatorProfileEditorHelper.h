#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"
#include <vector>

struct ListItem;
namespace TA_Base_App
{

class OperatorProfileEditorHelper
{
public:
	static const unsigned int INVALID_ID;
    OperatorProfileEditorHelper();
    ~OperatorProfileEditorHelper();
	std::vector<ListItem> getOperatorProfileItems();
	std::string getOperatorNameById(const unsigned int id);
private:
	UniqueListItemCache m_profiles;
};

}

