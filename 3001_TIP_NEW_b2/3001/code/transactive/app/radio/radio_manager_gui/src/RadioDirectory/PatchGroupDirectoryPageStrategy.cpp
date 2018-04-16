/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/PatchGroupDirectoryPageStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/PatchGroupDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"


PatchGroupDirectoryPageStrategy::PatchGroupDirectoryPageStrategy()
{
	m_CRadioDirectoryPageDlg = NULL;
}

PatchGroupDirectoryPageStrategy::~PatchGroupDirectoryPageStrategy()
{

}


void PatchGroupDirectoryPageStrategy::initialise(CRadioDirectoryPageDlg& radioDirectoryPageDlg)
{
	radioDirectoryPageDlg.m_ResourceTypeCollectionarray = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_PATCHGROUP);

	std::sort(radioDirectoryPageDlg.m_ResourceTypeCollectionarray.begin(), radioDirectoryPageDlg.m_ResourceTypeCollectionarray.end(), SortAliasAsc());

	radioDirectoryPageDlg.setFilterListRowData();

	m_CRadioDirectoryPageDlg = &radioDirectoryPageDlg;

}


