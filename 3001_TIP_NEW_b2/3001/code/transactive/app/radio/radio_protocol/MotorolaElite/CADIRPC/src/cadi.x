/*
+------------------------------------------------------------------------------+
|                                                                              |
|    R E M O T E   P R O C E D U R E   C A L L   S P E C I F I C A T I O N     |
|                                                                              |
|                      COPYRIGHT 1994 - 1997 MOTOROLA INC.                     |
|                                                                              |
+------------------------------------------------------------------------------+

    PRODUCT NAME:  SmartZone/MultiZone CADI
    HEADER  NAME:  cadi.x
    CODED     BY:  09/22/94   Steppen Yi

------------------------------- RPC DESCRIPTION --------------------------------

    general definitions for CADI

------------------------------- REVISION HISTORY -------------------------------

<date>    <name>       <problem#> <description>
09/22/94  Steppen Yi              Initial version.
09/22/94  Tam Nguyen              Modify to make this file common to 
                                  cadcmd.h & cadev.h
11/22/94  Tam Nguyen              Revamp to make it correspond to the API
04/24/97  Steppen Yi              OL changes
08/04/98  Daniel Yu               Ported to NT
02/25/99  Daohui Li               add new datatype for failsoft for GEMINI
------------------------------------- CODE -----------------------------------*/

#ifdef RPC_HDR
%#ifndef    H__CADI
%#define    H__CADI
#endif

%#include "sys_rpc.h"

%
%/*
%**-----------------------------------------------------------------------------
%**    Command number range is 1 - 0x7FFFFFFF inclusive.  Once it reaches the
%**    upper limit, it will restart from 1.
%**-----------------------------------------------------------------------------
%*/
%
const    CADI__MAX_ALIAS_LEN    = 15;         /* maximum octets for alias    */
const    CADI__MAX_CMD_NO       = 0x7FFFFFFF; /* maximum number cmd can have */
const    CADI__RADIO_ID_MASK    = 700000;     /* for smartzone               */
const    CADI__TG_ID_MASK       = 800000;     /* for smartzone               */
const    CADI__APCO_TG_ID_MASK  = 80000000;   /* base 80,000,000 for APCO    */
%
%/*
%**-----------------------------------------------------------------------------
%**    General/Common Used variable type
%**-----------------------------------------------------------------------------
%*/
%
typedef int              CADI__CMD_NO;
typedef unsigned long    CADI__GROUP_ID;
typedef char             CADI__GROUP_ALIAS [CADI__MAX_ALIAS_LEN];
typedef char             CADI__RADIO_ALIAS [CADI__MAX_ALIAS_LEN];
typedef unsigned long    CADI__RADIO_ID;
%
%/*  CADI__FS_CHNL is used by GEMINI project */
%
typedef int              CADI__FS_CHNL;

#ifdef RPC_HDR
%#endif    /* H__CADI */
#endif    
