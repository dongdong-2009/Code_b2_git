/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Robin Ashcroft
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#include "app/signs/tis_agent/src/STISTrainInformationMessage.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "ILocationStore.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISTrainInformationMessage::STISTrainInformationMessage()
	{
		m_messageIDString = "M09";
	}


	STISTrainInformationMessage::~STISTrainInformationMessage()
	{
	}


	std::vector<unsigned char> STISTrainInformationMessage::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M09

        // Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)
        std::stringstream dataStream;

		// Now the actual data

        // Station
        dataStream << m_stationID;

        // platform id (1-9)
        dataStream << m_platformID;

        // platform information
        platformInfoToStream(m_platformInfo,
			m_trainInfo.firstTrain.valid,
			m_trainInfo.secondTrain.valid,
			dataStream);

        // next train information
        trainInfoToStream(m_trainInfo.firstTrain, dataStream);

        // second next train information
        trainInfoToStream(m_trainInfo.secondTrain, dataStream);

        // Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
        messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating it
		messageStream << dataString;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISTrainInformationMessage::log(bool sendSuccess)
	{
	}


    void STISTrainInformationMessage::platformInfoToStream(const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo,
														   bool nextTrainValid, bool secondNextTrainValid,
                                                           std::stringstream& dataStream)
    {
        booleanToStream(platformInfo.preArrival, dataStream);
		booleanToStream(platformInfo.arrival, dataStream);
		booleanToStream(platformInfo.preDeparture, dataStream);
		booleanToStream(platformInfo.departure, dataStream);
		booleanToStream(platformInfo.skip, dataStream);
		booleanToStream(platformInfo.hold, dataStream);
		booleanToStream(platformInfo.evacuation, dataStream);
		booleanToStream(platformInfo.trainOverrun, dataStream);
		booleanToStream(nextTrainValid, dataStream);
		booleanToStream(secondNextTrainValid, dataStream);
    }


    void STISTrainInformationMessage::trainInfoToStream(const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfo& trainInfo,
                                                        std::stringstream& dataStream)
    {
        if (trainInfo.valid)
        {
    	    dataStream << std::setw(3) << std::setfill('0') << static_cast<int>(trainInfo.physicalTrainNumber);

		    dataStream << std::setw(3) << std::setfill('0') << static_cast<int>(trainInfo.serviceNumber);

		    
            if (trainInfo.destinationStationId != 0)  // TD20559 LC ++
            {

                TA_Base_Core::ILocation* location = NULL;
                //wenbo++,CL-21314
                location = ILocationStore::getInstance()->getLocationByKey(trainInfo.destinationStationId);
				//++wenbo

                if (location != NULL)
                {
                    dataStream << location->getName();
                }
            }
            else
            {
                //dataStream << std::setw(3) <<std::setfill('0');
				dataStream << "000";
            }   

		    // Is this train the last train in service?
		    booleanToStream(trainInfo.lastTrain, dataStream);

		    // Predicated time of this train
		    dataStream << std::setw(4) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.year);

		    dataStream << std::setw(2) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.month);
		    dataStream << std::setw(2) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.day);
		    dataStream << std::setw(2) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.hour);
		    dataStream << std::setw(2) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.minute);
		    dataStream << std::setw(2) << std::setfill('0') << static_cast<int>(trainInfo.predictedTime.second);

		    booleanToStream( trainInfo.stationType, dataStream );
        }
        else
        {
            // fill the structure with zeros
            dataStream << std::setw(25) << std::setfill('0') << 0;
        }
    }


    void STISTrainInformationMessage::booleanToStream(bool value,
                                                      std::stringstream& dataStream)
    {
        if (value)
        {
            dataStream << "1";
        }
        else
        {
            dataStream << "0";
        }
    }

	void STISTrainInformationMessage::setData( TA_IRS_Bus::PlatformIdType platformId, const std::string& stationId, 
			const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo, 
			const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair& trainInfoPair)
	{
		m_platformID = platformId;

		m_stationID = stationId;

		m_platformInfo = platformInfo;

		m_trainInfo = trainInfoPair;

        if (m_platformID > 9)
        {
            std::stringstream err;
            err << "Invalid Platform Id (" << m_platformID << ") to send to the STIS server. Must be between 1 and 9.";

            TA_THROW( TA_IRS_Core::TISAgentException( err.str().c_str() ) );
        }
	}
	
}