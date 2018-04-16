#pragma once;

#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <vector>
#include <set>
#include <ace/RW_Mutex.h>

namespace TA_Base_App
{

class TrainIDEditorHelper
{
public:
    TrainIDEditorHelper();
    TrainIDEditorHelper(const unsigned int trainIdBegin, const unsigned int trainIdEnd);
	TrainIDEditorHelper(const std::set<unsigned int>& trainIds);
	void initHelper(const std::set<unsigned int>& trainIds);
    ~TrainIDEditorHelper();
	bool isTrainIdValid(const unsigned int trainId);
    std::vector<ListItem> getTrainIdItems();
private:
	std::set<unsigned int> m_trainIds;
	std::vector<ListItem> m_trainIdItems;
	ACE_RW_Mutex m_mtxTrainIds;
};

}