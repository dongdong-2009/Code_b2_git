/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/Telegram.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * The top level telegram - it contains the VLS and SCADA headers only
  */

#if !defined(AFX_TELEGRAM_H__41CD7809_9CFC_400E_B7E3_84AD21D70292__INCLUDED_)
#define AFX_TELEGRAM_H__41CD7809_9CFC_400E_B7E3_84AD21D70292__INCLUDED_

#include <time.h>
#include <vector>
#include <map>

#include "core/types/src/ta_types.h"
#define MESSAGE_POLL_LENGTH           2
#define TRAIN_LOCATION_MSG_LENGTH     27 
#define BLOCK_INFO_MSG_LENGTH         11
#define DYNAMIC_TRAIN_DATA_MSG_LENGTH 22
#define SIGNAL_STATUS_MSG_LENGTH      33
#define TRAIN_PIS_INFO_MSG_LENGTH     82

typedef std::vector <unsigned char> MessageData;
typedef std::vector <MessageData> MessageContainer;

class Telegram  
{
	public:

		/**
		 * Possible error codes that can be contained within a telegram header
		 */
		enum EErrorCode
		{
			/**
			 * No error encountered
			 */
			NoError = 0,
			/**
			 * Unknown Interface Error
			 */
			UnknownInterface = 1,
			/**
			 * Interface Not Reachable Error
			 */
			InterfaceNotReachable = 2,
			/**
			 * Reserved for future use.
			 */
			OtherError = 3
		};

		enum EMultiFlag
		{
			Multiflag_Not_Set = 0,
			Multiflag_Set = 1
		};

		/**
		  * getMultiFlag
		  * 
		  * Gets the multiFlag from the packet.
		  * 
		  * @return unsigned char 
		  */
		unsigned char getMultiFlag() const;

		std::string getDataLog();

		/**
		 * Represents the interfaces this agent exchanges data for.
		 */
		enum EInterfaceType
		{
			

			//Lucky: Interface Type
			Message_Polling_Interface = 0,
			Train_Location_Interface = 1,
			Block_Information_Interface = 2,
			Dynamic_Train_Data_Interface = 3,
			Signal_Status_Interface = 4,
			Train_PIS_Info_Interface = 5,
			Planned_Data_Start_Interface = 6,
			Planned_Data_Content_Interface = 7,
			Planned_Data_End_Interface = 8,
			Practical_Data_Start_Interface = 9,
			Practical_Data_Content_Interface = 10,
			Practical_Data_End_Interface = 11,

			/**
			 * Interface number for the PTI system
			 */
			PtiInterface = 0,
			/**
			 * Interface number for third rail power status
			 */
			ThirdRailInterface = 1,
			/**
			 * Interface number for the radio system
			 */
			RadioInterface = 2,
			/**
			 * Interface number for the congestion
			 */
			CongestionInterface = 3,
			/**
			 * Interface number for the PIDS system.
			 */
			PidsInterface = 4,


		};

		/**
		 * This represents the type of telegram - a data or connect telegram
		 */
		enum ETelegramIdentification
		{
			/**
			 * Represents a Data Telegram
			 */
		//	DataTelegram = 0xFF9A,
			/**
			 * Represents a connect telegram.
			 */
			//ConnectTelegram = 0xFF98,

			/**
			 * Represents a Message Polling
			 */
			 MESSAGE_POLLING = 0x3001,

			 /**
			  * Represents a ATS_ISCS_TRAIN_LOCATION
			  */
			  ATS_ISCS_TRAIN_LOCATION = 0x1001,

			  /**
			  * Represents a ATS_ISCS_BLOCK_INFO
			  */
			  ATS_ISCS_BLOCK_INFO = 0x1002,

			  /**
			  * Represents a ATS_ISCS_DYNAMIC_TRAIN_DATA
			  */
			  ATS_ISCS_DYNAMIC_TRAIN_DATA = 0x1005,

			  /**
			  * Represents a ATS_ISCS_SIGNAL_STATUS
			  */
			  ATS_ISCS_SIGNAL_STATUS = 0x1006,

