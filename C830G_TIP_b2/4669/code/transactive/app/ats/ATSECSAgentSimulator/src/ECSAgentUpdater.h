/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

namespace TA_Core { class CommsMessageSender; }

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
    static TA_Core::ReEntrantThreadLockable m_lock;
    static ECSAgentUpdater*                 m_me;
    TA_Core::CommsMessageSender*            m_commsSender;
    unsigned long                           m_entityKey;
    unsigned long                           m_locationKey;

};


#endif // !defined(AFX_ECSAgentUpdater_H__6BFC468B_1F81_4AF9_9459_2A65ECBE1E8A__INCLUDED_)
