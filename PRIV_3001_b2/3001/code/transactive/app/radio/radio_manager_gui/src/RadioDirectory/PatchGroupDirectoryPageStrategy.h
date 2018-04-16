/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/PatchGroupDirectoryPageStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_PATCHGROUPDIRECTORYPAGESTRATEGY_H__ECCC066D_1518_4248_836A_A84E41A1B0E5__INCLUDED_)
#define AFX_PATCHGROUPDIRECTORYPAGESTRATEGY_H__ECCC066D_1518_4248_836A_A84E41A1B0E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h"

class PatchGroupDirectoryPageStrategy : public IRadioDirectoryPageDlgStrategy
{
	public:
		PatchGroupDirectoryPageStrategy();
		virtual ~PatchGroupDirectoryPageStrategy();

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg);

	private:
        //disable method
        PatchGroupDirectoryPageStrategy (PatchGroupDirectoryPageStrategy& radioObj);
        void operator=(const PatchGroupDirectoryPageStrategy& radioObj);

};

#endif // !defined(AFX_PATCHGROUPDIRECTORYPAGESTRATEGY_H__ECCC066D_1518_4248_836A_A84E41A1B0E5__INCLUDED_)
