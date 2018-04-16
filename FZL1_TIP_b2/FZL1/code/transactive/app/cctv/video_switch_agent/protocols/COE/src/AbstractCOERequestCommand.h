#ifndef ABSTRACT_COE_REQUEST_COMMAND_H
#define ABSTRACT_COE_REQUEST_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  *
  */

#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include <ace/Condition_Thread_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <boost/noncopyable.hpp>
#include "COEHelper.h"

namespace TA_IRS_App
{
    class AbstractCOERequestCommand : public virtual IAgentRequestCommand, boost::noncopyable
    {
        public:
            AbstractCOERequestCommand();
            virtual ~AbstractCOERequestCommand();
            virtual std::string getDataToSend();
            virtual bool isResponseExpected()
            {
                return true;
            }
            virtual void waitForCompletion();
            virtual IAgentRequestCommand::ERequestCommandCompletionState getCompletionState();
            virtual void signalResponse();
            virtual void signalCouldNotBeSent();
            virtual void signalTimedOut();
        protected:

            std::string getDataLogStr(const std::vector<unsigned char>& data);
            bool processGenericResponsePacketAttributes(const std::vector< unsigned char >& data,
                                                        unsigned char command,
														unsigned int minAcceptableLength = TA_COE::PACKET_HEADER_LENGTH + TA_COE::NUM_LENGTH_BYTES,
                                                        unsigned int maxAcceptableLength = TA_COE::PACKET_HEADER_LENGTH + TA_COE::NUM_LENGTH_BYTES);

            std::string m_data;
            IAgentRequestCommand::ERequestCommandCompletionState m_completionState;
        private:
            ACE_Thread_Mutex m_mutex;
            ACE_Condition_Thread_Mutex m_condtion;
    };
}

#endif // ABSTRACT_COE_REQUEST_COMMAND_H
