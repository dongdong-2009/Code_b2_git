#if !defined(TA_GF_3002_App_base_BASManager_CommonDef_H)
#define TA_GF_3002_App_base_BASManager_CommonDef_H
#pragma once

// include the common headers
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>
#include "omniORB4/CORBA.h"
#include "app/bas/Common/BasDefinitions.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"

// some constant
const unsigned int gDlgTopBlank = 7;
const unsigned int gDlgLeftBlank = 7;
const unsigned int gDlgRightBlank = 7;
const unsigned int gDlgBottomBlank = 7;

const std::string gControlManualToString = _T("手动");
const std::string gControlHMIToString = _T("HMI模式控制");
const std::string gControlTimeTabToString = _T("时间表");
const std::string gControlAutoToString = _T("自动");
const std::string gControlIBPToString = _T("IBP盘控制");
const std::string gControlFASToString = _T("FAS触发火灾");
const std::string gControlLinkageToString = _T("联动");

const std::string gSetControlTypeHMIToString = _T("HMI模式控制");
const std::string gSetControlTypeTimeTabToString = _T("时间表");
const std::string gSetControlTypeAutoToString = _T("自动");

const std::string gInvalidValueToString = _T("未定义");

// BasMangaer Action enum
enum BasManagerAction
{
	BasManagerAction_Initialize = 0,
	BasManagerAction_ChangeStation
};

enum EquipmentValueType
{
    EVT_CurrentExcuteMode = 0,
    EVT_CurrentExcuteModeBit = 1,
    EVT_CurrentExcuteModeStatus = 2,
};

#define ReportlistCtrlColorTimer 55
#define ReportListCtrlColorTimerInternal 1000
#define ModeListCtrlColorTimer 11
#define ModeListCtrlColorTimerInternal 1000
#define ManagerQuitColorTimer 22
#define ManagerQuitColorTimerInternal 400 

#define MANAGERSTATUS_FIRE "StationFire"
#define RPARAM_MANAGERSTATUS "ManagerMode"
#define RPARAM_MANAGERMODE_VALUE "ModeValue"
#define RPARAM_FIRELOCATIONKEY "FireLocationKey"

//MessageBox Caption
#define MessageBox_Caption "模式管理器"
// Bool to string
#define BooleanToString(value) (value ? "true" : "false")

// some macro define 
class BasNotDeployStationException;
#define BAS_TRY \
std::string strExceptionString = ""; \
try

#define BAS_CATCH \
catch(const TA_Base_Core::TransactiveException& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what()); \
} \
catch(const std::exception& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what()); \
} \
catch(const CORBA::Exception& ex) \
{ \
	strExceptionString = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex); \
	LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", strExceptionString.c_str()); \
} \
catch(...) \
{ \
	strExceptionString = " 未知错误 "; \
	LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur."); \
}


#define BAS_CATCH_UNDEPLOY_EXCEPTION \
catch(const TA_IRS_Bus::BasNotDeployStationException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what()); \
	MessageBox("此站点还没有部署，请联系管理员！", MessageBox_Caption, MB_OK | MB_ICONERROR ); \
} \
catch(const TA_Base_Core::TransactiveException& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what()); \
} \
catch(const std::exception& ex) \
{ \
	strExceptionString = ex.what(); \
	LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what()); \
} \
catch(const CORBA::Exception& ex) \
{ \
	strExceptionString = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex); \
	LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", strExceptionString.c_str()); \
} \
catch(...) \
{ \
	strExceptionString = " 未知错误 "; \
	LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur."); \
}

#define BAS_GetExceptionString strExceptionString.c_str()

#define BAS_WhetherHaveError (!strExceptionString.empty())


#define BAS_IsWindow(ctrl_ID) (NULL != GetDlgItem(ctrl_ID) && IsWindow(GetDlgItem(ctrl_ID)->m_hWnd))

