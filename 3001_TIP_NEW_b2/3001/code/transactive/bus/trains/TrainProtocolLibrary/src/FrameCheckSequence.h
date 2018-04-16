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
#if !defined(FRAMECHECKSEQUENCE_H)
#define FRAMECHECKSEQUENCE_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Calculates CRC of given data. Code provided by Alstom and slightly modified.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:08 PM
     */

    class FrameCheckSequence
    {

    public:

        
        /**
         * Calculates the CRC for the given data
         *
         * @param buffer    The data to CRC
         * @param bufferLen    How many bytes of the data to CRC
         */
        static unsigned short calculate( const TrainMessageData& buffer, unsigned int bufferLen );


    private:

        /**
         * FrameCheckSequence is not meant to be instantiated
         */
        FrameCheckSequence();
        virtual ~FrameCheckSequence();


        /**
         * Lookup table for calculating CRC
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:09 PM
         */

        class FCSLookupTable
        {

        public:


            /**
             * Constructor
             * Initialises the lookup table
             */
            FCSLookupTable();


            /**
             * Destructor
             */
            ~FCSLookupTable();


            /**
             * Gets the entry at the given position in the lookup table
             *
             * @param iPosition The index in the table
             *
             * @return the lookup value
             */
            unsigned short getLookup( unsigned char iPosition ) const;


        private:

            /**
             * Private copy operations
             */
            FCSLookupTable( const FCSLookupTable& );
            FCSLookupTable& operator=( const FCSLookupTable& );


            /**
             * Initialises the lookup table
             */
            void createLookupTable();


            /**
             * Part of the CRC16 algorithm
             */
            const unsigned short m_CRC16;


            /**
             * The actual table
             */
            unsigned short m_lookupTable[256];

        };

        static FCSLookupTable s_lookupTable;

    };

}

#endif // !defined(FrameCheckSequence_H)
