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

#if !defined(STATIONLIVEBROADCAST__INCLUDED_)
#define STATIONLIVEBROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{

class StationLiveBroadcast : public virtual AbstractStationBroadcast
{

public:

    StationLiveBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                          const std::string& sessionId);
        // throws CorruptInternalStateDataException

    virtual ~StationLiveBroadcast();

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

    virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);

    virtual void execute();
    bool executePhaseOneAndReturnIsCompleted();
    bool executePhaseTwoAndReturnIsCompleted();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_theSpecifics.zones;
    }

    // Agent-synchronised BroadcastParameter
    bool getIsConnected();
    void setIsConnected(bool value, bool sendSynchronisation = true);

	// TD17456
	virtual bool isAllFailureZones();
	// TD17456

protected:
    
    virtual void runtimeSpecifics();

    void updateProgressReport(bool isRuntime = true);



private:

    StationLiveBroadcast( const StationLiveBroadcast& theStationLiveBroadcast);    

protected:

    TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig&    m_theSpecifics; // For convenience

    bool                                            m_isConnected;

};



////////////////////////
//      INLINES
////////////////////////


inline bool StationLiveBroadcast::getIsConnected()
{
    return m_isConnected;
}

inline void StationLiveBroadcast::setIsConnected(bool value, bool sendSynchronisation)
{
    m_isConnected = value;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(value);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED, data, getBroadcastId().c_str());
    }
}

inline TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* StationLiveBroadcast::getBroadcastParametersCopy()
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

inline void StationLiveBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
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

