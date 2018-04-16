/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSessionHelper.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Implementation of the RadioSessionHelper class
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"


// Forward declaration

using namespace TA_IRS_App;
using TA_Base_Core::DebugUtil;

RadioSessionHelper::RadioSessionHelper(SessionReference sessionRef, RadioTcpServer & radioSvr)
: m_radioSvr(radioSvr) 
, m_sessionRef(sessionRef)
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}


SessionReference RadioSessionHelper::getSessionRef()
{
	FUNCTION_ENTRY("getSessionRef()");

	FUNCTION_EXIT;
	return m_sessionRef;
}


RadioEvent * RadioSessionHelper::sendMethod(RadioMethod & method)
{
	FUNCTION_ENTRY("sendMethod()");

	RadioEvent * event = m_radioSvr.sendMethod(method);


	// Check for an error
	if (event->getStatus()<0)
	{
		// Throw a radio session error
		TA_THROW(RadioSessionException(event->getTextStatus(),event->getStatus()));
	}

	FUNCTION_EXIT;
	return event;
}


void RadioSessionHelper::deleteReference(RadioReference reference,RadioMethodDeleteReference::ReferenceType refType)

{
    FUNCTION_ENTRY("deleteReference()");

    RadioMethodDeleteReference deleteReferenceMethod(getSessionRef());
    deleteReferenceMethod.setReference(refType,reference);
    RadioEvent* reply = sendMethod(deleteReferenceMethod);
    
    // The reply is for error checking only.
    //DeleteReferenceEventReader deleteReferenceReader(*reply);
    //if (deleteReferenceReader.getReference() != reference)
    //{
    //    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Reply to delete reference was incorrect. Expected %lu, got %lu",
    //        reference,deleteReferenceReader.getReference());
    //}

	delete reply;

    FUNCTION_EXIT;
}


std::string RadioSessionHelper::queryReference(RadioReference queryRef, RadioMethodQueryReference::EKeys key)

{
	FUNCTION_ENTRY("queryReference()");

	// Need to perform a query reference on the operator's session reference.
	RadioMethodQueryReference queryReferenceMethod(getSessionRef());
    queryReferenceMethod.setQueryType(key);
	queryReferenceMethod.setQueryReference(queryRef);
	RadioEvent* reply = sendMethod(queryReferenceMethod);

    // The reply should have the ITSI for the current session
    QueryReferenceEventReader queryReferencereader(*reply);
    std::string resultText = queryReferencereader.getDetails();

    // Delete the reply
    delete reply;
    reply = NULL;

	FUNCTION_EXIT;
	return resultText;
}


/*
 * Query the details from a reference.
 */
std::vector<std::string> RadioSessionHelper::queryReferenceAsList
(   RadioReference queryRef,
    RadioMethodQueryReference::EKeys key,
    char delimiter)
{
	FUNCTION_ENTRY("queryReferenceAsList(queryRef, key, delimiter)");

	// Need to perform a query reference on the operator's session reference.
	RadioMethodQueryReference queryReferenceMethod(getSessionRef());
    queryReferenceMethod.setQueryType(key);
	queryReferenceMethod.setQueryReference(queryRef);
	RadioEvent* reply = sendMethod(queryReferenceMethod);

    // The reply should have the ITSI for the current session
    QueryReferenceEventReader queryReferencereader(*reply);
    std::vector<std::string> resultList = queryReferencereader.getDetailsList(delimiter);

    // Delete the reply
    delete reply;
    reply = NULL;

	FUNCTION_EXIT;
	return resultList;
}

void RadioSessionHelper::queryReferenceAsMap(RadioReference queryRef, RadioMethodQueryReference::EKeys key, QueryKeyValueMap & queryMap)
{
    FUNCTION_ENTRY("getReferenceAsMap(queryRef, )");

    // Need to perform a query reference on the operator's session reference.
    RadioMethodQueryReference queryReferenceMethod(getSessionRef());
    queryReferenceMethod.setQueryType(key);
    queryReferenceMethod.setQueryReference(queryRef);
    RadioEvent * reply = sendMethod(queryReferenceMethod);

    // The reply should have the ITSI for the current session
    QueryReferenceEventReader queryReferencereader(*reply);
    /* int numValues = */ queryReferencereader.getDetailsKeyValueMap(queryMap);

    // Delete the reply
    delete reply;

    FUNCTION_EXIT;
}

