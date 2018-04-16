/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/ItaUserMessage/public_interfaces/ItaUserMessage.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface allows the outsourcer to use the TransActive user message Library.
  * 
  *
  */

#if !defined(ITA_USER_MESSAGE_H)
#define ITA_USER_MESSAGE_H

#include <string>
#include <afx.h>

namespace TA_Base_Bus
{

    class ItaUserMessage
    {
    public:
        /** 
          * constructMessage
          *
          * Constructs a formatted string using the loaded string resource identifier
          * and the pre-input arguments.
          *
          * @param uiMessageID Resource identifier of the string to display
          *
          * @return The formatted string.
          */
	    virtual CString constructMessage(UINT uiMessageID) = 0;


        /** 
          * showMsgBox
          *
          * Displays a message box with text and appearance loaded from the input string resource
          * identifier. Returns the result of ::MessageBox, 0 upon failure.
          *
          * @param uiMessageID Resource identifier of the string to display
          */
	    virtual void showMsgBox(UINT uiMessageID) = 0;


        /** 
          * Destructor
          *
          * Free resource library if loaded and destroy ItaUserMessage object.
          *
          */
        virtual ~ItaUserMessage() {};


        /** 
          * Generic parameter vector input operators
          *
          * Inserts a string into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const std::string& arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts a string into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const CString& arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts a boolean into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const bool arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts an integer into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const unsigned long arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts a short integer into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const short arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts an integer into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const int arg) = 0;


        /** 
          * Generic parameter vector input operators
          *
          * Inserts a double into the message.
          *
          * @param arg Parameter to be inserted into the message arguments member vector.
          */
        virtual ItaUserMessage& operator << (const float arg) = 0;


        /** 
          * clear
          *
          * Empties the member vector of message arguments
          *
          */
	    virtual void clear() = 0;
    };
};
#endif