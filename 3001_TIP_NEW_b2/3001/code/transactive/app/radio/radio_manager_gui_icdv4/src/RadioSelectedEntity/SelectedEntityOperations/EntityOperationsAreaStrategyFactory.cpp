/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/EntityOperationsAreaStrategyFactory.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/EntityOperationsAreaStrategyFactory.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/DynamicRegroupEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/EmptyEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/MultiselectEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/MultiSiteOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/PatchGroupEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/RadioEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/TalkGroupEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/SiteEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/AnnouncementGroupEntityOperationsAreaStrategy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EntityOperationsAreaStrategyFactory::EntityOperationsAreaStrategyFactory()
{

}

EntityOperationsAreaStrategyFactory::~EntityOperationsAreaStrategyFactory()
{

}


IEntityOperationsAreaStrategy* EntityOperationsAreaStrategyFactory::getEntityOperationsAreaStrategy(RadioResourceType type)
{
	IEntityOperationsAreaStrategy* pEntityOperationsAreaStrategy = NULL;

	switch(type)
	{
		case RR_DYNGROUP:
		{
			DynamicRegroupEntityOperationsAreaStrategy* pDynamicStrategy = new DynamicRegroupEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pDynamicStrategy;
			break;
		}
		case RR_MSELECT:
		{
			MultiselectEntityOperationsAreaStrategy* pMultiselectStrategy = new MultiselectEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pMultiselectStrategy;
			break;
		}

		//This case is missed in design-notes & is added here;
		case RR_ANNGROUP:
		{
			AnnouncementGroupEntityOperationsAreaStrategy* pAnnouncementStrategy = new AnnouncementGroupEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pAnnouncementStrategy;
			break;
		}

		case RR_MSITE:
		{
			MultiSiteOperationsAreaStrategy* pMultiSiteStrategy = new MultiSiteOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pMultiSiteStrategy;
			break;
		}
		case RR_PATCHGROUP:
		{
			PatchGroupEntityOperationsAreaStrategy* pPatchStrategy = new PatchGroupEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pPatchStrategy;
			break;
		}
		case RR_RADIO:
		{
			RadioEntityOperationsAreaStrategy* pRadioStrategy = new RadioEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pRadioStrategy;
			break;
		}
		case RR_SITE:
		{
			SiteEntityOperationsAreaStrategy* pSiteStrategy = new SiteEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pSiteStrategy;
			break;
		}
		case RR_TALKGROUP:
		{
			TalkGroupEntityOperationsAreaStrategy* pTalkGrpStrategy = new TalkGroupEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pTalkGrpStrategy;
			break;
		}
		default:
		{
			EmptyEntityOperationsAreaStrategy* pEmptyStrategy = new EmptyEntityOperationsAreaStrategy();
			pEntityOperationsAreaStrategy = pEmptyStrategy;
		}
	}

	return pEntityOperationsAreaStrategy;
}


