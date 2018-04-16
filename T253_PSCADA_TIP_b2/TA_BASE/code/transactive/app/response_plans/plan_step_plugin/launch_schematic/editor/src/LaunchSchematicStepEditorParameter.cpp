#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicStepParameter.h"
#include "LaunchSchematicStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

LaunchSchematicStepEditorParameter::LaunchSchematicStepEditorParameter(LaunchSchematicStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

LaunchSchematicStepEditorParameter::~LaunchSchematicStepEditorParameter()
{
}

bool LaunchSchematicStepEditorParameter::failureIgnorable()
{
    return true;
}

bool LaunchSchematicStepEditorParameter::skippable()
{
    return true;
}

bool LaunchSchematicStepEditorParameter::delayable()
{
    return true;
}

bool LaunchSchematicStepEditorParameter::responseRequired()
{
    return true;
}

time_t LaunchSchematicStepEditorParameter::minimumTimeout()
{
    return 30u;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void LaunchSchematicStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamSchematicValid = !m_refParameter.schematic.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.schematic, ParamSchematic, ParamSchematic, eList, ParamSchematicValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.screen, ParamScreen, ParamScreen, eUnsignedInteger, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.commands, ParamCommands, ParamCommands, eString, true);
}

IStringEditor* LaunchSchematicStepEditorParameter::getStringEditor(const std::string& strName)
{
    return this;
}


// [ToDo] Design general solution for all steps, using only defineAddonParameters
bool LaunchSchematicStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
{
    if(0 == strName.compare(ParamScreen))
    {
        convertToActualValue(strDisplayValue, m_refParameter.screen);
        bValid = true;
    }
    else if(0 == strName.compare(ParamCommands))
    {
        convertToActualValue(strDisplayValue, m_refParameter.commands);
        bValid = true;
    }
    // No parameter list changed
    return false;
}

IListPicker* LaunchSchematicStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void LaunchSchematicStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamSchematic)
    {
        vecKeyItems = m_schematicEditorHelper.getAllItems();
        unsigned int id = m_schematicEditorHelper.getIdBySchematic(m_refParameter.schematic);
        if(m_schematicEditorHelper.INVALID_ID != id)
        {
            vecSelected.push_back(id);
        }
        nMaxSelection = 1;
    }
}

bool LaunchSchematicStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(vecSelectedKeys.empty())
    {
        return false;
    }
    if(0 == strName.compare(ParamSchematic))
    {
        m_refParameter.schematic = m_schematicEditorHelper.getSchematicById(vecSelectedKeys[0]);
		strDisplayValue = m_refParameter.schematic;
    }
	return false;
}

void LaunchSchematicStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
{
    if(0 == strName.compare(ParamScreen))
    {
		convertToDisplayValue(m_refParameter.screen, strCurValue);
		szMaxLength = 2;
    }
    else if(0 == strName.compare(ParamCommands))
    {
		convertToDisplayValue(m_refParameter.commands, strCurValue);
		szMaxLength = 800;
    }
}

}
