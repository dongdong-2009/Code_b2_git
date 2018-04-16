/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/Common/BasDefinitions.h $
  * @author:	Huang Jian
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2016/01/28 15:26:34 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  * The definition of BAS in GZL9.
  *
  */
#ifndef APP_BAS_DEFINITIONS_H
#define APP_BAS_DEFINITIONS_H

//defined in data table
enum ModeRunningStatus
{
	eModeStateUnknown 		= 0,
	eModeExecuting 			= 1,
	eModeExecutionFail 		= 2,
	eModeExecutionSuccess 	= 4,
	eModeNoMode			 	= 8
};

/*  value  status, defined in data table
	1		Auto
    2		Manual */
enum BasControlPrivilege
{
    eControlUnknown = 0,
    eControlAuto 	= 1,
    eControlManual 	= 2
};

enum FireRecoverMode
{
    eModeUnknown 	= 0,
    eModeReset 		= 1
};

#endif //APP_BAS_DEFINITIONS_H

