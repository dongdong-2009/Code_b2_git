/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADICommand.c $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include <rpc/rpc.h>


#include "cadcmd.h"

/*
**-----------------------------------------------------------------------------
**    Define constants
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
**    Define fields specific to CADCMD
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
**    Define command error/return code
**-----------------------------------------------------------------------------
*/


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


/*
**----------------------------------------------------------------------------
** cadcmd__failsoft()     *** THIS IS SPECIFICALLY FOR SMARTNET SYSTEM ***
**----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
** cadcmd__login()
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
** Application Programming Interface for CADI Commands
**-----------------------------------------------------------------------------
*/


/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

u_long *
cadcmd__handshake_1(argp, clnt)
	void *argp;
	CLIENT *clnt;
{
	static u_long res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__HANDSHAKE, xdr_void, argp, xdr_u_long, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__login_1(argp, clnt)
	CADCMD__LOGIN_DATA *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__LOGIN, xdr_CADCMD__LOGIN_DATA, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__logout_1(argp, clnt)
	void *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__LOGOUT, xdr_void, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__zc_query_1(argp, clnt)
	void *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__ZC_QUERY, xdr_void, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__regroup_1(argp, clnt)
	CADCMD__REGROUP_DATA *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__REGROUP, xdr_CADCMD__REGROUP_DATA, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__xregroup_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__XREGROUP, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__inhibit_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__INHIBIT, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__xinhibit_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__XINHIBIT, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__lock_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__LOCK, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__unlock_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__UNLOCK, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__radio_check_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__RADIO_CHECK, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__failsoft_1(argp, clnt)
	CADCMD__FAILSOFT_DATA *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__FAILSOFT, xdr_CADCMD__FAILSOFT_DATA, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__xfailsoft_1(argp, clnt)
	CADI__RADIO_ID *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__XFAILSOFT, xdr_CADI__RADIO_ID, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


CADCMD__STATUS *
cadcmd__xregroup_w_gid_1(argp, clnt)
	CADCMD__REGROUP_DATA *argp;
	CLIENT *clnt;
{
	static CADCMD__STATUS res;

	bzero((char *)&res, sizeof(res));
	if (clnt_call(clnt, CADCMD__XREGROUP_W_GID, xdr_CADCMD__REGROUP_DATA, argp, xdr_CADCMD__STATUS, &res, TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

