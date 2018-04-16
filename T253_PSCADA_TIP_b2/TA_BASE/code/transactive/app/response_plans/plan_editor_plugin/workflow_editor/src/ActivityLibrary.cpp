
#include "app/response_plans/plan_editor_plugin/workflow_element/src/WorkflowActivity.h"
#include "GlobalStructure.h"
#include "ActivityLibrary.h"

ActivityLibrary* ActivityLibrary::s_pInstance = 0;

ActivityLibrary::ActivityLibrary()
{
}

ActivityLibrary::~ActivityLibrary()
{
}

ActivityLibrary& ActivityLibrary::instance()
{
    if ( 0 == s_pInstance )
    {
        s_pInstance = new ActivityLibrary();
    }

    return *s_pInstance;
}

void ActivityLibrary::plugActivities( const std::string& strCategory, const std::vector<IActivityComponent*>& vecActivityComponent )
{
    m_strCategroyName = strCategory.c_str();

    for ( std::vector<IActivityComponent*>::const_iterator itLoop = vecActivityComponent.begin(); vecActivityComponent.end() != itLoop; ++itLoop )
    {
        IActivityComponent* pComponent = *itLoop;
        if ( NULL == pComponent )
        {
            continue;
        }

        m_mapTypeToActivityCreators.insert( std::map<int, CreateActivitycPtr>::value_type( pComponent->getStepType(), pComponent->getActivityCreationFuncPtr() ));
		m_mapTypeToShortcuts.insert( std::map<int, std::vector<std::pair<std::string, std::vector<int>>>>::value_type(pComponent->getStepType(), pComponent->getShortcuts() ));

        ActivityItem stItem = { !pComponent->readonly(), pComponent->getStepType(), pComponent->getName(), pComponent->getResource() };
        m_vecActivities.push_back( stItem );
    }
}

WorkflowActivity* ActivityLibrary::createActivity( const int nType )
{
    WorkflowActivity* pNewElement = NULL;

    std::map<int, CreateActivitycPtr>::const_iterator itFound = m_mapTypeToActivityCreators.find( nType );

    if ( m_mapTypeToActivityCreators.end() != itFound )
    {
        pNewElement = itFound->second();
        pNewElement->Type = nType;

		std::vector<std::pair<std::string, std::vector<int>>>& refShortcuts = m_mapTypeToShortcuts[nType];
		for ( std::vector<std::pair<std::string, std::vector<int>>>::iterator itLoop = refShortcuts.begin(); refShortcuts.end() != itLoop; ++itLoop )
		{
			pNewElement->attachShortcut( itLoop->first, itLoop->second );
		}
    }

    return pNewElement;
}

std::vector<ActivityItem>& ActivityLibrary::getActivityItems()
{
    return m_vecActivities;
}

std::string ActivityLibrary::getCategoryName()
{
    return m_strCategroyName;
}

