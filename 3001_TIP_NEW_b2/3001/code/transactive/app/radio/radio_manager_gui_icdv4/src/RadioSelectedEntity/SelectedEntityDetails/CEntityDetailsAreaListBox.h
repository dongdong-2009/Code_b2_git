#pragma once

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class CEntityDetailsAreaListBox : public CListBox
{
	public:
		CEntityDetailsAreaListBox(); 
		virtual ~CEntityDetailsAreaListBox();


		DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnSetFocus(CWnd* hWnd);

	public:
		void setReadOnly(bool val);
		bool isReadOnly();
	private:
		bool readOnly;
};