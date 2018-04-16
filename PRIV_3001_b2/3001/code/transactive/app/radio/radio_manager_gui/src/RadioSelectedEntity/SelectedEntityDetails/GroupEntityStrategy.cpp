
#include "app\radio\radio_manager_gui\src\RadioSelectedEntity\SelectedEntityDetails\GroupEntityStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "core\utilities\src\DebugUtil.h"

#include <algorithm>
#include <vector>

#define EMPTY_STRING	""
#define EQUAL_STRING	0

GroupEntityStrategy::GroupEntityStrategy(void) : m_bNewEntry (false)
{
	FUNCTION_ENTRY ("GroupEntityStrategy");
	FUNCTION_EXIT;
}

GroupEntityStrategy::~GroupEntityStrategy(void)
{
	FUNCTION_ENTRY ("~GroupEntityStrategy");
	FUNCTION_EXIT;
}

bool GroupEntityStrategy::isDescriptionAlreadyExist (const std::string & strDesc, 
													 RadioResourceType radioGrpType)
{
	FUNCTION_ENTRY ("isDescriptionAlreayExist");
	bool bReturn = false;
	

	if (m_bNewEntry // add new entry 
		|| 
		0 != m_resourceBeingEdit.alias.compare(strDesc)) // check for edit)	
	{
		std::vector <RadioResource> vctrItemData = The_RadioSystemStatus::instance()->getResourceCollectioForType(radioGrpType);	
	
		GroupNameCollection::iterator iterFindGrpDesc;
		GroupNameCollection grpCollection = getAllGroupName(vctrItemData);;

		iterFindGrpDesc = std::find(grpCollection.begin(),grpCollection.end(),strDesc);
		
		if (iterFindGrpDesc != grpCollection.end())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Group Description is Existing");
			bReturn = true;
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Group Description is Not Existing");
		}
	}
	
	FUNCTION_EXIT;
	return bReturn;
}

GroupNameCollection GroupEntityStrategy::getAllGroupName(RadioResourceCollection radioGrpCollection)
{
	FUNCTION_ENTRY ("getAllGroupName");
	GroupNameCollection grpNameCollection;
	
	RadioResourceCollection::iterator iterRadioCollection = radioGrpCollection.begin();

	for (;iterRadioCollection!=radioGrpCollection.end();
		  iterRadioCollection++)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"Group Description : %s", (*iterRadioCollection).alias.c_str());
		grpNameCollection.push_back((*iterRadioCollection).alias);
	}

	FUNCTION_EXIT;
	return grpNameCollection;
}


void GroupEntityStrategy::setFlagIfNewEntry (RadioResource radioResource)
{
	FUNCTION_ENTRY ("isDescriptionAlreayExist");
	
	if ( EQUAL_STRING == radioResource.alias.compare(EMPTY_STRING))
	{
		m_bNewEntry = true;
	}
	else
	{
		m_bNewEntry = false;

		m_resourceBeingEdit = radioResource;
	}

	FUNCTION_EXIT;
}

bool GroupEntityStrategy::isNewEntry ()
{
	FUNCTION_ENTRY ("isNewEntry");
	FUNCTION_EXIT;
	return m_bNewEntry;
}