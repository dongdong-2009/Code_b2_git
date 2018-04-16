/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/control_station/src/ConsecutiveNetworkFailureAlarmPolicy.h $
  * @author:  Teik Oh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This policy allows for a consecutive number of failures before
  * indicating "down".  A single success resets it to "up".
  *
  * Added a method so that m_consecutiveFails can be reset after construction
  *
  */

#ifndef CONSECUTIVE_NETWORK_FAILURE_ALARM_POLICY_H
#define CONSECUTIVE_NETWORK_FAILURE_ALARM_POLICY_H

#include "INetworkFailureAlarmPolicy.h"


class ConsecutiveNetworkFailureAlarmPolicy : public INetworkFailureAlarmPolicy
{
public:

    /** 
      * ConsecutiveNetworkFailureAlarmPolicy
      *
      * construct with the number of consecutive fails that represents
	  * a failure
      *
      *
      * @param unsigned int maxConsecutiveFails: number of allowable
	  * consecutive ping failures
      */
	ConsecutiveNetworkFailureAlarmPolicy(unsigned int maxConsecutiveFails);


    /** 
      * initPolicyState
      *
      * set state: true = point Is Pingable, false = point Is NOT Pingable
      *
	  * Call this with true after you have responded to the point not 
	  * being pinigable so that the failure count is reset to zero
      * @param bool state
      */
	void initPolicyState(bool state);


    /** 
      * addResult
      *
      * add the next ping result
      *
      * @param bool pingSuccess
      */
	void addResult(bool pingSuccess);

    /** 
      * pointIsPingable
      *
      * based on the sequences of inputs applied via addResult(), are
	  * we pingable or not?
	  *
      *
      * @return bool 
      *
      */
	bool pointIsPingable();

    /** 
      * setMaxConsecutiveFails
      *
      * apply a new number of allowable consecutive ping failures
      *
      * @param unsigned int maxConsecutiveFails: number of allowable
	  * consecutive ping failures
	  *
	  * Note that this method is not in the INetworkFailureAlarmPolicy
	  * interface
      */
	void setMaxConsecutiveFails(unsigned int maxConsecutiveFails);


private:
	unsigned int m_maxConsecutiveFails;
	unsigned int m_failCount;
};


#endif
