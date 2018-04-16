/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/Attic/CRadioStatusPage.cpp,v $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $Date: 2009/12/15 $
 * Last modified by:  $Author: grace.koh $
 * 
 */



#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#pragma once




#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/Resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/CRadioStatusPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"

#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



#include "bus/radio/Radio_Agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "bus/radio/radio_agent/idl/src/RadioCommonExceptionsIDL.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core\message\types\radiocomms_messagetypes.h"
#include "core/message/src/MessagePublicationManager.h"

#include <sstream>

//fixed TD14302
const int CRadioStatusPage::BUTTON_SPACE_H = 1; //2;
const int CRadioStatusPage::BUTTON_SPACE_V = 1; //2;
//fixed TD14302

const int CRadioStatusPage::MAX_PROFILE_BUTTONS = 12;
const int CRadioStatusPage::MAX_USERDEFINED_BUTTONS = 64;
int CRadioStatusPage::BUTTON_WIDTH = 97;
int CRadioStatusPage::BUTTON_HEIGHT = 24;
unsigned long CRadioStatusPage::BUTTON_REFERENCE_NUM = 1;

CRadioStatusPage::CRadioStatusPage(RECT* parentRect) : 
        CPropertyPage(CRadioStatusPage::IDD)
{
	FUNCTION_ENTRY("CRadioStatusPage");
	
    if (parentRect != NULL)
    {
        m_parentRect = *parentRect;
    }

    //{{AFX_DATA_INIT(CRadioStatusPage)
	//}}AFX_DATA_INIT 
	

	FUNCTION_EXIT;
}


CRadioStatusPage::~CRadioStatusPage()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_FAIL);
	clearButtons();
}

void CRadioStatusPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioStatusPage)
	//DDX_Control(pDX, IDC_STATIC_MONITOR, m_monitoredText);
	DDX_Control(pDX, IDC_STATIC_MONITOR3, m_cadText);
	DDX_Control(pDX, IDC_STATIC_MONITOR2, m_broadcastText);
	//DDX_Control(pDX, IDC_EMERGENCY_STATIC, m_emergencyText);
	//DDX_Control(pDX, IDC_EMERGENCY_BTN, m_emergencyBtn);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EMERGENCY_CHECK, m_emergencyBtn);
}


BEGIN_MESSAGE_MAP(CRadioStatusPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRadioStatusPage)
	ON_MESSAGE(WM_DATABASE_READY,onNotifiedDatabaseReady)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED, onNotifiedEntitySelectionChanged)
	ON_MESSAGE(WM_OPERATION_FAIL, onNotifiedOperationFail)
	ON_BN_CLICKED(IDC_EMERGENCY_CHECK, onEmergencyPressed)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()

	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRadioStatusPage::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (counter1 % 2 == 0) {
	RadioResource r;
	r.alias = "Conventional 1";
	r.id = 268435459;
	r.type = RR_TALKGROUP;
	The_RadioOperations::instance()->onInboundCallEvent(r);
	} else {
	RadioResource r;
	r.alias = "Conventional 1";
	r.id = 268435459;
	r.type = RR_TALKGROUP;
	The_RadioOperations::instance()->onInboundCallCanceledEvent(r);
	}
	counter1++;
}

