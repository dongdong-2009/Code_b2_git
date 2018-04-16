/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/DataPointHelper.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * Utility functions for Ats DataPoint Addresses
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/message/types/AtsAgentAlarms_MessageTypes.h"

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

#include "app/ats/AtsAgent/src/DataPointHelper.h"

namespace TA_IRS_App
{
	DataPointHelper::DataPointHelper(TA_Base_Core::AtsAgentEntityDataPtr entityData)
    : m_entityData (entityData),
	  m_isInControlMode(false), //wenching++ (TD14526)
      //TD18093, jianghp, TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper(). ( TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper() ),
   	  m_atstwpentitydata( NULL ),  //TD15213
	  m_atspsdentitydata( NULL ),
      m_proxyFactoryPtr(NULL)
    {
        FUNCTION_ENTRY("DataPointHelper()");

        FUNCTION_EXIT;
    }

	DataPointHelper::~DataPointHelper()
	{
		FUNCTION_ENTRY("~DataPointHelper()");

		m_dataPointDetailMap.clear();

		m_alarmDetailMap.clear();

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		//TD15213
		if ( m_atstwpentitydata != NULL)
		{
			delete m_atstwpentitydata;
			m_atstwpentitydata = NULL;
		}

		if ( m_atspsdentitydata != NULL)
		{
			delete m_atspsdentitydata;
			m_atspsdentitydata = NULL;
		}

        if (m_proxyFactoryPtr != 0)
		{
			m_proxyFactoryPtr->removeInstance();
			m_proxyFactoryPtr = NULL;
		}

		FUNCTION_EXIT;
	}

	void DataPointHelper::addInputBooleanDataPoint(unsigned long entityKey)
	{
		DataPointDetail *dpDetail = new DataPointDetail();
		// Get the DataPoint Proxy
		createDataPointProxy(entityKey, dpDetail->dataPointProxy);

		// Check the type
		
		if (getDataPointType(dpDetail->dataPointProxy->getEntityName()) != TA_Base_Bus::DPT_BOOLEAN_TYPE_STRING ||
			getDataPointWriteable(dpDetail->dataPointProxy->getEntityName()) == "1")
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " is the wrong type.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		// Add it to the map
		addDataPointDetail(entityKey, dpDetail, "");
	}

	void DataPointHelper::addInputTextDataPoint(unsigned long entityKey)
	{
		DataPointDetail *dpDetail = new DataPointDetail();

		// Get the DataPoint Proxy
		createDataPointProxy(entityKey, dpDetail->dataPointProxy);

		// Check the type
		if (getDataPointType(dpDetail->dataPointProxy->getEntityName()) != TA_Base_Bus::DPT_TEXT_TYPE_STRING ||
			getDataPointWriteable(dpDetail->dataPointProxy->getEntityName()) == "1")
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " is the wrong type.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		// Add it to the map
		addDataPointDetail(entityKey, dpDetail, "");
	}

	void DataPointHelper::addOutputBooleanDataPoint(unsigned long entityKey, const TA_Base_Core::MessageType& alarmType)
	{
		// Add the alarm type to the map
		addAlarmDetail(alarmType);
		
		// Add the datapoint
		addOutputBooleanDataPoint(entityKey, alarmType.getTypeKey());
	}

	void DataPointHelper::addOutputBooleanDataPoint(unsigned long entityKey, std::string alarmTypeKey)
	{
		DataPointDetail *dpDetail = new DataPointDetail();
		

		// Get the DataPoint Proxy
		createDataPointProxy(entityKey, dpDetail->dataPointProxy);
	
		// Check the type
		if (getDataPointType(dpDetail->dataPointProxy->getEntityName()) != TA_Base_Bus::DPT_BOOLEAN_TYPE_STRING ||
			getDataPointWriteable(dpDetail->dataPointProxy->getEntityName()) == "0")
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " is the wrong type.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		// Add the datapoint to the map
		addDataPointDetail(entityKey, dpDetail, alarmTypeKey);
	}

	bool DataPointHelper::isDataPointAdded(unsigned long entityKey)
	{
		DataPointDetailMapIt dpDetailIt = m_dataPointDetailMap.find(entityKey);
		
		return (dpDetailIt != m_dataPointDetailMap.end());
	}

