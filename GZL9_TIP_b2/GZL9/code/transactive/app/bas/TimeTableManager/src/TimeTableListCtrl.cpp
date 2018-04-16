// src\TimeTableListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <iomanip>
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/TimeTableListCtrl.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerDataHelper.h"
#include "core/data_access_interface/bas/src/ModeTypeAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"


namespace TA_IRS_App
{
    const CHAR* Occ_TimeTable_Column_Text_mode = _T("模式描述+编号");
    const CHAR* Occ_TimeTable_Column_Text_BeginTime = _T("开始时间");
    const CHAR* Station_TimeTable_Column_Text_mode = _T("模式描述+编号");
    const CHAR* Station_TimeTable_Column_Text_BeginTime = _T("开始时间");
    const CHAR* ModeCodeAndDescDelimier = _T(" - ");
    const CHAR* ModifyItemToken = _T("* ");
    const CHAR* NewItemToken = _T("# ");
    const unsigned int WM_USER_REFRESH = WM_USER + 0x1001u;
    const unsigned int WM_USER_UPDATE_ITEM = WM_USER + 0x1002u;
    const unsigned int Occ_Station_TimeTable_Column_BeginTime_Width = 200u;

	// TimeTableListCtrl
	IMPLEMENT_DYNAMIC(TimeTableListCtrl, CEditableListCtrl)
	TimeTableListCtrl::TimeTableListCtrl() : CEditableListCtrl(true),
    m_isTodayTimetable(false), 
    m_canEditable(false),
    m_locationFilter(NULL)
	{
		SetManager(this);
		SetComboxCellDataSortable(FALSE);
	}

	TimeTableListCtrl::~TimeTableListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(TimeTableListCtrl, CEditableListCtrl)
		ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
        ON_MESSAGE(WM_SETFONT, OnSetFont)
        ON_MESSAGE(WM_USER_REFRESH, OnRefresh)
        ON_MESSAGE(WM_USER_UPDATE_ITEM, OnUpdateItem)
		ON_WM_MEASUREITEM_REFLECT()
	END_MESSAGE_MAP()