void CRadioStatusPage::test()
{
    RECT rc;

    GetClientRect(&rc);
    char buff[100];
    sprintf(buff, "RadioStatus: %d, %d", rc.right, rc.bottom);
    AddDebugMessage(buff);
    
    sprintf(buff, "Parent: %d, %d", m_parentRect.right, m_parentRect.bottom);
    AddDebugMessage(buff);

	SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

BOOL CRadioStatusPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	RECT rc = m_parentRect;

	// TD14957 - dynamically compute the resource button size 
	BUTTON_WIDTH = (m_parentRect.right / 3) - 2;
	//BUTTON_HEIGHT = (BUTTON_WIDTH / 4) - 2;
	BUTTON_HEIGHT = (m_parentRect.bottom/23) - 2;

	SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	// Convert to client coordinates
	ScreenToClient (&rc);
	
	int xPos = 8;
	int yPos = 25; //25;

	//createProfileButtons(rc,xPos,yPos);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling createTalkgroupStatusButtons");
	createTalkgroupStatusButtons(rc,xPos,yPos);

	yPos += BUTTON_HEIGHT * 13 + 3 * BUTTON_HEIGHT / 4;
	CRect dummyRect;
	GetDlgItem(IDC_STATIC_MONITOR3)->GetWindowRect(&dummyRect);
	ScreenToClient(&dummyRect);
	m_cadText.SetWindowPos(NULL,dummyRect.left,yPos-dummyRect.Height()-2,dummyRect.Width(),dummyRect.Height(),SWP_SHOWWINDOW);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "yPos = %d", yPos);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling createCADStatusButtons");
	createCADStatusButtons(rc,xPos,yPos);

	yPos += BUTTON_HEIGHT * 4 + BUTTON_HEIGHT / 4;
	GetDlgItem(IDC_STATIC_MONITOR2)->GetWindowRect(&dummyRect);
	ScreenToClient(&dummyRect);
	m_broadcastText.SetWindowPos(NULL,dummyRect.left,yPos-dummyRect.Height()-2,dummyRect.Width(),dummyRect.Height(),SWP_SHOWWINDOW);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "yPos = %d", yPos);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling createBroadcastStatusButtons");
	createBroadcastStatusButtons(rc,xPos,yPos);


	// Set emergency checkbox position
	GetDlgItem(IDC_EMERGENCY_CHECK)->GetWindowRect(&dummyRect);
	ScreenToClient(&dummyRect);
	m_emergencyBtn.SetWindowPos(NULL,400, dummyRect.top, dummyRect.Width(), dummyRect.Height(), SWP_SHOWWINDOW);
	//GetDlgItem(IDC_EMERGENCY_STATIC)->GetWindowRect(&dummyRect);
	//ScreenToClient(&dummyRect);
	//m_emergencyText.SetWindowPos(NULL,335, dummyRect.top, dummyRect.Width(), dummyRect.Height(), SWP_SHOWWINDOW);
	

	//Fixed TD#14284
	int interval = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::FLASH_BUTTON_INTERVAL);

	if (interval == 0)
	{
		interval = 500; // ms
	}

	SetTimer(1,interval,NULL); 


	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_FAIL);


	counter = 1;
	counter1 = 1;

	return TRUE;


}
   
HBRUSH CRadioStatusPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// Call original functions
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//CBrush m_brush;
	//m_brush.CreateSolidBrush(RGB(0, 255, 0));

	// Create a blue color
	COLORREF textColor = RGB(255, 0, 0);

	// Set text color
	if (pWnd->GetDlgCtrlID() == IDC_EMERGENCY_STATIC)
	{
		pDC->SetTextColor(textColor);
	}

	
	//if (nCtlColor == CTLCOLOR_DLG) {
	//	return m_brush;
	//} else {
		return hbr;
	//}
}


void CRadioStatusPage::addTimerObserver(CWnd* observer)
{
	std::set<CWnd*>::iterator iter;
	
	// check if observer already exists
	iter = m_timerSubscribedButtonCollection.find(observer);

	if ( iter == m_timerSubscribedButtonCollection.end() )
	{
		m_timerSubscribedButtonCollection.insert(observer);	
	}

	return;
}


void CRadioStatusPage::removeTimerObserver(CWnd* observer)
{
	FUNCTION_ENTRY("removeTimverObserver");
	
	std::set<CWnd*>::iterator iter;
	
	iter =m_timerSubscribedButtonCollection.find(observer);

	if ( iter != m_timerSubscribedButtonCollection.end() )
	{
		m_timerSubscribedButtonCollection.erase(iter);
	}
	
	

	FUNCTION_EXIT;

}


void CRadioStatusPage::createProfileButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = MAX_PROFILE_BUTTONS;

	try
	{
		nButtons = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::PROFILE_BUTTONS);
	}
    catch(TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for profile buttons");
    }
	
	for(int i = 0; i < nButtons; i++)
	{
		CResourceButton* temp = new CResourceButton();

		temp->setButtonReference(BUTTON_REFERENCE_NUM++);

		temp->Create ("", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  CRect (5, 30, 80, 105),
							  this, i);
		//fixed TD14302
		temp->SetWindowPos(&wndTop, i%4 *(BUTTON_WIDTH + BUTTON_SPACE_H) + xPos, 
						   int(i/4)*(BUTTON_HEIGHT + BUTTON_SPACE_V) + yPos, 
						   BUTTON_WIDTH, BUTTON_HEIGHT, SWP_SHOWWINDOW);
		//fixed TD14302

		// append to profile-based buttons collection
		m_profileButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
		temp->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
		pTempResourceButtonStrategy->initialise(temp);
		
		// set IResourceButtonButtonContainer for Timer functionality
		temp->setResourceButtonContainer(this);

		/* set if this is a profile-based button
		 * 1 = profile-based
		 * 0 = user-defined
		 */
		temp->setProfileButton(1);
	}
}


