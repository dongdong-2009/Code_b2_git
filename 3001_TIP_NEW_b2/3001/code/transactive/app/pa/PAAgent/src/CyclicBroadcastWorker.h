/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/CyclicBroadcastWorker.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/04/05 14:57:31 $
  * Last modified by:  $Author: qi.huang $
  *
  */


#if !defined(CYCLICBROADCASTWORKER_INCLUDED_)
#define CYCLICBROADCASTWORKER_INCLUDED_


#include <string>
#include "core/threads/src/Thread.h"


namespace TA_IRS_App
{
class AbstractBroadcastCircularList;
class AbstractBroadcast;
class ScheduledDvaHistory;
class IBroadcastManager;

class CyclicBroadcastWorker : public TA_Base_Core::Thread
{
public:
    CyclicBroadcastWorker( AbstractBroadcastCircularList& refObject, 
        ScheduledDvaHistory& refCyclicDvaHistory, IBroadcastManager& refInterface );
    virtual ~CyclicBroadcastWorker();
  
    virtual void run();
    virtual void terminate();

    void enable( const bool bEnable );
	void setModeChangingFlag(bool flag);

    bool processNextBroadcast();

protected:
    bool shouldRemoveBroadcast(AbstractBroadcast& broadcast);
    void sendProgressUpdateIfChanged(AbstractBroadcast& broadcast, const bool bNotifyClient = true);

    AbstractBroadcastCircularList& m_allBroadcasts;
    IBroadcastManager& m_refIBroadcastManager;
    ScheduledDvaHistory& m_refCyclicDvaHistory;
    volatile bool m_isRunning;
    volatile bool m_isEnabled;
	volatile bool m_isChangingToControlMode;

private:
    CyclicBroadcastWorker();
    CyclicBroadcastWorker( const CyclicBroadcastWorker& theCyclicBroadcastWorker);
    const CyclicBroadcastWorker& operator=( const CyclicBroadcastWorker& theCyclicBroadcastWorker);
};

} // namespace TA_IRS_App

#endif // !defined(CYCLICBROADCASTWORKER_INCLUDED_)

