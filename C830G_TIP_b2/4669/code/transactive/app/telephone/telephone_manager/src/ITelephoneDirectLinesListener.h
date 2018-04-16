/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/ITelephoneDirectLinesListener.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This interface provides coupling to notifications of changes to the Telephone
  * Speed Dial buttons according to the Observer pattern.
  */

#if !defined(ITelephoneDirectLinesListener_548C601F_9473_49c3_92AA_003CD024EEB1__INCLUDED_)
#define ITelephoneDirectLinesListener_548C601F_9473_49c3_92AA_003CD024EEB1__INCLUDED_

struct TelephoneSpeedDialButtonDetails;
class TelephoneDirectLinesMessageSubscriber;

class ITelephoneDirectLinesListener : public IGenericObserverListener
{

public:
    virtual ~ITelephoneDirectLinesListener() {};

	/**
	 * onDirectLineModifiedEvent  This must be implemented by the realised class and
	 * will receive notifications of changes to the Telephone Speed Dial buttons from
	 * the publisher.
	 * @param ppublisher    The source of the notification
	 * @param details    The detials of the changed button.
	 * 
	 */
	virtual void onDirectLineModifiedEvent(TelephoneDirectLinesMessageSubscriber* ppublisher, const TelephoneSpeedDialButtonDetails& details) =0;
};
#endif // !defined(ITelephoneDirectLinesListener_548C601F_9473_49c3_92AA_003CD024EEB1__INCLUDED_)
