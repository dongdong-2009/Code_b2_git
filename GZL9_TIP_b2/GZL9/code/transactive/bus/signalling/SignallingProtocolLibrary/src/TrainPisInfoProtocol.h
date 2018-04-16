
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\Train1PisInfoTelegram.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/07/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Protocol that used for the trains.
  */

#ifndef _TRAIN_PIS_INFO_PROTOCOLS_H
#define _TRAIN_PIS_INFO_PROTOCOLS_H

namespace TA_IRS_Bus
{

	/**
	 * ETrainPosition
	 */
	enum ETrainPosition
	{
		//TRAIN 1
		TRAIN1_VALIDITYFIELD_POSITION              = 8,
		TRAIN1_UNIT_NUMBER_POSITION                = 9,
		TRAIN1_SERVICE_NUMBER_POSITION             = 11,
		TRAIN1_DESTINATION_ID_POSITION             = 13,
		TRAIN1_PRE_ARRIVAL_POSITION                = 15,
		TRAIN1_SCHEDULE_ARRIVAL_TIME_POSITION      = 16,
		TRAIN1_SCHEDULE_DEPARTURE_TIME_POSITION    = 20,
		TRAIN1_ARRIVAL_STATUS_POSITION             = 24,
		TRAIN1_DEPARTURE_STATUS_POSITION           = 25,
		TRAIN1_HOLD_STATUS_POSITION                = 26,
		TRAIN1_SKIP_STATUS_POSITION                = 27,
		TRAIN1_OUT_OF_SERVICE_POSITION             = 28,
		TRAIN1_LAST_TRAIN_POSITION                 = 29,

		//TRAIN 2
		TRAIN2_VALIDITYFIELD_POSITION              = 30,
		TRAIN2_UNIT_NUMBER_POSITION                = 31,
		TRAIN2_SERVICE_NUMBER_POSITION             = 33,
		TRAIN2_DESTINATION_ID_POSITION             = 35,
		TRAIN2_PRE_ARRIVAL_POSITION                = 37,
		TRAIN2_SCHEDULE_ARRIVAL_TIME_POSITION      = 38,
		TRAIN2_SCHEDULE_DEPARTURE_TIME_POSITION    = 42,
		TRAIN2_ARRIVAL_STATUS_POSITION             = 46,
		TRAIN2_DEPARTURE_STATUS_POSITION           = 47,
		TRAIN2_HOLD_STATUS_POSITION                = 48,
		TRAIN2_SKIP_STATUS_POSITION                = 49,
		TRAIN2_OUT_OF_SERVICE_POSITION             = 50,
		TRAIN2_LAST_TRAIN_POSITION                 = 51,

		//TRAIN 3
		TRAIN3_VALIDITYFIELD_POSITION              = 52,
		TRAIN3_UNIT_NUMBER_POSITION                = 53,
		TRAIN3_SERVICE_NUMBER_POSITION             = 55,
		TRAIN3_DESTINATION_ID_POSITION             = 57,
		TRAIN3_PRE_ARRIVAL_POSITION                = 59,
		TRAIN3_SCHEDULE_ARRIVAL_TIME_POSITION      = 60,
		TRAIN3_SCHEDULE_DEPARTURE_TIME_POSITION    = 64,
		TRAIN3_ARRIVAL_STATUS_POSITION             = 68,
		TRAIN3_DEPARTURE_STATUS_POSITION           = 69,
		TRAIN3_HOLD_STATUS_POSITION                = 70,
		TRAIN3_SKIP_STATUS_POSITION                = 71,
		TRAIN3_OUT_OF_SERVICE_POSITION             = 72,
		TRAIN3_LAST_TRAIN_POSITION                 = 73,

		//TRAIN 4
		TRAIN4_VALIDITYFIELD_POSITION              = 74,
		TRAIN4_UNIT_NUMBER_POSITION                = 75,
		TRAIN4_SERVICE_NUMBER_POSITION             = 77,
		TRAIN4_DESTINATION_ID_POSITION             = 79,
		TRAIN4_PRE_ARRIVAL_POSITION                = 81,
		TRAIN4_SCHEDULE_ARRIVAL_TIME_POSITION      = 82,
		TRAIN4_SCHEDULE_DEPARTURE_TIME_POSITION    = 86,
		TRAIN4_ARRIVAL_STATUS_POSITION             = 90,
		TRAIN4_DEPARTURE_STATUS_POSITION           = 91,
		TRAIN4_HOLD_STATUS_POSITION                = 92,
		TRAIN4_SKIP_STATUS_POSITION                = 93,
		TRAIN4_OUT_OF_SERVICE_POSITION             = 94,
		TRAIN4_LAST_TRAIN_POSITION                 = 95
		
	};

	/**
	 * ETrainNumber
	 */
	enum ETrainNumber
	{
		TRAIN_1 = 0,
		TRAIN_2 = 1,
		TRAIN_3 = 2,
		TRAIN_4 = 3
	};

}
#endif