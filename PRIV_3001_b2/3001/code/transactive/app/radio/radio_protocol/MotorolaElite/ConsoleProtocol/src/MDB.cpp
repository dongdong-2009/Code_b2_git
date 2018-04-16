/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/MDB.cpp $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include "MDB.h"


#include <windows.h> 
#include "Mca/mca.h"
#include "Mcdb/mcdb.h"

#include "Mcdb/trk_res.h"
#include "Mcdb/trunk_sys_db.h"
#include "Mcdb/op_position_db.h"
#include "Mcdb/res_capability_db.h"
#include "Mcdb/nonsig_conv_radio.h"


#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/IMcdbObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_manager_gui/src/DebugMsg.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"


using namespace TA_Base_Core;


const std::string MDB::TALK_GROUP_RES_TYPE       = "TG";
const std::string MDB::MULTI_GROUP_RES_TYPE      = "MG"; // Announcement Group
const std::string MDB::PRIVATE_CALL_RES_TYPE     = "PVC";
const std::string MDB::SITE_WIDE_CALL_RES_TYPE   = "SW";
const std::string MDB::DYNAMIC_GROUP_RES_TYPE    = "DG";
const std::string MDB::BIM_RES_TYPE    = "BIM";
const std::string MDB::TALK_GROUP_RES_TYPE2 = "CCGW";

const unsigned long MDB::MIN_TALKGROUP_ID    = 1000;
const unsigned long MDB::MAX_TALKGROUP_ID    = 5999;
const unsigned long MDB::MIN_MULTIGROUP_ID   = 7000;
const unsigned long MDB::MAX_MULTIGROUP_ID   = 7999;
const unsigned long MDB::MIN_SITEWIDE_ID     = 8000;
const unsigned long MDB::MAX_SITEWIDE_ID     = 8999;
const unsigned long MDB::MIN_DYNAMICGROUP_ID = 9000;
const unsigned long MDB::MAX_DYNAMICGROUP_ID = 9999;

MDB::MDB(IMcdbObserver* callback,MCC7500SDK::IAPIRequest_ptr& ptr, TA_Base_Core::ReEntrantThreadLockable& apiLock, MDBQueryTimeout mdbTimeout)
:m_mcdbObserver(callback), m_isInitializing(false), m_pAPIRequest(ptr),m_apiLock(apiLock), m_mdbQueryTimeout(mdbTimeout), m_MccReconnection(false)
{
	FUNCTION_ENTRY("constructor");
    TA_Assert(callback != NULL);
	FUNCTION_EXIT;
}


MDB::~MDB()
{
	FUNCTION_ENTRY("destructor");
	FUNCTION_EXIT;
}


void MDB::run()
{
    FUNCTION_ENTRY("run");
    m_isInitializing = true;

    try
    {
        // initalizeResourceDatabase
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Initializing database resource");
        initalizeResourceDatabase();

        m_isInitializing = false;

		//Should only call notify if there is no MCC Reconnection.
		if ( false == m_MccReconnection )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling Notify..");
			//Should only call notify if the there is no reconnection from MCC Server due to Database problem
			// notify caller initalizeResourceDatabase complete
			m_mcdbObserver->notify();
		}
        
    }
    catch (...)
    {
        m_isInitializing = false;
		//::MessageBox(NULL,"Cannot initialize MCC Database","ERROR",MB_OK); //Suggested by Bihui
		The_StatusManager::instance()->failConnectToRadio();
    }

    FUNCTION_EXIT;
}


void MDB::terminate()
{
    FUNCTION_ENTRY("terminate");
    FUNCTION_EXIT;
}


bool MDB::getRadioResource(std::vector<RadioResource>& radioResource)
{
    FUNCTION_ENTRY("getRadioResource");

    bool ret = false;

    if (m_isInitializing == false)
    {
        radioResource.clear();
        if (!m_radioResource.empty())
        {
            radioResource.resize(m_radioResource.size());
			std::copy(m_radioResource.begin(), m_radioResource.end(), radioResource.begin());
        }
        ret = true;
    }

    FUNCTION_EXIT;
    return ret;
}


