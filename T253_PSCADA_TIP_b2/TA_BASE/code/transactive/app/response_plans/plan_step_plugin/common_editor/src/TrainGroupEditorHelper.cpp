#include "app/response_plans/plan_step_plugin/common_editor/src/TrainGroupEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"

namespace TA_Base_App
{


TrainGroupEditorHelper::TrainGroupEditorHelper()
{
}

TrainGroupEditorHelper::~TrainGroupEditorHelper()
{
}

std::vector<ListItem> TrainGroupEditorHelper::getTrainGroupItems()
{
	if(m_groupItems.empty())
	{
		for(std::map<int, std::string>::iterator it = m_groupTypeAndDescMap.begin(); it != m_groupTypeAndDescMap.end(); ++it)
		{
			ListItem tmp = {(*it).first, (*it).second};
			m_groupItems.push_back(tmp);
		}
	}
	return m_groupItems;
}

bool TrainGroupEditorHelper::isTrainGroupValid(const int type)
{
	return m_groupTypeAndDescMap.find(type) != m_groupTypeAndDescMap.end();
}

std::string TrainGroupEditorHelper::getTrainGroupName(const int type)
{
	if(isTrainGroupValid(type))
	{
		return m_groupTypeAndDescMap[type];
	}
	return "";
}

void TrainGroupEditorHelper::insertGroup(const int type, const std::string& description)
{
	m_groupTypeAndDescMap.insert(std::make_pair(type, description));
}

}