SubscriberReference RadioSessionHelper::getSubscriberReference(const std::string & tsi)
{
    FUNCTION_ENTRY("getSubscriberReference(TSI)");
    SubscriberReference subscriber_ref = 0;
	// only use a "converted" tsi because the search here fails for tsi's in "short" form, i.e. without site suffix
	std::string tsiWithSiteSuffix = getTsiWithSuffix(tsi);

    std::string searchCriteria = "TSI='" + tsiWithSiteSuffix + "'";
    SearchResultReference searchResultRef = performSubscriberSearch(searchCriteria, RadioMethodSearchSubscribers::NORMAL);

    // Step 2. get subscriber reference as [RCS Proprietry Internal Format] text
    std::string subscriberTextRef = queryReference(searchResultRef, RadioMethodQueryReference::K_MEMBERS);

    // Before using this text reference need to delete the previous search reference
    deleteReference(searchResultRef,RadioMethodDeleteReference::SEARCH_REFERENCE);

    // if (subscriberTextRef.size() <= 0)
    if (subscriberTextRef.empty())
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve subscriber text reference");
        FUNCTION_EXIT;
        return 0;
    }
    // Step 3. convert text to [numerical] subscriber reference
    subscriber_ref = convertTextToSubscriberReference(subscriberTextRef);

    FUNCTION_EXIT;
    return subscriber_ref;
}

std::string RadioSessionHelper::getSubscriberName(const std::string & tsi)
{
    FUNCTION_ENTRY("getSubscriberName(TSI)");

    checkRadioFallback();

    std::string subscriberName = "";
    QueryKeyValueMap subscriberDetails;
    subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
    int num_details = getSubscriberDetails(tsi, subscriberDetails);
    if ( num_details == 1 )
    {
        subscriberName = subscriberDetails[(unsigned long)RadioMethodQueryReference::K_NAME];
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Subscriber details, K_NAME:%s", subscriberName.c_str());
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve Calling subscriber details");
    }


    FUNCTION_EXIT;
    return subscriberName;
}


int RadioSessionHelper::getCallDetails(CallID callId, QueryKeyValueMap & queryResults)
{
    FUNCTION_ENTRY("getCallDetails(callId, queryResults)");
    int successful_queries = 0;

    if (!queryResults.empty())
    {
        // Perform queries with the CallReference
        for (QueryKeyValueMap::iterator iter = queryResults.begin() ;
             iter != queryResults.end();
             iter++
            )
        {
            unsigned long queryKey = (*iter).first;
            try
            {
                std::string queryResult = queryReference(callId, RadioMethodQueryReference::EKeys(queryKey));

                if (!queryResult.empty())
                {
                    queryResults[queryKey] = queryResult;
                    successful_queries++;
                }
            }
            catch (RadioSessionException & rse)
            {
                // we don't want one failed QueryReference to stop all
                // the rest of the details from being retreived
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                             "RadioSessionException Caught for QueryReference of CallID=%lu QueryKey=%lu: %s",
                             callId, queryKey, rse.what());
            }
        }
    }

    FUNCTION_EXIT;
    return successful_queries;
}


int RadioSessionHelper::getSubscriberDetails(SubscriberReference subscriberRef, QueryKeyValueMap & queryMap)
{
    FUNCTION_ENTRY("getSubscriberDetails(subscriberRef, queryMap)");

    checkRadioFallback();

    int successful_queries = 0;

    if ((!queryMap.empty()) && (subscriberRef>0))
    {
        // Perform queries with subscriber reference
        for (QueryKeyValueMap::iterator iter = queryMap.begin() ;
             iter != queryMap.end();
             iter++
            )
        {
            unsigned long queryKey = (*iter).first;
            std::string queryResult = queryReference(subscriberRef, RadioMethodQueryReference::EKeys(queryKey));
            if (!queryResult.empty())
            {
                queryMap[queryKey] = queryResult;
                successful_queries++;
            }
        }
    }

    FUNCTION_EXIT;
    return successful_queries;
}


