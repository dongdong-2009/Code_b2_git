/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    //
    // AbstractCOERequestCommand
    //
    AbstractCOERequestCommand::AbstractCOERequestCommand()
        : m_data(), m_completionState( IAgentRequestCommand::COMMAND_LIFECYCLE_INCOMPLETE ), m_semaphore( 0 )
    {
        FUNCTION_ENTRY( "Constructor" );

        FUNCTION_EXIT;
    }


    //
    // ~AbstractCOERequestCommand
    //
    AbstractCOERequestCommand::~AbstractCOERequestCommand()
    {
        FUNCTION_ENTRY( "Destructor" );

        // Nothing to do

        FUNCTION_EXIT;
    }


    //
    // getDataToSend
    //
    std::string AbstractCOERequestCommand::getDataToSend()
    {
        FUNCTION_ENTRY( "getDataToSend" );

        return m_data;

        FUNCTION_EXIT;
    }


    //
    // waitForCompletion
    //
    void AbstractCOERequestCommand::waitForCompletion()
    {
        FUNCTION_ENTRY( "waitForCompletion" );

        m_semaphore.wait();

        FUNCTION_EXIT;
    }


    // 
    // getCompletionState
    //
    IAgentRequestCommand::ERequestCommandCompletionState AbstractCOERequestCommand::getCompletionState()
    {
        FUNCTION_ENTRY( "getCompletionState" );

        return m_completionState;

        FUNCTION_EXIT;
    }


    // 
    // signalResponse
    //
    void AbstractCOERequestCommand::signalResponse()
    {
        FUNCTION_ENTRY( "signalResponse" );

        TA_ASSERT( m_completionState != IAgentRequestCommand::COMMAND_LIFECYCLE_INCOMPLETE,
            "Command completion state was not set during processResponse!" );

        m_semaphore.post();

        FUNCTION_EXIT;
    }


    // 
    // signalCouldNotBeSent
    //
    void AbstractCOERequestCommand::signalCouldNotBeSent()
    {
        FUNCTION_ENTRY( "signalCouldNotBeSent" );

        m_completionState = COULD_NOT_BE_SENT;

        m_semaphore.post();

        FUNCTION_EXIT;
    }


    // 
    // signalTimedOut
    //
    void AbstractCOERequestCommand::signalTimedOut()
    {
        FUNCTION_ENTRY( "signalTimedOut" );

        m_completionState = TIMED_OUT;

        m_semaphore.post();

        FUNCTION_EXIT;
    }


    //
    // processGenericResponsePacketAttributes
    //
    bool AbstractCOERequestCommand::processGenericResponsePacketAttributes( const std::vector< unsigned char >& data, unsigned char command, unsigned int minAcceptableLength, unsigned int maxAcceptableLength )
    {
        FUNCTION_ENTRY( "processGenericResponsePacketAttributes" );

        //
        // ensure minimum size before checking other attributes.
        //
        if ( data.size() < ( TA_COE::NUM_LENGTH_BYTES + TA_COE::PACKET_HEADER_LENGTH ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received length is below minimum values, this will cause an ASSERT" );
			
			FUNCTION_EXIT;
            return false;
        }

        //
        // Validate length bytes against actual length received.
        //
        unsigned short packetLength = TA_COE::getPacketLength( data[ TA_COE::LSB_LENGTH_BYTE ], data[ TA_COE::MSB_LENGTH_BYTE ] );
        if ( data.size() != ( TA_COE::NUM_LENGTH_BYTES + packetLength ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received length does not match header length, this will cause an ASSERT" );
			
			FUNCTION_EXIT;
            return false;
        }

		// Log the packet
		unsigned char* pBuf=new unsigned char[data.size()*3+1];
		memset(pBuf,' ',data.size()*3+1);
		unsigned char* p =pBuf;
		for(int i=0;i<data.size();i++)
		{
			sprintf((char*)p,"%.2x.",(unsigned char)data[i]);	
			p+=3;
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Received reply:[%s]",pBuf);
		delete[] pBuf; 
        // Validate command byte
        if ( data[ TA_COE::COMMAND_BYTE ] != command )
        {
			unsigned char* pBuf=new unsigned char[data.size()*3+1];
			memset(pBuf,' ',data.size()*3+1);
			unsigned char* p =pBuf;
			for(int i=0;i<data.size();i++)
			{
				sprintf((char*)p,"%.2x.",(unsigned char)data[i]);	
				p+=3;
			}
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Command received does not match command pending.data:[%s],want command:[%x]",pBuf,command );
			delete[] pBuf; 
			return false;
        }

        //
        // Process the Error byte, validating length against acceptable range according to error code.
        //                   
        switch ( data[ TA_COE::ERROR_BYTE ] )
        {
        case TA_COE::OK:
            if ( ( packetLength >= minAcceptableLength ) 
                && ( packetLength <= maxAcceptableLength ) ) 
            {
                m_completionState = SUCCEEDED;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                return false;
            }
        case TA_COE::NOT_IMPLEMENTED:
            if ( packetLength <= maxAcceptableLength )
            {
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                return false;
            }                            
        case TA_COE::PROHIBITED:
            if ( packetLength <= maxAcceptableLength )
            {
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                return false;
            } 
        case TA_COE::BAD_LENGTH:
            if ( packetLength <= maxAcceptableLength )
            {
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                FUNCTION_EXIT;
                return false;
            } 
        case TA_COE::BAD_COMMAND:
            if ( packetLength <= maxAcceptableLength )
            {
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                FUNCTION_EXIT;
                return false;
            } 
        case TA_COE::BUSY:
            if ( packetLength <= maxAcceptableLength )
            {
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                FUNCTION_EXIT;
                return true;
            }
            else
            {
                FUNCTION_EXIT;
                return false;
            } 
        default:
            FUNCTION_EXIT;
            return false;
        }
    }
}
