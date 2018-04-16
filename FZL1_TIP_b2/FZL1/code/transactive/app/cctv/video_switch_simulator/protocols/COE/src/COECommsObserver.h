#if !defined(AFX_COECOMMSOBSERVER_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_)
#define AFX_COECOMMSOBSERVER_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COECommsObserver.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * CommsObserver manages COE server communications for particual host:port.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <vector>
#include <map>

#include "app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h"

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"


namespace TA_IRS_App
{
    class COECommsObserver : public virtual TA_Base_Core::ITcpSocketObserver
    {
        public:
            typedef void ( *MessageProcessorFunction )( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver  );
            
            //Construction & Destruction
            
            //we have no need for a default constructor.
            COECommsObserver(const std::string& simulatorName, const std::string& location, bool isActive, TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket);
            COECommsObserver();
            
            virtual ~COECommsObserver();

            /** 
              * registerCommandWithRangeAndProcessorFunction
              *
              * description
              *
              * @param command
              * @param minPacketLength
              * @param minPacketLength
              * @param processorFunction - Pointer to static function that processes COE Packet of type <command>.
              */
            void registerCommandWithRangeAndProcessorFunction( unsigned char command, 
                unsigned short minPacketLength, unsigned short maxPacketLength, MessageProcessorFunction processorFunction );

            
            /** 
              * setSwitchStatus
              *
              * description
              *
              * @param 
              */
            void setSwitchStatus( bool isActive );

            
            /** 
              * sendPacket
              *
              * description
              *
              * @param 
              */
            void sendPacket( const std::vector< unsigned char >& message );


            /** 
              * isActive
              *
              * description
              *
              * @return 
              */
            bool isActive() { return m_isActive; }


            /** 
              * sendMonitorState
              *
              * description
              *
              * @param monitorID
              * @param 
              */
            void sendMonitorState( unsigned char monitorID, const COEEquipmentState::MonitorState& state );


            /** 
              * sendSequenceState
              *
              * description
              *
              * @param monitorID
              * @param 
              */
            void sendSequenceState( unsigned char sequenceID, const COEEquipmentState::SequenceState& state );


            /** 
              * sendQuadState
              *
              * description
              *
              * @param monitorID
              * @param 
              */
            void sendQuadState( unsigned char quadID, const COEEquipmentState::QuadState& state );


            /** 
              * sendRecordingUnitState
              *
              * description
              *
              * @param recordingUnitID
              * @param 
              */
            void sendRecordingUnitState( unsigned char recordingUnitID, COEEquipmentState::ERecordingUnitState state );


            /** 
              * sendTriggeringEvents
              *
              * description
              *
              * @param TrigerringEventList
              * @param 
              */
            void sendTriggeringEvents( TriggeringEventList& events );
			