int RadioSessionHelper::getSubscriberDetails(const std::string & tsi, QueryKeyValueMap & queryMap)
{
    FUNCTION_ENTRY("getSubscriberDetails(TSI)");
    int successful_queries = 0;

    checkRadioFallback();

    if ((!queryMap.empty()) && (!tsi.empty()))
    {

        // Step 1. get search reference for subscriber details
        std::string subscriberName = "";
        std::string searchCriteria = "TSI='" + tsi + "'";
        unsigned long searchResultRef = performSubscriberSearch("TSI='" + tsi + "'", RadioMethodSearchSubscribers::NORMAL);

        // Step 2. get subscriber reference as [RCS Proprietry Internal Format] text
        std::string subscriberTextRef = queryReference(searchResultRef, RadioMethodQueryReference::K_MEMBERS);

        // Before using this text reference need to delete the previous search reference
        deleteReference(searchResultRef,RadioMethodDeleteReference::SEARCH_REFERENCE);

        // if (subscriberTextRef.size() <= 0)
        if (subscriberTextRef.empty())
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve subscriber text reference");
            FUNCTION_EXIT;
            return 0;
        }
        // Step 3. convert text to [numerical] subscriber reference
        unsigned long subscriberRef = convertTextToSubscriberReference(subscriberTextRef);

        // Step 4. perform queries with subscriber reference
        successful_queries = getSubscriberDetails(subscriberRef, queryMap);
    }

    FUNCTION_EXIT;
    return successful_queries;
}


SubscriberReference RadioSessionHelper::convertTextToSubscriberReference(std::string textReference)
{
	FUNCTION_ENTRY("convertTextToSubscriberReference(textReference)");

    checkRadioFallback();

    // Now convert the text reference to a radio reference so we can use it in a QueryReference
    RadioMethodText2SubscriberReference text2SubRef(getSessionRef());
    text2SubRef.setTextReference(textReference);
    RadioEvent* reply = sendMethod(text2SubRef);

    // The reply will contain the subscriber reference
    Text2SubscriberReferenceEventReader* text2SubRefReader = dynamic_cast<Text2SubscriberReferenceEventReader*>(reply->getEventReader());
    SubscriberReference subReference = text2SubRefReader->getSubscriberReference();

    // Delete the reply
    delete reply;
    reply = NULL;

	FUNCTION_EXIT;
	return subReference;
}


SearchResultReference RadioSessionHelper::performSubscriberSearch
(   std::string searchCriteria,
    RadioMethodSearchSubscribers::EView view /* =NORMAL*/)
{
	FUNCTION_ENTRY("performSubscriberSearch(searchCriteria, view)");

    checkRadioFallback();

    // Search for the groups to be patched
    RadioMethodSearchSubscribers searchSubscriberMethod(getSessionRef());
    searchSubscriberMethod.setSearchType(view);
    searchSubscriberMethod.setCriteria(searchCriteria);
    RadioEvent* reply = sendMethod(searchSubscriberMethod);

    // The reply will be a reference to a result set (i.e. not the actual results we want)
    SearchSubscribersEventReader searchSubscriberReader(*reply);
    SearchResultReference searchResultSet = searchSubscriberReader.getSubscriberReferenceResultSet();

    // Delete the reply event
    delete reply;

	FUNCTION_EXIT;
	return searchResultSet;
}

bool RadioSessionHelper::isPresent(const std::string & text, std::vector<std::string> textList)
{
	FUNCTION_ENTRY("isPresent(text, textList)");
	for (unsigned int iLoop=0;iLoop<textList.size();iLoop++)
	{
		if (text==textList.at(iLoop)) return true;
	}

	return false;
}


ECallType RadioSessionHelper::convert2CallType(const std::string & callTypeStr)
{
    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"convert2CallType(\"%s\")", callTypeStr.c_str());

	std::string realstr;
	char cc;
	// Make uppercase and remove spaces
	for ( std::string::const_iterator siter=callTypeStr.begin() ;
		  siter != callTypeStr.end() ;
		  siter++
		)
	{
		cc = *siter;
		if (( cc >= 'a' ) && ( cc <= 'z' ))
		{
			realstr.append(1,cc-0x20);
		}
		else if (cc==' ')
		{
			continue;
		}
		else
		{
			realstr.append(1,cc);
		}
	}

    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"convert2CallType() actual string for comparison \"%s\"", realstr.c_str());

	if (realstr=="INDIVIDUAL")
	{
		return SingleCall;
    }
    else if (realstr=="GROUPCALLUNACK")
    {
        return GroupUnacknowledgeCall;
    }
    else if (realstr=="GROUPCALLACK")
    {
        return GroupAcknowledgeCall;
    }
    else if (realstr=="GROUPCALLBROADCAST")
    {
        return GroupBroadcastCall;
    }
    // else
    // TODO raise an exception of some sort
    // FOr the time being, return individual
    return SingleCall;
}


