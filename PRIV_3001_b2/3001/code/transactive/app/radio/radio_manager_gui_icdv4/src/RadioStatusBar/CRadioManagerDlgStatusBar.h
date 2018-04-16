/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusBar/CRadioManagerDlgStatusBar.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */
#ifndef RADIO_MANAGER_DLG_STATUS_BAR_H_INCLUDED
#define RADIO_MANAGER_DLG_STATUS_BAR_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"

#include <string>


class CRadioManagerDlgStatusBar : public CStatusBar
{
	public:
		CRadioManagerDlgStatusBar();

		virtual ~CRadioManagerDlgStatusBar();

	//Message maps;
	protected:
		//{{AFX_MSG(CRadioManagerDlgStatusBar)
		virtual BOOL OnInitDialog();
		//afx_msg LRESULT onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM lParam);
		//afx_msg LRESULT onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM lParam);
		//afx_msg LRESULT onNotifiedInboundCall(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedPrivateCallEstablished(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedPrivateCallEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundPTTStatus(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundPTTStatusReleased(WPARAM wParama, LPARAM lParam);
		afx_msg LRESULT onNotifiedPrivateCallRinging(WPARAM wParam, LPARAM lParam); // TD#14289
		afx_msg LRESULT onNotifiedOutgoingCall(WPARAM wParam, LPARAM lParam); // TD#14289
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		//static UINT BASED_CODE m_indicators[4];
		CFont m_font;
};
#endif
