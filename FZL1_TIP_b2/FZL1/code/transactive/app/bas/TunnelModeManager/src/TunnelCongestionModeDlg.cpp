// src\TunnelCongestionModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManager.h"
#include "app/bas/TunnelModeManager/src/TunnelCongestionModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDataHelper.h"
#include "core/data_access_interface/src/DatabaseKey.h"

namespace TA_IRS_App
{
    const char* const DIALOG_TITLE = "隧道阻塞模式管理器";

	// CTunnelCongestionModeDlg dialog
	IMPLEMENT_DYNAMIC(CTunnelCongestionModeDlg, CTunnelModeBaseDialog)
	CTunnelCongestionModeDlg::CTunnelCongestionModeDlg(CWnd* pParent /*=NULL*/)
		: CTunnelModeBaseDialog(CTunnelCongestionModeDlg::IDD, pParent)
	{

	}

	CTunnelCongestionModeDlg::~CTunnelCongestionModeDlg()
	{
	}

	void CTunnelCongestionModeDlg::DoDataExchange(CDataExchange* pDX)
	{
		CTunnelModeBaseDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST_CONGESTIONMODE_LISTCTRL, m_modeListCtrl);
		DDX_Control(pDX, IDC_COMBO_SECTIONS, m_Sections);
		DDX_Control(pDX, IDC_COMBO_LINETYPE, m_LineType);
		DDX_Control(pDX, IDC_EDIT_TRACK, m_trackNumber);
	}


	BEGIN_MESSAGE_MAP(CTunnelCongestionModeDlg, CTunnelModeBaseDialog)
		ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnSearchButtonClick)
		ON_BN_CLICKED(IDC_BUTTON_EXECUTE, OnExecuteModeClick)
		ON_BN_CLICKED(IDC_BUTTON_QUIT, OnQuit)
		ON_EN_CHANGE(IDC_EDIT_TRACK, OnDlgTrackNumberChanged)
        ON_WM_SIZE()
	END_MESSAGE_MAP()


	// CTunnelCongestionModeDlg message handlers
	BOOL CTunnelCongestionModeDlg::OnInitDialog()
	{
        CTunnelModeBaseDialog::OnInitDialog();

		m_modeListCtrl.SetupListCtrl();

		m_lineTypeMap[TA_IRS_Core::enum_UpBound] = "上行";
		m_lineTypeMap[TA_IRS_Core::enum_DownBound] = "下行";

		m_initializeThread = new TunnelModeManagerActionThread(this);
		m_initializeThread->setAction(TunnelModeManagerAction_Initialize);
		m_initializeThread->start();

		return TRUE;
	}

	bool CTunnelCongestionModeDlg::canCloseWindow()
	{
		FUNCTION_ENTRY("canCloseWindow");
		bool bCanCloseWindow = (false == isThreadRunning(m_initializeThread)) && (false == getIsExecuteModeFlag());
		FUNCTION_EXIT;
		return bCanCloseWindow;
	}

	TA_Base_Bus::ResizingProperties CTunnelCongestionModeDlg::getResizingProperties()
	{
		FUNCTION_ENTRY("getResizingProperties");
		TA_Base_Bus::ResizingProperties resizeProperty;
		resizeProperty.canMaximise = true;
		resizeProperty.maxHeight = gMFT_Screen_Height;
        resizeProperty.maxWidth = gMFT_Screen_Width;
        resizeProperty.minHeight = gMin_Screen_Height;
        resizeProperty.minWidth = 1100;
		FUNCTION_EXIT;
		return resizeProperty;
	}

	DlgInitializeSize CTunnelCongestionModeDlg::getDlgInitSize()
	{
		FUNCTION_ENTRY("getDlgInitSize");
		DlgInitializeSize InitDlgSize(gMFT_Screen_Width,gMin_Screen_Height);
		FUNCTION_EXIT;
		return InitDlgSize;
	}
	
	void CTunnelCongestionModeDlg::OnSize(UINT nType, int cx, int cy)
	{
		CTunnelModeBaseDialog::OnSize(nType, cx, cy);
		if (IsWindow(this->GetSafeHwnd()) && BAS_IsWindow(IDC_LIST_MODE))
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			CRect buttonRect;
			GetDlgItem(IDC_BUTTON_EXECUTE)->GetWindowRect(&buttonRect);
			BAS_MoveFixedSizeCtrl(IDC_BUTTON_EXECUTE, clientRect.right-200, clientRect.bottom-Dlg_Bottom_Blank_Size-buttonRect.Height());
			BAS_MoveFixedSizeCtrl(IDC_BUTTON_QUIT, clientRect.right-100, clientRect.bottom-Dlg_Bottom_Blank_Size-buttonRect.Height());
			CRect listMode;
			GetDlgItem(IDC_LIST_MODE)->GetWindowRect(&listMode);
			ScreenToClient(&listMode);
			listMode.right = clientRect.right - Dlg_Right_Blank_Size;
			listMode.bottom = clientRect.bottom-Dlg_Bottom_Blank_Size*2-buttonRect.Height();
			GetDlgItem(IDC_LIST_MODE)->MoveWindow(&listMode);
		}
	}

    std::string CTunnelCongestionModeDlg::getApplicationCaption()
	{
		return DIALOG_TITLE;
	}

	void CTunnelCongestionModeDlg::ActionThreadFunction(TunnelModeManagerAction action)
	{
		FUNCTION_ENTRY("ActionThreadFunction");
		switch (action)
		{
		case TunnelModeManagerAction_Initialize:
			initializeDlgData();
			break;
		}


		FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::initializeDlgData()
	{
		FUNCTION_ENTRY("initializeDlgData");
		BAS_TRY
		{
			std::vector<TA_IRS_Core::StationSectionInfo> sectionInfo = TunnelModeManagerDataHelperInstance::instance()->getAllStationSections();
			std::vector<TA_IRS_Core::StationSectionInfo>::iterator it;
			unsigned long index = 0;
			for (it = sectionInfo.begin(); it != sectionInfo.end(); it++)
			{
				int item = m_Sections.InsertString(index++, (*it).strName.c_str());
				m_Sections.SetItemData(item, static_cast<DWORD>((*it).pkey));
			}
			if (m_Sections.GetCount() > 0)
			{
				m_Sections.SetCurSel(0);
			}

			for (std::map<unsigned long, std::string>::iterator it = m_lineTypeMap.begin(); it != m_lineTypeMap.end(); it++)
			{
				int index = m_LineType.InsertString(m_LineType.GetCount(), (*it).second.c_str());
				m_LineType.SetItemData(index, (*it).first);
			}
			m_LineType.SetCurSel(0);

			processTrackNumberFromHMI();
		}
		BAS_CATCH;

		FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::processTrackNumberFromHMI()
	{
		// fill the track number automatic, if configed
		std::string strTrackName = getTrackNumberFromSchematic();
		if (!strTrackName.empty())
		{
			m_trackNumber.SetWindowText(strTrackName.c_str());
            m_trackNumber.EnableWindow(FALSE);
		}
	}

	void CTunnelCongestionModeDlg::OnSearchButtonClick()
	{
		FUNCTION_ENTRY("OnSearchButtonClick");

		TA_IRS_Core::CongestionModeInfoVct modeVct;
		BAS_TRY
		{
			CString strTrackName;
			m_trackNumber.GetWindowText(strTrackName);
			//if( strTrackName.IsEmpty())
			//{
			//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Track name is null.");
			//	m_modeListCtrl.DeleteAllItems();
			//	return;
			//}

			unsigned long sectionKey = static_cast<DWORD>(m_Sections.GetItemData(m_Sections.GetCurSel()));
			TA_IRS_Core::LineType lineType = TA_IRS_Core::enum_Unknown;
			if (CB_ERR != m_LineType.GetCurSel())
			{
				if (TA_IRS_Core::enum_UpBound == m_LineType.GetItemData(m_LineType.GetCurSel()))
				{
					lineType = TA_IRS_Core::enum_UpBound;
				}
				else if (TA_IRS_Core::enum_DownBound == m_LineType.GetItemData(m_LineType.GetCurSel()))
				{
					lineType = TA_IRS_Core::enum_DownBound;
				}
			}

			modeVct = TunnelModeManagerDataHelperInstance::instance()->getCongestionModesByTrackNameAndSectionAndLineType( strTrackName.GetBuffer(), sectionKey, lineType);
		}
		BAS_CATCH;

		m_modeListCtrl.DeleteAllItems();
		removeDuplicateItems(modeVct);
		TA_IRS_Core::CongestionModeInfoVct::iterator it;
		for (it = modeVct.begin(); it != modeVct.end(); it++)
		{
			if((*it)->frontStation_modeName.empty() && (*it)->backStation_modeName.empty())
			{
                continue;
			}
			std::string strLeftModeType = ((*it)->frontStation_modeType.empty()) ? "上行：（无）" : "上行：" + (*it)->frontStation_modeType;
			std::string strRightModeType = ((*it)->backStation_modeType.empty()) ? " 下行：（无）" : " 下行：" + (*it)->backStation_modeType;
			std::string strLeftModeDesc = ((*it)->frontStation_modeDesc.empty()) ? "上行：（无）" : "上行：" + (*it)->frontStation_modeDesc;
			std::string strRightModeDesc = ((*it)->backStation_modeDesc.empty()) ? " 下行：（无）" : " 下行：" + (*it)->backStation_modeDesc;
			CString leftBount = (*it)->frontStation_modeName.c_str();
			CString rightBount = (*it)->backStation_modeName.c_str();
			unsigned long itemdata = (*it)->pkey;
			m_modeListCtrl.InsertItemTexts((strLeftModeType + strRightModeType).c_str(), (strLeftModeDesc + strRightModeDesc).c_str(), leftBount, rightBount, itemdata);
		}
		FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::OnExecuteModeClick()
    {
        FUNCTION_ENTRY("OnExecuteModeClick");

		using namespace TA_IRS_Core;
		POSITION pos = m_modeListCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			MessageBox("请先选择一个要执行的模式", DIALOG_TITLE, MB_OK | MB_ICONERROR);
			return;
		}
		int item = m_modeListCtrl.GetNextSelectedItem(pos);
        unsigned long pkey = static_cast<unsigned long>(m_modeListCtrl.GetItemData(item));

        setIsExecuteModeFlag(true);

		bool bFrontModeConfigured = false;
		bool bBackModeConfigured = false;
		bool bFrontModeSuccess = false;
		bool bBackModeSuccess = false;
        std::string strExecuteResultDes= "";
		TA_IRS_Core::Mode_SmartPtr frontModePtr;
		TA_IRS_Core::Mode_SmartPtr backModePtr;

		BAS_TRY
		{
            unsigned long frontStationKey = 0u;
            unsigned long backStationKey = 0u;
			TA_IRS_Core::BasTunnelCongestionModeAccessFactoryInstance::instance()->getFrontAndBackStationByKey(pkey, frontStationKey, backStationKey);
            TA_IRS_Core::CongestionModeInfo_SmartPtr modePtr = getSelectCongestionModeInfo(pkey);
			bool bFrontValidModeKey = TA_Base_Core::DatabaseKey::isValidKey( modePtr->frontStation_modeKey );
			bool bBackValidModeKey = TA_Base_Core::DatabaseKey::isValidKey( modePtr->backStation_modeKey );

			if (bFrontValidModeKey)
			{
				frontModePtr = TunnelModeManagerDataHelperInstance::instance()->getModeByKey(frontStationKey, modePtr->frontStation_modeKey);
			}
			if (bBackValidModeKey)
			{
				backModePtr = TunnelModeManagerDataHelperInstance::instance()->getModeByKey(backStationKey, modePtr->backStation_modeKey);
			}

			std::string strSession = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

            if (bFrontValidModeKey)
            {
                bFrontModeConfigured = true;

                std::string strFrontStationException = executeStationMode(frontModePtr, false, strSession);
                if (!strFrontStationException.empty())
                {
                    strExecuteResultDes = strExecuteResultDes + "\n上行车站模式 " + frontModePtr->getName().c_str() + " " + strFrontStationException.c_str();
                }
				else
				{
					bFrontModeSuccess = true;
                    strExecuteResultDes = strExecuteResultDes + "\n上行车站模式 " + frontModePtr->getName().c_str() + " " + "执行成功！";
				}
            }

            if (bBackValidModeKey)
            {
                bBackModeConfigured = true;

                std::string strBackStationException = executeStationMode(backModePtr, false, strSession);
                if (!strBackStationException.empty())
                {
                    strExecuteResultDes = strExecuteResultDes + "\n下行车站模式 " + backModePtr->getName().c_str() + " " + strBackStationException.c_str();
                }
				else
				{
					bBackModeSuccess = true;
					strExecuteResultDes = strExecuteResultDes + "\n下行车站模式 " + backModePtr->getName().c_str() + " " + "执行成功！";
				}
            }
		}
		BAS_CATCH_NOT_DEPLOY;

		setIsExecuteModeFlag(false);

		if (BAS_WhetherHaveError || (!bFrontModeConfigured && !bBackModeConfigured))
		{
			MessageBox("下发模式时发生错误，请检查数据库中的模式配置！", DIALOG_TITLE, MB_OK | MB_ICONERROR);
		}
		else if (!strExecuteResultDes.empty())
        {
			strExecuteResultDes = "执行模式结果：" + strExecuteResultDes;

			if ((bFrontModeSuccess && bBackModeSuccess) || (!bFrontModeConfigured && bBackModeSuccess) || (!bBackModeConfigured && bFrontModeSuccess))
			{
			    MessageBox(strExecuteResultDes.c_str(), DIALOG_TITLE, MB_OK | MB_ICONINFORMATION);	
			}
			else if(((!bBackModeSuccess && bFrontModeSuccess) || (!bFrontModeSuccess && bBackModeSuccess)) && (bFrontModeConfigured && bBackModeConfigured))
			{
				MessageBox(strExecuteResultDes.c_str(), DIALOG_TITLE, MB_OK | MB_ICONWARNING);
			}
			else
			{
				MessageBox(strExecuteResultDes.c_str(), DIALOG_TITLE, MB_OK | MB_ICONERROR);
			} 			
        }

        FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::OnQuit()
	{
		FUNCTION_ENTRY("OnQuit");	
		GetParent()->PostMessage(WM_CLOSE, 0, 0);
		FUNCTION_EXIT;
	}

	TA_IRS_Core::CongestionModeInfo_SmartPtr CTunnelCongestionModeDlg::getSelectCongestionModeInfo(unsigned long pkey)
	{
		using namespace TA_IRS_Core;
		TA_IRS_Core::CongestionModeInfoVct modeVct;
		CongestionModeInfo_SmartPtr modeInfo;
		CString strTrackName;
		m_trackNumber.GetWindowText(strTrackName);
		unsigned long sectionKey = static_cast<DWORD>(m_Sections.GetItemData(m_Sections.GetCurSel()));
		TA_IRS_Core::LineType lineType = TA_IRS_Core::enum_Unknown;
		if (CB_ERR != m_LineType.GetCurSel())
		{
			if (TA_IRS_Core::enum_UpBound == m_LineType.GetItemData(m_LineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_UpBound;
			}
			else if (TA_IRS_Core::enum_DownBound == m_LineType.GetItemData(m_LineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_DownBound;
			}
		}

		modeVct = TunnelModeManagerDataHelperInstance::instance()->getCongestionModesByTrackNameAndSectionAndLineType( strTrackName.GetBuffer(), sectionKey, lineType);
		TA_IRS_Core::CongestionModeInfoVct::iterator it;
		for (it = modeVct.begin(); it != modeVct.end(); it++)
		{
			if ((*it)->pkey == pkey)
			{
				modeInfo = (*it);
				break;
			}
		}
		if (NULL == modeInfo.get())
		{
			TA_THROW(TA_Base_Core::TransactiveException("There no find the select congestion mode info"));
		}
		return modeInfo;
	}

	void CTunnelCongestionModeDlg::OnDlgTrackNumberChanged()
	{
		FUNCTION_ENTRY("OnDlgTrackNumberChanged");

		fillwithSectionAndLineTypeByTrackName();
		
		FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::fillwithSectionAndLineTypeByTrackName()
	{
		FUNCTION_ENTRY("fillwithSectionAndLineTypeByTrackName");
		CString strTrackName; 
		m_trackNumber.GetWindowText(strTrackName);
		//if( strTrackName.IsEmpty())
		//{
		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Track name is null.");
		//	return;
		//}

		TA_IRS_Core::TrackInfoVct trackInfoVct = TunnelModeManagerDataHelperInstance::instance()->getTrackInfosByTrackName(strTrackName.GetBuffer());

		if (trackInfoVct.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Can't found the track name of including string:%s, please check the data.", strTrackName.GetBuffer());

			//Disable filter Controls
			m_Sections.EnableWindow(FALSE);
			m_LineType.EnableWindow(FALSE);
		}
		else
		{
			//Enable filter Controls
			m_Sections.EnableWindow(TRUE);
			m_LineType.EnableWindow(TRUE);

			int indexLineType = 0;
			int indexSection = 0;
			bool isRepeatForLineType = false;
			bool isRepeatForSection = false;

			TA_IRS_Core::TrackInfoVctIter itTrackInfoVct = trackInfoVct.begin();
			m_Sections.ResetContent();
			m_LineType.ResetContent();

			for(; itTrackInfoVct != trackInfoVct.end(); ++itTrackInfoVct)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found track info by track name of including string %s sectionKey=%d LineType=%s",
					strTrackName.GetBuffer(), (*itTrackInfoVct)->sectionKey, lineTypeToString((*itTrackInfoVct)->lineType).c_str());
                
				isRepeatForLineType = false;
				isRepeatForSection = false;

				//Set line type
				TA_IRS_Core::LineType lineType = TA_IRS_Core::LineType::enum_Unknown;
				for(int nCount=0; nCount < m_LineType.GetCount();nCount++)
				{					
					lineType = static_cast<TA_IRS_Core::LineType>(m_LineType.GetItemData(nCount));
					if((*itTrackInfoVct)->lineType == lineType)
					{
						isRepeatForLineType = true;
						break;
					}
				}

				if((!isRepeatForLineType) && ((*itTrackInfoVct)->lineType != TA_IRS_Core::LineType::enum_Unknown))
				{
					int item = m_LineType.InsertString(indexLineType++, m_lineTypeMap[(*itTrackInfoVct)->lineType].c_str());
					m_LineType.SetItemData(item, (*itTrackInfoVct)->lineType);
				}

				//Set sections
				unsigned long sectionKey = TA_Base_Core::DatabaseKey::getInvalidKey();
				for(int nCount=0; nCount < m_Sections.GetCount();nCount++)
				{
					sectionKey = static_cast<DWORD>(m_Sections.GetItemData(nCount));
					if((*itTrackInfoVct)->sectionKey == sectionKey)
					{
                        isRepeatForSection = true;
						break;
					}
				}

				BAS_TRY
				{
					if(!isRepeatForSection)
					{
						TA_IRS_Core::StationSectionInfo sectionInfo =  TunnelModeManagerDataHelperInstance::instance()->getStationSectionByKey((*itTrackInfoVct)->sectionKey);

						int item = m_Sections.InsertString(indexSection++, sectionInfo.strName.c_str());
						m_Sections.SetItemData(item, sectionInfo.pkey);
					}
				}
				BAS_CATCH;
				if (BAS_WhetherHaveError)
				{
					CString strErrorMsg;
					strErrorMsg.Format("根据轨道号[%s]找不到相匹配的区间信息", strTrackName.GetBuffer());
					MessageBox(strErrorMsg, DIALOG_TITLE, MB_OK | MB_ICONERROR);
					break;
				}
			}

			if(m_Sections.GetCount() > 0)
			{
				m_Sections.SetCurSel(0);
			}

			if(m_LineType.GetCount() > 0)
			{
				m_LineType.SetCurSel(0);
			}
		}

		FUNCTION_EXIT;
	}

	void CTunnelCongestionModeDlg::setSelItemByData(CComboBox* comboBox, unsigned long data)
	{
		if (IsWindow(comboBox->m_hWnd))
		{
			for (int index = 0; index < comboBox->GetCount(); index++)
			{
				if (comboBox->GetItemData(index) == data)
				{
					comboBox->SetCurSel(index);
					break;
				}
			}
		}		
	}

	void CTunnelCongestionModeDlg::removeDuplicateItems(TA_IRS_Core::CongestionModeInfoVct& congestionModes)
	{
		TA_IRS_Core::CongestionModeInfoVct::iterator it;
		TA_IRS_Core::CongestionModeInfoVct::iterator next;
		for (it = congestionModes.begin(); it != congestionModes.end(); it++)
		{
			for (next = it + 1; next != congestionModes.end(); )
			{
				if ((*it)->frontStation_modeKey == (*next)->frontStation_modeKey &&
					(*it)->backStation_modeKey == (*next)->backStation_modeKey)
				{
					next = congestionModes.erase(next);
				}
				else
				{
					next++;
				}
			}
		}
	}
}
