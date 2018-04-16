/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/BasEventSubmitter.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/27 16:26:25 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */

#include "app/bas/BASAgent/src/BasEventSubmitter.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "IDataPointUpdateObserver.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/ECSAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"

namespace TA_IRS_App
{

    BasEventSubmitterImpl::BasEventSubmitterImpl() : 
    m_auditMessageSender(NULL),
    m_isControlMode(false)
    {
        try
        {
            m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::ECSAudit::Context);
        }
        catch (...)
        {
        }
    }

    BasEventSubmitterImpl::~BasEventSubmitterImpl()
    {
        if (NULL != m_auditMessageSender)
        {
            delete m_auditMessageSender;
            m_auditMessageSender = NULL;
        }
    }

    void BasEventSubmitterImpl::submitBasSystemModeControlEvent(const std::string& dpEventMessage, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName)
    {
        TA_Base_Core::DescriptionParameters eventParams;

        TA_Base_Core::NameValuePair basModePair("DataPointEventMessage", dpEventMessage);
        TA_Base_Core::NameValuePair valueStringPair("ValueString", eventValue);
        TA_Base_Core::NameValuePair resultStringPair("result", ( E_STATE_SUCCESS == nResult )  ? "成功" : "失败" );

        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&basModePair);
        eventParams.push_back(&resultStringPair);

        submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasSystemModeControl, operatorSession, entityKey, assetName);
		LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send BasSystemModeControl event, DataPointEventMessage:%s, result:%d, value:%s, session:%s, entityKey:%lu,assetName=%s ", 
            dpEventMessage.c_str(), nResult, eventValue.c_str(), operatorSession.c_str(), entityKey, assetName.c_str());
    }

    void BasEventSubmitterImpl::submitBasSystemModeExecutionEvent( const std::string& basModeDescription, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName )
    {
        TA_Base_Core::DescriptionParameters eventParams;

        TA_Base_Core::NameValuePair basModePair("BasModeDescription", basModeDescription);
        TA_Base_Core::NameValuePair valueStringPair("ValueString", eventValue);

        std::string strResult = "";
        switch ( nResult )
        {
        case E_STATE_SUCCESS:
            strResult = "成功";
            break;
        case E_STATE_FAIL_GENERAL:
            strResult = "失败";
            break;
        case E_STATE_FAIL_TIME_OUT:
            strResult = "超时";
            break;
        default:
            break;
        }

        TA_Base_Core::NameValuePair resultStringPair("result", strResult);

        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&basModePair);
        eventParams.push_back(&resultStringPair);

        submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasSystemModeExecution, operatorSession, entityKey, assetName);
        LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send BasSystemModeExecution event, BasModeDescription:%s, result:%d, value:%s, session:%s, entityKey:%lu,assetName=%s", 
            basModeDescription.c_str(), nResult, eventValue.c_str(), operatorSession.c_str(), entityKey, assetName.c_str());
    }

    void BasEventSubmitterImpl::submitBasSystemModeChangeEvent( const std::string& basModeDescription, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName )
    {
        TA_Base_Core::NameValuePair basModePair("BasModeDescription", basModeDescription);
        TA_Base_Core::NameValuePair valueStringPair("ValueString", eventValue);

        TA_Base_Core::DescriptionParameters eventParams;
        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&basModePair);

        submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasSystemModeChanged, operatorSession, entityKey, assetName);
        LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send BasSystemModeChanged event, BasModeDescription:%s, value:%s, session:%s, entityKey:%lu,assetName=%s", 
            basModeDescription.c_str(), eventValue.c_str(), operatorSession.c_str(), entityKey, assetName.c_str());
    }

    void BasEventSubmitterImpl::submitBasValveParamDownloadEvent( const std::string& dpEventMessage, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName )
    {
        TA_Base_Core::DescriptionParameters eventParams;

        TA_Base_Core::NameValuePair basModePair("DataPointEventMessage", dpEventMessage);
        TA_Base_Core::NameValuePair valueStringPair("ValueString", eventValue);
        TA_Base_Core::NameValuePair resultStringPair("result", ( E_STATE_SUCCESS == nResult )  ? "成功" : "失败");
        
        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&basModePair);
        eventParams.push_back(&resultStringPair);

        submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasValveParamDownload, operatorSession, entityKey, assetName);
        LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send BasValveParamDownload event, DataPointEventMessage:%s, result:%d, value:%s, session:%s, entityKey:%lu, assetName=%s", 
            dpEventMessage.c_str(), nResult, eventValue.c_str(), operatorSession.c_str(), entityKey, assetName.c_str());
    }

	void BasEventSubmitterImpl::submitBasCancelFireModeEvent( const std::string& dpEventMessage, const int nResult, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName )
    {
        TA_Base_Core::DescriptionParameters eventParams;

        TA_Base_Core::NameValuePair basModePair("DataPointEventMessage", dpEventMessage);
        TA_Base_Core::NameValuePair valueStringPair("ValueString", "灾后复位");
        TA_Base_Core::NameValuePair resultStringPair("result", ( E_STATE_SUCCESS == nResult )  ? "成功" : "失败");

        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&basModePair);
        eventParams.push_back(&resultStringPair);

        submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasCancelFireMode, operatorSession, entityKey, assetName);
        LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send BasCancelFireMode event, DataPointEventMessage:%s, result:%d, session:%s, entityKey:%lu, assetName=%s", 
            dpEventMessage.c_str(), nResult, operatorSession.c_str(), entityKey, assetName.c_str());
    }

    void BasEventSubmitterImpl::submitAuditMessage( const TA_Base_Core::DescriptionParameters & description, const TA_Base_Core::MessageType & messageType, const std::string& sessionID, const unsigned long entityKey, const std::string& assetName )
    {
        if ( !m_isControlMode )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "submitAuditMessage %s when in Monitor Mode", messageType.getTypeKey().c_str() );
            return;
        }

        if ( NULL == m_auditMessageSender )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "submitAuditMessage %s when sender is NULL", messageType.getTypeKey().c_str() );
            return;
        }

        try
        {
			m_auditMessageSender->sendAuditMessageWithAsset(messageType, entityKey, description, "", sessionID, "", "", "", assetName);
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to submitAuditMessage");
            // do nothing
        }
    }

    void BasEventSubmitterImpl::submitBasTimetableDownloadEvent( const int& timetableType, const std::string& tableName, const bool bSucceed, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName )
    {
        TA_Base_Core::DescriptionParameters eventParams;

        TA_Base_Core::NameValuePair valueStringPair("ValueString", "时间表下载");
        TA_Base_Core::NameValuePair locationNamePair("LocationName", BasCachedConfig::getInstance().getLocDispName());
        TA_Base_Core::NameValuePair timetableTypePair("TimetableType", tableName);
        TA_Base_Core::NameValuePair resultPair("result", bSucceed ? "成功" : "失败" );

        eventParams.push_back(&valueStringPair);
        eventParams.push_back(&locationNamePair);
        eventParams.push_back(&timetableTypePair);
        eventParams.push_back(&resultPair);

        switch (timetableType)
        {
            case TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_Today:
            {
                submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasTodayTimetableDownload, operatorSession, entityKey, assetName);
                break;
            }
            case TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_NextDay:
            {
                submitAuditMessage(eventParams, TA_Base_Core::ECSAudit::BasTomorrowTimetableDownload, operatorSession, entityKey, assetName);
                break;
            }
            default:
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized message Type %lu", timetableType);
                return;
        }

        LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Send timetable download event, today/tomorrow:%lu, time table name:%s, success:%d, session:%s, entityKey:%lu, assetName=%s", 
            timetableType, tableName.c_str(), bSucceed, operatorSession.c_str(), entityKey, assetName.c_str());
    }

    void BasEventSubmitterImpl::setControlMode( const bool bControlMode )
    {
        m_isControlMode = bControlMode;
    }

}