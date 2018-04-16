/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/AbstractBroadcastCircularList.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#if !defined(AbstractBroadcastCircularList_INCLUDED)
#define AbstractBroadcastCircularList_INCLUDED

#include "app/pa/PAAgent/src/CircularList.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_App
{

class AbstractTrainBroadcast;
class TrainDVABroadcast;

class AbstractBroadcastCircularList : public CircularList<AbstractBroadcast*>
{
public:
    AbstractBroadcastCircularList();
    ~AbstractBroadcastCircularList();

    AbstractBroadcast* getBroadcast(const std::string& broadcastId);
    AbstractTrainBroadcast* getTrainBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId);
    AbstractTrainBroadcast* getTrainBroadcast(CORBA::Octet announcementId);

    std::list<TrainDVABroadcast*> getAllTrainDvaBroadcasts();

    bool isAnyBroadcastBusy();
    void clearAllBroadcasts();

    void appendAllBroadcastIdTypes(TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq& broadcastIdTypeSeq);
};


} // namespace TA_IRS_App

#endif // !defined(AbstractBroadcastCircularList_INCLUDED)

