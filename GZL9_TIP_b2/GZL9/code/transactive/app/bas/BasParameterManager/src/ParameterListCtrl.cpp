// src\ParameterListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BasParameterManager/src/ParameterListCtrl.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BasParameterManager/src/EquipmentManager.h"
#include "app/bas/BasParameterManager/src/BasParasManagerDataHelper.h"

namespace TA_IRS_App
{
	// CParameterListCtrl
	const std::string CParameterListCtrl::ColumnName_Location = "车站";
	const std::string CParameterListCtrl::ColumnName_ValueType = "设定值类型";
	const std::string CParameterListCtrl::ColumnName_ValueTypeTag = "设定值类型标签";
	const std::string CParameterListCtrl::ColumnName_Value = "预设值";
	const std::string CParameterListCtrl::ColumnName_Status = "执行状态";
	const int CParameterListCtrl::ColumnIndex_Location = 0;
	const int CParameterListCtrl::ColumnIndex_ValueType = 1;
	const int CParameterListCtrl::ColumnIndex_ValueTypeTag = 2;
	const int CParameterListCtrl::ColumnIndex_Value = 3;
	const int  CParameterListCtrl::ColumnIndex_Status = 4;
	const std::string CParameterListCtrl::m_Status_Success = "成功";
	const std::string CParameterListCtrl::m_Status_Processing = "处理中";
	const std::string CParameterListCtrl::m_Status_LCC_Failed = "LCC校验失败";
	const std::string CParameterListCtrl::m_Status_Comm_Failed = "通讯失败";
	const std::string CParameterListCtrl::m_Status_Failed = "失败";

	unsigned long CParameterListCtrl::m_currentLocationKey = 0xFFFFFFFF;
	unsigned long CParameterListCtrl::m_currentValueTypeKey = 0xFFFFFFFF;
	IMPLEMENT_DYNAMIC(CParameterListCtrl, CListCtrl)
	CParameterListCtrl::CParameterListCtrl()
	{
		m_equipmentManager = std::auto_ptr<EquipmentManager>(new EquipmentManager( this ));
		m_isPrepareToExist = false;
	}

	CParameterListCtrl::~CParameterListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(CParameterListCtrl, CListCtrl)
        ON_WM_SIZE()
    END_MESSAGE_MAP()



	// CParameterListCtrl message handlers
	void CParameterListCtrl::setupListCtrl()
	{
		InsertColumn(ColumnIndex_Location, ColumnName_Location.c_str(), LVCFMT_CENTER, 70);
		InsertColumn(ColumnIndex_ValueType, ColumnName_ValueType.c_str(), LVCFMT_CENTER, 150);
		InsertColumn(ColumnIndex_ValueTypeTag, ColumnName_ValueTypeTag.c_str(), LVCFMT_CENTER, 380);
		InsertColumn(ColumnIndex_Value, ColumnName_Value.c_str(), LVCFMT_CENTER, 100);
		InsertColumn(ColumnIndex_Status, ColumnName_Status.c_str(), LVCFMT_CENTER, 100);
		setRowFullSelect();
	}

