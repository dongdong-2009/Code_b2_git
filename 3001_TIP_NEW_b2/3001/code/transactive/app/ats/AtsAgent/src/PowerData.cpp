/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/PowerData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS ElectricalSubsection Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "app/ats/AtsAgent/src/RawTable.h"
#include "app/ats/AtsAgent/src/DataPointHelper.h"
#include "app/ats/AtsAgent/src/PowerData.h"

namespace TA_IRS_App
{
	PowerData::PowerData(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
						 unsigned short subsectionId,
						 std::vector<TA_IRS_Core::IAtsPowerData*> powerData, DataPointHelper& dpHelper)
	:
	m_subsectionId (subsectionId),
	m_valid (false),
	m_dpHelper(dpHelper)
	{
		if (powerData.size() > 8 || powerData.size() == 0)
		{
	//		TA_THROW(TA_Base_Core::FatalConfigurationException(
	//					"Configuration Error: Wrong number of Electrical Subsection Datapoints (should be between 1 and 8 inclusive)"));
		}
		
		std::vector<TA_IRS_Core::IAtsPowerData*>::iterator it;

		for (it = powerData.begin(); it != powerData.end(); it++)
		{
			try
			{
				unsigned long entityKey = (*it)->getTractionPowerDataPointEntityKey();

				unsigned long bitPosition = (*it)->getTractionPowerBitPosition();

				if (!m_dpHelper.isDataPointAdded(entityKey))
				{
					m_dpHelper.addInputBooleanDataPoint(entityKey);
				}
				m_powerStatusProxies.push_back(entityKey);
				//CL20376, hongzhi, Electrical Subsection data exchange between ATS has a problem
				m_bitPositions.push_back(bitPosition);
			}
			catch (TA_Base_Core::DatabaseException& e)
			{
				std::ostringstream text;
				text << "PowerData() - error when retrieving CorbaName for entity with key ";
				text << (*it)->getTractionPowerDataPointEntityKey() << " - " << std::string(e.what());
 				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text.str());
			}
			catch (TA_Base_Core::DataException& e)
			{
				std::ostringstream text;
				text << "PowerData() - error when retrieving CorbaName for entity with key ";
				text << (*it)->getTractionPowerDataPointEntityKey() << " - " << std::string(e.what());
 				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text.str());
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				std::ostringstream text;
				text << "PowerData() - error adding DataPoint for entity with key ";
				text << (*it)->getTractionPowerDataPointEntityKey() << " - " << std::string(e.what());
 				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text.str());
			}
			catch (...)
			{
				std::ostringstream text;
				text << "PowerData() - error adding Power Data for entity with key ";
				text << (*it)->getTractionPowerDataPointEntityKey();
 				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
			}
		}

		if (m_powerStatusProxies.size() != powerData.size())
		{
			std::ostringstream message;
			message << "Configuration Error - failed to create DataPoint Proxies for Electrical Subsection "
				    << m_subsectionId;
			TA_THROW(TA_Base_Core::AtsAgentException(message.str()));
		}
	};

	PowerData::~PowerData()
	{
	};

	unsigned short PowerData::getSubsectionId()
	{
		return m_subsectionId;
	}

	unsigned char PowerData::getValidityByte()
	{
		unsigned short validity = 0x00ff;

		std::vector<unsigned long>::iterator it;
		std::vector<unsigned long>::iterator itBitPosition;
		int bit = 0;

		for(it = m_powerStatusProxies.begin(), itBitPosition = m_bitPositions.begin();
			it != m_powerStatusProxies.end();
			it++,itBitPosition++)
		{
			try
			{
				bit = *itBitPosition;
				if (!m_dpHelper.isBooleanDataPointQualityGood(*it))
				{
					validity = RawTable::bitClear(validity, bit);
				}
			}
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				std::ostringstream message;
				message << "getValidityByte() - Bad Configuration for DataPoint with entity key ";
				message << (*it) << " - " << e.what();
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", message.str().c_str());

				validity = RawTable::bitClear(validity, bit);
			}
		}
	
		return (unsigned char) (validity & 0x00ff);
	}

	unsigned char PowerData::getPowerStatusByte()
	{
		unsigned short powerStatus = 0x00ff;

		std::vector<unsigned long>::iterator it;
		std::vector<unsigned long>::iterator itBitPosition;
		int bit = 0;

		for(it = m_powerStatusProxies.begin(), itBitPosition = m_bitPositions.begin();
			it != m_powerStatusProxies.end();
			it++, itBitPosition++)
		{
			bool dpValue = false;
			
			try
			{
				bit = *itBitPosition;
				dpValue = m_dpHelper.getBooleanDataPointValue(*it);
			}
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				std::ostringstream message;
				message << "getPowerStatusByte() - Bad Configuration for DataPoint with entity key ";
				message << (*it) << " - " << e.what();
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", message.str().c_str());
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				std::string text = "getPowerStatusByte() - " + std::string(e.what());
 				//LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text);
				//commented about line to avoid large amount of logs
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, text.c_str());
			}

			if (dpValue == false)
			{
				powerStatus = RawTable::bitClear(powerStatus, bit);
			}
		}
	
		return (unsigned char) (powerStatus & 0x00ff);
	}
}

