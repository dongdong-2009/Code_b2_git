/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.h"

//#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/RadioRadioResourceButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.h"

//#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/AnnouncementGroupRadioResourceButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/PatchGroupRadioResourceButtonStrategy.h"

//#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/DynamicRegroupRadioResourceButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/MultiSelectRadioResourceButtonStrategy.h"

//#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/SiteRadioResourceButtonStrategy.h"

//#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/MultiSiteRadioResourceButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"

#include "core/utilities/src/DebugUtil.h"


ResourceButtonStrategyFactory::ResourceButtonStrategyFactory()
{

}


ResourceButtonStrategyFactory::~ResourceButtonStrategyFactory()
{

}

IResourceButtonStrategy* ResourceButtonStrategyFactory::getResourceButtonStrategy(RadioResourceType radioResourceType)
{
	FUNCTION_ENTRY("getResourceButtonStrategy");

	IResourceButtonStrategy *pNewResourceButtonStrategy = NULL;

	switch( radioResourceType ) 
	{
	//For completeness;
	case RR_NIL:
		pNewResourceButtonStrategy = new EmptyButtonStrategy();
		break;

	//case RR_RADIO:
	//	pNewResourceButtonStrategy = new RadioRadioResourceButtonStrategy();
	//	break;

	case RR_TALKGROUP:
		pNewResourceButtonStrategy = new TalkGroupRadioResourceButtonStrategy();
		break;

	//case RR_ANNGROUP:
	//	pNewResourceButtonStrategy = new AnnouncementGroupRadioResourceButtonStrategy();
	//	break;

	case RR_PATCHGROUP:
		pNewResourceButtonStrategy = new PatchGroupRadioResourceButtonStrategy();
		break;

	//case RR_DYNGROUP:
	//	pNewResourceButtonStrategy = new DynamicRegroupRadioResourceButtonStrategy();
	//	break;

	case RR_MSELECT:
		pNewResourceButtonStrategy = new MultiSelectRadioResourceButtonStrategy();
		break;

	//case RR_SITE:
	//	pNewResourceButtonStrategy = new SiteRadioResourceButtonStrategy();
	//	break;

	//case RR_MSITE:
	//	pNewResourceButtonStrategy = new MultiSiteRadioResourceButtonStrategy();
	//	break;

	//case RR_TRAIN:
	//	//pNewResourceButtonStrategy = new TelephoneResourceButtonStrategy();
	//	break;

	//For completeness;
	default:
		break;
	}

	FUNCTION_EXIT;

	return pNewResourceButtonStrategy;
}