void CRadioStatusPage::createUserDefinedButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = MAX_USERDEFINED_BUTTONS;

	try
	{
		nButtons = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::USER_BUTTONS);
	}
    catch(TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for user buttons");
    }

	for(int i = 0; i < nButtons; i++)
	{
		CResourceButton* temp = new CResourceButton();

		temp->Create ("", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  CRect (5, 30, 80, 105),
							  this, i);
		//fixed TD14302,increase the gap among buttons 
		temp->SetWindowPos(&wndTop, i%4 *(BUTTON_WIDTH + BUTTON_SPACE_H) + xPos, 
							int(i/4)*(BUTTON_HEIGHT + BUTTON_SPACE_V) + yPos, 
							BUTTON_WIDTH, BUTTON_HEIGHT,SWP_SHOWWINDOW);
		//fixed TD14302
		// append to user-defined buttons collection
		m_userButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
		temp->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
		pTempResourceButtonStrategy->initialise(temp);

		// set IResourceButtonButtonContainer for Timer functionality
		temp->setResourceButtonContainer(this);

		/* set if this is a profile-based button
		 * 1 = profile-based
		 * 0 = user-defined
		 */
		temp->setProfileButton(0);

		temp->setButtonReference(BUTTON_REFERENCE_NUM++);

		temp->UpdateWindow();

	}
}


void CRadioStatusPage::createTalkgroupStatusButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = 12*3;
	
	for(int i = 0; i < nButtons; i++)
	{
		CResourceButton* temp = new CResourceButton();

		temp->setButtonReference(BUTTON_REFERENCE_NUM++);

		temp->Create ("", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  CRect (5, 30, 80, 105),
							  this, i);
		//fixed TD14302
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set Window Position");
		temp->SetWindowPos(&wndTop, i%3 *(BUTTON_WIDTH + BUTTON_SPACE_H) + xPos, 
						   int(i/3)*(BUTTON_HEIGHT + BUTTON_SPACE_V) + yPos, 
						   BUTTON_WIDTH, BUTTON_HEIGHT, SWP_SHOWWINDOW);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished setting windows position");
		//fixed TD14302

		// append to profile-based buttons collection
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Store button into m_talkgroupButtons");
		m_talkgroupButtons.push_back(temp);

		// initialize button decorators for each resource
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create Strategy");
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
		temp->setResourceButtonStrategy(pTempResourceButtonStrategy);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set the strategy");
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Initialize the strategy");
		pTempResourceButtonStrategy->initialise(temp);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set resource button container");
		// set IResourceButtonButtonContainer for Timer functionality
		temp->setResourceButtonContainer(this);

		/* set if this is a profile-based button
		 * 1 = profile-based
		 * 0 = user-defined
		 */
		temp->setProfileButton(0);
	}
}

void CRadioStatusPage::createCADStatusButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = 2*2;
	
	for(int i = 0; i < nButtons; i++)
	{
		CResourceButton* temp = new CResourceButton();

		temp->setButtonReference(BUTTON_REFERENCE_NUM++);

		temp->Create ("", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  CRect (5, 30, 80, 105),
							  this, i);
		//fixed TD14302
		temp->SetWindowPos(&wndTop, (i&1) *(BUTTON_WIDTH + BUTTON_SPACE_H) + xPos, 
						   int(i>>1)*(BUTTON_HEIGHT + BUTTON_SPACE_V) + yPos, 
						   BUTTON_WIDTH, BUTTON_HEIGHT, SWP_SHOWWINDOW);
		//fixed TD14302

		// append to profile-based buttons collection
		m_cadButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
		temp->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
		pTempResourceButtonStrategy->initialise(temp);
		
		// set IResourceButtonButtonContainer for Timer functionality
		temp->setResourceButtonContainer(this);

		/* set if this is a profile-based button
		 * 1 = profile-based
		 * 0 = user-defined
		 */
		temp->setProfileButton(0);
	}
}

