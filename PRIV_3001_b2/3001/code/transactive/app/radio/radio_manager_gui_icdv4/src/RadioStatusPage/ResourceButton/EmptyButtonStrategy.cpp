/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/EmptyButtonStrategy.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"




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
	CDecoratorButton* undecoratedButton = m_pResourceButton->undecoratedResourceButton();
	
	m_pResourceButton->m_pFinalDecorator = undecoratedButton;

	m_pResourceButton->setCheck(false);
}


void EmptyButtonStrategy::resourceAssignment()
{
    FUNCTION_ENTRY("resourceAssignment");

	IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy( m_pResourceButton->m_RadioResource.type );

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);

	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
    FUNCTION_EXIT;
}


