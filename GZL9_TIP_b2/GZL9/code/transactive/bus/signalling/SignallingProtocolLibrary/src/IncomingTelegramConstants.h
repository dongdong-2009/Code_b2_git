/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/IncomingTelegramConstants.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * IncomingTelegramProcessor Constants
  */

#ifndef IncomingTelegramConstants_6350EE6A_7BA6_43f1_9062_DB89A2F20A88__INCLUDED_
#define IncomingTelegramConstants_6350EE6A_7BA6_43f1_9062_DB89A2F20A88__INCLUDED_


/*
//Constants
//TrainDataProcessor
static const unsigned char TEST_RADIO_MESSAGE_TYPE;
static const unsigned char REFRESH_RADIO_MESSAGE_TYPE;
static const unsigned char TRAIN_DELETE_RADIO_MESSAGE_TYPE;
static const unsigned char TRAIN_POSITION_RADIO_MESSAGE_TYPE;
static const unsigned char UPDATE_DATA_RADIO_MESSAGE_TYPE;
static const unsigned char NEW_TRAIN_RADIO_MESSAGE_TYPE;
static const unsigned char LEAVING_MAIN_LINE_RADIO_MESSAGE_TYPE;
static const unsigned char ENTERING_MAIN_LINE_TRAIN_RADIO_MESSAGE_TYPE;
static const unsigned char DIRECTION_CHANGE_RADIO_MESAGE_TYPE;

//LessDataProcessor
static const unsigned char TEST_LESS_MESSAGE_TYPE;
static const unsigned char ALARM_CLEAR_LESS_MESSAGE_TYPE;
static const unsigned char ALARM_LESS_MESSAGE_TYPE;

//PlatformDataProcessor
static const unsigned char EARLY_APPROACH_PIDS_TYPE;
static const unsigned char DWELL_TIME_PIDS_TYPE;
static const unsigned char HOLD_PIDS_TYPE; 
static const unsigned char INFORMATION_PIDS_TYPE;
static const unsigned char TEST_MSG_PIDS_TYPE;
*/


//TrainDataProcessor
static const unsigned char TEST_RADIO_MESSAGE_TYPE                         = 'T';
static const unsigned char REFRESH_RADIO_MESSAGE_TYPE                      = 'R';
static const unsigned char TRAIN_DELETE_RADIO_MESSAGE_TYPE                 = 'E';
static const unsigned char TRAIN_POSITION_RADIO_MESSAGE_TYPE               = 'P';
static const unsigned char UPDATE_DATA_RADIO_MESSAGE_TYPE                  = 'U';
static const unsigned char NEW_TRAIN_RADIO_MESSAGE_TYPE                    = 'N';
static const unsigned char LEAVING_MAIN_LINE_RADIO_MESSAGE_TYPE            = 'L';
static const unsigned char ENTERING_MAIN_LINE_TRAIN_RADIO_MESSAGE_TYPE     = 'D';
static const unsigned char DIRECTION_CHANGE_RADIO_MESAGE_TYPE              = 'M';

//LessDataProcessor
static const unsigned char TEST_LESS_MESSAGE_TYPE           = 'T';
static const unsigned char ALARM_CLEAR_LESS_MESSAGE_TYPE    = 'C';
static const unsigned char ALARM_LESS_MESSAGE_TYPE          = 'A';

//PlatformDataProcessor
static const unsigned char EARLY_APPROACH_PIDS_TYPE      = 'A';
static const unsigned char DWELL_TIME_PIDS_TYPE          = 'D';
static const unsigned char HOLD_PIDS_TYPE                = 'H';
static const unsigned char INFORMATION_PIDS_TYPE         = 'I';
static const unsigned char TEST_MSG_PIDS_TYPE            = 'T';


#endif