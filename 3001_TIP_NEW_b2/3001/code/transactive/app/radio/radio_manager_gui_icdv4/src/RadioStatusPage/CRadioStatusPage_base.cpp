/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/Attic/CRadioStatusPage.cpp,v $
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


#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/Resource.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/CRadioStatusPage.h"
#include "app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_manager_gui/src/DebugMsg.h"

#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

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
	clearButtons();
}

void CRadioStatusPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioStatusPage)
	DDX_Control(pDX, IDC_STATIC_MONITOR, m_monitoredText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioStatusPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRadioStatusPage)
	ON_MESSAGE(WM_DATABASE_READY,onNotifiedDatabaseReady)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED, onNotifiedEntitySelectionChanged)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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
	BUTTON_WIDTH = (m_parentRect.right / 4) - 2;
	//BUTTON_HEIGHT = (BUTTON_WIDTH / 4) - 2;
	BUTTON_HEIGHT = (m_parentRect.bottom/22) - 2;

	SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	// Convert to client coordinates
	ScreenToClient (&rc);
	
	int xPos = 8;
	int yPos = 26; //25;

	createProfileButtons(rc,xPos,yPos);
	//createTalkgroupStatusButtons(rc,xPos,yPos);

	// TD14957
	// create User-defined monitored resources
	yPos = 140; //130;
	CRect dummyRect;
	GetDlgItem(IDC_STATIC_MONITOR)->GetWindowRect(&dummyRect);
	ScreenToClient(&dummyRect);
	m_monitoredText.SetWindowPos(NULL,dummyRect.left,yPos-dummyRect.Height()-2,dummyRect.Width(),dummyRect.Height(),SWP_SHOWWINDOW);

	createUserDefinedButtons(rc,xPos,yPos);

	//Fixed TD#14284
	int interval = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::FLASH_BUTTON_INTERVAL);

	if (interval == 0)
	{
		interval = 500; // ms
	}

	SetTimer(1,interval,NULL); 


	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);

	return TRUE;

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
	int nButtons = 14*4;
	
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
		m_talkgroupButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_TALKGROUP);
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

void CRadioStatusPage::createCADStatusButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = 4*4;
	
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
		m_cadButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_PATCHGROUP);
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

void CRadioStatusPage::createBroadcastStatusButtons(RECT rc, int xPos, int yPos)
{
	int nButtons = 3;
	
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
		m_broadcastButtons.push_back(temp);

		// initialize button decorators for each resource
		IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(RR_MSELECT);
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
	
	try
	{
		std::vector<ResourceButton> resources = The_RadioSystemStatus::instance()->getMonitoredResource();
		std::vector<ResourceButton>::iterator it;

		for(it=resources.begin();it!=resources.end();++it)
		{
			std::vector<CResourceButton*>::iterator pIter;

			if ((*it).isProfile)
			{
				int index = (*it).buttonRef - 1;
				pIter = m_profileButtons.begin() + index;
				//RadioResource res = The_RadioSystemStatus::instance()->getResourceByRID((*it).id,(*it).type);
				//(*pIter)->assignButtonOnInit(res);
			}
			else
			{
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getting button from database, button deleted, not profile based: %u", (*it).id
				int index = (*it).buttonRef - m_profileButtons.size() - 1;
				pIter = m_userButtons.begin() + index;
			}

			RadioResource res = The_RadioSystemStatus::instance()->getResourceByRID((*it).id,(*it).type);
			(*pIter)->assignButtonOnInit(res);
		}
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
	}
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

bool CRadioStatusPage::isMonitored(int nProfile,const RadioResource& resource)
{
//fixed TD14304, add codes here to check whether the resouce is monitered in GUI.
	CResourceButton *temp = NULL;
	RadioResource tempResource;

	if (nProfile == 1)//profile button
	{
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
	}else{
		std::vector<CResourceButton*>::iterator it = m_userButtons.begin();
		for (; it != m_userButtons.end(); ++it)
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
	}
	//fixed TD14304
	
    return false;
}
