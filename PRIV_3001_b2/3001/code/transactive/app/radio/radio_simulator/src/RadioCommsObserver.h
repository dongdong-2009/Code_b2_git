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
#endif // #ifdef WIN32

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "app/radio/radio_simulator/src/CMHelper.h"
#include "app/radio/radio_simulator/src/CMTypes.h"
#include "app/radio/radio_simulator/src/RadioIdentity.h"
#include "app/radio/radio_simulator/src/CMData.h"


namespace TA_IRS_App
{

    /**
      * CommsObserver holds a member observed socket, and acts as an interface for objects
      * to access that socket.
      */
    class RadioCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver
    {
    public:

        //Construction & Destruction
        
        //we have no need for a default constructor.
        RadioCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket);
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
        void processReceivedData( std::vector<unsigned  char> & pData, TA_Base_Core::ISocket * pSocket );

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

		// For Registration Request 
		bool parseRegistrationRequest(const std::vector<unsigned char>& pData, RadioIdentity* id);
		CMRegistrationStatus processIdentity(RadioIdentity* id);
		void buildRegisterResponse(const std::vector<unsigned char>& pData, CMRegistrationStatus status);

		// For CM Broadcast 
		bool parseCMBroadcast(const std::vector<unsigned char>& pData);

		// For CM Data
		bool parseCMData(const std::vector<unsigned char>& pData);
		void buildTLReport();
		void buildCMReport();
		void buildISCSResponse(const std::vector<unsigned char>& pData);
		void buildPAReadyResponse(const std::vector<unsigned char>& pData);
		void buildPAMessageCompleteResponse(const std::vector<unsigned char>& pData);
		void buildPecReadyResponse(const std::vector<unsigned char>& pData);
		void buildPecResetResponse(const std::vector<unsigned char>& pData);
        void buildLiveRequestResetResponse(const std::vector<unsigned char>& pData);
        void buildLivePaReset(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp);
        void buildPreRecordedPaTimeOutResponse(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp);

		// Sending response/report
		void sendData(const std::vector<unsigned char>& pData);

		// Display user data
		void displayData(const std::vector<unsigned char>& pData);


		void processData(std::vector <unsigned char> &pData);

        // Execute methods
		bool execPassnumber_check(bool bEnable);
		static bool execAddISSI(const std::string& cmdValue);
		static bool execListISSIPassNum();
		bool execDeliveryStatus(const std::string& cmdValue);
		bool execSetCmdata(const std::string& cmdValue);
		bool execSendCmdata();
		void execShowMessage(const std::string& params);

		void setCMHeader(std::string header);
		void setTLHeader(std::string header);
		std::string getCMHeader();
		std::string getTLHeader();
		static const int USER_DATA_INDEX;
		void printData(const std::vector<unsigned char>& rawMessage, bool isReceived, bool display = false);

		void setTtsiLibVer(const std::string& ver);
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

		void processIncomeSDS ( std::vector<unsigned char> & pData ) ;

		std::string m_ttisLibVer;
		
        // Request processor to handle incoming packets.    
        //PATableRequestProcessor * m_requestProcessor;
    };

}//close namespace
#endif // ifndef(SDH_COMMS_OBSERVER_H)