	DWORD CParameterListCtrl::setRowFullSelect()
	{
		LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
		style = SetWindowLong(this->m_hWnd, GWL_STYLE, style | LVS_SINGLESEL);
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES );
		return SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	}

	void CParameterListCtrl::refreshParameterValues(unsigned long locationKey, unsigned long valueType)
	{
		FUNCTION_ENTRY("refreshParameterValues");
		using namespace TA_Base_Core;
		if (m_currentLocationKey == locationKey && m_currentValueTypeKey == valueType)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
				"the filter same as current value ignore locationKey=%d valueTypeKey=%d",locationKey,valueType);
			return;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
				"the filter is locationKey=%d valueTypeKey=%d current filter is locationKey=%d valueTypeKey=%d",
				locationKey,valueType,m_currentLocationKey,m_currentValueTypeKey);
		}
		
		DeleteAllItems(); // clean the control
		EquipmentMap equipmentMap;
		EquipmentVct equipments;
		m_equipmentManager->getLocationEquipmentMap(locationKey, equipmentMap);

		EquipmentMap::iterator itFoundEquip = equipmentMap.find(valueType);
		if (itFoundEquip != equipmentMap.end())
		{
			equipments = itFoundEquip->second;
		}

		ILocation_SmartPtr location = BasParasManagerDataHelperInstance::instance()->getLocationByKey(locationKey);
		if (NULL == location.get())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Don't get any location. locationKey=%d", locationKey);
			return;
		}

		CString strLocation =location->getDisplayName().c_str();

		for( EquipmentVct::iterator it = equipments.begin(); it != equipments.end(); it++)
		{
			
			CString strValueType = (*it)->getValueType().c_str();
			CString strParameterLable = (*it)->getEquipmentLable().c_str();
			CString strValue = "未定义";
			BAS_TRY
			{
				strValue = (*it)->getCurrentValue().c_str();
			}
			BAS_CATCH;			
			DWORD itemData = (*it)->getEquipmentKey();
			int index = InsertItem(GetItemCount(), strLocation);
			SetItemText(index, ColumnIndex_ValueType, strValueType);
			SetItemText(index, ColumnIndex_ValueTypeTag, strParameterLable);
			SetItemText(index, ColumnIndex_Value, strValue);
			SetItemText(index, ColumnIndex_Status, (*it)->getExecuteStatus().c_str());
			SetItemData(index, itemData);
		}
		
		FUNCTION_EXIT;
	}

	void CParameterListCtrl::UpdateDatapointValue(unsigned long datapointKey, const std::string& newValue)
	{
		FUNCTION_ENTRY("UpdateDatapointValue");

		if (m_isPrepareToExist)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_isPrepareToExist flag is set to true, app will quit.");
			FUNCTION_EXIT;
			return;
		}

		for (int index = 0; index < GetItemCount(); index++)
		{
			if (GetItemData(index) == datapointKey)
			{

				SetItemText(index, ColumnIndex_Value, newValue.c_str());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"The data point have update the value in GUI DataPointKey:%lu NewValue:%s",
					datapointKey, newValue.c_str());

				CWnd* parentDlg = GetParent();
				if (NULL != parentDlg && IsWindow(parentDlg->m_hWnd))
				{
					parentDlg->PostMessage(Msg_UpdateSelectedItemValue, datapointKey, index);
				}
				FUNCTION_EXIT;
				return;
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"datapoint value update not successfully, not found the datapoint in ListCtrl.datapointKey=%lu value=%s",
				datapointKey, newValue.c_str());
		FUNCTION_EXIT;
	}

	void CParameterListCtrl::UpdateDatapointStatus(unsigned long datapointKey, const std::string& strStatus)
	{
		FUNCTION_ENTRY("UpdateDatapointStatus");

		if (m_isPrepareToExist)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_isPrepareToExist flag is set to true, app will quit.");
			FUNCTION_EXIT;
			return;
		}

		for (int index = 0; index < GetItemCount(); index++)
		{
			if (GetItemData(index) == datapointKey)
			{

				SetItemText(index, ColumnIndex_Status, strStatus.c_str());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "The data point have update the status in GUI DataPointKey:%lu NewStatus:%s", datapointKey, strStatus.c_str());

				CWnd* parentDlg = GetParent();
				if (NULL != parentDlg && IsWindow(parentDlg->m_hWnd))
				{
					parentDlg->PostMessage(Msg_UpdateResolveButtonStatus,datapointKey,index);
				}
			}
		}
		FUNCTION_EXIT;
	}

	void CParameterListCtrl::prepareToClose()
	{
		m_isPrepareToExist = true;
		m_equipmentManager->prepareToExist();
	}

	void CParameterListCtrl::setEquipmentValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double value)
	{
		m_equipmentManager->setEquipmentValue(locationKey, typeKey, entityKey, value);
	}

	bool CParameterListCtrl::getEquipmentLimitedValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double& lowValue, double& highValue)
	{
		return m_equipmentManager->getEquipmentLimitedValue(locationKey, typeKey, entityKey, lowValue, highValue);
	}

	void CParameterListCtrl::setExecuteStatus( unsigned long locationKey,unsigned long typeKey, unsigned long entityKey, const std::string& strStatus )
	{
		m_equipmentManager->setExecuteStatus(locationKey, typeKey, entityKey, strStatus);
	}

}

//void TA_IRS_App::CParameterListCtrl::OnSize(UINT nType, int cx, int cy)
//{
//    __super::OnSize(nType, cx, cy);
//
//    // reset column width
//    SetRedraw(FALSE);
//    CRect listCtrlRect;
//    GetWindowRect(&listCtrlRect);
//    int listctrlWidth = listCtrlRect.Width();
//
//    if ( 0 == listctrlWidth )
//    {
//        return;
//    }
//
//    bool displayAllColumn; //auto display the all column header
//    int listctrlHeaderWidth = 0;
//    int headerColumnCount = GetHeaderCtrl()->GetItemCount();
//    if (headerColumnCount > 0)
//    {
//        for (int index = 0; index < headerColumnCount; index++)
//        {
//            CRect headerItemRect;
//            GetHeaderCtrl()->GetItemRect(index, &headerItemRect);
//            listctrlHeaderWidth += headerItemRect.Width();
//        }
//    }
//
//
//    // change the column width
//    int tempWidth = 0;
//    for (int index = 0; index < headerColumnCount; index++)
//    {
//        CRect headerItemRect;
//        GetHeaderCtrl()->GetItemRect(index, &headerItemRect);
//
//        tempWidth = listctrlWidth * ((double)headerItemRect.Width() / listctrlHeaderWidth);
//
//        SetColumnWidth(index, tempWidth);
//    }
//    SetRedraw(TRUE);
//}
