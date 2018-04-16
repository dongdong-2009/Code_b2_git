/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/EventToResultFactory.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPIRadioEvent.h"
#include "app/radio/radio_mft_agent_icdv4/src/EventToResultFactory.h"

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#include "Mca/mca.h"
#include "Mcdi/callback.h"
#include "Mcdi/registration.h"
#include "Mcdi/applications_reset.h"
#include "Mcdi/cdi_status.h"
#include "Mcdi/dispatch_status.h"
#include "Mcdi/pers_db_sync.h"
#include "Mcdi/rc_db_sync.h"

#include "Mca/al_call.h"
#include "Mca/emergency_alarm.h"
#include "Mca/emergency_call.h"
#include "Mca/general_transmit.h"
#include "Mca/inbound_call.h"  	
#include "Mca/inbound_ptt_id.h"	
#include "Mca/transmit_mode_select.h"  
#include "Mca/multi_select.h"
#include "Mca/patch.h"
#include "Mca/private_call.h"
#include "Mca/radio_message.h"
#include "Mca/refresh.h"
#include "Mca/rsc_assignment.h"
#include "Mca/volume.h"
#include "Mca/resource_reject_status.h"
#include "Mca/resource_xmit_status.h"
#include "Mca/rsc_select.h"
#include"RemoteAPICallBackMessage.h"

