/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/wild/wild_agent/src/DataCorrelation.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2010/05/12 12:20:13 $
 * Last modified by:  $Author: builder $
 * 
 * Implementation file
 */

#include "app/wild/wild_agent/src/DataCorrelation.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "core/message/types/WILDAgentAlarms_MessageTypes.h"
#include "core/message/types/WILDAgentStateUpdate_MessageTypes.h"
#include "core/message/types/WILDAgentAudit_MessageTypes.h"
#include "app/wild/wild_agent/src/SingletonTrainDataMap.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "app/wild/wild_agent/src/DataPointCache.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "app/wild/wild_agent/src/WILDConfiguration.h"
#include "core/exceptions/src/WILDAgentExceptions.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/Timer.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"

#include "ace/OS.h"

#include <time.h>
#include <map>
#include <string>
#include <sstream>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace TA_IRS_App
{
	DataCorrelation::DataCorrelation()
		: //m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
		//m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper()),
		m_raisedInvalidWILDConfigurationAlarm(false) //TD15198 azenitha
	{
		try
		{
			// Look up table which helps the creation of wheel IDs to search.
			// It has been compressed to save space and exclude redundant information.
			m_absoluteWheelAxleLookUpTable.clear();
			m_absoluteWheelAxleLookUpTable.push_back(11); //  C1B1 0
			m_absoluteWheelAxleLookUpTable.push_back(12); //  C1B2 5
			m_absoluteWheelAxleLookUpTable.push_back(21); //  C2B1 8
			m_absoluteWheelAxleLookUpTable.push_back(22); // C2B2 12
			m_absoluteWheelAxleLookUpTable.push_back(32); // C3B2 16
			m_absoluteWheelAxleLookUpTable.push_back(31); // C3B1 20

			// Fixed ISCS Template - Don't Change unless SICD has!
			m_virtualDPLookUpTable.clear();
			m_virtualDPLookUpTable.push_back(10); //1A
			m_virtualDPLookUpTable.push_back(11); //1B
			m_virtualDPLookUpTable.push_back(20); //2A
			m_virtualDPLookUpTable.push_back(21); //2B
			m_virtualDPLookUpTable.push_back(10); //3A
			m_virtualDPLookUpTable.push_back(11); //3B
			m_virtualDPLookUpTable.push_back(20); //4A
			m_virtualDPLookUpTable.push_back(21); //4B
			m_virtualDPLookUpTable.push_back(10); //1A
			m_virtualDPLookUpTable.push_back(11); //1B
			m_virtualDPLookUpTable.push_back(20); //2A
			m_virtualDPLookUpTable.push_back(21); //2B
			m_virtualDPLookUpTable.push_back(10); //3A
			m_virtualDPLookUpTable.push_back(11); //3B
			m_virtualDPLookUpTable.push_back(20); //4A
			m_virtualDPLookUpTable.push_back(21); //4B
			m_virtualDPLookUpTable.push_back(21); //4B
			m_virtualDPLookUpTable.push_back(20); //4A
			m_virtualDPLookUpTable.push_back(11); //3B
			m_virtualDPLookUpTable.push_back(10); //3A
			m_virtualDPLookUpTable.push_back(21); //2B
			m_virtualDPLookUpTable.push_back(20); //2A
			m_virtualDPLookUpTable.push_back(11); //1B
			m_virtualDPLookUpTable.push_back(10); //1A

		m_auditSender = 
             TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core:: WILDAgentAudit::Context );
		}
		catch(...)
		{
			// Should not even get to here..
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception",
				"Failed to create DataCorrelation Object!");

			m_absoluteWheelAxleLookUpTable.clear();
			m_virtualDPLookUpTable.clear();

		}
	}


	DataCorrelation::~DataCorrelation()
	{
		if (m_auditSender)
		{
			delete m_auditSender;
			m_auditSender = NULL;
		}

		m_absoluteWheelAxleLookUpTable.clear();
		m_virtualDPLookUpTable.clear();
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
	}


	unsigned int DataCorrelation::decToBin(unsigned char decimal)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "decToBin");
		
		// Special case when decimal is 1 or 0..
		if(decimal == 1 || decimal == 0)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "decToBin");
			return decimal ? 1 : 0;
		}

		char str[100]; 
		char result[100]; 
		unsigned int i = 0; 
		unsigned int k = 0; 
		do 
		{ 
			str[i] = static_cast<char>((decimal % 2) + 48);
			decimal /= 2;
			i++; 
		} 
		while (decimal != 1); 

		str[i] = '1';
		str[i+1]='\0';
		i = strlen(str); 
		
		for (k = 0; i != 0; k++, i--) 
		{ 
			result[k] = str[i - 1]; 
		} 
		
		result[k]='\0'; 
		
		k = atoi (result); 

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "decToBin");
		return k; 
	}

	
	int DataCorrelation::binToDec(const std::string& binary) 
	{ 
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "binToDec");

		int i = 0; 
		int j = 0;
		int k = 0;
		int z = 0;
		int x = strlen(binary.c_str()); j = atoi(binary.c_str()); 
		for (i = 0; i < x; i++) 
		{ 
			k = j % 10; z += static_cast<int>(k * (pow ((float)2, i))); j = j/10; 
		} 

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "binToDec");
		return z; 
	} 


	DataCorrelation::EIsTrainGoingInDirectionA DataCorrelation::isTrainGoingInDirectionA(unsigned long activeCabNumber, unsigned int trainNumber)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "isTrainGoingInDirectionA");
		unsigned int direction = 0;
	    if (activeCabNumber >= 256)
	    {
			direction = activeCabNumber - (4*trainNumber) - 252;
	    }
		else if(activeCabNumber <=252 && activeCabNumber >=64)
		{
			direction = activeCabNumber -  (4*trainNumber) - 60;
		}
		//TA_ASSERT((direction == 1 || direction == 0), "Result can only be 1 or 0");
		
		if(direction == 1)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "isTrainGoingInDirectionA");
			return isTRUE;
		}
		else if(direction == 0)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "isTrainGoingInDirectionA");
			return isFALSE;
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "isTrainGoingInDirectionA");
			return NA;
		}
	}

	//void DataCorrelation::submitCriticalAlarm( const std::string& trainName )

    //  [7/13/2009 HuangQi++]:CL-17026 Add Param long timeStamp
    void DataCorrelation::submitCriticalAlarm( const std::string& trainName, long timeStamp )
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugDebug,
			"LICHAO:submitCriticalAlarm Entry.");

		TA_Base_Core::DescriptionParameters descriptionParameters;
		TA_Base_Core::NameValuePair pair1 ( "PhysicalTrainId", trainName);
		descriptionParameters.push_back ( &pair1 );
		//std::string wildTimeStamp = DataPointCache::getInstance()->getwildTimeStamp(timeStamp);

		//  [7/13/2009 HuangQi++]:CL-17026
        time_t timep = timeStamp;
        time(&timep);
        std::string wildTimeStamp;
		wildTimeStamp =ctime(&timep);
        TA_Base_Core::NameValuePair pair2 ( "TimeStamp", wildTimeStamp);
		descriptionParameters.push_back ( &pair2 );
		//  [7/13/2009 HuangQi--]:CL-17026

		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(TA_Base_Core::WILDAgentAlarms::CriticalAlarm, 
			WILDConfiguration::getInstance()->getAgentKey(),
			WILDConfiguration::getInstance()->getAgentTypeKey(),
			descriptionParameters, 
			WILDConfiguration::getInstance()->getAgentName(),
			WILDConfiguration::getInstance()->getAgentLocationKey(),
			WILDConfiguration::getInstance()->getAgentSubsystemKey(),
			WILDConfiguration::getInstance()->getAgentAssetName(),

			TA_Base_Core::AlarmConstants::defaultTime,
			TA_Base_Core::MMS_SEMI);

		descriptionParameters.clear();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugDebug,
			"submitCriticalAlarm Exit.");
	}
	void DataCorrelation::submitAlarmForRevenueTrain(const std::string& trainName, const std::string& wheelList, const long timeStamp,const std::string& leftOrRight)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "submitAlarmForRevenueTrain");

		TA_Base_Core::DescriptionParameters descriptionParameters;
		TA_Base_Core::NameValuePair pair1 ( "PhysicalTrainId", trainName);
		descriptionParameters.push_back ( &pair1 );

		TA_Base_Core::NameValuePair pair2 ( "PhysicalAxleNumber", wheelList);
		descriptionParameters.push_back ( &pair2 );

		std::string wildTimeStamp ;
		time_t timep = timeStamp;
		time(&timep);
		wildTimeStamp =ctime(&timep);
		TA_Base_Core::NameValuePair pair3 ( "TimeStamp", wildTimeStamp );
		descriptionParameters.push_back ( &pair3 );


		TA_Base_Core::NameValuePair pair4 ( "LeftOrRight", leftOrRight );
		descriptionParameters.push_back ( &pair4 );
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(TA_Base_Core::WILDAgentAlarms::WheelAlarm, 
			WILDConfiguration::getInstance()->getAgentKey(),
			WILDConfiguration::getInstance()->getAgentTypeKey(),
			descriptionParameters, 
			WILDConfiguration::getInstance()->getAgentName(),
			WILDConfiguration::getInstance()->getAgentLocationKey(),
			WILDConfiguration::getInstance()->getAgentSubsystemKey(),
			WILDConfiguration::getInstance()->getAgentAssetName(),

			TA_Base_Core::AlarmConstants::defaultTime,
			TA_Base_Core::MMS_SEMI);
								
		descriptionParameters.clear();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "submitAlarmForRevenueTrain");
	}

	void DataCorrelation::correlateData(const std::string& dataPointValue)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "correlateData");

        //WILDDetails* wildDetails = extractWILDData(dataPointValue); //This returns NULL if fails //limin--, CL-17761
        WILDDetails* wildDetails = extractWILDDataDecimalEncoding( dataPointValue ); //This returns NULL if fails //limin++, CL-21009
		if(wildDetails != NULL && wildDetails->DateTime>0)
		{
			parseWILDData(wildDetails);
		}
		else if(wildDetails != NULL && wildDetails->DateTime<=0)
		{
			DataPointCache::getInstance()->submitWILDErrorAlarm("Invalid Timestamp received from WILD eqipment from Text Datapoint. Correlation will not be performed.");
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
				"Failed to perform correlation: Failed to extract WILD data from Text/Bool Data Points");

			DataPointCache::getInstance()->submitWILDErrorAlarm("Unable to extract WILD information from Text Datapoint. Correlation will not be performed.");
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "correlateData");
	}

	void DataCorrelation::parseWILDData(const WILDDetails* wildDetails)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "parseWILDData");
		TA_ASSERT(wildDetails != NULL, "NULL pointer observed whilst correlating data");
	
		TA_Base_Core::Timer timer;
		long WILDTimeStamp = wildDetails->DateTime;
		std::vector<bool> wheelAxleAlarms = wildDetails->wheelAxleAlarms;
		bool isCriticalAlarm = wildDetails->isCriticalAlarm;

        // ss++ TD16766
		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* currentTrainInfo = 0;

        while(currentTrainInfo == NULL && static_cast<unsigned long>(timer.elapsedTime()) < 70 /* WILDConfiguration::getInstance()->getCorrelationThresholdTimeoutSecs() */ )//limin++, CL-21009
		{
			currentTrainInfo = SingletonTrainDataMap::getInstance()->retrieveTrainInformation(WILDTimeStamp);	
			if(currentTrainInfo)
			{
				break; // circumvents the small sleep period, so processing can get underway.
			}
			// Check the queue again. We don't implement a semaphore- its not quite what we want..
			TA_Base_Core::Thread::sleep(2000);  
		}
	

		if(currentTrainInfo != NULL) // Matching ATS record
		{
			//  Train ID/Direction/ Physical Wheel Axle Number / Wheel side A or B + Critical Alarm
			
			// From ATS
			unsigned long trainID = currentTrainInfo->physicalTrainNumber;
			std::string TrainName(getTrainIDToken(trainID));
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Correlation exists for train %d", trainID);

			// Now determine which type of train, alarms required to submit, there 
            // are 4 possible scenarios.
            if ( currentTrainInfo->isRevenue )
            {
                if ( doesTrainHaveAnyWheelAlarms(wheelAxleAlarms) )
                {
                    // No need to update wheel alarms as this is a train that we should
                    // not know about.
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugDebug,
                        "LICHAO:Non Revenue train detected, submitting Alarm ,alarm num = %d..", wheelAxleAlarms.size());
                    int wheelNum = 0; 

                    for ( std::vector<bool>::iterator  iter = wheelAxleAlarms.begin();
				          iter != wheelAxleAlarms.end(); ++iter )
                    { 
                        if ( *iter )
                        {
                            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugDebug,
                                "LICHAO:wheel num = %d..", wheelNum );
                            std::ostringstream axle;
                            int alexNumIndex = wheelNum / 2 + 1;
                            axle << alexNumIndex;
                            std::string  rightOrlift = "" ;
						    
                            if ( wheelNum % 2 == 1 )
                            {
                                rightOrlift = "Left";
                            }
                            else
                            {
                                rightOrlift = "Right";
                            }
                        
                            submitAlarmForRevenueTrain( TrainName, axle.str() ,WILDTimeStamp , rightOrlift );
                        }
					    
                        wheelNum ++;
                    }
				    
                    // updateWheelDataPoints(wildDetails, currentTrainInfo); //limin-- CL-21133
				    
                    if ( isCriticalAlarm )
                    {
                        //  [7/13/2009 HuangQi++]:CL-17026
                        submitCriticalAlarm(TrainName, WILDTimeStamp);
                        //  [7/13/2009 HuangQi--]:CL-17026
                    }
                }

                updateWheelDataPoints(wildDetails, currentTrainInfo);//limin++ CL-21133, CL-21352
            }
            else if( !currentTrainInfo->isRevenue )
            {
                submitNonRevenueCoupledTrainAuditMessage(trainID);
            }

			wheelAxleAlarms.clear();

            delete currentTrainInfo;
			currentTrainInfo = 0;
		}
		else // structure returned was NULL, indicating no physical train was identified.
		{		
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
					"Unable to correlate data, No matching ATS Train information");

			if( doesTrainHaveAnyWheelAlarms(wheelAxleAlarms))
			{
				// There is an alarm here and we can't match the train
				submitCorrelationAlarm(WILDTimeStamp);
			}
		}
        // ++ss

		if(wildDetails != NULL)
		{
			delete wildDetails;
			wildDetails = 0;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "parseWILDData");
	}


	unsigned long DataCorrelation::returnCriticalPointID(unsigned long trainID)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "returnCriticalPointID");
		// The following creates unique search "keys" for the DataPoint map.
		// As long as these created keys don't clash with proper Data Point pkeys
		// we're safe.
		std::ostringstream ostr;
		ostr << WILDConfiguration::getInstance()->getAgentLocationKey();
		
		if(trainID < 10)
		{
			ostr << 0;
		}
		ostr << trainID;
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnCriticalPointID");
		return atol( (ostr.str()).c_str() );
	}


	void DataCorrelation::updateWheelDataPoints(const WILDDetails* wildDetails, const TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* currentTrainInfo)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "updateWheelDataPoints");

		TA_Base_Bus::DataPoint* theDataPoint = 0;
		TA_Base_Bus::DataPoint* theCriticalDataPoint = 0;

		// From WILD 
		long WILDTimeStamp = wildDetails->DateTime;
		bool isCriticalAlarm = wildDetails->isCriticalAlarm;
		std::vector<bool> wheelAxleAlarms = wildDetails->wheelAxleAlarms;

		// From ATS
		unsigned long trainID = currentTrainInfo->physicalTrainNumber;
		std::string TrainName(getTrainIDToken(trainID));
		EIsTrainGoingInDirectionA isInDirectionA = isTrainGoingInDirectionA(currentTrainInfo->activeCabNumber, trainID);

        // ss++ TD16550
        unsigned int wildDpKey = 0;
		//if(isCriticalAlarm)
		{
            wildDpKey = DataPointCache::getInstance()->getCAEntityKeyFromInhibitMonitorList();
			theCriticalDataPoint = DataPointCache::getInstance()->updateAndRetrieveDataPoint(wildDpKey, returnCriticalPointID(trainID), isCriticalAlarm, WILDTimeStamp);
		}
        // ++ss

		theCriticalDataPoint = 0;
		
		for(unsigned int i=0; i<wheelAxleAlarms.size(); i++)
		{
            // ss++ TD16765 - same train pass through the track with no fault, wild automatically normalize this fault
			try
			{
				// Have to reverse the order of m_virtualDPLookUpTable if the direction is different. i.e B
				unsigned int index = i;
				if(isInDirectionA == isFALSE)
				{
					index = (m_virtualDPLookUpTable.size() - i) - 1;
				}
				else if(isInDirectionA == NA)
				{
					std::ostringstream alarmDesc;
					alarmDesc << "Failed to correlate wheel point. ATC Car Number: ";
					alarmDesc << currentTrainInfo->activeCabNumber;
					alarmDesc << " does not map to Train ";
					alarmDesc << trainID;

					//TD15198 azenitha++
					//Changed InvalidWILDConfiguration to CorrelationAlarm (accdg to adam)
					//DataPointCache::getInstance()->submitInvalidConfigAlarm(alarmDesc.str());
					submitCorrelationAlarm(WILDTimeStamp);
					//TD15198 ++azenitha

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", alarmDesc.str().c_str());
					LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateWheelDataPoints");
					return;
				}

				// TO DO: The data point details can get of synch here... consider returning the
				// whole datapoint object instead of a pointer. It can get out of synch if 
				// its configuration information changes, though unlikely. Drawback of returning
				// actual object of course is its memory but its only one data point. Should
				// bring this up during code review.
				unsigned long searchID = createWheelID(trainID, index);

                // ss++
                wildDpKey = DataPointCache::getInstance()->getProxyEntityKeyFromInhibitMonitorList(i);
                // ++ss

				theDataPoint = DataPointCache::getInstance()->updateAndRetrieveDataPoint(wildDpKey, searchID, wheelAxleAlarms[i], WILDTimeStamp);
				theDataPoint = 0; // Safeguard against dangling pointers.
				//TD15198 azenitha++
				if (m_raisedInvalidWILDConfigurationAlarm == true)
				{
					DataPointCache::getInstance()->closeAlarm(TA_Base_Core::WILDAgentAlarms::InvalidWILDConfiguration);
					m_raisedInvalidWILDConfigurationAlarm = false;
				}
				//TD15198 ++azenitha
			}
			catch(TA_Base_Core::InvalidWILDConfigurationException&)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "InvalidWILDConfigurationException",
					"Unable to retrieve entity to update");

				std::string alarmDesc("Invalid configuration was received for Train No. ");
				alarmDesc += TrainName;
				DataPointCache::getInstance()->submitInvalidConfigAlarm(alarmDesc);	
				m_raisedInvalidWILDConfigurationAlarm = true; //TD15198 azenitha
				
			}
			catch(...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
					"Unknown exception caught");
			}
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateWheelDataPoints");
	}
	void DataCorrelation::submitNonRevenueCoupledTrainAuditMessage(unsigned long physicalTrainId)
	{
		FUNCTION_ENTRY("submitNonRevenueCoupledTrainAuditMessage");
		
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Submit audit message WILDAgentAudit::NonRevenueCoupledTrain [train id: %d]", physicalTrainId );

		// build the parameters
		TA_Base_Core::DescriptionParameters parameters;
		std::stringstream trainId;
		trainId << static_cast<unsigned long>(physicalTrainId);
		TA_Base_Core::NameValuePair parameterParam ( "PhysicalTrainId", trainId.str().c_str());
		parameters.push_back ( &parameterParam );
		
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		// send the message
		submitAuditMessage( TA_Base_Core::WILDAgentAudit::NonRevenueCoupledTrain,
			parameters, sessionId );
		
		FUNCTION_EXIT;
	}

	void DataCorrelation::submitAuditMessage( const TA_Base_Core::MessageType& messageType,
		const TA_Base_Core::DescriptionParameters& parameters,
		const std::string& sessionId /* = "" */ )
	{
		FUNCTION_ENTRY("submitAuditMessage");
		TA_ASSERT(m_auditSender, "m_auditSender is NULL");
		
		bool submitted = false;
		std::string reason = "";
		
		// If the agent has not been initialised, we can only log this message
		if (WILDConfiguration::getInstance()->getAgentKey() != 0)
		{
			try
			{            
				m_auditSender->sendAuditMessage(
					messageType,                                 // Message Type
					WILDConfiguration::getInstance()->getAgentKey(),  // Entity key

					parameters,                       // Description
					"",                                          // Additional details
					sessionId,                                   // SessionID if applicable
					"",                                          // AlarmID if an alarm associated with event
					"",                                          // IncidentKey if incident associated with event
					"");                                         // EventID of a parent event, used to link events
				
				submitted = true;
				
			} // try block
			catch( TA_Base_Core::TransactiveException& ex )
			{
				reason = ex.what();
			}
			catch( ... )
			{
				reason = "Unknown reason.";
			}
		}
		else
		{
			// log the fact that the message cant be sent
			// also log the details of the message
			reason = "Agent not initialised.";
		}
		
		// failed to submit - make sure all the details are logged
		if (!submitted)
		{
			std::ostringstream msg;
			msg << "Failed to submit audit message " << messageType.getTypeKey() << ". " << reason << std::endl
				<< "Parameters are: ";
			
			for (TA_Base_Core::DescriptionParameters::const_iterator iter = parameters.begin();
			iter != parameters.end(); iter++ )
			{
				msg << (*iter)->name << "=" << (*iter)->value << ", ";
			}
			msg << "SessionId=" << sessionId;
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
		}
		
		 FUNCTION_EXIT;
	}

	void DataCorrelation::submitCorrelationAlarm(const long timeStamp)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "submitCorrelationAlarm");

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
			"No train was retrieved from the ATS table for the specified time or a problem occured whilst retrieving data point status/value");
				
		TA_Base_Core::DescriptionParameters descriptionParameters;
		
        const unsigned int bufLen = 27;
        char buf[bufLen];

		time_t timeTmp = timeStamp;
        std::string wildTimeStamp = ACE_OS::ctime_r(&timeTmp, buf, bufLen);
		// Get rid of the return characters so that it will be displayed correctly in the alarm viewer.
		wildTimeStamp = wildTimeStamp.erase(wildTimeStamp.length() -1, wildTimeStamp.length());
		wildTimeStamp = formatTime(wildTimeStamp);

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"formated datatime is %s", wildTimeStamp.c_str());

		TA_Base_Core::NameValuePair pair1 ( "TimeStamp", wildTimeStamp);
		descriptionParameters.push_back ( &pair1 );

		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(TA_Base_Core::WILDAgentAlarms::CorrelationAlarm, 
			WILDConfiguration::getInstance()->getAgentKey(),
			WILDConfiguration::getInstance()->getAgentTypeKey(),
			descriptionParameters, 
			WILDConfiguration::getInstance()->getAgentName(),
			WILDConfiguration::getInstance()->getAgentLocationKey(),
			WILDConfiguration::getInstance()->getAgentSubsystemKey(),
			
			WILDConfiguration::getInstance()->getAgentAssetName(),
			
			TA_Base_Core::AlarmConstants::defaultTime,
            TA_Base_Core::MMS_SEMI);

		descriptionParameters.clear();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "submitCorrelationAlarm");
	}


	bool DataCorrelation::doesTrainHaveAnyWheelAlarms(const std::vector<bool>& wheelAlarmVector)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "doesTrainHaveAnyWheelAlarms");

		for(unsigned int i=0; i<wheelAlarmVector.size(); i++)
		{
			if(wheelAlarmVector[i] == true)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "doesTrainHaveAnyWheelAlarms");
				return true;
			}
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "doesTrainHaveAnyWheelAlarms");
		return false;
	}


	unsigned long DataCorrelation::extractTime(unsigned char timePair)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "extractTime");
		// The following extracts the BCD time formatted byte and converts
		// it into a numerical time value.
		unsigned int timeBin = decToBin(timePair);
	
		std::stringstream ss;
		ss << timeBin;
		std::string binaryTime(ss.str());
		unsigned int numberOfBitsToAdd = 8 - binaryTime.length();

		std::string prefixBits("");
		if (numberOfBitsToAdd > 0)
		{
			for(unsigned int i=0; i<numberOfBitsToAdd; i++)
			{
				prefixBits += "0";
			}
		}

		prefixBits.append(binaryTime);
		// Extract the bytes into LSB, MSB nibbles
		std::string LSB(prefixBits.substr(4)); 
		std::string MSB(prefixBits.erase(4));

		std::stringstream tmp;
		tmp << binToDec(MSB.c_str());
		tmp << binToDec(LSB.c_str());

		std::string stringResult(tmp.str());

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractTime");
		return atol(stringResult.c_str());
	}


	DataCorrelation::WILDDetails* DataCorrelation::extractWILDData(const std::string& dataPointValue)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "extractWILDData");
		WILDDetails* tmpWILDDetails = 0;

		for(unsigned int i=0; i<dataPointValue.size(); i++)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"value of TEXT datapoint[%d] is %d", i, static_cast<unsigned char>(dataPointValue[i]));
		}

		try
		{
			//Extract the BCD time stamp from the text datapoint value, format: YYYYMMDDHHMMSS
			TA_ASSERT(dataPointValue.size() > 6, "There should be at least 12 bytes in this text data point value string");
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"Size of dataPointText is %d", dataPointValue.size());

			tmpWILDDetails = new WILDDetails();
			//TD15784 the code here is not so clear. but it works. we need a doc to know how the datapointvalue is filled.
			// First check if the critical bit is set, which is the first bit of the Text Dp.
			bool isCriticalAlarm = false;
			if((dataPointValue[0] & 2) == 2)
			{
				isCriticalAlarm = true;
			}

			tmpWILDDetails->isCriticalAlarm = isCriticalAlarm;

			std::vector<bool> wheelsInAlarm;
			wheelsInAlarm.clear();

			// Process the remaning 6 bits of the 2nd byte which are wheel alarms..
			for(unsigned int i=3; i<=8; i++)
			{ 
				unsigned int bit = static_cast<unsigned char>(pow((float)2, (int)(i-1)));	
				bool isInAlarm = false;
				if((dataPointValue[0] & bit) == bit)
				{
					isInAlarm = true;
				}
				wheelsInAlarm.push_back(isInAlarm);
			}

			// Now process the next two bytes
			for(unsigned int j=3; j>=2; j--)
			{
				for(unsigned int k=1; k<=8; k++)
				{	
					bool isInAlarm = false;
					unsigned int bit = static_cast<unsigned char>(pow((float)2, (int)(k-1)));
					if((dataPointValue[j] & bit) == bit)
					{
						isInAlarm = true;
					}
					wheelsInAlarm.push_back(isInAlarm);
				}
			}

			//Finally process the first 2 bits of a new byte and this rounds up our wheel alarms;
			for(unsigned int l=1; l<3; l++)
			{
				bool wheelAlarm = false;
				if((dataPointValue[5] & l) == l)
				{
					wheelAlarm = true;
				}
				wheelsInAlarm.push_back(wheelAlarm);
			}

			TA_ASSERT(wheelsInAlarm.size() == 24, "Not all wheel alarm bits were set!");
			tmpWILDDetails->wheelAxleAlarms = wheelsInAlarm;

			for(unsigned int m=0; m<wheelsInAlarm.size(); m++)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "wheel[%d] is %d", m, static_cast<unsigned int>(wheelsInAlarm[m]));
			}

			// Now we're retrieving the timestamp information... YYYYMMDDHHMMSS

			unsigned long YEAR_MSB, YEAR_LSB, MONTH, DAY, HOUR, MINUTE, SECONDS = 0;
			
			YEAR_MSB = extractTime(dataPointValue[6]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted Year MSB is %lu", YEAR_MSB);

			YEAR_LSB = extractTime(dataPointValue[7]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted Year LSB is %lu", YEAR_LSB);
						
			std::stringstream combinedYears;			
			combinedYears << YEAR_MSB;

			if(YEAR_LSB < 10)
			{
				combinedYears << 0;
			}
			
			combinedYears << YEAR_LSB;

			std::string combinedYearsStr(combinedYears.str());

            // ss++
            // TD4666
			//TD15784  change the order
			MONTH = extractTime(dataPointValue[9]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted MONTH is %lu", MONTH);

			DAY = extractTime(dataPointValue[11]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted DAY is %lu", DAY);

			HOUR = extractTime(dataPointValue[13]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted HOUR is %lu", HOUR);

			MINUTE = extractTime(dataPointValue[15]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted MINUTE is %lu", MINUTE);

			SECONDS = extractTime(dataPointValue[17]);
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted SECONDS is %lu", SECONDS);
            // ++ss

			// IMPORTANT. The month is zero indexed. 
			struct tm WILDStamp = {SECONDS, MINUTE, HOUR, DAY, MONTH-1, (atol(combinedYearsStr.c_str()) - 1900)};
			time_t timeStamp = mktime(&WILDStamp);

			tmpWILDDetails->DateTime = timeStamp;
	
			// TO DO: We need to verify that the following addresses are correct. Otherwise, we
			// can just get the correct pkeys of the data point and call getBooleanDataPointProxyValue. 
			// However, this is worse off as we're depending on fixed configuration information.

		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception",
			"Caught uknown exception whilst extracting Text datapoint");

			if(tmpWILDDetails != NULL)
			{
				delete tmpWILDDetails;
				tmpWILDDetails = 0;
			}

			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractWILDData");
			return tmpWILDDetails;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractWILDData");
		return tmpWILDDetails;
	}


    //limin++, CL-21009
    DataCorrelation::WILDDetails* DataCorrelation::extractWILDDataDecimalEncoding(const std::string& dataPointValue)
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "extractWILDDataDecimalEncoding");
        WILDDetails* tmpWILDDetails = 0;

        for(unsigned int i=0; i<dataPointValue.size(); i++)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
            "value of TEXT datapoint[%d] is %d", i, static_cast<unsigned char>(dataPointValue[i]));
        }

        try
        {
            //Extract the BCD time stamp from the text datapoint value, format: YYYYMMDDHHMMSS
            TA_ASSERT(dataPointValue.size() > 6, "There should be at least 12 bytes in this text data point value string");
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                    "Size of dataPointText is %d", dataPointValue.size());

            tmpWILDDetails = new WILDDetails();
            //TD15784 the code here is not so clear. but it works. we need a doc to know how the datapointvalue is filled.
            // First check if the critical bit is set, which is the first bit of the Text Dp.
            bool isCriticalAlarm = false;
            if((dataPointValue[0] & 2) == 2)
            {
                isCriticalAlarm = true;
            }

            tmpWILDDetails->isCriticalAlarm = isCriticalAlarm;

            std::vector<bool> wheelsInAlarm;
            wheelsInAlarm.clear();

            // Process the remaning 6 bits of the 2nd byte which are wheel alarms..
            for(unsigned int i=3; i<=8; i++)
            { 
                unsigned int bit = static_cast<unsigned char>(pow((float)2, (int)(i-1)));
                bool isInAlarm = false;
                if((dataPointValue[0] & bit) == bit)
                {
                    isInAlarm = true;
                }
                wheelsInAlarm.push_back(isInAlarm);
            }

            // Now process the next two bytes
            for(unsigned int j=3; j>=2; j--)
            {
                for(unsigned int k=1; k<=8; k++)
                {    
                    bool isInAlarm = false;
                    unsigned int bit = static_cast<unsigned char>(pow((float)2, (int)(k-1)));
                    if((dataPointValue[j] & bit) == bit)
                    {
                        isInAlarm = true;
                    }
                    wheelsInAlarm.push_back(isInAlarm);
                }
            }

            //Finally process the first 2 bits of a new byte and this rounds up our wheel alarms;
            for(unsigned int l=1; l<3; l++)
            {
                bool wheelAlarm = false;
                if((dataPointValue[5] & l) == l)
                {
                    wheelAlarm = true;
                }
                wheelsInAlarm.push_back(wheelAlarm);
            }

            TA_ASSERT(wheelsInAlarm.size() == 24, "Not all wheel alarm bits were set!");
            tmpWILDDetails->wheelAxleAlarms = wheelsInAlarm;

            for(unsigned int m=0; m<wheelsInAlarm.size(); m++)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "wheel[%d] is %d", m, static_cast<unsigned int>(wheelsInAlarm[m]));
            }

            // Now we're retrieving the timestamp information... YYYYMMDDHHMMSS

            unsigned long YEAR = 0;
            unsigned long MONTH = 0;
            unsigned long DAY = 0;
            unsigned long HOUR = 0;
            unsigned long MINUTE = 0;
            unsigned long SECONDS = 0;

            YEAR |= static_cast< unsigned char >( dataPointValue[ 6 ] );
            YEAR <<= 8;
            YEAR |= static_cast< unsigned char >( dataPointValue[ 7 ] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted YEAR is %lu", YEAR);

            // ss++
            // TD4666
            //TD15784  change the order
            MONTH |= static_cast< unsigned char >( dataPointValue[9] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted MONTH is %lu", MONTH);

            DAY |= static_cast< unsigned char >( dataPointValue[11] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted DAY is %lu", DAY);

            HOUR |= static_cast< unsigned char >( dataPointValue[13] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted HOUR is %lu", HOUR);

            MINUTE |= static_cast< unsigned char >( dataPointValue[15] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted MINUTE is %lu", MINUTE);

            SECONDS |= static_cast< unsigned char >( dataPointValue[17] );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Extracted SECONDS is %lu", SECONDS);
            // ++ss

            // IMPORTANT. The month is zero indexed. 
            struct tm WILDStamp = { SECONDS, MINUTE, HOUR, DAY, ( MONTH - 1 ), ( YEAR - 1900 ) };
            time_t timeStamp = mktime(&WILDStamp);

            tmpWILDDetails->DateTime = timeStamp;
    
            // TO DO: We need to verify that the following addresses are correct. Otherwise, we
            // can just get the correct pkeys of the data point and call getBooleanDataPointProxyValue. 
            // However, this is worse off as we're depending on fixed configuration information.
        }
        catch(...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception",
            "Caught uknown exception whilst extracting Text datapoint");

            if(tmpWILDDetails != NULL)
            {
                delete tmpWILDDetails;
                tmpWILDDetails = 0;
            }

            LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractWILDDataDecimalEncoding");
            return tmpWILDDetails;
        }

        LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractWILDDataDecimalEncoding");
        return tmpWILDDetails;
    }
    //++limin, CL-21009


	unsigned long DataCorrelation::extractTrainID(unsigned long PVNumber)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "extractTrainID");

		std::stringstream PVNumberStr;
		PVNumberStr << PVNumber;

		std::string result(PVNumberStr.str());
		result.resize(3);

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractTrainID");
		return (atol(result.c_str()) - 800);
	}


	std::string DataCorrelation::getTrainIDToken(unsigned long trainID)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getTrainIDToken");

		std::string tmp("");
		std::stringstream ss;
		ss << trainID;

		if(trainID < 10) // if the train ID is from 1 - 9
		{
			tmp += "0";
			tmp  += ss.str();
			
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getTrainIDToken");
			return tmp;
		}
		else // trainID is more than 9
		{
			tmp += ss.str();

			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getTrainIDToken");
			return tmp;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getTrainIDToken");
	}


	unsigned int DataCorrelation::returnLookUpIndex(unsigned int index)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "returnLookUpIndex");

		TA_ASSERT(0 <= index && index <=23, "Index out of bounds!");
		
		if(0 <= index && index <= 3)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			return 0;
		}
		else if(4 <= index && index <= 7)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			return 1;
		}
		else if(8 <= index && index <= 11)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			return 2;
		}
		else if(12 <= index && index <= 15)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			 return 3;
		}
		else if(16 <= index && index <= 19)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			 return 4;
		}
		else if(20 <= index && index <= 23)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
			 return 5;
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "returnLookUpIndex");
		return 0; // Should not get to here.
	}


	unsigned long DataCorrelation::createWheelID(unsigned long trainID, unsigned int index)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "createWheelID");

		unsigned int carBogLookUpIndex = returnLookUpIndex(index);

		std::stringstream searchKey;
		searchKey << WILDConfiguration::getInstance()->getAgentLocationKey();
		
		if(trainID < 10)
		{
			searchKey << 0;
		}

		searchKey << trainID;
		// The cast is required, otherwise atol won't return a valid number.
		searchKey << static_cast<unsigned int>(m_absoluteWheelAxleLookUpTable[carBogLookUpIndex]);
		searchKey << static_cast<unsigned int>(m_virtualDPLookUpTable[index]);
			
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "createWheelID");
		return atol(searchKey.str().c_str());
	}

	// format datatime to "Mon Dec 11 2006 17:29:02"
	// @param strtime the original time format returned from ctime.
	std::string DataCorrelation::formatTime(std::string& strtime)
	{
		int lastSpace =  strtime.rfind(" ");
		
		std::string year = strtime.substr(lastSpace+1,4);
		std::string beforeyear = strtime.substr(0,lastSpace);
		
		int secondLastSpace = beforeyear.rfind(" ");
		std::string dayandmonth = beforeyear.substr(0,secondLastSpace) ;
		
		std::string hhmmss = beforeyear.substr(secondLastSpace+1,8);
		std::string result = dayandmonth + " " +  year + " " + hhmmss;
		
		return result;
	}
}
