/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/SiteDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_SITEDIRECTORYPAGESTRATEGY_H__E224DE3A_6FAE_4277_AD89_AA98CE9CDC26__INCLUDED_)
#define AFX_SITEDIRECTORYPAGESTRATEGY_H__E224DE3A_6FAE_4277_AD89_AA98CE9CDC26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.h"

class SiteDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy 
{
	public:
		SiteDirectoryPageStrategy();
		virtual ~SiteDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);


	private:
        //disable method
        SiteDirectoryPageStrategy (SiteDirectoryPageStrategy& radioObj);
        void operator=(const SiteDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_SITEDIRECTORYPAGESTRATEGY_H__E224DE3A_6FAE_4277_AD89_AA98CE9CDC26__INCLUDED_)
