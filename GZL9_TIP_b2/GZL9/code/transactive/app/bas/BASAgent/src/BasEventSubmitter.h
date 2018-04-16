/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasEventSubmitter.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#ifndef BASEVENTSUBMITTER_H
#define BASEVENTSUBMITTER_H

#include "ace/Singleton.h"
#include <string>
#include "core/message/src/NameValuePair.h"

namespace TA_Base_Core
{
    class MessageType;
    class AuditMessageSender;
}

namespace TA_IRS_App
{
    class BasEventSubmitterImpl
    {
    public:
        friend class ACE_Singleton<BasEventSubmitterImpl, ACE_Recursive_Thread_Mutex>;

		void submitBasSystemModeControlEvent(const std::string& dpEventMessage, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);
        void submitBasSystemModeExecutionEvent(const std::string& basModeDescription, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);
        void submitBasTimetableDownloadEvent(const int& timetableType, const std::string& tableName, const bool bSucceed, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);
        void submitBasSystemModeChangeEvent(const std::string& basModeDescription, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);
        void submitBasValveParamDownloadEvent(const std::string& dpEventMessage, const int nResult, const std::string& eventValue, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);
        void submitBasCancelFireModeEvent(const std::string& dpEventMessage, const int nResult, const std::string& operatorSession, const unsigned long entityKey, const std::string& assetName);

        void submitAuditMessage ( const TA_Base_Core::DescriptionParameters & description, const TA_Base_Core::MessageType & messageType, const std::string& sessionID, const unsigned long entityKey, const std::string& assetName);

        void setControlMode(const bool bControlMode);

    private:
        BasEventSubmitterImpl();
        ~BasEventSubmitterImpl();

        BasEventSubmitterImpl(const BasEventSubmitterImpl&);
        BasEventSubmitterImpl& operator=(const BasEventSubmitterImpl&);

        TA_Base_Core::AuditMessageSender * m_auditMessageSender;
        bool m_isControlMode;
    };
    typedef ACE_Singleton<BasEventSubmitterImpl, ACE_Recursive_Thread_Mutex> BasEventSubmitter;
}
#endif // BASEVENTSUBMITTER_H