			  /**
			  * Represents a ATS_ISCS_TRAIN_PIS_INFO
			  */
			  ATS_ISCS_TRAIN_PIS_INFO = 0x1007,

			  /**
			  * Represents a ISCS_ATS_SCADA_INFO
			  */
			  ISCS_ATS_SCADA_INFO = 0x2001,

			  /**
			  * Represents a ATS_ISCS_PLANNED_TRAIN_DATA 
			  */
			  ISCS_PLANNED_TRAIN_DATA = 0x1003,

			  /**
			  * Represents a ATS_ISCS_PRACTICAL_TRAIN_DATA 
			  */
			  ISCS_PRACTICAL_TRAIN_DATA = 0x1004,

			  /**
			  * Represents a ATS_ATS ISCS ATS_ISCS_DATA_START
			  */
			  ISCS_DATA_START = 0x1101,

			  /**
			  * Represents a ATS ISCS ATS_ISCS_DATA_CONTENT
			  */
			  ISCS_DATA_CONTENT = 0x1102,

			  /**
			  * Represents a ATS ISCS ATS_ISCS_DATA_END
			  */
			  ISCS_DATA_END = 0x1103,
			  
			  /**
			   * Unknown Telegram
			   */
			   UNKNOWN_TELEGRAM = 0

		};

		enum EMessageType
		{
			/**
			 * Represents a spontaneous message (e.g. a request, or an event type message)
			 */
			SpontaneousMessage = 0,
			/**
			 * Represents an acknowledgment message - response to a spontaneous message.
			 */
			AckMessage = 1
		};

		/**
		  * Telegram
		  * 
		  * Incoming data that is read from the socket. 
		  * This should be the actual socket data and 
		  * the first two bytes should be the telegram length,
		  * i.e. the first byte in the vector should be the start of the telegram
		  *
		  * Create a telegram from incoming raw data.
		  * If the data constitutes a full packet, 
		  * the telegram object will be constructed and
		  * the telegram data will be removed from the incoming data vector.
		  * 
		  * If there is not a full packet, 
		  * an exception will be thrown and the data will be remain untouched
		  *
		  * If there is more than one packet, the Telegram object will be constructed,
		  * and the telegram data will be removed from the incoming data vector, 
		  * the remaining data should then be used to create a second Telegram object and so on
		  * 
		  * @param : std::vector<unsigned char>& incomingData
		  * @exception 
		  *     throws a Telegram Exception 
		  *     if the data is not as long as the header indicates it should be      
		  */
		Telegram(std::vector<unsigned char>& incomingData, bool bEnableExceptionMechanism=true);




		/**
		  * ~Telegram
		  * 
		  * Default Destructor
		  * 
		  * @return virtual 
		  */
		virtual ~Telegram();

		/**
		  * getLength
		  * 
		  * Gets the length of the internal message buffer. 
		  * This matches the length set in the header for incoming packets.
		  * For newly constructed (outgoing) packets it will be m_rawData.size() 
		  * and will be used to set the length in the header
		  * 
		  * @return ta_uint16 
		  */
		ta_uint16 getLength() const;
		ta_uint16 getMsgLength() const;

		/**
		  * getPacketData
		  * 
		  * Gets the packet data to write to the socket
		  * 
		  * @return std::vector<unsigned char> 
		  */
		std::vector<unsigned char> getPacketData();

		/**
		  * getIdentification
		  * 
		  * Gets the telegram type, data or connect.
		  * 
		  * @return ETelegramIdentification 
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		ETelegramIdentification getIdentification() const;

		/**
		  * getInterface
		  * 
		  * Gets the interface number from data.
		  * 
		  * @return EInterfaceType 
		  * 
		  * @exception throws TelegramException if the operations fails
		  */
		EInterfaceType getInterface() const;

		/**
		  * getMessageType
		  * 
		  * Gets the message type from the packet data
		  * 
		  * @return EMessageType 
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		EMessageType getMessageType() const;

		/**
		  * getErrorCode
		  * 
		  * Gets the error code set in the packet.
		  * 
		  * @return EErrorCode 
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		EErrorCode getErrorCode() const;

		/**
		  * setErrorCode
		  * 
		  * Sets the error flag in the header
		  * 
		  * @return void 
		  * @param : EErrorCode errorCode
		  */
		void setErrorCode(EErrorCode errorCode);

