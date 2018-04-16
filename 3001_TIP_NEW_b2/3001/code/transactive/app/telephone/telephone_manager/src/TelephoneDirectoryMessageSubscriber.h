/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryMessageSubscriber.h $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class will receive all notifications of changes posted on change of the
  * Telephone directory. The class also acts as the publisher for typically GUI
  * components interested in notifications of these changes according to the
  * Observer pattern.
  */

#if !defined(TelephoneDirectoryMessageSubscriber_6B8C2642_9F03_40ba_817E_70994AC2825B__INCLUDED_)
#define TelephoneDirectoryMessageSubscriber_6B8C2642_9F03_40ba_817E_70994AC2825B__INCLUDED_

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/GenericSubscriberPublisher.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"

class ITelephoneDirectoryListener;

class TelephoneDirectoryMessageSubscriber : public GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef> 
{

public:
	TelephoneDirectoryMessageSubscriber();
	virtual ~TelephoneDirectoryMessageSubscriber();

public:
	/**
	  * addListener
	  * 
 	  * Register an ITelephoneDirectoryListener. This must call addGenericListener of
	  * GenericSubscriberPublisher.
	  * 
	  * @return void 
	  * @param : ITelephoneDirectoryListener* plistener
	  * 
	  * @exception <exceptions> Optional
	  */
	void addListener(ITelephoneDirectoryListener* plistener);

protected:
	/**
	  * ExtractMessageData
	  * 
	  * This will extract the appropriate Corba data object from the messageState, and
	  * subsequenty convert the Corba object to a non-Corba type to be passed on the
	  * the listeners. TA_Base_Bus::TelephoneDirectoryCorbaDef::TelephoneDirectoryEntry
	  * and  from this a TelephoneDirectoryEntryData object will be extracted for the
	  * following message types: TelephoneDirectoryEntryAddNotification
	  * TelephoneDirectoryEntryModifyNotification
	  * TelephoneDirectoryEntryDeleteNotification TA_Base_Bus::
	  * TelephoneDirectoryCorbaDef::TelephoneDirectory and  from this a
	  * TelephoneDirectoryData object will be extracted for the following message types:
	  * A TelephoneDirectoryData object will be extracted for the following message
	  * types: TelephoneSubdirectoryAddNotification
	  * TelephoneSubdirectoryModifyNotification
	  * TelephoneSubdirectoryDeleteNotification
	  * 
	  * @return virtual void 
	  * @param : const TA_Base_Core::CommsMessageCorbaDef& message
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message);

	/**
	  * Subscribe
	  * 
	  * This will subscribe to the desired notifications of changes posted on change of
	  * the Telephone directory. The receiveSpecialisedMessage method of
	  * GenericSubscriberPublisher will receive the notifications. The following
	  * message types will be registered: TelephoneDirectoryEntryAddNotification
	  * TelephoneDirectoryEntryModifyNotification
	  * TelephoneDirectoryEntryDeleteNotification  TelephoneSubdirectoryAddNotification
	  * TelephoneSubdirectoryModifyNotification
	  * TelephoneSubdirectoryDeleteNotification TelephoneDirectoryUploadNotification
	  * 
	  * @return virtual void 
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void Subscribe();

	/**
	  * PublishToListener
	  * 
	  * This will cast the IGenericObserverListener to a ITelephoneDirectoryListener
	  * and call the appropriate method on the listener based on the messageType. The
	  * following messageType/method mapping will be used:
	  * TelephoneDirectoryEntryAddNotification: onDirectoryEntryEvent with name = "New"
	  * TelephoneDirectoryEntryModifyNotification: onDirectoryEntryEvent with name =
	  * "Update" TelephoneDirectoryEntryDeleteNotification: onDirectoryEntryEvent with
	  * name = "Delete" TelephoneSubdirectoryAddNotification: onDirectoryEvent with
	  * name = "New" TelephoneSubdirectoryModifyNotification: onDirectoryEvent with
	  * name = "Update" TelephoneSubdirectoryDeleteNotification: onDirectoryEvent with
	  * name = "Delete" TelephoneDirectoryUploadNotification: onDirectoryUploadEvent
	  * 
	  * @return virtual void 
	  * @param : const TA_Base_Core::CommsMessageCorbaDef& message
	  * @param : IGenericObserverListener* listener
	  * 
	  * @exception <exceptions> Optional
	  */
	virtual void PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener);

private:
	TelephoneDirectoryEntryData m_directoryEntryData;
	TelephoneDirectoryData m_directoryData;

};
#endif // !defined(TelephoneDirectoryMessageSubscriber_6B8C2642_9F03_40ba_817E_70994AC2825B__INCLUDED_)
