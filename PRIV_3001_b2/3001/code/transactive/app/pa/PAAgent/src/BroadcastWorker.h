/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/BroadcastWorker.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#if !defined(BroadcastWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define BroadcastWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_


#include <string>
#include "core/threads/src/Thread.h"


namespace TA_Base_Core 
{ 
    class CommsMessageSender; 
    class StateUpdateMessageSender; 
}

namespace TA_IRS_App
{
class AbstractBroadcastCircularList;
class AbstractBroadcast;
class IBroadcastManager;

class BroadcastWorker : public TA_Base_Core::Thread
{

public:
    BroadcastWorker( AbstractBroadcastCircularList& refObject, IBroadcastManager& refInterface );
    virtual ~BroadcastWorker();
  
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
    volatile bool m_isRunning;
    volatile bool m_isEnabled;
	volatile bool m_isChangingToControlMode;

private:
    BroadcastWorker();
    BroadcastWorker( const BroadcastWorker& theBroadcastWorker);
    const BroadcastWorker& operator=( const BroadcastWorker& theBroadcastWorker);
};

} // namespace TA_IRS_App

#endif // !defined(BroadcastWorker_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

