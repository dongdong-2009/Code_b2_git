/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractStationBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */


#if !defined(AbstractStationBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractStationBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "ace/RW_Mutex.h"
#include "ace/Guard_T.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"

namespace TA_IRS_App
{

class AbstractStationBroadcast : public AbstractBroadcast
{
public:
    AbstractStationBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                             const std::string& sessionId);
        // throw CorruptInternalStateDataException

    void retry( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId );
        // throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) ;

    virtual ~AbstractStationBroadcast();

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getEffectiveZoneCoverage( 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones );
    
    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage() = 0;

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices calculateEffectiveCoverageIndices( 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& allZones, 
        const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& newZones );
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
	//added by lixiaoxia
	// Agent-synchronised BroadcastParameter
	CORBA::UShort getAnnouncementId();
	void setAnnouncementId(CORBA::UShort announcementId, bool sendSynchronisation = true);

	CORBA::UShort getNextAvailableStationAnnouncementId();
	//throw(TA_Base_Core::NoFreeAnnounceIdException);
	void releaseUsingAnnouncementId( const CORBA::UShort cUsingId );
	void occupyUsingAnnouncementId( const CORBA::UShort announcementId );

	virtual TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* getBroadcastParametersCopy();
	virtual void setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter );

private:
    AbstractStationBroadcast( const AbstractStationBroadcast& theAbstractStationBroadcast);   

protected:
    virtual int writeStartToPas() = 0;
    virtual int writeStopToPas() = 0;

    virtual void updateEffectiveZones( const std::vector<unsigned long>& vecNewZoneKeys );

protected:
    CORBA::UShort m_announceId;
    int m_nTransactionResult;
	static ACE_RW_Mutex s_mtxUsedAnnouncementId;
	static std::set<CORBA::UShort> s_setUsedAnnouncementIds;
};

} // namespace TA_IRS_App

#endif

