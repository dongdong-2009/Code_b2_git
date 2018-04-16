#pragma once


#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
//#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h >       // MFC OLE automation classes
//#include <afxpriv.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class RadioManagerFilterList : public TA_Base_Bus::MultiFilterListCtrl,
						       public TA_Base_Bus::ISortingCallback
{
	public:
		RadioManagerFilterList(int maxColumns=2);   // standard constructor
		virtual ~RadioManagerFilterList();

		void setRadioResourceType(RadioResourceType);
		RadioResourceType getRadioResourceType();

		virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort);
		void useCallback(int columnNo);

		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		virtual void OnPaint();

	private:
		RadioResourceType m_resType;
};