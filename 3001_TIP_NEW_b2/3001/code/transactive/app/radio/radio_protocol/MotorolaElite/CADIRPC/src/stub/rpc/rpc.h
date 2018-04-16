/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/stub/rpc/rpc.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Stub library for RPC call in Windows environment
  */

#include <memory.h>

#ifndef __RPCSTUB23490234824__
#define __RPCSTUB23490234824__

typedef int CLIENT;
typedef int SVCXPRT;
typedef int bool_t;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;

typedef int XDR;

#define CLSET_TIMEOUT 0
#define RPC_ANYSOCK 1

#ifndef IPPROTO_UDP
#define IPPROTO_UDP 2
#define IPPROTO_TCP 3
#endif

#define NULLPROC 0

#define FALSE 0
#define TRUE 1

#ifndef NULL
    #define NULL 0
#endif

typedef	bool_t (*xdrproc_t)();

enum clnt_stat {
    RPC_SUCCESS=0,			/* call succeeded */
    /*
    * local errors
    */
    RPC_CANTENCODEARGS=1,		/* can't encode arguments */
    RPC_CANTDECODERES=2,		/* can't decode results */
    RPC_CANTSEND=3,			/* failure in sending call */
    RPC_CANTRECV=4,			/* failure in receiving result */
    RPC_TIMEDOUT=5,			/* call timed out */
    /*
    * remote errors
    */
    RPC_VERSMISMATCH=6,		/* rpc versions not compatible */
    RPC_AUTHERROR=7,		/* authentication error */
    RPC_PROGUNAVAIL=8,		/* program not available */
    RPC_PROGVERSMISMATCH=9,		/* program version mismatched */
    RPC_PROCUNAVAIL=10,		/* procedure unavailable */
    RPC_CANTDECODEARGS=11,		/* decode arguments error */
    RPC_SYSTEMERROR=12,		/* generic "other problem" */

    /*
    * callrpc & clnt_create errors
    */
    RPC_UNKNOWNHOST=13,		/* unknown host name */
    RPC_UNKNOWNPROTO=17,		/* unkown protocol */

    /*
    * _ create errors
    */
    RPC_PMAPFAILURE=14,		/* the pmapper failed in its call */
    RPC_PROGNOTREGISTERED=15,	/* remote program is not registered */
    /*
    * unspecified error
    */
    RPC_FAILED=16
};

struct rpc_err
{
    int err_code;
};

#ifndef _SYS_TIME_H
struct timeval
{
    int tv_sec;
    int tv_usec;
};
#endif

struct svc_req
{
    u_long		rq_prog;	/* service program number */
    u_long		rq_vers;	/* service protocol version */
    u_long		rq_proc;	/* the desired procedure */
//    struct opaque_auth rq_cred;	/* raw creds from the wire */
//    caddr_t		rq_clntcred;	/* read only cooked cred */
	SVCXPRT	*rq_xprt;		/* associated transport */
};


#ifdef __cplusplus
extern "C" 
{
#endif

    void clnt_geterr(CLIENT* clnt, struct rpc_err* errp);

    CLIENT* clnt_create(char* host, u_long prog, u_long vers, char* proto);
    void clnt_destroy(CLIENT* clnt);
    bool_t clnt_control(CLIENT *cl, u_long req, char* info);
    enum clnt_stat clnt_call (CLIENT* clnt, u_long procnum, 
        void* inproc, char* in, void* outproc, char* out, struct timeval tout);

    SVCXPRT *svcudp_create(int sock);
    SVCXPRT *svctcp_create (int sock, u_int sendsz, u_int recvsz);

    bool_t svc_sendreply (SVCXPRT* xprt, void* outproc, char* out);
    void svc_run(void);

    bool_t svc_register (SVCXPRT* xprt, u_long prognum, u_long versnum, void* dispatch, int protocol);
    void pmap_unset (u_long prognum, u_long versnum);
    void svcerr_noproc (SVCXPRT* xprt);
    void svcerr_decode(SVCXPRT* xprt);
    void svcerr_systemerr(SVCXPRT* xprt);
    bool_t svc_getargs (SVCXPRT* xprt, void* inproc, char* in);
    bool_t svc_freeargs (SVCXPRT* xprt, void* inproc, char* in);

    bool_t xdr_void();
    bool_t xdr_u_long(XDR* xdrs, u_long* p);

    #define bzero(v, sz) memset(v, 0, sz)

     
#ifdef __cplusplus
}
#endif

#endif
