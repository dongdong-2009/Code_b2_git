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
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/FrameCheckSequence.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    const unsigned short TtdFrame::MAX_FRAME_SIZE = 240;
    const unsigned char TtdFrame::MESSAGE_DESTINATION_VMC = 0;
	const unsigned char TtdFrame::MESSAGE_DESTINATION_TIMS = 0;
    const unsigned short TtdFrame::MSB_MASK = 0x100;
    const unsigned char TtdFrame::MESSAGE_FLAG = 0x07;
	long TtdFrame::m_messageType = 0;

	// Dennis Abriol
	// modification
	// change datatype to msgtype for TTIS LIb protocol
	
    TtisTypes::TtisFrameList TtdFrame::frameData( const TtisTypes::LibraryDataType& rawData,
                                                  CommonTypes::TrainIdType trainId,
                                                  EMessageType msgType,
                                                  unsigned char origin )
    {
        FUNCTION_ENTRY( "frameData" );

        // go through each byte of the data creating ttd frame objects, and adding data to them
        //
        // once all frames are created, set the total number of frames in each, and finalise the data
        //
        // store the data in the return vector and return it

        FrameList frameList;
        TtdFrame* currentFrame = NULL;
		
		m_messageType = msgType;

        TtisTypes::LibraryDataType::const_iterator currentByte = rawData.begin();

        while ( currentByte != rawData.end() )
        {
            // Create the next frame
            // the frame number is the next element in the vector
            currentFrame = new TtdFrame( frameList.size() + 1, trainId, msgType, origin );

            TA_ASSERT( NULL != currentFrame, "Unable to create frame for train data." );

            // add the data
            // this will accept all the data it can, or it will run out of data trying
            // it will return the iterator to the next byte (which may be the end)
            currentByte = currentFrame->addTransferData( currentByte, rawData.end() );

            // add the frame to the vector
            frameList.push_back( currentFrame );
        }

        TtisTypes::TtisFrameList framedData;

        // Update the total frames for all frames
        // and since the message is now complete, each one can be checksummed and byte stuffed

        for ( FrameList::iterator frameIter = frameList.begin();
              frameIter != frameList.end(); ++frameIter )
        {
            ( *frameIter )->finaliseFrame( frameList.size() );

            // now get the data from the frame, and store it
            framedData.push_back(  ( *frameIter )->getFrameData() );

            // now its safe to delete this frame because its no longer necessary
            delete ( *frameIter );
			*frameIter = NULL;
        }

        FUNCTION_EXIT;
        return framedData;
    }


    TtdFrame::TtdFrame( unsigned short frameNumber,
                        CommonTypes::TrainIdType trainId,
                        EMessageType msgType,
                        unsigned char origin )
        : m_frameComplete( false ),
          m_lengthPosition( 0 )
    {
        FUNCTION_ENTRY( "TtdFrame" );

        // add directly, no stuffing
        m_headerData.push_back( MESSAGE_FLAG );

        addByte( m_headerData, msgType );		// FLAG
        addByte( m_headerData, trainId );		// MSGID

        // store the field position of the length byte so it can be updated later
        m_lengthPosition = m_headerData.size();

        // initial length, minimun frame size
        addByte( m_headerData, 10 );

        addByte( m_headerData, origin );
        addByte( m_headerData, MESSAGE_DESTINATION_TIMS);
		// this means cancel has been encountered
		if (m_messageType == DownloadCtrl)
		{
			FUNCTION_EXIT;
			return;
		}	
        // add the frame number
        addByte( m_headerData, frameNumber % MSB_MASK ); // LSB of frame number
        addByte( m_headerData, frameNumber / MSB_MASK ); // MSB of frame number

        // total frames will be added later

        // data will be added later

        // CRC will be added later

        FUNCTION_EXIT;
    }


    TtdFrame::~TtdFrame()
    {
        FUNCTION_ENTRY( "~TtdFrame" );
        FUNCTION_EXIT;
    }


    TtisTypes::LibraryDataType::const_iterator TtdFrame::addTransferData( const TtisTypes::LibraryDataType::const_iterator& currentByte,
                                                                          const TtisTypes::LibraryDataType::const_iterator& end )
    {

        //This will go through the data, byte stuffing and adding to the payload as it goes.
        //It stops when there is no more data, or no more data will fit.
        //
        //It will return the next iterator in the data

        FUNCTION_ENTRY( "addTransferData" );

        TA_ASSERT( false == m_frameComplete, "Frame has been finalised, cannot add more data" );

        // the number of bytes that will fit are the max size, minus the header and the CRC
        // the header has been built, except for the total frames - we must assume this needs to be byte stuffed for now (so 4 bytes)
        // the CRC is 2 bytes
        unsigned short bytesLeft = MAX_FRAME_SIZE - ( m_headerData.size() + 6 );

        TtisTypes::LibraryDataType::const_iterator currentPosition = currentByte;

        m_payload.clear();
        m_payload.reserve( bytesLeft );

        // while data can be added, add each byte

        while ( currentPosition != end )
        {
            unsigned short bytesNeeded = 1;

            if ( ( *currentPosition == 0x06 ) ||
                 ( *currentPosition == 0x07 ) )
            {
                bytesNeeded = 2;
            }

            // if there is enough space for this byte, add it
            if ( bytesNeeded <= bytesLeft )
            {
                // this will add the byte, padding if necessary
                addByte( m_payload, *currentPosition );

                // the last byte was added, continue to the next
                ++currentPosition;

                // decrement from the allowable number of bytes
                bytesLeft -= bytesNeeded;
            }
            else
            {
                // no more data will fit, exit the loop
                break;
            }
        }

        FUNCTION_EXIT;
        return currentPosition;
    }


    void TtdFrame::finaliseFrame( unsigned int totalFrames )
    {
        FUNCTION_ENTRY( "finaliseFrame " );

        TA_ASSERT( false == m_frameComplete, "Frame has been finalised, cannot add more data" );

        // for efficiency, reserve max size
        m_frameData.reserve( MAX_FRAME_SIZE );
		
		// this means cancel has been encountered
		if (m_messageType != DownloadCtrl)
		{
			// step one, add the final frame count to the header
			addByte( m_headerData, totalFrames % MSB_MASK ); // LSB of frame number
			addByte( m_headerData, totalFrames / MSB_MASK ); // MSB of frame number
		}	

        // step two, we now know the total data length, set it
        // header + payload + CRC
		long lPos = m_headerData.size() + m_payload.size() + 2;

        m_headerData[m_lengthPosition] = m_headerData.size() + m_payload.size() + 2;

        // step three, assemble the frame (minus the CRC)
        // add the header to the final frame
        m_frameData.insert( m_frameData.end(), m_headerData.begin(), m_headerData.end() );
        // add the payload to the final frame
        m_frameData.insert( m_frameData.end(), m_payload.begin(), m_payload.end() );

        // step four, calculate and set the CRC
        unsigned short CRC = FrameCheckSequence::calculate( m_frameData, m_frameData.size() );
        m_frameData.push_back( CRC % MSB_MASK );  // LSB of sum check
        m_frameData.push_back( CRC / MSB_MASK );  // MSB of sum check

        // the frame is ready
        m_frameComplete = true;

        FUNCTION_EXIT;
    }


    TtisTypes::TtisFrameData TtdFrame::getFrameData()
    {
        FUNCTION_ENTRY( "getFrameData" );

        TA_ASSERT( true == m_frameComplete, "Frame has not been finalised, cannot get data" );
        
        TtisTypes::TtisFrameData data;
        data.frameData = m_frameData;
        return data;

        FUNCTION_EXIT;
    }


    void TtdFrame::addByte( TtisTypes::LibraryDataType& frameData, unsigned char dataByte )
    {
        //Adds the byte to the data, applying byte stuffing if it is needed
        FUNCTION_ENTRY( "addByte" );
		
		// Dennis Abriol
		// Remove conditions that check
		// for 6 and 7, so far, found out
		// no relevance on TTIS Download

        frameData.push_back( dataByte ); 

        FUNCTION_EXIT;
    }

}
