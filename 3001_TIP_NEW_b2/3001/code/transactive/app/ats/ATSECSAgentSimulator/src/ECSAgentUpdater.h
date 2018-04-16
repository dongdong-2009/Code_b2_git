/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/15 18:11:22 $
  * Last modified by:  $Author: qi.huang $
  *
  */

// ECSAgentUpdater.h: interface for the ECSAgentUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECSAgentUpdater_H__6BFC468B_1F81_4AF9_9459_2A65ECBE1E8A__INCLUDED_)
#define AFX_ECSAgentUpdater_H__6BFC468B_1F81_4AF9_9459_2A65ECBE1E8A__INCLUDED_


#include<string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/exceptions/src/ECSAgentExceptions.h"

namespace TA_Base_Core { class CommsMessageSender; }

class ECSAgentUpdater  
{


public:

    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static ECSAgentUpdater* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    void setEntityKey (unsigned long data);

    virtual ~ECSAgentUpdater();

    void sendUpdate(unsigned long ECSZoneId, 
                    unsigned long maxExternalTemperatureInCelsius, 
                    unsigned long stallTimeInSeconds,
                    bool          isInEmergency );

private:

	ECSAgentUpdater();

    // Synchronisation lock for safe multithreaded access
    static TA_Base_Core::ReEntrantThreadLockable m_lock;
    static ECSAgentUpdater*                 m_me;
    TA_Base_Core::CommsMessageSender*            m_commsSender;
    unsigned long                           m_entityKey;
    unsigned long                           m_locationKey;

};


#endif // !defined(AFX_ECSAgentUpdater_H__6BFC468B_1F81_4AF9_9459_2A65ECBE1E8A__INCLUDED_)
