/* DataCorrelation
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/DataCorrelation.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This class is responsible for performing the Data Correlation function between the WILD and ATS 
 * subsystems. Correlation is triggered by the DataPointCache class when it receives an update
 * for the TEXT data point proxy.
 */

#if !defined(AFX_DATACORRELATION_H__6658D5CB_2313_4976_B760_1084A0E05AEC__INCLUDED_)
#define AFX_DATACORRELATION_H__6658D5CB_2313_4976_B760_1084A0E05AEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/ats/ats_agent/IDL/src/IAtsWildCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/src/AuditMessageSender.h"
#include <deque>
#include <vector>

// Forward declarations.

namespace TA_Base_Core
{
	class IEntityData;
}

namespace TA_IRS_App
{
	class DataCorrelation
	{
	public:
	// Member funtions
		DataCorrelation();
		virtual ~DataCorrelation();

		//
		// correlateData
		//
		// This method simply calls extractWILDData and parseWILDData, the two methods
		// which performs the actual correlation.
		//
		// @param The dataPoint value string.
		// 
		void correlateData(const std::string& dataPointValue);

		// Typedefs and structs
		typedef struct
		{
			unsigned long DateTime;
			//std::vector<std::string> wheelAxleAlarms;
			std::vector<bool> wheelAxleAlarms;
			bool isCriticalAlarm;
		} WILDDetails;

		// This enumerated type is used 
		typedef enum
        {
            isFALSE = 0,
            isTRUE = 1,
            NA = 2
        } EIsTrainGoingInDirectionA;
		
	private:
		// Disable the copy constructor and assignment operator so the compiler won't create its own version.
		// Reference: Effective C++
		DataCorrelation( const DataCorrelation& DataCorrelation);
		DataCorrelation& operator=(const DataCorrelation&);

	// Member variables
		std::vector<unsigned char> m_absoluteWheelAxleLookUpTable;
		std::vector<unsigned char> m_virtualDPLookUpTable;
		 TA_Base_Core::AuditMessageSender* m_auditSender;
		
		//TD18093
		//TA_Base_Core::AlarmHelper& m_alarmHelper;
		//TA_Base_Core::NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;
		//TD18093

		bool m_raisedInvalidWILDConfigurationAlarm; //TD15198 azenitha

	// Member functions
		
		//
		// extractWILDData
		//
		// This method extracts the DataPoint value and puts it into a WILDDetails structure
		// and adds it to the WILDinformation queue.
		//
		// @return Returns the WILDDetails structure containing the WILD timestamp, wheel alarms 
		//         and critical bit. 
		//
		// @param The dataPoint value string.
		// 
		WILDDetails* extractWILDData(const std::string& dataPointValue);

		//
		// parseWILDData
		//
		// This method parses the WILD information, checks the ATS Queue for a matching 
		// timestamp, submits the alarms and updates the associated virtual data points
		//
		// @param The WILDDetails structure that is populated after receiving a PMS Agent datapoint
		//        state update. 
		// 
		void parseWILDData(const WILDDetails* wildDetails);

		//
		// extractTrainID
		//
		// This method extracts the train's physical ID from the PV Number. 
		//
		// @return Returns the physical train ID, which is meant to span 01-99. 
		//
		// @param The revenue train's PV Number.
		// 
		unsigned long extractTrainID(unsigned long PVNumber);

		//
		// createWheelID
		//
		// This method extracts the train's wheel ID.
		//
		// @return Returns the wheelID, which contains information about the train's
		//		   location, train number, car number, bog number, axle number and side.
		//
		// @param The train's name that is to be used to match the entity in the database
		//
		// @param The index to the internal look up table 
		// 
		unsigned long createWheelID(unsigned long trainID, unsigned int index);

		//
		// getTrainIDToken
		//
		// This method checks the SingletonTrainMap which the ATS queues against with 
		// the asscociated timestamps and returns the associated ATS train structure.
		//
		// @return Returns the Train token which denotes the physical train number 
		//         i.e 01, 02, 03 etc
		//
		// @param The train ID. 
		// 
		std::string getTrainIDToken(unsigned long trainID);


