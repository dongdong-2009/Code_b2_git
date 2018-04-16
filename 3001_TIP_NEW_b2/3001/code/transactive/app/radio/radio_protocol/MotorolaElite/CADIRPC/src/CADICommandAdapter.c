/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADICommandAdapter.c $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Public interface to CADI commands
  */


#include <string.h>
#include <stdio.h>

#include <rpc/rpc.h>


#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADICommandAdapter.h"
#include "cadcmd.h"
#include "cadev.h"


static CLIENT* sCadiClient = NULL;
unsigned long gCadiProgramNumber = 0;


static int sIsLoggedIn = 0;

CADCMD__STATUS cadi_handshake(const char* host)
{
    struct timeval rpcTimeout;
    u_long *pRtn = NULL;
    int nTryCnt = 0;

	if (sCadiClient == NULL)
	{
	    // Create a connection to the Server.
		sCadiClient = clnt_create(host, CADCMD__PROG, CADCMD__VERS, "tcp");
    
		if ( sCadiClient == NULL )
		{
            printf("Could not establish RPC connection to server");

			return CADCMD__FAILURE;
		}

		// Reset the timeout.
		rpcTimeout.tv_sec = 15;
		rpcTimeout.tv_usec = 0;
		if (! clnt_control(sCadiClient, CLSET_TIMEOUT, (char*)(&rpcTimeout) ))
		{
            printf("Set timeout failed");
			return CADCMD__FAILURE;
		}
	}
	

	// Handshake - try 3 times.

	while (pRtn == NULL && nTryCnt < 3)
	{
		pRtn = cadcmd__handshake_1(0, sCadiClient);
		nTryCnt++;
	}

	if (pRtn == NULL)
	{
        printf("Handshake failed");
		return CADCMD__FAILURE;
	}

    if (*pRtn == CADI_INVALID_PROGRAM_NUMBER)
	{
        printf("Invalid RPC program number from handshake");
		return CADCMD__FAILURE;
	}

	// Set the shared state
	gCadiProgramNumber = *pRtn; // the event api 

	// Reset the timeout.
	rpcTimeout.tv_sec = 15;
	rpcTimeout.tv_usec = 0;
	if (! clnt_control(sCadiClient, CLSET_TIMEOUT, (char*)(&rpcTimeout) ))
	{
        printf("Set timeout failed");
		return CADCMD__FAILURE;
	}
	return CADCMD__SUCCESS;
}

CADCMD__STATUS cadi_login(const char* userName, const char* password, int isEncrypted)
{
    static CADCMD__LOGIN_DATA     login;
	CADCMD__STATUS* ret = NULL;

    strncpy(login.user_name, userName, sizeof(login.user_name) );
    strncpy(login.password, password, sizeof(login.password) );
    login.is_password_encrypted = isEncrypted;

	ret = cadcmd__login_1(&login, sCadiClient);
	if (ret == NULL)
	{
		return CADCMD__FAILURE;
	}

	sIsLoggedIn = (*ret == CADCMD__SUCCESS);
	return *ret;
}

CADCMD__STATUS cadi_logout(void)
{
	CADCMD__STATUS* ret = cadcmd__logout_1(0, sCadiClient);

	if (ret == NULL)
	{
		return CADCMD__FAILURE;
	}

	sIsLoggedIn = 0;
	return *ret;
}

CADCMD__STATUS cadi_regroup(int radioId, int groupId)
{
	CADCMD__REGROUP_DATA  data;
    CADCMD__STATUS* ret;

	data.target_radio_id = radioId;
	data.target_group_id = groupId;
	ret = cadcmd__regroup_1(&data, sCadiClient);

	if (ret == NULL)
	{
		return CADCMD__FAILURE;
	}

	return *ret;
}

CADCMD__STATUS cadi_cancelRegroup(int radioId)
{
	CADI__RADIO_ID        radio_id = radioId;
	CADCMD__STATUS* ret = cadcmd__xregroup_1(&radio_id, sCadiClient);

	if (ret == NULL)
	{
		return CADCMD__FAILURE;
	}

	return *ret;
}

CADCMD__STATUS cadi_cancelRegroupWGid(int radioId, int groupId)
{
    CADCMD__STATUS* ret;
	CADCMD__REGROUP_DATA  data;
	data.target_radio_id = radioId;
	data.target_group_id = groupId;

	ret = cadcmd__xregroup_w_gid_1(&data, sCadiClient);

	if (ret == NULL)
	{
		return CADCMD__FAILURE;
	}

	return *ret;
}

void cadi_clean_up()
{
    if (sCadiClient != NULL)
    {
        if (sIsLoggedIn)
        {
           cadcmd__logout_1(0, sCadiClient);
           sIsLoggedIn = 0;
        }
        clnt_destroy( sCadiClient );
        sCadiClient = NULL;
    }
}