int RadioSessionHelper::getGroupMemberReferences(const std::string & gtsi, std::vector<std::string> & groupMembers)
{
    checkRadioFallback();

	// Query current subscribers in dgna
	unsigned long groupResultRef = performSubscriberSearch("PARENT='" + gtsi + "'", RadioMethodSearchSubscribers::K_MEMBERS);
	groupMembers = queryReferenceAsList(groupResultRef,RadioMethodQueryReference::K_MEMBERS);
	deleteReference(groupResultRef,RadioMethodDeleteReference::SEARCH_REFERENCE);

    return groupMembers.size();
}


int RadioSessionHelper::populateCallParticipantData(const std::string & gtsi, TA_IRS_App::CallDetailsType * details, ECallType callType )
{
    FUNCTION_ENTRY("populateCallParticipantData(const std::string &, TA_IRS_App::CallDetailsType *, ECallType)");

    checkRadioFallback();

    std::vector<std::string> groupMemberRefs;
    int numParticipants = getGroupMemberReferences(gtsi, groupMemberRefs);

    QueryKeyValueMap participantDetails;

    //
    // Just in case the caller wasn't part of the group...
    std::string callerTSI = details->callerTSI;
    bool callerIsParticipant = false;
    bool isAckCall = (callType==GroupAcknowledgeCall);

    for ( int pNum = 0; pNum < numParticipants ; pNum++ )
    {
        std::string subscriberTextRef = groupMemberRefs[pNum];
        long int subscriberRef = convertTextToSubscriberReference(subscriberTextRef);

        participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
        participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION] = "";
        participantDetails[(unsigned long)RadioMethodQueryReference::K_TSI] = "";
        int num_details = getSubscriberDetails(subscriberRef, participantDetails);

        if ( num_details == 3 )
        {
            CallParticipant participant;
            participant.name            = participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME];
            participant.TSI             = participantDetails[(unsigned long)RadioMethodQueryReference::K_TSI];
            participant.location        = participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION];
            participant.isAcknowledged  = isAckCall;
            participant.pttState        = PTT_CEASE;

            details->participants.push_back(participant);

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, name:%s TSI:%s location:%s", details->participants[pNum].name.c_str(), details->participants[pNum].TSI.c_str(), details->participants[pNum].location.c_str());

            if (callerTSI == participant.TSI)
            {
                callerIsParticipant = true;
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve group subscriber details subscriberTextRef=%s subscriberRef=0x%X", subscriberTextRef.c_str(), subscriberRef);
        }
    }

    //
    // Add the caller as a participant if they weren't in the called group
    if (!callerIsParticipant)
    {
        CallParticipant participant;
        participant.name            = details->callerName;
        participant.TSI             = details->callerTSI;
        participant.location        = details->callerLocation;
        participant.isAcknowledged  = isAckCall;
        participant.pttState        = PTT_CEASE;

        details->participants.push_back(participant);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, name:%s TSI:%s location:%s", participant.name.c_str(), participant.TSI.c_str(), participant.location.c_str());

        numParticipants++;
    }
    

    FUNCTION_EXIT;
    return numParticipants;
}


