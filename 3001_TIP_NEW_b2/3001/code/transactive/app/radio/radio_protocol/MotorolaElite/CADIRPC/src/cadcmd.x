/*
+------------------------------------------------------------------------------+
|                                                                              |
|    R E M O T E   P R O C E D U R E   C A L L   S P E C I F I C A T I O N     |
|                                                                              |
|                       COPYRIGHT 1994 - 2001 MOTOROLA INC.                    |
|                                                                              |
+------------------------------------------------------------------------------+

    PRODUCT NAME:  SmartZone/MultiZone CADI
    HEADER  NAME:  cadcmd.x
    CODED     BY:  09/15/94   Steppen Yi

------------------------------- RPC DESCRIPTION --------------------------------

    RPC definitions and declarations for command functions.

------------------------------- REVISION HISTORY -------------------------------

<date>    <name>      <problem#> <description>
09/15/94  Steppen Yi             Initial version.
10/04/94  Tam Nguyen             Modify to include the common header, cadi.h
05/08/95  Tam Nguyen             Add CADCMD__ZM_CONN_FAILED to the error
                                 code for the ZC/M broken connection
04/24/97  Steppen Yi             OL Changes
02/24/99  Daohui Li              Gemini changes.
08/11/00  Daohui Li              Introduce CADCMD__XREGROUP_W_GID() for 
                                 Dimetra project.
09/06/00  Daohui Li              Update the file based on FTR.
08/11/00  Daohui Li              Introduce CADCMD__XREGROUP_W_GID() for 
                                 Dimetra project.
09/06/00  Daohui Li              Update the file based on FTR.
19 Feb 01 Jesper Esbensen        Merged from Dimetra 3.5. Added function 
                                 cadcmd__xregroup_w_gid()
05/17/01  Thomy N. Bui           Remove ifdef GEMINI around CADCMD__FAILSOFT and
				 CADCMD__XFAILSOFT for Dimetra

------------------------------------- CODE -----------------------------------*/

#ifdef    RPC_HDR
%#ifndef  H__CADCMD
%#define  H__CADCMD
%
%#include "cadi.h"
%
#endif
%
%/*
%**-----------------------------------------------------------------------------
%**    Define constants
%**-----------------------------------------------------------------------------
%*/
%
const   CADCMD__FALSE             = 0;
const   CADCMD__HOSTNAME          = tempesthost;
const   CADCMD__MAX_USERNAME_LEN  = 9;
const   CADCMD__MAX_PASSWORD_LEN  = 31;
const   CADCMD__TRUE              = 1;

%
%/*
%**-----------------------------------------------------------------------------
%**    Define fields specific to CADCMD
%**-----------------------------------------------------------------------------
%*/
%
typedef    int             CADCMD__BOOL;
typedef    char            CADCMD__PASSWD[CADCMD__MAX_PASSWORD_LEN];
typedef    CADI__CMD_NO    CADCMD__STATUS;
typedef    char            CADCMD__USER_NAME[CADCMD__MAX_USERNAME_LEN];
%
%/*
%**-----------------------------------------------------------------------------
%**    Define command error/return code
%**-----------------------------------------------------------------------------
%*/
%
const    CADCMD__SUCCESS              =  0;
const    CADCMD__FAILURE              = -1;
const    CADCMD__LOGIN_REQUIRED       = -2;
const    CADCMD__ZC_CONN_FAILED       = -3;
const    CADCMD__INVALID_RADIO        = -4;
const    CADCMD__INVALID_COMMAND      = -5;
const    CADCMD__INVALID_TARGET       = -6;
const    CADCMD__NO_PERMISSION        = -7;

%
%/*
%**-----------------------------------------------------------------------------
%**    Define command arguments
%**
%**    The following commands have the same data type for their arguments:
%**
%**    CADI__RADIO_ID:
%**          cadcmd__lock,
%**          cadcmd__unlock,
%**          cadcmd__inhibit,
%**          cadcmd__xinhibit,
%**          cadcmd__xregroup,
%**          cadcmd__radio_check
%**          cadcmd__xfailsoft
%**-----------------------------------------------------------------------------
%** cadcmd__regroup()
%** cadcmd__xregroup_w_gid()
%**-----------------------------------------------------------------------------
%*/
%
struct CADCMD__REGROUP_DATA
{
    CADI__RADIO_ID        target_radio_id;
    CADI__GROUP_ID        target_group_id;
};
%
%/*
%**----------------------------------------------------------------------------
%** cadcmd__failsoft()     *** THIS IS SPECIFICALLY FOR SMARTNET SYSTEM ***
%**----------------------------------------------------------------------------
%*/
%
struct CADCMD__FAILSOFT_DATA
{
    CADI__RADIO_ID        target_radio_id;
    CADI__FS_CHNL         target_fs_chnl;
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadcmd__login()
%**-----------------------------------------------------------------------------
%*/
%
struct CADCMD__LOGIN_DATA
{
    CADCMD__USER_NAME     user_name;
    CADCMD__PASSWD        password;
    CADCMD__BOOL          is_password_encrypted;
};
%
%/*
%**-----------------------------------------------------------------------------
%** Application Programming Interface for CADI Commands
%**-----------------------------------------------------------------------------
%*/
%
program CADCMD__PROG
{
    version CADCMD__VERS
    {
        /* 
        **    Protocol Handshake
        **    this must be done before LOGIN
        */
        unsigned long     CADCMD__HANDSHAKE   ( void )                 = 100;
        /* 
        **    Commands available for CAD Host
        **    Administrative commands
        */
        CADCMD__STATUS    CADCMD__LOGIN       ( CADCMD__LOGIN_DATA )   = 101;
        CADCMD__STATUS    CADCMD__LOGOUT      ( void )                 = 102;
        CADCMD__STATUS    CADCMD__ZC_QUERY    ( void )                 = 103;
        /*
        **    Radio commands
        */
        CADCMD__STATUS    CADCMD__REGROUP     ( CADCMD__REGROUP_DATA ) = 105;
        CADCMD__STATUS    CADCMD__XREGROUP    ( CADI__RADIO_ID )       = 106;
        CADCMD__STATUS    CADCMD__INHIBIT     ( CADI__RADIO_ID )       = 107;
        CADCMD__STATUS    CADCMD__XINHIBIT    ( CADI__RADIO_ID )       = 108;
        CADCMD__STATUS    CADCMD__LOCK        ( CADI__RADIO_ID )       = 109;
        CADCMD__STATUS    CADCMD__UNLOCK      ( CADI__RADIO_ID )       = 110;
        CADCMD__STATUS    CADCMD__RADIO_CHECK ( CADI__RADIO_ID )       = 111;

        CADCMD__STATUS    CADCMD__FAILSOFT    ( CADCMD__FAILSOFT_DATA )= 112;
        CADCMD__STATUS    CADCMD__XFAILSOFT   ( CADI__RADIO_ID )       = 113;
        /* function for Dimetra only */
        CADCMD__STATUS    CADCMD__XREGROUP_W_GID( CADCMD__REGROUP_DATA )= 114;

    }=1;
}=0x20000001;

#ifdef    RPC_HDR
%#endif   /* H__CADCMD */
#endif
