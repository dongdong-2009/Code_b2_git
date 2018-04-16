/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADICommandAdapter.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Public interface to CADI commands
  */

#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"


#if !defined(AFX_CADICOMMANDADAPTER_H__A7873727_1FED_447C_9BB7_8071ADFF12C1__INCLUDED_)
#define AFX_CADICOMMANDADAPTER_H__A7873727_1FED_447C_9BB7_8071ADFF12C1__INCLUDED_

#ifdef __cplusplus
extern "C" 
{
#endif

	CADIStatus cadi_handshake(const char* host);
	CADIStatus cadi_login(const char* userName, const char* password, int isEncrypted);
	CADIStatus cadi_logout(void);
	CADIStatus cadi_regroup(int radioId, int groupId);
	CADIStatus cadi_cancelRegroup(int radioId);
	CADIStatus cadi_cancelRegroupWGid(int radioId, int groupId);

    void cadi_cleanup();

#ifdef __cplusplus
}
#endif

#endif
