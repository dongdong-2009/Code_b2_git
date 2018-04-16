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


#include "bus/trains/TrainProtocolLibrary/src/AbstractTableEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const unsigned short AbstractTableEvent::EXPECTED_MESSAGE_LENGTH = 31;
    const unsigned short AbstractTableEvent::DATA_LENGTH = 24;
    const unsigned short AbstractTableEvent::DATA_POSITION = 5;


    AbstractTableEvent::AbstractTableEvent( const TrainMessageData& data,
                                            const std::string& sendingTsi,
                                            const std::string& destinationTsi )
        : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "AbstractTableEvent" );

        m_tableData = getBytes( DATA_POSITION, DATA_LENGTH );

        FUNCTION_EXIT;
    }


    AbstractTableEvent::~AbstractTableEvent()
    {
        FUNCTION_ENTRY( "~AbstractTableEvent" );
        FUNCTION_EXIT;
    }


    const TrainMessageData& AbstractTableEvent::getTableData() const
    {
        FUNCTION_ENTRY( "getTableData" );
        FUNCTION_EXIT;
        return m_tableData;
    }


    unsigned short AbstractTableEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}

