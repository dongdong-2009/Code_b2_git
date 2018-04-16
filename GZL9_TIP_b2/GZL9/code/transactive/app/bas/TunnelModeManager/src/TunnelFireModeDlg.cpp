// src\TunnelFireModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManager.h"
#include "app/bas/TunnelModeManager/src/TunnelFireModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDataHelper.h"
#include "core/data_access_interface/src/DatabaseKey.h"

namespace TA_IRS_App
{
    const char* const DIALOG_TITLE = "隧道火灾模式管理器";

	// CTunnelFireModeDlg dialog
	IMPLEMENT_DYNAMIC(CTunnelFireModeDlg, CTunnelModeBaseDialog)
	CTunnelFireModeDlg::CTunnelFireModeDlg(CWnd* pParent /*=NULL*/)
		: CTunnelModeBaseDialog(CTunnelFireModeDlg::IDD, pParent)
	{

	}

	CTunnelFireModeDlg::~CTunnelFireModeDlg()
	{
	}

	void CTunnelFireModeDlg::DoDataExchange(CDataExchange* pDX)
	{
		CTunnelModeBaseDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST_FIREMODE_LISTCTRL, m_modeListCtrl);
		DDX_Control(pDX, IDC_COMBO_SECTIONS_FIRE, m_sections);
		DDX_Control(pDX, IDC_COMBO_LINETYPE_FIRE, m_lineType);
		DDX_Control(pDX, IDC_COMBO_SITUATION_FIRE, m_fireCase);
		DDX_Control(pDX, IDC_EDIT_TRACK_FIRE, m_trackNumber);
		DDX_Control(pDX, IDC_RADIO_TRAIN_POSTION_LEFT, m_firePositionLeft);
		DDX_Control(pDX, IDC_RADIO_TRAIN_POSTION_RIGHT, m_firePositionRight);
	}


	BEGIN_MESSAGE_MAP(CTunnelFireModeDlg, CTunnelModeBaseDialog)
		ON_BN_CLICKED(IDC_BUTTON_SEARCH_FIRE, OnSearchButtonClick)
		ON_BN_CLICKED(IDOK, CTunnelFireModeDlg::OnButtonExecuteClick)
		ON_EN_CHANGE(IDC_EDIT_TRACK_FIRE, OnDlgTrackNumberChanged)
		ON_BN_CLICKED(IDC_BUTTON_QUIT_FIRE, OnQuit)
		ON_WM_SIZE()
	END_MESSAGE_MAP()


	// CTunnelFireModeDlg message handlers
	BOOL CTunnelFireModeDlg::OnInitDialog()
	{
		CTunnelModeBaseDialog::OnInitDialog();

		m_modeListCtrl.SetupListCtrl();

		m_lineTypeMap[TA_IRS_Core::enum_UpBound] = "上行";
		m_lineTypeMap[TA_IRS_Core::enum_DownBound] = "下行";

		//init shaft button
		m_firePositionLeft.SetCheck(BST_UNCHECKED);
		m_firePositionRight.SetCheck(BST_UNCHECKED);
		m_firePositionLeft.EnableWindow(FALSE);
		m_firePositionRight.EnableWindow(FALSE);

		m_InitializeThread = new TunnelModeManagerActionThread(this);
		m_InitializeThread->setAction(TunnelModeManagerAction_Initialize);
		m_InitializeThread->start();
		return TRUE;
	}

	void CTunnelFireModeDlg::OnQuit()
	{
		FUNCTION_ENTRY("OnQuit");	
		GetParent()->PostMessage(WM_CLOSE, 0, 0);
		FUNCTION_EXIT;
	}

	bool CTunnelFireModeDlg::canCloseWindow()
	{
		FUNCTION_ENTRY("canCloseWindow");
		bool bCanCloseWindow = (false == isThreadRunning(m_InitializeThread)) && (false == getIsExecuteModeFlag());
		FUNCTION_EXIT;
		return bCanCloseWindow;	
	}

	TA_Base_Bus::ResizingProperties CTunnelFireModeDlg::getResizingProperties()
	{
        FUNCTION_ENTRY("getResizingProperties");
        TA_Base_Bus::ResizingProperties resizeProperty;
		resizeProperty.canMaximise = true;
		resizeProperty.maxHeight = gMFT_Screen_Height;
		resizeProperty.maxWidth = gMFT_Screen_Width;
		resizeProperty.minHeight = gMin_Screen_Height;
		resizeProperty.minWidth = 1220;
		FUNCTION_EXIT;
		return resizeProperty;
	}

	DlgInitializeSize CTunnelFireModeDlg::getDlgInitSize()
	{
		FUNCTION_ENTRY("getDlgInitSize");
		DlgInitializeSize retValue(gMFT_Screen_Width, gMin_Screen_Height);
		FUNCTION_EXIT;
		return retValue;
	}

	void CTunnelFireModeDlg::OnSize(UINT nType, int cx, int cy)
	{
		CTunnelModeBaseDialog::OnSize(nType, cx, cy);
		if (IsWindow(this->GetSafeHwnd()) && BAS_IsWindow(IDC_LIST_MODE_LIST2))
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			CRect executeButton;
			GetDlgItem(IDOK)->GetWindowRect(&executeButton);
			int button_cy = clientRect.bottom - Dlg_Bottom_Blank_Size - executeButton.Height();
			BAS_MoveFixedSizeCtrl(IDOK, clientRect.right - executeButton.Width() *4 , button_cy);
			BAS_MoveFixedSizeCtrl(IDC_BUTTON_QUIT_FIRE, clientRect.right - executeButton.Width() *2, button_cy);

			CRect modeList;		
			GetDlgItem(IDC_LIST_MODE_LIST2)->GetWindowRect(&modeList);
			ScreenToClient(&modeList);
			modeList.right = clientRect.right - Dlg_Right_Blank_Size * 2;
			modeList.bottom = button_cy - Dlg_Bottom_Blank_Size;
			GetDlgItem(IDC_LIST_MODE_LIST2)->MoveWindow(&modeList, TRUE);
		}
	}

    std::string CTunnelFireModeDlg::getApplicationCaption()
	{
		return DIALOG_TITLE;
	}

	void CTunnelFireModeDlg::ActionThreadFunction(TunnelModeManagerAction action)
	{
		FUNCTION_ENTRY("ActionThreadFunction");
		switch (action)
		{
		case TunnelModeManagerAction_Initialize:
			InitializeDlgData();
			break;
		}
		FUNCTION_EXIT;
	}

	void CTunnelFireModeDlg::InitializeDlgData()
	{
		FUNCTION_ENTRY("InitializeDlgData");

		// load the station sections info from DB
		{
			std::vector<TA_IRS_Core::StationSectionInfo> StationSectionInfos;
			BAS_TRY
			{
				StationSectionInfos = TunnelModeManagerDataHelperInstance::instance()->getAllStationSections();
			}
			BAS_CATCH;
			std::vector<TA_IRS_Core::StationSectionInfo>::iterator it;
			int index = 0;
			for (it = StationSectionInfos.begin(); it != StationSectionInfos.end(); it++)
			{
				int item = m_sections.InsertString(index++, (*it).strName.c_str());
				m_sections.SetItemData(item, (*it).pkey);
			}
			if(m_sections.GetCount() > 0)
			{
				m_sections.SetCurSel(0);
			}
		}

		// load the fire cases info from DB
		{
			//Set first item
			std::string firstFireCaseName = "任意";
			int firstItem = m_fireCase.InsertString( 0, firstFireCaseName.c_str());
			m_fireCase.SetItemData(firstItem, 0);

			TA_IRS_Core::TunnelFireCaseInfoVct FireCaseInfos;
			BAS_TRY
			{
				FireCaseInfos = TunnelModeManagerDataHelperInstance::instance()->getAllFireCases();
			}
			BAS_CATCH;
			std::vector<TA_IRS_Core::TunnelFireCaseInfo>::iterator it;
			int index = 1;
			for (it = FireCaseInfos.begin(); it != FireCaseInfos.end(); it++)
			{
				int item = m_fireCase.InsertString(index++, (*it).name.c_str());
				m_fireCase.SetItemData(item, (*it).pkey);
			}
			if(m_fireCase.GetCount() > 0)
			{
				m_fireCase.SetCurSel(0);
			}
		}

		
		// FILL THE LINE INFO
		for (std::map<unsigned long, std::string>::iterator it = m_lineTypeMap.begin(); it != m_lineTypeMap.end(); it++)
		{
			int index = m_lineType.InsertString(m_lineType.GetCount(), (*it).second.c_str());
			m_lineType.SetItemData(index, (*it).first);
		}
		m_lineType.SetCurSel(0);

		// process the track number if configed
		processTrackNumberFromHMI();

		FUNCTION_EXIT;
	}

	void CTunnelFireModeDlg::processTrackNumberFromHMI()
	{
		// fill the track number automatic, if configed
		std::string strTrackName = getTrackNumberFromSchematic();
		if (!strTrackName.empty())
		{
			m_trackNumber.SetWindowText(strTrackName.c_str());
			m_trackNumber.EnableWindow(FALSE);
		}		
	}

	void CTunnelFireModeDlg::OnSearchButtonClick()
	{
		FUNCTION_ENTRY("OnSearchButtonClick");

		CString strTrackName;
		m_trackNumber.GetWindowText(strTrackName);
		//if( strTrackName.IsEmpty())
		//{
		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Track name is null.");
		//	m_modeListCtrl.DeleteAllItems();
		//	return;
		//}

		unsigned long sectionKey = static_cast<DWORD>(m_sections.GetItemData(m_sections.GetCurSel()));
		TA_IRS_Core::LineType lineType = TA_IRS_Core::enum_Unknown;
		if (CB_ERR != m_lineType.GetCurSel())
		{
			if (TA_IRS_Core::enum_UpBound == m_lineType.GetItemData(m_lineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_UpBound;
			}
			else if (TA_IRS_Core::enum_DownBound == m_lineType.GetItemData(m_lineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_DownBound;
			}
		}

		TA_IRS_Core::TrainPositionByShaft trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Any;
		if ( m_firePositionLeft.IsWindowEnabled() || m_firePositionRight.IsWindowEnabled())
		{
			if ( BST_CHECKED == m_firePositionLeft.GetCheck() ) trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Front;
			if ( BST_CHECKED == m_firePositionRight.GetCheck() ) trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Back;
		}

		unsigned long fireCaseKey = static_cast<DWORD>(m_fireCase.GetItemData(m_fireCase.GetCurSel()));

		TA_IRS_Core::TunnelFireModeInfoVct fireModes = 
			TunnelModeManagerDataHelperInstance::instance()->getFireModesBySelectedCondition(strTrackName.GetBuffer(), sectionKey, lineType,trainPositionByShaft, fireCaseKey);

		m_modeListCtrl.DeleteAllItems();
		removeDuplicateItems(fireModes);
		TA_IRS_Core::TunnelFireModeInfoVct::iterator it;
		for (it = fireModes.begin(); it != fireModes.end(); it++)
		{
			if ((*it)->FrontStation_ModeName.empty() && (*it)->BackStation_ModeName.empty())
			{
				continue;
			}
			std::string strLeftModeType = ((*it)->FrontStation_ModeType.empty()) ? "上行：（无）" : "上行：" + (*it)->FrontStation_ModeType;
			std::string strRightModeType = ((*it)->BackStation_ModeType.empty()) ? " 下行：（无）" : " 下行：" + (*it)->BackStation_ModeType;
			int item = m_modeListCtrl.InsertItem(m_modeListCtrl.GetItemCount(), (strLeftModeType + strRightModeType).c_str());
			std::string strLeftModeDesc = ((*it)->FrontStation_ModeDescription.empty()) ? "上行：（无）" : "上行：" + (*it)->FrontStation_ModeDescription;
			std::string strRightModeDesc = ((*it)->BackStation_ModeDescription.empty()) ? " 下行：（无）" : " 下行：" + (*it)->BackStation_ModeDescription;
			m_modeListCtrl.SetItemText(item, m_modeListCtrl.ColumnIndex_ModeDesc,(strLeftModeDesc + strRightModeDesc).c_str());
			m_modeListCtrl.SetItemText(item, m_modeListCtrl.ColumnIndex_LeftStation,(*it)->FrontStation_ModeName.c_str());
			m_modeListCtrl.SetItemText(item, m_modeListCtrl.ColumnIndex_RightStation,(*it)->BackStation_ModeName.c_str());
			m_modeListCtrl.SetItemData(item, (*it)->Pkey);
		}

		FUNCTION_EXIT;
	}

	void CTunnelFireModeDlg::OnButtonExecuteClick()
	{
		FUNCTION_ENTRY("OnButtonExecuteClick");
		POSITION pos = m_modeListCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			MessageBox("请先选择一个要执行的模式", DIALOG_TITLE, MB_OK | MB_ICONERROR);
			return;
		}

		bool bCanOverride = true;
		int item = m_modeListCtrl.GetNextSelectedItem(pos);
        unsigned long pkey = static_cast<unsigned long>(m_modeListCtrl.GetItemData(item));

        // Control notification handler code here
        int confirmResult = ConfirmModeExe(m_modeListCtrl, item, "执行隧道阻塞模式确认", "隧道阻塞模式");
        if (IDYES != confirmResult)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Cancel tunnel fire mode execution.");
            FUNCTION_EXIT;
            return;
        }

        setIsExecuteModeFlag(true);

        bool bFrontModeConfigured = false;
		bool bBackModeConfigured = false;
		bool bFrontModeSuccess = false;
		bool bBackModeSuccess = false;
        std::string strExecuteResultDes = "";
		TA_IRS_Core::Mode_SmartPtr frontModePtr;
		TA_IRS_Core::Mode_SmartPtr backModePtr;

        BAS_TRY
        {
            unsigned long frontStationKey = 0u;
            unsigned long backStationKey = 0u;
            TA_IRS_Core::BasTunnelFireModeAccessFactoryInstance::instance()->getFrontAndBackStationByKey(pkey, frontStationKey, backStationKey);
            TA_IRS_Core::TunnelFireModeInfo_SmartPtr modePtr = getSelectFireModeInfo(pkey);
			bool bFrontValidModeKey = TA_Base_Core::DatabaseKey::isValidKey( modePtr->FrontStation_ModeKey );
			bool bBackValidModeKey = TA_Base_Core::DatabaseKey::isValidKey( modePtr->BackStation_ModeKey );

			if (bFrontValidModeKey)
			{
				frontModePtr = TunnelModeManagerDataHelperInstance::instance()->getModeByKey(frontStationKey, modePtr->FrontStation_ModeKey);
			}
			if (bBackValidModeKey)
			{
				backModePtr = TunnelModeManagerDataHelperInstance::instance()->getModeByKey(backStationKey, modePtr->BackStation_ModeKey);
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

	TA_IRS_Core::TunnelFireModeInfo_SmartPtr CTunnelFireModeDlg::getSelectFireModeInfo(unsigned long pkey)
	{
		FUNCTION_ENTRY("getSelectFireModeInfo");

		using namespace TA_IRS_Core;
		TA_IRS_Core::TunnelFireModeInfoVct modeVct;
		TunnelFireModeInfo_SmartPtr modeInfo;

		CString strTrackName;
		m_trackNumber.GetWindowText(strTrackName);

		unsigned long sectionKey = static_cast<DWORD>(m_sections.GetItemData(m_sections.GetCurSel()));
		TA_IRS_Core::LineType lineType = TA_IRS_Core::enum_Unknown;
		if (CB_ERR != m_lineType.GetCurSel())
		{
			if (TA_IRS_Core::enum_UpBound == m_lineType.GetItemData(m_lineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_UpBound;
			}
			else if (TA_IRS_Core::enum_DownBound == m_lineType.GetItemData(m_lineType.GetCurSel()))
			{
				lineType = TA_IRS_Core::enum_DownBound;
			}
		}

		TA_IRS_Core::TrainPositionByShaft trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Any;
		if ( m_firePositionLeft.IsWindowEnabled() || m_firePositionRight.IsWindowEnabled())
		{
			if ( BST_CHECKED == m_firePositionLeft.GetCheck() ) trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Front;
			if ( BST_CHECKED == m_firePositionRight.GetCheck() ) trainPositionByShaft = TA_IRS_Core::enum_TrainPos_Back;
		}

		unsigned long fireCaseKey = static_cast<DWORD>(m_fireCase.GetItemData(m_fireCase.GetCurSel()));

		modeVct = TunnelModeManagerDataHelperInstance::instance()->getFireModesBySelectedCondition(strTrackName.GetBuffer(), sectionKey, lineType, trainPositionByShaft, fireCaseKey);
		TA_IRS_Core::TunnelFireModeInfoVct::iterator it;
		for (it = modeVct.begin(); it != modeVct.end(); it++)
		{
			if ((*it)->Pkey == pkey)
			{
				modeInfo = (*it);
				break;
			}
		}
		if (NULL == modeInfo.get())
		{
			TA_THROW(TA_Base_Core::TransactiveException("There no find the select congestion mode info"));
		}
		
		FUNCTION_EXIT;
		return modeInfo;
	}

	void CTunnelFireModeDlg::OnDlgTrackNumberChanged()
	{
		FUNCTION_ENTRY("OnDlgTrackNumberChanged");

		fillwithSectionAndLineTypeAndShaftPositionByTrackName();

		FUNCTION_EXIT;
	}

	void CTunnelFireModeDlg::fillwithSectionAndLineTypeAndShaftPositionByTrackName()
	{
		FUNCTION_ENTRY("fillwithSectionAndLineTypeAndShaftPositionByTrackName");

		CString strTrackName; 
		m_trackNumber.GetWindowText(strTrackName);
		//if( strTrackName.IsEmpty())
		//{
		//	  LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Track name is null.");
        //    return;
		//}

		TA_IRS_Core::TrackInfoVct approxiTrackInfoVct;
		TA_IRS_Core::TrackInfoVct accuTrackInfoVct;
		TA_IRS_Core::TrackInfoVctIter itApproxiTrackInfoVct;
		
		TunnelModeManagerDataHelperInstance::instance()->getTrackInfosByTrackName(strTrackName.GetBuffer(), approxiTrackInfoVct, accuTrackInfoVct);

		if (approxiTrackInfoVct.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Can't found the track name of including string:%s, please check the data.", strTrackName.GetBuffer());

			//Disable filter Controls
			m_sections.EnableWindow(FALSE);
			m_lineType.EnableWindow(FALSE);
			m_fireCase.EnableWindow(FALSE);
			m_firePositionLeft.EnableWindow(FALSE);
			m_firePositionRight.EnableWindow(FALSE);
		}
		else
		{
			//Enable filter Controls
			m_sections.EnableWindow(TRUE);
			m_lineType.EnableWindow(TRUE);
			m_fireCase.EnableWindow(TRUE);
			m_firePositionLeft.EnableWindow(TRUE);
			m_firePositionRight.EnableWindow(TRUE);

			int indexLineType = 0;
			int indexSection = 0;
			bool isRepeatForLineType = false;
			bool isRepeatForSection = false;
			itApproxiTrackInfoVct = approxiTrackInfoVct.begin();
			m_sections.ResetContent();
			m_lineType.ResetContent();

			for(; itApproxiTrackInfoVct != approxiTrackInfoVct.end(); ++itApproxiTrackInfoVct)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found track info by track name of including string %s sectionKey=%d LineType=%s FirePositionByShaft=%s",
					strTrackName.GetBuffer(), (*itApproxiTrackInfoVct)->sectionKey, lineTypeToString((*itApproxiTrackInfoVct)->lineType).c_str(), trainPositionByShaftToString((*itApproxiTrackInfoVct)->firePosition).c_str());

				isRepeatForLineType = false;
				isRepeatForSection = false;

				//Set line type
				TA_IRS_Core::LineType lineType = TA_IRS_Core::LineType::enum_Unknown;
				for(int nCount=0; nCount < m_lineType.GetCount();nCount++)
				{					
					lineType = static_cast<TA_IRS_Core::LineType>(m_lineType.GetItemData(nCount));
					if((*itApproxiTrackInfoVct)->lineType == lineType)
					{
						isRepeatForLineType = true;
						break;
					}
				}

				if((!isRepeatForLineType) && ((*itApproxiTrackInfoVct)->lineType != TA_IRS_Core::LineType::enum_Unknown))
				{
					int item = m_lineType.InsertString(indexLineType++, m_lineTypeMap[(*itApproxiTrackInfoVct)->lineType].c_str());
					m_lineType.SetItemData(item, (*itApproxiTrackInfoVct)->lineType);
				}

				//Set sections
				unsigned long sectionKey = TA_Base_Core::DatabaseKey::getInvalidKey();
				for(int nCount=0; nCount < m_sections.GetCount();nCount++)
				{
					sectionKey = static_cast<DWORD>(m_sections.GetItemData(nCount));
					if((*itApproxiTrackInfoVct)->sectionKey == sectionKey)
					{
						isRepeatForSection = true;
						break;
					}
				}

				BAS_TRY
				{
					if(!isRepeatForSection)
					{
						TA_IRS_Core::StationSectionInfo sectionInfo =  TunnelModeManagerDataHelperInstance::instance()->getStationSectionByKey((*itApproxiTrackInfoVct)->sectionKey);

						int item = m_sections.InsertString(indexSection++, sectionInfo.strName.c_str());
						m_sections.SetItemData(item, sectionInfo.pkey);
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
			if(m_sections.GetCount() > 0)
			{
				m_sections.SetCurSel(0);
			}

			if(m_lineType.GetCount() > 0)
			{
				m_lineType.SetCurSel(0);
			}

			//Set fire shaft position and update radio status
            updateShaftRadioButton(accuTrackInfoVct);
		}

		FUNCTION_EXIT;
	}

	void CTunnelFireModeDlg::setSelItemByData(CComboBox* comboBox, unsigned long data)
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

	void CTunnelFireModeDlg::removeDuplicateItems(TA_IRS_Core::TunnelFireModeInfoVct& fireModes)
	{
		TA_IRS_Core::TunnelFireModeInfoVct::iterator it;
		TA_IRS_Core::TunnelFireModeInfoVct::iterator next;
		for (it = fireModes.begin(); it != fireModes.end(); it++)
		{
			for (next = it + 1; next != fireModes.end(); )
			{
				if ((*it)->FrontStation_ModeKey == (*next)->FrontStation_ModeKey &&
					(*it)->BackStation_ModeKey == (*next)->BackStation_ModeKey)
				{
					next = fireModes.erase(next);
				}
				else
				{
					next++;
				}
			}
		}
	}

	void CTunnelFireModeDlg::updateShaftRadioButton(TA_IRS_Core::TrackInfoVct& trackInfoVct)
	{
		FUNCTION_ENTRY("updateShaftRadioButton");

		if(trackInfoVct.empty())
		{
			//Return to initial status
			m_firePositionLeft.SetCheck(BST_UNCHECKED);
			m_firePositionRight.SetCheck(BST_UNCHECKED);
			m_firePositionLeft.EnableWindow(FALSE);
			m_firePositionRight.EnableWindow(FALSE);

			return;
		}

		const int ACCURATE_TRACT_INFORS_ONE = 1;
		const int ACCURATE_TRACT_INFORS_TWO = 2;

		TA_IRS_Core::TrackInfoVctIter itTrackInfoVct = trackInfoVct.begin();

		if( ACCURATE_TRACT_INFORS_ONE == trackInfoVct.size())
		{
			if ((*itTrackInfoVct)->firePosition == TA_IRS_Core::enum_TrainPos_Front)
			{
				m_firePositionLeft.SetCheck(BST_CHECKED);
				m_firePositionRight.SetCheck(BST_UNCHECKED);
				m_firePositionLeft.EnableWindow(TRUE);
				m_firePositionRight.EnableWindow(FALSE);
			}
			else if ((*itTrackInfoVct)->firePosition == TA_IRS_Core::enum_TrainPos_Back)
			{
				m_firePositionLeft.SetCheck(BST_UNCHECKED);
				m_firePositionRight.SetCheck(BST_CHECKED);
				m_firePositionLeft.EnableWindow(FALSE);
				m_firePositionRight.EnableWindow(TRUE);				
			}
			else
			{
				m_firePositionLeft.SetCheck(BST_UNCHECKED);
				m_firePositionRight.SetCheck(BST_UNCHECKED);
				m_firePositionLeft.EnableWindow(FALSE);
				m_firePositionRight.EnableWindow(FALSE);
			}
		}
		else if (ACCURATE_TRACT_INFORS_TWO == trackInfoVct.size() && (*itTrackInfoVct)->firePosition != TA_IRS_Core::enum_TrainPos_Any && (*(itTrackInfoVct + 1))->firePosition != TA_IRS_Core::enum_TrainPos_Any)
		{
			if ((*itTrackInfoVct)->firePosition == TA_IRS_Core::enum_TrainPos_Front && (*( itTrackInfoVct + 1))->firePosition == TA_IRS_Core::enum_TrainPos_Back)
			{
				m_firePositionLeft.SetCheck(BST_CHECKED);
				m_firePositionRight.SetCheck(BST_UNCHECKED);
				m_firePositionLeft.EnableWindow(TRUE);
				m_firePositionRight.EnableWindow(TRUE);
			}
			if ((*itTrackInfoVct)->firePosition == TA_IRS_Core::enum_TrainPos_Back && (*( itTrackInfoVct + 1))->firePosition == TA_IRS_Core::enum_TrainPos_Front)
			{
				m_firePositionLeft.SetCheck(BST_UNCHECKED);
				m_firePositionRight.SetCheck(BST_CHECKED);
				m_firePositionLeft.EnableWindow(TRUE);
				m_firePositionRight.EnableWindow(TRUE);
			}
		}
		else
		{
			//Return to initial status
			m_firePositionLeft.SetCheck(BST_UNCHECKED);
			m_firePositionRight.SetCheck(BST_UNCHECKED);
			m_firePositionLeft.EnableWindow(FALSE);
			m_firePositionRight.EnableWindow(FALSE);

			CString strTrackName; 
			m_trackNumber.GetWindowText(strTrackName);
			CString strErrorMsg;
			strErrorMsg.Format("轨道号[%s]配置风井位置信息错误", strTrackName.GetBuffer());
			MessageBox(strErrorMsg, DIALOG_TITLE, MB_OK | MB_ICONERROR);
		}

		FUNCTION_EXIT;
	}
}