void MDB::initalizeResourceDatabase()
{
    FUNCTION_ENTRY("initalizeResourceDatabase");
    try
    {
        unsigned hDb;

        if (openDatabase(hDb))
        {
            initResourceList(hDb);
            closeDatabase(hDb);
        }
        else
        {
			// TD17159 if opening the SERVER mcdb is unsuccessful, access LOCAL mcdb instead
			if (openDatabase(hDb))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Database opened successfully, now initialize resource list");
				initResourceList(hDb);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Closing database");
				closeDatabase(hDb);				
			}
            else
			{
				TA_THROW(TA_Base_Core::TransactiveException("MCDB openDatabase() Failed!"));
			}
        }
    }
	catch(TA_Base_Core::TransactiveException& ex)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"%s",ex.what());
	}
    catch (...)
    {
        TA_THROW(TA_Base_Core::TransactiveException("unkown exception occur while initalizeResourceDatabase" ));
    }

    FUNCTION_EXIT;
}

static void testme(void* phDb, RID rid)
{
    WORD wError;
    HQ hQ;
    DWORD dwNumRecs;
    wError = McdbCreateResCapabilityQuery(static_cast<HDB*>(phDb), &hQ, &dwNumRecs, rid);
	
    if (wError == MCDB_NO_ERROR)
    {
        RES_CID res_cid;
        RES_CAP_INDEX iResCapIndex;
        char szLabel[MAX_LABEL_LENGTH];
        for (; dwNumRecs > 0; --dwNumRecs)
        {
            wError = McdbGetNextResourceCapability(hQ, &res_cid, szLabel, &iResCapIndex);
//            wError = McdbGetResCapabilityLabel(static_cast<HDB*>(phDb), rid, res_cid, szLabel, iResCapIndex);
            char buff[512] = {0};
            sprintf(buff, "ResCap: %s %d, %d", szLabel, res_cid, iResCapIndex);
            //AddDebugMessage(buff);

            if (MCDB_NO_ERROR == wError && NULL_CID != res_cid)
            {
            }
        }
        McdbDestroyQuery(hQ);				
    }
}

