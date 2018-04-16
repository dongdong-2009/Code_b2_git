#include "core/data_access_interface/src/IProfile.h"
#include "OperatorProfileEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ProfileConfigAccess.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

const unsigned int OperatorProfileEditorHelper::INVALID_ID = -1;

OperatorProfileEditorHelper::OperatorProfileEditorHelper()
{
    try
    {
        const ProfileMap& profileMap = ProfileConfigAccess::instance()->getAllProfile();
		BOOST_FOREACH(const ProfileMap::value_type& i, profileMap)
        {
			TA_Base_Core::IProfile* profile = i.second;
			m_profiles.insertItem(profile->getKey(), profile->getName());
        }
    }
    catch (...)
    {
    }
}

OperatorProfileEditorHelper::~OperatorProfileEditorHelper()
{
}

std::vector<ListItem> OperatorProfileEditorHelper::getOperatorProfileItems()
{
	return m_profiles.getAllItems();
}

std::string OperatorProfileEditorHelper::getOperatorNameById( const unsigned int id )
{
	return m_profiles.getItemById(id);
}

}



