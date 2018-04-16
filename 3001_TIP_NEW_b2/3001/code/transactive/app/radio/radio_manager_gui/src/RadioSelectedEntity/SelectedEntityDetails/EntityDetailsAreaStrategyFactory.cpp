/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/EntityDetailsAreaStrategyFactory.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/EntityDetailsAreaStrategyFactory.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/RadioEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/TalkGroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/MultiselectEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/PatchGroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/EmptyEntityDetailsAreaStrategy.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EntityDetailsAreaStrategyFactory::EntityDetailsAreaStrategyFactory()
{

}

EntityDetailsAreaStrategyFactory::~EntityDetailsAreaStrategyFactory()
{

}


IEntityDetailsAreaStrategy * EntityDetailsAreaStrategyFactory::getEntityDetailsAreaStrategy(RadioResourceType type)
{
    FUNCTION_ENTRY("getEntityDetailsAreaStrategy");
    
	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = NULL;

	switch(type)
	{
		case RR_RADIO: 
		{
			RadioEntityDetailsAreaStrategy* pRadio = new RadioEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pRadio;
		}
		break;
		//case RR_ANNGROUP: 
		//{
		//	AnnouncementGroupEntityDetailsAreaStrategy* pAnnGroup = new AnnouncementGroupEntityDetailsAreaStrategy();
		//	pEntityDetailsStrategy = pAnnGroup;
		//}
		case RR_DYNGROUP: 
		{
			DynamicRegroupEntityDetailsAreaStrategy* pDynamic = new DynamicRegroupEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pDynamic;
		}
		break;
		case RR_TALKGROUP:
		{
			TalkGroupEntityDetailsAreaStrategy* pTalkGroup = new TalkGroupEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pTalkGroup;
		}
		break;
		case RR_PATCHGROUP: 
		{
			PatchGroupEntityDetailsAreaStrategy* pPatchGroup = new PatchGroupEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pPatchGroup;
		}
		break;
		case RR_MSELECT: 
		{
			MultiselectEntityDetailsAreaStrategy* pMultiSelect = new MultiselectEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pMultiSelect;
		}
		break;
		//case RR_SITE: 
		//{
		//	SiteEntityDetailsAreaStrategy* pSite= new SiteEntityDetailsAreaStrategy();
		//	pEntityDetailsStrategy = pSite;
		//}
		//break;
		//case RR_MSITE: 
		//{
		//	MultiSiteEntityDetailsAreaStrategy* pMultiSite = new MultiSiteEntityDetailsAreaStrategy();
		//	pEntityDetailsStrategy = pMultiSite;
		//}
		//break;

		default:
		{
			EmptyEntityDetailsAreaStrategy* pEmptyStrategy = new EmptyEntityDetailsAreaStrategy();
			pEntityDetailsStrategy = pEmptyStrategy;
		}

	}

	FUNCTION_EXIT;
	return pEntityDetailsStrategy;	
}
