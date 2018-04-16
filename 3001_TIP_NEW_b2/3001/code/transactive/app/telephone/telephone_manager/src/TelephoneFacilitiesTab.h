/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/TelephoneFacilitiesTab.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// TelephoneFacilitiesTab.h: interface for the TelephoneFacilitiesTab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELEPHONEFACILITIESTAB_H__33CA3ED3_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_TELEPHONEFACILITIESTAB_H__33CA3ED3_8829_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "DirectLineDialog.h"
#include "ConfigureDirectLinesDialog.h"
#include "ConfigureDirectoryDialog.h"
#include "DirectorySearchDialog.h"

class TelephoneFacilitiesTab : public CTabDialogCtrl
{
public:
	TelephoneFacilitiesTab();
	virtual ~TelephoneFacilitiesTab();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TelephoneFacilitiesTab)
	//}}AFX_VIRTUAL

private:
	
	DirectLineDialog				m_directLineDialog;
	DirectorySearchDialog           m_directorySearchDialog;
	ConfigureDirectLinesDialog		m_configureDirectLinesDialog;
	ConfigureDirectoryDialog		m_configureDirectoryDialog;

		// Generated message map functions
protected:
	//{{AFX_MSG(TelephoneFacilitiesTab)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
};

#endif // !defined(AFX_TELEPHONEFACILITIESTAB_H__33CA3ED3_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
