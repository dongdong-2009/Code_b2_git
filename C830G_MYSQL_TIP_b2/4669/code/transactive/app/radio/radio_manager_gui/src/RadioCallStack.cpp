/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// RadioCallStack.cpp: implementation of the CRadioCallStack class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"
#include "app/radio/radio_manager_gui/src/RadioCallStack.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRadioCallStack::CRadioCallStack() 
                : CTabDialogCtrl()
{
	AddPage(m_CurrentPage,IDD_CALLSTACK_CURRENT,CString("Current"));
	AddPage(m_OtherPage,IDD_CALLSTACK_OTHER,CString("Others"));
	AddPage(m_HistoryPage,IDD_CALLSTACK_HISTORY,CString("History"));

	AddPage(m_DynamicGroupPage,IDD_DYNAMIC_GROUP,CString("Dynamic group"));
    m_DynamicGroupPageIndex = 3;
}

void CRadioCallStack::Init()
{
	//init dynamic group page first 'cause the train selection component is needed by others
	m_DynamicGroupPage.Init();
	m_CurrentPage.Init();
	m_OtherPage.Init();
	m_HistoryPage.Init();
}

void CRadioCallStack::setRadioSessionReference(long sessionRef)
{
	m_CurrentPage.setRadioSessionReference(sessionRef);
	m_OtherPage.setRadioSessionReference(sessionRef);
}


void CRadioCallStack::setRadioEntityName(std::string radioEntityName)
{
	m_CurrentPage.setRadioEntityName(radioEntityName);
	m_OtherPage.setRadioEntityName(radioEntityName);
}

CRadioCallStack::~CRadioCallStack()
{

}


BEGIN_MESSAGE_MAP(CRadioCallStack, CTabDialogCtrl)
	//{{AFX_MSG_MAP(CRadioCallStack)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CRadioCallStack::addDynamicGroupMembers(std::vector<std::vector<CString> >* newMembers)
{
    // Automatically display the dynamic groups page when adding dynamic group members
    if (getActiveDialog() != &m_DynamicGroupPage)
    {
        SetCurSel(m_DynamicGroupPageIndex);
    }

    m_DynamicGroupPage.PostMessage(ADD_DYNAMIC_GROUP_MEMBERS,reinterpret_cast<WPARAM>(newMembers));
}

void CRadioCallStack::updateExistingDynamicGroupMembers( CString& oldTsiNumber,
                                                         CString& oldServiceNumber,
                                                         CString& oldLocation,
                                                         CString& newTsiNumber,
                                                         CString& newServiceNumber,
                                                         CString& newLocation)
{
    m_DynamicGroupPage.updateExistingDynamicGroupMembers( oldTsiNumber, oldServiceNumber, oldLocation,
                                                          newTsiNumber, newServiceNumber, newLocation);
}
/////////////////////////////////////////////////////////////////////////////
// CRadioCallStack message handlers

void CRadioCallStack::onUpdateRights()
{
	// Update all the pages
	m_CurrentPage.onUpdateRights();
	m_OtherPage.onUpdateRights();
	m_HistoryPage.onUpdateRights();
	m_DynamicGroupPage.onUpdateRights();
}