void MDB::initResourceList(unsigned &phDb)
{
    FUNCTION_ENTRY("initResourceList");

	//Put some delay to give time for MCC to reload its Database records 
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Give %d sec delay for MCC to refresh its Database records..",(m_mdbQueryTimeout.DatabaseMCCQueryTimeout)/1000);
	Thread::sleep(m_mdbQueryTimeout.DatabaseMCCQueryTimeout);

	char    szLabel[MAX_LABEL_LENGTH];
	RID     Rid;
	WORD    wError;
	HQ      hQ;
	std::vector<WORD> trkSystemIds;

	//Make sure to set m_MccReconnection to false by default during initialization of Resources from MCC Database
	m_MccReconnection = false;

	RID privateCallRid = NULL_RID;

	DWORD   dwNumRecs;

	m_radioResource.clear();

	// get the list of trunking systems
	//wError = McdbCreateTrunkSysQuery(static_cast<HDB*>(phDb), &hQ, &dwNumRecs);	


	MCC7500SDK::Parameters in_tcq; // talk channel query
	MCC7500SDK::Parameters_var out_tcq;
	MCC7500SDK::UVariant_var result_tcq;

	in_tcq.length(2);
	in_tcq[0].V_UINT32(phDb);
	in_tcq[1].V_UINT32(0);
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MaxRetries : %d, MaxTalkgroupQueryTimeout : %d, MaxRadioUserQueryTimeout : %d",
		m_mdbQueryTimeout.MaxRetries, m_mdbQueryTimeout.TalkGroupQueryTimeout, m_mdbQueryTimeout.RadioUserQueryTimeout);

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MCC Connection status : %d",
		The_StatusManager::instance()->getMccConnectionStatus() );

	//Make sure that the current connection between MCC and RadioManger is OK
	//APIRequest must not be executed if the current connection is not OK
	if ( false == The_StatusManager::instance()->getMccConnectionStatus() )
	{
		TA_THROW(TA_Base_Core::TransactiveException("MCC Connection is not OK"));
	}

	//If the number of records returned by MCC for Talkgroup query is 0
	//then repeat the query based on the number of retries
	for (int retry = 0; retry < m_mdbQueryTimeout.MaxRetries; retry++ )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo," Retries: %d, Calling eMcdbCreateTalkChannelQueryID to MCC",
			           retry+1 );
		//m_pAPIRequest->ApiRequest(eMcdbCreateTalkChannelQueryID,in_tcq,out_tcq,result_tcq);
		recurseMccCall(eMcdbCreateTalkChannelQueryID,in_tcq,out_tcq,result_tcq);

		wError = result_tcq->V_UINT16();
		dwNumRecs = out_tcq[1].V_UINT32();
		hQ = (void*)out_tcq[0].V_UINT32();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "McdbCreateTalkChannelQuery success, Number of Records for Talkgroup : %u",dwNumRecs);

		//Check if the number of records returned by Talkgroup Query is not equal to 0.
		//Else repeat for the Talkgroup Query.
		if ( 0 != dwNumRecs)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Exit retry, Number of records returned is not equal to 0");
			break;
		}

		//Sleep for a while to give time for MCC
		Thread::sleep(m_mdbQueryTimeout.TalkGroupQueryTimeout);
	}
		
	if (wError == MCDB_NO_ERROR)
	{
		WORD wExternalSysId;
		WORD wSystemType;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Getting System resource IDs : %u",dwNumRecs);
		for (; dwNumRecs > 0; --dwNumRecs)
		{	
			RadioResource radioResource;


			//wError = McdbGetNextTrunkSys(hQ, &wInternalSysId, szLabel, &wExternalSysId, &wSystemType);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start of loop");
		

			MCC7500SDK::Parameters in_tc;
			MCC7500SDK::Parameters_var out_tc;
			MCC7500SDK::UVariant_var result_tc;

			in_tc.length(1);
			in_tc[0].V_UINT32((unsigned long)(hQ));

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling eMcdbGetNextTalkChannelID to MCC, Getting talkchannel");

			//m_pAPIRequest->ApiRequest(eMcdbGetNextTalkChannelID,in_tc,out_tc,result_tc);
			recurseMccCall (eMcdbGetNextTalkChannelID,in_tc,out_tc,result_tc);

			wError = result_tc->V_UINT16();
			unsigned long urid = out_tc[0].V_UINT32();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Got talkchannel RESID: %u",urid);
			{
							RadioResource radioResource;
							radioResource.id = urid;
							std::wstring ws = out_tc[1].V_WSTRING();
							string s(ws.begin(), ws.end());
							radioResource.alias=s;
							radioResource.type = getResourceTypeFromLabel(s);
							m_radioResource.push_back(radioResource);
							if (radioResource.type == RadioResourceType::RR_PRIVATECALL) {
								privateCallRid = urid;
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Private Call Resource Found: %u,", urid);
							}
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Adding RadioResource : %u  Alias: %s  Type: %d",radioResource.id,radioResource.alias.c_str(),radioResource.type);
			}
			if (( MCDB_NO_ERROR == wError) && (urid != NULL_RID) )
			{					
				MCC7500SDK::Parameters in_rcq;
				MCC7500SDK::Parameters_var out_rcq;
				MCC7500SDK::UVariant_var result_rcq;
				DWORD rcNumRecs;
				HQ      rchQ;

				in_rcq.length(4);
				in_rcq[0].V_UINT32(phDb);
				in_rcq[1].V_UINT32(urid);
				in_rcq[2].V_UINT16(0);
				in_rcq[3].V_UINT32(0);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling eMcdbCreateResCapabilityIndexQueryID to MCC");
				//m_pAPIRequest->ApiRequest(eMcdbCreateResCapabilityIndexQueryID,in_rcq,out_rcq,result_rcq);
				recurseMccCall(eMcdbCreateResCapabilityIndexQueryID,in_rcq,out_rcq,result_rcq);

				wError = result_rcq->V_UINT16();

				if (MCDB_NO_ERROR == wError) 
				{
					rcNumRecs = out_rcq[1].V_UINT32();
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Getting resource capability %u",rcNumRecs);
					rchQ = (void*)out_rcq[0].V_UINT32();
					//for (; rcNumRecs > 0; --rcNumRecs) {						
						MCC7500SDK::Parameters in_rc;
						MCC7500SDK::Parameters_var out_rc;
						MCC7500SDK::UVariant_var result_rc;

						in_rc.length(1);
						in_rc[0].V_UINT32((unsigned long)(rchQ));			

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbGetNextResCapabilityIndexRecID to MCC");
						
						//m_pAPIRequest->ApiRequest(eMcdbGetNextResCapabilityIndexRecID,in_rc,out_rc,result_rc);
						recurseMccCall(eMcdbGetNextResCapabilityIndexRecID,in_rc,out_rc,result_rc);
						
						wError = result_rc->V_UINT16();

						if ( MCDB_NO_ERROR == wError) {
							radioResource.id = (ResourceIdentifier)out_rc[0].V_UINT16();
							std::wstring ws = out_rc[1].V_WSTRING();
							string s(ws.begin(), ws.end());
							radioResource.alias=s;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before calling radioresource : %s ",s.c_str());
							radioResource.type = getResourceTypeFromLabel(s);
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After calling radioresource");

							if (radioResource.type == RadioResourceType::RR_PRIVATECALL) {
								privateCallRid = urid;
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Private Call Resource Found: %u,", urid);
							}
							if (radioResource.type != RR_NIL)
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Adding RadioResource : %u  Alias: %s  Type: %d",radioResource.id,radioResource.alias.c_str(),radioResource.type);
								m_radioResource.push_back(radioResource);
							}
							else
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid radio resource type: %s", radioResource.alias.c_str());
							}
						} else {	
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error getting resource capability of talk channel id %u", urid);
						}
					//}

					MCC7500SDK::Parameters in_rcd;
					MCC7500SDK::Parameters_var out_rcd;
					MCC7500SDK::UVariant_var result_rcd;
					in_rcd.length(1);
					in_rcd[0].V_UINT32((unsigned long)rchQ);
					
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbDestroyQueryID to MCC");

					//m_pAPIRequest->ApiRequest(eMcdbDestroyQueryID,in_rcd,out_rcd,result_rcd);
					recurseMccCall(eMcdbDestroyQueryID,in_rcd,out_rcd,result_rcd);
				}
				else {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error creating resource capability of talk channel id %u", urid);
				}
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error getting talk channel");
			}
		}

		MCC7500SDK::Parameters in_tcd;
		MCC7500SDK::Parameters_var out_tcd;
		MCC7500SDK::UVariant_var result_tcd;
		in_tcd.length(1);
		in_tcd[0].V_UINT32((unsigned long)(hQ));
		//out->release();
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbDestroyQueryID to MCC");

		//m_pAPIRequest->ApiRequest(eMcdbDestroyQueryID,in_tcd,out_tcd,result_tcd);
		recurseMccCall(eMcdbDestroyQueryID,in_tcd,out_tcd,result_tcd);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Destroy, query.");
	} 
	else 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error creating talk channel query");
	}

	//McdbDestroyQuery(hQ);

	MCC7500SDK::Parameters radioid_q; // radio ID query
	MCC7500SDK::Parameters_var radioid_out;
	MCC7500SDK::UVariant_var radioid_result;

	radioid_q.length(3);
	radioid_q[0].V_UINT32(phDb);
	radioid_q[1].V_UINT32(privateCallRid);
	radioid_q[2].V_UINT32(0);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbCreateUnitAliasListQueryID to MCC");

	DWORD radioid_total;
	HQ radioid_hq;

	//If the number of records returned by MCC for RadioUser query is 0
	//then repeat the query based on the number of retries
	for (int retry = 0; retry < m_mdbQueryTimeout.MaxRetries; retry++ )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retries : %d, Calling createunitaliasListquery API with resourceID=%u(%x)", 
			           retry+1, privateCallRid, privateCallRid );

		//m_pAPIRequest->ApiRequest(eMcdbCreateUnitAliasListQueryID, radioid_q, radioid_out, radioid_result);
		recurseMccCall(eMcdbCreateUnitAliasListQueryID, radioid_q, radioid_out, radioid_result);

		wError = radioid_result->V_UINT16();
		radioid_hq = (void*)radioid_out[0].V_UINT32();
		radioid_total = radioid_out[1].V_UINT32();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Number of Records for Radio User : %u",radioid_total);

		//Check if the number of records returned by RadioUser Query is not equal to 0.
		//Else repeat the RadioUser Query.
		if ( 0 != radioid_total)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Exit retry, Number of records returned is not equal to 0");
			break;
		}

		//Sleep for a while to give time to MCC
		Thread::sleep(m_mdbQueryTimeout.RadioUserQueryTimeout);
	}

	if ( 0 == radioid_total )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Radio User records %d, calling reconnection to MCC", radioid_total);
		
		//Call Reconnection
		m_mcdbObserver->reconnectMCC();
		m_MccReconnection = true;

	}

	if (MCDB_NO_ERROR == wError) {
		for (; radioid_total > 0; --radioid_total) {			
			RadioResource radioResource;


			//wError = McdbGetNextTrunkSys(hQ, &wInternalSysId, szLabel, &wExternalSysId, &wSystemType);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Getting next query of radio id");
		

			MCC7500SDK::Parameters in_tc;
			MCC7500SDK::Parameters_var out_tc;
			MCC7500SDK::UVariant_var result_tc;

			in_tc.length(1);
			in_tc[0].V_UINT32((unsigned long)(radioid_hq));

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Getting talkchannel");

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbGetNextUnitAliasID to MCC");

			//m_pAPIRequest->ApiRequest(eMcdbGetNextUnitAliasID,in_tc,out_tc,result_tc);
			recurseMccCall(eMcdbGetNextUnitAliasID,in_tc,out_tc,result_tc);

			wError = result_tc->V_UINT16();
			unsigned long urid = out_tc[1].V_UINT32();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Got radio RESID: %u",urid);
			if (MCDB_NO_ERROR == wError) {
				radioResource.id = urid;
				std::wstring ws = out_tc[0].V_WSTRING();
				string s(ws.begin(), ws.end());
				radioResource.alias=s;
				radioResource.type = RadioResourceType::RR_RADIO;
				radioResource.ssi = urid;
				m_radioResource.push_back(radioResource);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Adding RadioResource Unit Alias : %u  Alias: %s  Type: %d",radioResource.id,radioResource.alias.c_str(),radioResource.type);
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error getting next Unit Alias query");
			}
		}

		MCC7500SDK::Parameters in_rcd;
		MCC7500SDK::Parameters_var out_rcd;
		MCC7500SDK::UVariant_var result_rcd;
		in_rcd.length(1);
		in_rcd[0].V_UINT32((unsigned long)radioid_hq);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling eMcdbDestroyQueryID to MCC");

		//m_pAPIRequest->ApiRequest(eMcdbDestroyQueryID,in_rcd,out_rcd,result_rcd);
		recurseMccCall(eMcdbDestroyQueryID,in_rcd,out_rcd,result_rcd);

	}
	else 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating Unit Alias query");
	}

    FUNCTION_EXIT;
}


