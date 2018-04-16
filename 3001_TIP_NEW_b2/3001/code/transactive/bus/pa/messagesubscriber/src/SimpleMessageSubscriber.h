/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * SimpleMessageSubscriber is a skeleton implementation of GenericMessageSubscriber
  * which provides basic (minimal) subscription functionality for client code - but
  * sufficient functionality to be used without extension in many cases
  *
  */

#pragma once

#include "bus/pa/messagesubscriber/src/GenericMessageSubscriber.h"

namespace TA_IRS_Bus
{

    template <class Type>
    class SimpleMessageSubscriber 
    : 
    public GenericMessageSubscriber<Type>
    {
    public:
        
        /**
         * SimpleMessageSubscriber(constructor)
         *
         * @param messageType defines the channel we're subscribing to
         *
         * @param notificationMessage the message to be posted out upon 
         *          receipt of updated data
         *
         * @param locationKey defines the location we're bound to (required
         *          as a parameter defining what messages we're subscribed to)
         *
         */
        SimpleMessageSubscriber(const TA_Base_Core::MessageType& messageType, 
                                DWORD notificationMessage,
                                unsigned long locationKey)
        :
        GenericMessageSubscriber<Type>(notificationMessage, locationKey),
        m_messageType(messageType)
        {}
    
        /**
         * getMessageType (interface implementation)
         *
         * @see GenericMessageSubscriber::getMessageType for more details
         *
         */
        virtual const TA_Base_Core::MessageType& getMessageType() const { return m_messageType; }
    
    protected:
    
        /**
         * getNotificationData (interface implementation)
         *
         * @see GenericMessageSubscriber::getNotificationData
         *
         */
        virtual Type getNotificationData(const Type& packetData) { return packetData; }    
    
        /**
         * onSubscriptionSuccessful (interface implementation)
         *
         * @see GenericMessageSubscriber::onSubscriptionSuccessful for more details
         *
         */
        virtual void onSubscriptionSuccessful() {}
    
        /**
         * onPreProcessMessage (interface implementation)
         *
         * @see GenericMessageSubscriber::onPreProcessMessage for more details
         *
         */
        virtual void onPreProcessMessage(const Type& updateData) {}
        
        /**
         * onPostProcessMessage (interface implementation)
         *
         * @see GenericMessageSubscriber::onPostProcessMessage for more details
         *
         */
        virtual void onPostProcessMessage(const Type& updateData) {}
    
    private:
    
        const TA_Base_Core::MessageType& m_messageType;
    };
    
} // end namespace TA_IRS_Bus
