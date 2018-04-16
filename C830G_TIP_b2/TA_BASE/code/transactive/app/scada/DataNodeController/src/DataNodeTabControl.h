/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeTabControl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_DATANODETABCONTROL_H__97E16A7B_85CA_46E6_93CD_801DE6C7D160__INCLUDED_)
#define AFX_DATANODETABCONTROL_H__97E16A7B_85CA_46E6_93CD_801DE6C7D160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h"
#include "InhibitDialog.h"
#include "NotesDialog.h"
#include "SummaryDialog.h"

namespace TA_Base_App
{

	class CDataNodeControllerTabControl : public CTabDialogCtrl  
	{
	public:
		CDataNodeControllerTabControl();
		virtual ~CDataNodeControllerTabControl();

	private:
		CInhibitDialog m_InhibitDlg;
		CSummaryDialog m_SummaryDlg;
		CNotesDialog m_NotesDlg;
	};

}
#endif // !defined(AFX_DATANODETABCONTROL_H__97E16A7B_85CA_46E6_93CD_801DE6C7D160__INCLUDED_)
