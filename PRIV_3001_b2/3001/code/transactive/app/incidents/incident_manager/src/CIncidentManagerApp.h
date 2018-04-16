/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/incident_manager/src/CIncidentManagerApp.h $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * CIncidentManagerApp inherits from TransActiveWinApp. Object of 
  * CIncidentManagerApp provides member functions for initializing 
  * and running the application
  */

#if !defined(CIncidentManagerApp_95E35674_65D2_48cb_8D13_EDC3CCE6270B__INCLUDED_)
#define CIncidentManagerApp_95E35674_65D2_48cb_8D13_EDC3CCE6270B__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "app/incidents/incident_manager/src/IncidentManagerFramework.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"

/**
 * @version 1.0
 * @created 16-May-2005 12:18:49 PM
 */
namespace TA_IRS_App
{	
	class CIncidentManagerApp: public TA_Base_Bus::TransActiveWinApp
	{
		
	public:
		CIncidentManagerApp();
		virtual ~CIncidentManagerApp();
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CIncidentManagerApp)
	public:
		//}}AFX_VIRTUAL
		virtual BOOL InitInstance();
		
		// Implementation
		//{{AFX_MSG(CIncidentManagerApp)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	};
}// end TA_IRS_App
#endif // !defined(CIncidentManagerApp_95E35674_65D2_48cb_8D13_EDC3CCE6270B__INCLUDED_)
