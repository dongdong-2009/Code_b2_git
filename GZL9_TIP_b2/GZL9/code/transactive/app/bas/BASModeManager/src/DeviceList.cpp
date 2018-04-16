// src\DeviceList.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/DeviceList.h"

namespace TA_IRS_App
{
	const unsigned long CDeviceList::m_COLUMN_INDEX_DEVICENUM 		= 0;
	const unsigned long CDeviceList::m_COLUMN_INDEX_DEVICENAME 		= 1;
	const unsigned long CDeviceList::m_COLUMN_INDEX_AUTOMANUAL 		= 2;
	const unsigned long CDeviceList::m_COLUMN_INDEX_CURRENTSTATUS 	= 3;
	const unsigned long CDeviceList::m_COLUMN_INDEX_EXPECTSTATUS 	= 4;

	const std::string CDeviceList::m_HEADER_DEVICENUM 		= "设备编码";
	const std::string CDeviceList::m_HEADER_DEVICENAME 		= "设备名称";
	const std::string CDeviceList::m_HEADER_AUTOMANUAL 		= "手/自动状态";
	const std::string CDeviceList::m_HEADER_CURRENTSTATUS 	= "当前状态";
	const std::string CDeviceList::m_HEADER_EXPECTSTATUS 	= "预期状态";
	#define COLOR_RED RGB(255,0,0)
	#define COLOR_BLACK RGB(0,0,0)
	// CDeviceList
	IMPLEMENT_DYNAMIC(CDeviceList, BASBaseListCtrl)
	CDeviceList::CDeviceList()
	{
		m_isStoploading = false;
	}

	CDeviceList::~CDeviceList()
	{
	}


	BEGIN_MESSAGE_MAP(CDeviceList, BASBaseListCtrl)
		ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
		ON_MESSAGE(Msg_UpdateDataPointValue, &CDeviceList::OnUpdateDatapointValue)//like++. GF-78.

	END_MESSAGE_MAP()



	// CDeviceList message handlers
	void CDeviceList::Initialise()
	{
		FUNCTION_ENTRY("Initialise");
		setupListCtrl();
		FUNCTION_EXIT;
	}

	void CDeviceList::setupListCtrl()
	{
		FUNCTION_ENTRY("setupListCtrl");
		InsertColumn(m_COLUMN_INDEX_DEVICENUM, _T(m_HEADER_DEVICENUM.c_str()), LVCFMT_CENTER, 70);
		InsertColumn(m_COLUMN_INDEX_DEVICENAME, _T(m_HEADER_DEVICENAME.c_str()), LVCFMT_CENTER, 250);
		InsertColumn(m_COLUMN_INDEX_AUTOMANUAL, _T(m_HEADER_AUTOMANUAL.c_str()), LVCFMT_CENTER, 60);
		InsertColumn(m_COLUMN_INDEX_CURRENTSTATUS, _T(m_HEADER_CURRENTSTATUS.c_str()), LVCFMT_CENTER, 60);
		InsertColumn(m_COLUMN_INDEX_EXPECTSTATUS, _T(m_HEADER_EXPECTSTATUS.c_str()), LVCFMT_CENTER, 60);
		FUNCTION_EXIT;
	}

	void CDeviceList::fillwithModeDevice(unsigned long locationKey, unsigned long modeKey)
	{
		FUNCTION_ENTRY("fillwithModeDevice");
		m_locationKey = locationKey;
		m_modeKey = modeKey;
		using namespace TA_IRS_Core;
		ModeDevices modeDevices = BasManagerDataHelperInstance::instance()->getModeDevicesByModeKey(locationKey, modeKey);
		DeleteAllItems();
		int index = 0;
		for (ModeDevicesIter it = modeDevices.begin(); it != modeDevices.end(); it++, index++)
		{
			index = InsertItem(index, (*it).deviceName.c_str());
			SetItemText(index,m_COLUMN_INDEX_DEVICENAME,(*it).description.c_str());
			SetItemText(index,m_COLUMN_INDEX_EXPECTSTATUS,(*it).expectStatus.c_str());
			SetItemData(index, static_cast<DWORD>((*it).pkey));
		}

		DeviceEquipmentManagerInstance::instance()->resetListener(this);

		EquipmentVct equipmentVct;
		DeviceEquipmentManagerInstance::instance()->getLocationDeviceEquipments(locationKey, modeKey, equipmentVct);
		index = 0;
		for (EquipmentVct::iterator it = equipmentVct.begin(); it != equipmentVct.end(); it++)
		{
			if (IsWindow(this->m_hWnd) && !m_isStoploading)
			{
				index = getItemIndexByData((*it)->getDeviceKey());
				if (index >= 0 && index < GetItemCount())
				{
					std::string strAutoManual = (*it)->getDeviceEquipmentAutoDpTag().empty() ? "" : (*it)->getCurrentAutoValue();
					std::string strStatus = (*it)->getCurrentStatusValue();
					SetItemText(index, m_COLUMN_INDEX_AUTOMANUAL, strAutoManual.c_str());
					SetItemText(index, m_COLUMN_INDEX_CURRENTSTATUS, strStatus.c_str());
					refreshItemGUI(index);
				}
			}
		}
		FUNCTION_EXIT;
	}

