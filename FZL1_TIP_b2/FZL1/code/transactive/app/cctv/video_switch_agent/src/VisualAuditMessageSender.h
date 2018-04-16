#ifndef VISUAL_AUDIT_MESSAGE_SENDER_H
#define VISUAL_AUDIT_MESSAGE_SENDER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/VisualAuditMessageSender.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Broadcasts messages to the GUIs and any other Video Switch Agents
  * notifying them of state updates.
  * This class is a singleton.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <memory>
#include <string>
#include <boost/noncopyable.hpp>
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>


// Forward declarations.

namespace TA_Base_Core
{
    class AuditMessageSender;
}

namespace TA_IRS_App
{
	class VisualAuditMessageSender : boost::noncopyable
    {
		friend class ACE_Singleton<VisualAuditMessageSender, ACE_Recursive_Thread_Mutex>;
        public:

            enum EAuditMessageType
            {
                SwitchInputToOutputAttempt,
                SwitchInputToOutputSuccess,
                SwitchInputToOutputFailure,
                LockAttempt,
                LockSuccess,
                LockFailure,
                UnlockAttempt,
                UnlockSuccess,
                UnlockFailure,
                OverrideLockAttempt,
                OverrideLockSuccess,
                OverrideLockFailure,
                ActionAttempt,
                ActionSuccess,
                ActionFailure,
                EditSequence  //TD14548
            };

            static VisualAuditMessageSender& getInstance();
            ///////////////////////////////////////////////////////////////////////
            //
            // Audit Methods
            //
            ///////////////////////////////////////////////////////////////////////


            /**
              * sendMessage
              *
              * Sends an audit message of the type provided.
              *
              * @param      EAuditMessageType messageType
              *             The type of audit message.
              * @param      unsigned long entityKey
              *             The key of an entity that is associated with this Message.
              * @param      const std::string& sessionId
              *             The session ID of the operator whose action initiated this
              *             Audit Message.
              */

            void sendMessage(EAuditMessageType messageType,
                             unsigned long entityKey,
                             const std::string& sessionId,
                             const std::string& optionalParameter1 = "",
                             const std::string& optionalParameter2 = "");

        private:
            VisualAuditMessageSender();
            virtual ~VisualAuditMessageSender();

            /**
              * getWorkstationForSession
              *
              * Returns whether the VideoInput is locked or not.
              *
              * @param      const std::string& sessionId
              *             The session ID to retrieve the workstation for.
              *
              * @return     std::string
              *             The workstation
              *
              */
            std::string getWorkstationForSession(const std::string& sessionId) const;
            //
            // The message sender.  This is an std::auto_ptrs so they will automatically be cleaned up.
            //

            std::auto_ptr< TA_Base_Core::AuditMessageSender > m_auditSender;

    }; // VisualAuditMessageSender

} // TA_IRS_App

#endif // VISUAL_AUDIT_MESSAGE_SENDER_H
