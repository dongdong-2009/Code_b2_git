/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsTwpData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS TWP Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/ats/src/AtsTWPDataFactory.h"
#include "core/data_access_interface/ats/src/AtsTWPData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/message/types/AtsAgentAlarms_MessageTypes.h"
#include "core/corba/src/CorbaUtil.h"

#include "bus/ats/ats_agent/CommonATS/src/AddressUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/DataPointHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsTwpData.h"

using TA_Base_Core::AtsAgentAlarms::AtsTwpDataSend;

namespace TA_IRS_Bus
{
	AtsTwpData::AtsTwpData(	unsigned long entityKey, unsigned long locationKey, DataPointHelper& dpHelper)
	:
	m_valid (false),
	m_noWash (false),
	m_doNoWashControl (false),
	m_dailyRinseWash (false),
	m_doDailyRinseWashControl (false),
	m_dailyDetergentWash (false),
	m_doDailyDetergentWashControl (false),
	m_weeklyWash (false),
	m_doWeeklyWashControl (false),
	m_trainStoppedForCabEndWash (false),
	m_doTrainStoppedForCabEndWashControl (false),
	m_stopWp (false),
	m_doStopWpControl (false),
	m_restartWp (false),
	m_doRestartWpControl (false),
	m_entityKey (entityKey), 
	m_locationKey (locationKey),
	m_dpHelper(dpHelper),
	//TD19853 
	m_bTwpNormalChange(false),
	m_bPreTwpLink(false)
	{
		std::vector<TA_IRS_Core::IAtsTWPData*>::iterator twpIt;
		std::vector<TA_IRS_Core::IAtsTWPData*> atsOutputTwpData;

		try
		{
			atsOutputTwpData = TA_IRS_Core::AtsTWPDataFactory::getInstance().getAllTWPDataOfDataPointType(
							TA_IRS_Core::AtsTWPData::OUTPUT_TYPE);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "AtsTwpData() - TWP configuration error - " + std::string(e.what());
  			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "AtsTwpData() - TWP configuration error - " + std::string(e.what());
  			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
		}

		for (twpIt = atsOutputTwpData.begin(); twpIt != atsOutputTwpData.end(); twpIt++)
		{
			try
			{
				m_dpHelper.addOutputBooleanDataPoint((*twpIt)->getTWPDataPointEntityKey(), TA_Base_Core::AtsAgentAlarms::AtsTwpDataSend);
                	
				m_twpOutputDataPoints[(*twpIt)->getTWPCommand()] = (*twpIt)->getTWPDataPointEntityKey();
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "AtsTwpData() - Unknown Exception adding DataPoint");

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"AtsTwpData() - Error: did not create proxy for entity with key %d", (*twpIt)->getTWPDataPointEntityKey());
			}

