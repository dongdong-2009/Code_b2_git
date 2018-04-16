#pragma once


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
//#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h >       // MFC OLE automation classes
//#include <afxpriv.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class CallStackFilterList : public TA_Base_Bus::ListCtrlSelNoFocus
{
	public:
		CallStackFilterList();   // standard constructor
		virtual ~CallStackFilterList();


		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};