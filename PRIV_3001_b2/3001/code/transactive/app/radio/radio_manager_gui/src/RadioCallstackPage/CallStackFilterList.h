#pragma once


#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class CallStackFilterList : public TA_Base_Bus::ListCtrlSelNoFocus
{
	public:
		CallStackFilterList();   // standard constructor
		virtual ~CallStackFilterList();


		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(NMHDR* pNMHDR, LRESULT* pResult);
		void OnMakeCall();
		void OnRemoveCall();
	private:
		RadioResource m_rightClickResource;
};