/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/MultiSiteDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_MULTISITEDIRECTORYPAGESTRATEGY_H__683B3C6B_2EC7_44FE_BE54_A519407A361D__INCLUDED_)
#define AFX_MULTISITEDIRECTORYPAGESTRATEGY_H__683B3C6B_2EC7_44FE_BE54_A519407A361D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.h"

class MultiSiteDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy 
{
	public:
		MultiSiteDirectoryPageStrategy();
		virtual ~MultiSiteDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);


	private:
        //disable method
        MultiSiteDirectoryPageStrategy (MultiSiteDirectoryPageStrategy& radioObj);
        void operator=(const MultiSiteDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_MULTISITEDIRECTORYPAGESTRATEGY_H__683B3C6B_2EC7_44FE_BE54_A519407A361D__INCLUDED_)
