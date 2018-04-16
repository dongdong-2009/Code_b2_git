#ifndef H__CADCMD
#define H__CADCMD

#include "cadi.h"


/*
**-----------------------------------------------------------------------------
**    Define constants
**-----------------------------------------------------------------------------
*/

#define CADCMD__FALSE 0
#define CADCMD__HOSTNAME tempesthost
#define CADCMD__MAX_USERNAME_LEN 9
#define CADCMD__MAX_PASSWORD_LEN 31
#define CADCMD__TRUE 1

/*
**-----------------------------------------------------------------------------
**    Define fields specific to CADCMD
**-----------------------------------------------------------------------------
*/


typedef int CADCMD__BOOL;
bool_t xdr_CADCMD__BOOL();


typedef char CADCMD__PASSWD[CADCMD__MAX_PASSWORD_LEN];
bool_t xdr_CADCMD__PASSWD();


typedef CADI__CMD_NO CADCMD__STATUS;
bool_t xdr_CADCMD__STATUS();


typedef char CADCMD__USER_NAME[CADCMD__MAX_USERNAME_LEN];
bool_t xdr_CADCMD__USER_NAME();


/*
**-----------------------------------------------------------------------------
**    Define command error/return code
**-----------------------------------------------------------------------------
*/

#define CADCMD__SUCCESS 0
#define CADCMD__FAILURE -1
#define CADCMD__LOGIN_REQUIRED -2
#define CADCMD__ZC_CONN_FAILED -3
#define CADCMD__INVALID_RADIO -4
#define CADCMD__INVALID_COMMAND -5
#define CADCMD__INVALID_TARGET -6
#define CADCMD__NO_PERMISSION -7

/*
**-----------------------------------------------------------------------------
**    Define command arguments
**
**    The following commands have the same data type for their arguments:
**
**    CADI__RADIO_ID:
**          cadcmd__lock,
**          cadcmd__unlock,
**          cadcmd__inhibit,
**          cadcmd__xinhibit,
**          cadcmd__xregroup,
**          cadcmd__radio_check
**          cadcmd__xfailsoft
**-----------------------------------------------------------------------------
** cadcmd__regroup()
** cadcmd__xregroup_w_gid()
**-----------------------------------------------------------------------------
*/


struct CADCMD__REGROUP_DATA {
	CADI__RADIO_ID target_radio_id;
	CADI__GROUP_ID target_group_id;
};
typedef struct CADCMD__REGROUP_DATA CADCMD__REGROUP_DATA;
bool_t xdr_CADCMD__REGROUP_DATA();


/*
**----------------------------------------------------------------------------
** cadcmd__failsoft()     *** THIS IS SPECIFICALLY FOR SMARTNET SYSTEM ***
**----------------------------------------------------------------------------
*/


struct CADCMD__FAILSOFT_DATA {
	CADI__RADIO_ID target_radio_id;
	CADI__FS_CHNL target_fs_chnl;
};
typedef struct CADCMD__FAILSOFT_DATA CADCMD__FAILSOFT_DATA;
bool_t xdr_CADCMD__FAILSOFT_DATA();


/*
**-----------------------------------------------------------------------------
** cadcmd__login()
**-----------------------------------------------------------------------------
*/


struct CADCMD__LOGIN_DATA {
	CADCMD__USER_NAME user_name;
	CADCMD__PASSWD password;
	CADCMD__BOOL is_password_encrypted;
};
typedef struct CADCMD__LOGIN_DATA CADCMD__LOGIN_DATA;
bool_t xdr_CADCMD__LOGIN_DATA();


/*
**-----------------------------------------------------------------------------
** Application Programming Interface for CADI Commands
**-----------------------------------------------------------------------------
*/


#define CADCMD__PROG ((u_long)0x20000001)
#define CADCMD__VERS ((u_long)1)
#define CADCMD__HANDSHAKE ((u_long)100)
extern u_long *cadcmd__handshake_1();
#define CADCMD__LOGIN ((u_long)101)
extern CADCMD__STATUS *cadcmd__login_1();
#define CADCMD__LOGOUT ((u_long)102)
extern CADCMD__STATUS *cadcmd__logout_1();
#define CADCMD__ZC_QUERY ((u_long)103)
extern CADCMD__STATUS *cadcmd__zc_query_1();
#define CADCMD__REGROUP ((u_long)105)
extern CADCMD__STATUS *cadcmd__regroup_1();
#define CADCMD__XREGROUP ((u_long)106)
extern CADCMD__STATUS *cadcmd__xregroup_1();
#define CADCMD__INHIBIT ((u_long)107)
extern CADCMD__STATUS *cadcmd__inhibit_1();
#define CADCMD__XINHIBIT ((u_long)108)
extern CADCMD__STATUS *cadcmd__xinhibit_1();
#define CADCMD__LOCK ((u_long)109)
extern CADCMD__STATUS *cadcmd__lock_1();
#define CADCMD__UNLOCK ((u_long)110)
extern CADCMD__STATUS *cadcmd__unlock_1();
#define CADCMD__RADIO_CHECK ((u_long)111)
extern CADCMD__STATUS *cadcmd__radio_check_1();
#define CADCMD__FAILSOFT ((u_long)112)
extern CADCMD__STATUS *cadcmd__failsoft_1();
#define CADCMD__XFAILSOFT ((u_long)113)
extern CADCMD__STATUS *cadcmd__xfailsoft_1();
#define CADCMD__XREGROUP_W_GID ((u_long)114)
extern CADCMD__STATUS *cadcmd__xregroup_w_gid_1();

#endif /* H__CADCMD */