			/** 
              * processSelectCamAndMon
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processSelectCamAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processSelectSeqAndMon
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processSelectSeqAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processSelectQuadAndMon
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processSelectQuadAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processSelectBVSStageAndMon
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processSelectBVSStageAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processAckTrainAlarm
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processAckTrainAlarm( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processClearTrainAlarm
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              */
            static void processClearTrainAlarm( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processClearMonitor
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processClearMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processSetRecUnit
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processSetRecUnit( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processSetQuad
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processSetQuad( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processProgSeq
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processProgSeq( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processControlSeq
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processControlSeq( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetMonStat
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetMonStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetSeqStat
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetSeqStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetRecUnitStat
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetRecUnitStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetQuadStat
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetQuadStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetSummaryStat
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetSummaryStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * processRetTriggeringEvents
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processRetTriggeringEvents( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );
   
			/** 
              * processAckTriggeringEvent
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processAckTriggeringEvent( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );
   
			/** 
              * processClearTriggeringEvent
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              */
            static void processClearTriggeringEvent( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );
   
			/** 
              * printValidPacketReceived
              *
              * description
              *
              * @param messageBegin
              * @param messageEnd
              * @param 
              *
              */
            static void printValidPacketReceived( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver );


            /** 
              * getLocation
              *
              * description
              *
              * @return 
              */
            inline std::string getLocation() { return m_location; }


            /** 
              * getSimulatorName
              *
              * description
              *
              * @return 
              */
            inline std::string getSimulatorName() { return m_simulatorName; }


           /** 
             * short getUnsignedShortFromMessageData
             *
             * description
             *
             * @param mostSigByte
             * @param leastSigByte
             *
             * @return The corresponding unsigned short value.
             */
            inline static unsigned short getUnsignedShortFromMessageData( unsigned char mostSigByte, unsigned char leastSigByte )
            { return ( ( mostSigByte * 0x0100 ) + leastSigByte ); }

            
            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                          ITcpObserver methods                            //
            //                                                                          //
            //////////////////////////////////////////////////////////////////////////////


            /**
              * processReceivedData
              * 
              * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
              * packet is received on the specified socket, this method is called.
              *
              * @param std::vector<unsigned char>& pData the data contained in the received packet
              *
              * @param TA_Base_Core::ISocket* pSocket a pointer to the socket the packet was received on
              */
            void processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket );


            /**
              * writeFailed
              * 
              * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
              * request to write to the observed socket fails, this method is called.  It allows
              * the user to include any cleanup they require after a failed write.
              *
              */
            void writeFailed();


            /**
              * connectionLost
              * 
              * This method must be implemented to inherit ITcpSocketObserver.  This method is called
              * whenever the observed socket loses it's connection to the remote socket.
              *
              * @return bool continueToObserve returns true if the socket should continue to be observed
              *         and returns false if the socket should no longer be observed.
              *
              */
            bool connectionLost();


            /**
              * connectionEstablished
              *
              * This method must be implemented to inherit ITcpSocketObserver. it is called after a
              * connection is successfully established.
              */
            void connectionEstablished();


            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                          CommsObserver methods                           //
            //                                                                          //
            //////////////////////////////////////////////////////////////////////////////

            
            /**
              * getSocket
              *
              * returns a pointer to the socket being observed.
              *
              * @return TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket> socket the observed socket.
              */
            TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>   * getSocket() { return m_clientSocket; }


            /**
              * stillConnected
              *
              * indicates if the observed socket is still connected to it's remote host.
              *
              * @return bool true if the socket is still connected, false otherwise.
              */
            bool stillConnected() { return m_stillConnected; }


        private:

            typedef enum
            {
                LSB_LENGTH_BYTE = 0,
                MSB_LENGTH_BYTE = 1,
                ERROR_BYTE      = 2,
                COMMAND_BYTE    = 3
            }  EPacketHeaderFormat;
            enum { NUM_LENGTH_BYTES = 2 };
            
            typedef std::pair< unsigned short, unsigned short > PacketLengthRange;
            typedef std::pair< PacketLengthRange, MessageProcessorFunction > PacketRangeAndProcessor;
            typedef std::map< unsigned char, PacketRangeAndProcessor > PacketCommandToRangeAndProcessor;
  
            
            /** 
              * validateRequestPacketHeader
              *
              * description
              */
            void validateRequestPacketHeader();


            /** 
              * setUpPacketCommandToRangeAndProcessor
              *
              * description
              */
            void setUpCommandToRangeAndProcessorMappings();


            /** 
              * processCurrentPacket
              *
              * description
              *
              */
            void processCurrentPacket();

        private:
            std::string m_simulatorName;

            std::string m_location;

            bool m_isActive;

            TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* m_clientSocket;  // the observed socket

            volatile bool m_stillConnected;                                         // indicates if the observed socket
                                                                            // is still connected.
            std::vector<unsigned char> m_messageBuffer;

            unsigned short m_currentPacketLength;

            PacketCommandToRangeAndProcessor m_packetCommandToRangeAndProcessor;

            
};

}//close namespace
#endif // !defined(AFX_COECOMMSOBSERVER_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_)

