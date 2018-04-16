/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/RadioDirectoryPageStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/RadioDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

RadioDirectoryPageStrategy::RadioDirectoryPageStrategy()
{
	m_CRadioDirectoryPageDlg = NULL;
}

RadioDirectoryPageStrategy::~RadioDirectoryPageStrategy()
{

}


void RadioDirectoryPageStrategy::initialise(CRadioDirectoryPageDlg& radioDirectoryPageDlg)
{
  
	radioDirectoryPageDlg.m_ResourceTypeCollectionarray = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_RADIO);

	radioDirectoryPageDlg.setFilterListRowData();
	
	m_CRadioDirectoryPageDlg = &radioDirectoryPageDlg;

}

