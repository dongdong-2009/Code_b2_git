/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/CResourceButton.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/IResourceButtonContainer.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/SameWidthBorderDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/AbstractDecoratorButton.h"
#include "bus/mfc_extensions/src/decorator_button/CDecoratorButton.h"
#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/BackgroundDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/BorderDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/BitmapDecorator.h"
#include "bus/mfc_extensions/src/decorator_button/TextDecorator.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_manager_gui/src/resource.h"

#include <set>


RadioResourceButtonStrategy::RadioResourceButtonStrategy()
{
	m_pResourceButton = NULL;
}


RadioResourceButtonStrategy::~RadioResourceButtonStrategy()
{
	// cleanup decorators
	if (NULL != m_pResourceButton) {
		m_pResourceButton->deleteTree();
	}

}


void RadioResourceButtonStrategy::initialise(CResourceButton* pResourceButton)
{
	FUNCTION_ENTRY("initialise");

	m_pResourceButton = pResourceButton;

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}
    
	reDraw();

    // do not set selected button state for patch group, mselect & msite.
    // selected state change is not handled properly yet for these types
	if (m_pResourceButton->m_RadioResource.type != RR_NIL &&
        m_pResourceButton->m_RadioResource.type != RR_PATCHGROUP &&
        m_pResourceButton->m_RadioResource.type != RR_MSELECT &&
        m_pResourceButton->m_RadioResource.type != RR_MSITE) 
	{
		if (The_RadioSystemStatus::instance()->getResource_isSelected(m_pResourceButton->m_RadioResource))
		{
			m_pResourceButton->setCheck(true);
		}

        // check for private call status
        //if (m_pResourceButton->m_RadioResource.type == RR_RADIO)
        //{
        //    PrivateCallState cState = The_RadioSystemStatus::instance()->getResource_isInPrivateCall(m_pResourceButton->m_RadioResource);
        //    if (cState == PC_INCOMING)
        //    {
        //        inboundCallRinging();
        //    }
        //    else if (cState == PC_ESTABLISHED)
        //    {
        //        inboundCall();
        //    }

        //}

		if (The_RadioSystemStatus::instance()->getResource_isEmergencyMode(m_pResourceButton->m_RadioResource) != RE_NO_EMERGENCY)
		{
			resourceEmergency();	
		}
	}

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::leftClickBehaviour()
{
	FUNCTION_ENTRY("leftClickBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	bool ch = m_pResourceButton->isChecked();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "leftClickBehaviour, resource id = %u", m_pResourceButton->m_RadioResource.id);

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Button check = %u", ch);

	if( ch )
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Button check = %u", m_pResourceButton->isChecked());
		unselectionBehaviour();
		//m_pResourceButton->setCheck(0);
	}
	else
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Button check = %u", m_pResourceButton->isChecked());
		// peter.wong, selectionBehavior is called after all other buttons are not in pressed state.
		//selectionBehaviour();
		m_pResourceButton->setPressInProcess();
		//m_pResourceButton->setCheck(1);
	}

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::rightClickBehaviour()
{
	FUNCTION_ENTRY("rightClickBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	CRect rect;
	m_pResourceButton->GetWindowRect(&rect);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(0,ID_STOP_MONITOR_MENU, "Stop Monitoring");

   // display the popup menu

    CPoint mousePoint;
    ::GetCursorPos(&mousePoint);

	menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, m_pResourceButton, rect);

	// When "Stop Monitoring" is selected from the popup menu,
	// implementation is done in CResourceButton::OnStopMonitorMenu() 

	menu.DestroyMenu();

	FUNCTION_EXIT;

}


void RadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	//m_pResourceButton->setCheck(true);

	The_RadioOperations::instance()->entitySelectionChange(m_pResourceButton->m_RadioResource, true);

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	//m_pResourceButton->setCheck(false);

	The_RadioOperations::instance()->entitySelectionChange( m_pResourceButton->m_RadioResource, false );

	FUNCTION_EXIT;
}

int RadioResourceButtonStrategy::assignmentBehaviour(bool pOnInit)
{
	FUNCTION_ENTRY("assignmentBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return -1;
	}

    if (!pOnInit)
    {
	    The_RadioSystemStatus::instance()->addResourceButtonConfig( 
			    m_pResourceButton->m_RadioResource, 
			    m_pResourceButton->m_isProfileButton,
			    m_pResourceButton->m_buttonRef);
    }
    else
    {
		The_StatusManager::instance()->setFeatureAttributeButton(
            m_pResourceButton->m_RadioResource, 
            m_pResourceButton->m_isProfileButton != 0, 
            m_pResourceButton->m_buttonRef);
    }

	FUNCTION_EXIT;

	return 0;
}


