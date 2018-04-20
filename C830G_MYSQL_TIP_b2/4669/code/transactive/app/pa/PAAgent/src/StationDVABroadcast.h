/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationDVABroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(StationDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define StationDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/Table200.h"

namespace TA_IRS_App
{
    class Table201;
    class Table301;
    class Table302;
    class Table350;

class StationDVABroadcast : public virtual AbstractNonTrainDVABroadcast
{

public:

    StationDVABroadcast( Table202& table202,
                         Table201& table201,
                         Table302& table302,
                         Table350& table350,
                         Table200& table200,
                         Table301& table301,
                         TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                         const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~StationDVABroadcast();

    virtual void validateBroadcastConfigSpecifics(); 
        //throw(TA_Base_Core::BadParameterException);

    virtual void writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId);
        // throws (PasWriteErrorException, PasConnectionException)

	//Maochun Sun++
	//TD13411
    //virtual void updateCacheWithLatestZoneStatus(unsigned long zoneKey);
	virtual void updateCacheWithLatestZoneStatus();
        // throws (PasConnectionException)
	//++Maochun Sun
	//TD13411

    virtual void sendRequestToTisAgent(const TA_IRS_Bus::StationDvaMessageRecord& record, 
                                        const char* startTimeString,
                                        const char* stopTimeString,
                                        CORBA::UShort tisPriority);
        // throws (...)

    virtual void disconnectSourceId();
    virtual void disconnectSequenceId();
	// jeffrey++ Bug457
	virtual int getZoneStatusTableIndex() { return m_table200.m_readIndex; };
	// ++jeffrey Bug457

	// TD17456
	virtual bool isAllFailureZones();
	// TD17456

protected:

    std::string getEquipmentComponentAndVerify(const std::string& equipmentName);
        //throw(TA_Base_Core::InvalidPasConfigurationException);

    void setupAndValidateTisParameters(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
        //throw(TA_Base_Core::BadParameterException);
    
private:

    StationDVABroadcast( const StationDVABroadcast& theStationDVABroadcast);    

protected:

    TA_IRS_App::Table200&                                       m_table200;
    TA_IRS_App::Table201&                                       m_table201;
    TA_IRS_App::Table301&                                       m_table301;
    
    std::string                                     m_locationName;

};

} // namespace TA_IRS_App

#endif