			delete (*twpIt);
			(*twpIt) = NULL;
		}

		std::vector<TA_IRS_Core::IAtsTWPData*> atsInputTwpData;

		try
		{
			atsInputTwpData = TA_IRS_Core::AtsTWPDataFactory::getInstance().getAllTWPDataOfDataPointType(
							TA_IRS_Core::AtsTWPData::INPUT_TYPE);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "AtsTwpData() - TWP configuration error - " + std::string(e.what());
  			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "AtsTwpData() - TWP configuration error - " + std::string(e.what());
  			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
		}

		for (twpIt = atsInputTwpData.begin(); twpIt != atsInputTwpData.end(); twpIt++)
		{
			try
			{
				unsigned long entityKey = (*twpIt)->getTWPDataPointEntityKey();

				if (!m_dpHelper.isDataPointAdded(entityKey))
				{
					if ((*twpIt)->getTWPCommand() == TA_IRS_Core::AtsTWPData::WP_STATUS)
					{
						m_dpHelper.addInputTextDataPoint(entityKey);
					}
					else
					{
						m_dpHelper.addInputBooleanDataPoint(entityKey);
					}
                }	
				m_twpInputDataPoints[(*twpIt)->getTWPCommand()] = entityKey;
				m_dpnametovalue[(*twpIt)->getTWPCommand()] = 0;  //initial value TD15264
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "AtsTwpData() - Unknown Exception adding DataPoint");

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"AtsTwpData() - Error: did not create proxy for entity with key %d", (*twpIt)->getTWPDataPointEntityKey());
			}

			delete (*twpIt);
			(*twpIt) = NULL;
		}
	};

	AtsTwpData::~AtsTwpData()
	{
		m_twpOutputDataPoints.clear();
		m_twpInputDataPoints.clear();
	}

	void AtsTwpData::updateValidity(bool valid)
	{
		if (m_valid != valid)
		{
			//if(!m_valid)
			//	m_bTwpRecoverLink = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateValidity() - validity, was %s now %s", 
				(m_valid?"valid":"invalid"), 
				(valid?"valid":"invalid"));
			m_valid = valid;
			if (!valid)
			{
				m_noWash = false;
				m_dailyRinseWash = false;
				m_dailyDetergentWash = false;
				m_weeklyWash = false;
				m_trainStoppedForCabEndWash = false;
				m_stopWp = false;
				m_restartWp = false;
			}
			else
			{
				m_bTwpNormalChange = true;
			}
		}
	}

	void AtsTwpData::updateNoWashControl(bool doControl)
	{
		if(m_noWash !=doControl)
		   m_bTwpNormalChange = true;

        m_noWash = doControl;
		m_doNoWashControl = true;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateNoWashControl() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateDailyRinseWashControl(bool doControl)
	{
 		if(m_dailyRinseWash != doControl)
		   m_bTwpNormalChange = true;   
		  
		  m_dailyRinseWash = doControl;
		m_doDailyRinseWashControl = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateDailyRinseWashControl() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateDailyDetergentWashControl(bool doControl)
	{
		 if(m_dailyDetergentWash != doControl)
		   m_bTwpNormalChange = true;
        m_dailyDetergentWash = doControl;
        m_doDailyDetergentWashControl = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateDailyDetergentWashControl() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateWeeklyWashControl(bool doControl)
	{
		 if(m_weeklyWash != doControl)
		   m_bTwpNormalChange = true;
        m_weeklyWash = doControl;
        m_doWeeklyWashControl = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateDailyDetergentWashControl() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateTrainStoppedForCabEndWash(bool doControl)
	{
		 if(m_trainStoppedForCabEndWash != doControl)
		   m_bTwpNormalChange = true;
        m_trainStoppedForCabEndWash = doControl;
        m_doTrainStoppedForCabEndWashControl = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateTrainStoppedForCabEndWash() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateStopWpControl(bool doControl)
	{
		 if(m_stopWp != doControl)
		   m_bTwpNormalChange = true;
		m_stopWp = doControl;
        m_doStopWpControl = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"updateStopWpControl() - Debug: Control [%d] requested", doControl);
	}
		
	void AtsTwpData::updateRestartWpControl(bool doControl)
	{
		 if(m_restartWp != doControl)
		   m_bTwpNormalChange = true;
        m_restartWp = doControl;
        m_doRestartWpControl = true;

	    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
		    "updateRestartWpControl() - Debug: Control [%d] requested", doControl);
	}

	//
	void AtsTwpData::ReSendUpdata()
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		    "TD19853 ReSendUpdata() -  ReSend all comman to Twp");
		bool sendAlarm = false; 
		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::NO_WASH, m_noWash))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send NO_WASH Command falied");
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send NO_WASH Command OK");
		}
		
		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::DAILY_RINSE, m_dailyRinseWash))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send DAILY_RINSE Command falied");
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send DAILY_RINSE Command OK");
		}

		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::DAILY_DETERGENT, m_dailyDetergentWash))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send DAILY_DETERGENT Command falied");	
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send DAILY_DETERGENT Command OK");
		}

		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::WEEKLY_WASH, m_weeklyWash))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send WEEKLY_WASH Command falied");	
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send WEEKLY_WASH Command OK");
		}

		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::CAB_END_WASH, m_trainStoppedForCabEndWash))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send CAB_END_WASH Command falied");	
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send CAB_END_WASH Command OK");	
		}

		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::STOP_WP, m_stopWp))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send STOP_WP Command falied");	
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send STOP_WP Command OK");	
		}

		if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::RESTART_WP, m_restartWp))
		{
			sendAlarm = true;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send RESTART_WP Command falied");	
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send RESTART_WP Command OK");			
		}

		if (sendAlarm)  
		{
            // only set to true here
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD19853 Send Alarm ");	
			m_dpHelper.setAlarmState(AtsTwpDataSend.getTypeKey(), true);
		}
	}

	//Command send when Twp link Normal and last command have diffenern part with perious times.
	void AtsTwpData::sendUpdates()
	{
		bool   bTwpLink = false;
		getTwpValue(TA_IRS_Core::AtsTWPData::WP_STATUS,bTwpLink);

		if(!bTwpLink)
		{
			m_bPreTwpLink = false;
			m_bTwpNormalChange = false;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"TD19853 Twp Link  CurTwpLink  false, Not Send Command");
			return;
		}

		bool bSendAll = false;
		if(bTwpLink && !m_bPreTwpLink)
		{
			bSendAll = true;
		}
		else
		{ 
			bSendAll  = false;
		}
		
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"TD19853 Twp Link  CurTwpLink = %d,PreTwpLink=%d,bSendAll = %d,m_valid = %d,m_bTwpNormalChange = %d", bTwpLink ,m_bPreTwpLink,bSendAll,m_valid,m_bTwpNormalChange);		//if(m_bTwpRecoverLink)

		if(bSendAll)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD19853  Twp Link Reconver, send all command");
			ReSendUpdata();
			m_bTwpNormalChange = false;
		}
		else
		{
			//other only send different command to Twp.
			if (m_valid && m_bTwpNormalChange)
			{
				bool sendAlarm = false;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD19853 Send diffenert to Twp");
				if (m_doNoWashControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::NO_WASH, m_noWash))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send NO_WASH Command falied");
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send NO_WASH Command OK");
					}
					m_doNoWashControl = false;
				}

				if (m_doDailyRinseWashControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::DAILY_RINSE, m_dailyRinseWash))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send DAILY_RINSE Command falied");
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send DAILY_RINSE Command OK");
					}
					m_doDailyRinseWashControl = false;
				}
				
				if (m_doDailyDetergentWashControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::DAILY_DETERGENT, m_dailyDetergentWash))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send DAILY_DETERGENT Command falied");	
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send DAILY_DETERGENT Command OK");
					}
					m_doDailyDetergentWashControl = false;
				}
				
				if (m_doWeeklyWashControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::WEEKLY_WASH, m_weeklyWash))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send WEEKLY_WASH Command falied");	
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send WEEKLY_WASH Command OK");
					}
					m_doWeeklyWashControl = false;
				}
				
				if (m_doTrainStoppedForCabEndWashControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::CAB_END_WASH, m_trainStoppedForCabEndWash))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send CAB_END_WASH Command falied");	
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send CAB_END_WASH Command OK");	
					}
					m_doTrainStoppedForCabEndWashControl = false;
				}	
				
				if (m_doStopWpControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::STOP_WP, m_stopWp))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send STOP_WP Command falied");	
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send STOP_WP Command OK");	
					}
					m_doStopWpControl = false;
				}
				if (m_doRestartWpControl)
				{
					if (!controlTwpDataPoint(TA_IRS_Core::AtsTWPData::RESTART_WP, m_restartWp))
					{
						sendAlarm = true;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send RESTART_WP Command falied");	
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
							"TD19853 Send RESTART_WP Command OK");			
					}
					m_doRestartWpControl = false;
				}
				m_bTwpNormalChange = false;
				if (sendAlarm)  
				{
					m_dpHelper.setAlarmState(AtsTwpDataSend.getTypeKey(), true);
				}
			}
			else
			{
				if(m_valid && !m_bTwpNormalChange)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					 "TD19853 -  same as previous times, send nothing");
				}
				else if(!m_valid)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					 "TD19853 - m_vaild = flase send noting");
				}
				m_bTwpNormalChange = false;
			}
		}
	}


	//bool AtsTwpData::controlTwpDataPoint(std::string command)
	bool AtsTwpData::controlTwpDataPoint(std::string command, bool bstatus)
	{
		bool success = false;
		std::map<std::string, unsigned long>::iterator it = m_twpOutputDataPoints.find(command);

		if (it != m_twpOutputDataPoints.end())
		{
			try
			{
				//TD19853
				//success = m_dpHelper.setBooleanDataPointValue((*it).second, true);
				success = m_dpHelper.setBooleanDataPointValue((*it).second, bstatus);
				//TD19853

				if (success)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"controlTwpDataPoint() - Info: DataPoint set successfully for command %s", command.c_str());
					m_bPreTwpLink = true;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"controlTwpDataPoint() - Error: DataPoint could not be controlled for command %s", command.c_str());
				}
			}
			catch (TA_Base_Core::FatalConfigurationException &)
			{
				std::string text = "controlTwpDataPoint() - Error: Bad Configuration for datapoint for command " + command;
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", text.c_str());
			}
			catch( const CORBA::Exception& e )
			{
				std::string text = "controlTwpDataPoint() - Error: DataPoint could not be controlled for command " + command
									+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
				LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
			}
            catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "AtsTwpData() - Unknown Exception adding DataPoint");

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"controlTwpDataPoint() - Error: DataPoint could not be controlled for command %s", command.c_str());
			}
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"controlTwpDataPoint() - Error: DataPoint not found for command %s", command.c_str());
		}

		return success;
	}

	unsigned char AtsTwpData::getTwpValue(std::string dp, bool& valid)
	{
		unsigned char value = false;
		std::map<std::string, unsigned long>::iterator it = m_twpInputDataPoints.find(dp);
		std::map<std::string, unsigned char>::iterator it2 = m_dpnametovalue.find(dp);
		
		std::string assertText = "Fatal: Bad Configuration - Missing TWP DataPoint " + dp;
		TA_ASSERT(it != m_twpInputDataPoints.end(), assertText.c_str());
		TA_ASSERT(it2 != m_dpnametovalue.end(), assertText.c_str());

		try
		{
			if (dp == TA_IRS_Core::AtsTWPData::WP_STATUS)
			{
				std::vector<unsigned char> result = 
					m_dpHelper.getCovertedBcdTextDataPointValue((*it).second);

				if (result.size() != 1)
				{
					value = 0;
					valid = false;
				}
				else
				{
					value = result[0];
				}

                valid = m_dpHelper.isTextDataPointQualityGood((*it).second);
			}
			else
			{
				bool result = m_dpHelper.getBooleanDataPointValue((*it).second);
				value = (result?1:0);

                valid = m_dpHelper.isBooleanDataPointQualityGood((*it).second);
			}
		}
		catch (TA_Base_Core::FatalConfigurationException &e)
		{
			std::string message = "getTwpValue() - Bad Configuration for DataPoint " + dp + " - " + e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", message.c_str());

			value = 0;
			valid = false;
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getTwpValue() - Unknown Exception retrieving TWP Value");

			value = 0;
			valid = false;
		}
		//TD15264++
		if ( value != (*it2).second )
		{		
			// different with the previous value, log it.
			//first get cobarname of the datapoint ( ex. KCD.TWP.TWP.TWP.diitwp-nowashmode)

			std::string dpcorbaname = m_dpHelper.getTWPDataPointCorbaName((*it).second);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"value of DataPoint\"%s\" changed from %d to %d.",
									dpcorbaname.c_str(),
									(*it2).second,
									value);
		}

		(*it2).second = value ;
		//++TD15264
		return value;
	}
}
