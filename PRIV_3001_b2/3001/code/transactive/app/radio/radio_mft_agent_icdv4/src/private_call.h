/* @(#) private_call.h \main\6 Wed Apr 13 19:17:24 2005 */
#ifndef D_PRIVATE_CALL_H
#define D_PRIVATE_CALL_H

/*
*******************************************************************************

                  H E A D E R    S P E C I F I C A T I O N

                         COPYRIGHT 1994-2008 MOTOROLA

*******************************************************************************

---------------------------------- PURPOSE ------------------------------------
    This header describes the data structures and prototypes for the
    API functions for the Private Call feature.

--------------------------- DEPENDENCY COMMENTS -------------------------------
    The following headers must be included before this header is:-
        windows.h
        Mca\mca.h

*******************************************************************************
*/


/*-------------------------- HEADER FILE INCLUDES ----------------------------*/
/* The use of nested include headers, is limited to those headers that are in a
   conditional block. */

#ifdef __cplusplus
extern "C" {
#endif
/*--------------------------- CONSTANTS/DEFINES ------------------------------*/

/**** MESSAGE IDENTIFIERS ****/
/* Only _STATUS, _ERR, and _WRN message identifiers 
   apply to the User Interface */

#define     MCA_PRIVATE_CALL_MSG                  (MCA_MSG_CODE + 0x1140)
#define     MCA_BEGIN_PVT_CALL_REQ                (MCA_PRIVATE_CALL_MSG + 0x0)
#define     MCA_END_PVT_CALL_REQ                  (MCA_PRIVATE_CALL_MSG + 0x1)
#define     MCA_ANSWER_PVT_CALL_REQ               (MCA_PRIVATE_CALL_MSG + 0x2) 
#define     MCA_INBOUND_PVT_CALL_STATUS_STS       (MCA_PRIVATE_CALL_MSG + 0x3)
#define     MCA_PVT_CALL_STATUS_STS               (MCA_PRIVATE_CALL_MSG + 0x4)
#define     MCA_BEGIN_PVT_CALL_ERR                (MCA_PRIVATE_CALL_MSG + 0x5)
#define     MCA_END_PVT_CALL_ERR                  (MCA_PRIVATE_CALL_MSG + 0x6)
#define     MCA_ANSWER_PVT_CALL_ERR               (MCA_PRIVATE_CALL_MSG + 0x7)
#define     MCA_BEGIN_INTR_PRIORITY_PVT_CALL_REQ  (MCA_PRIVATE_CALL_MSG + 0x8)
#define     MCA_PVT_CALL_WRN      /* unused */    (MCA_PRIVATE_CALL_MSG + 0x9)
#define     MCA_REJECT_PVT_CALL_REQ               (MCA_PRIVATE_CALL_MSG + 0x0A)
#define     MCA_REJECT_PVT_CALL_ERR               (MCA_PRIVATE_CALL_MSG + 0x0B)

#define     MCA_MAX_PRIVATE_CALL_MSG              (MCA_PRIVATE_CALL_MSG + 0x3F) 

/**** FEATURE SPECIFIC STATUS CODES ****/
#define     MCA_INBOUND_PVT_CALL  /* unused */    (MCA_FEATURE_STATUS_CODE + 0x0)
#define     MCA_INBOUND_RADIO_PVT_CALL            (MCA_FEATURE_STATUS_CODE + 0x0)
#define     MCA_INBOUND_CONSOLE_PVT_CALL          (MCA_FEATURE_STATUS_CODE + 0x1)

/**** FEATURE SPECIFIC ERROR CODES ****/
#define     MCA_NO_ANSWER                         (MCA_FEATURE_ERROR_CODE + 0x0)

/**** FEATURE SPECIFIC WARNING CODES ****/
#define     MCA_NO_PC_ACTIVITY_WRN  /* unused */  (MCA_FEATURE_WARNING_CODE + 0x0)

/*------------------------------- TYPEDEFS - ENUMS ---------------------------*/

/*------------------------------- TYPEDEFS - STRUCTS--------------------------*/

/**** API MESSAGE STRUCTURE ****/
#ifndef MOTOROLA_INTERNAL 
#pragma pack(push, entry, 2) 
#endif 

typedef struct tagPrivateCall
{
    WORD    wMsgId;
    LC_ID   channel;
    union 
    {   
        struct
        {
            WORD       wStatus;
            UNIT_ID    unit_id;
            WORD       wRcvMode;
            UCHAR      IndividualAlias[MCA_MAX_ALIAS_SIZE];
            WORD       wZoneId;
            WORD       wSiteId;
            WORD       wStatusType;
            UNIT_ID    HeadNumber;
            UCHAR      HeadNumberAlias[MCA_MAX_ALIAS_SIZE];
        }InboundPvtCallStatus;
        struct
        {
            WORD       wStatus;
            UNIT_ID    unit_id;
            UCHAR      IndividualAlias[MCA_MAX_ALIAS_SIZE];
            WORD       wZoneId;
            WORD       wSiteId;
            WORD       wStatusType;
            WORD       wCallMode;
        }PvtCallStatus;
        struct
        {
            WORD       wWarningNum;           
        }PvtCallWarning;  /* unused */
        
        /* The following structure is to begin all types of private call         */
        /* including the interrupt priority private call (dispatcher interrupt). */        
        struct
        {
            BYTE       byClientId;
            UNIT_ID    unit_id;
        }BeginPvtCall;
        struct
        {
            WORD       wErrorNum;
            UNIT_ID    unit_id;
            BYTE       byClientId;
        }BeginPvtCallError;
        struct
        {
            WORD       wErrorNum;
        }EndPvtCallError;
        struct
        {
            BYTE       byClientId;
            UNIT_ID    unit_id;
        }AnswerPvtCall;
        struct
        {
            WORD       wErrorNum;
            UNIT_ID    unit_id;
            BYTE       byClientId;
        }AnswerPvtCallError;
        struct
        {
            BYTE       byClientId;
            UNIT_ID    unit_id;
            UNIT_ID    retry_id;
        }RejectPvtCall;
        struct
        {
            WORD       wErrorNum;
            UNIT_ID    unit_id;
            BYTE       byClientId;
            UNIT_ID    retry_id;
        }RejectPvtCallError;
    } u;
} PRIVATE_CALL_MSG;

#ifndef MOTOROLA_INTERNAL 
#pragma pack(pop, entry) 
#endif 

#ifndef MOTOROLA_INTERNAL       
/*---------------------------- FUNCTION PROTOTYPE(S) -------------------------*/

extern BOOL APIENTRY McaBeginPrivateCall( LC_ID channel, UNIT_ID unit_id, BYTE byClientId );
extern BOOL APIENTRY McaEndPrivateCall( LC_ID channel );
extern BOOL APIENTRY McaAnswerPrivateCall( LC_ID channel, UNIT_ID unit_id, BYTE byClientId );
extern BOOL APIENTRY McaRejectPrivateCall( LC_ID channel, UNIT_ID unit_id, BYTE byClientId , UNIT_ID retry_id);
extern BOOL APIENTRY McaBeginInterruptPriorityPrivateCall( LC_ID channel, UNIT_ID unit_id, BYTE byClientId );

/*----------------------------------- MACROS ---------------------------------*/

#endif  /* MOTOROLA_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif  /* D_PRIVATE_CALL_H */