void RadioResourceButtonStrategy::inboundCallRinging()
{
	FUNCTION_ENTRY("inboundCall");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	// add background decorator
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BACKGROUND);
	m_pResourceButton->m_isActive = false;

	// to receive timer notifications for flashing
	m_pResourceButton->m_pResourceButtonContainer->addTimerObserver(this->m_pResourceButton);

	reDraw();
	
	FUNCTION_EXIT;
}

void RadioResourceButtonStrategy::inboundCall()
{
	FUNCTION_ENTRY("inboundCall");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

    m_pResourceButton->m_isFlashOff = true;
	m_pResourceButton->m_isActive = true;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting radio button %s to green", m_pResourceButton->m_RadioResource.alias.c_str());
	// add background decorator
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BACKGROUND);

	reDraw(); 

	FUNCTION_EXIT;
}



void RadioResourceButtonStrategy::inboundCallCanceled()
{
	FUNCTION_ENTRY("inboundCallCanceled");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

    m_pResourceButton->m_isFlashOff = true;
	m_pResourceButton->m_isActive = false;

    // remove background color
	std::set<ButtonDecoratorType>::iterator bdtSetIter = m_pResourceButton->m_appliedDecoratorTypesCollection.find(DT_BACKGROUND);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting radio button %s to gray", m_pResourceButton->m_RadioResource.alias.c_str());
	if( bdtSetIter != m_pResourceButton->m_appliedDecoratorTypesCollection.end() )
	{
		m_pResourceButton->m_appliedDecoratorTypesCollection.erase( bdtSetIter );
	}

	reDraw(); 

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::selectSpeakerSelected()
{
	FUNCTION_ENTRY("selectSpeakerSelected");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	m_pResourceButton->setCheck(true);

	FUNCTION_EXIT;
}



void RadioResourceButtonStrategy::selectSpeakerUnselected()
{
	FUNCTION_ENTRY("selectSpeakerUnselected");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	m_pResourceButton->setCheck(false);

	FUNCTION_EXIT;
}



void RadioResourceButtonStrategy::unselectSpeakerDeAssignment()
{
	FUNCTION_ENTRY("unselectSpeakerDeAssignment");

	removeAssignedResource();

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::resourceActivity()
{
	FUNCTION_ENTRY("resourceActivity");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

    m_pResourceButton->m_isFlashOff = true;
	m_pResourceButton->m_isActive = true;

	// add background decorator
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BACKGROUND);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting radio button %s to green", m_pResourceButton->m_RadioResource.alias.c_str());

	reDraw();

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::resourceActivityEnded()
{
	FUNCTION_ENTRY("resourceActivityEnded");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	m_pResourceButton->m_isActive = false;

	std::set<ButtonDecoratorType>::iterator bdtSetIter = m_pResourceButton->m_appliedDecoratorTypesCollection.find(DT_BACKGROUND);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting radio button %s to gray", m_pResourceButton->m_RadioResource.alias.c_str());
	if( bdtSetIter != m_pResourceButton->m_appliedDecoratorTypesCollection.end() )
	{
		m_pResourceButton->m_appliedDecoratorTypesCollection.erase( bdtSetIter );
	}

	reDraw(); 

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::resourceEmergency()
{
	FUNCTION_ENTRY("resourceEmergency");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	// add border decorator
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BORDER);

	// to receive timer notifications for flashing
	m_pResourceButton->m_pResourceButtonContainer->addTimerObserver(this->m_pResourceButton);


	reDraw();

	FUNCTION_EXIT;
}



void RadioResourceButtonStrategy::resourceEmergencyAcknowledged()
{
	FUNCTION_ENTRY("resourceEmergencyAcknowledged");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}


	// disable receiving of timer notifications
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BORDER);
	m_pResourceButton->m_pResourceButtonContainer->removeTimerObserver(this->m_pResourceButton);

	// stop flashing
	m_pResourceButton->m_isFlashOff = true;

	reDraw();


	FUNCTION_EXIT;
}



