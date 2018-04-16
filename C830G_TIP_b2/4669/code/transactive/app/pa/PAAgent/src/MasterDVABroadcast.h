/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/MasterDVABroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(MasterDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define MasterDVABroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"


namespace TA_IRS_App
{
    class Table1200;
    class Table1201;
    class Table1301;
    class Table1610;

class MasterDVABroadcast : public virtual AbstractNonTrainDVABroadcast
{

public:

    MasterDVABroadcast( Table202& table202,
                        Table302& table302,
                        Table350& table350,
                        Table1200& table1200,
                        Table1201& table1201,
                        Table1301& table1301,
                        Table1610& table1610,
                        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                        const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~MasterDVABroadcast();

    virtual void validateBroadcastConfigSpecifics(); 
        //throw(TA_Base_Core::BadParameterException);

    virtual void writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId);
        // throws (PasWriteErrorException, PasConnectionException)

    virtual void performVersionCheck();

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
	virtual int getZoneStatusTableIndex();
	// ++jeffrey Bug457

	// TD17456
	virtual bool isAllFailureZones();
	// TD17456

protected:

    void getEquipmentComponentAndVerify( const std::string& equipmentName, 
                                         std::string& locationName, 
                                         std::string& pidName);
        //throw(TA_Base_Core::InvalidPasConfigurationException);

    void setupAndValidateTisParameters(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones);
        //throw(TA_Base_Core::BadParameterException);
    

private:

    MasterDVABroadcast( const MasterDVABroadcast& theMasterDVABroadcast);    

protected:

    typedef std::map<std::string, TA_Base_Core::STISDestination>     LocationNameToDestinationMap;
    typedef LocationNameToDestinationMap::iterator              LocationNameToDestinationMapIt;

    TA_IRS_App::Table1200&                                      m_table1200;
    TA_IRS_App::Table1201&                                      m_table1201;
    TA_IRS_App::Table1301&                                      m_table1301;
    TA_IRS_App::Table1610&                                      m_table1610;

    LocationNameToDestinationMap                    m_tisCoverageMap;

};

} // namespace TA_IRS_App

#endif

