#pragma once;

#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <vector>
#include <map>

namespace TA_Base_App
{

class TrainGroupEditorHelper
{
public:
    TrainGroupEditorHelper();
    ~TrainGroupEditorHelper();
	bool isTrainGroupValid(const int type);
    std::vector<ListItem> getTrainGroupItems();
	std::string getTrainGroupName(const int type);
	void insertGroup(const int type, const std::string& description);
private:
	std::map<int, std::string> m_groupTypeAndDescMap;
	std::vector<ListItem> m_groupItems;
};

}