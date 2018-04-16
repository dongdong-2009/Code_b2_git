/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/RadioDirectoryPageDlgStrategyFactory.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/RadioDirectoryPageDlgStrategyFactory.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/RadioDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/TalkGroupDirectoryPageStrategy.h"
//#include "app/radio/radio_manager_gui/src/RadioDirectory/AnnouncementGroupDirectoryPanelStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/PatchGroupDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/DynamicGroupDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/MultiSelectDirectoryPageStrategy.h"
//#include "app/radio/radio_manager_gui/src/RadioDirectory/SiteDirectoryPageStrategy.h"
//#include "app/radio/radio_manager_gui/src/RadioDirectory/MultiSiteDirectoryPageStrategy.h"


RadioDirectoryPageDlgStrategyFactory::RadioDirectoryPageDlgStrategyFactory()
{

}

RadioDirectoryPageDlgStrategyFactory::~RadioDirectoryPageDlgStrategyFactory()
{

}


IRadioDirectoryPageDlgStrategy* RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(RadioResourceType type)
{
	IRadioDirectoryPageDlgStrategy* pDirectoryStrategy;
	switch(type)
	{
		case RR_RADIO: 
		{
			RadioDirectoryPageStrategy* pRadio = new RadioDirectoryPageStrategy();
			pDirectoryStrategy = pRadio;
		}
		break;
		case RR_TALKGROUP:
		{
			TalkGroupDirectoryPageStrategy* pTalkGroup = new TalkGroupDirectoryPageStrategy();
			pDirectoryStrategy = pTalkGroup;
		}
		break;
		//case RR_ANNGROUP: 
		//{
		//	AnnouncementGroupDirectoryPanelStrategy* pAnnGroup = new AnnouncementGroupDirectoryPanelStrategy();
		//	pDirectoryStrategy = pAnnGroup;
		//}
		//break;
		case RR_PATCHGROUP: 
		{
			PatchGroupDirectoryPageStrategy* pPatchGroup = new PatchGroupDirectoryPageStrategy();
			pDirectoryStrategy = pPatchGroup;
		}
		break;
		case RR_DYNGROUP: 
		{
			DynamicGroupDirectoryPageStrategy* pDynamic = new DynamicGroupDirectoryPageStrategy();
			pDirectoryStrategy = pDynamic;
		}
		break;
		case RR_MSELECT: 
		{
			MultiSelectDirectoryPageStrategy* pMultiSelect = new MultiSelectDirectoryPageStrategy();
			pDirectoryStrategy = pMultiSelect;
		}
		break;
		//case RR_SITE: 
		//{
		//	SiteDirectoryPageStrategy* pSite= new SiteDirectoryPageStrategy();
		//	pDirectoryStrategy = pSite;
		//}
		//break;
		//case RR_MSITE: 
		//{
		//	MultiSiteDirectoryPageStrategy* pMultiSite = new MultiSiteDirectoryPageStrategy();
		//	pDirectoryStrategy = pMultiSite;
		//}
		//break;			
			
		default:
			pDirectoryStrategy = NULL;

	}

	return pDirectoryStrategy;	
}