		//
		// decToBin
		//
		// This method sends the datapoint state update to all interested subscribers.
		//
		// @return the binary converted decimal.
		//
		// @pre-cond Parameter cannot be 1. 
		//
		// @param The decimal to be converted
		// 
		unsigned int decToBin(unsigned char decimal);

		//
		// decToBin
		//
		// This method sends the datapoint state update to all interested subscribers.
		//
		// @return the decimal converted binary
		//
		// @param The binary to be converted
		// 
		int binToDec(const std::string& binary);

		//
		// extractTime
		//
		// The helper method which extracts the WILD time i.e MM, DD, YY, HH etc
		// Each byte contains the time pair values as each is stored as a nibble.
		//
		// @return the WILD timestamp
		//
		// @param The byte to be extracted. 
		// 
		unsigned long extractTime(unsigned char timePair);

		//
		// isTrainGoingInDirectionA
		//
		// The helper method which determines if the train is going in direction A
		// or not, i.e. M1 car is active. 
		//
		// @return TRUE if the train is going in direction A, NA if the activeCabNumber
		//         does not correspond to a physical train ID.
		//
		// @param The activeCabNumber which is retrieved from the ATSMessageCorbaDef
		//
		// @param The physical train number
		// 
		EIsTrainGoingInDirectionA isTrainGoingInDirectionA(unsigned long activeCabNumber, unsigned int trainNumber);	
		
		//
		// submitAlarmForNonRevenueTrain
		//
		// The helper method which submits an alarm for a non-revenue/Engineering  
		// train
		//
		// @param The train's ID 
		// 
		void submitAlarmForRevenueTrain(const std::string& trainName, const std::string& wheelList, const long timeStamp ,const std::string& leftOrRight);

		void submitCriticalAlarm( const std::string& trainName );
		//
		// submitCorrelationAlarm
		//
		// The helper method which submits correlation alarms in the event where  
		// a timestamp cannot be matched between the WILD subsystem and ATS.
		//
		// @param The WILD timestamp 
		// 
		void submitCorrelationAlarm(const long timeStamp);
		void submitNonRevenueCoupledTrainAuditMessage(unsigned long physicalTrainId);
		void submitAuditMessage( const TA_Base_Core::MessageType& messageType,
			const TA_Base_Core::DescriptionParameters& descriptionParameters,
                             const std::string& sessionId = "" );

		//
		// doesTrainHaveAnyWheelAlarms
		//
		// The helper method which determines if there are any wheel alarms
		// at all. Goes through the vector and checks its contents (BOOL)
		//
		// @return Returns TRUE if there is at least one wheel alarm in the vector, 
		//         FALSE if all items in the vector is false.
		//
		// @param  A reference to the wheel alarm vector. 
		// 
		bool doesTrainHaveAnyWheelAlarms(const std::vector<bool>& wheelAlarmVector);

		//
		// returnLookUpIndex
		//
		// The helper method that returns the corrected look up index from 0-5.
		//
		// @pre cond The index is within the 0-23
		//
		// @return The index of the car/bog look up table. 
		//
		// @param The train's wheelAxle index.
		// 
		unsigned int returnLookUpIndex(unsigned int index);


		//
		// updateWheelDataPoints
		//
		// The helper method that returns the correct byte value given the modbus addressing.
		//
		// @pre cond 
		//
		// @param Pointer to the WILDDetails structure which holds the Wheel information.
		//
		// @param Pointer to the AtsWildMessageCorbaDef CORBA structure which holds the Train information.
		// 
		void updateWheelDataPoints(const WILDDetails* wildDetails, const TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* currentTrainInfo);


		//
		// returnCriticalPointID
		//
		// The helper method that returns the critical datapoint ID that is kept in the DataPoint Cache.
		//
		// @param The train ID which is in critical alarm.
		// 
		unsigned long returnCriticalPointID(unsigned long trainID);

		// format datatime to "Mon Dec 11 2006 17:29:02"
		// @param strtime the original time format returned from ctime.
		std::string formatTime(std::string& strtime);
	
	};
}

#endif // !defined(AFX_DATACORRELATION_H__6658D5CB_2313_4976_B760_1084A0E05AEC__INCLUDED_)
