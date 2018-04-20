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
  * Implementation of CRC16 to validate TIMS messages
  * NOTE. The internal code is exempt from normal coding standards,
  * it is provided as part of the interface specification so it shouldnt
  * be modified other than to make it fit into a class.
  */

#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    using TA_Base_Core::DebugUtil;


    // explicit initialize static member (builds the lookup table)
    FrameCheckSequence::FCSLookupTable FrameCheckSequence::s_lookupTable;


    unsigned short FrameCheckSequence::calculate( const TrainMessageData& buffer, unsigned int bufferLen )
    {
        FUNCTION_ENTRY( "calculate" );
        TA_ASSERT( bufferLen <= buffer.size(), "Invalid buffer length" );

        unsigned short fcs; // frame check sequence
        unsigned short q;   // calculation register

        fcs = 0xffff;   // fcs initialised with all ones
        TrainMessageData::const_iterator iter = buffer.begin();

        while ( bufferLen-- )
        {
            q = s_lookupTable.getLookup( static_cast<unsigned char>( *iter ^ ( fcs >> 8 ) ) );
            fcs = static_cast<unsigned short>( ( ( q & 0xff00 ) ^ ( fcs << 8 ) ) | ( q & 0x00ff ) );

            ++iter;
        }

        FUNCTION_EXIT;
        return static_cast<unsigned short>( fcs ^ 0xffff ); /* return the fcs ones complement */
    }


    FrameCheckSequence::FCSLookupTable::FCSLookupTable()
        : m_CRC16( 0xA001 )
    {
        FUNCTION_ENTRY ( "FCSLookupTable" );

        createLookupTable();

        FUNCTION_EXIT;
    }


    FrameCheckSequence::FCSLookupTable::~FCSLookupTable()
    {
        FUNCTION_ENTRY( "~FCSLookupTable" );
        FUNCTION_EXIT;
    }


    unsigned short FrameCheckSequence::FCSLookupTable::getLookup( unsigned char iPosition ) const
    {
        return m_lookupTable[iPosition];
    }


    void FrameCheckSequence::FCSLookupTable::createLookupTable()
    {
        FUNCTION_ENTRY( "createLookupTable" );

        unsigned short btab[8]; // table btab
        unsigned short i, j;    // loop parameters
        unsigned short q;       // calculation register
        unsigned short shreg;   // shift-register
        unsigned short carry;
        unsigned short bit;     // bit parameters

        //************************************************************
        //*    Calculate the table btab:
        //************************************************************
        carry = 1;  // carry flag set to one
        shreg = 0;  // shreg initialised with 0

        for ( i = 0; i < 8; ++i )
        {
            if ( carry ) shreg ^= m_CRC16;

            btab[i] = static_cast<unsigned short>( ( shreg << 8 ) | ( shreg >> 8 ) );    // swap bytes

            carry = static_cast<unsigned short>( shreg & 1 );

            shreg >>= 1;
        }

        //************************************************************
        //*    Calculate the lookup table
        //************************************************************
        for ( i = 0; i < 256; ++i )
        {
            q = 0;
            bit = 0x80;

            for ( j = 0; j < 8; ++j )
            {
                if ( bit & i ) q ^= btab[j];

                bit >>= 1;
            }

            m_lookupTable[i] = q;
        }

        FUNCTION_EXIT;
    }

}
