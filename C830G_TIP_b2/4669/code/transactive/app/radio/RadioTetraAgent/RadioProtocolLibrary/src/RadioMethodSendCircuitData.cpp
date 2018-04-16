/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendCircuitData.cpp $
  * @author:  Abhinav Mathur
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendCircuitData method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodSendCircuitData.cpp
//  Implementation of the Class RadioMethodSendCircuitData
//  Created on:      27-Jan-2005 09:57:09 AM
///////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendCircuitData.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    const unsigned int RadioMethodSendCircuitData::SENDCIRCUITDATA_PACKET_SIZE                  = 18; // for One slot, 2.4 High Protection (ALL DATA MUST BE A MULTIPLE OF THIS)
const unsigned int RadioMethodSendCircuitData::SENDCIRCUITDATA_DATACALLREFERENCE_START_POS  = 0;
const unsigned int RadioMethodSendCircuitData::SENDCIRCUITDATA_FLUSH_START_POS              = 4;  // SENDCIRCUITDATA_DATACALLREFERENCE_START_POS+4;
const unsigned int RadioMethodSendCircuitData::SENDCIRCUITDATA_DATA_LENGTH_START_POS        = 5;  // SENDCIRCUITDATA_FLUSH_START_POS+1;
const unsigned int RadioMethodSendCircuitData::SENDCIRCUITDATA_DATA_START_POS               = 9;  // SENDCIRCUITDATA_DATA_START_POS+4;


RadioMethodSendCircuitData::RadioMethodSendCircuitData( RadioSessionReference sessionRef,
                                                            RadioCallReference callRef,
                                                            const std::vector<unsigned char>& data ) 
        : RadioMethodCallReference( SENDCIRCUITDATA_DATA_START_POS + data.size(),
                                METHOD_SENDCIRCUITDATA,
                                sessionRef,
                                callRef )
{
        TA_ASSERT( 0 == ( data.size() % SENDCIRCUITDATA_PACKET_SIZE ), "Data length must be a multiple of packet size");
    // DataCallReference set by RadioMethodCallReference constructor
    setParameterByte( SENDCIRCUITDATA_FLUSH_START_POS, 1 );
    // Data length set in ::setMessage()

        // set packet size
        setParameterLong( SENDCIRCUITDATA_DATA_LENGTH_START_POS, data.size() );

        // set data
	    for( unsigned int index = 0; index < data.size() ; ++index )
	    {
		    setParameterByte( index + SENDCIRCUITDATA_DATA_START_POS, data[index] );
	    }
    }


}

