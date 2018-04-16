#ifndef PMSAGENTPARAMS_H
#define PMSAGENTPARAMS_H

/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/PMSAgent/src/PMSAgentParams.h $
  * @author  hoav
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	This files contains the external subsystem IDs that have
  * been assigned to the word 13 to 31 of the RTU Status Table.
  *
  * NOTE: This file is to be modified to meet the project requirement.
  *
  */


//
// External SWC System ID associated with words 13 to 31 of the RTU Status Table
//

typedef enum
{
	WORD13_RTU_STATUS_TABLE = 1,	// PLC1
	WORD14_RTU_STATUS_TABLE = 144,	// ECS1
	WORD15_RTU_STATUS_TABLE = 49,	// FPS1
	WORD16_RTU_STATUS_TABLE = 33,	// POWER1
	WORD17_RTU_STATUS_TABLE = 34,	// POWER2
	WORD18_RTU_STATUS_TABLE = 35,	// POWER3
	WORD19_RTU_STATUS_TABLE = 128,	// ECS2
	WORD20_RTU_STATUS_TABLE = 50,	// FPS2
	WORD21_RTU_STATUS_TABLE = 9,	// SP
	WORD22_RTU_STATUS_TABLE = 10,	// SP
	WORD23_RTU_STATUS_TABLE = 11,	// SP
	WORD24_RTU_STATUS_TABLE = 12,	// SP
	WORD25_RTU_STATUS_TABLE = 13,	// SP
	WORD26_RTU_STATUS_TABLE = 14,	// SP
	WORD27_RTU_STATUS_TABLE = 15,	// SP
	WORD28_RTU_STATUS_TABLE = 16,	// SP
	WORD29_RTU_STATUS_TABLE = 17,	// UNDEFINED
	WORD30_RTU_STATUS_TABLE = 18,	// UNDEFINED
	WORD31_RTU_STATUS_TABLE = 19	// UNDEFINED
} SWCSystemID;

#endif
