/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/ostreamHelper.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper for stream functions
  */



#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/ostreamHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventHelper.h"


std::ostream &operator<< (std::ostream &o, CADI_TIMESTAMP &x)
{
    char buf[30] = {0}; // big enough, only need 22 chars
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%01d", x.year, x.month, x.day, x.hour, x.minute, x.second, x.decisecond);

    o << "timestamp[" << buf << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_ZONE_INFO &x)
{
    o << "zoneInfo[cz=" << static_cast<int>(x.cz) << ",pz=" << static_cast<int>(x.pz) << ",rz=" << static_cast<int>(x.rz) << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_RADIO_EVENT_INFO &x)
{
    o   << "radioEvent["
        << "unit=" << x.radio_id << "{" << x.radio_alias << "}" 
        << " group=" << x.group_id << "{" << x.group_alias << "} "
        << x.timestamp << "]";

    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_ZC_SEQ &x)
{
    o << "zcSeq[ucn=" << x.ucn << ",csn=" << x.csn << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_CMD_STATUS_DATA &x)
{
    o   << "commandStatusEvent["
        << "statusType=(" << x.cmd_status_type << ")[" << TA_IRS_App::CADIEventHelper::getCmdStatus(x.cmd_status_type) << "] " 
        << "commandNumber=" << x.cmd_number // todo: is the reject code helper call correct ???
        << " rejectCode=(" << x.cmd_reject_code << ")[" << TA_IRS_App::CADIEventHelper::getRejectCodeQualifier(x.cmd_reject_code) << "] " 
        << x.timestamp << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_GROUP_CALL_DATA &x)
{
    o   << "groupCallEvent["
        << x.radio_event
        << " evQual=(" << x.radio_ev_qualifier << ":0x" << std::hex << x.radio_ev_qualifier << std::dec << ")[" << TA_IRS_App::CADIEventHelper::getRadioEventQualifiers(x.radio_ev_qualifier) << "]"
        << " " << x.zc_seq_numbers
        << " " << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_PRIVATE_CALL_DATA &x)
{
    o   << "privateCallEvent["
        << x.radio_event
        << " target=" << x.target_id << "{" << x.target_alias << "} "
        << x.zc_seq_numbers
        << " " << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_EA_DATA &x)
{
    o   << "emergencyEvent[" 
        << &x.radio_event
        << " zcSeqNo=" << x.zc_ucn
        << " " << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_INTERCONNECT_DATA &x)
{
    o   << "interconnectCallEvent[";
    // same as groupCall
    o   << x.radio_event
        << " evQual=(" << x.radio_ev_qualifier << ":0x" << std::hex << x.radio_ev_qualifier << std::dec << ")[" << TA_IRS_App::CADIEventHelper::getRadioEventQualifiers(x.radio_ev_qualifier) << "] "
        << x.zc_seq_numbers
        << " " << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_RADIO_ACK_DATA &x)
{
    o   << "radioAckEvent["
        << x.radio_event
        << " ackType=(" << x.ack_type << ")[" << TA_IRS_App::CADIEventHelper::getAckQualifier(x.ack_type) << "]" 
        << " rejectReason=(" << x.reject_reason << ":0x" << std::hex << x.reject_reason << std::dec << ")[" << TA_IRS_App::CADIEventHelper::getRejectCodeQualifier(x.reject_reason) << "] " 
        << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_STATUS_DATA &x)
{
    o   << "statusEvent[" << x.radio_event
        << " status=(" << x.status_num << ":0x" << std::hex << x.status_num << std::dec << ")[" << "<UNSPECIFIED>" << "] " 
        << x.zone_info << "]";
    return o;
}

std::ostream &operator<< (std::ostream &o, CADI_ZC_STATUS_DATA &x)
{
    o   << "zoneControllerStatus["
        << "zone={" << x.zone_alias << "} " 
        << "status=(" << x.zone_status << ")[" << TA_IRS_App::CADIEventHelper::getZCStatus(x.zone_status) << "] " 
        << "izTrunk=(" << x.iz_trunking_capable << ")[" << TA_IRS_App::CADIEventHelper::getIZTrunkingStatus(x.iz_trunking_capable) << "] "
        << x.timestamp << "]";
    return o;
}
