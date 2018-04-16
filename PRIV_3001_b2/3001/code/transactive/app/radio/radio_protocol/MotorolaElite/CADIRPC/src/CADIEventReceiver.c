/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIEventReceiver.c $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#include <rpc/rpc.h>



#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIProtocol.h"

#include <stdio.h>
#include <string.h>

#include "cadev.h"

int g_fTcpSvcReg = FALSE;
//HSERVER g_tcpTransp = NULL;
SVCXPRT *g_tcpTransp = NULL;



/*
**-----------------------------------------------------------------------------
**      Common Definitions
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
** Application Programming Interface for CADI Events
**-----------------------------------------------------------------------------
*/

// CREATED BY RIPPLE FOR SERVER ROUTINES

/**
*
* These methods are called-back from the cadev_svc.c:server_proc() function.
* Use these hooks to translate the event into a specific CADIEvent and inject it
* into the ACE_Activation_Queue for processing by CADIEventManager.
*/

static void DecoupleTimestamp(CADI_TIMESTAMP* CADI_TIMESTAMP, const CADEV__TIMESTAMP* timestamp)
{
    CADI_TIMESTAMP->day = timestamp->day;
    CADI_TIMESTAMP->month = timestamp->month;
    CADI_TIMESTAMP->year = timestamp->year;
    CADI_TIMESTAMP->hour = timestamp->hour;
    CADI_TIMESTAMP->minute = timestamp->minute;
    CADI_TIMESTAMP->second = timestamp->second;
    CADI_TIMESTAMP->decisecond = timestamp->decisecond;
}

static void DecoupleRadioEventInfo(CADI_RADIO_EVENT_INFO* cadiData, const CADEV__RADIO_EVENT_INFO* data)
{
    strcpy(cadiData->group_alias, data->group_alias);
    cadiData->group_id = data->group_id;
    strcpy(cadiData->radio_alias, data->radio_alias);
    cadiData->radio_id = data->radio_id;
    DecoupleTimestamp(&cadiData->timestamp, &data->timestamp);

}


void * cadev__affiliation_1_svc(CADEV__RADIO_EVENT_INFO* data, struct svc_req* rqstp) 
{ 
	void * rtn = 0;

    // move data to the decoupled type
    CADI_RADIO_EVENT_INFO cadiData;
    DecoupleRadioEventInfo(&cadiData, data);

    notifyEventObserver(CADEV_Affiliation, &cadiData);

	return rtn;
}

void * cadev__call_alert_1_svc(CADEV__CALL_ALERT_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_CALL_ALERT_DATA cadiData;
    cadiData.target_id = data->target_id;
    strcpy(cadiData.target_alias, data->target_alias);

    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);

    notifyEventObserver(CADEV_CallAlert, &cadiData);

	return rtn;
}

void * cadev__change_me_1_svc(CADEV__CHANGE_ME_DATA* data, struct svc_req *rqstp) 
{
	void * rtn = 0; 
	
    CADI_CHANGE_ME_DATA cadiData;
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);

    notifyEventObserver(CADEV_ChangeMe, &cadiData);

	return rtn;
}

void * cadev__cmd_status_1_svc(CADEV__CMD_STATUS_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    // move data to the decoupled type
    CADI_CMD_STATUS_DATA cadiData;
    cadiData.cmd_number = data->cmd_number;
    cadiData.cmd_reject_code = data->cmd_reject_code;
    cadiData.cmd_status_type = data->cmd_status_type;
    memcpy(&cadiData.timestamp, &data->timestamp, sizeof(CADI_TIMESTAMP));
    
    notifyEventObserver(CADEV_CmdStatus, &cadiData);

    return rtn;
}

void * cadev__deaffiliation_1_svc(CADEV__RADIO_EVENT_INFO* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    // move data to the decoupled type
    CADI_RADIO_EVENT_INFO cadiData;
    DecoupleRadioEventInfo(&cadiData, data);

    notifyEventObserver(CADEV_Deaffiliation, &cadiData);
    
    return rtn;
}

void * cadev__emergency_alarm_1_svc(CADEV__EA_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_EA_DATA cadiData;
    cadiData.zc_ucn = data->zc_ucn;
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);

    notifyEventObserver(CADEV_EmergencyAlarm, &cadiData);

    return rtn;
}

void * cadev__group_call_1_svc(CADEV__GROUP_CALL_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_GROUP_CALL_DATA cadiData;
    cadiData.radio_ev_qualifier = data->radio_ev_qualifier;
    cadiData.zc_seq_numbers.csn = data->zc_seq_numbers.csn;
    cadiData.zc_seq_numbers.ucn = data->zc_seq_numbers.ucn;


    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);
	
    notifyEventObserver(CADEV_GroupCall, &cadiData);

    return rtn;
}

void * cadev__intercon_call_1_svc(CADEV__INTERCONNECT_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_INTERCONNECT_DATA cadiData;
    cadiData.radio_ev_qualifier = data->radio_ev_qualifier;
    cadiData.zc_seq_numbers.csn = data->zc_seq_numbers.csn;
    cadiData.zc_seq_numbers.ucn = data->zc_seq_numbers.ucn;
    
    
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);
    
    notifyEventObserver(CADEV_InterconCall, &cadiData);

	return rtn;
}

void * cadev__message_1_svc(CADEV__MESSAGE_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_MESSAGE_DATA cadiData;
    cadiData.message_num = data->message_num;
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);

    notifyEventObserver(CADEV_Message, &cadiData);

    return rtn;
}

