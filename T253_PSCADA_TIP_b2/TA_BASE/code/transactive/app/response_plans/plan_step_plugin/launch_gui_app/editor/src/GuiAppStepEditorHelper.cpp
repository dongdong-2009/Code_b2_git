#include "GuiAppStepEditorHelper.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{


const unsigned int GuiAppStepEditorHelper::INVALID_ID = -1;


GuiAppStepEditorHelper::GuiAppStepEditorHelper()
{
    std::vector<TA_Base_Core::IGui*> guis;
    try
    {
        guis = TA_Base_Core::GuiAccessFactory::getInstance().getAllGuis();
    }
    catch(...)
    {
        // TODO
    }
    BOOST_FOREACH(TA_Base_Core::IGui * i, guis)
    {
        m_guiApps.insertItem(i->getKey(), i->getName());
        delete i;
    }
}

GuiAppStepEditorHelper::~GuiAppStepEditorHelper()
{
}

std::vector<ListItem> GuiAppStepEditorHelper::getAllItems()
{
    return m_guiApps.getAllItems();
}

bool GuiAppStepEditorHelper::isValidGuiId(const unsigned int id)
{
    return !m_guiApps.getItemById(id).empty();
}

std::string GuiAppStepEditorHelper::getGuiNameById(const unsigned int id)
{
    return m_guiApps.getItemById(id);
}

}