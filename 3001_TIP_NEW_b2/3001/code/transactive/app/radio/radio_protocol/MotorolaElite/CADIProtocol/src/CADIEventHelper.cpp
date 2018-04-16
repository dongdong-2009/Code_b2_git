

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventHelper.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>


namespace TA_IRS_App {

/************************************************************************/
/* structure & data type                                                */
/************************************************************************/

struct CADCommandReturnCode
{ 
    char name[40]; 
    int  value; 
};
CADCommandReturnCode cadCommandTable[] =
{
    { "CADI_SUCCESS",            CADI_SUCCESS         }, //0
    { "CADI_FAILURE",            CADI_FAILURE         }, //-1
    { "CADI_LOGIN_REQUIRED",     CADI_LOGIN_REQUIRED  }, //-2
    { "CADI_ZC_CONN_FAILED",     CADI_ZC_CONN_FAILED  }, //-3
    { "CADI_INVALID_RADIO",      CADI_INVALID_RADIO   }, //-4
    { "CADI_INVALID_COMMAND",    CADI_INVALID_COMMAND }, //-5
    { "CADI_INVALID_TARGET",     CADI_INVALID_TARGET  }, //-6
    { "CADI_NO_PERMISSION",      CADI_NO_PERMISSION   }  //-7
    //{ "CADI_NOT_IMPLEMENTED",  CADI_NOT_IMPLEMENTED }  //-255
};


/// this is a set of flags
struct RadioEventQualifier
{
    char name[40];
    unsigned long value;
};
RadioEventQualifier radioEventQualifierTable[] =
{
    { "CADI_EMERGENCY",       CADI_EMERGENCY        }, //0x80000000
    { "CADI_MULTIGROUP",      CADI_MULTIGROUP       }, //0x40000000
    { "CADI_GROUP_REGROUP",   CADI_GROUP_REGROUP    }, //0x20000000
    { "CADI_MSEL",            CADI_MSEL             }, //0x10000000
    { "CADI_INTERCONNECT",    CADI_INTERCONNECT     }, //0x08000000
    { "CADI_CODED",           CADI_CODED            }, //0x04000000
    { "CADI_LANDLINE",        CADI_LANDLINE         }, //0x02000000
    { "CADI_AUDIO_PATCH",     CADI_AUDIO_PATCH      }, //0x01000000
    { "CADI_RING",            CADI_RING             }, //0x00800000
    { "CADI_TRANSPOND",       CADI_TRANSPOND        }, //0x00400000
    { "CADI_SITEWIDE",        CADI_SITEWIDE         }, //0x00200000
    { "CADI_ASTRO_CALL",      CADI_ASTRO_CALL       }, //0x00100000
    /* OmniLink/APCO Addition        */
    { "CADI_SITE_ONLY_CALL",  CADI_SITE_ONLY_CALL   }, //0x00080000
    { "CADI_REPEAT_DISABLED", CADI_REPEAT_DISABLED  }, //0x00040000
    { "CADI_AFFILIATED",      CADI_AFFILIATED       }, //0x00020000
    { "CADI_GO_CALL",         CADI_GO_CALL          }, //0x00010000
    { "CADI_DATA",            CADI_DATA             }  //0x00008000             
};

struct AckQualifier
{
    char name[40];
    unsigned short value;
};
AckQualifier ackQualifierTable[] =
{
    { "CADI_ACK_UNKNOWN",     CADI_ACK_UNKNOWN      }, //0
    { "CADI_ACK_REGROUP_TG",  CADI_ACK_REGROUP_TG   }, //1
    { "CADI_ACK_REGROUP_MG",  CADI_ACK_REGROUP_MG   }, //2
    { "CADI_ACK_INHIBIT",     CADI_ACK_INHIBIT      }, //3
    { "CADI_ACK_XINHIBIT",    CADI_ACK_XINHIBIT     }, //4
    { "CADI_ACK_LOCK",        CADI_ACK_LOCK         }, //5
    { "CADI_ACK_UNLOCK",      CADI_ACK_UNLOCK       }, //6
    // Additional Qualifiers for "ack_type" field for APCO system
    { "CADI_ACK_XREGROUP_TG", CADI_ACK_XREGROUP_TG  }, //7
    { "CADI_ACK_XREGROUP_MG", CADI_ACK_XREGROUP_MG  }, //8
    { "CADI_ACK_RADIO_CHECK", CADI_ACK_RADIO_CHECK  }, //9
    // Site Lens Specific Qualifiers
    { "CADI_ACK_FAILSOFT",    CADI_ACK_FAILSOFT     }, //10
    { "CADI_ACK_XFAILSOFT",   CADI_ACK_XFAILSOFT    }  //11 
};

struct CmdStatus
{
    char name[40];
    unsigned short value;
};
CmdStatus cmdStatusTable[] =
{
    { "CADI_ZC_NO_CMD_STATUS", CADI_ZC_NO_CMD_STATUS }, //0
    { "CADI_ZC_SENT_ACK",      CADI_ZC_SENT_ACK      }, //1
    { "CADI_ZC_BUSY_ACK",      CADI_ZC_BUSY_ACK      }, //2
    { "CADI_ZC_REJECTED_NACK", CADI_ZC_REJECTED_NACK }  //3
};

struct SessionStatus
{
    char name[40];
    unsigned short value;
};
SessionStatus sessionStatusTable[] =
{
    { "CADI_SESSION_UP",   CADI_SESSION_UP   }, //0
    { "CADI_NO_SESSION",   CADI_NO_SESSION   }, //1
    { "CADI_NO_LICENSE",   CADI_NO_LICENSE   }, //2
    { "CADI_SESSION_DOWN", CADI_SESSION_DOWN }, //3
    /*      OmniLink Addition     */
    { "CADI_SESSION_MAXEDOUT", CADI_SESSION_MAXEDOUT }, //4
    { "CADI_ZM_DB_DOWN",       CADI_ZM_DB_DOWN       }, //5
    { "CADI_ZM_DB_UP",         CADI_ZM_DB_UP         }, //6
    { "CADI_ZC_LINK_UP",       CADI_ZC_LINK_UP       }, //7
    { "CADI_ZC_LINK_DOWN",     CADI_ZC_LINK_DOWN     }, //8
    /*      Additional Session MSG  */
    { "CADI_SESSION_FORCED_OUT", CADI_SESSION_FORCED_OUT }  //9
};

struct ZCStatus
{
    char name[40];
    unsigned short value;
};
ZCStatus ZCStatusTable[] =
{
    { "CADI_ZC_STATUS_UNKNOWN", CADI_ZC_STATUS_UNKNOWN }, //0
    { "CADI_ZC_STATUS_ACTIVE",  CADI_ZC_STATUS_ACTIVE  }, //1
    { "CADI_ZC_STATUS_DISABLE", CADI_ZC_STATUS_DISABLE }, //2
    { "CADI_ZC_STATUS_IDLE",    CADI_ZC_STATUS_IDLE    }  //3
};

struct IZTrunkingStatus
{
    char name[40];
    unsigned short value;
};
IZTrunkingStatus ZCTrunkingStatusTable[] =
{
    { "CADI_IZT_CAPABLE",   CADI_IZT_CAPABLE   }, //1
    { "CADI_IZT_INCAPABLE", CADI_IZT_INCAPABLE }  //0
};

struct RejectCodeQualifiers
{
    char name[50];
    unsigned short value;
};
RejectCodeQualifiers rejectCodeTable[] =
{
    // Qualifiers for reject_code field (these are from the ZC)
    { "CADI_NO_REJECT_TYPE",                  CADI_NO_REJECT_TYPE                   }, // 0xFF
    { "CADI_INVALID_REQ_ID_REJ",              CADI_INVALID_REQ_ID_REJ               }, // 0x00
    { "CADI_INVALID_TARGET_ID_REJ",           CADI_INVALID_TARGET_ID_REJ            }, // 0x01
    { "CADI_DISABLED_REQ_ID_REJ",             CADI_DISABLED_REQ_ID_REJ              }, // 0x02
    { "CADI_DISABLED_TARGET_ID_REJ",          CADI_DISABLED_TARGET_ID_REJ           }, // 0x03
    { "CADI_INVALID_TG_REJ",                  CADI_INVALID_TG_REJ                   }, // 0x04
    { "CADI_DISABLED_TG_REJ",                 CADI_DISABLED_TG_REJ                  }, // 0x05
    { "CADI_TARGET_TG_INVALID_REJ",           CADI_TARGET_TG_INVALID_REJ            }, // 0x06
    { "CADI_TARGET_TG_DISABLED_REJ",          CADI_TARGET_TG_DISABLED_REJ           }, // 0x07
    { "CADI_SITEWIDE_INCAPABLE_REJ",          CADI_SITEWIDE_INCAPABLE_REJ           }, // 0x10
    { "CADI_MG_INCAPABLE_REJ",                CADI_MG_INCAPABLE_REJ                 }, // 0x11
    { "CADI_DVP_INCAPABLE_REJ",               CADI_DVP_INCAPABLE_REJ                }, // 0x12
    { "CADI_TG_INCAPABLE_REJ",                CADI_TG_INCAPABLE_REJ                 }, // 0x13
    { "CADI_PC_INCAPABLE_REQ_REJ",            CADI_PC_INCAPABLE_REQ_REJ             }, // 0x14
    { "CADI_PC_INCAPABLE_TARGET_REJ",         CADI_PC_INCAPABLE_TARGET_REJ          }, // 0x15
    { "CADI_INT_INCAPABLE_REJ",               CADI_INT_INCAPABLE_REJ                }, // 0x16
    { "CADI_DVP_UPGRADE_REJ",                 CADI_DVP_UPGRADE_REJ                  }, // 0x17
    { "CADI_DVP_DNGRADE_REJ",                 CADI_DVP_DNGRADE_REJ                  }, // 0x18
    { "CADI_DVP_CODED_ONLY_TARGET_REJ",       CADI_DVP_CODED_ONLY_TARGET_REJ        }, // 0x19
    { "CADI_NO_CAPABLE_RESOURCES_REJ",        CADI_NO_CAPABLE_RESOURCES_REJ         }, // 0x20
    { "CADI_ASTRO_INCAPABLE_TARGET_REJ",      CADI_ASTRO_INCAPABLE_TARGET_REJ       }, // 0x25
    { "CADI_TARGET_NOT_RESPONDED_REJ",        CADI_TARGET_NOT_RESPONDED_REJ         }, // 0x26
    { "CADI_ID_INVOLVED_IN_DI_REJ",           CADI_ID_INVOLVED_IN_DI_REJ            }, // 0x27
    { "CADI_TG_DENIED_DUE_TO_MG_REJ",         CADI_TG_DENIED_DUE_TO_MG_REJ          }, // 0x30
    { "CADI_INVALID_PATCH_OF_A_MG",           CADI_INVALID_PATCH_OF_A_MG            }, // 0x31
    { "CADI_INVALID_NORM_TAC_OF_A_MG",        CADI_INVALID_NORM_TAC_OF_A_MG         }, // 0x32
    { "CADI_INVALID_REPEAT_OF_A_MG",          CADI_INVALID_REPEAT_OF_A_MG           }, // 0x33
    { "CADI_INVALID_CTRL_CNSL_OP_INFO_REJ",   CADI_INVALID_CTRL_CNSL_OP_INFO_REJ    }, // 0x34
    { "CADI_NO_AFFILIATED_MEMBERS_REJ",       CADI_NO_AFFILIATED_MEMBERS_REJ        }, // 0x35
    { "CADI_INVALID_MSEL_GROUP_NUMBER",       CADI_INVALID_MSEL_GROUP_NUMBER        }, // 0x36
    { "CADI_INVALID_MG_IN_MSEL_REQUEST",      CADI_INVALID_MG_IN_MSEL_REQUEST       }, // 0x37
    { "CADI_RQSTR_RECORD_NOT_AVAIL_REJ",      CADI_RQSTR_RECORD_NOT_AVAIL_REJ       }, // 0x40
    { "CADI_TARGET_RECORD_NOT_AVAIL_REJ",     CADI_TARGET_RECORD_NOT_AVAIL_REJ      }, // 0x41
    { "CADI_TG_REC_NOT_AVAIL_REJ",            CADI_TG_REC_NOT_AVAIL_REJ             }, // 0x42
    { "CADI_RQSTR_PROHIBITED_ATIA_SITE_REJ",  CADI_RQSTR_PROHIBITED_ATIA_SITE_REJ   }, // 0x43
    { "CADI_TARGET_PROHIBITED_ATIA_SITE_REJ", CADI_TARGET_PROHIBITED_ATIA_SITE_REJ  }, // 0x44
    { "CADI_TG_PROHIBITED_ATIA_SITE_REJ",     CADI_TG_PROHIBITED_ATIA_SITE_REJ      }, // 0x45
    { "CADI_INT_CALL_LIMIT_EXCEEDED_REJ",     CADI_INT_CALL_LIMIT_EXCEEDED_REJ      }, // 0x46
    { "CADI_AUDIO_INTERRUPT_REJ",             CADI_AUDIO_INTERRUPT_REJ              }, // 0x47
    { "CADI_PC_TARGET_DEAFFILIATED_REJ",      CADI_PC_TARGET_DEAFFILIATED_REJ       }, // 0x48
    { "CADI_PC_TARGET_DEREGISTERED_REJ",      CADI_PC_TARGET_DEREGISTERED_REJ       }, // 0x48
    { "CADI_NO_BUSY_TO_BUSY_ORIDE_REJ",       CADI_NO_BUSY_TO_BUSY_ORIDE_REJ        }, // 0x49
    { "CADI_SITE_ACCESS_DENIAL",              CADI_SITE_ACCESS_DENIAL               }, // 0x4A
    { "CADI_MG_EMRG_INCAPABLE_REJ",           CADI_MG_EMRG_INCAPABLE_REJ            }, // 0x4B
    { "CADI_EMRG_NOT_ALLOWED_ON_GRP_REJ",     CADI_EMRG_NOT_ALLOWED_ON_GRP_REJ      }, // 0x4C
    { "CADI_CA_INCAPABLE_REQ_REJ",            CADI_CA_INCAPABLE_REQ_REJ             }, // 0x4D
    { "CADI_CA_INCAPABLE_TARGET_REJ",         CADI_CA_INCAPABLE_TARGET_REJ          }, // 0x4E
    { "CADI_CA_TARGET_DEAFFILIATED_REJ",      CADI_CA_TARGET_DEAFFILIATED_REJ       }, // 0x4F
    { "CADI_CA_TARGET_DEREGISTERED_REJ",      CADI_CA_TARGET_DEREGISTERED_REJ       }, // 0x4F
    { "CADI_NO_SITEWD_SITES_REJ",             CADI_NO_SITEWD_SITES_REJ              }, // 0x50
    { "CADI_SITEWD_OVERLAP_REJ",              CADI_SITEWD_OVERLAP_REJ               }, // 0x51
    { "CADI_TARGET_NOT_AFFILIATED",           CADI_TARGET_NOT_AFFILIATED            }, // 0x52
    { "CADI_TARGET_NOT_REGISTERED_REJ",       CADI_TARGET_NOT_REGISTERED_REJ        }, // 0x52
    { "CADI_CMD_NOT_ALLOWED",                 CADI_CMD_NOT_ALLOWED                  }, // 0x53
    { "CADI_UNABLE_TO_GET_PEND_BUFFER_REJ",   CADI_UNABLE_TO_GET_PEND_BUFFER_REJ    }, // 0x54
    // OmniLink Addition
    { "CADI_TG_REGRP_INCAPABLE_REJ",          CADI_TG_REGRP_INCAPABLE_REJ           }, // 0x55
    { "CADI_INCOMPAT_TG_IND_MOD_TYPE_REJ",    CADI_INCOMPAT_TG_IND_MOD_TYPE_REJ     }, // 0x56
    { "CADI_INCOMPAT_TG_IND_FRQ_BAND_REJ",    CADI_INCOMPAT_TG_IND_FRQ_BAND_REJ     }, // 0x57
    // OmniLink Modification: used to be 0x2f
    { "CADI_B_O_NOT_ALLOWED",                 CADI_B_O_NOT_ALLOWED                  }, // 0x58
    // OmniLink Addition
    { "CADI_TG_IS_SITE_ONLY_REJ",             CADI_TG_IS_SITE_ONLY_REJ              }, // 0x59
    { "CADI_DATA_PC_TARG_IN_GRP_ACT_REJ",     CADI_DATA_PC_TARG_IN_GRP_ACT_REJ      }, // 0x5A
    { "CADI_DATA_PC_TARG_IN_INT_ACT_REJ",     CADI_DATA_PC_TARG_IN_INT_ACT_REJ      }, // 0x5B
    { "CADI_GO_DENIED_DUE_TO_GO_REJ",         CADI_GO_DENIED_DUE_TO_GO_REJ          }, // 0x5C
    { "CADI_GO_DENIED_DUE_TO_LAND_REJ",       CADI_GO_DENIED_DUE_TO_LAND_REJ        }, // 0x5D
    { "CADI_INVALID_REQ_AUD_TME_SLT_REJ",     CADI_INVALID_REQ_AUD_TME_SLT_REJ      }, // 0x5E
    { "CADI_TRESPASS_PROTECTION_REJ",         CADI_TRESPASS_PROTECTION_REJ          }, // 0x60
    { "CADI_DUPLICATE_ID_REJ",                CADI_DUPLICATE_ID_REJ                 }, // 0x61
    { "CADI_FEATURE_NOT_SUPPORTED_REJ",       CADI_FEATURE_NOT_SUPPORTED_REJ        }, // 0x62
    { "CADI_IND_ATTRIBS_TEMP_NOT_AVAIL_REJ",  CADI_IND_ATTRIBS_TEMP_NOT_AVAIL_REJ   }, // 0x63
    { "CADI_GRP_ATTRIBS_TEMP_NOT_AVAIL_REJ",  CADI_GRP_ATTRIBS_TEMP_NOT_AVAIL_REJ   }, // 0x64
    { "CADI_SYSTEM_DENIED_IND_ACCESS_REJ",    CADI_SYSTEM_DENIED_IND_ACCESS_REJ     }, // 0x65
    { "CADI_SYSTEM_DENIED_GRP_ACCESS_REJ",    CADI_SYSTEM_DENIED_GRP_ACCESS_REJ     }, // 0x66
    { "CADI_RQSTR_AFF_TG_NOT_MEM_OF_MG_REJ",  CADI_RQSTR_AFF_TG_NOT_MEM_OF_MG_REJ   }, // 0x67
    { "CADI_NO_CAPABLE_SITES_REJ",            CADI_NO_CAPABLE_SITES_REJ             }, // 0x68
    { "CADI_NO_CAPABLE_DIU_REJ",              CADI_NO_CAPABLE_DIU_REJ               }, // 0x69
    { "CADI_NO_CAPABLE_CIU_REJ",              CADI_NO_CAPABLE_CIU_REJ               }, // 0x6A
    { "CADI_NO_CAPABLE_MBX_REJ",              CADI_NO_CAPABLE_MBX_REJ               }, // 0x6B
    { "CADI_NO_CAPABLE_IZAC_REJ",             CADI_NO_CAPABLE_IZAC_REJ              }, // 0x6C
    { "CADI_TARGET_ZONE_UNREACHABLE_REJ",     CADI_TARGET_ZONE_UNREACHABLE_REJ      }, // 0x6D
    { "CADI_TARGET_IHLR_UNREACHABLE_REJ",     CADI_TARGET_IHLR_UNREACHABLE_REJ      }, // 0x6E
    { "CADI_RQSTR_HLR_UNREACHABLE_TO_TARGET_REJ",   CADI_RQSTR_HLR_UNREACHABLE_TO_TARGET_REJ   }, // 0x6F
    { "CADI_TARGET_NOT_AT_TARGET_ZONE_REJ",         CADI_TARGET_NOT_AT_TARGET_ZONE_REJ         }, // 0x70
    { "CADI_TARGET_ATTRIBS_TEMP_NOT_AVAIL_REJ",     CADI_TARGET_ATTRIBS_TEMP_NOT_AVAIL_REJ     }, // 0x71
    { "CADI_NON_EMRG_RQST_DENIED_DUE_TO_EMRG_REJ",  CADI_NON_EMRG_RQST_DENIED_DUE_TO_EMRG_REJ  }, // 0x72
    { "CADI_EXCEEDED_MAX_NUM_CNSL_OPS_REJ",         CADI_EXCEEDED_MAX_NUM_CNSL_OPS_REJ         }, // 0x73
    { "CADI_BUSY_SERVICE_TERMINATED_REJ",           CADI_BUSY_SERVICE_TERMINATED_REJ           }, // 0x74
    { "CADI_DEVICE_TYPES_INCOMPAT_FOR_SERVICE_REJ", CADI_DEVICE_TYPES_INCOMPAT_FOR_SERVICE_REJ }, // 0x75
    { "CADI_INCOMPAT_PC_SECURE_MOD_TYPE_REJ",       CADI_INCOMPAT_PC_SECURE_MOD_TYPE_REJ       }, // 0x76
    { "CADI_UNCONFIRMED_TG_DENIED_DUE_TO_MG_REJ",   CADI_UNCONFIRMED_TG_DENIED_DUE_TO_MG_REJ   }, // 0x77
    { "CADI_UNABLE_TO_BSY_RQSTR_ON_ACT_CALL",       CADI_UNABLE_TO_BSY_RQSTR_ON_ACT_CALL       }, // 0x78
    { "CADI_LAND_TO_MOBILE_LANDLINE_REJ",           CADI_LAND_TO_MOBILE_LANDLINE_REJ           }, // 0x79
    /*  NEW SUBSCRIBER REJECTS for OZCAR5  */
    { "CADI_STEALTH_REJECT",                  CADI_STEALTH_REJECT                   }, // 0x90
    { "CADI_RQSTR_NOT_REG",                   CADI_RQSTR_NOT_REG                    }, // 0x91
    { "CADI_RQSTR_NOT_AFF_TO_TG",             CADI_RQSTR_NOT_AFF_TO_TG              }, // 0x92
    { "CADI_TG_RQST_ON_REGROUPED_TG",         CADI_TG_RQST_ON_REGROUPED_TG          }, // 0x93
    { "CADI_IND_TERMINATED_CALL_REJ",         CADI_IND_TERMINATED_CALL_REJ          }, // 0x94
    { "CADI_INVALID_SERVICE_STATE_REJ",       CADI_INVALID_SERVICE_STATE_REJ        }, // 0x95
    { "CADI_TARGET_IN_ANOTHER_CALL_REJ",      CADI_TARGET_IN_ANOTHER_CALL_REJ       }, // 0x96
    { "CADI_PTT_TEMP_NOT_ALLOWED_REJ",        CADI_PTT_TEMP_NOT_ALLOWED_REJ         }, // 0x97
    { "CADI_CALL_TIMEOUT_REJ",                CADI_CALL_TIMEOUT_REJ                 }, // 0x98
    { "CADI_SEC_RQ_ON_CLR_ONLY_GRP_REC",      CADI_SEC_RQ_ON_CLR_ONLY_GRP_REC       }, // 0x99
    { "CADI_CLR_RQ_ON_SEC_ONLY_GRP_REC",      CADI_CLR_RQ_ON_SEC_ONLY_GRP_REC       }, // 0x9A
    { "CADI_PC_TARGET_REC_CLR_ONLY",          CADI_PC_TARGET_REC_CLR_ONLY           }, // 0x9B
    { "CADI_PC_TARGET_REC_SEC_ONLY",          CADI_PC_TARGET_REC_SEC_ONLY           }, // 0x9C
    { "CADI_SEC_REQ_BY_IND_CLR_ONLY",         CADI_SEC_REQ_BY_IND_CLR_ONLY          }, // 0x9D
    { "CADI_CLR_REQ_BY_IND_SEC_ONLY",         CADI_CLR_REQ_BY_IND_SEC_ONLY          }, // 0x9E
    { "CADI_KEY_NOT_AVAIL_AT_CON",            CADI_KEY_NOT_AVAIL_AT_CON             }, // 0x9F
    { "CADI_INFRASTRUCTURE_ERROR_REJ",        CADI_INFRASTRUCTURE_ERROR_REJ         }, // 0xA0
    { "CADI_GRP_CALL_SETUP_ABORTED_REJ",      CADI_GRP_CALL_SETUP_ABORTED_REJ       }, // 0xA1
    { "CADI_STEALTH_TG_DENIED_DUE_TO_MG_REJ", CADI_STEALTH_TG_DENIED_DUE_TO_MG_REJ  }, // 0xA2
    { "CADI_ZONE_REMOVED_REJ",                CADI_ZONE_REMOVED_REJ                 }, // 0xA3
    { "CADI_IND_RECORD_DELETED_REJ",          CADI_IND_RECORD_DELETED_REJ           }, // 0xA4
    { "CADI_TARGET_REFUSED_CALL_REJ",         CADI_TARGET_REFUSED_CALL_REJ          }, // 0xA5
    { "CADI_IND_IN_ANOTHER_CALL_REJ",         CADI_IND_IN_ANOTHER_CALL_REJ          }, // 0xA6
    { "CADI_RSRC_PREEMPTION_REJ",             CADI_RSRC_PREEMPTION_REJ              }, // 0xA7
    { "CADI_RQSTR_CANT_SRC_AT_SITE_REJ",      CADI_RQSTR_CANT_SRC_AT_SITE_REJ       }, // 0xA8
    { "CADI_IND_NOT_AT_ZONE_REJ",             CADI_IND_NOT_AT_ZONE_REJ              }, // 0xA9
    // Radio Rejects
    { "CADI_RADIO_REJ", CADI_RADIO_REJ }  // 0x150
    
};

/************************************************************************/
/* methods                                                              */
/************************************************************************/
	
