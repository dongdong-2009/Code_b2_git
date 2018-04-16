/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/TrainDVABroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(TrainDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define TrainDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"


namespace TA_IRS_App
{

class TrainDVABroadcast : public virtual AbstractTrainBroadcast
{

public:

    TrainDVABroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                      const std::string& sessionId);
        // throws CorruptInternalStateDataException

    virtual ~TrainDVABroadcast();

    virtual void validateBroadcastConfig(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

    virtual void prepare();
    virtual void execute();
    virtual void aborting();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainCoverage();

    virtual void setHasOverrideOption(bool override);

    virtual void processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

    virtual bool getIsPartOfBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId);

    virtual void operatorAbort();

protected:
    
    virtual void runtimeSpecifics();
    bool isCyclic();


private:

    TrainDVABroadcast( const TrainDVABroadcast& theTrainDVABroadcast);    

    // every time an update is received it ensures the whole broadcast hasnt failed
    void checkAllTrainStatus();

    void disconnect();

    bool areAllTrainsFinished();

    // the train agent sends an asynchronous command result notification back
    void handleCommandResult( TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& effectiveCoverageIndices, 
        unsigned int trainIndexIndex,  const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

    protected:

        TA_Base_Bus::IPAAgentCorbaDef::TrainDvaConfig&      m_theSpecifics; // For convenience
    };

} // namespace TA_IRS_App

#endif

