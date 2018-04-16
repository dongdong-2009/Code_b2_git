/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/CRadioEntityPage.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#if !defined(AFX_CRADIOENTITYPAGE_H__FDFD7F10_C6B6_413B_89A9_FC9CB0B67B75__INCLUDED_)
#define AFX_CRADIOENTITYPAGE_H__FDFD7F10_C6B6_413B_89A9_FC9CB0B67B75__INCLUDED_


#include "bus/radio/radio_bus/src/RadioTypes.h"
//#include "bus/telephony/TelephoneBus/src/TelephoneTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CRadioEntityPage dialog

class CRadioEntityPage : public CDialog
{
	// Construction
	public:
		CRadioEntityPage(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CRadioEntityPage)
		enum { IDD = IDD_PATCHMANAGER_RADIO_PAGE };
		CButton	m_RadioEntityReleaseButton;
		CButton	m_RadioEntityDisplayDialpadButton;
		CButton	m_RadioEntityCallButton;
		CComboBox	m_RadioEntityIDCBCtrl;
		CComboBox	m_RadioEntityDescriptionCBCtrl;
		CString	m_RadioEntityDescriptionStr;
		CString	m_RadioEntityIDStr;
		//}}AFX_DATA


	// Overrides
		//{{AFX_VIRTUAL(CRadioEntityPage)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	public:

		void initialize(RadioResourceType type);
		void setRecentlyDialedRadioEntity(std::list<std::string>* recentDialedList);
		void setRadioResource(RadioResource* resource);

		//void setTelephoneResource(TA_IRS_Bus::TelephoneResource* resource)
		//{
		//	m_telresource = resource;
		//};

	// Implementation
	protected:

		//{{AFX_MSG(CRadioEntityPage)
		afx_msg void OnDropdownRadioEntityDescription();
		afx_msg void OnDropdownRadioEntityID();
		afx_msg void OnRadioEntityCall();
		afx_msg void OnRadioEntityRelease();
		afx_msg void OnRadioEntityDialpad();
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		 
	private:

		std::list<std::string>* m_recentDialedList;

		std::vector<RadioResource> m_resourceList;

		RadioResource* m_radioResource;

		//TA_IRS_Bus::TelephoneResource* m_telresource;

		bool m_launchedFromRadio;
};


#endif // !defined(AFX_CRADIOENTITYPAGE_H__FDFD7F10_C6B6_413B_89A9_FC9CB0B67B75__INCLUDED_)
