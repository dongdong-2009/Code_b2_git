#include "DataPointTestTypeEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"

namespace TA_Base_App
{

ListItem DataPointTestTypeEditorHelper::s_items[] =
{
    {EQUAL_TO_TEST, "Equal"},
    {NOT_EQUAL_TO_TEST, "Not Equal"},
    {LESS_THAN_TEST, "Less Than"},
    {GREATER_THAN_TEST, "Greater Than"},
    {LESS_THAN_OR_EQUAL_TO_TEST, "Less Than or Equal"},
    {GREATER_THAN_OR_EQUAL_TO_TEST, "Greater Than Or Equal"},
};

DataPointTestTypeEditorHelper::DataPointTestTypeEditorHelper()
{
}

DataPointTestTypeEditorHelper::~DataPointTestTypeEditorHelper()
{
}

std::vector<ListItem> DataPointTestTypeEditorHelper::getItems( const int eDpType )
{
    std::vector<ListItem> items;

    if ( NUMERIC_DATA_POINT == eDpType )
    {
        for(size_t i = 0; i < sizeof(s_items) / sizeof(ListItem); ++i)
        {
            items.push_back(s_items[i]);
        }
    }
    else
    {
        items.push_back(s_items[EQUAL_TO_TEST]);
        items.push_back(s_items[NOT_EQUAL_TO_TEST]);
    }

    return items;
}

bool DataPointTestTypeEditorHelper::isValidTestType(int type)
{
    return type >= EQUAL_TO_TEST && type <= GREATER_THAN_OR_EQUAL_TO_TEST;
}

std::string DataPointTestTypeEditorHelper::getTypeDescription(int type)
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