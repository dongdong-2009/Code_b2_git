/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/ITelephoneDirectoryListener.h $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface provides coupling to notifications of changes to the Telephone
  * Directory according to the Observer pattern.
  */


#if !defined(ITelephoneDirectoryListener_B1A2F5EA_00B4_4f24_843F_FBA66AB665BC__INCLUDED_)
#define ITelephoneDirectoryListener_B1A2F5EA_00B4_4f24_843F_FBA66AB665BC__INCLUDED_

#include "core/message/src/IGenericObserverListener.h"

struct TelephoneDirectoryData;
struct TelephoneDirectoryEntryData;

class TelephoneDirectoryMessageSubscriber;

class ITelephoneDirectoryListener : public IGenericObserverListener 
{
public:
    enum EventType { New, Update, Delete };

    virtual ~ITelephoneDirectoryListener() {};

	/**
	 * onDirectoryEntryEvent  This must be implemented by the realised class and will
	 * receive notifications of changes to the Telephone Directory Entries from the
	 * publisher.
	 * @param ppublisher    The source of the notification
	 * @param name    The name of the event that occurred, e.g. "New" or "Update" or
	 * "Delete".
	 * @param directoryEntryData    The data for the event.
	 * 
	 */
	virtual void onDirectoryEntryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, EventType type, const TelephoneDirectoryEntryData& directoryEntryData) =0;
	
    /**
	 * onDirectoryUploadEvent  This must be implemented by the realised class and will
	 * receive notifications of the Telephone Directory being reloaded from the
	 * publisher.
	 */
	virtual void onDirectoryUploadEvent() =0;
	
    /**
	 * publishDirectoryEvent  This must be implemented by the realised class and will
	 * receive notifications of changes to the Telephone Directory from the publisher.
	 * @param ppublisher    The source of the notification
	 * @param name    The name of the event that occurred, e.g. "New" or "Update" or
	 * "Delete".
	 * @param directoryData    The data for the event.
	 * 
	 */
	virtual void onDirectoryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, EventType type, const TelephoneDirectoryData& directoryData) =0;
};
#endif // !defined(ITelephoneDirectoryListener_B1A2F5EA_00B4_4f24_843F_FBA66AB665BC__INCLUDED_)
