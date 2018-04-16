/**
* This class encapulates the data associated with a ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:40:15
*/
#ifndef ATS_ITCP_METHOD_H
#define ATS_ITCP_METHOD_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodProtocol.h"

namespace TA_IRS_App
{
	class AtsITcpMethod
	{
	public:
	   /**
	    * Constructor
		* @param: trainIDNumber, criticalAlarmEvent
		*/
		AtsITcpMethod(DataGramVector criticalAlarmEvent); 
		virtual ~AtsITcpMethod();
	
	public:
		/** 
         * Used to set the data for the FCTLO
		 * F = Flag, C = Command, T = Train ID Number, L = Length, O = Origin
		 */
		 void setDataFCTLO(int loc,DataGramType data);
		
		/**
		 * Used to set the data for the CriticalAlarmEvents
		 */
		 void setDataCriticalAlarmEvents(DataGramVector criticalAlarmEvent);

		/**
         * Used to calculate the CRC using the FrameCheckSequence.
		 */
		 void calculateCRC();

		 /**
		  * Used to merge data of FCTLO and CriticalAlarmEvents.
		  */
		  void dataGramMerge();
		
		/**
		 * getMessageDataFromFCTLO
		 * Returned the FCTLO data so it can be accessed by the
		 * the TCP/IP socket for transmission.
		 */
		 const DataGramVector getMessageDataFromFCTLO()const;

		/**
		 * getMessageDataCriticalAlarmEvent
		 * Returned the Critical Alamrm/Event data so it can be accessed by the
		 * the TCP/IP socket for transmission.
		 */
		 const DataGramVector getMessageDataCriticalAlarmEvent()const;
		
		/** 
		 * getMessageDataCRC
		 * Returned the Critical Alamrm/Event data so it can be accessed by the
		 * the TCP/IP socket for transmission.
		 */
		 const DataGramVector getMessageDatagramTable() const;

	    /** 
		 * getMessageDataCRC
		 * Returned the Critical Alamrm/Event data so it can be accessed by the
		 * the TCP/IP socket for transmission.
		 */
		 const DataGramType* getMessageDataCRC() const;
		
		/**
		 * Returned the length of the CriticalAlarmEvents
		 */
		 unsigned short getLengthCriticalAlarmEvents();

		 /**
		  * Returned the length of the FCTLO
		  * F = Flag, C = Command, T = Train ID Number, L = Length, O = Origin
		  */
		 unsigned short getLengthFCTLO();
 
		 ///**
		 // * Set the data for CRC.
		 // */
		 //void setDataCRC(const unsigned char*);
		 
		 
		 /**
          * Get the message data in printable format
          */
         void getLogText(std::string& hexText, std::string& consoleText) const;
		
		 /**
		  * Returned the Flag
		  */
		 const unsigned char getFlag() const;
		 /**
		  * Returned the Command
		  */
		 const unsigned char getCommand()const;
		 /**
		  * Returned train Id
		  */
		 const unsigned char getTrainId()const;
		 /**
		  * Returned the Length
		  */
		 const unsigned char getLength()const;
		 /**
		  * Returned the Origin
		  */
		 const unsigned char getOrigin()const;
		 /**
		  * Returned the Critcal Alarm
		  */
		 void getCriticalAlarm(std::string& criticalAlarm)const;



	private:

		DataGram m_dataGram;
	
	private:

		/**
		 * Disable the assignment and copy constructors
		 */
	AtsITcpMethod& operator=(const AtsITcpMethod &);
			AtsITcpMethod( const AtsITcpMethod& itcpMethod);
	};
};

#endif // end of ATS_ITCP_METHOD_H