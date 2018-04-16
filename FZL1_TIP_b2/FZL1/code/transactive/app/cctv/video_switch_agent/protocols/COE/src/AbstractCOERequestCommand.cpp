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

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "core/utilities/src/TAAssert.h"
#include <boost/scoped_array.hpp>

namespace TA_IRS_App
{

    //
    // AbstractCOERequestCommand
    //
    AbstractCOERequestCommand::AbstractCOERequestCommand()
        : m_data(),
          m_completionState(IAgentRequestCommand::COMMAND_LIFECYCLE_INCOMPLETE),
          m_mutex(),
          m_condtion(m_mutex)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    //
    // ~AbstractCOERequestCommand
    //
    AbstractCOERequestCommand::~AbstractCOERequestCommand()
    {
        FUNCTION_ENTRY("Destructor");
        FUNCTION_EXIT;
    }


    //
    // getDataToSend
    //
    std::string AbstractCOERequestCommand::getDataToSend()
    {
        FUNCTION_ENTRY("getDataToSend");
        FUNCTION_EXIT;
        return m_data;
    }


    //
    // waitForCompletion
    //
    void AbstractCOERequestCommand::waitForCompletion()
    {
        FUNCTION_ENTRY("waitForCompletion");
        m_mutex.lock();
        while(m_completionState == IAgentRequestCommand::COMMAND_LIFECYCLE_INCOMPLETE)
        {
            m_condtion.wait();
        }
        FUNCTION_EXIT;
    }


    //
    // getCompletionState
    //
    IAgentRequestCommand::ERequestCommandCompletionState AbstractCOERequestCommand::getCompletionState()
    {
        FUNCTION_ENTRY("getCompletionState");
        FUNCTION_EXIT;
        return m_completionState;
    }


    //
    // signalResponse
    //
    void AbstractCOERequestCommand::signalResponse()
    {
        FUNCTION_ENTRY("signalResponse");
        m_mutex.lock();
        if(m_completionState == COMMAND_LIFECYCLE_INCOMPLETE)
        {
            m_completionState = RECEIVED_UNEXPECTED_PACKET;
        }
        m_condtion.signal();
        FUNCTION_EXIT;
    }


    //
    // signalCouldNotBeSent
    //
    void AbstractCOERequestCommand::signalCouldNotBeSent()
    {
        FUNCTION_ENTRY("signalCouldNotBeSent");
        m_mutex.lock();
        m_completionState = COULD_NOT_BE_SENT;
        m_condtion.signal();
        FUNCTION_EXIT;
    }


    //
    // signalTimedOut
    //
    void AbstractCOERequestCommand::signalTimedOut()
    {
        FUNCTION_ENTRY("signalTimedOut");
        m_mutex.lock();
        m_completionState = TIMED_OUT;
        m_condtion.signal();
        FUNCTION_EXIT;
    }



    std::string AbstractCOERequestCommand::getDataLogStr(const std::vector<unsigned char>& data)
    {
        boost::scoped_array<unsigned char> pBuf(new unsigned char[data.size() * 3 + 1]);
        memset(pBuf.get(), ' ', data.size() * 3 + 1);
        unsigned char* p = pBuf.get();
        for(int i = 0; i < data.size(); i++)
        {
            sprintf((char*)p, "%.2x.", (unsigned char)data[i]);
            p += 3;
        }
        return std::string((const char*)pBuf.get());
    }

    //
    // processGenericResponsePacketAttributes
    //
    bool AbstractCOERequestCommand::processGenericResponsePacketAttributes(const std::vector< unsigned char >& data,
                                                                           unsigned char command,
                                                                           unsigned int minAcceptableLength,
                                                                           unsigned int maxAcceptableLength)
    {
        FUNCTION_ENTRY("processGenericResponsePacketAttributes");
        bool validData = false;
        // ensure minimum size before checking other attributes.
        do
        {
			unsigned int totalPacketLenght = data.size();
            if(totalPacketLenght < TA_COE::MIN_VALID_RECEIVED_LENGTH)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received length is below minimum values");
                break;
            }
            // Validate length bytes against actual length received.
			if(totalPacketLenght != TA_COE::getPacketLength(data[ TA_COE::LSB_LENGTH_BYTE ], data[ TA_COE::MSB_LENGTH_BYTE ]) + TA_COE::NUM_LENGTH_BYTES)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received length does not match header length");
                break;
            }
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received reply:[%s]", getDataLogStr(data).c_str());
            // Validate command byte
            if(data[ TA_COE::COMMAND_BYTE ] != command)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received does not match command pending, command:[%x]", command);
                break;
            }
            //
            // Process the Error byte, validating length against acceptable range according to error code.
            //
            switch(data[ TA_COE::ERROR_BYTE ])
            {
                case TA_COE::OK:
                    if((totalPacketLenght >= minAcceptableLength)
                            && (totalPacketLenght <= maxAcceptableLength))
                    {
                        m_completionState = SUCCEEDED;
                        validData = true;
                    }
                    break;
                case TA_COE::NOT_IMPLEMENTED:
                case TA_COE::PROHIBITED:
                case TA_COE::BAD_LENGTH:
                case TA_COE::BAD_COMMAND:
                case TA_COE::BUSY:
                    if(totalPacketLenght <= maxAcceptableLength)
                    {
                        validData = true;
                        m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                    }
                    break;
                default:
                    break;
            }
        }
        while(0);
        return validData;
    }
}
