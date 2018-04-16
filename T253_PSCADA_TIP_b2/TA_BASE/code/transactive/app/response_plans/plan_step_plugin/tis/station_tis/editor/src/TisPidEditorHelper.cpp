#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/TisPidEditorHelper.h"
#include "app/response_plans/plan_step_plugin/tis/editor_common/TisConfig.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

const unsigned int TisPidEditorHelper::INVALID_ID = -1;

TisPidEditorHelper::TisPidEditorHelper()
{
}

TisPidEditorHelper::~TisPidEditorHelper()
{

}

std::vector<ListItem> TisPidEditorHelper::getPidItems(const std::string& station)
{
	TisConfig::TisPidSet pids = TisConfig::getSTisPids(station);
	m_pids.clear();
	unsigned int id = 1;
	BOOST_FOREACH(const std::string& i, pids)
	{
		m_pids.insertItem(id++, i);
	}
	return m_pids.getAllItems();
}

std::string TisPidEditorHelper::getPidById( const unsigned int id )
{
	return m_pids.getItemById(id);
}

unsigned int TisPidEditorHelper::getIdByPid( const std::string& pid )
{
	return m_pids.getIdByItem(pid);
}

}


