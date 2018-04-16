/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/TelegramException.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Thrown by accessor methods when values are not correct - 
  * this indicates an invalid packet
  */

#if !defined(AFX_TELEGRAMEXCEPTION_H__72CE7E1D_9652_4E3C_8C52_D0D33F27EA1B__INCLUDED_)
#define AFX_TELEGRAMEXCEPTION_H__72CE7E1D_9652_4E3C_8C52_D0D33F27EA1B__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"


class TelegramException: public TA_Base_Core::TransactiveException  
{
public:
    
	enum EErrorType
	{
		/**
		 * An unknown error
		 */
		UnknownError = 0,
		/**
		 * The requested field is not defined
		 */
		FieldNotDefined = 1,
		/**
		 * The requested field has an invalid value
		 */
		InvalidValue = 2,
		/**
		 * This indicates that the packet length is not the length specified in the header
		 */
		IncompletePacket = 3,
		/**
		 * This indicates that the packet SystemID is not a valid ID
		 */
		 InvalidSystemID = 4,
		/**
		 * This indicates that the packet Header size is incorrect
		 */
		 InvalidHeaderSize = 5
	};

	/**
	 * Create a telegram exception with an error code and description (the base class
	 * gets constructed with the description)
	 * @param errorType    The error causing the exception to be raised
	 * @param description    An optional description of the error
	 * @param fieldName    Optionally specify the field name that caused the error to
	 * be thrown
	 * 
	 */
    TelegramException(EErrorType errorType, const std::string& description = "", const std::string& fieldName = "") throw();

	/**
	  * ~TelegramException
	  * 
	  * Default Destructor
	  * 
	  * @return virtual 
	  */
	virtual ~TelegramException() throw();

    /**
      * Overrides the TransactiveException implementation. 
      * Formats the message depending on the information presents.
      *
      * Action: Builds the message in the following format
      * 
      * TransactiveException.what() + ErrorType + m_errorType + Caused by field + m_fieldName
      *
      * m_errorType will be printed as a string representation, 
      * not the enumeration integer value.
      * If the caused by field is not set, then the "Caused by field" text will not be given
      */
    virtual const char* what() const throw();

    /**
      * get the error type causing this exception
      */
    EErrorType getError() const;

private:

    //Disabled Method
    /**
      * Default Constructor
      */
	TelegramException();

    /**
      * Assignment Overloading
      */
    TelegramException& operator=(const TelegramException&);    


private:

	EErrorType  m_errorType;    //The error type initially unknown
    std::string m_fieldName;    //The name of the field that caused the exception
    std::string m_msg;
};

#endif // !defined(AFX_TELEGRAMEXCEPTION_H__72CE7E1D_9652_4E3C_8C52_D0D33F27EA1B__INCLUDED_)