void CRadioStatusPage::createBroadcastStatusButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = 1;
	
	for(int i = 0; i < nButtons; i++)
	{
		CResourceButton* temp = new CResourceButton();

		temp->setButtonReference(BUTTON_REFERENCE_NUM++);

		temp->Create ("", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  CRect (5, 30, 80, 105),
							  this, i);
		//fixed TD14302
		temp->SetWindowPos(&wndTop, i%3 *(BUTTON_WIDTH + BUTTON_SPACE_H) + xPos, 
						   int(i/3)*(BUTTON_HEIGHT + BUTTON_SPACE_V) + yPos, 
						   BUTTON_WIDTH, BUTTON_HEIGHT, SWP_SHOWWINDOW);
		//fixed TD14302

		// append to profile-based buttons collection
		m_broadcastButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_NIL);
		temp->setResourceButtonStrategy(pTempResourceButtonStrategy);
		
		pTempResourceButtonStrategy->initialise(temp);
		
		// set IResourceButtonButtonContainer for Timer functionality
		temp->setResourceButtonContainer(this);

		/* set if this is a profile-based button
		 * 1 = profile-based
		 * 0 = user-defined
		 */
		temp->setProfileButton(0);
	}
}





void CRadioStatusPage::OnTimer(UINT nIDEvent) 
{
	m_isTimerToggleOn = !m_isTimerToggleOn;

	std::set<CWnd*>::iterator iter;

	for ( iter=m_timerSubscribedButtonCollection.begin(); iter != m_timerSubscribedButtonCollection.end(); ++iter)
	{
		::PostMessage((*iter)->m_hWnd,WM_TIMER_TRIGGERED,m_isTimerToggleOn,0);
	}
	
	CPropertyPage::OnTimer(nIDEvent);
}



void CRadioStatusPage::clearButtons()
{
    FUNCTION_ENTRY("clearButtons");
    
	std::vector<CResourceButton*>::iterator iter = m_profileButtons.begin();

	for ( ; iter != m_profileButtons.end(); ++iter )
	{
		delete *iter;
	}
    m_profileButtons.clear();

	for ( iter= m_userButtons.begin(); iter != m_userButtons.end(); ++iter )
	{
		delete *iter;
	}
    m_userButtons.clear();


	FUNCTION_EXIT;
}

LRESULT CRadioStatusPage::onNotifiedDatabaseReady(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDatabaseReady");
	
	try
	{
		std::vector<ResourceButton> resources = The_RadioSystemStatus::instance()->getMonitoredResource();
		std::vector<ResourceButton>::iterator it;

		for(it=resources.begin();it!=resources.end();++it)
		{
			std::vector<CResourceButton*>::iterator pIter;
			int index;

			if ((*it).isProfile)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getting button from database, button deleted, profile based: %u", (*it).id);
				//int index = (*it).buttonRef - 1;
				//pIter = m_profileButtons.begin() + index;
			}
			else
			{	
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getting button from database, user defined based, size = %d, type %c, rid %u, talkgroup size = %u, cad size = %u", (*it).buttonRef, (*it).type,(*it).id,m_talkgroupButtons.size(), m_cadButtons.size());
				switch((*it).type) {
					case RR_TALKGROUP:						
						index = (*it).buttonRef - 1;
						pIter = m_talkgroupButtons.begin() + index;
						break;
					case RR_PATCHGROUP:
						index = (*it).buttonRef - m_talkgroupButtons.size() - 1;
						pIter = m_cadButtons.begin() + index;
						break;
					case RR_MSELECT:
						index = (*it).buttonRef - m_talkgroupButtons.size() - m_cadButtons.size() - 1;
						pIter = m_broadcastButtons.begin() + index;						
						break;
				}
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getting resource by rid %u, type %c", (*it).id,(*it).type);
				RadioResource res = The_RadioSystemStatus::instance()->getResourceByRID((*it).id,(*it).type);
				(*pIter)->assignButtonOnInit(res);
			}

			//RadioResource res = The_RadioSystemStatus::instance()->getResourceByRID((*it).id,(*it).type);
			//(*pIter)->assignButtonOnInit(res);
		}
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
	}
	FUNCTION_EXIT;
	return 0;	
}

