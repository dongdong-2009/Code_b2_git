
#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointValueTypeEditorHelper.h"

namespace TA_Base_App
{

ListItem DataPointValueTypeEditorHelper::s_items[] =
{
    {BOOLEAN_DATA_POINT, "Boolean"},
    {NUMERIC_DATA_POINT, "Numeric"},
    {COMBINED_DATA_POINT, "Numeric"},
    {TEXT_DATA_POINT, "Text"},
    {UNDEFINED_DATA_POINT, "Undefined"},
};

DataPointValueTypeEditorHelper::DataPointValueTypeEditorHelper()
{
}

DataPointValueTypeEditorHelper::~DataPointValueTypeEditorHelper()
{
}

std::vector<ListItem> DataPointValueTypeEditorHelper::getItems()
{
    std::vector<ListItem> items;
    for(size_t i = 0; i < sizeof(s_items) / sizeof(s_items[0]); ++i)
    {
        items.push_back(s_items[i]);
    }
    return items;
}

bool DataPointValueTypeEditorHelper::isValidType(int type)
{
    return type >= BOOLEAN_DATA_POINT && type <= UNDEFINED_DATA_POINT;
}

std::string DataPointValueTypeEditorHelper::getTypeDescription(int type)
{
    for(size_t i = 0; i < sizeof(s_items) / sizeof(s_items[0]); ++i)
    {
        if(s_items[i].ID == type)
        {
            return s_items[i].Item;
        }
    }
    return "";
}

std::string DataPointValueTypeEditorHelper::getTypeDescriptionByScadaType(const std::string& scadaType)
{
	return getTypeDescription(getTypeByScadaType(scadaType));
}

TA_Base_App::EDataPoint DataPointValueTypeEditorHelper::getTypeByScadaType( const std::string& scadaType )
{
    EDataPoint type = UNDEFINED_DATA_POINT;

    if(scadaType == TA_Base_Bus::DPT_CFG_UINT16)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_INT16)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_INT16S)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_UINT32)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_INT32)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_INT32S)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_IEEE32)
    {
        type = NUMERIC_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_BOOL)
    {
        type = BOOLEAN_DATA_POINT;
    }
    else if(scadaType == TA_Base_Bus::DPT_CFG_TEXT)
    {
        type = TEXT_DATA_POINT;
    }
	else if(scadaType == TA_Base_Bus::DPT_CFG_DERIVED)
	{
        type = COMBINED_DATA_POINT;
	}

	return type;
}

}