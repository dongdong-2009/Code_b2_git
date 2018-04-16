/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This is the common interface for the CRadioDirectoryPanelDlg strategy. These
 * strategies will define the behaviour of the CRadioDirectoryPanelDlg for
 * different Resource types. The only operation thas has to be explicitly
 * implemented on all strategies is the Initialise operation. All other operations
 * can either be overloaded with an empty stub, or the interface method itself may
 * be stubbed.
 */

#if !defined(AFX_IRADIODIRECTORYPAGEDLGSTRATEGY_H__0C60A94E_A6A1_4ED7_92DD_3BA74B5B9612__INCLUDED_)
#define AFX_IRADIODIRECTORYPAGEDLGSTRATEGY_H__0C60A94E_A6A1_4ED7_92DD_3BA74B5B9612__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRadioDirectoryPageDlg;

class IRadioDirectoryPageDlgStrategy  
{
	public:
		
		virtual ~IRadioDirectoryPageDlgStrategy() {};

		/**
		 * Initialize the tab by populating the directory list based on the selected tab.
		 */
		virtual void initialise(CRadioDirectoryPageDlg& radioDirectoryPanelDlg) = 0;


	
	public:

		CRadioDirectoryPageDlg* m_CRadioDirectoryPageDlg;

};



#endif // !defined(AFX_IRADIODIRECTORYPAGEDLGSTRATEGY_H__0C60A94E_A6A1_4ED7_92DD_3BA74B5B9612__INCLUDED_)