	bool  CDeviceList::stopFillwithModeDevice()
	{
		m_isStoploading = true;
		return DeviceEquipmentManagerInstance::instance()->stopLoadDeviceEquipments();
	}
	/*//like++. GF-78.
	*wParam: NULL.
	*lParam: PDATAPOINTINFO.
	*/
	LRESULT CDeviceList::OnUpdateDatapointValue(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("OnUpdateDatapointValue");

		PDATAPOINTINFO pInfo = reinterpret_cast<PDATAPOINTINFO>(lParam);
		if (NULL != pInfo)
		{
			for (int index = 0; index < GetItemCount(); index++)
			{
				if (GetItemData(index) == pInfo->deviceKey)
				{
					if (true == pInfo->status)
					{
						SetItemText(index, m_COLUMN_INDEX_CURRENTSTATUS, pInfo->newValue.c_str());
					}
					else
					{
						SetItemText(index, m_COLUMN_INDEX_AUTOMANUAL, pInfo->newValue.c_str());
					}
					refreshItemGUI(index);
					break;
				}
			}
		delete pInfo;
		pInfo = NULL;
		}
		FUNCTION_EXIT;
		return 0;

	}
	void CDeviceList::UpdateDatapointValue(unsigned long deviceKey, const std::string& newValue, bool status)
	{
		FUNCTION_ENTRY("UpdateDatapointValue");
		//like++ GF-78. post message to update datapoint value, this will work as NON block, to
		//avoid ModeDeviceDlg hang when quit before finished initializing data.

		//for (int index = 0; index < GetItemCount(); index++)
		//{
		//	if (GetItemData(index) == deviceKey)
		//	{
		//		if (true == status)
		//		{
		//			SetItemText(index, m_COLUMN_INDEX_CURRENTSTATUS, newValue.c_str());
		//		}
		//		else
		//		{
		//			SetItemText(index, m_COLUMN_INDEX_AUTOMANUAL, newValue.c_str());
		//		}
		//		refreshItemGUI(index);
		//		break;
		//	}
		//}
		PDATAPOINTINFO pInfo = new DATAPOINTINFO;
		pInfo->deviceKey = deviceKey;
		pInfo->newValue = newValue;
		pInfo->status = status;
		PostMessage(Msg_UpdateDataPointValue, 0, reinterpret_cast<LPARAM>(pInfo));
		//++like.GF-78.
		FUNCTION_EXIT;
	}

	bool CDeviceList::isDeviceRunningFailed(int item)
	{
		FUNCTION_ENTRY("isDeviceRunningFailed");
		bool needRefresh = false;
		if (item <= GetItemCount() && IsWindow(this->m_hWnd))
		{
			CString strCurrentStatus = GetItemText(item, m_COLUMN_INDEX_CURRENTSTATUS);
			CString strExpectStatus = GetItemText(item, m_COLUMN_INDEX_EXPECTSTATUS);
	// 		bool OpenOrClose = (strExpectStatus == "开") ? true : false;
	// 		strExpectStatus = DeviceEquipmentManagerInstance::instance().getEquipmentRunningTrueFalseLable(m_locationKey, m_modeKey, GetItemData(item), OpenOrClose).c_str();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Device Status locationKey:%d modeKey:%d Current Value:%s Expect Value:%s", m_locationKey, m_modeKey, strCurrentStatus.GetBuffer(), strExpectStatus.GetBuffer());
			if (strCurrentStatus != strExpectStatus)
			{
				needRefresh = true;
			}
		}

		FUNCTION_EXIT;
		return needRefresh;
	}


	void CDeviceList::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
		*pResult = 0;

		if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
		}
		else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
		{
			int nItem = pLVCD->nmcd.dwItemSpec;
			int nSubItem = pLVCD->iSubItem;
			if (IsWindow(this->m_hWnd))
			{
				if (GetItemCount() > nItem)			
				{
					if (m_COLUMN_INDEX_CURRENTSTATUS == nSubItem)
					{
						if(isDeviceRunningFailed(nItem))
						{
							pLVCD->clrText = COLOR_RED;
						}
					}
					else if (m_COLUMN_INDEX_EXPECTSTATUS == nSubItem)
					{
						pLVCD->clrText = COLOR_BLACK;
					}
				}

			}
		}
	}

	void CDeviceList::refreshItemGUI(int item)
	{
		if (IsWindow(this->m_hWnd))
		{
			CRect itemRect;
			if (GetItemRect(item, &itemRect, LVIR_BOUNDS))
			{
				InvalidateRect(itemRect, TRUE);
			}
		}
	}

	int CDeviceList::getItemIndexByData(unsigned long data)
	{
		int index = 0;
		bool bFound = false;
		for (index = 0; index < GetItemCount(); index++)
		{
			unsigned long itemData = static_cast<unsigned long>(GetItemData(index));
			if (itemData == data)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			index = GetItemCount() + 1;
		}

		return index;
	}
}

