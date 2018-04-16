/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/sound/sound_manager/stub/src/ConnectionPointTest.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// ConnectionPointTest.h : main header file for the CONNECTIONPOINTTEST application
//

#if !defined(AFX_CONNECTIONPOINTTEST_H__6A81A5AD_7D25_4042_8D9C_461CF437E0A4__INCLUDED_)
#define AFX_CONNECTIONPOINTTEST_H__6A81A5AD_7D25_4042_8D9C_461CF437E0A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "bus/sound/sound_client/src/ISoundObserver.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"

/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestApp:
// See ConnectionPointTest.cpp for the implementation of this class
//

class ConnectionPointTestApp : public CWinApp,
                               public TA_Base_Bus::ISoundObserver
{
public:
	ConnectionPointTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConnectionPointTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(ConnectionPointTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    // Event handlers
	virtual void handleCategorySilenced(const std::string& category, bool isSilenced);
	virtual void handleAllSilenced(bool isSilenced);

private:
	TA_Base_Bus::SoundManagerProxy m_soundManager;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONPOINTTEST_H__6A81A5AD_7D25_4042_8D9C_461CF437E0A4__INCLUDED_)
