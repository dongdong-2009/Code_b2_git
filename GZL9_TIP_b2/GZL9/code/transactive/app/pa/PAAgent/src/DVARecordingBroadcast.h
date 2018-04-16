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

#if !defined(DVARECORDINGBROADCAST__INCLUDED_)
#define DVARECORDINGBROADCAST__INCLUDED_

#include <map>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{
class DVARecordingBroadcast : public virtual AbstractStationBroadcast
{

public:

    DVARecordingBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                           const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~DVARecordingBroadcast();

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

    virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);

    virtual void execute();
    bool executePhaseOneAndReturnIsCompleted();
    bool executePhaseTwoAndReturnIsCompleted();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        // Shouldn't retry this broadcast - just return nothing
        return TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage();
    }

    // Agent-synchronised BroadcastParameter
    bool getIsConnected();
    void setIsConnected(bool value, bool sendSynchronisation = true);

protected:
    
    virtual void runtimeSpecifics();

private:

    DVARecordingBroadcast( const DVARecordingBroadcast& theDVARecordingBroadcast);    
   
protected:

    TA_Base_Bus::IPAAgentCorbaDef::StationRecordingConfig&   m_theSpecifics; // For convenience

    bool                                                m_isConnected;


};



////////////////////////
//      INLINES
////////////////////////




inline void DVARecordingBroadcast::setIsConnected(bool value, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    m_isConnected = value;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(value);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED, data, getBroadcastId().c_str());
    }
}

inline bool DVARecordingBroadcast::getIsConnected()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_isConnected;
}

inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* DVARecordingBroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = 
        AbstractStationBroadcast::getBroadcastParametersCopy();

    int originalLength = parameters->length();
    parameters->length(originalLength+1);

    CORBA::Any data1;
    data1 <<= CORBA::Any::from_boolean(getIsConnected());
    (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED;
    (*parameters)[originalLength].data        = data1;

    return parameters;
}


inline void DVARecordingBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{
    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED)
    {
        bool updateData = 0;
        
        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setIsConnected(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: IsConnected=%s", updateData?"true":"false");

        }
    }
    else
    {
        AbstractStationBroadcast::setBroadcastParameter(parameter);
    }
}

} // namespace TA_IRS_App

#endif

