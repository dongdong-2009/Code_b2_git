/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/TalkGroupDirectoryPageStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/TalkGroupDirectoryPageStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"

TalkGroupDirectoryPageStrategy::TalkGroupDirectoryPageStrategy()
{
	m_CRadioDirectoryPageDlg = NULL;
}

TalkGroupDirectoryPageStrategy::~TalkGroupDirectoryPageStrategy()
{

}


void TalkGroupDirectoryPageStrategy::initialise(CRadioDirectoryPageDlg& radioDirectoryPageDlg)
{
	radioDirectoryPageDlg.m_ResourceTypeCollectionarray = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_TALKGROUP);	

	std::sort(radioDirectoryPageDlg.m_ResourceTypeCollectionarray.begin(), radioDirectoryPageDlg.m_ResourceTypeCollectionarray.end(), SortAliasAsc());

	radioDirectoryPageDlg.setFilterListRowData();

	m_CRadioDirectoryPageDlg = &radioDirectoryPageDlg;
}

