#pragma once

#include <string>
#include <vector>
#include <map>

#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityComponent.h"

class WorkflowActivity;
struct ActivityItem;

class ActivityLibrary
{
public:
    ~ActivityLibrary();
    static ActivityLibrary& instance();

    void plugActivities( const std::string& strCategory, const std::vector<IActivityComponent*>& vecActivityComponent );
    WorkflowActivity* createActivity( const int nType );
    std::vector<ActivityItem>& getActivityItems();
    std::string getCategoryName();

private:
    ActivityLibrary();

private:
    static ActivityLibrary* s_pInstance;
    std::string m_strCategroyName;
	std::map<int, CreateActivitycPtr> m_mapTypeToActivityCreators;
	std::map<int, std::vector<std::pair<std::string, std::vector<int>>>> m_mapTypeToShortcuts;
    std::vector<ActivityItem> m_vecActivities;
};

