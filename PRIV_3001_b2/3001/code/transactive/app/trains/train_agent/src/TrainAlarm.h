/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainAlarm.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainAlarm.h
//  Implementation of the Class TrainAlarm
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#if !defined(TRAINAlarm_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
#define TRAINAlarm_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_

#include <string>

#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"

namespace TA_IRS_App
{
	class ITrainManager;
    class misc0A1TableEventReader;
    class miscATCAlarmEventReader;
	class OperationModeManager;
	class TrainActivityMonitor;

	/**
	* This class is responsible for handling alarm tables.  There are two
	* types of alarm tables; AO1 and ATC.  The table SDS messages are received
	* from a train via TIMS and the Radio Agent.  The tables are sent to the
	* ATS Agent.  If the tables are not recieved within their expected time
	* an alarm is raised.
	*
	* If an alarm table SDS message is received for a train that the Train
	* Agent is not in control of then the Agent takes control of that train.
	*/
    class TrainAlarm : public ITrainEventReaderDelegator
    {

    public:
        
        TrainAlarm(ITrainManager& callback, ITrainNotificationFacade& notifier,
				   OperationModeManager& operationModeManager,
			       TrainActivityMonitor& trainActivityMonitor);
        virtual ~TrainAlarm();

	public:
        bool delegate(TrainEvent& event);


        //*** Asynchronous-unsolicited messages

        // origin TIMS (61) OA1 Table
        void onOA1Table(TrainEvent & event, misc0A1TableEventReader * reader);

        // origin TIMS (62) ATC Alarm
        void onATCAlarm(TrainEvent & event, miscATCAlarmEventReader * reader);
        
	private:

		ITrainManager& m_trainManager;
        ITrainNotificationFacade& m_notifier;
		OperationModeManager& m_operationModeManager;
		TrainActivityMonitor& m_trainActivityMonitor;
    };

};
#endif // !defined(TRAINAlarm_H__9205ED3E_1BC0_419E_8DF1_5C1269E06EE8__INCLUDED_)
