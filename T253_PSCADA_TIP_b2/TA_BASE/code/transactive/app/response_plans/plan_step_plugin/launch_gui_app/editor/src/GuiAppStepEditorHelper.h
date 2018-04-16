#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"

struct ListItem;

namespace TA_Base_App
{
class GuiAppStepEditorHelper
{
public:
    static const unsigned int INVALID_ID;
    GuiAppStepEditorHelper();
    ~GuiAppStepEditorHelper();
    std::vector<ListItem> getAllItems();
    bool isValidGuiId(const unsigned int id);
    std::string getGuiNameById(const unsigned int id);
private:
    UniqueListItemCache m_guiApps;
};
}