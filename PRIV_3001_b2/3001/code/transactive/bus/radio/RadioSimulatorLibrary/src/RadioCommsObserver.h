/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioCommsObserver.h $
 * @author:  Ross Mckillop
 * @version: $Revision: #1 $
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * CommsObserver holds a member observed socket, and acts as an interface for objects
 * to access that socket.
 */

#if !defined(RadioCommsObserver_H)
#define RadioCommsObserver_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/radio/RadioSimulatorLibrary/src/SimulatorUseCase.h"
#include "bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h"

#include "core/defaults/src/DefaultEnvironment.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/threads/src/IWorkItem.h"

#include <vector>

namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}

namespace TA_IRS_Bus
{

    /**
     * These represent incoming radiomethods to be processed
     */
    typedef std::vector< unsigned char > IncomingPacket;
    typedef boost::shared_ptr< IncomingPacket > IncomingPacketPtr;

    
    /**
     * This is an interface called by the queued radio method tasks
     * to process the incoming data
     */
    class IRadioMethodProcessor
    {
    public:
        virtual void processRadioMethod( IncomingPacketPtr incomingMethod ) = 0;
    };

    
    /**
     * Processed and sends data
     */
    class RadioCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver,
                               public ISimulatorSender,
                               public IRadioMethodProcessor
    {
        public:

            /**
             * Constructor - requires a connected client socket
             */
            RadioCommsObserver( TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* socket );
            

            /**
             * Destructor
             */
            virtual ~RadioCommsObserver();
            
            
            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                            ITcpObserver methods                          //
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
             *
             */
            void processReceivedData( std::vector<unsigned char>& pData, TA_Base_Core::ISocket * pSocket );


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
             *            and returns false if the socket should no longer be observed.
             *
             */
            bool connectionLost();


            /**
             * connectionEstablished
             *
             * This method must be implemented to inherit ITcpSocketObserver. it is called after a
             * connection is successfully established.
             *
             */
            void connectionEstablished();


            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                      IRadioMethodProcessor methods                       //
            //                                                                          //
            //////////////////////////////////////////////////////////////////////////////


            /**
             * Processed a complete radio method
             */
            virtual void processRadioMethod( IncomingPacketPtr incomingMethod );


            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                            CommsObserver methods                         //
            //                                                                          //
            //////////////////////////////////////////////////////////////////////////////

            
            /**
             * getSocket
             *
             * returns a pointer to the socket being observed.
             *
             * @return TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket> socket the observed socket.
             */
            TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* getSocket() { return m_clientSocket; }


            /**
             * stillConnected
             *
             * indicates if the observed socket is still connected to it's remote host.
             *
             * @return bool true if the socket is still connected, false otherwise.
             *
             */
            bool stillConnected() { return m_stillConnected; }

            
            SimulatorUseCaseList& simulatorUseCasesRef();

            
            void simulatorRunUseCase( const std::vector<unsigned char>& data,
                                      const SimulatorUseCase& uc );
           
            
            void addSessionReference( unsigned long ref );
            void deleteSessionReference( unsigned long ref );
            void updateSessionReference( unsigned long ref, const std::string& status );

            std::map<unsigned long,std::string> getSessionReferences() { return m_sessionReferences; };

        private:

            RadioCommsObserver();


            //*****************
            // Message Events *
            //*****************
            void attachSession( unsigned long transactionId );

            void initialiseSession( unsigned long transactionId,
                                    unsigned long sessionId );

            void newReference( unsigned long transactionId,
                               unsigned long sessionId,
                               char refType );
            
            void setupCall( unsigned long transactionId,
                            unsigned long sessionId,
                            unsigned short refType,
                            unsigned long callRef );
            
            void disconnectCall( unsigned long transactionId,
                                 unsigned long sessionId,
                                 unsigned long callRef );
            
            void sendSDS( unsigned long transactionId,
                          unsigned long sessionId,
                          unsigned long sdsRef );
            
            void queryReference( unsigned long transactionId,
                                 unsigned long sessionId,
                                 unsigned long reference,
                                 unsigned long key );
            
            void searchSubscribers( unsigned long transactionId,
                                    unsigned long sessionId,
                                    unsigned char view,
                                    const std::string& criteria );
            
            void getGroupDetails( unsigned long transactionId,
                                  unsigned long sessionId,
                                  unsigned long reference,
                                  unsigned long rowsPerEvent,
                                  const std::string& fields );
            
            void deleteReference( unsigned long transactionId,
                                  unsigned long sessionId,
                                  const char refType[],
                                  unsigned long reference );
            
            void text2SubRef( unsigned long transactionId,
                              unsigned long sessionId,
                              std::string textReference );
            
            void changeReference( unsigned long transactionId,
                                  unsigned long sessionId,
                                  unsigned long reference,
                                  const std::string& details );

            //*******************
            // Helper functions *
            //*******************
            
            std::vector< IncomingPacketPtr > extractPacketsFromRawData( std::vector<unsigned char>& pData );

            unsigned long getMessageLong( const std::vector< unsigned char >& buffer,
                                          unsigned int iPosition ) const;

            unsigned short getMessageShort( const std::vector< unsigned char >& buffer,
                                            unsigned int iPosition ) const;
            
            std::string getMessageLongString( const std::vector< unsigned char >& buffer,
                                              unsigned int iPosition ) const;
            
            std::string getMessageShortString( const std::vector< unsigned char >& buffer,
                                               unsigned int iPosition ) const;

            void setMessageLong( char * buffer,
                                 unsigned long value ) const;

            void setMessageShort( char * buffer,
                                  unsigned short value ) const;
            
            void sendEvent( const char * event,
                            unsigned long eventLength );

            void simulatorSendEvent( const char * buf,
                                     const unsigned int length );

        private:

            // socket for incoming/outgoing data
            TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* m_clientSocket;

            //indicates if the observed socket is still connected.    
            volatile bool m_stillConnected;

            std::string m_unconditionalForwardNumber;
            std::string m_notReachableForwardNumber;

            SimulatorUseCaseList m_simulatorUseCases;

            std::map<unsigned long,std::string> m_sessionReferences;

            TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;
    };


    /**
     * Task to process incoming events simultaneously
     */
    class RadioMethodTask : public TA_Base_Core::IWorkItem
    {
    public:

        RadioMethodTask( IRadioMethodProcessor& callback,
                         IncomingPacketPtr data );

        
        virtual ~RadioMethodTask();


        virtual void executeWorkItem();

    private:

        RadioMethodTask();

        IRadioMethodProcessor& m_callback;
        IncomingPacketPtr m_data;

    };

}//close namespace

#endif // !defined(RadioCommsObserver_H)

