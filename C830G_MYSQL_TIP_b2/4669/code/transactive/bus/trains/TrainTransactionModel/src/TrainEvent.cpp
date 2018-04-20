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


#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>
#include <string>
#include <time.h>


namespace TA_IRS_Bus
{


    TrainEvent::TrainEvent( const TrainMessageData& data,
                            const std::string& sendingTsi,
                            const std::string& destinationTsi )
          : m_senderTsi( sendingTsi ),
            m_destinationTsi( destinationTsi ),
            m_data( data ),
            m_timestamp( 0 )
    {
        FUNCTION_ENTRY( "TrainEventWorkItem" );

        // initialises all members
        // sets the timestamp to the current time
        time( &m_timestamp );

        FUNCTION_EXIT;
    }


    TrainEvent::~TrainEvent()
    {
        FUNCTION_ENTRY( "~TrainEventWorkItem" );
        FUNCTION_EXIT;
    }


    std::string TrainEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // creates and returns a log string of the format:"[TrainID=][Command=][TSI=]"

        std::ostringstream logStream;
        logStream << "[TrainId=" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( getTrainId() )
        << "][Command=" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( getEventType() )
        << "][TSI=" << getTsi() << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    const std::string& TrainEvent::getTsi() const
    {
        FUNCTION_ENTRY( "getTsi" );
        // returns the Sender TSI
        FUNCTION_EXIT;
        return m_senderTsi;
    }


    const std::string& TrainEvent::getDestinationTsi() const
    {
        FUNCTION_ENTRY( "getDestinationTsi" );
        // returns the Destination TSI
        FUNCTION_EXIT;
        return m_destinationTsi;
    }


    time_t TrainEvent::getTime() const
    {
        FUNCTION_ENTRY( "getTime" );
        // returns the timestamp
        FUNCTION_EXIT;
        return m_timestamp;
    }


    const TrainMessageData& TrainEvent::getData() const
    {
        FUNCTION_ENTRY( "getData" );
        FUNCTION_EXIT;
        return m_data;
    }


    unsigned int TrainEvent::getDataLength() const
    {
        FUNCTION_ENTRY( "getDataLength" );
        FUNCTION_EXIT;
        return m_data.size();
    }
}
