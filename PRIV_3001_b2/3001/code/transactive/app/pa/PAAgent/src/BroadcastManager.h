#ifndef BROADCASTMANAGER_H
#define BROADCASTMANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/BroadcastManager.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include <string>
#include <map>

#include "ace/RW_Mutex.h"

#pragma warning ( disable : 4250 )
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#pragma warning ( default : 4250 )

#include "app/pa/PAAgent/src/IBroadcastManager.h"
#include "app/pa/PAAgent/src/IPasConnectionObserver.h"

namespace TA_IRS_App
{

class AbstractBroadcast;
class AbstractBroadcastCircularList;
class BroadcastWorker;
class CyclicBroadcastWorker;
class TrainAgentSubscriber;
class ScheduledDvaHistory;

typedef std::map<std::string, AbstractBroadcast*> IdToBroadcastMap;
typedef IdToBroadcastMap::iterator IdToBroadcastMapIt;

/**
 * This class manages the state of broadcasts within the PA Agent.
 */
class BroadcastManager : public IBroadcastManager, public IPasConnectionObserver
{
public:
    BroadcastManager();
    virtual ~BroadcastManager();

    void enable( const bool bEnable = true );
    void setModeChangingFlag(bool flag);

    virtual void connectionUpdated( bool bConnectionAvailable );
    bool isConnectionAvaliable();

    bool isBroadcastIdExisted( const std::string& strId );

    void addTrainBroadcast( AbstractBroadcast* pBroadcast );
    void addStationBroadcast( AbstractBroadcast* pBroadcast );
    void addCyclicBroadcast( AbstractBroadcast* pBroadcast );

    virtual void delBroadcast( const std::string& strId );

    bool updateBroadcastId( const std::string& strSrcId, const std::string& strObjId );

    unsigned long getTrainBroadcastCount();
    unsigned long getStationBroadcastCount();

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* getTrainBroadcastIdTypes();
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* getStationBroadcastIdTypes();

    AbstractBroadcast* getBroadcastById( const std::string& strId );

    const int getMusicStatus();
    void setMusicStatus( const int eType );

    void getCompleteBroadcastStateSeqCopy( TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq& refState );
    bool getCompleteAvaliableEventIds( TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds& refEventIds );
    void setCompleteAvaliableEventIds( const TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds& refEventIds );

    void processPAAgentNewBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& updateData );
    void processPAAgentRemovedBroadcastUpdate( const std::string& updateData );
    void processPAAgentRenamedBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData );
    void processPAAgentBroadcastParameterUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& updateData );
    void processPAAgentBroadcastProgressUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport& updateData );
    void processPAAgentEntireAgentStateUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CompleteAgentState& updateData );

    void restoreScheduledDva();

protected:
    ACE_RW_Mutex m_mtxBroadcastsLock;
    IdToBroadcastMap m_mapIdToBroadcasts;
    AbstractBroadcastCircularList* m_pStationBroadcasts;
    AbstractBroadcastCircularList* m_pCyclicDvaBroadcasts;
    AbstractBroadcastCircularList* m_pTrainBroadcasts;
    BroadcastWorker* m_pStationBroadcastWorker;
    BroadcastWorker* m_pTrainBroadcastWorker;
    CyclicBroadcastWorker* m_pCyclicDvaWorker;
    TrainAgentSubscriber* m_trainAgentSubscriber;
    ScheduledDvaHistory* m_pScheduledDvaHistory;

    int m_MusicType;
    bool m_bConnectionAvailable;
};

} // namespace TA_IRS_App

#endif // BROADCASTMANAGER_H

