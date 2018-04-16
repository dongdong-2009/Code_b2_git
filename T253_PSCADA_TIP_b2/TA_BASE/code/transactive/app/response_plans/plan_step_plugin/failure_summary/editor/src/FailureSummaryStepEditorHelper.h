#pragma once;

#include <vector>
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"

namespace TA_Base_App
{
class FailureSummaryStepEditorHelper
{
public:
    FailureSummaryStepEditorHelper();
    ~FailureSummaryStepEditorHelper();
    static std::vector<ListItem> getItems();
    static bool isValidFailureSummaryType(const int type);
    static std::string getFailureSummaryDescription(const int type);
private:
    static ListItem s_items[];
};
}