LRESULT CRadioStatusPage::onNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM checked)
{
	RadioResource* radioResource = (RadioResource*)pRadioResource;
	std::vector<CResourceButton*>::iterator iter = m_profileButtons.begin();

    if (0) //checked)
    {
//        if (radioResource->type == RR_RADIO)
        {
            // clear all other selections, because we are not going to get
            // the select/deselect event
		    for ( ; iter != m_profileButtons.end(); ++iter )
		    {
			    if ( (*iter)->getRadioResource().id != radioResource->id &&
                     (*iter)->isChecked())
			    {
                    (*iter)->setCheck(false);
			    }
		    }
		    for (iter = m_userButtons.begin() ; iter != m_userButtons.end(); ++iter )
		    {
			    if ( (*iter)->getRadioResource().id != radioResource->id &&
                     (*iter)->isChecked())
			    {
                    (*iter)->setCheck(false);
			    }
		    }
        }
/*        else
        {
            // clear all individual radio selections
		    for ( ; iter != m_profileButtons.end(); ++iter )
		    {
			    if ( (*iter)->getRadioResource().type == RR_RADIO &&
                     (*iter)->isChecked())
			    {
                    (*iter)->setCheck(false);
			    }
		    }
		    for (iter = m_userButtons.begin() ; iter != m_userButtons.end(); ++iter )
		    {
			    if ( (*iter)->getRadioResource().type == RR_RADIO &&
                     (*iter)->isChecked())
			    {
                    (*iter)->setCheck(false);
			    }
		    }
        }
        */
    }

    return 0;
}


void CRadioStatusPage::onEmergencyPressed()
{
	FUNCTION_ENTRY("onEmergencyPressed");
m_talkgroupButtons[2]->setCheck(true);
	//

	//	 //testing purpose
	//RadioResource r;
	//r.alias = "Radio 91113";
	//r.id = 91113;
	//r.ssi = 91113;
	//r.type = RR_RADIO;

	//RadioResource data;	
	//data.alias = "Conventional 1";
 //   data.id = 268435459;
	//data.type = RR_TALKGROUP;


	//if (counter % 5 == 1) {		
	//	The_RadioOperations::instance()->onResourceEmergencyEvent(data,r);
	//} else if (counter % 5 == 2){
	//	The_RadioOperations::instance()->onResourceEmergencyAcknowledgedEvent(data,r);
	//} else if (counter % 5 == 3){
	//	The_RadioOperations::instance()->onResourceEmergencyEndedEvent(data,r);
	//} else if (counter % 5 == 4){
	//	The_RadioOperations::instance()->onResourceEmergencyEvent(data,r);
	//} else if (counter % 5 == 0){
	//	The_RadioOperations::instance()->onResourceEmergencyEndedEvent(data,r);
	//}
	//counter++;

	

	//The_RadioOperations::instance()->onPrivateCallRinging(r);
	
	/*
	if (m_emergencyBtn.GetCheck()) {
	RadioResource r;
	r.alias = "Conventional 1";
	r.id = 268435459;
	r.type = RR_TALKGROUP;
	The_RadioOperations::instance()->onInboundCallEvent(r);
	} else {
	RadioResource r;
	r.alias = "Conventional 1";
	r.id = 268435459;
	r.type = RR_TALKGROUP;
	The_RadioOperations::instance()->onInboundCallCanceledEvent(r);
	}*/
	//TA_IRS_Bus::Radio::RadioResource caller;
	//caller.alias = "Radio 91102";
	//caller.id = 91102;
	//caller.ssi = 91102;
	//caller.type = TA_IRS_Bus::Radio::ERadioResourceType::RR_RADIO;
	//
	//caller.alias = "Radio Testing";
	//caller.id = 26843534;
	//caller.ssi = 26843534;
	//caller.type = TA_IRS_Bus::Radio::ERadioResourceType::RR_RADIO;

	//		TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
	//		callEvent.caller = caller;
	//		if (counter == 1)
	//		callEvent.status = TA_IRS_Bus::Radio::EPrivateCallState::PC_INCOMING;//PC_INCOMING;PC_NOT_INVOLVED
	//		else if (counter == 2)
	//			callEvent.status = TA_IRS_Bus::Radio::EPrivateCallState::PC_ESTABLISHED;
	//		else if (counter == 3)
	//			callEvent.status = TA_IRS_Bus::Radio::EPrivateCallState::PC_NOT_INVOLVED;
	//		else if (counter == 4)
	//			callEvent.status = TA_IRS_Bus::Radio::EPrivateCallState::PC_INCOMING;
	//		callEvent.isEmergencyMode = false;

	//		time_t now;
	//		time(&now);

	//		callEvent.timeOfCall = now;
	//		
	//		CORBA::Any msg;
	//		msg <<= callEvent;
	//TCHAR buff[200];
	//		wsprintf(buff, "Send comms: entity=%d, subsys=%d, loc=%d", 350030924, 12, 1);

	//		TA_Base_Core::MessagePublicationManager::getInstance().
 //       getCommsMessageSender(TA_Base_Core::RadioComms::Context)->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
	//		350030924,//m_entityKey, 
	//		msg,
	//		11,//m_entitySubsystem
	//		1);//, m_entityLocation);
	//		counter++;
	//		if (counter > 4) counter = 1;

	//if (The_RadioOperations::instance()->isInEmergency()) {
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The status is already in emergency");
	//	m_emergencyBtn.SetCheck(1);
	//} else {	
	//	if (The_RadioOperations::instance()->isEmergencyPressed()) {
	//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting emergency button status to false");
	//		The_RadioOperations::instance()->setEmergencyPressed(false);
	//	} else {
	//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting emergency button status to true");
	//		The_RadioOperations::instance()->setEmergencyPressed(true);
	//	}
	//}

	FUNCTION_EXIT;
}

