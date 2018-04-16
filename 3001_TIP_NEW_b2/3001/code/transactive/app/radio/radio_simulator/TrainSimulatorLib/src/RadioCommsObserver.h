#ifndef SDH_COMMS_OBSERVER_H
#define SDH_COMMS_OBSERVER_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioCommsObserver.h $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4267)
#pragma warning (disable : 4018)
#pragma warning (disable : 4996)
#pragma warning (disable : 4333)
#endif // #ifdef WIN32

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"


#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/RadioIdentity.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMData.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/IProtocolHelper.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/ISDSMessageObserver.h"
#include "app/radio/radio_simulator/TrainSimulator/IModelLogger.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/threads/src/IWorkItem.h"
#include<vector>

namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
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
      * CommsObserver holds a member observed socket, and acts as an interface for objects
      * to access that socket.
      */
    class RadioCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver,
							   public IRadioCommsObserver,
							   public IRadioMethodProcessor
    {
    public:

        //Construction & Destruction
        
        //we have no need for a default constructor.
        RadioCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket,
						   IModelLogger* modelLogger);
        RadioCommsObserver();
        
        virtual ~RadioCommsObserver();
        
        
        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                            ITcpObserver methods                            //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////


        /**
          * processReceivedData
          * 
          * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
          * packet is received on the specified socket, this method is called.
          *
          * @param pData the data contained in the received packet
          *
          * @param TA_Core::ISocket* pSocket a pointer to the socket the packet was received on
          *
          */
        void processReceivedData( std::vector<unsigned  char>&  pData, TA_Base_Core::ISocket * pSocket );

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
        //                                                                            //
        //                            CommsObserver methods                            //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

        
        /**
          * getSocket
          *
          * returns a pointer to the socket being observed.
          *
          * @return TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> socket the observed socket.
          *
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
	

		///
		// Implementation specific
		///

		// accessors:
		void setPassNumberCheck(bool bEnable) { m_passNumberCheck = bEnable; }
		bool getPassNumberCheck() { return m_passNumberCheck; }
		void setCMDeliveryStatus(CMDeliveryStatus deliveryStatus) { m_cmDeliveryStatus = deliveryStatus; }
		CMDeliveryStatus getCMDeliveryStatus() { return m_cmDeliveryStatus; }
	
		// Sending response/report
		void sendData( const std::vector<unsigned char>& pData);
	
		// Display user data
		void displayData(const std::vector<unsigned char>& pData);


		void processData(std::vector <unsigned char> &pData);

        // Execute methods
		bool execPassnumber_check(bool bEnable);
		/*static*/bool execAddISSI(const std::string& cmdValue);
		/*static*/ bool execListISSIPassNum();
		bool execDeliveryStatus(const std::string& cmdValue);
		bool execSetCmdata(const std::string& cmdValue);
		bool execSendCmdata();
		void execShowMessage(const std::string& params);

		void setCMHeader(std::string header);
		void setTLHeader(std::string header);
		std::string getCMHeader();
		std::string getTLHeader();
		static const int USER_DATA_INDEX;
		void printData(const std::vector<unsigned char>& rawMessage, bool isReceived = false, bool display = false);

		void setTtsiLibVer(const std::string& ver);

		void registerProtocolHelper( IProtocolHelper* protocolHelper);

		/**
         * Process a complete radio method
         */
         void processRadioMethod( IncomingPacketPtr incomingMethod );

		 std::vector< IncomingPacketPtr > extractPacketsFromRawData( std::vector<unsigned char>& pData );

		 std::vector< IncomingPacketPtr > getReceivedPackets(std::vector<unsigned char>& pData, unsigned long copyLength );

		 typedef std::vector<std::string> ClientName;

    private:

        // the observed socket
        TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* m_clientSocket;

        // indicates if the observed socket is still connected.    
        bool m_stillConnected;
		bool m_passNumberCheck;
		CMDeliveryStatus m_cmDeliveryStatus;
		CMData m_cmData;
		std::string m_userData;
		std::string m_cmHeader;
		std::string m_tlLength;
		std::string m_tlHeader;

		bool showMsg;

		std::string m_ttisLibVer;
		
        // Request processor to handle incoming packets.    
        //PATableRequestProcessor * m_requestProcessor;

		IProtocolHelper*  m_protocolHelper;
		IModelLogger* m_modelLogger;

		TA_Base_Core::ReEntrantThreadLockable m_dataLock;

		TA_Base_Core::ReEntrantThreadLockable m_receiveDataLock;

		TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;

		ClientName m_clientName;

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
#endif // ifndef(SDH_COMMS_OBSERVER_H)

