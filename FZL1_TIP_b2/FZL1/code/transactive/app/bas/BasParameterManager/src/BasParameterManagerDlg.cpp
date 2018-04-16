// BasParameterManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BasParameterManager/src/BasParameterManager.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerDlg.h"
#include "app/bas/BasParameterManager/src/CommonDefine.h"
#include "app/bas/BasParameterManager/src/BasParasManagerDataHelper.h"
#include "app/bas/BasParameterManager/src/BasParasManagerActionThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace TA_IRS_App
{
	// CAboutDlg dialog used for App About
	class CAboutDlg : public CDialog
	{
	public:
		CAboutDlg();

	// Dialog Data
		enum { IDD = IDD_ABOUTBOX };

		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
	protected:
		DECLARE_MESSAGE_MAP()
	};

	CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
	{
	}

	void CAboutDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}

	BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	END_MESSAGE_MAP()


	// CBasParameterManagerDlg dialog
	CBasParameterManagerDlg::CBasParameterManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent /*=NULL*/)
		: TA_Base_Bus::TransActiveDialog(controlClass, CBasParameterManagerDlg::IDD, pParent)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise	= true;
		properties.maxHeight	= -1;
		properties.maxWidth		= -1;
		properties.minWidth		= 800;
		properties.minHeight	= 600;
		setResizingProperties(properties);

		m_filterEquipmentThread = NULL;
		m_bIsRunningFileterEquipmentThread = false;
		m_initializeThread = NULL;
	}

	void CBasParameterManagerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TA_Base_Bus::TransActiveDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST_PARAMETER, m_parametervalues);
		DDX_Control(pDX, IDC_COMBO_LOCATION, m_location);
		DDX_Control(pDX, IDC_COMBO_VALUE_TYPE, m_valueTypes);
		DDX_Control(pDX, IDC_EDIT_STATION, m_editLocation);
		DDX_Control(pDX, IDC_EDIT_PARAMETER_TYPE, m_editValueType);
		DDX_Control(pDX, IDC_EDIT_PARAMETER_TYPE_TAG, m_editValueTypeTag);
		DDX_Control(pDX, IDC_EDIT_PARAMETER_VALUE, m_editValue);
		DDX_Control(pDX, IDC_BUTTON_RESOLVE,m_btnResolve);
	}

	BEGIN_MESSAGE_MAP(CBasParameterManagerDlg, TA_Base_Bus::TransActiveDialog)
		ON_WM_SYSCOMMAND()
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()
		ON_WM_SIZE()
		ON_BN_CLICKED(IDC_BUTTON_RESOLVE, &CBasParameterManagerDlg::OnBnClickedButtonResolve)
		ON_CBN_SELCHANGE(IDC_COMBO_VALUE_TYPE, CBasParameterManagerDlg::OnParasValueSelectChange)
		ON_CBN_SELCHANGE(IDC_COMBO_LOCATION, CBasParameterManagerDlg::OnParasValueSelectChange)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARAMETER, &CBasParameterManagerDlg::OnParameterValueChange)
		ON_NOTIFY(NM_CLICK, IDC_LIST_PARAMETER, &CBasParameterManagerDlg::OnParameterValueClick)
		ON_MESSAGE(Msg_UpdateSelectedItemValue, &CBasParameterManagerDlg::OnUpdateSelectedItemData)
		ON_MESSAGE(Msg_UpdateResolveButtonStatus, &CBasParameterManagerDlg::OnUpdateButtonForSelectedItem)
		//}}AFX_MSG_MAP	
	END_MESSAGE_MAP()


	// CBasParameterManagerDlg message handlers
	BOOL CBasParameterManagerDlg::OnInitDialog()
	{
		TA_Base_Bus::TransActiveDialog::OnInitDialog();

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

        m_editLocation.SetReadOnly( TRUE );
        m_editValueType.SetReadOnly( TRUE );
        m_editValueTypeTag.SetReadOnly( TRUE );

		// TODO: Add extra initialization here
		BasParasManagerActionThread* m_initializeThread = new BasParasManagerActionThread(this);
		m_initializeThread->setAction(BasParameterManagerAction_Initialize);
		m_initializeThread->start();
		
		// set up list
		PostMessage(WM_SIZE, 0, 0);
		m_parametervalues.setupListCtrl();

		setWindowCaption(false);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnInitDialog");
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

// 	void CBasParameterManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
// 	{
// 		if ((nID & 0xFFF0) == IDM_ABOUTBOX)
// 		{
// 			CAboutDlg dlgAbout;
// 			dlgAbout.DoModal();
// 		}
// 		else
// 		{
// 			CDialog::OnSysCommand(nID, lParam);
// 		}
// 	}

	// If you add a minimize button to your dialog, you will need the code below
	//  to draw the icon.  For MFC applications using the document/view model,
	//  this is automatically done for you by the framework.

	void CBasParameterManagerDlg::OnPaint()
	{
		if (IsIconic())
		{
			CPaintDC dc(this); // device context for painting

			SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

			// Center icon in client rectangle
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Draw the icon
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			CDialog::OnPaint();
		}
	}

	// The system calls this function to obtain the cursor to display while the user drags
	//  the minimized window.
	HCURSOR CBasParameterManagerDlg::OnQueryDragIcon()
	{
		return static_cast<HCURSOR>(m_hIcon);
	}

	LRESULT CBasParameterManagerDlg::OnUpdateSelectedItemData(WPARAM wParam, LPARAM lParam)
	{
		unsigned long itemData = static_cast<unsigned long>(wParam);
		int item = static_cast<int>(lParam);

		if (item < m_parametervalues.GetItemCount() && (m_parametervalues.GetItemData(item) == itemData))
		{
			selectParameterValue();
		}
		return 1;
	}

	LRESULT CBasParameterManagerDlg::OnUpdateButtonForSelectedItem(WPARAM wParam, LPARAM lParam)
	{
		unsigned long itemData = static_cast<unsigned long>(wParam);
		int item = static_cast<int>(lParam);

		if (0 == _strcmpi(m_parametervalues.GetItemText(item,CParameterListCtrl::ColumnIndex_Status),CParameterListCtrl::m_Status_Processing.c_str()))
		{
			m_btnResolve.EnableWindow(FALSE);
		}
		else
		{
			m_btnResolve.EnableWindow(TRUE);
		}

		return 1;
	}

	void CBasParameterManagerDlg::OnParameterValueChange(NMHDR *pNMHDR, LRESULT *pResult)
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		// TODO: Add your control notification handler code here
		selectParameterValue();
		*pResult = 0;
	}
	void CBasParameterManagerDlg::OnParameterValueClick(NMHDR *pNMHDR, LRESULT *pResult)
	{
		POSITION pos = m_parametervalues.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			return;
		}

		int item = m_parametervalues.GetNextSelectedItem(pos);
		if (item >= m_parametervalues.GetItemCount())
		{
			return;
		}

		if (0 == _strcmpi(m_parametervalues.GetItemText(item,CParameterListCtrl::ColumnIndex_Status),CParameterListCtrl::m_Status_Processing.c_str()))
		{
			m_btnResolve.EnableWindow(FALSE);
		}
		else
		{
            m_btnResolve.EnableWindow(TRUE);
		}
		selectParameterValue();
		*pResult = 0;
	}
	void CBasParameterManagerDlg::OnBnClickedButtonResolve()
	{
		// TODO: Add your control notification handler code here
		POSITION pos = m_parametervalues.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			MessageBox("请先选择一个要设置的设备", MessageBoxCaption, MB_OK | MB_ICONERROR);
			return;
		}
		int item = m_parametervalues.GetNextSelectedItem(pos);
		if (item >= m_parametervalues.GetItemCount())
		{
			return;
		}

		CString strNewValueString;
		m_editValue.GetWindowText(strNewValueString);
		std::string strNewValue = strNewValueString.GetBuffer();
		if (!IsNumeric(strNewValue))
		{
			MessageBox("将要设置的数据不正确，请修改", MessageBoxCaption, MB_OK | MB_ICONERROR);
			selectEquipmentInputValue();
			return;
		}

		unsigned long locationKey = m_location.GetItemData(m_location.GetCurSel());
		unsigned long entityKey = m_parametervalues.GetItemData(item);
		unsigned long typeKey = m_valueTypes.GetItemData(m_valueTypes.GetCurSel());

		double lowValue = 0;
		double highValue = 0;
		if (!m_parametervalues.getEquipmentLimitedValue(locationKey, typeKey, entityKey, lowValue, highValue))
		{
			double value = stringToDouble(strNewValue);
			if ((value < lowValue) || (value > highValue))
			{
				CString strMsg;
				strMsg.Format("将要设置的数据超出有效值范围，有效值应该是 %f ≤ 有效值 ≤ %f", lowValue, highValue);
				MessageBox(strMsg, MessageBoxCaption, MB_OK | MB_ICONERROR);
				selectEquipmentInputValue();
				return;
			}
		}

		CString strFirstConfirmMsg = "确信要更改此设备的值吗？";
		CString strSecondConfirmMsg = "接下来将要修改设备的值，是否继续？";
		if (IDYES == MessageBox(strFirstConfirmMsg, MessageBoxCaption, MB_YESNO | MB_ICONQUESTION))
		{
			if (IDYES == MessageBox(strSecondConfirmMsg, MessageBoxCaption, MB_YESNO | MB_ICONWARNING))
			{
				BAS_TRY
				{

					CString strEquipment = m_parametervalues.GetItemText(item, CParameterListCtrl::ColumnIndex_ValueTypeTag);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "prepare to set datapoint value locationKey[%d], equipment[%s], new value[%s]",
								locationKey, strEquipment.GetBuffer(), strNewValue.c_str());
					m_parametervalues.setEquipmentValue(locationKey, typeKey, entityKey, stringToDouble(strNewValue));
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "success to set datapoint value locationKey[%d], equipment[%s], new value[%s]",
							locationKey, strEquipment.GetBuffer(), strNewValue.c_str());
				}
				BAS_CATCH;
				if (BAS_WhetherHaveError)
				{
					MessageBox("设置数据值失败", MessageBoxCaption, MB_OK | MB_ICONERROR);
					selectEquipmentInputValue();
					return;
				}

                m_parametervalues.SetItemText(item,CParameterListCtrl::ColumnIndex_Status,CParameterListCtrl::m_Status_Processing.c_str());
                m_parametervalues.setExecuteStatus(locationKey,typeKey,entityKey,CParameterListCtrl::m_Status_Processing.c_str());
                m_btnResolve.EnableWindow(FALSE);
			}
		}
	}

	void CBasParameterManagerDlg::prepareClose()
	{
		FUNCTION_ENTRY("prepareClose");
		m_parametervalues.prepareToClose();
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::OnSize(UINT nType, int cx, int cy)
	{
		FUNCTION_ENTRY("OnSize");
		If_Not_Valid_Window_Return_Void(this);
		if (false == BAS_IsWindow(IDC_LIST_PARAMETER) ||
			false == BAS_IsWindow(IDC_STATIC_LOCATION) || 
			false == BAS_IsWindow(IDC_EDIT_STATION) )
		{
			return;
		}
		m_editLocation.Invalidate(TRUE);
		m_editValueType.Invalidate(TRUE);
		m_editValueTypeTag.Invalidate(TRUE);

		CRect clientRect;
		GetClientRect(&clientRect);
		// Move the right list ctrl
		CRect parameterListRect;
		m_parametervalues.GetWindowRect(&parameterListRect); 
		ScreenToClient(&parameterListRect);
		parameterListRect.right = clientRect.right - Dlg_Right_Blank_Size * 2;
		parameterListRect.bottom = clientRect.bottom - Dlg_Bottom_Blank_Size - 50;		
		m_parametervalues.MoveWindow(parameterListRect);
		// Move the right bottom ctrls
		CPoint DatumPoint(parameterListRect.left, parameterListRect.bottom);
		int cx_offset = 0; int cy_offset = 0; 
		CRect CStaticText;
		GetDlgItem(IDC_STATIC_LOCATION)->GetWindowRect(&CStaticText);
		ScreenToClient(&CStaticText);
		cx_offset = 0; cy_offset = Dlg_Bottom_Blank_Size;
		BAS_MoveFixedSizeCtrl(IDC_STATIC_LOCATION, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = 0; cy_offset = Dlg_Bottom_Blank_Size + CStaticText.Height();
		BAS_MoveFixedSizeCtrl(IDC_EDIT_STATION, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = CStaticText.Width()+Dlg_Right_Blank_Size*2; cy_offset = Dlg_Bottom_Blank_Size;
		BAS_MoveFixedSizeCtrl(IDC_STATIC_PARAMETER_TYPE, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = CStaticText.Width()+Dlg_Right_Blank_Size*2; cy_offset = Dlg_Bottom_Blank_Size + CStaticText.Height();
		BAS_MoveFixedSizeCtrl(IDC_EDIT_PARAMETER_TYPE, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = (CStaticText.Width()+Dlg_Right_Blank_Size*2)*2; cy_offset = Dlg_Bottom_Blank_Size;
		BAS_MoveFixedSizeCtrl(IDC_STATIC_PARAMETER_TYPE_TAG, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = (CStaticText.Width()+Dlg_Right_Blank_Size*2)*2; cy_offset = Dlg_Bottom_Blank_Size + CStaticText.Height();
		BAS_MoveFixedSizeCtrl(IDC_EDIT_PARAMETER_TYPE_TAG, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = (CStaticText.Width()+Dlg_Right_Blank_Size*2)*4; cy_offset = Dlg_Bottom_Blank_Size;
		BAS_MoveFixedSizeCtrl(IDC_STATIC_PARAMETER_VALUE, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		cx_offset = (CStaticText.Width()+Dlg_Right_Blank_Size*2)*4; cy_offset = Dlg_Bottom_Blank_Size + CStaticText.Height();
		BAS_MoveFixedSizeCtrl(IDC_EDIT_PARAMETER_VALUE, DatumPoint.x+cx_offset, DatumPoint.y+cy_offset);
		CRect buttonRect;
		m_btnResolve.GetWindowRect(&buttonRect);
		cx_offset = clientRect.right-Dlg_Right_Blank_Size * 2 - buttonRect.Width(); cy_offset = Dlg_Bottom_Blank_Size + CStaticText.Height();
		BAS_MoveFixedSizeCtrl(IDC_BUTTON_RESOLVE, cx_offset, DatumPoint.y+cy_offset);
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::InitializeData()
	{
		FUNCTION_ENTRY("InitializeData");
		BAS_TRY
		{
			fillwithLocation();
			fillwithValueType();
		}
		BAS_CATCH;
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::FileterEquipment()
	{
		FUNCTION_ENTRY("FileterEquipment");

		int locationCurSel = m_location.GetCurSel();
		int valueTypeCurSel = m_valueTypes.GetCurSel();
		if (CB_ERR == locationCurSel || CB_ERR == valueTypeCurSel)
		{
			return;
		}
		unsigned long curLocationKey = static_cast<unsigned long>(m_location.GetItemData(locationCurSel));
		unsigned long curSelItemKey = static_cast<unsigned long>(m_valueTypes.GetItemData(valueTypeCurSel));

		BAS_TRY
		{
			ApplicationBusyIndicator indicator;
			m_parametervalues.refreshParameterValues(curLocationKey, curSelItemKey);
		}
		BAS_CATCH;
		if (BAS_WhetherHaveError)
		{
			MessageBox(BAS_GetExceptionString, MessageBoxCaption, MB_ICONERROR | MB_OK);
		}
		m_bIsRunningFileterEquipmentThread = false;
		setWindowCaption(false);
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::fillwithLocation()
	{
		FUNCTION_ENTRY("fillwithLocation");
		BAS_TRY
		{
			m_location.Clear();
			if (BasParasManagerDataHelperInstance::instance()->isAtStation())
			{
				ILocation_SmartPtr LocationPtr = BasParasManagerDataHelperInstance::instance()->getLocalLocation();
				if ( NULL != LocationPtr.get() )
				{
					int index= m_location.InsertString(m_location.GetCount(), _T(LocationPtr->getDisplayName().c_str()));
					m_location.SetItemData(index, LocationPtr->getKey());
				}
				else
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Local Location is not loaded" );
				}
			}
			else
			{
				std::vector<ILocation_SmartPtr> locations = BasParasManagerDataHelperInstance::instance()->getAllLocations();
	
				std::vector<ILocation_SmartPtr>::iterator it;
				for (it = locations.begin(); it != locations.end(); it++)
				{
					if( (*it)->getLocationType() == TA_Base_Core::ILocation::STATION )
					{
						int index = m_location.InsertString(m_location.GetCount(), (*it)->getDisplayName().c_str());
						m_location.SetItemData(index, (*it)->getKey());
					}
				}
			}

            if ( m_location.GetCount() > 0 )
            {
                m_location.SetCurSel(0);
            }

			// select the local location
		}
		BAS_CATCH;
		
		if (BAS_WhetherHaveError)
		{
			MessageBox(BAS_GetExceptionString, MessageBoxCaption, MB_OK | MB_ICONERROR);
		}
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::fillwithValueType()
	{
		FUNCTION_ENTRY("fillwithValueType");
		TA_IRS_Core::ParameterValueTypeInfoVct valueTypeInfo;
		BasParasManagerDataHelperInstance::instance()->getParameterValueType(valueTypeInfo);
		TA_IRS_Core::ParameterValueTypeInfoVct::iterator it;
		int i = 0;
		for (it = valueTypeInfo.begin(); it != valueTypeInfo.end(); it++, i++)
		{
			m_valueTypes.InsertString(i, (*it).name.c_str());
			m_valueTypes.SetItemData(i, (*it).pkey);
		}
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::OnParasValueSelectChange()
	{
		FUNCTION_ENTRY("OnParasValueSelectChange");
		using namespace TA_Base_Core;
		if (m_location.GetCurSel() == CB_ERR || m_valueTypes.GetCurSel() == CB_ERR)
		{
			return;
		}

		if(m_bIsRunningFileterEquipmentThread)
		{
			if (MessageBox("程序正在查找符合条件的参数列表,请稍后再试.", MessageBoxCaption, MB_OK | MB_ICONWARNING))
			{
				return;
			}
		}
		BasParasManagerActionThread* m_filterEquipmentThread = new BasParasManagerActionThread(this);
		m_filterEquipmentThread->setAction(BasParameterManagerAction_FilterEquipment);
		m_filterEquipmentThread->start();

		setWindowCaption(true);
		m_bIsRunningFileterEquipmentThread = true;
		FUNCTION_EXIT;
	}

	void CBasParameterManagerDlg::selectParameterValue()
	{
		POSITION position = m_parametervalues.GetFirstSelectedItemPosition();
		while(NULL != position)
		{
			int nItem = m_parametervalues.GetNextSelectedItem(position);
           
			m_editLocation.SetWindowText(m_parametervalues.GetItemText(nItem, CParameterListCtrl::ColumnIndex_Location));
			m_editValueType.SetWindowText(m_parametervalues.GetItemText(nItem, CParameterListCtrl::ColumnIndex_ValueType));
			m_editValueTypeTag.SetWindowText(m_parametervalues.GetItemText(nItem, CParameterListCtrl::ColumnIndex_ValueTypeTag));
			m_editValue.SetWindowText(m_parametervalues.GetItemText(nItem, CParameterListCtrl::ColumnIndex_Value));
			break;
		}
	}

	void CBasParameterManagerDlg::setWindowCaption(bool busy)
	{
		CString strCaption = "设备参数管理器";
		if (busy)
		{
			strCaption = strCaption + " - 正在筛选符合条件的设备";
		}
		SetWindowText(strCaption);
	}

	bool CBasParameterManagerDlg::IsNumeric(const std::string& value)
	{
		for(size_t i = 0; i < value.length(); i++)
		{
			if(value[i] != '.' && ( value[i] < '0' || value[i] > '9'))
			{
				return false;
			}
		}
		return true;
	}

	double CBasParameterManagerDlg::stringToDouble(const std::string& value)
	{
		std::stringstream ss;
		ss << value;
		double returnValue;
		ss >> returnValue;
		return returnValue;
	}

	void CBasParameterManagerDlg::selectEquipmentInputValue()
	{
		CString strEquipmentValue;
		m_editValue.GetWindowText(strEquipmentValue);
		m_editValue.SetFocus();
		m_editValue.SetSel(0, -1);
	}
};
