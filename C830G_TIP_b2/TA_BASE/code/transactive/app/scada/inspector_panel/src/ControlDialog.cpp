/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/ControlDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which displays the control information tab shown
  * by the inspector panel.  It allows the operator to set the values of the data points.
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include <sstream>

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/ConfirmCommandDialog.h"
#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ControlDialog.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CControlDialog dialog


	CControlDialog::CControlDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CControlDialog::IDD, pParent), m_heightDifference(0), /*m_accessAllowed(false)*/
		  m_originalHeight (0)
	{
		FUNCTION_ENTRY ("CControlDialog()");

		//{{AFX_DATA_INIT(CControlDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT

		FUNCTION_EXIT;
	}


	void CControlDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("~CControlDialog()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CControlDialog)
			// NOTE: the ClassWizard will add DDX and DDV calls here
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CControlDialog, CDialog)
		//{{AFX_MSG_MAP(CControlDialog)
		ON_WM_SHOWWINDOW()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CControlDialog message handlers


	BOOL CControlDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CDialog::OnInitDialog();
		
		m_list.SubclassDlgItem(IDC_CONTROLLIST,this);

		FUNCTION_EXIT;

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	// refresh datapoints when the window was shown
	void CControlDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
		
		// TODO: Add your message handler code here

		if (bShow)
		{
			m_list.refreshDataPoints();
		}
		else
		{
			::SendMessage(m_list.m_hWnd,WM_KILLFOCUS,0,0);
		}

		FUNCTION_EXIT;
	}

	void CControlDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_list.setEquipment(equipment);

		FUNCTION_EXIT;
	}

	void CControlDialog::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		m_list.updateDataPoint(key, updateType);

		FUNCTION_EXIT;
	}

	void CControlDialog::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		m_list.updateDataNode(updateType);

		FUNCTION_EXIT;
	}

	bool CControlDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
		FUNCTION_EXIT;

		return false;
	}

	bool CControlDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");
		FUNCTION_EXIT;

		return true;
	}

	void CControlDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CControlDialog::getMinimumHeight()
	{
		if (m_heightDifference == 0)
		{
			CRect listRect, dialogRect;
			m_list.GetWindowRect(&listRect);
			GetWindowRect(&dialogRect);

			listRect.NormalizeRect();
			dialogRect.NormalizeRect();

			m_heightDifference = dialogRect.Height() - listRect.Height();
		}

		if (m_originalHeight == 0)
		{
			CRect dialogRect;
			GetWindowRect(&dialogRect);
			dialogRect.NormalizeRect();

			m_originalHeight = dialogRect.Height();
		}

		unsigned int nItem = m_list.GetItemCount();
		unsigned int height = m_heightDifference;

		if (nItem > 0)
		{
			if (nItem > 20)
			{
				nItem = 20;
			}

			CRect cellRect;
			m_list.GetSubItemRect(0, 0, LVIR_LABEL, cellRect);

			cellRect.NormalizeRect();

//			height += cellRect.Height()*(nItem+3);
			height += cellRect.Height()*(nItem+1)+9; // remove half line
		}
		else
		{
			height = m_originalHeight;
		}

		return height;
	}		

	void CControlDialog::resize(unsigned int height)
	{
		CRect listRect, dialogRect;
		m_list.GetWindowRect(&listRect);
		GetWindowRect(&dialogRect);

		listRect.NormalizeRect();
		dialogRect.NormalizeRect();

		int diff = height - dialogRect.Height();

		if (diff > 0)
		{
			listRect.InflateRect(0, 0, 0, diff);
			dialogRect.InflateRect(0, 0, 0, diff);
		}
		else
		{
			listRect.DeflateRect(0, 0, 0, -diff);
			dialogRect.DeflateRect(0, 0, 0, -diff);
		}

		m_list.SetWindowPos(NULL, 0, 0, listRect.Width(), listRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
	}

	void CControlDialog::updatePermissions()
	{
		m_list.updatePermissions();
	}

	LRESULT CControlDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{				
		if ((message == WM_COMMAND) && ((wParam == IDOK) || (wParam == IDCANCEL)))
		{
			return 0;
		} else
			return CDialog::WindowProc(message, wParam, lParam);
	}

	bool CControlDialog::isAnyDataPointsDisplayedInTab() const
	{
		// check if any data point is displayed in the list control
		return m_list.isAnyDataPointsDisplayedOnScreen();
	}

	void CControlDialog::resetList()
	{

	}
}
