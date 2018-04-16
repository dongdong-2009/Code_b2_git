#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/TisStationEditorHelper.h"
#include "app/response_plans/plan_step_plugin/tis/editor_common/TisConfig.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

const unsigned int TisStationEditorHelper::INVALID_ID = -1;

TisStationEditorHelper::TisStationEditorHelper()
{
    TisConfig::TisStationNameSet stations = TisConfig::getSTisStations();
    int id = 1;
    BOOST_FOREACH(const std::string & i, stations)
    {
		m_stations.insertItem(id++, i);
    }
}

TisStationEditorHelper::~TisStationEditorHelper()
{

}

std::vector<ListItem> TisStationEditorHelper::getStationItems()
{
	return m_stations.getAllItems();
}

std::string TisStationEditorHelper::getStationById(const unsigned int id)
{
	return m_stations.getItemById(id);
}

unsigned int TisStationEditorHelper::getIdByStation(const std::string& station)
{
	return m_stations.getIdByItem(station);
}

}
