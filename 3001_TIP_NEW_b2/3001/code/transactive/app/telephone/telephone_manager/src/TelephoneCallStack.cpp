/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/TelephoneCallStack.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// TelephoneCallStack.cpp: implementation of the TelephoneCallStack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "telephone_manager.h"
#include "TelephoneCallStack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(TelephoneCallStack, CTabDialogCtrl)
    // {{AFX_MSG_MAP(TelephoneCallStack)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TelephoneCallStack::TelephoneCallStack() : CTabDialogCtrl()
{
    FUNCTION_ENTRY( "TelephoneCallStack" );

    this->AddPage(m_currentCallStack,IDD_TELEPHONESTACK,CString("Current"));
    this->AddPage(m_othersCallStackDlg, IDD_STACK_OTHERS, CString("Others"));
    this->AddPage(m_historyCallStack,IDD_CALLSTACK_HISTORY,CString("History"));

    FUNCTION_EXIT;
}


TelephoneCallStack::~TelephoneCallStack()
{
    FUNCTION_ENTRY( "~TelephoneCallStack" );
    FUNCTION_EXIT;
}


