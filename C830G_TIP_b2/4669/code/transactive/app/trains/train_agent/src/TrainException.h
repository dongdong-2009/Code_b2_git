/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainException.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * All Train exceptions.
  *
  */

///////////////////////////////////////////////////////////
//  TrainInvalidParameterException.h
//  Implementation of the Class TrainInvalidParameterException
//  Created on:      28-Nov-2003 04:41:02 PM
///////////////////////////////////////////////////////////

#if !defined(TrainException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_)
#define TrainException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_App
{
    /**
     * Base class for all train exceptions.
     *
     * @todo Add operator<< to facilitate output of message
     */
	class TrainException : public TA_Base_Core::TransactiveException 
	{
		public:
            TrainException() throw() : TA_Base_Core::TransactiveException("") {};
            TrainException(const std::string& what) throw() : TA_Base_Core::TransactiveException(what) {};
            virtual ~TrainException() throw() {};
// TransactiveException provides a suitable method...
//            inline std::string details() const throw() { return m_what; };
//        private:
//            const std::string m_what;
	};

    /**
     * This class is used to indicate that a train was referenced with an
	 * invalid Train ID.
     */
    class TrainInvalidTrainException : public TrainException
	{
		public:
            TrainInvalidTrainException() throw() {};
            TrainInvalidTrainException(const std::string& what) throw() : TrainException(what) {};
            virtual ~TrainInvalidTrainException() throw() {};
	};

    /**
     * This class is used to indicate that an invalid parameter was detected when 
     * attempting to send a message to the train.
     */
	class TrainInvalidParameterException : public TrainException
	{
		public:
            TrainInvalidParameterException() throw() {};
            TrainInvalidParameterException(const std::string& what) throw() : TrainException(what) {};
            virtual ~TrainInvalidParameterException() throw() {};
	};

    /**
     * This class is used to indicate that a train selection list was
	 * referenced with an invalid name.
     */
    class TrainInvalidSelectionListException : public TrainException
	{
		public:
            TrainInvalidSelectionListException() throw() {};
            TrainInvalidSelectionListException(const std::string& what) throw() : TrainException(what) {};
            virtual ~TrainInvalidSelectionListException() throw() {};
	};

    /**
     * This class is used to indicate that a timeout occurred when attempting to send
     * a message to the train.  It will mean that the reply was not received within
     * the timeout period.
     */
    class TrainTimeoutException : public TrainException
	{
		public:
            TrainTimeoutException() throw() {};
            TrainTimeoutException(const std::string& what) throw() : TrainException(what) {};
            virtual ~TrainTimeoutException() throw() {};
	};

    /**
     * This class is used to indicate that a timeout occurred when attempting to send
     * a message to the train.  It will mean that the reply was not received within
     * the timeout period.
     */
    class TrainMessageTimeoutException : public TrainTimeoutException
	{
		public:
            TrainMessageTimeoutException() throw() {};
            TrainMessageTimeoutException(const std::string& what) throw() : TrainTimeoutException(what) {};
            virtual ~TrainMessageTimeoutException() throw() {};
	};

     /**
     * This class is used to indicate that a timeout occurred when attempting to send
     * a message to the train.  It will mean that the reply was not received within
     * the timeout period.
     */
    class TrainTimerTimeoutException : public TrainTimeoutException
	{
		public:
            TrainTimerTimeoutException() throw() {};
            TrainTimerTimeoutException(const std::string& what) throw() : TrainTimeoutException(what) {};
            virtual ~TrainTimerTimeoutException() throw() {};
	};

    /**
     * This class is used to indicate that the train is busy with another request.
     */
	class TrainBusyException : public TrainException
	{
		public:
            TrainBusyException(bool override) throw() : m_override(override) {};
            TrainBusyException(const std::string& what, bool override) throw() : TrainException(what), m_override(override) {};
            virtual ~TrainBusyException() throw() {};
            inline bool override() const throw() { return m_override; };
        private:
            TrainBusyException();
            const bool m_override;
	};

    /**
     * This class is used to indicate the train command has failed due to an 
     * unexplained event, unhandled status, or error in the current command sequence.
     * Could also be caused by a (66) Bad Command send from TIMS.
     * @todo Consider extending GeneralExc to say TrainVersionDiffException and so on...
     */
	class TrainGeneralFailureException : public TrainException
	{
		public:
            enum EFailureReason
            {
                UNSPECIFIED,
                BAD_COMMAND,
                RESET,
                COMMAND_FAILED,
                DIFF_VERSION,
				PROTOCOL_ERROR /// Error in message data received from TIMS via Tetra
            };
            TrainGeneralFailureException() throw() : m_reason(UNSPECIFIED) {};
            TrainGeneralFailureException(const std::string& what, enum EFailureReason reason = UNSPECIFIED) throw() : TrainException(what), m_reason(reason) {};
            virtual ~TrainGeneralFailureException() throw() {};
            inline EFailureReason reason() const throw() { return m_reason; };
        private:
            const EFailureReason m_reason;
	};

	/** 
     * This class is used to indicate that a protocol error has occured within the TrainProtocolLibrary
	 */
	class TrainProtocolException : public TrainGeneralFailureException
	{
	public:
		typedef enum TrainGeneralFailureException::EFailureReason EFailureReason;
		TrainProtocolException() throw() : TrainGeneralFailureException("", /*EFailureReason::*/ PROTOCOL_ERROR) {};
		TrainProtocolException(const std::string& what) throw() : TrainGeneralFailureException(what, /*EFailureReason::*/ PROTOCOL_ERROR) {};
		virtual ~TrainProtocolException() throw() {};
	};


	/**
	 * Occurs when an Access Controlled Action is violated.
	 */
	class TrainAccessDeniedException : public TrainException
	{
	public:
		TrainAccessDeniedException() throw() {};
		TrainAccessDeniedException(const std::string& what) throw() : TrainException(what) {};
		virtual ~TrainAccessDeniedException() throw() {};
	};

    // ss++
    // TD13952
    /**
     * It means that radio agent replies to a request for cyclic announcement with a first cycle canceled message
     */
    class TrainPaFirstCyclicException : public TrainException
	{
	public:
		TrainPaFirstCyclicException() throw() {};
		TrainPaFirstCyclicException(const std::string& what) throw() : TrainException(what) {};
		virtual ~TrainPaFirstCyclicException() throw() {};
	};
    // ++ss
    
}
#endif // !defined(TrainException_F59E6C28_F7B1_47d5_8347_7F2E01F0547D__INCLUDED_)
