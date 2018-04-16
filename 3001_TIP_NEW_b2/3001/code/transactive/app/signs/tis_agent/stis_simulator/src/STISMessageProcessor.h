#ifndef STIS_MESSAGE_PROCESSOR_H
#define STIS_MESSAGE_PROCESSOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * STISMessageProcessor is a thread which holds a queue of incoming packets.  The main method of the 
  * class loops, checking the queue for packets to be processed.
  */

#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <iostream>

#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"


namespace TA_IRS_App
{
	class STISCommsObserver;

	class STISMessageProcessor : public virtual TA_Base_Core::Thread
	{

	public:

		//Construction & Destruction
		STISMessageProcessor( TA_IRS_App::STISCommsObserver* owner );
        
        STISMessageProcessor();   //default constructor, to be used for local instantiation

		virtual ~STISMessageProcessor();


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//								Thread methods								//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

		/**
		  * run
		  * 
		  * The run method must be implemented in any class that inherits from TA_Base_Core::Thread.
  		  *
		  */
		void run();


		/**
		  * terminate
		  * 
		  * The terminate method must be implemented in any class that inherits from TA_Base_Core::Thread.
		  *
		  */
		void terminate();



		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//						STISMessageProcessor methods						//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

		/**
		  * addMessageRequest
		  * 
		  * This method allows users to add requests to the queue.
		  *
		  */
		void addMessageRequest( std::vector< unsigned char >& packetData );

        /**
		  * isLocal
		  * 
		  * This method checks if the instance of the SwtichProcessor is local or 
          * attached to an ObservedSocket.
          *
          * @return     bool    isLocal
		  *
		  */
        bool isLocal();



		/**
		  * isValidPacket
		  * 
		  * This method validates the packet that has just been pulled from
		  * the packet queue. This ensures that the packet length field is correct,
		  * and that the appropriate packet checksum has been used.  This method
		  * does NOT check to see what type of packet it has.  This is a simple matter
		  * of looking at bytes 4-7 in the packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  * @return bool isValid
		  *			Returns true if the packet is valid, false otherwise.
		  *
		  */
		bool isValidPacket( std::vector< unsigned char >& packet );

		/**
		  * sequenceNumberToString
		  * 
		  * This method converts an integer
		  * to a string in the form XXXX eg 0023
		  *
		  * @param 
		  *
		  */
		std::string sequenceNumberToString( int sequenceNumber );
     
		std::string getNewTimestampString();

        /**
		  * processOCCStatusRequest
		  * 
		  * This method processes a validated HRTB packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processOCCStatusRequest(std::vector< unsigned char >& packet);

		void processCCTStatusRequest(std::vector< unsigned char>& packet);
    

	    /**
		  * processStationStatusRequest
		  * 
		  * This method processes a validated RQIO packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processStationStatusRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processPredefinedDisplayRequest
		  * 
		  * This method processes a validated RPIO packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processPredefinedDisplayRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processAdhocDisplayRequest
		  * 
		  * This method processes a validated RQSO packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processAdhocDisplayRequest(std::vector< unsigned char >& packet);
    

         /**
		  * processClearRequest
		  * 
		  * This method processes a validated RQSO packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processClearRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processControlDevice
		  * 
		  * This method processes a validated STDS packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processSTISUpgradePredefinedMessageLibraryRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processApprovedRATISDisplayRequest
		  * 
		  * This method processes a validated RQST packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processApprovedRATISDisplayRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processTrainInformationRequest
		  * 
		  * This method processes a validated RQDS packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processTrainInformationRequest(std::vector< unsigned char >& packet);
    

        /**
		  * sendReportDeviceState
		  * 
		  * This method processes a validated RPDS packet.
		  *
		  * @param std::string deviceId The device to be reported on
		  *
		  */
        void processConfigurationDataRequest(std::vector< unsigned char >& packet);
    

        /**
		  * processStoreCameraPreset
		  * 
		  * This method processes a validated RQSP packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processCurrentDisplayingMessageRequest(std::vector< unsigned char >& packet);
    
       /**
		  * processControlPIDRequest
		  * 
		  * This method processes a PID Control request message.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void processPIDControlRequest(std::vector< unsigned char >& packet);

        /**
		  * sendPacket
		  * 
		  * This method adds the SOH byte, calculates the Packet Sum and Check Sum, 
          * adds the ETB and sends the packet to either the socket, or to std::cout.
		  *
		  * @param std::vector< char > packet   The packet to be sent WITHOUT SOH, PKT_LEN, PKT_SUM or ETB.
		  *
		  */
        void sendPacket(std::vector< unsigned char >& packet);

