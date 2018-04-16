// src\TunnelModeSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "core/exceptions/src/ApplicationException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/bas/TunnelModeManager/src/TunnelModeSelectDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelCongestionModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelFireModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDlg.h"

namespace TA_IRS_App
{
    const char* const DIALOG_TITLE = "隧道模式选择器";

	// CTunnelModeSelectDlg dialog
	IMPLEMENT_DYNAMIC(CTunnelModeSelectDlg, CTunnelModeBaseDialog)

	CTunnelModeSelectDlg::CTunnelModeSelectDlg(CWnd* pParent /*=NULL*/)
		: CTunnelModeBaseDialog(CTunnelModeSelectDlg::IDD, pParent)
	{

	}

	CTunnelModeSelectDlg::~CTunnelModeSelectDlg()
	{
	}

	void CTunnelModeSelectDlg::DoDataExchange(CDataExchange* pDX)
	{
		CTunnelModeBaseDialog::DoDataExchange(pDX);
	}


	BEGIN_MESSAGE_MAP(CTunnelModeSelectDlg, CTunnelModeBaseDialog)
		ON_BN_CLICKED(IDC_BUTTON_FIRE, OnButtonFireModeClick)
		ON_BN_CLICKED(IDC_BUTTON_BLOCK, OnButtonCongestionModeClick)
	END_MESSAGE_MAP()

	// CTunnelModeSelectDlg message handlers
	BOOL CTunnelModeSelectDlg::OnInitDialog()
	{
		return CTunnelModeBaseDialog::OnInitDialog();
	}

	bool CTunnelModeSelectDlg::canCloseWindow()
	{
		FUNCTION_ENTRY("canCloseWindow");
		FUNCTION_EXIT;
		return true;
	}

	TA_Base_Bus::ResizingProperties CTunnelModeSelectDlg::getResizingProperties()
	{
		FUNCTION_ENTRY("getResizingProperties");
		TA_Base_Bus::ResizingProperties resizeProperty;
		resizeProperty.canMaximise = false;
		resizeProperty.maxHeight = gMFT_Screen_Height;
		resizeProperty.maxWidth = gMFT_Screen_Width;
		resizeProperty.minHeight = 270;
		resizeProperty.minWidth = 550;
		FUNCTION_EXIT;
		return resizeProperty;
	}

	DlgInitializeSize CTunnelModeSelectDlg::getDlgInitSize()
	{
		FUNCTION_ENTRY("getDlgInitSize");
		DlgInitializeSize initDlgSize(550,270);
		FUNCTION_EXIT;
		return initDlgSize;
	}

	void CTunnelModeSelectDlg::OnButtonFireModeClick()
	{
		FUNCTION_ENTRY("OnButtonFireModeClick");
		launchSpecialApplication(TunnelModeType::TunnelMode_Fire);
		FUNCTION_EXIT;
	}

	void CTunnelModeSelectDlg::OnButtonCongestionModeClick()
	{
		FUNCTION_ENTRY("OnButtonCongestionModeClick");
		launchSpecialApplication(TunnelModeType::TunnelMode_Congestion);
		FUNCTION_EXIT;
	}

	bool CTunnelModeSelectDlg::launchSpecialApplication(TunnelModeType applicationType)
	{
		bool bLaunchSuccess = false;
		RECT newRect = { 0, 0, 0, 0};
		std::string commandLine = "";
		unsigned long appType = 0;

		//Get application type
		appType = (applicationType == TunnelModeType::TunnelMode_Fire) ? BAS_TUNNEL_FIRE_MANAGER_GUI_APPTYPE : BAS_TUNNEL_CONGESTION_MANAGER_GUI_APPTYPE;

		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_CONTROL_ENTITY))
		{
            commandLine += "--";
            commandLine += RPARAM_CONTROL_ENTITY;
            commandLine += "=";
            commandLine += TA_Base_Core::RunParams::getInstance().get(RPARAM_CONTROL_ENTITY);
		}

		try
		{
			CPoint pt = GetMessagePos();
			newRect = TA_Base_Bus::AppLauncher::getInstance().getRect( TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x );
		}
		catch( const TA_Base_Core::ApplicationException& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", expWhat.what());
		}
		catch( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when call getRect.");
		}

		try
		{
			TA_Base_Bus::AppLauncher::getInstance().launchApplication( appType, commandLine, TA_Base_Bus::TA_GenericGui::POS_BOUNDED, TA_Base_Bus::TA_GenericGui::ALIGN_FIT, NULL, &newRect );
			bLaunchSuccess = true;
		}
		catch( const TA_Base_Core::ApplicationException& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", expWhat.what());
		}
		catch( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when launch tunnel manager.");
		}

		if (bLaunchSuccess)
		{            
			CTunnelModeManagerDlg* parent = dynamic_cast<CTunnelModeManagerDlg*>(GetParent());
			parent->OnClose();
		}
		else
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			CString action = "打开";
			userMsg << action;
			userMsg.showMsgBox(IDS_UE_220005,getApplicationCaption().c_str());
		}

		return bLaunchSuccess;
	}

    std::string CTunnelModeSelectDlg::getApplicationCaption()
	{
		return DIALOG_TITLE;
	}
}

