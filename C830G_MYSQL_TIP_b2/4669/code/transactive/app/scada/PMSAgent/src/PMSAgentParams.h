#ifndef PMSAGENTPARAMS_H
#define PMSAGENTPARAMS_H

/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/PMSAgentParams.h $
  * @author  hoav
  * @version $Revision: #4 $
  * Last modification : $DateTime: 2011/03/29 10:23:27 $
  * Last modified by : $Author: builder $
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
	WORD13_RTU_STATUS_TABLE = 33,	// POWER1
	WORD14_RTU_STATUS_TABLE = 34,	// POWER2
	WORD15_RTU_STATUS_TABLE = 35,	// POWER3
	WORD16_RTU_STATUS_TABLE = 80,	// CLOCK
	WORD17_RTU_STATUS_TABLE = 128,	// WILD
	WORD18_RTU_STATUS_TABLE = 144,	// ECS
	WORD19_RTU_STATUS_TABLE = 160,	// FIRE
	WORD20_RTU_STATUS_TABLE = 176,	// TWP
	WORD21_RTU_STATUS_TABLE = 209,	// PSD1
	WORD22_RTU_STATUS_TABLE = 210,	// PSD2
	WORD23_RTU_STATUS_TABLE = 211,	// PSD3
	WORD24_RTU_STATUS_TABLE = 212,	// PSD4
	WORD25_RTU_STATUS_TABLE = 17,	// PLC1
	WORD26_RTU_STATUS_TABLE = 18,	// PLC2
	WORD27_RTU_STATUS_TABLE = 19,	// PLC3
	WORD28_RTU_STATUS_TABLE = 20,	// PLC4
	WORD29_RTU_STATUS_TABLE = 21,	// PLC5
	WORD30_RTU_STATUS_TABLE = -2,	// UNDEFINED
	WORD31_RTU_STATUS_TABLE = -3	// UNDEFINED
} SWCSystemID;

#endif
