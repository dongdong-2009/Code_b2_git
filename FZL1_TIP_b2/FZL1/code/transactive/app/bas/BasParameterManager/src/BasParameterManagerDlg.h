
#if !defined(TA_GF_3002_app_bas_BasParameterManager_CBasParameterManagerDlg)
#define TA_GF_3002_app_bas_BasParameterManager_CBasParameterManagerDlg
#pragma once


#include "resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "app/bas/BasParameterManager/src/ParameterListCtrl.h"

namespace TA_IRS_App
{
	// CBasParameterManagerDlg dialog
	class BasParasManagerActionThread;
	class CBasParameterManagerDlg : public TA_Base_Bus::TransActiveDialog
	{
	// Construction
	public:
		CBasParameterManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
		enum { IDD = IDD_BASPARAMETERMANAGER_DIALOG };

		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	public:
		void prepareClose();
		void InitializeData();
		void FileterEquipment();
	// Implementation
	protected:
		HICON m_hIcon;

		// Generated message map functions
		virtual BOOL OnInitDialog();
		//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnBnClickedButtonResolve();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnParasValueSelectChange();
		afx_msg void OnParameterValueChange(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnParameterValueClick(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg LRESULT OnUpdateSelectedItemData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnUpdateButtonForSelectedItem (WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
	protected:
		void fillwithLocation();
		void fillwithValueType();
	private:
		void selectParameterValue();
		void setWindowCaption(bool busy);
		bool IsNumeric(const std::string& value);
		double stringToDouble(const std::string& value);
		void selectEquipmentInputValue();
	private:		
		CParameterListCtrl m_parametervalues;
		CComboBox m_location;
		CComboBox m_valueTypes;
		CEdit m_editLocation;
		CEdit m_editValueType;
		CEdit m_editValueTypeTag;
		CEdit m_editValue;
		CButton m_btnResolve;

		BasParasManagerActionThread* m_initializeThread;
		BasParasManagerActionThread* m_filterEquipmentThread;
		bool m_bIsRunningFileterEquipmentThread;
	};
}

#endif //TA_GF_3002_app_bas_BasParameterManager_CBasParameterManagerDlg
