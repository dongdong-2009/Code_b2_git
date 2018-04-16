/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/stub/rpc/rpc.c $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Stub library for RPC call in Windows environment
  */

#include "rpc.h"
#include "app\radio\radio_protocol\MotorolaElite\CADIRPC\src\stub\cadcmd.h"

    void clnt_geterr(CLIENT* clnt, struct rpc_err* errp)
    {
    }

    CLIENT* clnt_create(char* host, u_long prog, u_long vers, char* proto)
    {
		CADIConnect(host);
        return (CLIENT*) 1;
    }

    void clnt_destroy(CLIENT* clnt)
    {
    }

    bool_t clnt_control(CLIENT *cl, u_long req, char* info)
    {
        return 1;
    }

    enum clnt_stat clnt_call (CLIENT* clnt, u_long procnum, 
        void* inproc, char* in, void* outproc, char* out, struct timeval tout)
    {
		switch(procnum)
		{
		case CADCMD__REGROUP:	//regroup
			CADIRegroup(in);
			break;
		case CADCMD__XREGROUP_W_GID:	//cancel regroup
			CADICancelRegroup(in);
			break;
		case CADCMD__LOGIN:
			if (!CADILogin(in))
			{
				return -1;
			}
			break;
		case CADCMD__LOGOUT:
			CADILogout();
			break;
		}
        return RPC_SUCCESS;
    }

    SVCXPRT *svcudp_create(int sock)
    {
        return 0;
    }

    SVCXPRT *svctcp_create (int sock, u_int sendsz, u_int recvsz)
    {
        return (SVCXPRT*) 1;
    }

    bool_t svc_sendreply (SVCXPRT* xprt, void* outproc, char* out)
    {
        return 1;
    }

    void svc_run(void)
    {
    }


    bool_t svc_register (SVCXPRT* xprt, u_long prognum, u_long versnum, void* dispatch, int protocol)
    {
        return 1;
    }

    void pmap_unset (u_long prognum, u_long versnum)
    {
    }

    void svcerr_noproc (SVCXPRT* xprt)
    {
    }

    void svcerr_decode(SVCXPRT* xprt)
    {
    }

    void svcerr_systemerr(SVCXPRT* xprt)
    {
    }

    bool_t svc_getargs (SVCXPRT* xprt, void* inproc, char* in)
    {
        return 1;
    }

    bool_t svc_freeargs (SVCXPRT* xprt, void* inproc, char* in)
    {
        return 1;
    }

    bool_t xdr_void()
    {
        return 1;
    }


    bool_t xdr_u_long(XDR* xdrs, u_long* p)
    {
        return 1;
    }


    