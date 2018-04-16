#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiAppStepParameter.h"
#include "LaunchGuiAppStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include <boost/lexical_cast.hpp>

namespace TA_Base_App
{

LaunchGuiAppStepEditorParameter::LaunchGuiAppStepEditorParameter(LaunchGuiAppStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

LaunchGuiAppStepEditorParameter::~LaunchGuiAppStepEditorParameter()
{
}

bool LaunchGuiAppStepEditorParameter::failureIgnorable()
{
    return true;
}

bool LaunchGuiAppStepEditorParameter::skippable()
{
    return true;
}

bool LaunchGuiAppStepEditorParameter::delayable()
{
    return true;
}

bool LaunchGuiAppStepEditorParameter::responseRequired()
{
    return true;
}

time_t LaunchGuiAppStepEditorParameter::minimumTimeout()
{
    return 30u;
}
// [ToDo] Design general solution for all steps, using only defineAddonParameters
void LaunchGuiAppStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamAppValid = false;

	std::string valueApp = m_guiAppStepEditorHelper.getGuiNameById(m_refParameter.app);

	ParamAppValid = !valueApp.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueApp, ParamApp, ParamApp, eList, ParamAppValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.args, ParamArgs, ParamArgs, eString, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pos.top, ParamPositionTop, ParamPositionTop, eUnsignedInteger, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pos.left, ParamPositionLeft, ParamPositionLeft, eUnsignedInteger, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pos.width, ParamPositionWidth, ParamPositionWidth, eUnsignedInteger, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.pos.height, ParamPositionHeight, ParamPositionHeight, eUnsignedInteger, true);
}

IStringEditor* LaunchGuiAppStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}


// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool LaunchGuiAppStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if(strName == ParamArgs)
    {
        m_refParameter.args = strDisplayValue;
        bValid = true;
        return false;
    }
	unsigned long value = 0;
    try
    {
        value = boost::lexical_cast<unsigned long>(strDisplayValue);
    }
    catch(...)
    {
        return false;
    }
    if(strName == ParamPositionTop)
    {
        m_refParameter.pos.top = value;
        bValid = true;
    }
    else if(strName == ParamPositionLeft)
    {
        m_refParameter.pos.left = value;
        bValid = true;
    }
    else if(strName == ParamPositionWidth)
    {
        m_refParameter.pos.width = value;
        bValid = true;
    }
    else if(strName == ParamPositionHeight)
    {
        m_refParameter.pos.height = value;
        bValid = true;
    }
    return false;
}

IListPicker* LaunchGuiAppStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void LaunchGuiAppStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamApp)
    {
        vecKeyItems = m_guiAppStepEditorHelper.getAllItems();
        if(m_guiAppStepEditorHelper.isValidGuiId(m_refParameter.app))
        {
            vecSelected.push_back(m_refParameter.app);
        }
        nMaxSelection = 1;
    }
}

bool LaunchGuiAppStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(strName == ParamApp)
    {
        m_refParameter.app = vecSelectedKeys[0];
		strDisplayValue = m_guiAppStepEditorHelper.getGuiNameById(vecSelectedKeys[0]);
    }
	return false;
}

void LaunchGuiAppStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if(strName == ParamArgs)
    {
		convertToDisplayValue(m_refParameter.args, strCurValue);
		szMaxLength = 800;
    }
    else if(strName == ParamPositionTop)
    {
		convertToDisplayValue(m_refParameter.pos.top, strCurValue);
		szMaxLength = 4;
    }
    else if(strName == ParamPositionLeft)
    {
		convertToDisplayValue(m_refParameter.pos.left, strCurValue);
		szMaxLength = 4;
    }
    else if(strName == ParamPositionWidth)
    {
		convertToDisplayValue(m_refParameter.pos.width, strCurValue);
		szMaxLength = 4;
    }
    else if(strName == ParamPositionHeight)
    {
		convertToDisplayValue(m_refParameter.pos.height, strCurValue);
		szMaxLength = 4;
    }
}

}