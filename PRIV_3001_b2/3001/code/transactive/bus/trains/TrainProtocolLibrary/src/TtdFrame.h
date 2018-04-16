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
#if !defined(TTDFRAME_H)
#define TTDFRAME_H

#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"


namespace TA_IRS_Bus
{
    /**
     * Represents a frame of data to be sent to the train via a circuit mode transfer.
     *
     * Also provides utilities for framing the data.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:32 PM
     */

    class TtdFrame
    {

    public:

        /**
         * What does the data represent
         * @author adamr
         * @version 1.0
         * @created 05-Feb-2008 3:20:33 PM
         */
        enum EDownloadDataType
        {
            LibraryData = 46,
            ScheduleData = 47
        };


        /**
         * Given input data, apply the framing and return the framed data.
         *
         * @return the data in framed format
         *
         * @param rawData    The raw data to be framed
         * @param trainId    The train this data is for
         * @param dataType    The type of data file
         * @param origin    The ISCS origin (same as for the TIMS message)
         */
        static TtisTypes::TtisFrameList frameData( const TtisTypes::LibraryDataType& rawData,
                                                   CommonTypes::TrainIdType trainId,
                                                   EDownloadDataType dataType,
                                                   unsigned char origin );


        /**
         * Destructor
         */
        virtual ~TtdFrame();


    private:

        /**
         * @author adamr
         * @version 1.0
         * @created 05-Feb-2008 3:20:33 PM
         */
        typedef std::vector< TtdFrame* > FrameList;


        /**
         * This will create the internal buffer used for the frame data and setup the header in it. It will initialise member variables.
         *
         *
         * @param frameNumber    The frame number in the sequence of frames
         * @param trainId    The train the frame is destined for
         * @param dataType    The download type
         * @param origin    the origin, same as the tims messages
         */
        TtdFrame( unsigned short frameNumber,
                  CommonTypes::TrainIdType trainId,
                  EDownloadDataType dataType,
                  unsigned char origin );


        /**
         * Unused Constructor
         */
        TtdFrame();


        /**
         * Add the given data to the frame data
         * This will continue to add as many bytes as will fit in the frame until the data runs out or the frame is full.
         *
         * Adding bytes to an already full frame will result in assert
         *
         * @return the reference to the next byte in the data (for the next frame) or the end of the data if there is no more data left
         *
         * @return true if there was enough space and the byte was added, false if this byte needs to go into a new frame
         *
         * @param currentByte    This is a reference to the next byte to be added to the frame
         * @param end    This is the end of the data, data will be added until this is hit
         */
        TtisTypes::LibraryDataType::const_iterator addTransferData( const TtisTypes::LibraryDataType::const_iterator& currentByte,
                                                                    const TtisTypes::LibraryDataType::const_iterator& end );


        /**
         * This will set the total frames, and apply the checksum/byte stuffing.
         * Once this is done, the frame is considered complete.
         *
         * Calling this after the frame is complete will result in an assert.
         *
         * @param totalFrames
         */
        void finaliseFrame( unsigned int totalFrames );


        /**
         * This gets the finished frame data, calling this before the frame is built will cause an assert.
         */
        TtisTypes::TtisFrameData getFrameData();


        /**
         * Adds (and stuffs) a byte to the given data vector.
         *
         * @param frameData    The data to add to
         * @param dataByte    The data to add
         */
        void addByte( TtisTypes::LibraryDataType& frameData, unsigned char dataByte );


        /**
         * Maximum size of a single frame
         */
        static const unsigned short MAX_FRAME_SIZE;


        /**
         * Fixed message destination
         */
        static const unsigned char MESSAGE_DESTINATION_VMC;


        /**
         * Used for a bitmask
         */
        static const unsigned short MSB_MASK;


        /**
         * Fixed message flag
         */
        static const unsigned char MESSAGE_FLAG;


        /**
         * Flag to indicate whether the frame building is complete
         */
        bool m_frameComplete;


        /**
         * The length field can move due to byte stuffing, this is its index
         */
        unsigned short m_lengthPosition;


        /**
         * This is the message data - header part only
         */
        TtisTypes::LibraryDataType m_headerData;


        /**
         * This is the message data field.
         */
        TtisTypes::LibraryDataType m_payload;


        /**
         * This is the message data (populated once finalised and assembled)
         */
        TtisTypes::LibraryDataType m_frameData;

    };

}

#endif // !defined(TtdFrame_H)
