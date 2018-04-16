#include "rpc/rpc.h"
#include "cadi.h"
#include "sys_rpc.h"


bool_t
xdr_CADI__CMD_NO(xdrs, objp)
	XDR *xdrs;
	CADI__CMD_NO *objp;
{
	return (TRUE);
}




bool_t
xdr_CADI__GROUP_ID(xdrs, objp)
	XDR *xdrs;
	CADI__GROUP_ID *objp;
{
	return (TRUE);
}




bool_t
xdr_CADI__GROUP_ALIAS(xdrs, objp)
	XDR *xdrs;
	CADI__GROUP_ALIAS objp;
{
	return (TRUE);
}




bool_t
xdr_CADI__RADIO_ALIAS(xdrs, objp)
	XDR *xdrs;
	CADI__RADIO_ALIAS objp;
{
	return (TRUE);
}




bool_t
xdr_CADI__RADIO_ID(xdrs, objp)
	XDR *xdrs;
	CADI__RADIO_ID *objp;
{
	return (TRUE);
}



/*  CADI__FS_CHNL is used by GEMINI project */



bool_t
xdr_CADI__FS_CHNL(xdrs, objp)
	XDR *xdrs;
	CADI__FS_CHNL *objp;
{
	return (TRUE);
}