void * cadev__private_call_1_svc(CADEV__PRIVATE_CALL_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_PRIVATE_CALL_DATA cadiData;
    cadiData.target_id = data->target_id;
    strcpy(cadiData.target_alias, data->target_alias);
    cadiData.zc_seq_numbers.csn = data->zc_seq_numbers.csn;
    cadiData.zc_seq_numbers.ucn = data->zc_seq_numbers.ucn;

    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);
    
    notifyEventObserver(CADEV_PrivateCall, &cadiData);

    return rtn;
}

void * cadev__radio_ack_1_svc(CADEV__RADIO_ACK_DATA* data, struct svc_req* rqstp) 
{
	void * rtn = 0; 

    CADI_RADIO_ACK_DATA cadiData;
    cadiData.ack_type = data->ack_type;
    cadiData.reject_reason = data->reject_reason;
    
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);
    
    notifyEventObserver(CADEV_RadioAck, &cadiData);

    return rtn;
}

void * cadev__session_1_svc(CADEV__SESSION_STATUS* data, struct svc_req* rqstp)
{
    void * rtn = 0; 

    CADI_SESSION_STATUS cadiData = *data;
    
    notifyEventObserver(CADEV_RadioAck, &cadiData);

    return rtn;
}

void * cadev__status_1_svc(CADEV__STATUS_DATA* data, struct svc_req* rqstp) 
{
    void * rtn = 0; 

    CADI_STATUS_DATA cadiData;
    cadiData.status_num = data->status_num;
    
    cadiData.zone_info.cz = data->zone_info.cz;
    cadiData.zone_info.pz = data->zone_info.pz;
    cadiData.zone_info.rz = data->zone_info.rz;
    DecoupleRadioEventInfo(&cadiData.radio_event, &data->radio_event);
    
    notifyEventObserver(CADEV_Status, &cadiData);

    return rtn;
}

void * cadev__zc_status_1_svc(CADEV__ZC_STATUS_DATA* data, struct svc_req* rqstp) 
{
    void * rtn = 0; 

    CADI_ZC_STATUS_DATA cadiData;
    cadiData.iz_trunking_capable = data->iz_trunking_capable;
    strcpy(cadiData.zone_alias, data->zone_alias);
    cadiData.zone_status = data->zone_status;
    DecoupleTimestamp(&cadiData.timestamp, &data->timestamp);
    
    notifyEventObserver(CADEV_ZCStatus, &cadiData);

    return rtn;
}



static void
cadev__prog_1(struct svc_req* rqstp, SVCXPRT *transp)
{
	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char *)NULL);
		return;

	case CADEV__AFFILIATION:
		cadev__affiliation_1_svc((CADEV__RADIO_EVENT_INFO*) transp, rqstp);
		break;

	case CADEV__CALL_ALERT:
		cadev__call_alert_1_svc((CADEV__CALL_ALERT_DATA*) transp, rqstp);
		break;

	case CADEV__CHANGE_ME:
		cadev__change_me_1_svc((CADEV__CHANGE_ME_DATA*) transp, rqstp);
		break;

	case CADEV__CMD_STATUS:
		cadev__cmd_status_1_svc((CADEV__CMD_STATUS_DATA*) transp, rqstp);
		break;

	case CADEV__DEAFFILIATION:
		cadev__deaffiliation_1_svc((CADEV__RADIO_EVENT_INFO*) transp, rqstp);
		break;

	case CADEV__EMERGENCY_ALARM:
		cadev__emergency_alarm_1_svc((CADEV__EA_DATA*) transp, rqstp);
		break;

	case CADEV__GROUP_CALL:
		cadev__group_call_1_svc((CADEV__GROUP_CALL_DATA*) transp, rqstp);
		break;

	case CADEV__INTERCON_CALL:
		cadev__intercon_call_1_svc((CADEV__INTERCONNECT_DATA*) transp, rqstp);
		break;

	case CADEV__MESSAGE:
		cadev__message_1_svc((CADEV__MESSAGE_DATA*) transp, rqstp);
		break;

	case CADEV__PRIVATE_CALL:
		cadev__private_call_1_svc((CADEV__PRIVATE_CALL_DATA*) transp, rqstp);
		break;

	case CADEV__STATUS:
		cadev__status_1_svc((CADEV__STATUS_DATA*) transp, rqstp);
		break;

	case CADEV__SESSION:
		cadev__session_1_svc((CADEV__SESSION_STATUS*) transp, rqstp);
		break;

	case CADEV__ZC_STATUS:
		cadev__zc_status_1_svc((CADEV__ZC_STATUS_DATA*) transp, rqstp);
		break;

	case CADEV__RADIO_ACK:
		cadev__radio_ack_1_svc((CADEV__RADIO_ACK_DATA*) transp, rqstp);
		break;

	default:
		svcerr_noproc(transp);
		return;
	}

}


int server_proc(u_long lProgram, u_long lVersion)
{
	SVCXPRT *transp;

	(void)pmap_unset(lProgram, lVersion);


	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		(void)fprintf(stderr, "cannot create tcp service.\n");
		return 1;
	}
	if (!svc_register(transp, lProgram, lVersion, cadev__prog_1, IPPROTO_TCP)) {
		(void)fprintf(stderr, "unable to register (CADEV__PROG, CADEV__VERS, tcp).\n");
		return 1;
	}
	svc_run();
	
	return (0) ;
}
