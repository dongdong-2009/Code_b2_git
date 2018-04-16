/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/MultiSelectDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_MULTISELECTDIRECTORYPAGESTRATEGY_H__D2D92303_8328_4AFF_9B8B_6FD766A994A2__INCLUDED_)
#define AFX_MULTISELECTDIRECTORYPAGESTRATEGY_H__D2D92303_8328_4AFF_9B8B_6FD766A994A2__INCLUDED_

#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MultiSelectDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy 
{
	public:

		MultiSelectDirectoryPageStrategy();
		virtual ~MultiSelectDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);


	private:

        //disable method
        MultiSelectDirectoryPageStrategy (MultiSelectDirectoryPageStrategy& radioObj);
        void operator=(const MultiSelectDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_MULTISELECTDIRECTORYPAGESTRATEGY_H__D2D92303_8328_4AFF_9B8B_6FD766A994A2__INCLUDED_)
