/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/DataPointData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "app/history/scada_historian/src/DataPointData.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_App
{
    //
    // DataPointData
    //
    DataPointData::DataPointData(unsigned long dpId)
	    : m_dataPointId(dpId),
          m_dataCollectOn(true),
          m_dataPointStatus(0),
          m_lastTimeStamp(0),
          m_commsError(false)
    {
	    m_dataPointValue.floatValue(0);
    }


    //
    // DataPointData
    //
    DataPointData::DataPointData(unsigned long dpId, const TA_Bus::DataPointCorbaDef::ClientDataPointState& dpState)
	    : m_dataCollectOn(true),
          m_commsError(false)
    {
	    m_dataPointId = dpId;
	    m_dataPointStatus = dpState.dataPointStatus;
	    m_dataPointValue = dpState.value;
	    m_lastTimeStamp = dpState.timeStamp;
    }


    //
    // DataPointData
    //
    DataPointData::DataPointData(const DataPointData& dpData)
    {
	    m_dataPointId = dpData.m_dataPointId;
	    m_dataPointStatus = dpData.m_dataPointStatus;
	    m_dataPointValue = dpData.m_dataPointValue;
	    m_dataCollectOn = dpData.m_dataCollectOn;
	    m_lastTimeStamp = dpData.m_lastTimeStamp;
        m_commsError = dpData.m_commsError;
    }


    //
    // getValueAsString
    //
    std::string DataPointData::getValueAsString() const
    {
	    char buffer[256];
	    std::string value;

	    switch(m_dataPointValue._d())
	    {
	        case TA_Bus::DataPointCorbaDef::dpvFloat:
                //
				// value is a double
                //
				sprintf(buffer, "%lf", m_dataPointValue.floatValue());
                value = buffer;
				break;

	        case TA_Bus::DataPointCorbaDef::dpvBoolean:
                //
				// value is a boolean
                //
				if(m_dataPointValue.booleanValue())
					value = "1";
				else
					value = "0";
				break;

	        case TA_Bus::DataPointCorbaDef::dpvText:
            {
                //
				// value is a string
                //
                std::string tempStr("");
                TA_Bus::DataPointCorbaDef::ByteSequence seq = m_dataPointValue.stringValue();
                for(unsigned int i = 0; i < seq.length(); i++)
                {
                    tempStr += (char) seq[i];
                }

                value = tempStr;
				//value = m_dataPointValue.stringValue();
				break;
            }
	        default:
                //
                // unknown
                //
                value = "";
	    }

	    return value;
    }


    //
    // getValueAsDouble
    //
    double DataPointData::getValueAsDouble() const
    {
	    double value;

	    switch(m_dataPointValue._d())
	    {
	        case TA_Bus::DataPointCorbaDef::dpvFloat:
                //
				// value is a double
                //
				value = m_dataPointValue.floatValue();
				break;

	        case TA_Bus::DataPointCorbaDef::dpvBoolean:
                //
				// value is a boolean
                //
				if(m_dataPointValue.booleanValue())
					value = 1;
				else
					value = 0;
				break;

	        case TA_Bus::DataPointCorbaDef::dpvText:
                //
				// value is a string
                //
				value = -1;
				break;

	        default:
                //
                // unknown
                //
				value = -1;
	    }

	    return value;
    }


    //
    // getValueAsDouble
    //
    int DataPointData::compareLastTimeStamp(time_t timeToCompare)
    {
	    if(timeToCompare > m_lastTimeStamp)
        {
            return 1;
        }
	    else if(timeToCompare < m_lastTimeStamp)
        {
            return -1;
        }
	    else
        {
            return 0;
        }
    }

} // TA_App