/*** open & close db ***/
bool MDB::openDatabase(unsigned &phDb)
{
	FUNCTION_ENTRY("openDatabase");

    bool bRetValue = false;
    WORD wDbStatus;

    //wDbStatus = McdbOpenDatabase(static_cast<HDB*>(phDb), dbLocation);
	MCC7500SDK::Parameters in;
	MCC7500SDK::Parameters_var out;
	MCC7500SDK::UVariant_var result;

	in.release();	
	//m_pAPIRequest->ApiRequest(eMcdbOpenDatabaseExID,in,out,result);
	recurseMccCall(eMcdbOpenDatabaseExID,in,out,result);
	wDbStatus = result->V_UINT16();
	
    switch(wDbStatus)
    {
        case MCDB_NO_ERROR:
            bRetValue = true;
            break;

        case MCDB_INVALID_DATABASE_LOCATION:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OpenDB err: invalid database location. Status = %d",wDbStatus);
            break;
        case MCDB_COMMUNICATION_FAILURE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OpenDB err: communication failure. Status = %d",wDbStatus);
            break;
        case MCDB_DATABASE_UNAVAILABLE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OpenDB err: database unavailable. Status = %d",wDbStatus);
            break;
        case MCDB_DATABASE_OUT_OF_DATE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OpenDB err: database out of date. Status = %d",wDbStatus);
            break;
        default:
			TA_THROW(TA_Base_Core::TransactiveException("overflow type in wDbStatus!"));
            break;
    }

    FUNCTION_EXIT;
	if(bRetValue)
	{
		phDb=out[0].V_UINT32();
	}
    return bRetValue;
}


