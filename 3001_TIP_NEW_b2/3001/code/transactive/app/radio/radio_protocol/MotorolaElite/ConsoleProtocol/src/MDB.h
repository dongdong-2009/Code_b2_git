/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/MDB.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  * 1: hided the inlcude <windows.h>
  * 2: added the type definition.
  * 
  */

#if !defined(AFX_MDB_H__E86ADD86_A6D6_44D2_BA08_003F1042E885__INCLUDED_)
#define AFX_MDB_H__E86ADD86_A6D6_44D2_BA08_003F1042E885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>


#include "core/threads/src/Thread.h"

#include "bus/radio/radio_bus/src/radiotypes.h"


#include "app/radio/radio_mft_agent/src/RemoteAPI.h"

#include "app\radio\radio_mft_agent\src\RadioMFTAgentRemoteAPICommandImpl.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


class IMcdbObserver;

class MDB : public TA_Base_Core::Thread
{
public:

	struct MDBQueryTimeout
	{
		int TalkGroupQueryTimeout;
		int RadioUserQueryTimeout;
		int DatabaseMCCQueryTimeout;
		int MaxRetries;
	};

	MDB(IMcdbObserver* callback, MCC7500SDK::IAPIRequest_ptr& ptr, TA_Base_Core::ReEntrantThreadLockable & apiLock, MDBQueryTimeout mdbTimeout);
	virtual ~MDB();


      /** 
      * run
	  *
	  * description: 
		The InitializeResourceDatabase thread is a temporary thread that is created to read the resource information 
		from the local Resource Configuration database and then is terminated.  Reading the resource information is a 
		relatively slow process so it is done in a separate thread to prevent the UI from becoming unresponsive or 
		the ApiMsgThread from ignoring messages and requests.
		The resource database should be read once when the application is started, before the application registers 
		with the CDI, and again whenever a database synchronization complete message is received 
    */
    void run();

    /** 
      * terminate
      *
      * description
      *
      */
    void terminate();

 
    /** 
      * getRadioResource
      *
      * description: to get the Radio resource from 
	  *
      * @param radioResource [out] the results are passed out 
      *
      * @return true, can get the right resource, otherwise can not get the resource.
      */
    bool getRadioResource(std::vector<RadioResource>& radioResource);

private:
    // Motorola API specific

    /** 
      * initalizeResourceDatabase
      *
      * description
      *
      */
    void initalizeResourceDatabase();

    /** 
      * initResourceList
      *
      * description
      *
      * @param phDb
      */
    void initResourceList(unsigned &phDb);

    /** 
      * openDatabase 
      *
      * description  to open the hardware information database
      *
      * @param phDb --  the database object 
      * @param dbLocation -- database location
      *
      * @return true, sucessfully opened the database,otherwise can not open the pointed database
      *
      */
    bool openDatabase(unsigned &phDb);

    /** 
      * closeDatabase
      *
      * description to close the hardware information database
      *
      * @param phDb -- the database to be closed
      *
      * @return -- true, the database is closed sucessfully,otherwise can not close the database.
      */
    bool closeDatabase(unsigned &phDb);

    /** 
      * getResourceTypeFromLabel
      *
      * description to get the resource type via label name
      *
      * @param label -- the resource type name
      *
      * @return -- the rediao resource type
      */
    RadioResourceType getResourceTypeFromLabel(std::string label);

    //RadioResourceType getResourceTypeFromId(unsigned long pId);

private:
    // disabled methods
    MDB();
    MDB(const MDB&);
    void operator=(const MDB&);

    //HQ  hQuery;

    std::vector<RadioResource>  m_radioResource;

    IMcdbObserver*  m_mcdbObserver;
    bool        m_isInitializing;
	MCC7500SDK::IAPIRequest_ptr &m_pAPIRequest;
	TA_Base_Core::ReEntrantThreadLockable &m_apiLock;

    static const std::string TALK_GROUP_RES_TYPE;
    static const std::string MULTI_GROUP_RES_TYPE;
    static const std::string PRIVATE_CALL_RES_TYPE;
    static const std::string SITE_WIDE_CALL_RES_TYPE;
    static const std::string DYNAMIC_GROUP_RES_TYPE;
    static const std::string BIM_RES_TYPE;
	static const std::string TALK_GROUP_RES_TYPE2;

    static const unsigned long MIN_TALKGROUP_ID;
    static const unsigned long MAX_TALKGROUP_ID;
    static const unsigned long MIN_MULTIGROUP_ID;
    static const unsigned long MAX_MULTIGROUP_ID;
    static const unsigned long MIN_SITEWIDE_ID;
    static const unsigned long MAX_SITEWIDE_ID;
	static const unsigned long MIN_DYNAMICGROUP_ID;
    static const unsigned long MAX_DYNAMICGROUP_ID;


	void recurseMccCall (const MCC7500SDK::METHOD_ID &id,
						 const MCC7500SDK::Parameters &in,
						 const MCC7500SDK::Parameters_out &out,
						 const MCC7500SDK::UVariant_out &result);

	MDBQueryTimeout m_mdbQueryTimeout;

	bool m_MccReconnection;


};

#endif // !defined(AFX_MDB_H__E86ADD86_A6D6_44D2_BA08_003F1042E885__INCLUDED_)
