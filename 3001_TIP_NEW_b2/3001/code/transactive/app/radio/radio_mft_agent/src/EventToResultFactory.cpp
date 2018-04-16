/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/EventToResultFactory.cpp $
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

#include "app/radio/radio_mft_agent/src/EventToResultFactory.h"

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
#include"app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"

using namespace TA_IRS_App;


ACE_Future<FutureResultT> EventToResultFactory::getResult(FutureManager* pFutureMgr,TA_IRS_App::RemoteAPIRadioEvent* event)
{
	ACE_Future<FutureResultT> future;

	//Manage the mapping of incoming RadioEvents to appropriate FutureResultT's
	// using commandID 

    switch (event->getMessageType())
    {
    case eMsgMcaAssignResourceErrorID:
		{
			McaAssignResourceError* pEvent = 
				dynamic_cast<McaAssignResourceError*>(event);
			future = pFutureMgr->getFutureResultForResource(ASSIGN_RESOURCE, pEvent->get_resource());
		}

        break;

	case eMsgMcaDeassignResourceErrorID:
		{
			McaDeassignResourceError* pEvent = 
				dynamic_cast<McaDeassignResourceError*>(event);
            future = pFutureMgr->getFutureResultForChannel(DEASSIGN_RESOURCE, pEvent->get_channel());
		}

        break;

    case eMsgMcaAssignmentStatusID:
		{
			McaAssignmentStatus* pEvent = 
				dynamic_cast<McaAssignmentStatus*>(event);

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
			McaMselGroupStatus* pEvent = 
				dynamic_cast<McaMselGroupStatus*>(event);		

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
			McaMselMemberStatus* pEvent = 
				dynamic_cast<McaMselMemberStatus*>(event);

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
			McaAddMselMemberWarning* pEvent = 
				dynamic_cast<McaAddMselMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaAddMselMemberErrorID:
		{
			McaAddMselMemberError* pEvent = 
				dynamic_cast<McaAddMselMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(ADD_MSEL_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaDeleteMselMemberErrorID:
		{
			McaDeleteMselMemberError* pEvent = 
				dynamic_cast<McaDeleteMselMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(DELETE_MSEL_MEMBER, pEvent->get_channel());
		}
        break;


    // patch
	case eMsgMcaAddPatchMemberWarningID:
		{
			McaAddPatchMemberWarning* pEvent = 
				dynamic_cast<McaAddPatchMemberWarning*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaAddPatchMemberErrorID:
		{
			McaAddPatchMemberError* pEvent = 
				dynamic_cast<McaAddPatchMemberError*>(event);
            future = pFutureMgr->getFutureResultForChannel(ADD_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

	case eMsgMcaDeletePatchMemberErrorID:
		{
			McaDeletePatchMemberError* pEvent = 
				dynamic_cast<McaDeletePatchMemberError*>(event);
			future = pFutureMgr->getFutureResultForChannel(DELETE_PATCH_MEMBER, pEvent->get_channel());
		}
        break;

    case eMsgMcaPatchMemberStatusID:
		{
			McaPatchMemberStatus* pEvent = 
				dynamic_cast<McaPatchMemberStatus*>(event);

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
			McaPrivateCallStatus* pEvent = 
				dynamic_cast<McaPrivateCallStatus*>(event);

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
			McaAmbienceListeningCallStatus* pEvent = 
				dynamic_cast<McaAmbienceListeningCallStatus*>(event);

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
			McaEmergencyCallStatus* pEvent = 
				dynamic_cast<McaEmergencyCallStatus*>(event);

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
			McaResourceSelectStatus* pEvent = 
				dynamic_cast<McaResourceSelectStatus*>(event);	
			
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