bool MDB::closeDatabase(unsigned &phDb)
{
	FUNCTION_ENTRY("closeDatabase");
    bool bRetValue = false;
    WORD wDbStatus;

    //wDbStatus = McdbCloseDatabase(static_cast<HDB*>(phDb));
	MCC7500SDK::Parameters in;
	MCC7500SDK::Parameters_var out;
	MCC7500SDK::UVariant_var result;

	in.release();
	in.length(1);	
	
	in[0].V_UINT32(phDb);
	//out->release();
	//m_pAPIRequest->ApiRequest(eMcdbCloseDatabaseID,in,out,result);
	recurseMccCall(eMcdbCloseDatabaseID,in,out,result);
	wDbStatus=result->V_UINT16();
    switch(wDbStatus)
    {
        case MCDB_NO_ERROR:
            bRetValue = true;
            break;

        case MCDB_INVALID_DATABASE_LOCATION:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OpenDB err: invalid database location");
            break;
        case MCDB_COMMUNICATION_FAILURE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OpenDB err: communication failure");
            break;
        case MCDB_DATABASE_UNAVAILABLE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OpenDB err: database unavailable");
            break;
        case MCDB_DATABASE_OUT_OF_DATE:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OpenDB err: database out of date");
            break;
        default:
            TA_THROW(TA_Base_Core::TransactiveException("overflow type in wDbStatus!"));
            break;
    }
    
	FUNCTION_EXIT;
    return bRetValue;
}


