/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/DynamicGroupDirectoryPageStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/DynamicGroupDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"


DynamicGroupDirectoryPageStrategy::DynamicGroupDirectoryPageStrategy()
{
	m_CRadioDirectoryPageDlg = NULL;
}

DynamicGroupDirectoryPageStrategy::~DynamicGroupDirectoryPageStrategy()
{

}


void DynamicGroupDirectoryPageStrategy::initialise(CRadioDirectoryPageDlg& radioDirectoryPageDlg)
{
	FUNCTION_ENTRY("initialise");
	radioDirectoryPageDlg.m_ResourceTypeCollectionarray = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_DYNGROUP);	

	std::sort(radioDirectoryPageDlg.m_ResourceTypeCollectionarray.begin(), radioDirectoryPageDlg.m_ResourceTypeCollectionarray.end(), SortAliasAsc());

	radioDirectoryPageDlg.setFilterListRowData();
	
	m_CRadioDirectoryPageDlg = &radioDirectoryPageDlg;
	FUNCTION_EXIT;
}

