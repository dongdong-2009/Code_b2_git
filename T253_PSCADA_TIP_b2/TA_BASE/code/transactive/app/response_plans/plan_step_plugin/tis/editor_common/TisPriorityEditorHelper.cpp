#include "TisPriorityEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

TisPriorityEditorHelper::TisPriorityEditorHelper()
{
	ListItem items[] =
	{
		{1, "Emergency - 1"},
		{2, "Emergency - 2"},
		{3, "Emergency - 3"},
		{4, "Normal - 4"},
		{5, "Normal - 5"},
		{6, "Normal - 6"},
		{7, "Normal - 7"},
		{8, "Normal - 8"}
	};
	for(size_t i = 0; i < sizeof(items)/sizeof(ListItem); ++i)
	{
	    m_priorities.insertItem(items[i]);
	}
}

TisPriorityEditorHelper::~TisPriorityEditorHelper()
{

}

std::vector<ListItem> TisPriorityEditorHelper::getPriorityItems()
{
	return m_priorities.getAllItems();
}

bool TisPriorityEditorHelper::isValidPriority(int priority)
{
	return m_priorities.isItemInCache(priority);
}

std::string TisPriorityEditorHelper::getPriorityDescription( int priority )
{
	return m_priorities.getItemById(priority);
}

}