/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/RadioDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_RADIODIRECTORYPAGESTRATEGY_H__AECA570F_2C5C_402B_A41C_DE5B9F8C1288__INCLUDED_)
#define AFX_RADIODIRECTORYPAGESTRATEGY_H__AECA570F_2C5C_402B_A41C_DE5B9F8C1288__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.h"


class RadioDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy
{
	public:

		RadioDirectoryPageStrategy();
		virtual ~RadioDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPageDlg);

	private:
        //disable method
        RadioDirectoryPageStrategy (RadioDirectoryPageStrategy& radioObj);
        void operator=(const RadioDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_RADIODIRECTORYPAGESTRATEGY_H__AECA570F_2C5C_402B_A41C_DE5B9F8C1288__INCLUDED_)