bool CRadioStatusPage::isMonitored(int nProfile,const RadioResource& resource)
{
	
//fixed TD14304, add codes here to check whether the resouce is monitered in GUI.
	CResourceButton *temp = NULL;
	RadioResource tempResource;

	if (nProfile == 1)//profile button
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isMonitored for profile button");
		/*
		std::vector<CResourceButton*>::iterator it = m_profileButtons.begin();
		for (; it != m_profileButtons.end(); ++it)
		{
			temp = NULL;
			temp = *it;
			TA_ASSERT(temp != NULL, "CResourceButton is equal to NULL");
			
			tempResource = temp->getRadioResource();
			if (tempResource.type == RR_NIL) continue;
			if (resource.id == tempResource.id && 
				resource.ssi == tempResource.ssi &&
				resource.type == tempResource.type)
			{
				if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
				{
					return true;
				}
			}
		}
		*/
	}else{
		std::vector<CResourceButton*>::iterator it1 = m_talkgroupButtons.begin();
		std::vector<CResourceButton*>::iterator it2 = m_cadButtons.begin();
		std::vector<CResourceButton*>::iterator it3 = m_broadcastButtons.begin();
		switch (resource.type) {
			case RR_DYNGROUP: case RR_TALKGROUP:
				for (; it1 != m_talkgroupButtons.end(); ++it1)
				{
					temp = NULL;
					temp = *it1;
					TA_ASSERT(temp != NULL, "CResourceButton is equal to NULL");			
					tempResource = temp->getRadioResource();
					if (tempResource.type == RR_NIL) continue;
					if (resource.id == tempResource.id && 
					resource.ssi == tempResource.ssi )
					{
						if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
						{
							return true;
						}
					}
				}
				break;
			case RR_PATCHGROUP:
				for (; it2 != m_cadButtons.end(); ++it2)
				{
					temp = NULL;
					temp = *it2;
					TA_ASSERT(temp != NULL, "CResourceButton is equal to NULL");			
					tempResource = temp->getRadioResource();
					if (tempResource.type == RR_NIL) continue;
					if (resource.id == tempResource.id && 
					resource.ssi == tempResource.ssi )
					{
						if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
						{
							return true;
						}
					}
				}
				break;
			case RR_MSELECT:
				for (; it3 != m_broadcastButtons.end(); ++it3)
				{
					temp = NULL;
					temp = *it3;
					TA_ASSERT(temp != NULL, "CResourceButton is equal to NULL");			
					tempResource = temp->getRadioResource();
					if (tempResource.type == RR_NIL) continue;
					if (resource.id == tempResource.id && 
					resource.ssi == tempResource.ssi )
					{
						if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
						{
							return true;
						}
					}
				}
				break;
			case RR_RADIO:
				tempResource = m_radioMonitorBtn->getRadioResource();
				if (resource.id == tempResource.id && 
					resource.ssi == tempResource.ssi )
					{
						if (The_StatusManager::instance()->getFeatureAttributeIsMonitored(resource))
						{
							return true;
						}
					}
				break;
		};
	}
	//fixed TD14304


    return false;
}


