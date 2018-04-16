/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsConstants.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This represents the ATS Constants value based on ICD Requirement
  */

#ifndef _ATS_CONSTANTS_H_
#define _ATS_CONSTANTS_H_

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsPasData.h"
#include <string>
#include <map>
#include <vector>

namespace TA_IRS_App
{
	typedef std::map<TA_IRS_Bus::PlatformIdType, TA_IRS_Bus::AtsPasData*> PasDataMap;
	typedef PasDataMap::iterator PasDataMapIt;

	//The Information below is Constant value based on ICD Requirement.
	//The values are fixed and can't be changed unless ICD Requirement has changed.
	const int MODBUS_START_ADDRESS           = 1;    //ModBus Start Address
	const int MODBUS_END_ADDRESS             = 1123; //ModBus End Address(in WORD), equivalent to 2246 Bytes
	//Constant value for PA Information
	const int MODBUS_BYTE_SIZE               = 2;    //MODBUS BYTE SIZE
	const int NUMBER_OF_PLATFORM_POS         = 403 - MODBUS_START_ADDRESS;  //Position for Number of Platform
	const int TRAIN_DEST_POS                 = 1;    //Position for Train Destination Station
	const int SKIP_POS                       = 8;	 //Position for Skip
	const int DEPOT_POS                      = 9;    //Position for Depot
	const int PRE_ARRIVAL_POS                = 12;   //Position for Pre-Arrival
	const int ARRIVAL_POS                    = 13;   //Position for Arrival
	const int PLATFORM_PA_RECORD_SIZE        = 1;    //PA Record size(in WORD)
	const int PLATFORM_PA_START_ADDRESS_POS  = 1044; //PA Start Address 
	const int PLATFORM_TRAIN_ARRIVAL_ADDRESS = 408 - MODBUS_START_ADDRESS;  //Platform Train Arrival Address
	

	//The Information below is Constant value based on ICD Requirement.
	//Constant value for Train Information
	const int NUMBER_OF_TRAIN_POS            = 2 - MODBUS_START_ADDRESS; //Position for Number of Trains
	const int TRAIN_START_ADDRESS_POS        = 3; //Position for Train Start Address
	const int TRAIN_RECORD_SIZE              = 4; //Train Record Size(in WORD)

	//Train Data position based on the byte Index
	const int TRAIN_CONGESTION_POS           = 1; //Position for Train Congestion
	const int TRAIN_SERVICE_NUM_POS	         = 2; //Position for Train Service Number
	const int TRAIN_DIRECTION_POS            = 5; //Position for Train Direction
	const int TRAIN_LOCATION_POS             = 6; //Position for Train Location

	//The Information below is Constant value based on ICD Requirement.
	//Constant value for Power Status
	const int POWER_START_ADDRESS            = 1501;  //Power Start Address 
	const int POWER_BIT_POS                  = 0x0100; // Power Bit Position

	//The Information below is Constant value based on ICD Requirement.
	//Constant value for PIS Information
	const int TRAIN_PIS_START_ADDRESS        = 404; //PIS Start Address
	const int TRAIN_PIS_RECORD_SIZE          = 8;   //PIS Record size(in WORD) 

}
#endif