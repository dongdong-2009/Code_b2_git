/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/DVARecordingBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#if !defined(DVARecordingBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define DVARecordingBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/ITransactionOwner.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"

namespace TA_IRS_App
{

class DVARecordingBroadcast : public virtual AbstractStationBroadcast, public ITransactionOwner
{

public:

    DVARecordingBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~DVARecordingBroadcast();

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

    virtual void validateBroadcastConfig(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

    virtual void prepare();
    virtual void execute();
    virtual void terminate();
    virtual void aborting();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        // Shouldn't retry this broadcast - just return nothing
        return TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage();
    }

    // Agent-synchronised BroadcastParameter
    bool getIsConnected();
    void setIsConnected(bool bValue, bool sendSynchronisation = true);
	virtual void transactionExecuted( int nResult );

protected:
    
    virtual void runtimeSpecifics();
    virtual int writeStartToPas();
    virtual int writeStopToPas();
	void executeSendStartCommand();
	void executeSendStopCommand();
	void executeWaitStartAcknoledge();
    void executeWaitStopAcknoledge();
    bool shouldSendStopCommand();
	bool waitAcknowledge();

private:
    DVARecordingBroadcast( const DVARecordingBroadcast& theDVARecordingBroadcast);    

protected:
    StdStationRecordingConfig m_theSpecifics; // For convenience
    bool m_isConnected;
    unsigned long m_ulConsoleUpdateTime;
    unsigned long m_ulRecordingCounting;
};


////////////////////////
//      INLINES
////////////////////////
inline void DVARecordingBroadcast::setIsConnected(bool bValue, bool sendSynchronisation)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    if ( m_isConnected == bValue )
    {
        return;
    }

    m_isConnected = bValue;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(bValue);

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

