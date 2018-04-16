/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/message/src/GenericSubscriberPublisher.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  GenericSubscriberPublisher.h
//  Implementation of the Class GenericSubscriberPublisher
//  Created on:      19-May-2005 06:47:03 PM
//  Original author: F. Stuart
///////////////////////////////////////////////////////////

#if !defined(GenericSubscriberPublisher_B8F8A6EF_4796_4ff9_B0F1_A91669953D0D__INCLUDED_)
#define GenericSubscriberPublisher_B8F8A6EF_4796_4ff9_B0F1_A91669953D0D__INCLUDED_

#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "core\message\src\IGenericObserverListener.h"

/**
 * Interface for implementing a SpecialisedMessageSubscriber combined with the
 * Observer pattern.
 * @created 20-Mar-2005 04:35:55 PM
 * @author F. Stuart
 * @version 1.0
 */
template<class CorbaDef>
class GenericSubscriberPublisher : public TA_Base_Core::SpecialisedMessageSubscriber<CorbaDef>
{

public:
    GenericSubscriberPublisher() { }
    virtual ~GenericSubscriberPublisher() { }

public:
	/**
	 * @param	T	The narrowed Message
	 * @param message    message
	 * 
	 */
	virtual void receiveSpecialisedMessage(const CorbaDef& message)
    {
        // Do you conversion from the Corba Any type to your internal data structure here.
        // Cache the data structure for later use in the PublishToListener method
        ExtractMessageData(message);
        
        // Now publish the event
        Publish(message);
    }

protected:
	/**
	 * Register a generic listener. This method should only be called by the child
	 * class to ensure only the correct type of listeners can be added. This is
	 * necessary to ease the dynamic cast of the IGenericObserverListener to the
	 * correct generalised listener class.
	 * @param listener
	 * @param plistener
	 * 
	 */
	virtual void addGenericListener(IGenericObserverListener* plistener)
    {
        m_genericListeners.push_back(plistener);
    }

	/**
	 * This is overriden by the child classs to subscribe to its specialised messages.
	 */
	virtual void Subscribe() =0;
	
    /**
	 * This is overriden by the child class to extract of the desired data structure
	 * from the messageState.
	 * @param message    message
	 * 
	 */
	virtual void ExtractMessageData(const CorbaDef& message) =0;

	/**
	 * This is overriden by the child class to publish to the appropriate type of
	 * listener.
	 * @param message
	 * @param listener    listener
	 * 
	 */
	virtual void PublishToListener(const CorbaDef& message, IGenericObserverListener* listener) =0;

private:
	static std::vector<IGenericObserverListener*> m_genericListeners;

	/**
	 * This will iterate through all the m_genericListeners and call
	 * PublishToListeners on the child class for each listener.
	 * @param message    message
	 * 
	 */
	void Publish(const CorbaDef& message)
    {
        // Publish the event to all listeners
        std::vector<IGenericObserverListener*>::iterator iter;
        for(iter = m_genericListeners.begin(); iter != m_genericListeners.end(); iter++)
        {
            PublishToListener(message, *iter);
        }
    }
};
#endif // !defined(GenericSubscriberPublisher_B8F8A6EF_4796_4ff9_B0F1_A91669953D0D__INCLUDED_)