		/**
		  * getTimeStamp
		  * 
		  * Gets the receive time for incoming packets. 
		  * This is the time they came from the socket,
		  * not the time they were processed.
		  * 
		  * @return time_t 
		  */
		time_t getTimeStamp() const;

		/**
		  * getHeader
		  * 
		  * Gets the header of the incoming packets.
		  * 
		  * @return std::vector<unsigned char> 
		  */
		std::vector<unsigned char>& getHeader();

		/**
		  * getHeader
		  * 
		  * Gets the header of the incoming packets.
		  * 
		  * @return std::vector<unsigned char> 
		  */
		std::vector<unsigned char>& getConnHeader();

		/**
		 * isValidTeleram
		 */
		bool isValidTelegram();

		/**
		  * getWord
		  * 
		  * Gets the specified word from the buffer,
		  * uses the ntohs function to do so as the raw data is in network byte order.
		  * 
		  * @return ta_uint16
		  * @param : ta_uint16 position
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		ta_uint16 getWord(ta_uint16 position) const;

		/**
		 * Used to append telegram data
		 */
		void appendData( Telegram& telData );

		/**
		 * Returns the Message Data Content
		 */
		std::vector<unsigned char> getMessageContent();

	protected: 

		/**
		  * Telegram
		  * 
		  * Construct a new telegram, with details populated 
		  * - only more specific classes can create a new telegram.
		  * 
		  * @return 
		  * @param : ETelegramIdentification identification
		  * @param : EInterfaceType interfaceType
		  * @param : EMessageType messageType
		  * @param : EErrorCode errorCode
		  */
		Telegram(ETelegramIdentification identification, EInterfaceType interfaceType, EMessageType messageType, EErrorCode errorCode);

		/**
		 * Telegram
		 *
		 * Construct a new Telegram, with details populated
		 *
		 * @return
		 * @param : ta_unit16 msglength
		 * @param : ETelegramIdentification identification
		 */
		Telegram(ta_uint16 msgLength, ETelegramIdentification identification, ta_uint16 telegramSize);

		Telegram(ta_uint16 msgLength, ETelegramIdentification identification);
	    
		/**
		  * Copy Constructor
		  */    
		Telegram(const Telegram& rhs);

		/**
		  * setMessageType
		  * 
		  * Sets the message type - this is used when a packet is received.
		  * i.e. a request packet, exactly the same packet is sent back, 
		  * but it is changed to an acknowledgment rather than a spontaneous message.
		  * 
		  * @return void 
		  * @param : EMessageType
		  */
		void setMessageType(EMessageType messageType);

		/**
		  * setTelegramIdentification
		  * 
		  * sets the telegram identification field
		  * 
		  * @return void 
		  * @param : ETelegramIdentification identification
		  */
		void setTelegramIdentification(ETelegramIdentification identification);

