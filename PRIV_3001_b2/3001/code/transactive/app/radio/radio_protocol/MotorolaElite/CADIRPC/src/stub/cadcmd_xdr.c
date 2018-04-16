#include "rpc/rpc.h"
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



bool_t
xdr_CADCMD__BOOL(xdrs, objp)
	XDR *xdrs;
	CADCMD__BOOL *objp;
{
	return (TRUE);
}




bool_t
xdr_CADCMD__PASSWD(xdrs, objp)
	XDR *xdrs;
	CADCMD__PASSWD objp;
{
	return (TRUE);
}




bool_t
xdr_CADCMD__STATUS(xdrs, objp)
	XDR *xdrs;
	CADCMD__STATUS *objp;
{
	return (TRUE);
}




bool_t
xdr_CADCMD__USER_NAME(xdrs, objp)
	XDR *xdrs;
	CADCMD__USER_NAME objp;
{
	return (TRUE);
}



/*
**-----------------------------------------------------------------------------
**    Define command error/return code
**-----------------------------------------------------------------------------
*/




bool_t
xdr_CADCMD__REGROUP_DATA(xdrs, objp)
	XDR *xdrs;
	CADCMD__REGROUP_DATA *objp;
{
	return (TRUE);
}



/*
**----------------------------------------------------------------------------
** cadcmd__failsoft()     *** THIS IS SPECIFICALLY FOR SMARTNET SYSTEM ***
**----------------------------------------------------------------------------
*/



bool_t
xdr_CADCMD__FAILSOFT_DATA(xdrs, objp)
	XDR *xdrs;
	CADCMD__FAILSOFT_DATA *objp;
{
	return (TRUE);
}



/*
**-----------------------------------------------------------------------------
** cadcmd__login()
**-----------------------------------------------------------------------------
*/



bool_t
xdr_CADCMD__LOGIN_DATA(xdrs, objp)
	XDR *xdrs;
	CADCMD__LOGIN_DATA *objp;
{
	return (TRUE);
}



/*
**-----------------------------------------------------------------------------
** Application Programming Interface for CADI Commands
**-----------------------------------------------------------------------------
*/