int RadioSessionHelper::populateCallParticipantData(const std::string & gtsi, TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * details )
{
    FUNCTION_ENTRY("populateCallParticipantData( const std::string & , TA_IRS_Bus::IRadioCorbaDef::MonitoredCallDetailsType * )");

    checkRadioFallback();

    std::vector<std::string> groupMemberRefs;
    int numParticipants = getGroupMemberReferences(gtsi, groupMemberRefs);

    details->participants.length(numParticipants);
    QueryKeyValueMap participantDetails;

    //
    // Just in case the caller wasn't part of the group...
    char * detailsCallerTSI = details->callerTSI;
    std::string callerTSI = detailsCallerTSI;
    bool callerIsParticipant = false;

    for ( int pNum = 0; pNum < numParticipants ; pNum++ )
    {
        std::string subscriberTextRef = groupMemberRefs[pNum];
        long int subscriberRef = convertTextToSubscriberReference(subscriberTextRef);

        participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
        participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION] = "";
        participantDetails[(unsigned long)RadioMethodQueryReference::K_TSI] = "";
        int num_details = getSubscriberDetails(subscriberRef, participantDetails);
        if ( num_details == 3 )
        {
            details->participants[pNum].name            = CORBA::string_dup((participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME]).c_str());
            details->participants[pNum].TSI             = CORBA::string_dup((participantDetails[(unsigned long)RadioMethodQueryReference::K_TSI]).c_str());
            details->participants[pNum].location        = CORBA::string_dup((participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION]).c_str());
            details->participants[pNum].isAcknowledged  = false;
            details->participants[pNum].pttState        = TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;

            char * zz_participant_name       = details->participants[pNum].name;
            char * zz_participant_tsi        = details->participants[pNum].TSI;
            char * zz_participant_location   = details->participants[pNum].location;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, name:%s TSI:%s location:%s", zz_participant_name, zz_participant_tsi, zz_participant_location);

            if (callerTSI == zz_participant_tsi)
            {
                callerIsParticipant = true;
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve group subscriber details subscriberTextRef=%s subscriberRef=0x%X", subscriberTextRef.c_str(), subscriberRef);
        }
    }

    //
    // Add the caller as a participant if they weren't in the called group
    if (!callerIsParticipant)
    {
        details->participants.length(numParticipants+1);
        details->participants[numParticipants].name            = CORBA::string_dup(details->callerName);
        details->participants[numParticipants].TSI             = CORBA::string_dup(details->callerTSI);
        details->participants[numParticipants].location        = CORBA::string_dup(details->callerLocation);
        details->participants[numParticipants].isAcknowledged  = false;
        details->participants[numParticipants].pttState        = TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;

        numParticipants++;

		char * zz_participant_name       = details->participants[numParticipants].name;
		char * zz_participant_tsi        = details->participants[numParticipants].TSI;
		char * zz_participant_location   = details->participants[numParticipants].location;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, name:%s TSI:%s location:%s", zz_participant_name, zz_participant_tsi, zz_participant_location);
    }
    

    FUNCTION_EXIT;
    return numParticipants;
}

int RadioSessionHelper::populateCallParticipantData(const std::vector<std::string> & tsiVector, TA_IRS_App::CallDetailsType * details)
{
    FUNCTION_ENTRY("populateCallParticipantData(const std::vector<std::string> & , TA_IRS_App::CallDetailsType * )");

    checkRadioFallback();

    std::vector<long int> subscriberRefs;
    std::string searchText = "";
    //
    // Get actual subscriber references for each tsi
    for ( std::vector<std::string>::const_iterator tsiter = tsiVector.begin() ;
          tsiter != tsiVector.end() ;
          tsiter++ )
    {
        if (tsiter!=tsiVector.begin())
        {
            searchText.append(" OR ");
        }
        searchText += "TSI='" + *tsiter + "'";
    }
	unsigned long referenceResultSet = performSubscriberSearch(searchText);
	std::vector<std::string> groupCallAckRefs = queryReferenceAsList(referenceResultSet,RadioMethodQueryReference::K_MEMBERS);
	deleteReference(referenceResultSet,RadioMethodDeleteReference::SEARCH_REFERENCE);

    QueryKeyValueMap participantDetails;

    int numParticipants = groupCallAckRefs.size();

    for ( int pNum = 0; pNum < numParticipants ; pNum++ )
    {
        std::string subscriberTextRef = groupCallAckRefs[pNum];
        long int subscriberRef = convertTextToSubscriberReference(subscriberTextRef);

        participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME] = "";
        participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION] = "";
        unsigned int num_details = getSubscriberDetails(subscriberRef, participantDetails);

        if ( num_details == participantDetails.size() )
        {
            CallParticipant participant;
            participant.name            = participantDetails[(unsigned long)RadioMethodQueryReference::K_NAME];
            participant.TSI             = tsiVector[pNum];
            participant.location        = participantDetails[(unsigned long)RadioMethodQueryReference::K_ORGANISATION];
            participant.isAcknowledged  = true;
            participant.pttState        = PTT_CEASE;

            details->participants.push_back(participant);

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, name:%s TSI:%s location:%s", details->participants[pNum].name.c_str(), details->participants[pNum].TSI.c_str(), details->participants[pNum].location.c_str());
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve group subscriber details subscriberTextRef=%s subscriberRef=0x%X", subscriberTextRef.c_str(), subscriberRef);
        }
    }

    return numParticipants;
}

