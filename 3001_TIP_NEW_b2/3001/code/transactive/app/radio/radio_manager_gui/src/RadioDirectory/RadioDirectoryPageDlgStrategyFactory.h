/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/RadioDirectoryPageDlgStrategyFactory.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IDirectoryPanelDlgStrategy type for the
 * RadioResouceType
 */

#if !defined(AFX_RADIODIRECTORYPAGEDLGSTRATEGYFACTORY_H__4699497E_1855_434C_A3F6_DF4E8FD6C93C__INCLUDED_)
#define AFX_RADIODIRECTORYPAGEDLGSTRATEGYFACTORY_H__4699497E_1855_434C_A3F6_DF4E8FD6C93C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"


class RadioDirectoryPageDlgStrategyFactory  
{
	public:
		
		RadioDirectoryPageDlgStrategyFactory();
		virtual ~RadioDirectoryPageDlgStrategyFactory();


	public:
		/**
		 * Constructs the appropriate strategy and returns it
		 */
		static IRadioDirectoryPageDlgStrategy* getRadioDirectoryPageDlgStrategy(RadioResourceType type);


	private:
        //disable method
        RadioDirectoryPageDlgStrategyFactory (RadioDirectoryPageDlgStrategyFactory& radioObj);
        void operator=(const RadioDirectoryPageDlgStrategyFactory& radioObj);
};

#endif // !defined(AFX_RADIODIRECTORYPAGEDLGSTRATEGYFACTORY_H__4699497E_1855_434C_A3F6_DF4E8FD6C93C__INCLUDED_)
