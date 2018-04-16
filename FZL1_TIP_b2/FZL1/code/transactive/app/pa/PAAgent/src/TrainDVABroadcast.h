/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#if !defined(TRAINDVABROADCAST__INCLUDED_)
#define TRAINDVABROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainCommonCorba.h"


namespace TA_IRS_App
{

class TrainDVABroadcast : public virtual AbstractTrainBroadcast
{

public:

    TrainDVABroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                      const std::string& sessionId);
        // throws CorruptInternalStateDataException

    virtual ~TrainDVABroadcast();

    virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);

    virtual void execute();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage getTrainCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_theSpecifics.trains;
    }

    virtual void setHasOverrideOption(bool override)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        m_theSpecifics.hasOverrideOption = override;
    }

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


    // the train agent sends an asynchronous command result notification back
    void handleCommandResult( 
        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& effectiveCoverageIndices, 
        unsigned int trainIndexIndex,
            const TA_IRS_Bus::PaTypes::PaUpdateEvent& event );

        // this populates m_okTrainListWhileAborting with the current active trains during abort
        void populateActiveTrains();

        // to prevent the broadcast finishing while waiting for train statuses to return
        time_t m_terminateTimeLimit;

    protected:

        TA_Base_Bus::IPAAgentCorbaDef::TrainDvaConfig&      m_theSpecifics; // For convenience

        TA_Base_Bus::ITrainCommonCorba::TrainList           m_okTrainListWhileAborting;

    };

} // namespace TA_IRS_App

#endif

