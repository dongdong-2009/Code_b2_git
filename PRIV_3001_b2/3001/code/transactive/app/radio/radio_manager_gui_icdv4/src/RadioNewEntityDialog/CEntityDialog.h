/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioNewEntityDialog/CEntityDialog.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Allows the operator to create new entities of a chosen resource type
 */

#if !defined(AFX_CENTITYDIALOG_H__B7B8D92A_5544_4402_AD32_9D442FD980D3__INCLUDED_)
#define AFX_CENTITYDIALOG_H__B7B8D92A_5544_4402_AD32_9D442FD980D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/radio/radio_bus/src/RadioTypes.h"

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CEntityDialog dialog

class CEntityDialog : public CDialog
{
	// Construction
	public:
		CEntityDialog(CWnd* pParent = NULL);   


		//{{AFX_DATA(CEntityDialog)
		enum { IDD = IDD_NEW_ENTITY_DLG };
		CComboBox	m_entityTypeCB;
		CEdit	m_descriptionEL;
		//}}AFX_DATA



		//{{AFX_VIRTUAL(CEntityDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL



	protected:
		//{{AFX_MSG(CEntityDialog)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		virtual void OnCancel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


	private:
//TD18869
		bool bCheckifExisting(RadioResource &);
		bool bsaveOnDatabase(RadioResource &);

//TD18869
		
		/*
		 * list of creatable resource types
		 */
		std::vector<RadioResourceType> m_resourceTypelist;
};


#endif // !defined(AFX_CENTITYDIALOG_H__B7B8D92A_5544_4402_AD32_9D442FD980D3__INCLUDED_)
