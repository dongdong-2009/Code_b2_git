/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"

#include "app/radio/radio_manager_gui/src/CRadioManagerDlg.h"

#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"




const int  EmptyButtonStrategy::INT_PROFILE_BUTTON = 1;

EmptyButtonStrategy::EmptyButtonStrategy()
{
	m_pResourceButton = NULL;
}


EmptyButtonStrategy::~EmptyButtonStrategy()
{
}


void EmptyButtonStrategy::initialise(CResourceButton* pResourceButton)
{
	FUNCTION_ENTRY("initialise");

	m_pResourceButton = pResourceButton;

	//Has yet to define the behavior for EmptyButtonStrategy;
	reDraw();

	FUNCTION_EXIT;
}


void EmptyButtonStrategy::dropBehaviour(HDROP hDropInfo)
{
	FUNCTION_ENTRY("dropBehaviour");

	if( canDropTarget() == true )
	{
		//Static function to new a concrete button;
		IResourceButtonStrategy *pTempResourceButtonStrategy  = ResourceButtonStrategyFactory::getResourceButtonStrategy( m_pResourceButton->m_RadioResource.type);
		
		// validate window button text first..
		// revice if needed
		CString strAliasFilter = this->m_pResourceButton->m_RadioResource.alias.c_str();
		validateTextForButton (strAliasFilter);

		this->m_pResourceButton->SetWindowText(strAliasFilter);
		
		pTempResourceButtonStrategy->initialise(this->m_pResourceButton);

		pTempResourceButtonStrategy->assignmentBehaviour(false);

		//setResourceButtonStrategy();
		m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);
	}

	FUNCTION_EXIT;
}



bool EmptyButtonStrategy::canDropTarget()
{
	FUNCTION_ENTRY("canDropTarget");
	
	bool bHasRights = true;

	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	TA_ASSERT( pMainWnd != NULL, "pMainWind is NULL");	

	if ( m_pResourceButton->m_isProfileButton )
	{
		//Check rights for configuration 
		bHasRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_MONITOR_PROFILE_BTN); 
	}
	else
	{
		bHasRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_MONITOR_OPERATOR_BTN); 
	}
	
	FUNCTION_EXIT;
	return bHasRights;
}


void EmptyButtonStrategy::reDraw()
{	
	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	CDecoratorButton* undecoratedButton = m_pResourceButton->undecoratedResourceButton();
	
	m_pResourceButton->m_pFinalDecorator = undecoratedButton;

	m_pResourceButton->setCheck(false);
}


void EmptyButtonStrategy::resourceAssignment()
{
    FUNCTION_ENTRY("resourceAssignment");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}
	IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy( m_pResourceButton->m_RadioResource.type );

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);

	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
    FUNCTION_EXIT;
}

void EmptyButtonStrategy::validateTextForButton (CString & strAliasFilter)
{
	
	FUNCTION_ENTRY ("validateTextForButton");

	CString strText= strAliasFilter;
	CString strHotKeyIndicator = "&";
	
	int nNotFind = 0;
	int nIndex;
	
	std::vector<long> vctrIndex;
	CString strTempBuffer = strAliasFilter;

	long lRunningIndex = 0;

	while(-1 != nNotFind)
	{
		nNotFind = strTempBuffer.Find(strHotKeyIndicator,lRunningIndex);

		if (-1 != nNotFind)
		{
			vctrIndex.push_back(nNotFind);
			lRunningIndex = nNotFind + 1;
		}

	}

	for (int nCounter = 0; nCounter < vctrIndex.size(); nCounter++)
	{
		strText.Insert(vctrIndex.at(nCounter),'&');

		if (nCounter != vctrIndex.size()-1)
		{
			long lIndex = vctrIndex.at(nCounter + 1);
			vctrIndex.at(nCounter + 1) = lIndex + (nCounter + 1);
		}
	}
	
	strAliasFilter = strText;

	FUNCTION_EXIT;
}