void RadioSessionHelper::populateMonitoredCallDetails(MonitoredCallDetailsType * monitoredCallDetails)
{
    FUNCTION_ENTRY("populateMonitoredCallDetails");

    QueryKeyValueMap callValues;
    queryReferenceAsMap( monitoredCallDetails->callID, RadioMethodQueryReference::K_KEY_ALL, callValues);

    // CALL CONNECT TIME
    monitoredCallDetails->callTime = callValues[RadioMethodQueryReference::K_CALLCONNECT_TIME];

    // CALL CATEGORY
    if (RadioHelper::isOTETrue(callValues[RadioMethodQueryReference::K_FULL_DUPLEX]))
    {
        monitoredCallDetails->callCategory = TA_IRS_App::FullDuplex; // ECallCategory
    }
    else // all other call types are assumed half duplex
    {
        monitoredCallDetails->callCategory = TA_IRS_App::HalfDuplex; // ECallCategory
    }

    // CALL TYPE
    monitoredCallDetails->callType = convert2CallType(callValues[RadioMethodQueryReference::K_CALL_TYPE]);

    // EMERGENCY
    std::string priority_str = callValues[RadioMethodQueryReference::K_PRIORITY];
    int priority = atoi(priority_str.c_str());
    monitoredCallDetails->isEmergency = (priority >= 15); // TODO: get global entity param DefaultEmergencyCallPriority;

    //
    // Get the two TSIs
    monitoredCallDetails->callerTSI = callValues[RadioMethodQueryReference::K_CALLING];
    monitoredCallDetails->calledTSI = callValues[RadioMethodQueryReference::K_CALLED];

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"Monitored Call %lu Details, Time %s, Category %s, Type %d, Caller %s, Called %s",
			monitoredCallDetails->callID,
			monitoredCallDetails->callTime.c_str(),
			((FullDuplex==monitoredCallDetails->callCategory) ? "FullDuplex" : "HalfDuplex"),
			(int)(monitoredCallDetails->callType),
			monitoredCallDetails->callerTSI.c_str(),
			monitoredCallDetails->calledTSI.c_str());


    FUNCTION_EXIT;
}

void RadioSessionHelper::populateMonitoredCallSubscriberDetails(MonitoredCallDetailsType * monitoredCallDetails)
{
    FUNCTION_ENTRY("populateMonitoredCallSubscriberDetails");
    try
    {
        //
        // CALLING/CALLER Subscriber monitoredCallDetails
        SubscriberReference callerRef = getSubscriberReference(monitoredCallDetails->callerTSI);
        monitoredCallDetails->callerName = queryReference(callerRef, RadioMethodQueryReference::K_NAME);
        monitoredCallDetails->callerLocation = queryReference(callerRef, RadioMethodQueryReference::K_ORGANISATION);

        if (monitoredCallDetails->callerName.empty())
        {
            monitoredCallDetails->callerName = monitoredCallDetails->callerTSI;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Call %lu calling party details. Name '%s' TSI '%s' Location '%s'",
                    monitoredCallDetails->callID,
                    monitoredCallDetails->callerName.c_str(),
                    monitoredCallDetails->callerTSI.c_str(),
                    monitoredCallDetails->callerLocation.c_str());


        //
        // CALLED Subscriber monitoredCallDetails
        SubscriberReference calledRef = getSubscriberReference(monitoredCallDetails->calledTSI);
        monitoredCallDetails->calledName = queryReference(calledRef, RadioMethodQueryReference::K_NAME);
        monitoredCallDetails->calledLocation = queryReference(calledRef, RadioMethodQueryReference::K_ORGANISATION);

        if (monitoredCallDetails->calledName.empty())
        {
            monitoredCallDetails->calledName = monitoredCallDetails->calledTSI;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Call %lu called party details. Name '%s' TSI '%s' Location '%s'",
                    monitoredCallDetails->callID,
                    monitoredCallDetails->calledName.c_str(),
                    monitoredCallDetails->calledTSI.c_str(),
                    monitoredCallDetails->calledLocation.c_str());

    }
    catch (RadioFallbackException rfe)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

        std::string replacement = getFallbackReplacementString();
        monitoredCallDetails->callerName = replacement;
        monitoredCallDetails->callerLocation = replacement;
        monitoredCallDetails->calledName = replacement;
        monitoredCallDetails->calledLocation = replacement;
    }

    FUNCTION_EXIT;
}

