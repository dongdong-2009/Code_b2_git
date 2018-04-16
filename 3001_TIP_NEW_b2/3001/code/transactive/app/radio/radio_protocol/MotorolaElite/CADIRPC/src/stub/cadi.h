#ifndef H__CADI
#define H__CADI

/*
**-----------------------------------------------------------------------------
**    Command number range is 1 - 0x7FFFFFFF inclusive.  Once it reaches the
**    upper limit, it will restart from 1.
**-----------------------------------------------------------------------------
*/

#define CADI__MAX_ALIAS_LEN 15
#define CADI__MAX_CMD_NO 0x7FFFFFFF
#define CADI__RADIO_ID_MASK 700000
#define CADI__TG_ID_MASK 800000
#define CADI__APCO_TG_ID_MASK 80000000

/*
**-----------------------------------------------------------------------------
**    General/Common Used variable type
**-----------------------------------------------------------------------------
*/


typedef int CADI__CMD_NO;
bool_t xdr_CADI__CMD_NO();


typedef u_long CADI__GROUP_ID;
bool_t xdr_CADI__GROUP_ID();


typedef char CADI__GROUP_ALIAS[CADI__MAX_ALIAS_LEN];
bool_t xdr_CADI__GROUP_ALIAS();


typedef char CADI__RADIO_ALIAS[CADI__MAX_ALIAS_LEN];
bool_t xdr_CADI__RADIO_ALIAS();


typedef u_long CADI__RADIO_ID;
bool_t xdr_CADI__RADIO_ID();


/*  CADI__FS_CHNL is used by GEMINI project */


typedef int CADI__FS_CHNL;
bool_t xdr_CADI__FS_CHNL();

#endif /* H__CADI */
