/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/TimeoutCallback.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Manage timeout so caller doesn't have to.
 */

#include "app/radio/radio_mft_agent/src/TimeoutCallback.h"
#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "core/utilities/src/DebugUtil.h"

#include <iostream>
#include <sstream>

// !for debugging only !!!!!!
//#include "app/radio/radio_manager_gui/src/DebugMsg.h"


TimeoutCallback::TimeoutCallback(const ACE_Time_Value& timeout) : m_timeoutTime(timeout) 
{
}

TimeoutCallback::~TimeoutCallback()
{
}

void TimeoutCallback::setFuture(ACE_Future<FutureResultT>& future)
{
	m_future = future;
}

int TimeoutCallback::handle_timeout(const ACE_Time_Value &current_time,const void *arg)
{
    FUNCTION_ENTRY("handle_timeout");
    
	FutureResultT future;
	m_future.get(future);

	// set the future's TransactionState as timedout
	future.m_exceptionIndicator = FutureResultT::TIMEDOUT;

	m_future.set(future);

	std::ostringstream logStr;
	logStr << "Command [" << commandToString(future.get()) << future.get()
           << " id=" << future.m_resourceId << "] has timedout.";
    //AddDebugMessage(logStr.str());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,logStr.str().c_str());

    RadioResource res;
    res.id = future.m_resourceId;
    res.type = (RadioResourceType) future.m_resourceType;

    unsigned int resultT = future.get();
    
    switch(resultT)
    {
    case BEGIN_EMERGENCY_CALL:
        //AddErrorMessage("Begin emergency call has timedout");
        break;

    case ASSIGN_RESOURCE:
        if (res.type == RR_SITE) // timeout in assigning the site
        {
            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(res, NAG);
        }
        break;

    case GET_FREE_MSEL:
        if (res.type == RR_MSITE || res.type == RR_MSELECT)
        {
            //timeout in getting free msel
            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(res, NAG);
            The_StatusManager::instance()->clearPendingConnectResource(RR_MSELECT);
        }
        break;
        
    case GET_FREE_PATCHGRP:
        if (res.type == RR_PATCHGROUP) // timeout in getting free patch group
        {
            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(res, NAG);
            The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
        }
        break;
    }
        
	if ( arg != NULL )
	{
        FutureManager* man = (FutureManager*) arg;
		man->removeFuture(m_future,true);
	}

	FUNCTION_EXIT;

	return 0;
}


ACE_Future<FutureResultT> TimeoutCallback::getFuture()
{
	return m_future;
}

std::string TimeoutCallback::commandToString(unsigned int id)
{
	std::string cmdStr;

	switch(id)
	{
	case REGISTER_AP:
		cmdStr = "REGISTER_AP";
		break;
	case DEREGISTER_AP:
		cmdStr = "DEREGISTER_AP";
		break;
	case RETRIEVE_RCDB_GRANT:
		cmdStr = "RETRIEVE_RCDB_GRANT";
		break;
	case ASSIGN_RESOURCE:
		cmdStr = "ASSIGN_RESOURCE";
		break;
	case DEASSIGN_RESOURCE:
		cmdStr = "DEASSIGN_RESOURCE";
		break;
	case SELECT_RESOURCE:
		cmdStr = "SELECT_RESOURCE";
		break; 
	case DESELECT_RESOURCE:
		cmdStr = "DESELECT_RESOURCE";
		break;
	case CHANGE_VOLUME:
		cmdStr = "CHANGE_VOLUME";
		break;
	case GET_FREE_PATCHGRP:
		cmdStr = "GET_FREE_PATCHGRP";
		break;
	case ADD_PATCH_MEMBER:
		cmdStr = "ADD_FREE_PATCHGRP";
		break;
	case DELETE_PATCH_MEMBER:
		cmdStr = "DELETE_PATCH_MEMBER";
		break;
	case GET_FREE_MSEL:
		cmdStr = "GET_FREE_MSEL";
		break;
	case OPEN_MSEL:
		cmdStr = "OPEN_MSEL";
		break;
	case CLOSE_MSEL:
		cmdStr = "CLOSE_MSEL";
		break;
	case ADD_MSEL_MEMBER:
		cmdStr = "ADD_MSEL_MEMBER";
		break;
	case DELETE_MSEL_MEMBER:
		cmdStr = "DELETE_MSEL_MEMBER";
		break;
	case BEGIN_PRIVATE_CALL:
		cmdStr = "BEGIN_PRIVATE_CALL";
		break;
	case ANSWER_PRIVATE_CALL:
		cmdStr = "ANSWER_PRIVATE_CALL";
		break;
	case END_PRIVATE_CALL:
		cmdStr = "END_PRIVATE_CALL";
		break;
	case BEGIN_AMBIENCE_LISTENING:
		cmdStr = "BEGIN_AMBIENCE_LISTENING";
		break;
	case END_AMBIENCE_LISTENING:
		cmdStr = "END_AMBIENCE_LISTENING";
		break;
	case BEGIN_EMERGENCY_CALL:
		cmdStr = "BEGIN_EMERGENCY_CALL";
		break;
	case END_EMERGENCY_CALL:
		cmdStr = "END_EMERGENCY_CALL";
		break;
	case BEGIN_GENERAL_XMIT:
		cmdStr = "BEGIN_GENERAL_XMIT";	
		break;
	case END_GENERAL_XMIT:
		cmdStr = "END_GENERAL_XMIT";	
		break;
	case BROADCAST_TRAIN:
		cmdStr = "BROADCAST_TRAIN";
	}

	return cmdStr;
}