		/**
		  * setMessageByte
		  * 
		  * Actually sets the internal buffer at the given position. 
		  * Position is relative to the start of the data. This just calls setByte,
		  * adding the header offset.
		  * 
		  * @return  void 
		  * @param : ta_uint16 position
		  * @param : unsigned char value
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setMessageByte(ta_uint16 position, unsigned char value);

		/**
		  * setMessageBytes
		  * 
		  * Sets a number of bytes in a message, 
		  * given a number of bytes to be written as bytes. 
		  * this just calls setBytes, adding the header offset.
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : const std::vector<unsigned char>& values
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setMessageBytes(ta_uint16 position, const std::vector<unsigned char>& values);

		/**
		  * setMessageWord
		  * 
		  * Given a value to be written as a word, set it in the message.
		  * Position is relative to the start of the data.
		  * This just calls setWord, adding the header offset.
		  * 
		  * @return void  
		  * @param : ta_uint16 position
		  * @param : ta_uint16 value
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setMessageWord(ta_uint16 position, ta_uint16 value);

		/**
		  * setMessageWords
		  * 
		  * Given a number of values to be written as words,
		  * sets them in the message. 
		  * This is like repeatedly calling setMessageWord for each element in the vector.
		  * This just calls setWords, adding the header offset.
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : const std::vector<ta_uint16>& values
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setMessageWords(ta_uint16 position, const std::vector<ta_uint16>& values);


		/**
		  * getMessageByte
		  * 
		  * Gets the given byte of the data. 
		  * This just calls getByte, 
		  * adding the header offset.
		  * 
		  * @return unsigned char  
		  * @param : ta_uint16 position
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		unsigned char getMessageByte(ta_uint16 position) const;

		/**
		  * getMessageBytes
		  * 
		  * Gets the given bytes of the data.
		  * This just calls getBytes, 
		  * adding the header offset.
		  * 
		  * @return std::vector<unsigned char>
		  * @param : ta_uint16 position
		  * @param : ta_uint16 length
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		std::vector<unsigned char> getMessageBytes(ta_uint16 position, ta_uint16 length) const;

		/**
		  * getMessageWord
		  * 
		  * gets the specified word from the buffer.
		  * this just calls getWord, 
		  * adding the header offset
		  * 
		  * @return ta_uint16
		  * @param : ta_uint16 position
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		ta_uint16 getMessageWord(ta_uint16 position) const;

		/**
		  * getMessageWords
		  * 
		  * Gets the specified number of words from the buffer.
		  * This just calls getWords, 
		  * adding the header offset
		  * 
		  * @return std::vector<ta_uint16>
		  * @param : ta_uint16 position
		  * @param : ta_uint16 length
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		std::vector<ta_uint16> getMessageWords(ta_uint16 position, ta_uint16 length) const;



		protected:

		/**
		  * setLength
		  * 
		  * Sets the length field in the packet header.
		  * This will be called by this class before returning the raw data 
		  * - this means the length is set last,
		  * after more specific classes have added their data.
		  * 
		  * @return void 
		  * @param : ta_uint16 length
		  */
		void setLength(ta_uint16 length);
		void setMsgLength (ta_uint16 length);
		/**
		  * setInterface
		  * 
		  * Sets the interface flag
		  * 
		  * @return void 
		  * @param : EInterfaceType interfaceType
		  */
		void setInterface(EInterfaceType interfaceType);

		/**
		  * setByte
		  * 
		  * Actually sets the internal buffer at the given position.
		  * Position is relative to the start of the buffer.
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : unsigned char value
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setByte(ta_uint16 position, unsigned char value);

		/**
		  * setBytes
		  * 
		  * Sets a number of bytes in a message,
		  * given a number of bytes to be written as bytes.
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : const std::vector<unsigned char>& values
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setBytes(ta_uint16 position, const std::vector<unsigned char>& values);

		/**
		  * setWord
		  * 
		  * Actually sets the internal buffer at the given position.
		  * Position is relative to the start of the buffer.
		  * This converts the word to network byte order, it uses the function ntohs 
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : ta_uint16 value
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setWord(ta_uint16 position, ta_uint16 value);

		/**
		  * setWords
		  * 
		  * Given a number of values to be written as words,
		  * sets them in the message.
		  * This converts the word to network byte order, it uses the function ntohs 
		  * 
		  * @return void 
		  * @param : ta_uint16 position
		  * @param : const std::vector<ta_uint16>& values
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		void setWords(ta_uint16 position, const std::vector<ta_uint16>& values);

		/**
		  * getByte
		  * 
		  * Gets the given byte of the data.
		  * 
		  * @return unsigned char 
		  * @param : ta_uint16 position
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		unsigned char getByte(ta_uint16 position) const;

		/**
		  * getBytes
		  * 
		  * Gets the given byte of the data.
		  * 
		  * @return std::vector<unsigned char>
		  * @param : ta_uint16 position
		  * @param : ta_uint16 length
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		std::vector<unsigned char> getBytes(ta_uint16 position, ta_uint16 length) const;

		ta_uint32 getDWord(ta_uint16 position) const;

		/**
		  * getWords
		  * 
		  * Gets the specified number of words from the buffer,
		  * uses the ntohs function to do so as the raw data is in network byte order.
		  * 
		  * @return std::vector<ta_uint16>
		  * @param : ta_uint16 position
		  * @param : ta_uint16 length
		  *
		  * @exception throws TelegramException if the operations fails
		  */
		std::vector<ta_uint16> getWords(ta_uint16 position, ta_uint16 length) const;

