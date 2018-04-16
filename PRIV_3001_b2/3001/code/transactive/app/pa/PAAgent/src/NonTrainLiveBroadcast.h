/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/NonTrainLiveBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#if !defined(NonTrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define NonTrainLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/ITransactionOwner.h"

namespace TA_IRS_App
{

class NonTrainLiveBroadcast : public AbstractStationBroadcast, public ITransactionOwner
{
public:
    NonTrainLiveBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId );
    virtual ~NonTrainLiveBroadcast();

    virtual void validateBroadcastConfig(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);
    virtual void validateBroadcastConfigSpecifics()=0; //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

    virtual void prepare();
    virtual void execute();
    virtual void aborting();
    virtual bool isBusy();

    void updateCurrentlyPlayingZones();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_broadcastConfigV->theBroadcastConfigSpecifics.theStationLiveConfig().zones;
    }

    virtual void transactionExecuted( int nResult );

	bool checkAndApplyTransactionId();
	bool isTransactionIdValid();
    bool isPttActive();

    // Agent-synchronised BroadcastParameter
    virtual bool getIsConnected();
    virtual void setIsConnected(bool bValue, bool sendSynchronisation = true);

    virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
    virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

private:
    NonTrainLiveBroadcast( const NonTrainLiveBroadcast& theNonTrainLiveBroadcast); 
    NonTrainLiveBroadcast();   

protected:
    virtual void updateEffectiveZones( const std::vector<unsigned long>& vecNewZoneKeys );
    virtual void runtimeSpecifics();
    void executeSendStartCommand();
    void executeSendStopCommand();
    void executeWaitStartAcknoledge();
    void executeWaitStopAcknoledge();
    bool shouldSendStopCommand();
    bool waitAcknowledge();

protected:
    StdStationLiveConfig m_theSpecifics;

    bool m_bConnected;
    unsigned long m_ulConsoleUpdateTime;
    unsigned long m_ulZoneStateUpdateTime;

	std::map<unsigned long, unsigned short> m_mapLastTransactionState;
};

} // namespace TA_IRS_App

#endif

