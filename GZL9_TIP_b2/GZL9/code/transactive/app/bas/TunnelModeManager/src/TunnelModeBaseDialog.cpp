// src\TunnelModeBaseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeBaseDialog.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxyFactory.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "bus/bas/bas_agent/idl/src/IStationBasAgentCorbaDef.h"
#include "core/exceptions/idl/src/CommonExceptionsCorbaDef.h"
namespace TA_IRS_App
{
	// CTunnelModeBaseDialog dialog
	CTunnelModeBaseDialog::CTunnelModeBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
		: CDialog(nIDTemplate, pParent),m_isExecuteMode(false)
	{

	}

	CTunnelModeBaseDialog::~CTunnelModeBaseDialog()
	{
	}

	void CTunnelModeBaseDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}

	// CTunnelModeBaseDialog message handlers
	BOOL CTunnelModeBaseDialog::OnInitDialog()
	{
		return CDialog::OnInitDialog();
	}

	void CTunnelModeBaseDialog::OnCancel()
	{
	}

    int CTunnelModeBaseDialog::ConfirmModeExe(CTunnelModeListCtrl& modeList, int item, std::string titel, std::string modeTypeName)
    {
        FUNCTION_ENTRY("CTunnelModeBaseDialog::ConfirmModeExe");
        
        TA_ASSERT(item >= 0, "Item number cant be less then 0.");
        std::stringstream ss;
        ss << "请确认是否执行" << modeTypeName << modeList.GetItemText(item, modeList.ColumnIndex_ModeType)
            << ":" << modeList.GetItemText(item, modeList.ColumnIndex_ModeDesc);
        std::string upStation = modeList.GetItemText(item, modeList.ColumnIndex_LeftStation);
        std::string downStation = modeList.GetItemText(item, modeList.ColumnIndex_RightStation);
        if (upStation.empty())
            upStation = "(无)";
        if (downStation.empty())
            upStation = "(无)";
        ss << "在上行车站 " << upStation << " 和下行车站 " << downStation;
        int confirmResult = MessageBox(_T(ss.str().c_str()), _T(titel.c_str()), MB_YESNO | MB_TOPMOST);

        FUNCTION_EXIT;
        return confirmResult;
    }

	bool CTunnelModeBaseDialog::isThreadRunning(const TA_Base_Core::Thread* thread)
	{
		bool bRunning = false;
		if (NULL != thread)
		{
			if(thread->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_NEW || 
				thread->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
			{
				bRunning = true;
			}
		}
		return bRunning;
	}

	void CTunnelModeBaseDialog::setIsExecuteModeFlag(bool bExecuteNow)
	{
		m_isExecuteMode = bExecuteNow;
	}

	bool CTunnelModeBaseDialog::getIsExecuteModeFlag()
	{
		return m_isExecuteMode;
	}

	std::string CTunnelModeBaseDialog::getTrackNumberFromSchematic()
	{
		std::string strTrackNumber = "";
		using namespace TA_Base_Core;
		if (false == RunParams::getInstance().isSet(RPARAM_CONTROL_ENTITY))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Please Check the Run Parameter : Miss the track number!!!");
		}
		else
		{
			strTrackNumber = RunParams::getInstance().get(RPARAM_CONTROL_ENTITY);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receive the track number from HMI [%s]", strTrackNumber.c_str());
		}
		return strTrackNumber;
	}

	std::string CTunnelModeBaseDialog::executeStationMode(TA_IRS_Core::Mode_SmartPtr stationModePtr, bool canOverrideStationMode, const std::string& sessionId)
	{
		FUNCTION_ENTRY("executeStationMode");
		std::string strSubsystemName = stationModePtr->getSubsystemName();
		std::string strExceptionString = "";

		try
		{
			//BasAgentAccessFactoryInstance::instance().executeStationMode(locationKey, stationModePtr->getSubsystemKey(), stationModePtr->getValue(), canOverrideStationMode, sessionId);
            TA_IRS_Bus::BasAgentProxyFactoryInstance::instance()->getBasAgentProxyByLocationKey(stationModePtr->getLocationKey())->executeStationMode(stationModePtr->getSubsystemKey(), stationModePtr->getValue(), canOverrideStationMode, sessionId);
		}
        catch(const TA_IRS_Bus::BasNotDeployStationException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            strExceptionString = "此站点还没有部署，请联系管理员！";
        }
		catch(const TA_IRS_Bus::BasAgentInvalidException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
			strExceptionString = "执行过程中系统异常，无法连接到服务器！";
		}
		catch(const TA_Base_Core::TransactiveException& ex)
		{		
			strExceptionString = ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
			strExceptionString = "执行过程中系统异常";
		}
        catch(const TA_Base_Core::AccessDeniedException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Access denied", ex.reason.in());
            strExceptionString = "无权限执行模式";
        }
		catch(const TA_Base_Core::NotAvailableException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", ex.reason.in());
			strExceptionString = "执行过程中数据配置异常";
		}
		catch(const std::exception& ex)
		{
			strExceptionString = ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
			strExceptionString = "执行过程中数据异常";
		}
		catch(const TA_IRS_Bus::IStationBasAgentCorbaDef::StationInLocalMode& ex)
		{
			strExceptionString = static_cast< const char* >(ex.what);
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::IStationBasAgentCorbaDef::StationInLocalMode", strExceptionString.c_str());
			strExceptionString = strSubsystemName + " 当前为时间表控制, 模式只能在手动模式时执行";		
		}
		catch(const TA_IRS_Bus::IStationBasAgentCorbaDef::StationModeInProgress& ex)
		{
			strExceptionString = static_cast< const char* >(ex.what);
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::IStationBasAgentCorbaDef::StationModeInProgress", strExceptionString.c_str());
			strExceptionString =strSubsystemName +" 当前有正在执行的模式";
		}
		catch(const CORBA::Exception& ex)
		{
			strExceptionString = static_cast< const char* >(ex._name());
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", strExceptionString.c_str());
			strExceptionString = "执行过程中通信异常";
		} 
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur.");
			strExceptionString = "执行过程中出现未知异常";
		}
		FUNCTION_EXIT;
		return strExceptionString;
	}

}