	std::string DataPointHelper::getDataPointType(std::string objectName)
	{
		TA_Base_Core::IData* typePar = NULL;

		try
		{
			typePar = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
					objectName, TA_Base_Core::DataPointEntityData::DATAPOINT_TYPE );
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "getDataPointType() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

			return "";
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "getDataPointType() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

			return "";
		}

		std::string typeString = "";

		if (typePar != NULL)
		{
			if (typePar->getNumRows() == 1)
			{
				try
				{
					typeString = typePar->getStringData(0, "VALUE");
				}
				catch (TA_Base_Core::DataException& e)
				{
					std::string text = "getDataPointType() - " + std::string(e.what());
 					LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

					typeString = "";
				}
			}
			else
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					 "getDataPointType() - DataPoint Entity Parameter missing for %s",
					 objectName.c_str());

				typeString = "";
			}

			delete typePar;
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "getDataPointType() - DataPoint Entity Parameter can not be retrieved for %s",
				 objectName.c_str());

			typeString = "";
		}

		return typeString;
	}

	std::string DataPointHelper::getDataPointWriteable(std::string objectName)
	{
		TA_Base_Core::IData* writeablePar = NULL;

		try
		{
			writeablePar = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
					objectName, TA_Base_Core::DataPointEntityData::DATAPOINT_ALLOW_WRITE );
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "getDataPointWriteable() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

			return "";
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "getDataPointWriteable() - " + std::string(e.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

			return "";
		}

		std::string writeable = "";

		if (writeablePar != NULL)
		{
			if (writeablePar->getNumRows() == 1)
			{
				try
				{
					writeable = writeablePar->getStringData(0, "VALUE");
				}
				catch (TA_Base_Core::DataException& e)
				{
					std::string text = "getDataPointWriteable() - " + std::string(e.what());
 					LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

					writeable = "";
				}
			}
			else
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					 "getDataPointWriteable() - DataPoint Entity Parameter missing for %s",
					 objectName.c_str());

				writeable = "";
			}

			delete writeablePar;
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "getDataPointWriteable() - DataPoint Entity Parameter can not be retrieved for %s",
				 objectName.c_str());

			writeable = "";
		}

		return writeable;
	}

	bool DataPointHelper::getBooleanDataPointValue(unsigned long entityKey)
	{
		DataPointDetail& dpDetail = getDataPointDetail(entityKey);

		bool value = false;
    
        // ss++
        // TD14582
		if (dpDetail.dataPointProxy->isConfigValid())
		{
			// Check the type
			if (!dpDetail.dataPointProxy->isBoolean() || dpDetail.dataPointProxy->isWriteable())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException("getBooleanDataPointValue() - not a boolean input DataPoint"));
			}

			try
			{
				value = dpDetail.dataPointProxy->getValueAsBool();
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				std::string description = "getBooleanDataPointValue() - ScadaProxyException: " + std::string(e.what());
				//LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);
				//commented the above line to avoid large amount of logs
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, description.c_str());

				std::ostringstream message;
				message << "Error: DataPoint with Entity Key "
						<< entityKey
						<< " could not be read.";
				//commented below line to avoid large amount of logs
				//TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
			}
		}
		else
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " could not be read.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}
        // ++ss
		return value;
	}

	std::vector<unsigned char> DataPointHelper::getCovertedBcdTextDataPointValue(unsigned long entityKey)
	{
		DataPointDetail& dpDetail = getDataPointDetail(entityKey);

		std::vector<unsigned char> result;

        // ss++
        // TD14582
		if (dpDetail.dataPointProxy->isConfigValid())
		{
			// Check the type
			if (!dpDetail.dataPointProxy->isText() || dpDetail.dataPointProxy->isWriteable())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException("getCovertedBcdTextDataPointValue() - not a text input DataPoint"));
			}

            try
			{
                TA_Base_Bus::DpValue value( dpDetail.dataPointProxy->getValue() );

                TA_ASSERT(value.getType() == TA_Base_Bus::DPT_TEXT_TYPE, "getCovertedBcdTextDataPointValue() - value has incorrect type");
                
                std::string strValue = value.getText();
				// Now need to convert bytes
				for (int i = 0; i < strValue.size(); i++)
				{
					unsigned char converted = ((strValue[i]&0xF0)>>4)*10 +
											  (strValue[i]&0x0F);

					result.push_back(converted);
				}
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				std::string description = "getCovertedBcdTextDataPointValue() - ScadaProxyException: " + std::string(e.what());
				LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);

				std::ostringstream message;
				message << "Error: DataPoint with Entity Key "
						<< entityKey
						<< " could not be read.";
				TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
			}
		}
		else
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " could not be read.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}
        // ++ss

		return result;
	}

	bool DataPointHelper::isBooleanDataPointQualityGood(unsigned long entityKey)
	{
		try
		{
			DataPointDetail& dpDetail = getDataPointDetail(entityKey);

            // ss++
            // TD14582
			if (dpDetail.dataPointProxy->isConfigValid())
			{
				// Check the type
				if (!dpDetail.dataPointProxy->isBoolean() || dpDetail.dataPointProxy->isWriteable())
				{
					TA_THROW(TA_Base_Core::FatalConfigurationException("isBooleanDataPointQualityGood() - not a boolean input DataPoint"));
				}

				try
				{
					unsigned long quality = dpDetail.dataPointProxy->getStatus();

					bool goodQuality = ((quality & TA_Base_Bus::QUALITY_MASK) == TA_Base_Bus::QUALITY_GOOD);

					if (!goodQuality)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"DataPoint \"%s\" (%d) has bad quality - %d.",
									dpDetail.dataPointProxy->getDescription().c_str(),
									dpDetail.dataPointProxy->getEntityKey(),
									quality);
					}

					return goodQuality;
				}
				catch (TA_Base_Core::ScadaProxyException& e)
				{
					std::string description = "isBooleanDataPointQualityGood() - ScadaProxyException: " + std::string(e.what());
					LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);

					return false;
				}
			}
			else
			{
				return false;
			}
            // ++ss
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			std::string description = "isBooleanDataPointQualityGood() - AtsAgentException: " + std::string(e.what());
			LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);
		}

		return false;
	}

	bool DataPointHelper::isTextDataPointQualityGood(unsigned long entityKey)
	{
		try
		{
			DataPointDetail& dpDetail = getDataPointDetail(entityKey);

            // ss++
            // TD14582
			if (dpDetail.dataPointProxy->isConfigValid())
			{
				// Check the type
				if (!dpDetail.dataPointProxy->isText() || dpDetail.dataPointProxy->isWriteable())
				{
					TA_THROW(TA_Base_Core::FatalConfigurationException("isTextDataPointQualityGood() - not a text input DataPoint"));
				}

				try
				{
					unsigned long quality = dpDetail.dataPointProxy->getStatus();

					bool goodQuality = ((quality & TA_Base_Bus::QUALITY_MASK) == TA_Base_Bus::QUALITY_GOOD);

					if (!goodQuality)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"DataPoint \"%s\" (%d) has bad quality - %d.",
									dpDetail.dataPointProxy->getDescription().c_str(),
									dpDetail.dataPointProxy->getEntityKey(),
									quality);
					}

					return goodQuality;
				}
				catch (TA_Base_Core::ScadaProxyException& e)
				{
					std::string description = "isTextDataPointQualityGood() - ScadaProxyException: " + std::string(e.what());
					LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);

					return false;
				}
			}
			else
			{
				return false;
			}
            //++ss
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			std::string description = "isTextDataPointQualityGood() - AtsAgentException: " + std::string(e.what());
			LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);
		}

		return false;
	}

	bool DataPointHelper::setBooleanDataPointValue(unsigned long entityKey, bool value)
	{
		bool success = false;

		if ( !m_isInControlMode ) //wenching++ (TD14526)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
                    "setBooleanDataPointValue() - Error: DataPoint with entity key %d - DataPoint in monitor mode",
                    entityKey);
			return false;
        }
        
		try
		{
			DataPointDetail& dpDetail = getDataPointDetail(entityKey);

            // ss++
            // TD14582
			if (dpDetail.dataPointProxy->isConfigValid())
			{
				// Check the type
				if (!dpDetail.dataPointProxy->isBoolean() || !dpDetail.dataPointProxy->isWriteable())
				{
					TA_THROW(TA_Base_Core::FatalConfigurationException("setBooleanDataPointValue() - not a boolean output DataPoint"));
				}

				// Try to write to the datapoint
				bool retry = false;
				unsigned int attempt = 0;

				do
				{
					attempt++;

					try
					{
						std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();
						std::string controlValue = dpDetail.dataPointProxy->getBooleanLabel(value);
						//TD15264

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"Attempt to Write value %d to DataPoint %s ",								
								  	value?1:0,
									dpDetail.dataPointProxy->getEntityName().c_str()
									);

                        const unsigned long HEARTBEART_TRIGGER = 0xFFFFFFFE;//limin++, PSD Control Command access DB issue
						dpDetail.dataPointProxy->setValue(controlValue, session, HEARTBEART_TRIGGER );//limin++ PSD Control Command access DB issue

						retry = false;
						success = true;
					}
					catch (TA_Base_Core::ScadaProxyException& e)
					{
						std::string description = "setBooleanDataPointValue() - ScadaProxyException: " + std::string(e.what());
						LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);

						LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
							"setBooleanDataPointValue() - Error: DataPoint with entity key %d - DataPoint write failed",
							entityKey);
						retry = (attempt < 2);
					}
					catch (...)
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							"setBooleanDataPointValue() - Error: DataPoint write failed");
						retry = (attempt < 2);

						continue;
					}
				}
				while (retry);
			}
            // ++ss			
		}
		catch (TA_Base_Core::AtsAgentException &e)
		{
			std::string description = "processEntityUpdateEvent() - AtsAgentException: " + std::string(e.what());
			LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);

			return false;
		}

		return success;
	}


	void DataPointHelper::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		if (updateType == TA_Base_Bus::ControlStateUpdate)
		{
			try
			{
				DataPointDetail& dpDetail = getDataPointDetail(entityKey);

				//  TD14999 
				if ( dpDetail.dataPointProxy->isBoolean() && dpDetail.dataPointProxy->isWriteable())
				{
					TA_Base_Bus::EDataPointControlState controlState = dpDetail.dataPointProxy->getControlState();

					if ((controlState == TA_Base_Bus::failed ||
						 controlState == TA_Base_Bus::LCC_failed ||
						 controlState == TA_Base_Bus::RCC_failed ||
						 controlState == TA_Base_Bus::comms_failed) &&
						 dpDetail.alarmTypeKey != "")
					{
						//TD15264
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"fail to Write value %d to DataPoint %s.",
									dpDetail.dataPointProxy->getValueAsBool()?1:0,
									dpDetail.dataPointProxy->getEntityName().c_str()
									);



						setAlarmState(dpDetail.alarmTypeKey, true);
					}
					// TD15257
                    else if ((controlState == TA_Base_Bus::conditionalSuccess ||
						 controlState == TA_Base_Bus::success ) &&
						 dpDetail.alarmTypeKey != "")
                    {
						//TD15264
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"success to Write value %d to DataPoint %s.",
									dpDetail.dataPointProxy->getValueAsBool()?1:0,
									dpDetail.dataPointProxy->getEntityName().c_str()
									);
                        setAlarmState(dpDetail.alarmTypeKey, false);
                    }
				}
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				std::string description = "processEntityUpdateEvent() - AtsAgentException: " + std::string(e.what());
				LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);
			}
			catch (TA_Base_Core::ScadaProxyException& e)
			{
				std::string description = "processEntityUpdateEvent() - ScadaProxyException: " + std::string(e.what());
				LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", 
									 "processEntityUpdateEvent() - Unknown Exception Caught");
			}
		}
	}
	

	void DataPointHelper::setAlarmState(std::string alarmTypeKey, bool isInAlarm)
	{
		FUNCTION_ENTRY("setAlarmState()");


		//TD15213 benefit debug
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"DataPointHelper:setAlarmState alarmTypekey is %s,AtsTwp typekey is %s",
				alarmTypeKey.c_str(),TA_Base_Core::AtsAgentAlarms::AtsTwpDataSend.getTypeKey().c_str());
		
		// Get the alarm detail
		try
		{
			AlarmDetail& alarmDetail = getAlarmDetail(alarmTypeKey);
			
			// if the current comms status is bad then raise an alarm
			if ( true == isInAlarm  )
			{
				// if no alarm and event have been generated
				if ( false == alarmDetail.alarmGenerated )
				{
					// construct the DescriptionParameters list
					TA_Base_Core::DescriptionParameters descriptionParameters;
					TA_Base_Core::NameValuePair pair1 ( "AtsAgentName", m_entityData->getName() );
					descriptionParameters.push_back ( &pair1 );
					
					try
					{
						//TD15213++
						if ( alarmTypeKey == TA_Base_Core::AtsAgentAlarms::AtsTwpDataSend.getTypeKey() )
						{
							
							
							if ( NULL == m_atstwpentitydata )
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
									"DataPointHelper:setAlarmState m_atstwpentitydata is NULL");
								return;
							}
							// construct the DescriptionParameters list
							TA_Base_Core::DescriptionParameters descriptionParameters2;
							TA_Base_Core::NameValuePair pair2 ( "AtsTwpName", m_atstwpentitydata->getName() );
							descriptionParameters2.push_back ( &pair2 );
							TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm(alarmDetail.alarmType,
								m_atstwpentitydata->getKey(),			// entity key
								m_atstwpentitydata->getTypeKey(),		// entity type key
								descriptionParameters2,
								m_atstwpentitydata->getName(),		// entity name
								m_atstwpentitydata->getLocation(),	// location key
								m_atstwpentitydata->getPhysicalSubsystem());	// subystem key
							//m_atstwpentitydata->getAssetName(),	// asset name
							//TA_Base_Core::AlarmConstants::defaultTime );
						}
						else if ( alarmTypeKey == TA_Base_Core::AtsAgentAlarms::AtsPsdDataSend.getTypeKey() )
						{
							
							
							if ( NULL == m_atspsdentitydata )
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
									"DataPointHelper:setAlarmState m_atspsdentitydata is NULL");
								return;
							}
							// construct the DescriptionParameters list
							TA_Base_Core::DescriptionParameters descriptionParameters3;
							TA_Base_Core::NameValuePair pair3 ( "AtsPsdName", m_atspsdentitydata->getName() );
							descriptionParameters3.push_back ( &pair3 );
							TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm(alarmDetail.alarmType,
								m_atspsdentitydata->getKey(),			// entity key
								m_atspsdentitydata->getTypeKey(),		// entity type key
								descriptionParameters3,
								m_atspsdentitydata->getName(),		// entity name
								m_atspsdentitydata->getLocation(),	// location key
								m_atspsdentitydata->getPhysicalSubsystem());	// subystem key
							//m_atstwpentitydata->getAssetName(),	// asset name
							//TA_Base_Core::AlarmConstants::defaultTime );
						}
						//++TD15213
						else
						{
							//submit an unique alarm
							TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset ( alarmDetail.alarmType,
								m_entityData->getKey(),			// entity key
								m_entityData->getTypeKey(),		// entity type key
								descriptionParameters,
								m_entityData->getName(),		// entity name
								m_entityData->getLocation(),	// location key
								m_entityData->getSubsystem(),	// subystem key
								m_entityData->getAssetName(),	// asset name														 
								TA_Base_Core::AlarmConstants::defaultTime );
						}
                        
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"Alarm submitted. AlarmTypeKey=%s, Key=%u, TypeKey=%u, Name=%s, locationKey=%u, subsystemKey=%u",
							alarmDetail.alarmType.getTypeKey().c_str(),
							m_entityData->getKey(),m_entityData->getTypeKey(),
							m_entityData->getName().c_str(),m_entityData->getLocation(),
							m_entityData->getSubsystem());
					}
					catch ( ... )
					{
						LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
							"setAlarmState() - Unknown Exception caught while submitting alarm.");
					}
					
					// set up the flags
					alarmDetail.alarmGenerated = true;
				}
			}
			
			//
			// Else if the current comms state is good then close the alarm
			//
			else
			{
				// if alarm and event have been generated
				if ( true == alarmDetail.alarmGenerated )
				{
					// fwc++
					if ( alarmTypeKey == TA_Base_Core::AtsAgentAlarms::AtsTwpDataSend.getTypeKey() )
					{
						if ( NULL == m_atstwpentitydata )
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
								"DataPointHelper:setAlarmState m_atstwpentitydata is NULL");
							return;
						}

						// close the unique alarm
						TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation ( alarmDetail.alarmType,
							m_atstwpentitydata->getKey(),
							m_atstwpentitydata->getLocation() );
						
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"Alarm closed at location. AlarmTypeKey=%s, Key=%u, Location=%u",
							alarmDetail.alarmType.getTypeKey().c_str(),
							m_atstwpentitydata->getKey(),m_atstwpentitydata->getLocation());
						
						// set up the flags
						alarmDetail.alarmGenerated = false;
					}
					else if ( alarmTypeKey == TA_Base_Core::AtsAgentAlarms::AtsPsdDataSend.getTypeKey() )
					{
                        if ( NULL == m_atspsdentitydata )
                        {
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "DataPointHelper:setAlarmState m_atspsdentitydata is NULL");
                            return;
						}
						// close the unique alarm
						TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation ( alarmDetail.alarmType,
							m_atspsdentitydata->getKey(),
							m_atspsdentitydata->getLocation() );
						
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"Alarm closed at location. AlarmTypeKey=%s, Key=%u, Location=%u",
							alarmDetail.alarmType.getTypeKey().c_str(),
							m_atspsdentitydata->getKey(),m_atspsdentitydata->getLocation());
						
						// set up the flags
						alarmDetail.alarmGenerated = false;
					}
					else
					{
						// close the unique alarm
						TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation ( alarmDetail.alarmType,
							m_entityData->getKey(),
							m_entityData->getLocation() );
							
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"Alarm closed at location. AlarmTypeKey=%s, Key=%u, Location=%u",
							alarmDetail.alarmType.getTypeKey().c_str(),
							m_entityData->getKey(),m_entityData->getLocation());
						
						// set up the flags
						alarmDetail.alarmGenerated = false;
					}//fwc++ end
				}
			}
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			std::string description = "setAlarmState() - AtsAgentException: " + std::string(e.what());
			LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);
		}

		FUNCTION_EXIT;
	}

	bool DataPointHelper::isInAlarm(std::string alarmTypeKey)
	{
		try
		{
			// Get the alarm detail
			AlarmDetail& alarmDetail = getAlarmDetail(alarmTypeKey);

			return alarmDetail.alarmGenerated;
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			std::string description = "isInAlarm() - AtsAgentException: " + std::string(e.what());
			LOG_EXCEPTION_CATCH( SourceInfo, "AtsAgentException", description);
		}

		return false;
	}

	//
	// Protected members
	//

	void DataPointHelper::createDataPointProxy(unsigned long entityKey, TA_Base_Bus::DataPointProxySmartPtr & dataPoint)
	{
		DataPointDetailMapIt dpDetailIt = m_dataPointDetailMap.find(entityKey);

		if (dpDetailIt != m_dataPointDetailMap.end())
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " has already been added.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		try
		{				
			TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(entityKey, *this, dataPoint);
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "createDataPointProxy() - Unknown Exception Caught");

			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " can not be created.";

			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}
	}

	void DataPointHelper::addDataPointDetail(unsigned long entityKey,
											 DataPointDetail *dpDetail,
											 std::string alarmTypeKey)
	{
		
		dpDetail->alarmTypeKey = alarmTypeKey;

		std::pair<DataPointDetailMapIt, bool> result = 
				m_dataPointDetailMap.insert(DataPointDetailMap::value_type(entityKey, dpDetail));

		// Insert should succeed
		TA_ASSERT(result.second == true, "addDataPointDetail() - insertion of DataPointDetail failed");
	}

	DataPointHelper::DataPointDetail& DataPointHelper::getDataPointDetail(unsigned long entityKey)
	{
		DataPointDetailMapIt dpDetailIt = m_dataPointDetailMap.find(entityKey);

		if (dpDetailIt == m_dataPointDetailMap.end())
		{
			std::ostringstream message;
			message << "Error: DataPoint with Entity Key "
					<< entityKey
					<< " has not been added.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		return *((*dpDetailIt).second);
	}

	void DataPointHelper::addAlarmDetail(const TA_Base_Core::MessageType& alarmType)
	{
		AlarmDetailMapIt it = m_alarmDetailMap.find(alarmType.getTypeKey());
			
		if (it == m_alarmDetailMap.end())
		{
			AlarmDetail alarmDetail(alarmType);

			alarmDetail.alarmGenerated = true;

			std::pair<AlarmDetailMapIt, bool> result = 
					m_alarmDetailMap.insert(AlarmDetailMap::value_type(alarmType.getTypeKey(), alarmDetail));

			// Insert should succeed
			TA_ASSERT(result.second == true, "addAlarmDetail() - insertion of AlarmDetail failed");
			
			// Clear out old alarms
			// setAlarmState(alarmType.getTypeKey(), false);
			// fwc++
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation ( alarmDetail.alarmType,
			m_entityData->getKey(),
			m_entityData->getLocation() );
		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Alarm closed at location. AlarmTypeKey=%s, Key=%u, Location=%u",
			alarmDetail.alarmType.getTypeKey().c_str(),
			m_entityData->getKey(),m_entityData->getLocation());		
			
			// set up the flags
			alarmDetail.alarmGenerated = false;
			//fwc++ end
		}
	}

	DataPointHelper::AlarmDetail& DataPointHelper::getAlarmDetail(std::string alarmTypeKey)
	{
		AlarmDetailMapIt alarmDetailIt = m_alarmDetailMap.find(alarmTypeKey);

		if (alarmDetailIt == m_alarmDetailMap.end())
		{
			std::ostringstream message;
			message << "Error: Alarm with Type Key "
					<< alarmTypeKey
					<< " has not been added.";
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}

		return (*alarmDetailIt).second;
	}

	//wenching++ (TD14526)
	void DataPointHelper::updateIsInControlMode( bool isControlMode )
	{
		m_isInControlMode = isControlMode;
	}
	//++wenching (TD14526)



	 //TD15264++ 
	std::string     DataPointHelper::getTWPDataPointCorbaName(unsigned long entityKey)
	{
		DataPointDetail& dpDetail = getDataPointDetail(entityKey);

		std::string dpcorbaname;

		try
			{
				dpcorbaname = dpDetail.dataPointProxy->getEntityName();
			}
		catch (TA_Base_Core::ScadaProxyException& e)
			{
				std::string description = "getTWPDataPointCorbaName() - ScadaProxyException: " + std::string(e.what());
				LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", description);

				std::ostringstream message;
				message << "Error: could not get TWP DataPoint corbaname with Entity Key "
						<< entityKey;
						
				TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
			}
		catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"getTWPDataPointCorbaName:unknown error when getCorbaName "								
							);		
			}

		return dpcorbaname;
	}
	//++TD15264
	//TD15213++
	void DataPointHelper::setAtsTwpEntityData(TA_Base_Core::AtsTwpEntityData* atstwpentitydata)
	{
			unsigned long entitykey = atstwpentitydata->getKey();   			
		
			m_atstwpentitydata = new TA_Base_Core::AtsTwpEntityData(entitykey) ;		

			if ( NULL == m_atstwpentitydata )
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"DataPointHelper:fail to create twpentitydata"							
							);		
		
	}
	void DataPointHelper::setAtsPsdEntityData(TA_Base_Core::AtsPsdEntityData* atspsdentitydata)
	{
			unsigned long entitykey = atspsdentitydata->getKey();   			
		
			m_atspsdentitydata = new TA_Base_Core::AtsPsdEntityData(entitykey) ;		

			if ( NULL == m_atspsdentitydata )
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"DataPointHelper:fail to create psdentitydata"							
							);		
		
	}
	//++TD15213
}
