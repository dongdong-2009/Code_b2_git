/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/AssetDialog.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(AFX_ASSETDIALOG_H__9466647F_3A43_4358_8CB7_17FB600DAFAB__INCLUDED_)
#define AFX_ASSETDIALOG_H__9466647F_3A43_4358_8CB7_17FB600DAFAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AssetDialog.h : header file
//
#include "app/incidents/incident_manager/src/resource.h"
#include "core/data_access_interface/incident_dai/src/AssetType.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"

/////////////////////////////////////////////////////////////////////////////
// AssetDialog dialog
using namespace TA_IRS_Core;

namespace TA_IRS_App
{
	
	class AssetDialog : public CDialog
	{
		
	public:
		// Construction
		AssetDialog(bool editable, AssetType& asset, CWnd* pParent = NULL);   // user constructor
		
		// Dialog Data
		//{{AFX_DATA(AssetDialog)
		enum { IDD = IDD_ASSET_DIALOG };
		TA_Base_Bus::CScadaTreeCtrl m_AssetTree;
		CButton	m_okBtn;
		
		//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(AssetDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(AssetDialog)
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL OnInitDialog();
		afx_msg void OnClickAssetTypeTree(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	public:
	/**
	* Returns the selected asset.
	* @return the selected asset.
		*/
		AssetType getSelectedAsset() const;
		
	private:
		bool    m_editable;
		AssetType m_currentAsset;
	};
	
}// end TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSETDIALOG_H__9466647F_3A43_4358_8CB7_17FB600DAFAB__INCLUDED_)

