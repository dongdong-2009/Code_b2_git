/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/DynamicGroupDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_DYNAMICGROUPDIRECTORYPAGESTRATEGY_H__92435C9B_E4D9_4E66_8E92_80EC54754498__INCLUDED_)
#define AFX_DYNAMICGROUPDIRECTORYPAGESTRATEGY_H__92435C9B_E4D9_4E66_8E92_80EC54754498__INCLUDED_

#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DynamicGroupDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy 
{
	public:
		DynamicGroupDirectoryPageStrategy();
		virtual ~DynamicGroupDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);


	private:
        //disable method
        DynamicGroupDirectoryPageStrategy (DynamicGroupDirectoryPageStrategy& radioObj);
        void operator=(const DynamicGroupDirectoryPageStrategy& radioObj);
};

#endif // !defined(AFX_DYNAMICGROUPDIRECTORYPAGESTRATEGY_H__92435C9B_E4D9_4E66_8E92_80EC54754498__INCLUDED_)