ACE_Future<FutureResultT> EventToResultFactory::getResult(FutureManager* pFutureMgr,TA_IRS_App::ConsoleDispatchEvent* event)
{
	ACE_Future<FutureResultT> future;

	//Manage the mapping of incoming RadioEvents to appropriate FutureResultT's
	// using commandID 

    switch (event->getEvent())
    {
	case MCA_ASSIGN_RESOURCE_ERR:
		{
			TA_IRS_App::Mca::AssignResourceError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AssignResourceError*>(event);
            future = pFutureMgr->getFutureResultForResource(ASSIGN_RESOURCE, pEvent->getResourceId());
		}

        break;

    case MCA_DEASSIGN_RESOURCE_ERR:
		{
			TA_IRS_App::Mca::DeassignResourceError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::DeassignResourceError*>(event);
            future = pFutureMgr->getFutureResultForChannel(DEASSIGN_RESOURCE, pEvent->getChannel());
		}

        break;

    case MCA_ASSIGNMENT_STATUS:
		{
			TA_IRS_App::Mca::AssignmentStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AssignmentStatus*>(event);

			if (pEvent->getStatus()==TA_IRS_App::Mca::AssignmentStatus::Resource_Assigned)
			{
				future = pFutureMgr->getFutureResultForResource(ASSIGN_RESOURCE, pEvent->getResourceId());
			}
			else if (pEvent->getStatus()==TA_IRS_App::Mca::AssignmentStatus::Resource_Deassigned)
			{
				future = pFutureMgr->getFutureResultForResource(DEASSIGN_RESOURCE, pEvent->getResourceId());
			}
		}
		break;

    // msel
    case MCA_MSEL_GROUP_STATUS:
		{	
			TA_IRS_App::Mca::MselGroupStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::MselGroupStatus*>(event);		

			if (pEvent->getStatus()==TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Opened)
			{
				future = pFutureMgr->getFutureResult(OPEN_MSEL);
			}
			else if (pEvent->getStatus()==TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Closed)
			{
				future = pFutureMgr->getFutureResult(CLOSE_MSEL);
			}
		}
		break;
    case MCA_MSEL_MBR_STATUS:
		{
			TA_IRS_App::Mca::MselMemberStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::MselMemberStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to MselMemberStatus");

			if (pEvent->getStatus()==TA_IRS_App::Mca::MselMemberStatus::Multi_Select_Member_Added)
			{
				future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->getChannel());
			}
			else if (pEvent->getStatus()==TA_IRS_App::Mca::MselMemberStatus::Multi_Select_Member_Deleted)
			{
				future = pFutureMgr->getFutureResultForChannel(DELETE_MSEL_MEMBER, pEvent->getChannel());
			}
		}
		break;

	case MCA_ADD_MSEL_MBR_WRN:
		{
			TA_IRS_App::Mca::AddMselMemberWarning* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AddMselMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->getChannel());
		}
        break;

	case MCA_ADD_MSEL_MBR_ERR:
		{
			TA_IRS_App::Mca::AddMselMemberError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AddMselMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->getChannel());
		}
        break;

	case MCA_DELETE_MSEL_MBR_ERR:
		{
			TA_IRS_App::Mca::DeleteMselMemberError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::DeleteMselMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(DELETE_MSEL_MEMBER, pEvent->getChannel());
		}
        break;


    // patch
	case MCA_ADD_PATCH_MEMBER_WRN:
		{
			TA_IRS_App::Mca::AddPatchMemberWarning* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AddPatchMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->getChannel());
		}
        break;

	case MCA_ADD_PATCH_MEMBER_ERR:
		{
			TA_IRS_App::Mca::AddPatchMemberError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AddPatchMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->getChannel());
		}
        break;

	case MCA_DELETE_PATCH_MEMBER_ERR:
		{
			TA_IRS_App::Mca::DeletePatchMemberError* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::DeletePatchMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(DELETE_PATCH_MEMBER, pEvent->getChannel());
		}
        break;

    case MCA_PATCH_MEMBER_STATUS:
		{
			TA_IRS_App::Mca::PatchMemberStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::PatchMemberStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to PatchMemberStatus");
			switch (pEvent->getStatus())
			{
			case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Added:
				future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->getChannel());
				break;
			case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Deleted:
				future = pFutureMgr->getFutureResultForChannel(DELETE_PATCH_MEMBER, pEvent->getChannel());
				break;
			case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_In_Used:
				// do nothing
				break;
			}
		}
		break;
    case MCA_PVT_CALL_STATUS_STS:
		{
			TA_IRS_App::Mca::PrivateCallStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::PrivateCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to PrivateCallStatus");

			switch(pEvent->getStatus())
			{
			case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Attempt_To_Send:
				future = pFutureMgr->getFutureResult(BEGIN_PRIVATE_CALL);
				break;
			case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Begun:
				future = pFutureMgr->getFutureResult(ANSWER_PRIVATE_CALL);
				break;
			case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Ended:
				future = pFutureMgr->getFutureResult(END_PRIVATE_CALL);
				break;
			}
		}
		break;
	case MCA_AMB_LISTENING_STATUS:
		{
			TA_IRS_App::Mca::AmbienceListeningCallStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::AmbienceListeningCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to AmbienceListeningCallStatus");

			if (pEvent->getStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Begun)
			{
				future = pFutureMgr->getFutureResult(BEGIN_AMBIENCE_LISTENING);
			}
			else if (pEvent->getStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Ended)
			{
				future = pFutureMgr->getFutureResult(END_AMBIENCE_LISTENING);
			}
		}
		break;
	case MCA_EMERGENCY_CALL_STATUS:
		{
			TA_IRS_App::Mca::EmergencyCallStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::EmergencyCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to EmergencyCallStatus");

			if (pEvent->getStatus()==TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_State)
			{
				future = pFutureMgr->getFutureResult(BEGIN_EMERGENCY_CALL);
			}
			else if (pEvent->getStatus() == TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_Ended )		
			{
				future = pFutureMgr->getFutureResult(END_EMERGENCY_CALL);
			}
		}
		break;
	case MCA_RESOURCE_SELECT_STATUS:
		{
			TA_IRS_App::Mca::ResourceSelectStatus* pEvent = 
				dynamic_cast<TA_IRS_App::Mca::ResourceSelectStatus*>(event);	
			
			TA_ASSERT( pEvent != NULL, "Error in downcast to ResourceSelectStatus");

			if (pEvent->getStatus()==TA_IRS_App::Mca::ResourceSelectStatus::Resource_Selected)
			{
				future = pFutureMgr->getFutureResult(SELECT_RESOURCE);
			}
			else if (pEvent->getStatus()==TA_IRS_App::Mca::ResourceSelectStatus::Resource_Deselected)		
			{
				future = pFutureMgr->getFutureResult(DESELECT_RESOURCE);
			}
		}
		
	}
	return future;
}
ACE_Future<FutureResultT> EventToResultFactory::getResult(FutureManager* pFutureMgr,ICDV4::RemoteAPIRadioEvent* event)
{
	ACE_Future<FutureResultT> future;

	//Manage the mapping of incoming RadioEvents to appropriate FutureResultT's
	// using commandID 

    switch (event->getMessageType())
    {
    case eMsgMcaAssignResourceErrorID:
		{
			ICDV4::McaAssignResourceError* pEvent = 
				dynamic_cast<ICDV4::McaAssignResourceError*>(event);
			future = pFutureMgr->getFutureResultForResource(ASSIGN_RESOURCE, pEvent->get_resource());
		}

        break;

	case eMsgMcaDeassignResourceErrorID:
		{
			ICDV4::McaDeassignResourceError* pEvent = 
				dynamic_cast<ICDV4::McaDeassignResourceError*>(event);
            future = pFutureMgr->getFutureResultForChannel(DEASSIGN_RESOURCE, pEvent->get_channel());
		}

        break;

    case eMsgMcaAssignmentStatusID:
		{
			ICDV4::McaAssignmentStatus* pEvent = 
				dynamic_cast<ICDV4::McaAssignmentStatus*>(event);

			if (pEvent->get_wStatus()==MCA_RESOURCE_ASSIGNED)
			{
				future = pFutureMgr->getFutureResultForResource(ASSIGN_RESOURCE, pEvent->get_resource());
			}
			else if (pEvent->get_wStatus()==MCA_RESOURCE_DEASSIGNED)
			{
				future = pFutureMgr->getFutureResultForResource(DEASSIGN_RESOURCE, pEvent->get_resource());
			}
		}
		break;

    // msel
    case eMsgMcaMselGroupStatusID:
		{	
			ICDV4::McaMselGroupStatus* pEvent = 
				dynamic_cast<ICDV4::McaMselGroupStatus*>(event);		

			if (pEvent->get_wStatus()==MCA_OPENED)
			{
				future = pFutureMgr->getFutureResult(OPEN_MSEL);
			}
			else if (pEvent->get_wStatus()==MCA_CLOSED)
			{
				future = pFutureMgr->getFutureResult(CLOSE_MSEL);
			}
		}
		break;
    case eMsgMcaMselMemberStatusID:
		{
			ICDV4::McaMselMemberStatus* pEvent = 
				dynamic_cast<ICDV4::McaMselMemberStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to MselMemberStatus");

			if (pEvent->get_wStatus()==MCA_ADDED)
			{
				future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->get_channel());
			}
			else if (pEvent->get_wStatus()==MCA_DELETED)
			{
				future = pFutureMgr->getFutureResultForChannel(DELETE_MSEL_MEMBER, pEvent->get_channel());
			}
		}
		break;

	case eMsgMcaAddMselMemberWarningID:
		{
			ICDV4::McaAddMselMemberWarning* pEvent = 
				dynamic_cast<ICDV4::McaAddMselMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaAddMselMemberErrorID:
		{
			ICDV4::McaAddMselMemberError* pEvent = 
				dynamic_cast<ICDV4::McaAddMselMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaDeleteMselMemberErrorID:
		{
			ICDV4::McaDeleteMselMemberError* pEvent = 
				dynamic_cast<ICDV4::McaDeleteMselMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(DELETE_MSEL_MEMBER, pEvent->get_channel());
		}
        break;


    // patch
	case eMsgMcaAddPatchMemberWarningID:
		{
			ICDV4::McaAddPatchMemberWarning* pEvent = 
				dynamic_cast<ICDV4::McaAddPatchMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaAddPatchMemberErrorID:
		{
			ICDV4::McaAddPatchMemberError* pEvent = 
				dynamic_cast<ICDV4::McaAddPatchMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaDeletePatchMemberErrorID:
		{
			ICDV4::McaDeletePatchMemberError* pEvent = 
				dynamic_cast<ICDV4::McaDeletePatchMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(DELETE_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

    case eMsgMcaPatchMemberStatusID:
		{
			ICDV4::McaPatchMemberStatus* pEvent = 
				dynamic_cast<ICDV4::McaPatchMemberStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to PatchMemberStatus");
			switch (pEvent->get_wStatus())
			{
			case MCA_ADDED:
				future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->get_channel());
				break;
			case MCA_DELETED:
				future = pFutureMgr->getFutureResultForChannel(DELETE_PATCH_MEMBER, pEvent->get_channel());
				break;
			case MCA_IN_USE:
				// do nothing
				break;
			}
		}
		break;
    case eMsgMcaPrivateCallStatusID:
		{
			ICDV4::McaPrivateCallStatus* pEvent = 
				dynamic_cast<ICDV4::McaPrivateCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to PrivateCallStatus");

			switch(pEvent->get_wStatus())
			{
			case MCA_ATTEMPTING_TO_SEND:
				future = pFutureMgr->getFutureResult(BEGIN_PRIVATE_CALL);
				break;
			case MCA_BEGUN:
				future = pFutureMgr->getFutureResult(ANSWER_PRIVATE_CALL);
				break;
			case MCA_ENDED:
				future = pFutureMgr->getFutureResult(END_PRIVATE_CALL);
				break;
			}
		}
		break;
	case eMsgMcaAmbienceListeningCallStatusID:
		{
			ICDV4::McaAmbienceListeningCallStatus* pEvent = 
				dynamic_cast<ICDV4::McaAmbienceListeningCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to AmbienceListeningCallStatus");

			if (pEvent->get_wStatus()==MCA_BEGUN)
			{
				future = pFutureMgr->getFutureResult(BEGIN_AMBIENCE_LISTENING);
			}
			else if (pEvent->get_wStatus()==MCA_ENDED)
			{
				future = pFutureMgr->getFutureResult(END_AMBIENCE_LISTENING);
			}
		}
		break;
	case eMsgMcaEmergencyCallStatusID:
		{
			ICDV4::McaEmergencyCallStatus* pEvent = 
				dynamic_cast<ICDV4::McaEmergencyCallStatus*>(event);

			TA_ASSERT( pEvent != NULL, "Error in downcast to EmergencyCallStatus");

			if (pEvent->get_wStatus()==MCA_EMERG_CALL_STATE)
			{
				future = pFutureMgr->getFutureResult(BEGIN_EMERGENCY_CALL);
			}
			else if (pEvent->get_wStatus() == MCA_EMERG_ENDED )		
			{
				future = pFutureMgr->getFutureResult(END_EMERGENCY_CALL);
			}
		}
		break;
	case eMsgMcaResourceSelectStatusID:
		{
			ICDV4::McaResourceSelectStatus* pEvent = 
				dynamic_cast<ICDV4::McaResourceSelectStatus*>(event);	
			
			TA_ASSERT( pEvent != NULL, "Error in downcast to ResourceSelectStatus");

			if (pEvent->get_wStatus()==MCA_RSC_SELECTED)
			{
				future = pFutureMgr->getFutureResult(SELECT_RESOURCE);
			}
			else if (pEvent->get_wStatus()==MCA_RSC_DESELECTED)		
			{
				future = pFutureMgr->getFutureResult(DESELECT_RESOURCE);
			}
		}
		
	}
	return future;
}
