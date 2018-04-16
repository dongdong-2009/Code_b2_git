/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType ChangeoverStatusEvent::MESSAGE_TYPE = 65;
    const unsigned short ChangeoverStatusEvent::EXPECTED_MESSAGE_LENGTH = 9;
    const unsigned short ChangeoverStatusEvent::STATUS_POSITION = 5;
    const unsigned short ChangeoverStatusEvent::MODE_POSITION = 6;
    const unsigned char ChangeoverStatusEvent::CHANGEOVER_FOR_SOME_REASON = 0;
	const unsigned char ChangeoverStatusEvent::CHANGEOVER_DUE_TO_ACSU_FAILURE = 1;


    ChangeoverStatusEvent::ChangeoverStatusEvent( const TrainMessageData& data,
                                                  const std::string& sendingTsi,
                                                  const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "ChangeoverStatusEvent" );

        // get the MPU mode
        m_mpuMode = static_cast<EMpuMode>( getByte( MODE_POSITION ) );

		// get the status
        unsigned char status = getByte( STATUS_POSITION );
		
		m_changeOverStatus = status;

        // validate the MPU mode
        switch ( m_mpuMode )
        {
            // valid values

            case MpuStandby:
                // fall through

            case MpuMaster:
                break;

            // invalid values
            default:
                std::ostringstream invalidMPU;
                invalidMPU << static_cast<int>( m_mpuMode );
                TA_THROW( ProtocolException( "Invalid MPU mode received (Status 15)", "MpuMode (Status 15)", invalidMPU.str() ) );
				// fall through
        }

        // status
        if ( status != CHANGEOVER_DUE_TO_ACSU_FAILURE && 
			 status != CHANGEOVER_FOR_SOME_REASON)
        {
            std::ostringstream  invalidstatus;
            invalidstatus << static_cast<int>( status );

            TA_THROW( ProtocolException( "Invalid status received (Status 8)", "Status (Status 8)", invalidstatus.str() ) );
        }

        FUNCTION_EXIT;
    }


    ChangeoverStatusEvent::~ChangeoverStatusEvent()
    {
        FUNCTION_ENTRY( "~ChangeoverStatusEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType ChangeoverStatusEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string ChangeoverStatusEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        //append: "[MpuMode=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[MpuMode=" << static_cast<int>( m_mpuMode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    ChangeoverStatusEvent::EMpuMode ChangeoverStatusEvent::getMpuMode() const
    {
        FUNCTION_ENTRY( "getMpuMode" );
        FUNCTION_EXIT;
        return m_mpuMode;
    }


    unsigned short ChangeoverStatusEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

	unsigned char ChangeoverStatusEvent::getStatus() const
	{
		FUNCTION_ENTRY( "getStatus" );
		FUNCTION_EXIT;
		return m_changeOverStatus;
	}

}//end of  namespace
