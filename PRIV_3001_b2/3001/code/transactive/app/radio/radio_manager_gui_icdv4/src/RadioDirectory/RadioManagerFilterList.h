#pragma once


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
//#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h >       // MFC OLE automation classes
//#include <afxpriv.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class RadioManagerFilterList : public TA_Base_Bus::MultiFilterListCtrl
{
	public:
		RadioManagerFilterList(int maxColumns=2);   // standard constructor
		virtual ~RadioManagerFilterList();

		void setRadioResourceType(RadioResourceType);
		RadioResourceType getRadioResourceType();


		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	private:
		RadioResourceType m_resType;
};