#define BAS_MoveFixedSizeCtrl(ctrl_ID, cx, cy) \
{ \
	CRect ctrlRect;	\
	GetDlgItem(ctrl_ID)->GetWindowRect(ctrlRect); \
	ctrlRect.MoveToXY(cx, cy); \
	GetDlgItem(ctrl_ID)->MoveWindow(ctrlRect, FALSE); \
}

// For the namespace 
#define Namespace_TA_IRS_APP_Begin \
namespace TA_IRS_App \
{ 

#define Namespace_TA_IRS_APP_End \
}

// Following is the user defined message
#define UserDefinedMessageBase (WM_USER + 100)
// use it when dialog start up, it should be used once
#define Msg_InitializeDialogData (UserDefinedMessageBase + 1)
#define Msg_UpdateCurrentMode    (UserDefinedMessageBase + 2)
#define Msg_UpdateExpectMode     (UserDefinedMessageBase + 3)
#define Msg_UpdateSubsystemAutoManual (UserDefinedMessageBase + 4)
#define Msg_UpdateModeSatus      (UserDefinedMessageBase + 5)

//This for device list. Cuilike++. GF-78;
#define Msg_UpdateDataPointValue	(UserDefinedMessageBase + 6)

class BasNoSubsystemException : public TA_Base_Core::TransactiveException
{
public:
	BasNoSubsystemException(const std::string& msg) throw()
		:TA_Base_Core::TransactiveException(msg)
	{
	}
};

class BasUnknownMode : public TA_Base_Core::TransactiveException
{
public:
	BasUnknownMode(const std::string& msg) throw()
		:TA_Base_Core::TransactiveException(msg)
	{
	}
};
class BASManagerUtility
{
public:
	static unsigned long stringToUnsignedLong(const std::string& value)
	{
		unsigned long retValue;
		std::stringstream ss;
		ss << value;	
		ss >> retValue;
		return retValue;
	}

	static std::string unsignedlongToString(unsigned long value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
	
	static std::string doubleToString(double value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	static std::string autoCFGValueToString(const int nValue)
	{
		static std::string strRetString = "";
        switch (nValue)
        {
        case eControlAuto:
            strRetString = gControlAutoToString;
            break;
        case eControlManual:
            strRetString = gControlManualToString;
            break;
        case eControlHMI:
            strRetString = gControlHMIToString;
            break;
        case eControlTimeTalbe:
            strRetString = gControlTimeTabToString;
            break;
        case eControlIBP:
            strRetString = gControlIBPToString;
            break;
        case eControlFAS:
            strRetString = gControlFASToString;
            break;
        case eControlLinkage:
            strRetString = gControlLinkageToString;
            break;
        default:
            strRetString = gInvalidValueToString;
            break;
        }
		return strRetString;
	}

    static std::string systemControlModeValueToString(const int nValue)
    {
        static std::string strRetString = "";
        switch (nValue)
        {
        case eSetControlTypeHMI:
            strRetString = gSetControlTypeHMIToString;
            break;
        case eSetControlTypeTimeTable:
            strRetString = gSetControlTypeTimeTabToString;
            break;
        case eSetControlTypeAuto:
            strRetString = gSetControlTypeAutoToString;
            break;
        default:
            strRetString = gInvalidValueToString;
            break;
        }
        return strRetString;
    }

    static std::map<unsigned long, std::string> getAllSystemControlModeValues()
    {
        std::map<unsigned long, std::string> mapRetStrings;
        unsigned long ulValue = eSetControlTypeUnknown;
        ulValue = eSetControlTypeHMI;
        mapRetStrings[ulValue] = gSetControlTypeHMIToString;
        ulValue = eSetControlTypeTimeTable;
        mapRetStrings[ulValue] = gSetControlTypeTimeTabToString;
        ulValue = eSetControlTypeAuto;
        mapRetStrings[ulValue] = gSetControlTypeAutoToString;
        return mapRetStrings;
    }
};

#endif //TA_GF_3002_App_base_BASManager_CommonDef_H
