/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsAgentCompleteState.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CtaEcsAgentCompleteState_INCLUDED)
#define CtaEcsAgentCompleteState_INCLUDED

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"

class ItaEcsComms;
class ItaEcsSynchronisation;
class ItaEcsCachedConfig;




class CtaEcsAgentCompleteState: public virtual ItaEcsAgentCompleteState, 
                                public virtual TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>
                         
{

public:

    CtaEcsAgentCompleteState(
        ItaEcsComms& commsInstance, 
        ItaEcsSynchronisation& synchronisationInstance, 
        ItaEcsCachedConfig& cachedConfigInstance);

    virtual ~CtaEcsAgentCompleteState();

    virtual void setMonitor(bool isOnStartup=false);
    virtual void setControl(bool isOnStartup=false);

    
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Set Methods
    //
    ///////////////////////////////////////////////////////////////////////////////


    virtual void setDataPoints(DataPoints& dataPoints);
    virtual void setStationModeState(const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& stationModeState);
    virtual void setMasterModeState(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& masterModeState);

    virtual void commitProposedModes();

    // Modifiers for ProposedModes
    virtual void addProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode);
    virtual void removeProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode);        
    virtual void clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType);
    virtual void clearProposedOverridingStationModes(unsigned long locationKey);
    virtual void clearProposedStationModes(unsigned long locationKey);


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Get Methods
    //
    ///////////////////////////////////////////////////////////////////////////////

        
    virtual void getStationModeState(TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& returnValue);
    // Just the execution state enum
    virtual void getStationModeState(TA_IRS_Bus::EEcsExecutionState& returnValue);
    virtual void getMasterModeState(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& returnValue);

    virtual void getProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes& returnValue);
    virtual void getProposedModeSet(ProposedModeSet& returnValue);
    virtual void getProposedModeSet(
        unsigned long locationKey, 
        TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType,
        ProposedModeSet& returnValue);



      
protected:

    void processEcsAgentStateUpdateRequest();

    void processEcsAgentParameterUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message );

    void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Send To Monitor Methods
    //
    ///////////////////////////////////////////////////////////////////////////////

    void sendStationModeState();
    void sendMasterModeState();
    void sendProposedModes();


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Receive From Control Methods
    //
    ///////////////////////////////////////////////////////////////////////////////

    void receiveStationModeState(const CORBA::Any& data);
    void receiveMasterModeState(const CORBA::Any& data);
    void receiveProposedModes(const CORBA::Any& data);


    ItaEcsComms&                                    m_commsInstance;
    ItaEcsSynchronisation&                          m_synchronisationInstance;
    ItaEcsCachedConfig&                             m_cachedConfigInstance;

    volatile bool                                   m_isInControl;
    bool                                            m_hasProposedModesChangedSinceCommit;

    // Common
    DataPoints*                                     m_dataPoints;
    ProposedModeSet                                 m_proposedModeSet;
    
    
    // Master
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData
                                                    m_masterModeState;

    // Station
    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData
                                                    m_stationModeState;

    // Synchronisation locks for each of the items
    TA_Base_Core::ReEntrantThreadLockable           m_dataPointsLock;
    TA_Base_Core::ReEntrantThreadLockable           m_masterModeStateLock;
    TA_Base_Core::ReEntrantThreadLockable           m_stationModeStateLock;
    TA_Base_Core::ReEntrantThreadLockable           m_proposedModeSetLock;

};

#endif // !defined(CtaEcsAgentCompleteState_INCLUDED)