void RadioSessionHelper::checkRadioFallback() throw(TA_IRS_App::RadioFallbackException)
{
    if (m_radioSvr.inFallback())
    {
        TA_THROW(RadioFallbackException());
    }
}

std::string RadioSessionHelper::getFallbackReplacementString() const
{
    return m_radioSvr.getFallbackReplacementString();
}

/**
 * getTsiWithSuffix
 *
 * Converts the provided Tsi into a Tsi with site suffix if the tsi is in "short" format, i.e. without site suffix
 *
 * @param tsiToConvert		The Tsi that is to be converted
 * @return					the converted Tsi
 */
std::string RadioSessionHelper::getTsiWithSuffix(const std::string& tsiToConvert)
{
	FUNCTION_ENTRY("getTsiWithSuffix(TSI)");

	std::string tsiSuffixStr = TA_Base_Core::RunParams::getInstance().get("TsiSuffix");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithSuffix: tsiToConvert=%s, tsi-suffix=%s",
		tsiToConvert.c_str(), tsiSuffixStr.c_str());

	std::string tsiWithSuffix = tsiToConvert;

	std::string::size_type idx = tsiToConvert.find('-');
	if(idx == std::string::npos) // No suffix is found, append it
	{
		tsiWithSuffix = tsiToConvert;
		tsiWithSuffix += tsiSuffixStr;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithSuffix: tsi %s converted, tsi-suffix=%s, tsiWithSuffix=%s", 
			tsiToConvert.c_str(), tsiSuffixStr.c_str(), tsiWithSuffix.c_str());
	}
	else // Assume that the site suffix exist
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithSuffix: No need to convert: %s", tsiToConvert.c_str());
	}

	FUNCTION_EXIT;
	return tsiWithSuffix;
}

/**
 * getTsiWithoutSuffix
 *
 * Converts the provided Tsi into a Tsi without site suffix.
 * If the tsi is already in "short" form, i.e. without suffix, no conversion is done.
 * If the tsi is with suffix, the suffix is stripped.
 *
 * @param tsiToConvert		The Tsi that is to be converted
 * @return					the converted Tsi
 */
std::string RadioSessionHelper::getTsiWithoutSuffix(const std::string& tsiToConvert)
{
	FUNCTION_ENTRY("getTsiWithSuffix(TSI)");

	std::string tsiSuffixStr = TA_Base_Core::RunParams::getInstance().get("TsiSuffix");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithSuffix: tsiToConvert=%s, tsi-suffix=%s",
		tsiToConvert.c_str(), tsiSuffixStr.c_str());

	std::string tsiWithoutSuffix = tsiToConvert;

	std::string::size_type idx = tsiToConvert.find('-');
	if(idx == std::string::npos) // No suffix is found, nothing to do
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithSuffix: No need to convert: %s", tsiToConvert.c_str());
	}
	else // Suffix exist, strip it from the string
	{
		tsiWithoutSuffix = tsiToConvert.substr(0,idx);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTsiWithoutSuffix: tsi %s converted, tsi-suffix=%s, tsiWithoutSuffix=%s", 
			tsiToConvert.c_str(), tsiSuffixStr.c_str(), tsiWithoutSuffix.c_str());
	}

	FUNCTION_EXIT;
	return tsiWithoutSuffix;
}

