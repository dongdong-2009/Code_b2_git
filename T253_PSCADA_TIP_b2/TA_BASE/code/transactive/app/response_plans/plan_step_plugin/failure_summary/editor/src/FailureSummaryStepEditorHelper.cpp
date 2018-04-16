#include "FailureSummaryStepEditorHelper.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryStepParameter.h"

namespace TA_Base_App
{

ListItem FailureSummaryStepEditorHelper::s_items[] =
{
    {FailureSummaryStepParameter::SFC_ALL_STEPS, "All Steps"},
    {FailureSummaryStepParameter::SFC_SET_DATA_POINT_STEPS, "Datapoint Steps"}
};

FailureSummaryStepEditorHelper::FailureSummaryStepEditorHelper()
{
}

FailureSummaryStepEditorHelper::~FailureSummaryStepEditorHelper()
{
}

std::vector<ListItem> FailureSummaryStepEditorHelper::getItems()
{
    std::vector<ListItem> items;
    for(size_t i = 0; i < sizeof(s_items) / sizeof(ListItem); ++i)
    {
        items.push_back(s_items[i]);
    }
    return items;
}

bool FailureSummaryStepEditorHelper::isValidFailureSummaryType(const int type)
{
    return type >= FailureSummaryStepParameter::SFC_ALL_STEPS && type <= FailureSummaryStepParameter::SFC_SET_DATA_POINT_STEPS;
}

std::string FailureSummaryStepEditorHelper::getFailureSummaryDescription(const int type)
{
    for(size_t i = 0; i < sizeof(s_items) / sizeof(ListItem); ++i)
    {
		if(s_items[i].ID == type)
		{
			return s_items[i].Item;
		}
    }
	return "";
}

}