		/**
		  * sendConfigurationDataReport
		  * 
		  * This method creates and sends an A00 message in response to
		  * an M00 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendConfigurationDataReport(std::vector< unsigned char >& command );

		/**
		  * sendDisplayReport
		  * 
		  * This method creates and sends an A10 message in response to either
		  * an M09, M10, M11, or M12 message. The packet that is passed
          * in is assumed to be correct (the process methods call sendAck() )
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendDisplayReport( std::vector< unsigned char >& command );

		/**
		  * sendClearReport
		  * 
		  * This method creates and sends an A20 message in response to
		  * an M20 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendClearReport(std::vector< unsigned char >& command );

		/**
		  * sendStationStatusReport
		  * 
		  * This method creates and sends an A30 message in response to
		  * an M30 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendStationStatusReport(std::vector< unsigned char >& command );

		/**
		  * sendOCCStatusReport
		  * 
		  * This method creates and sends an A31 message in response to
		  * an M31 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendOCCStatusReport(std::vector< unsigned char >& command);

		void sendCCTStatusReport(std::vector< unsigned char>& command);

		/**
		  * sendCurrentDisplayingMessageReport
		  * 
		  * This method creates and sends an A50 message in response to
		  * an M50 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendCurrentDisplayingMessageReport(std::vector< unsigned char >& command );

		/**
		  * sendCurrentDisplayingMessageReport
		  * 
		  * This method creates and sends an A70 message in response to
		  * an M70 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendUpgradePredefinedMessageLibraryReport(std::vector< unsigned char >& command );

		/**
		  * sendPIDControlReport
		  * 
		  * This method creates and sends an A21 message in response to
		  * an M21 request message
          *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendPIDControlReport(std::vector< unsigned char >& command );

       /**
		  * sendNack
		  * 
		  * This method processes a validated NACK packet.
		  *
		  * @param std::vector< unsigned char > packet
		  *			The packet to be validated.
		  *
		  */
        void sendNack(std::vector< unsigned char >& command, int reason );

		void updatePIDStatus( int pidAddress, int status );

		void updateResourceStatus( const std::string& resourceName, int status );

		// Updates the NEXT STIS OR TTIS library version
		void updateNextLibraryVersion( const std::string& libraryType, unsigned long version );

		// Updates the Current STIS library version
		void updateCurrentSTISLibraryVersion( unsigned long version );

		void updateCurrentTTISLibraryVersion( unsigned long version );

		// This function upgrades the CURRENT message library version (only applicable to STIS)
		void upgradeSTISLibrary( unsigned long version );

		void displayResourceStatus();

		void initialise( const std::string& location );

		void readLibraryVersions();

		void writeLibraryVersions();

    	void sendRATISPacketToStation( const std::string& command );

    	void sendRATISPacketToTrain( const std::string& command );

    	void sendRATISPacketToAllStations( const std::string& command );

    	void sendRATISPacketToAllTrains( const std::string& command );


		union PacketLength        // Declare a union that can hold the following:
		{
			unsigned char		cValue[2];  // two unsigned chars
			unsigned short		sValue;		// one unsigned short
		};

		union PacketLengthUnion
	    {
		    unsigned short us;
		    char uc[2];
	    };

	private:

		//TD 15686
		//zhou yuan++
		std::priority_queue< std::vector< unsigned char > > m_incomingMessages;	//switchRequests are received from the CommsObserver
																		//in the form of a vector of unsigned chars.  We have a
																		//queue of them.

		TA_Base_Core::ReEntrantThreadLockable m_incomingMessageLock;
		TA_Base_Core::Semaphore m_semaphone;
		//++zhou yuan

		volatile bool m_doThreadTerminate;
		volatile bool m_threadIsRunning;

		TA_IRS_App::STISCommsObserver* m_parent;                                        //The CommsObserver that owns this instance of the processor
        bool m_localProcessor;                                          //This is a flag that indicates if the switch processor is bound to an observed socket, or is a local instance

		// Only create one new timestamp at any point in time
		TA_Base_Core::ReEntrantThreadLockable m_timestampLock;

		std::string m_locationName;

		// Status for each PID
		std::map<int, int> m_pidStatusMap;

		int m_rxhLinkSummary;
		int m_connectionLinkSummary;
		int m_clockLinkSummary;
		int m_tisOccServerSummary;
		int m_ledDisplaySummary;
		int m_plasmaDisplaySummary;
		unsigned long m_newStisLibrary;
		unsigned long m_currentStisLibrary;

		std::fstream m_statusFile;
		std::string m_filename;

        std::string m_ratisCommand;
		bool m_isTimeScheduleDownloadComplete;

		// CCT added data
		bool m_isCCTsimulator;
		int m_cctComputerStatus; //0 normal, 1 alarm
		unsigned long m_newTtisLibrary;
		unsigned long m_currentTtisLibrary;
	};
}
#endif // !defined(AFX_SWITCHREQUESTPROCESSOR_H__683746F8_FFCF_4EC6_B348_B06AA6457BEA__INCLUDED_)
