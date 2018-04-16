/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewer.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * main header file for the INTEGRATED ALARM VIEWER application  
  */

#if !defined(AFX_ATSISCSALARMVIEWERGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
#define AFX_ATSISCSALARMVIEWERGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/ats/AtsIscsAlarmViewer/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CAtsIscsAlarmViewerApp:
// See AlarmsGui.cpp for the implementation of this class
//

class CAtsIscsAlarmViewerApp : public TA_Base_Bus::TransActiveWinApp
{
public:

	/**
	  * CAtsIscsAlarmViewerApp
	  *
	  * Constructor for the CAtsIscsAlarmViewerApp object. 
	  * 
	  * @return 
	  *
	  * @exception 
	  */
	CAtsIscsAlarmViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtsIscsAlarmViewerApp)
	public:
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAtsIscsAlarmViewerApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATSISCSALARMVIEWERGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)

