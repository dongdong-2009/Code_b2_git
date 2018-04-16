/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/TelephoneDirectLinesMessageSubscriber.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class will receive all notifications of changes posted on change of the
  * Telephone Speed Dial buttons. The class also acts as the publisher for
  * typically GUI components interested in notifications of these changes according
  * to the Observer pattern.
  */

#if !defined(TelephoneDirectLinesMessageSubscriber_5C9F7072_159D_4acc_B309_914795035BEA__INCLUDED_)
#define TelephoneDirectLinesMessageSubscriber_5C9F7072_159D_4acc_B309_914795035BEA__INCLUDED_

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/GenericSubscriberPublisher.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.h"

class ITelephoneDirectLinesListener;

class TelephoneDirectLinesMessageSubscriber : public GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef> 
{

public:
	TelephoneDirectLinesMessageSubscriber();
	virtual ~TelephoneDirectLinesMessageSubscriber();

public:
	std::vector<ITelephoneDirectLinesListener> *m_listeners;

	/**
	  * addListener
	  * 
	  * Register an ITelephoneDirectLinesListener. This must call addGenericListener of
	  * GenericSubscriberPublisher.
	  * 
	  * @return void 
	  * @param : ITelephoneDirectLinesListener* plistener
	  * 
	  * @exception <exceptions> Optional
	  */
	void addListener(ITelephoneDirectLinesListener* plistener);

protected:

	/**
	  * Subscribe
	  * 
	  * This will subscribe to the desired notifications of changes posted on change of
	  * the Telephone Speed Dial buttons. The receiveSpecialisedMessage method of
	  * GenericSubscriberPublisher will receive the notifications.  The following
	  * message types will be registered: TelephoneSpeedDialButtonModifiedNotification
	  * 
	  * @return virtual void 
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void Subscribe();

	/**
	  * ExtractMessageData
	  * 
	  * This will extract the appropriate Corba data object from the messageState, and
	  * subsequenty convert the Corba object to a non-Corba type to be passed on the
	  * the listeners. TA_Base_Bus::TelephoneDirectLinesCorbaDef::
	  * TelephoneSpeedDialButton and  from this a TelephoneSpeedDialButtonDetails
	  * object will be extracted for the following message types:
	  * TelephoneSpeedDialButtonModifiedNotification
	  * 
	  * @return virtual void 
	  * @param : const TA_Base_Core::CommsMessageCorbaDef& message
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message);

	/**
	  * PublishToListener
	  * 
	  * This will cast the IGenericObserverListener to a ITelephoneDirectLinesListener
	  * and call the appropriate method on the listener based on the messageType. The
	  * following messageType/method mapping will be used:
	  * TelephoneSpeedDialButtonModifiedNotification: onDirectLineModifiedEvent
	  * 
	  * @return virtual void 
	  * @param : const TA_Base_Core::CommsMessageCorbaDef& message
	  * @param : IGenericObserverListener* listener
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener);

private:
	TelephoneSpeedDialButtonDetails m_speedDialButtonDetails;

};
#endif // !defined(TelephoneDirectLinesMessageSubscriber_5C9F7072_159D_4acc_B309_914795035BEA__INCLUDED_)