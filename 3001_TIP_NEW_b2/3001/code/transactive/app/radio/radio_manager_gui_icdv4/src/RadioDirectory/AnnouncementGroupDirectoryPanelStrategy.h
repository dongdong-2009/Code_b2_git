/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/AnnouncementGroupDirectoryPanelStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_ANNOUNCEMENTGROUPDIRECTORYPANELSTRATEGY_H__EF820F8B_677E_49E7_A822_DC8D6BB44EE6__INCLUDED_)
#define AFX_ANNOUNCEMENTGROUPDIRECTORYPANELSTRATEGY_H__EF820F8B_677E_49E7_A822_DC8D6BB44EE6__INCLUDED_

#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AnnouncementGroupDirectoryPanelStrategy : public IRadioDirectoryPageDlgStrategy
{
	public:
		AnnouncementGroupDirectoryPanelStrategy();
		virtual ~AnnouncementGroupDirectoryPanelStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);
	

	private:
        //disable method
        AnnouncementGroupDirectoryPanelStrategy (AnnouncementGroupDirectoryPanelStrategy& radioObj);
        void operator=(const AnnouncementGroupDirectoryPanelStrategy& radioObj);	
};

#endif // !defined(AFX_ANNOUNCEMENTGROUPDIRECTORYPANELSTRATEGY_H__EF820F8B_677E_49E7_A822_DC8D6BB44EE6__INCLUDED_)