RadioResourceType MDB::getResourceTypeFromLabel(std::string label)
{
    FUNCTION_ENTRY("getResourceTypeFromLabel");

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Resource Label : %s",label.c_str());

    RadioResourceType rsType = RR_NIL;

	// only identify talkgroups now
  //  if (label.find(TALK_GROUP_RES_TYPE) == 0 || label.find(TALK_GROUP_RES_TYPE2) == 0 || 
		//(label.size() > 1 && label.at(0) == 'S'&& label.at(1) >= '0' && label.at(1) <= '9'))
  //  {
  //      rsType = RR_TALKGROUP;
  //  }
  //  else if (label.find(MULTI_GROUP_RES_TYPE) == 0)
  //  {
  //      rsType = RR_ANNGROUP;
  //  }
  //  else 
	if (label.find(PRIVATE_CALL_RES_TYPE) != std::string::npos)
    {
        rsType = RR_PRIVATECALL;
	} else if (label != "") {
		rsType = RR_TALKGROUP;
	}


    //else if (label.find(SITE_WIDE_CALL_RES_TYPE) == 0)
    //{
    //    rsType = RR_SITE;
    //}
    //else if (label.find(DYNAMIC_GROUP_RES_TYPE) == 0)
    //{
    //    // AZ: temporarily make this TG, until we can associate
    //    // the transactive dynamic group with it
    //    //rsType = RR_TALKGROUP;
    //    rsType = RR_DYNGROUP;
    //}
    //else if (label.find(BIM_RES_TYPE) != std::string::npos)
    //{
    //    rsType = RR_BIM;
    //}

    FUNCTION_EXIT;
    return rsType;
}


void MDB::recurseMccCall (const MCC7500SDK::METHOD_ID &id,
						  const MCC7500SDK::Parameters &in,
						  const MCC7500SDK::Parameters_out &out,
						  const MCC7500SDK::UVariant_out &result)
{
	bool bIsSuccess = true;
	int nCallCntr = 1;
	int nCallmax = 1; //Suggested by Bihui

	while (nCallmax >= nCallCntr)
	{
		try
		{

			{
				TA_Base_Core::ThreadGuard guard(m_apiLock);
				if(!CORBA::is_nil(m_pAPIRequest))
				{
					m_pAPIRequest->ApiRequest(id,in,out,result);
				}
				else
				{
					bIsSuccess = false;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ApiRequest is null");
				}
			}	
		}
		
		catch (MCC7500SDK::IAPIRequest::UnknownApiRequest & unknownRequestEx)
		{
			bIsSuccess = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "UnknownApiRequest Exception encountered");
			//TA_Base_Core::Thread::sleep(3000); //Suggested by Bihui
		}
		catch (MCC7500SDK::IAPIRequest::ParameterTypeMismatch & paramMisMatch)
		{
			bIsSuccess = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ParameterTypeMismatch Exception encountered");
			//TA_Base_Core::Thread::sleep(3000); //Suggested by Bihui
		}
		catch (MCC7500SDK::IAPIRequest::ParameterCountMismatch & paramCountMisMatch)
		{
			bIsSuccess = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ParameterCountMismatch Exception encountered");
			//TA_Base_Core::Thread::sleep(3000); //Suggested by Bihui
		}
		catch (MCC7500SDK::IAPIRequest::ApiRequestInternalFail & apiInternalFail)
		{
			bIsSuccess = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ApiRequestInternalFail Exception encountered");
			//TA_Base_Core::Thread::sleep(3000); //Suggested by Bihui
		}
		catch(...)
		{
			bIsSuccess = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unknown Exception encountered");
			//TA_Base_Core::Thread::sleep(3000); //Suggested by Bihui
		}
		
		if (bIsSuccess)
		{
			break;
		}
		nCallCntr++;
	}
	
	// if the call is unsucccessful
	// throw awn exception
	if (!bIsSuccess)
	{
		std::string strEx ("Cannot perform ApiRequest at this time");
		throw strEx;
	}
}