void RadioResourceButtonStrategy::resourceEmergencyEnded()
{
	FUNCTION_ENTRY("resourceEmergencyEnded");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	std::set<ButtonDecoratorType>::iterator bdtSetIter = m_pResourceButton->m_appliedDecoratorTypesCollection.find(DT_BORDER);
	if( bdtSetIter != m_pResourceButton->m_appliedDecoratorTypesCollection.end() )
	{
		m_pResourceButton->m_appliedDecoratorTypesCollection.erase( bdtSetIter );
	}

	// if (RadioResource not isSelected)
//	if(!The_RadioSystemStatus::instance()->getResource_isSelected(m_pResourceButton->m_RadioResource))
//	{
		m_pResourceButton->m_pResourceButtonContainer->removeTimerObserver(this->m_pResourceButton);
    	m_pResourceButton->m_isFlashOff = true;
		m_pResourceButton->m_isActive = false;  // lichao ++
		m_pResourceButton->m_isAckEmergency = false;  // lichao ++
		reDraw();
//	}

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::reDraw()
{
	FUNCTION_ENTRY("reDraw");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	//Clear decorators;
	CDecoratorButton* undecoratedButton = m_pResourceButton->undecoratedResourceButton();
	
	m_pResourceButton->m_pFinalDecorator = undecoratedButton;

	// add common decorators for all resources ( Text & Bitmap )
	commonDecoration();


    std::set<ButtonDecoratorType>::iterator bdtSetIter;

    bdtSetIter = m_pResourceButton->m_appliedDecoratorTypesCollection.begin();

    //re-apply the set of decorators;
    for( ; bdtSetIter != m_pResourceButton->m_appliedDecoratorTypesCollection.end(); ++bdtSetIter )
    {
        applyDecorator( (*bdtSetIter) );
    }

	m_pResourceButton->RedrawWindow();

	FUNCTION_EXIT;
}


void RadioResourceButtonStrategy::commonDecoration()
{
	FUNCTION_ENTRY("commonDecoration");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	//Append DT_BITMAP from enum struct: ButtonDecoratorType;
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_BITMAP);

	//Append DT_TEXT from enum struct: ButtonDecoratorType;	
	m_pResourceButton->m_appliedDecoratorTypesCollection.insert(DT_TEXT);

	FUNCTION_EXIT;

}


void RadioResourceButtonStrategy::applyDecorator(ButtonDecoratorType buttonDecoratorType)
{
	FUNCTION_ENTRY("applyDecorator");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}


	switch( buttonDecoratorType )
	{
	 case DT_BACKGROUND:
		{

			TA_Base_Bus::BackgroundDecorator* pBkDecorator = new TA_Base_Bus::BackgroundDecorator(m_pResourceButton->m_pFinalDecorator);

			if (m_pResourceButton->m_isActive || m_pResourceButton->m_isFlashOff == true)
            {
				pBkDecorator->setBackgroundColor(RGB(0, 255, 0));
			}
			else
			{	
				pBkDecorator->setBackgroundColor(::GetSysColor(COLOR_BTNFACE));
			}
		}
		break;

	 case DT_BORDER:
		{
			if (m_pResourceButton->m_isAckEmergency)
            {
				// peter.wong, DTL-366, can also extend BorderDecorator class (better?)
				// extend BorderDecorator, otherwise can cause intermittent bug
				SameWidthBorderDecorator* pBorderDecorator = new SameWidthBorderDecorator(m_pResourceButton->m_pFinalDecorator);
    			//TA_Base_Bus::BorderDecorator* pBorderDecorator = new TA_Base_Bus::BorderDecorator(m_pResourceButton->m_pFinalDecorator);
				pBorderDecorator->setBorderProperties(RGB(255,0,0),3);
				//if (m_pResourceButton->isChecked()) pBorderDecorator->setBorderProperties(RGB(255, 0, 0),3); 
				//else pBorderDecorator->setBorderProperties(RGB(255, 0, 0),3); 
			}
            else
            {
			    if (m_pResourceButton->m_isFlashOff == false ) 
			    {	
					// peter.wong, DTL-366
					SameWidthBorderDecorator* pBorderDecorator = new SameWidthBorderDecorator(m_pResourceButton->m_pFinalDecorator);
    			    //TA_Base_Bus::BorderDecorator* pBorderDecorator = new TA_Base_Bus::BorderDecorator(m_pResourceButton->m_pFinalDecorator);
					pBorderDecorator->setBorderProperties(RGB(255,0,0),3);
					//if (m_pResourceButton->isChecked()) pBorderDecorator->setBorderProperties(RGB(255, 0, 0),6); 
					//else pBorderDecorator->setBorderProperties(RGB(255, 0, 0),3); 
			    }
            }
		}
		break;

	 case DT_BITMAP:
		//{
		//	CBitmap* lbmp = new CBitmap();
		//	lbmp->LoadBitmap( getBitmapForType(m_pResourceButton->m_RadioResource.type) );
		//	
		//	TA_Base_Bus::BitmapDecorator* pBitmapDecorator = new TA_Base_Bus::BitmapDecorator(m_pResourceButton->m_pFinalDecorator);
		//	pBitmapDecorator->setBitmap(lbmp,3,3);
		//}
		break;

	 case DT_TEXT:
		{
			TA_Base_Bus::TextDecorator* pTextDecorator = new TA_Base_Bus::TextDecorator(m_pResourceButton->m_pFinalDecorator);
			//pTextDecorator->setTextPosition(5,5);
			pTextDecorator->setTextColor(RGB(0,0,0)); // Black
		}
		break;
	}

	FUNCTION_EXIT;
}


