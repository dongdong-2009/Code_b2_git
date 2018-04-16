#ifndef DATAPOINTDATA_H
#define DATAPOINTDATA_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/DataPointData.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class contains the attributes that are stored for
  * trending and methods to access them.
  *
  */

#include <time.h>
#include <string>
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"


namespace TA_App
{
    class DataPointData
    {

    public:

        /**
          * DataPointData
          *
          * Constructor.
		  *
		  */
	    DataPointData() {};

	    /**
          * DataPointData
          *
          * Constructor.
		  *
		  * @param dpId pkey of datapoint
          *
          */
	    DataPointData(unsigned long dpId);

        /**
          * DataPointData
          *
          * Constructor.
		  *
		  * @param dpId pkey of datapoint
          * @param dpState structure containing datapoint value, state and timestamp
          *
          */
	    DataPointData(unsigned long dpId, const TA_Bus::DataPointCorbaDef::ClientDataPointState& dpState);

	    /**
          * DataPointData
          *
          * Copy constructor.
		  *
		  */
	    DataPointData(const DataPointData& dpData);

	    /**
          * DataPointData
          *
          * Destructor.
		  *
		  */
	    ~DataPointData() {};

	    /**
          * dataPointId
          *
          * Get datapoint pkey.
          *
          * @return Datapoint pkey
          *
		  */
	    unsigned long dataPointId() const
        {
            return m_dataPointId;
        }

	    /**
          * dataPointValue
          *
          * Get datapoint value.
          *
          * @return Datapoint value
          *
		  */
	    TA_Bus::DataPointCorbaDef::DataPointValue dataPointValue() const
        {
            return m_dataPointValue;
        }

        /**
          * getValueAsString
          *
          * Get datapoint value as a string.
          *
          * @return Datapoint value in string format
          *
		  */
	    std::string getValueAsString() const;

        /**
          * getValueAsDouble
          *
          * Get datapoint value as a double.
          *
          * @return Datapoint value in floating point format
          *
		  */
	    double getValueAsDouble() const;

	    /**
          * dataPointValue
          *
          * Set datapoint value.
          *
          * @param dpValue Datapoint value
          *
		  */
	    void dataPointValue(TA_Bus::DataPointCorbaDef::DataPointValue dpValue)
        {
            m_dataPointValue = dpValue;
        }

	    /**
          * dataPointStatus
          *
          * Get datapoint status.
          *
          * @return Datapoint status
          *
		  */
	    unsigned long dataPointStatus() const
        {
            return m_dataPointStatus;
        }

	    /**
          * dataPointStatus
          *
          * Set datapoint status.
          *
          * @param dpStatus Datapoint status
          *
		  */
	    void dataPointStatus(unsigned long dpStatus)
        {
            m_dataPointStatus = dpStatus;
        }

	    /**
          * dataCollectionOn
          *
          * Get datapoint trending status.
          *
          * @return TRUE if trending enabled
          *
		  */
	    bool dataCollectionOn() const
        {
            return m_dataCollectOn;
        }

	    /**
          * dataCollectionOn
          *
          * Set datapoint trending status.
          *
          * @param dataCollection TRUE if trending enabled
          *
		  */
	    void dataCollectionOn(bool dataCollectionOn)
        {
            m_dataCollectOn = dataCollectionOn;
        }

	    /**
          * dataTimeStamp
          *
          * Get latest timestamp.
          *
          * @return Timestamp
          *
		  */
	    time_t dataTimeStamp() const
        {
            return m_lastTimeStamp;
        }

	    /**
          * dataTimeStamp
          *
          * Set latest timestamp.
          *
          * @param time Timestamp
          *
		  */
	    void dataTimeStamp(time_t time)
        {
            m_lastTimeStamp = time;
        }

	    /**
          * compareLastTimeStamp
          *
          * Compare a time to the last timestamp.
          *
          * @return 1 if time is GREATER than the last timestamp
          *         -1 if time is LESS than last timestamp
          *         0 if times are the SAME
          *
		  */
	    int compareLastTimeStamp(time_t timeToCompare);

        /**
          * setError
          *
          * Set the comms error flag.
          *
          * @param isError TRUE: there is a comms error, FALSE: no error
          *
		  */
        void setError(bool isError)
        {
            m_commsError = isError;
        }

        /**
          * getError
          *
          * Determine if there is a comms error.
          *
          * @return TRUE: there is a comms error, FALSE: no error
          *
		  */
        bool getError() const
        {
            return m_commsError;
        }

    private:

	    unsigned long m_dataPointId;                            // Pkey of data point

        bool m_dataCollectOn;	                                // Trending status of datapoint

	    TA_Bus::DataPointCorbaDef::DataPointValue m_dataPointValue;  // Value associated with data point

	    unsigned long m_dataPointStatus;	                    // Status of data point

	    time_t m_lastTimeStamp;		                            // Time of last update

        std::string m_dataPointName;                            // Name of data point

        bool m_commsError;                                      // Set if datapoint couldn't be resolved

    };

} // TA_App

#endif // DATAPOINTDATA_H
