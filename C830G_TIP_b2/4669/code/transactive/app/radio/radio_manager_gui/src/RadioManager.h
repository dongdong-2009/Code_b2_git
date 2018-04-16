/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioManager.h : main header file for the RadioManager application
//

#if !defined(AFX_RADIOMANAGER_H__AB68D995_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
#define AFX_RADIOMANAGER_H__AB68D995_4E17_11D7_B14B_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "RadioManagerGUI.h"
#include "resource.h"	
#include "bus\generic_gui\src\TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CRadioManager:
// See RadioManager.cpp for the implementation of this class
//

class CRadioManager : public TA_Base_Bus::TransActiveWinApp
{
public:
	CRadioManager();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioManager)
	public:
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CRadioManager)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

    // runparam. When this is set, the call with this id will be selected in the calls list
    static const char* RPARAM_CALLID;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOMANAGER_H__AB68D995_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
