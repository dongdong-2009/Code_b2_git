/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/ITelephoneDirectLinesListener.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
