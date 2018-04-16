/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioManager.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#if !defined(AFX_RADIOMANAGER_H__419C3567_A922_4DCE_A304_CF6931AB8957__INCLUDED_)
#define AFX_RADIOMANAGER_H__419C3567_A922_4DCE_A304_CF6931AB8957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/radio/radio_manager_gui_icdv4/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"


/////////////////////////////////////////////////////////////////////////////
// CRadioManagerApp:
//

class CRadioManagerApp : public TA_Base_Bus::TransActiveWinApp
{
	public:
		CRadioManagerApp();
	
		virtual ~CRadioManagerApp();


		//{{AFX_VIRTUAL(CRadioManagerApp)
		public:
		virtual BOOL InitInstance();
		//}}AFX_VIRTUAL


		//{{AFX_MSG(CRadioManagerApp)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_RADIOMANAGER_H__419C3567_A922_4DCE_A304_CF6931AB8957__INCLUDED_)