	CADIEventHelper::CADIEventHelper()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CADIEventHelper::~CADIEventHelper()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}


	std::string CADIEventHelper::getCADCommandReturnCode(int event)
	{
		FUNCTION_ENTRY("getCADCommandReturnCode");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(cadCommandTable)/sizeof(CADCommandReturnCode); ++i)
		{
			if (event == cadCommandTable[i].value)
			{
				result = cadCommandTable[i].name;
				break;
			}
		}

		FUNCTION_EXIT;
		return result;
	}


	std::string CADIEventHelper::getRadioEventQualifiers(unsigned long event)
	{
		FUNCTION_ENTRY("getRadioEventQualifiers");
		std::ostringstream result;
		bool isEmpty = true;
		for (int i = 0 ; i < sizeof(radioEventQualifierTable)/sizeof(RadioEventQualifier); ++i)
		{
			//std::cout << std::hex << "value=" << value << " eq.value=" << radioEventQualifierTable[i].value << " & result=" << (value & radioEventQualifierTable[i].value) << std::endl;
			// bitwise comparison to see if flag set
			if ( (event & radioEventQualifierTable[i].value) != 0)
			{
				if (false == isEmpty) 
				{
					result << "|";              
				}
				result << radioEventQualifierTable[i].name;
				isEmpty = false;
			}
		}
		if (true == isEmpty)
		{
			result << "<NoQualifiers>";
		}
		result << std::ends;

		FUNCTION_EXIT;
		return result.str();
	}

	std::string CADIEventHelper::getAckQualifier(unsigned short event)
	{
		FUNCTION_ENTRY("getAckQualifier");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(ackQualifierTable)/sizeof(AckQualifier); ++i)
		{
			if (event == ackQualifierTable[i].value)
			{
				result = ackQualifierTable[i].name;
				break;
			}
		}

		FUNCTION_EXIT;
		return result;
	}

	std::string CADIEventHelper::getCmdStatus(unsigned short event)
	{
		FUNCTION_ENTRY("getCmdStatus");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(cmdStatusTable)/sizeof(CmdStatus); ++i)
		{
			if (event == cmdStatusTable[i].value)
			{
				result = cmdStatusTable[i].name;
				break;
			}
		}
		FUNCTION_EXIT;
		return result;
	}

	std::string CADIEventHelper::getSessionStatus(unsigned short event)
	{
		FUNCTION_ENTRY("getSessionStatus");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(sessionStatusTable)/sizeof(SessionStatus); ++i)
		{
			if (event == sessionStatusTable[i].value)
			{
				result = sessionStatusTable[i].name;
				break;
			}
		}
		FUNCTION_EXIT;
		return result;
	}

	std::string CADIEventHelper::getZCStatus(unsigned short event)
	{
		FUNCTION_ENTRY("getZCStatus");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(ZCStatusTable)/sizeof(ZCStatus); ++i)
		{
			if (event == ZCStatusTable[i].value)
			{
				result = ZCStatusTable[i].name;
				break;
			}
		}
		FUNCTION_EXIT;
		return result;
	}

	std::string CADIEventHelper::getIZTrunkingStatus(unsigned short event)
	{
		FUNCTION_ENTRY("getIZTrunkingStatus");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(ZCTrunkingStatusTable)/sizeof(IZTrunkingStatus); ++i)
		{
			if (event == ZCTrunkingStatusTable[i].value)
			{
				result = ZCTrunkingStatusTable[i].name;
				break;
			}
		}
		FUNCTION_EXIT;
		return result;
	}

	std::string CADIEventHelper::getRejectCodeQualifier(unsigned short event)
	{
		FUNCTION_ENTRY("getRejectCodeQualifier");
		std::string result = "<UNKNOWN>";
		for (int i = 0 ; i < sizeof(rejectCodeTable)/sizeof(RejectCodeQualifiers); ++i)
		{
			if (event == rejectCodeTable[i].value)
			{
				result = rejectCodeTable[i].name;
				break;
			}
		}
		FUNCTION_EXIT;
		return result;
	}

}