bool RadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return false;
	}

	m_pResourceButton->m_RadioResource.initialise();

	//Static function to create an empty button;
	IResourceButtonStrategy *pTempResourceButtonStrategy  = ResourceButtonStrategyFactory::getResourceButtonStrategy( m_pResourceButton->m_RadioResource.type );

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);

	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);

	FUNCTION_EXIT;
    return true;
}


int RadioResourceButtonStrategy::getBitmapForType(RadioResourceType radioResourceType)
{
	FUNCTION_ENTRY("getBitmapForType");

	//Invalid bitmap ID;
	int resourceBitmapID = 0;
	switch( radioResourceType ) 
	{
	case RR_NIL:
		break;

	//case RR_RADIO:
	//	resourceBitmapID = IDB_BITMAP_Rad;
	//	break;

	case RR_TALKGROUP:
		resourceBitmapID = IDB_BITMAP_TG;
		break;

	//case RR_ANNGROUP:
	//	resourceBitmapID = IDB_BITMAP_AG;
	//	break;

	case RR_PATCHGROUP:
		resourceBitmapID = IDB_BITMAP_PG;
		break;

	//case RR_DYNGROUP:
	//	resourceBitmapID = IDB_BITMAP_DG;
	//	break;

	case RR_MSELECT:
		resourceBitmapID = IDB_BITMAP_MS;
		break;

	//case RR_SITE:
	//	resourceBitmapID = IDB_BITMAP_SW;
	//	break;

	//case RR_MSITE:
	//	resourceBitmapID = IDB_BITMAP_Mu_SW;
	//	break;
	
	//case RR_TRAIN:
	//	break;
	default:
		break;
	}

	FUNCTION_EXIT;

	return resourceBitmapID;
}

void RadioResourceButtonStrategy::dropBehaviour(HDROP hDropInfo)
{
	FUNCTION_ENTRY("dropBehaviour");

	FUNCTION_EXIT;
}

void RadioResourceButtonStrategy::cancelCurrentOperation(ApiState state)
{
	FUNCTION_ENTRY("cancelCurrentOperation");

	FUNCTION_EXIT;
}


bool RadioResourceButtonStrategy::canDropTarget()
{
	FUNCTION_ENTRY("canDropTarget");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return false;
	}

	bool bDrop = true;

	if (m_pResourceButton->m_RadioResource.type != RR_NIL)
	{
		bDrop = false;
	}
		
	FUNCTION_EXIT;

	return bDrop;
}


void RadioResourceButtonStrategy::resourceAssignment()
{
	FUNCTION_ENTRY("resourceAssignment");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy( m_pResourceButton->m_RadioResource.type );
	TA_ASSERT(pTempResourceButtonStrategy != NULL, "RadioResourceButtonStrategy is NULL");

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);
		
	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);

	if (!m_pResourceButton->m_RadioResource.alias.empty())
	{
		m_pResourceButton->SetWindowText(m_pResourceButton->m_RadioResource.alias.c_str());
	}
	else {
		m_pResourceButton->SetWindowText("");
	}
	
	
	FUNCTION_EXIT;

}


void RadioResourceButtonStrategy::resourceDeAssignment()
{
	FUNCTION_ENTRY("resourceDeAssignment");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	m_pResourceButton->m_RadioResource.initialise();

	IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(m_pResourceButton->m_RadioResource.type);
	TA_ASSERT(pTempResourceButtonStrategy != NULL, "RadioResourceButtonStrategy strategy");

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);
		
	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);

	FUNCTION_EXIT;
}

