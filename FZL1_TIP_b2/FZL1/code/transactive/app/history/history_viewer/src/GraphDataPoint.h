#if !defined(AFX_GRAPHDATAPOINT_H__06F3D9F6_8C8B_11D6_B24E_0050BAB25370__INCLUDED_)
#define AFX_GRAPHDATAPOINT_H__06F3D9F6_8C8B_11D6_B24E_0050BAB25370__INCLUDED_

/**
  * GraphDataPoint.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/GraphDataPoint.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the graphing for a data point.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <time.h>
#ifdef USE_OCCI
#include <occi.h>
#endif // USE_OCCI
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <series.h>


namespace TA_App
{
    //
    // GraphDataPoint
    //
    class GraphDataPoint  
    {
    public:

        //
        // Reasons for Bad Quality
        //
        enum eQualityReason
        {
            COMMS_ERROR,    // Displayed as Red
            BAD_STATUS      // Displayed as Black
        };

    public:

        /**
		  * GraphDataPoint
		  * 
		  * Constructor.
		  *
		  */
        GraphDataPoint(long entityKey, CSeries& seriesIn, long seriesNum,
				       double engLow, double engHigh, double scaleFactor);

        /**
		  * ~GraphDataPoint
		  * 
		  * Destructor.
		  *
		  */
        virtual ~GraphDataPoint() {}

        /**
		  * LoadData
		  * 
		  * Load data for the given date/time range
		  *
		  * @param histConfigKey - key representing the type of data (eg. 1 min samples, 6 min ave)
          * @param startTime - Start time of graph
          * @param endTime - End time of graph
		  *
		  */
        void LoadData(unsigned long histConfigKey, const COleDateTime& startTime, const COleDateTime& endTime);

        /**
		  * LoadData
		  * 
		  * Load data for the given date/time range
		  *
		  * @param startTime - Start time of graph
          * @param endTime - End time of graph
          * @param tableName - Name of table to load from in database
          * @param firstLoad - Indicates whether data has been loaded previously
		  *
		  */
        void LoadData(const COleDateTime& startTime, const COleDateTime& endTime,
			          const std::string& tableName, bool firstLoad = false);

        /**
		  * InRange
		  * 
		  * Test if the given values are in the data range
		  *
		  * @param startTime - Start time to test
          * @param endTime - End time to test
          *
          * @return TRUE/FALSE - True if times are in range
          *
		  */
        bool InRange(const COleDateTime& startTime, const COleDateTime& endTime) const;

        /**
		  * EmptySeries
		  * 
		  * Empty the series and reset the start and end times
		  *
		  */
        void EmptySeries();

        /**
		  * IsSameHistoryType
		  * 
		  * Returns true if it is the same history type (eg. 1 min samples, 6 min ave).
		  *
		  * @param histConfigKey - Key representing the type of data
          *
          * @return TRUE/FALSE - True if is same type
          *
		  */
        bool IsSameHistoryType(unsigned long histConfigKey) const;

        /**
		  * DeleteFromStart
		  * 
		  * Delete points earlier than the start time
		  *
		  * @param startTime - Delete points up until this time
          *
		  */
        void DeleteFromStart(const COleDateTime& startTime);

        /**
		  * DeleteFromEnd
		  * 
		  * Delete points later than the end time
		  *
		  * @param endTime - Delete points after this time
          *
		  */
        void DeleteFromEnd(const COleDateTime& endTime);

        /**
		  * CheckExpansion
		  * 
		  * Check the expansion of the range and delete values if necessary.
		  *
		  * @param startTime - New start time
          * @param endTime - New end time
          * @param samplePeriod - Sample period
          *
		  */
        void CheckExpansion(const COleDateTime& startTime, const COleDateTime& endTime, long samplePeriod);

        /**
		  * GetEndTimes
		  * 
		  * Get the first and last date/times in the data series
		  *
		  * @param firstTime - OUT parameter: will contain the first time in the data series
          * @param lastTime - OUT parameter: will contain the last time in the data series
          *
		  */
        void GetEndTimes(COleDateTime& firstTime, COleDateTime& lastTime);

        /**
		  * NullData
		  * 
		  * Add nulls where gaps greater than the sample period appear in the data.
		  *
		  * @param firstTime - First time in the data series
          * @param lastTime - Last time in the data series
          *
		  */
        void NullData(const COleDateTime& firstTime, const COleDateTime& lastTime);

        /**
		  * AdjustSeriesNumber
		  * 
		  * Adjust the series numbering for series added after the deleted one
		  *
		  * @param seriesNum - Series number for the new series
          *
		  */
        void AdjustSeriesNumber(long seriesNum);

        /**
		  * updatePlot
		  * 
		  * Add a new point to the graph.
		  *
		  * @param timestamp - Time (x-axis) at which to plot the point
          * @param value - Value (y-axis) at which to plot the point
          * @param status - Status of the data point
          * @param badQuality - Indicates whether there is a communications error (0:NO, 1:YES, -1:UNKNOWN)
          *
		  */
        void updatePlot(time_t timestamp, double value, unsigned long status, int badQuality = 0);

        /**
		  * updatePenStyle
		  * 
		  * Change the pen style (pattern of the line) of the trend.
		  *
		  * @param style - Style identifier
          *
		  */
        void updatePenStyle(long style);

        /**
		  * getActualValue
		  * 
		  * Retrieve the actual value (not percentage) of a point.
		  *
		  * @param index - Index of the point in the trend (series)
          *
		  */
        double getActualValue(int index);


        class CouldNotLoadException
        {
        public:
	        const std::string GetErrorMsg() const
            { 
                return m_ErrorMsg; 
            }
	        
            void SetErrorMsg( std::string errorMsg) 
            { 
                m_ErrorMsg = errorMsg; 
            }

        private:
	        std::string m_ErrorMsg;
        };

    private:

        void setBadQualityPoint(eQualityReason reason);
        bool isBadStatus(int status) const;

    public:

        long GetEntityKey() const
        {
            return m_EntityKey;
        }

        long GetSeriesNum() const
        {
            return m_SeriesNum;
        }

        void SetAxisIndex(long axisIndex) 
        {
            m_AxisIndex = axisIndex;
        }

        long GetAxisIndex() const
        {
            return m_AxisIndex;
        }

        double getLatestValue() const
        {
            return m_latestGraphValue;
        }

        void setLatestValue(double value)
        {
            m_latestGraphValue = value;
        }

        double getEngLow() const
        {
            return m_EngLow;
        }

        double getEngHigh() const
        {
            return m_EngHigh;
        }

     private:

        long m_EntityKey;
        COLORREF m_LineColour;
        CSeries m_Series;
        long m_SeriesNum;
        COleDateTime m_DataStartDateTime;
        COleDateTime m_DataEndDateTime;
        long m_HistConfigKey;
        long m_AxisIndex;
        double m_EngLow;
        double m_EngHigh;
        double m_ScaleFactor;

        double m_latestGraphValue;
        time_t m_latestTime;

        bool m_commsError;
        bool m_badStatus;

        static TA_Core::ReEntrantThreadLockable m_lock;
    };
}

#endif // !defined(AFX_GRAPHDATAPOINT_H__06F3D9F6_8C8B_11D6_B24E_0050BAB25370__INCLUDED_)




