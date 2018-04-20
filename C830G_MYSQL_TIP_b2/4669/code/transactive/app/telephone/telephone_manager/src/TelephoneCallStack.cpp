/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/TelephoneCallStack.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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