int CRadioStatusPage::getTalkgroupSize() 
{
	FUNCTION_ENTRY("getTalkgroupSize");
	return m_talkgroupButtons.size();
	FUNCTION_EXIT;
}

int CRadioStatusPage::getCadSize()
{
	FUNCTION_ENTRY("getCadSize");
	return m_cadButtons.size();
	FUNCTION_EXIT;
}

static _inline COLORREF _stdcall GetParentDlgBkColor2(
	IN CONST HWND hWnd)
{
	COLORREF crRet = CLR_INVALID;
	if (hWnd && IsWindow(hWnd))
	{
		HWND hWndParent = GetParent(hWnd);
		if (hWndParent)
		{
			RECT rc;
			if (GetClientRect(hWndParent, &rc))
			{
				HDC hDC = GetDC(hWndParent);
				if (hDC)
				{
					HDC hdcMem = CreateCompatibleDC(hDC);
					if (hdcMem)
					{
						HBITMAP hBmp = CreateCompatibleBitmap(hDC,
							rc.right, rc.bottom);
						if (hBmp)
						{
							HGDIOBJ hOld = SelectObject(hdcMem, hBmp);
							if (hOld)
							{
								if (SendMessage(hWndParent,
									WM_ERASEBKGND, (WPARAM)hdcMem, 0))
								{
									crRet = GetPixel(hdcMem, 0, 0);
								}
								SelectObject(hdcMem, hOld);
							}
							DeleteObject(hBmp);
						}
						DeleteDC(hdcMem);
					}
					ReleaseDC(hWndParent, hDC);
				}
			}
		}
	}
	return crRet;
}

CResourceButton* CRadioStatusPage::getRadioMonitorBtn()
{
	FUNCTION_ENTRY("getRadioMonitorBtn");

	return m_radioMonitorBtn;
	FUNCTION_EXIT;
}


void CRadioStatusPage::setRadioMonitorBtn(CResourceButton* btn)
{
	FUNCTION_ENTRY("setRadioMonitorBtn");
	m_radioMonitorBtn = btn;
	FUNCTION_EXIT;
}

CResourceButton* CRadioStatusPage::getResourceButton(unsigned int butRef)
{
	FUNCTION_ENTRY("getResourceButton");

	if (butRef == CResourceButton::RADIO_BUTTON_REFERENCE) {
		return m_radioMonitorBtn;
	}

	int totSize = m_talkgroupButtons.size() + m_cadButtons.size() + m_broadcastButtons.size();

	if ((butRef < 1) || (butRef >totSize)) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Button reference out of bounds");
		return NULL;
	}

	if (butRef <= m_talkgroupButtons.size()) {
		return m_talkgroupButtons[butRef-1];
	} else if (butRef <= m_talkgroupButtons.size() + m_cadButtons.size()) {
		return m_cadButtons[butRef-m_talkgroupButtons.size()-1];
	} else {
		return m_broadcastButtons[butRef-m_talkgroupButtons.size()-m_cadButtons.size()-1];
	}

	FUNCTION_EXIT;
}


LRESULT CRadioStatusPage::onNotifiedOperationFail(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedOperationFail");
	// change back the state of respective button to previous state

	RadioResource* radioResource = (RadioResource*)wParam;
	int butId = (int) lParam;
	int realId = 0;

	if (RR_TALKGROUP == radioResource->type || RR_DYNGROUP == radioResource->type) {
		realId = butId - 1;
		Sleep(100);
		if (m_talkgroupButtons[realId]->isChecked()) {
			m_talkgroupButtons[realId]->setCheck(false);
			m_talkgroupButtons[realId]->SetCheck(BST_UNCHECKED);
		} else {
			m_talkgroupButtons[realId]->setCheck(true);
			m_talkgroupButtons[realId]->SetCheck(BST_CHECKED);
		}
	} else if (RR_PATCHGROUP == radioResource->type) {
		realId = butId - m_talkgroupButtons.size() - 1;
	} else if(RR_MSELECT == radioResource->type) {
		realId = butId - m_talkgroupButtons.size() - m_cadButtons.size() - 1;
	}

	FUNCTION_EXIT;
	return 0;
}