	void TimeTableListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
		
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
			/*int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
			if (IsWindow(this->m_hWnd))
			{
				if (GetItemCount() > nItem)			
				{
					if (GetItemData(nItem) == ERROR_ITEM_DATA_VALUE)
					{
						pLVCD->clrText = COLOR_RED;
						*pResult = CDRF_NOTIFYSUBITEMDRAW;
					}
					else if (isItemChanged(nItem, false))
					{
						pLVCD->clrTextBk = COLOR_YELLOW;
						*pResult = CDRF_NOTIFYSUBITEMDRAW;
					}
					else if (isItemChanged(nItem, true))
					{
						pLVCD->clrTextBk = COLOR_GREEN;
						*pResult = CDRF_NOTIFYSUBITEMDRAW;
					}
				}
			}*/
		}
	}

	void TimeTableListCtrl::MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
	{
		LOGFONT lf;
		GetFont()->GetLogFont( &lf );

		lpMeasureItemStruct->itemHeight = (lf.lfHeight < 0) ? -lf.lfHeight : lf.lfHeight;		
	}

	LRESULT TimeTableListCtrl::OnSetFont(WPARAM wParam, LPARAM)
	{
		LRESULT res =  Default();

		CRect rc;
		GetWindowRect( &rc );

		WINDOWPOS wp;
		wp.hwnd = m_hWnd;
		wp.cx = rc.Width();
		wp.cy = rc.Height();
		wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
		SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

		return res;
	}

	// TimeTableListCtrl message handlers
	void TimeTableListCtrl::setupList()
	{
		if (FALSE != IsWindow(this->GetSafeHwnd()))
		{
            setRowFullSelect();
            InsertColumn(TimeTable_Column_Index_mode, Occ_TimeTable_Column_Text_mode);
            InsertColumn(TimeTable_Column_Index_BeginTime, Occ_TimeTable_Column_Text_BeginTime);
		}
	}

	void TimeTableListCtrl::resizeHeaderCtrl()
	{
		CHeaderCtrl* headerHandle = GetHeaderCtrl();
		if (NULL != headerHandle)
		{
			const int fitableWidth = 5;
			CRect headerRect;
            headerHandle->GetWindowRect(&headerRect);
            SetColumnWidth(TimeTable_Column_Index_mode, headerRect.Width() - fitableWidth -Occ_Station_TimeTable_Column_BeginTime_Width);
            SetColumnWidth(TimeTable_Column_Index_BeginTime,Occ_Station_TimeTable_Column_BeginTime_Width);
		}
	}

	DWORD TimeTableListCtrl::setRowFullSelect()
	{
		LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
		style = SetWindowLong(this->m_hWnd, GWL_STYLE, style | LVS_SINGLESEL);
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES );
		return SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_OWNERDRAWFIXED); 
	}

	//[[ ------------------------ The implement of ICellManager ------------------------------
	ICellManager::CellType TimeTableListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		// If the list ctrl type is not Occ or the item subItem invalid
		// we can not edit the value , in this case.
		if (false == checkRowColumn(item, subItem) || false == m_canEditable)
		{
			return ICellManager::CT_NOTEDITABLE;
		}

		// the Occ list ctrl
		ICellManager::CellType cellType = ICellManager::CT_NOTEDITABLE;
		if(TimeTable_Column_Index_mode == subItem)
		{
			cellType = ICellManager::CT_SELECTION;
		}
		else if(TimeTable_Column_Index_BeginTime == subItem && !GetItemText(item, TimeTable_Column_Index_mode).IsEmpty())
		{
			cellType = ICellManager::CT_TIME;
		}
		return cellType;
	}

	unsigned long TimeTableListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		if (false == checkRowColumn(item, subItem))
		{
			return 200;
		}
		return 200;
	}

	ICellManager::CellData TimeTableListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		ICellManager::CellData cellData;
		if (false == checkRowColumn(item, subItem))
		{
			return cellData;
		}
		ICellManager::CellData::iterator it;
		locationModes modes;
		TimeTableManagerDataHelperInstance::instance()->getModesByLocation(m_locationFilter->getLocationKey(), modes);

		// pushback the blank item for delete
		std::string currentItemText = GetItemText(item,subItem).GetBuffer();
		cellData.push_back("");
		if (!currentItemText.empty())
		{
			cellData.push_back(currentItemText);
		}
		
		if (isItemChanged(item, false))
		{
			currentItemText = getOriginalModeNameString(currentItemText, false);
		}
		else if(isItemChanged(item, true))
		{
			currentItemText = getOriginalModeNameString(currentItemText, true);
		}
		for (locationModesIter it = modes.begin(); it != modes.end(); it++ )
		{
			std::string strFormat = formatNameString((*it)->getName(), (*it)->getDescription());
			unsigned long typeKey = (*it)->getTypeKey();
			if (currentItemText != strFormat &&  
				typeKey != TA_IRS_Core::ModeTypeAccessFctory::modeTypeKey_Fire &&
				typeKey != TA_IRS_Core::ModeTypeAccessFctory::modeTypeKey_Congestion)
			{
				cellData.push_back(strFormat);
			}	
		}
		
		return cellData;
	}

	CString TimeTableListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		return "***.***";
	}

	char TimeTableListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		return '*';
	}

	bool TimeTableListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		if(false == checkRowColumn(item, subItem))
		{
			return false;
		}

		bool bChanged = false;
		if (m_canEditable && subItem == TimeTable_Column_Index_mode)
		{
			bChanged = false; // we will update the text manually
			CString strOldMode = GetItemText(item, subItem);
			if (strOldMode != value)
			{
				if (!strOldMode.IsEmpty() && value.IsEmpty()) //delete the old value
                {
                    DeleteItem(item);

                    checkAndAppendInsertionItem();
				}
				else if (strOldMode.IsEmpty() && !value.IsEmpty())
				{
                    //add a blank item for add the new mode
					strOldMode = updateItemChange(value, strOldMode.IsEmpty() ? true : false);
					std::string strOrgModeName = getOriginalModeShortNameString(value);
					unsigned long ulModeKey = TimeTableManagerDataHelperInstance::instance()->getModeKeyByName(m_locationFilter->getLocationKey(), strOrgModeName.c_str());

					if( TA_Base_Core::DatabaseKey::isInvalidKey(ulModeKey) )
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't find corresponding mode in location %d,invalid mode name:%s", m_locationFilter->getLocationKey(), strOrgModeName.c_str());
						return bChanged;
					}

					SetItemText(item, subItem, strOldMode);
                    SetItemData(item, ulModeKey);

                    // Set time for new item
                    CTime ctmTime( 2000, 1, 1, 0, 0, 0 );
                    std::string strTime = formatTimeString(ctmTime.GetTime());
                    SetItemText(item, TimeTable_Column_Index_BeginTime, strTime.c_str());

                    checkAndAppendInsertionItem();
				}
				else if (!strOldMode.IsEmpty() && !value.IsEmpty())
				{
					strOldMode = updateItemChange(value, strOldMode.IsEmpty() ? true : false);
                    std::string strOrgModeName = getOriginalModeShortNameString(value);
					unsigned long ulModeKey = TimeTableManagerDataHelperInstance::instance()->getModeKeyByName(m_locationFilter->getLocationKey(), strOrgModeName.c_str());

					if( TA_Base_Core::DatabaseKey::isInvalidKey(ulModeKey) )
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't find corresponding mode in location %d,invalid mode name:%s", m_locationFilter->getLocationKey(), strOrgModeName.c_str());
						return bChanged;
					}

					SetItemText(item, subItem, strOldMode);
					SetItemData(item, ulModeKey);
				}
			}
		}
		else if (m_canEditable && subItem == TimeTable_Column_Index_BeginTime)
		{
			bChanged = false;// we will update the text manually
            COleDateTime timeDest;
            timeDest.ParseDateTime(value.GetBuffer());
            SYSTEMTIME stTime;
            timeDest.GetAsSystemTime(stTime);
            stTime.wYear = ( stTime.wYear < 1970 ) ? 2000 : stTime.wYear;
            CTime ctmTime( stTime );
            std::string strTime = formatTimeString(ctmTime.GetTime());
			SetItemText(item, subItem, strTime.c_str());
		}

		return bChanged;
	}
	//]] ------------------------ The implement of ICellManager ------------------------------


	bool TimeTableListCtrl::checkRowColumn(int item, int subItem)
	{
		bool bSuccess = ( item < GetItemCount() ) && ( subItem <= TimeTable_Column_Index_BeginTime );

		if (!bSuccess)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "CEditableListCtrl Invalid call data, Item:%d  subItem:%d",item, subItem);	
		}

		return bSuccess;
	}

	ILocationFilter* TimeTableListCtrl::setLocationFilter(ILocationFilter* locationFilter)
	{
		ILocationFilter* old = m_locationFilter;
		m_locationFilter = locationFilter;
		return old;
	}

	std::string TimeTableListCtrl::formatNameString(const std::string& strModeName, const std::string& strModeDesc)
	{
		FUNCTION_ENTRY("formatNameString");
		std::string strName;
		strName = strModeName + ModeCodeAndDescDelimier + strModeDesc;
		FUNCTION_EXIT;
		return strName;
	}

    std::string TimeTableListCtrl::formatTimeString( const time_t tmTime )
    {
        tm tmConvert;
        localtime_s( &tmConvert, &tmTime );

        std::stringstream stmConvertor;
        stmConvertor << std::setw(2) << std::setfill( '0' )<< tmConvert.tm_hour <<  " 时 " << std::setw(2) << std::setfill( '0' ) << tmConvert.tm_min << " 分";

        return stmConvertor.str().c_str();
    }

	CString TimeTableListCtrl::updateItemChange(CString strNewValue, bool isNew)
	{
		CString strText;
		if (false == isNew)
		{
			strText = ModifyItemToken + strNewValue;
		} 
		else
		{
			strText = NewItemToken + strNewValue;
		}
		return strText;
	}

	bool TimeTableListCtrl::isItemChanged(int item, bool isNew)
	{
		bool bChanged;
		if (false ==  isNew)
		{
			bChanged = GetItemText(item, 0).Mid(0, 2).Compare(ModifyItemToken) == 0 ? true : false;
		} 
		else
		{
			bChanged = GetItemText(item, 0).Mid(0, 2).Compare(NewItemToken) == 0 ? true : false;
		}	
		return bChanged;
	}

	bool TimeTableListCtrl::isItemChanged(const std::string& strNewValue)
	{
		bool bChanged = true;
		if (strNewValue.size() > 2)
		{
			bChanged = (strNewValue.substr(0, 2).compare(ModifyItemToken) == 0) || (strNewValue.substr(0, 2).compare(NewItemToken) == 0) ? true : false;
		}
		return bChanged;
	}

	std::string TimeTableListCtrl::getOriginalModeNameString(const std::string& strNewValue, bool isNew)
	{
		std::string strOriginalString;
		if (strNewValue.length() > 2)
		{
			if (false == isNew)
			{
				strOriginalString = strNewValue.substr(2);
			} 
			else
			{
				strOriginalString = strNewValue.substr(2);
			}
		}
		return strOriginalString;
	}

	CString TimeTableListCtrl::getOriginalModeShortNameString(const CString& strNewValue)
	{
		CString strReturn = strNewValue;
		std::string strTemp = strReturn.GetBuffer();
		if (isItemChanged(strTemp))
		{
			strReturn = strNewValue.Mid(2);
		}
		strReturn = strReturn.Mid(0,strReturn.Find(ModeCodeAndDescDelimier));
		return strReturn;
	}

	std::string TimeTableListCtrl::getOriginalModeBeginTime(const std::string& strNewTime)
	{
		if (strNewTime.size() > 2)
		{
			std::string strNewTimeString = strNewTime;
			std::string strHour = strNewTimeString.substr(0,strNewTimeString.find_first_of(' '));
			if (strHour.size() == 1)
			{
				strHour = "0" + strHour;
			}
			strNewTimeString = strNewTimeString.substr(strNewTimeString.find_first_of(' ') + 4); //strNewTimeString is "mm 分"
			std::string strMin = strNewTimeString.substr(0, strNewTimeString.find_first_of(' '));
			if (strMin.size() == 1)
			{
				strMin = "0" + strMin;
			}
			return strHour + strMin;
		}
		return "";
	}

	void TimeTableListCtrl::updateModeNameToNormal()
	{

		for (int i = 0; i < GetItemCount(); i++ )
		{
			CString strOriginalText = GetItemText(i, TimeTable_Column_Index_mode);
			if (isItemChanged(i, true))
			{
				strOriginalText = getOriginalModeNameString(strOriginalText.GetBuffer(), true).c_str();
			}
			else if (isItemChanged(i, false))
			{
				strOriginalText = getOriginalModeNameString(strOriginalText.GetBuffer(), false).c_str();
			}
			SetItemText(i, TimeTable_Column_Index_mode, strOriginalText);
		}

	}

	void TimeTableListCtrl::canEditModeList(bool bEditable)
	{
		m_canEditable = bEditable;
	}

	void TimeTableListCtrl::loadTimeTableFromAgent(unsigned long locationKey, bool todayTimeTable)
	{
        FUNCTION_ENTRY("loadTimeTableFromAgent");

        m_isTodayTimetable = todayTimeTable;

        TimeTableDatapointPairManagerInstance::instance()->registerModeBeginTimeUpdateListener(this);

        TimeTableManagerDataHelperInstance::instance()->loadModesByLocation( locationKey );

        TimeTableDatapointPairManagerInstance::instance()->initializeTimeTablePairsByLocation( locationKey );

        PostMessage( WM_USER_REFRESH, locationKey, 0u );

		FUNCTION_EXIT;
	}

	void TimeTableListCtrl::updateModeBeginTime(const unsigned long locationKey, const unsigned long entityKey)
	{
        FUNCTION_ENTRY("updateModeBeginTime");

        if ( m_canEditable )
        {
            return;
        }

        PostMessage( WM_USER_UPDATE_ITEM, locationKey, entityKey );

		FUNCTION_EXIT;
	}

    LRESULT TimeTableListCtrl::OnRefresh( WPARAM wParam, LPARAM lParam )
    {
        const unsigned long locationKey = static_cast<unsigned long>(wParam);

        DeleteAllItems();

        ModeAndTimeDatapointPairVct datapoints;
        TimeTableDatapointPairManagerInstance::instance()->getTimeTablePairsByLocation(locationKey, m_isTodayTimetable, datapoints);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadTimeTableFromAgent, location=%d TimeTable=%s datapointsPairCound=%d",
            locationKey, m_isTodayTimetable ? "TodayTimeTable" : "NextDayTimeTable", datapoints.size());

        for (ModeAndTimeDatapointPairVct::iterator it = datapoints.begin(); it != datapoints.end(); it++)
        {
            unsigned long modeValue = 0u;
            time_t tmBeginTime = 0u;
            if ((*it)->getModeCodeAndBeginTime(modeValue, tmBeginTime))
            {
                insertModeTime((*it)->getDpKey(), locationKey, modeValue, tmBeginTime);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get the mode=%s and begintime=%s", (*it)->getModeDataPointName().c_str(), (*it)->getBeginTimeDataPointName().c_str());
            }
        }

        return 0u;
    }

    LRESULT TimeTableListCtrl::OnUpdateItem( WPARAM wParam, LPARAM lParam )
    {
        const unsigned long ulLocationKey = static_cast<unsigned long>(wParam);
        const unsigned long ulEntityKey = static_cast<unsigned long>(lParam);

        bool bIsToday = false;
        bool bIsTomorrow = false;
        ModeAndTimeDatapointPair_SmartPtr ptrModeTime = TimeTableDatapointPairManagerInstance::instance()->getTimeTablePairsByEntityKey(ulLocationKey, ulEntityKey, bIsToday, bIsTomorrow);

        if ( ptrModeTime.get() == NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Cannot find mode and time pair.locationKey=%d, entityKey=%d",ulLocationKey,ulEntityKey);
            return 0u;
        }

        if (( m_isTodayTimetable && !bIsToday ) || ( !m_isTodayTimetable && !bIsTomorrow ))
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Today timetable or tomorrow timetable found is inconsistent.locationKey=%d, entityKey=%d",ulLocationKey,ulEntityKey);
            return 0u;
        }

        unsigned long modeValue = 0u;
        time_t tmBeginTime = 0u;
        if (ptrModeTime->getModeCodeAndBeginTime(modeValue, tmBeginTime))
        {
            int item = -1;
            for (int index = 0; index < GetItemCount(); index++)
            {
                if (GetItemData(index) == ulEntityKey)
                {
                    item = index;
                    break;
                }
            }

            if ( 0 <= item )
            {
                Mode_SmartPtr modePtr = TimeTableManagerDataHelperInstance::instance()->getModeByModeValue(ulLocationKey, modeValue);
                if (NULL != modePtr.get())
                {
                    std::string strDisplayName= formatNameString(modePtr->getName(), modePtr->getDescription());
                    std::string strBeginTime = formatTimeString( tmBeginTime );
                    SetItemText(item, TimeTable_Column_Index_mode, strDisplayName.c_str());
                    SetItemText(item, TimeTable_Column_Index_BeginTime, strBeginTime.c_str());
                }
                else
                {
                    DeleteItem( item );
                }
            }
            else
            {
                insertModeTime(ulEntityKey, ulLocationKey, modeValue, tmBeginTime);
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get the mode=%s and begintime=%s", ptrModeTime->getModeDataPointName().c_str(), ptrModeTime->getBeginTimeDataPointName().c_str());
        }

        return 0u;
    }

    void TimeTableListCtrl::insertModeTime( unsigned long entityKey, unsigned long locationKey, unsigned long modeValue, time_t tmBeginTime )
    {
        Mode_SmartPtr modePtr = TimeTableManagerDataHelperInstance::instance()->getModeByModeValue(locationKey, modeValue);
        if (NULL != modePtr.get())
        {
            std::string strDisplayName= formatNameString(modePtr->getName(), modePtr->getDescription());
            std::string strBeginTime = formatTimeString(tmBeginTime);

            int item = InsertItem(GetItemCount(), strDisplayName.c_str());
            SetItemData(item, entityKey);

            SetItemText(item, TimeTable_Column_Index_BeginTime, strBeginTime.c_str());
        } 
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get mode info by mode value , ModeValue:%d", modeValue);
        }
    }

    void TimeTableListCtrl::checkAndAppendInsertionItem()
    {
        // The last item should be unique insertion item only if item count less than MaxModesSendPerLocation
        if (( GetItemCount() < MaxModesSendPerLocation ) && !isInsertionItem( GetItemCount() - 1 ))
        {
            int item = InsertItem(GetItemCount(), "");
            SetItemData(item, TA_Base_Core::DatabaseKey::getInvalidKey());
        }
    }

    bool TimeTableListCtrl::isInsertionItem( const int nItem )
    {
        if (( nItem < 0 ) || ( nItem >= GetItemCount() ))
        {
            return false;
        }

        unsigned long ulModeKey = static_cast<unsigned long>(GetItemData(nItem));

        return TA_Base_Core::DatabaseKey::isInvalidKey( ulModeKey );
    }
}
