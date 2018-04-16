/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/TalkGroupDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_TALKGROUPDIRECTORYPAGESTRATEGY_H__514CCD06_E45D_4B4C_951D_3215C6D782AF__INCLUDED_)
#define AFX_TALKGROUPDIRECTORYPAGESTRATEGY_H__514CCD06_E45D_4B4C_951D_3215C6D782AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"


class TalkGroupDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy
{
	public:
		TalkGroupDirectoryPageStrategy();
		virtual ~TalkGroupDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);


	private:
        //disable method
        TalkGroupDirectoryPageStrategy (TalkGroupDirectoryPageStrategy& radioObj);
        void operator=(const TalkGroupDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_TALKGROUPDIRECTORYPAGESTRATEGY_H__514CCD06_E45D_4B4C_951D_3215C6D782AF__INCLUDED_)
