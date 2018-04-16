#if !defined(AFX_ECSMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
#define AFX_ECSMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSManager.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Derived from the TransActiveWinApp, this is the main application class for the ECSManager.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

namespace TA_IRS_App
{
	class ECSManagerApp : public TA_Base_Bus::TransActiveWinApp
	{
	public:
		ECSManagerApp();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(ECSManagerApp)
		public:
		//}}AFX_VIRTUAL

	// Implementation

		//{{AFX_MSG(ECSManagerApp)
			// NOTE - the ClassWizard will add and remove member functions here.
			//	  DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


		/**
		 * InitInstance
		 *
		 * This method must be implemented for ECSManager to ensure that the mandatory command line 
		 * parameter that determines the ECS dialog type for this application instance (AND hence 
		 * the corresponding application type) should be set as a runparam before the framework 
		 * demands the application type from ECSManagerGUI.
		 */
		virtual BOOL InitInstance();

	};
} // namespace TA_IRS_App

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECSMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