		ta_uint32 getWord32(ta_uint16 position) const;

		/**
		  * setTimeStamp
		  * 
		  * Set the m_timeStamp
		  */
		void setTimeStamp();

		//Disabled Methods
		/**
		  * Default Constructor
		  */
		Telegram();    

		/**
		  * Assignment Overloading
		  */
		Telegram& operator=(const Telegram&);

	
	private:
		static const ta_uint16 IDENTIFICATION_WORD_POSITION;   //The offset into the buffer of the length position
		static const ta_uint16 RESERVED_1_WORD_POSITION;       //The offset into the raw data for the first reserved word
		static const ta_uint16 RESERVED_2_WORD_POSITION;       //The offset into the raw data for the second reserved word
		static const ta_uint16 INTERFACE_NUMBER_BYTE_POSITION; //The position of the interface number byte
		static const ta_uint16 MESSAGE_TYPE_BYTE_POSITION;     //The position of the message type parameter
		static const ta_uint16 ERROR_CODE_BYTE_POSITION;       //The position of the error code field
		static const ta_uint16 RESERVED_3_BYTE_POSITION;

		/**
		  * The raw packet data. The only access to the raw data 
		  * for more specific classes is via the setMessage* methods.
		  */
		std::vector<unsigned char> m_rawData;   
		std::vector<unsigned char> m_header;
	    	
		/**
		  * This is a time stamp for incoming data 
		  *  -it will be set when the telegram is created from incoming data.
		  */
		time_t m_timeStamp;    


	protected:
		static const ta_uint16 MAX_PACKET_LENGTH;              //The maximum packet length

	public:
		static const ta_uint16 HEADER_LENGTH;                  //The header length, also the index into the complete packet of the application layer data
		static const ta_uint16 CONN_HEADER_LENGTH;             //The packet length for ConnectionConfirmation from RTC Server
		static const ta_uint16 LENGTH_WORD_POSITION;           //The offset into the packet for the position of the packet length
		static const ta_uint16 DWORD_SIZE;

		// HEADER Format for SIG Generic Data
		static const ta_uint8	HEADER_SYSTEM_ID;				// system id (Sender System Id)
		static const ta_uint16	HEADER_TOTAL_LEN;				// Total Length of the frame being send
		static const ta_uint8	HEADER_MULTI_FLAG;				// Flag that signifies if the frame that is to be send will be the last frame to send
		static const ta_uint16	SIG_HEADER_LEN;					// Total SIG Header Length
		static const ta_uint8	MSG_LENGTH_SIZE;				// the size of the msg length in bytes
		
		static const ta_uint16 LENGTH_MSG_POSITION;

		// SIG 
		static const ta_uint16 SIG_MAX_PACKET_LENTH;
		static const ta_uint8 SIG_HEADER_SIZE;
		static const ta_uint8 SIG_MULTIFLAG_POS;
		static const ta_uint8 SIG_MESSAGE_ID_POS;
		static const ta_uint8 SIG_MSG_LEN_POS;

	protected:
		
		ta_uint16 getSigHeaderLength ();
		void setSigHeader (ta_uint16 msgLength);
		void setSigSystemId ();
		void setSigTotalLen (ta_uint16 msgLength);
		void setSigMultiFlag (ta_uint16 msgLength);

	public:

		MessageContainer getAllMessages ();
		Telegram::ETelegramIdentification getIdentification(std::vector <unsigned char> & vctrData) const;	
		ETelegramIdentification getSIGIdentification() const;
		bool hasError ();
	private:
		MessageContainer m_messageContainer;
		void collectMessages ();
		bool validateMessage (const MessageData & messageData);

		ta_uint16 getWord(ta_uint16 position, MessageData & msgData) const;
		void setHasError (bool bValue) const;
		bool m_bHasError;

	
};		

#endif // !defined(AFX_TELEGRAM_H__41CD7809_9CFC_400E_B7E3_84AD21D70292__INCLUDED_)
