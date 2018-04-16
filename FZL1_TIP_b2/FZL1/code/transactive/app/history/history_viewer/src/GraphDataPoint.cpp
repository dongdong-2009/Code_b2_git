/**
  * GraphDataPoint.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/GraphDataPoint.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the graphing for a data point.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/historyviewer.h"
#include "app/history/history_viewer/src/GraphDataPoint.h"
#include "app/history/history_viewer/src/DataBaseConnection.h"
#include "app/history/history_viewer/src/HistoryConfig.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/TimeFunction.h"
#include "app/history/history_viewer/src/globals.h"
#include "app/history/history_viewer/src/DataPointInfo.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/HistoryInitialisationException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/DBException.h"
#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

#ifdef USE_OCCI
#include <occi.h>
#endif // USE_OCCI

#include <valuelist.h>
#include <lineseries.h>
#include <pen.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace // unnamed
{
	const long EXTRA_SAMPLES = 120;	                             // The number of extra samples to load

	const COLORREF badQualityColourCommsError = RGB(255, 0, 0);  // Red
	const COLORREF badQualityColourBadStatus = RGB(0, 0, 0);     // Black

	const double MAX_POINT = 100;                                // Maximum value on graph
	const double MIN_POINT = 0;                                  // Minimum value on graph
} // unnamed namespace

using namespace TA_Core;

namespace TA_App
{
    ReEntrantThreadLockable GraphDataPoint::m_lock;

    //
    // GraphDataPoint
    //
    GraphDataPoint::GraphDataPoint(long entityKey, CSeries& seriesIn, long seriesNum, 
        double engLow, double engHigh, double scaleFactor)
        : m_EntityKey(entityKey),
        m_SeriesNum(seriesNum),
        m_DataStartDateTime(0.0),
        m_DataEndDateTime(0.0),
        m_HistConfigKey(0),
        m_EngLow(engLow),
        m_EngHigh(engHigh),
        m_ScaleFactor(scaleFactor),
        m_latestGraphValue(0),
        m_latestTime(0),
        m_commsError(false),
        m_badStatus(false)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Construct GraphDataPoint Object");

        m_Series = seriesIn;
        m_LineColour = m_Series.GetColor();

        //
        // Used to convert from engineering units to a percentage
        //
        //m_ScaleFactor = 100.0/(m_EngHigh - m_EngLow);
    }

    
    // 
    // Load Data
    //
    void GraphDataPoint::LoadData( unsigned long histConfigKey,
						           const COleDateTime& origStartTime,
						           const COleDateTime& origEndTime )
    {
        FUNCTION_ENTRY("LoadData");

        TA_ASSERT(origStartTime < origEndTime, "Start time is greater than end time");

        //
        // Get the history configuration key
        //
        std::string tableName = HistoryConfig::getInstance().GetTableName(histConfigKey);

        //
        // If we have changed history types, get rid of the old data
        //
        // Don't want to empty the series if this object has just been created. Checking
        // m_HistConfigKey will tell us if this method has been called for the first time.
        //
        if( !IsSameHistoryType( histConfigKey ) && m_HistConfigKey != 0)
        {
	        EmptySeries();
        }
        m_HistConfigKey = histConfigKey;

        //
        // Get the sample period in seconds (given the history config key)
        //
        long samplePeriod = HistoryConfig::getInstance().GetSamplePeriod( histConfigKey );
        TA_ASSERT(samplePeriod > 0 , "Sample period is invalid. Must be greater than 0");

        //
        // If start time is greater than current time, we won't have data so return.
        //
        COleDateTime currentTime = COleDateTime::GetCurrentTime();
        if(origStartTime > currentTime)
        {
	        return;
        }

        //
        // One second time span used for adjustments later
        //
        COleDateTimeSpan oneSecond( 1.0/(double)secInDay );

        //
        // Adjust start and end to load more data than requested
        //
        COleDateTimeSpan extraTime( ((double)EXTRA_SAMPLES*(double)samplePeriod)/(double)secInDay );
        COleDateTime startTime = origStartTime - extraTime;
        COleDateTime endTime = origEndTime + extraTime;

        //
        // If end time is greater than current time, adjust it to current time.
        //
        if( endTime > currentTime )
        {
	        endTime = currentTime;
        }

        //
        // Check the number of samples if expanding range
        //
        if(	m_DataStartDateTime.m_dt != 0.0 || m_DataEndDateTime.m_dt != 0.0 )
        {
            //
            // Check the expansion of the range and delete values in necessary.
            //
	        CheckExpansion( startTime, endTime, samplePeriod);
        }

        //
        // If this is the first load
        //
        if(	m_DataStartDateTime.m_dt == 0.0 && m_DataEndDateTime.m_dt == 0.0 )
        {
	        LoadData( startTime, endTime, tableName, true );
	        m_DataStartDateTime = startTime;
	        m_DataEndDateTime = endTime;
        }
        else 
        {
            //
	        // Not the first load, may need to expand range
            //
	        if( startTime < m_DataStartDateTime )
	        {
		        COleDateTime endLoad = m_DataStartDateTime - oneSecond;
		        LoadData( startTime, endLoad, tableName );
		        m_DataStartDateTime = startTime;
	        }
	        if( endTime > m_DataEndDateTime )
	        {
		        COleDateTime startLoad = m_DataEndDateTime + oneSecond;
		        LoadData( startLoad, endTime, tableName );
		        m_DataEndDateTime = endTime;
	        }
        }

        FUNCTION_EXIT;
    }


    // 
    // LoadData
    //
    void GraphDataPoint::LoadData( const COleDateTime& startTime,
						           const COleDateTime& endTime,
						           const std::string& tableName, 
                                   bool firstLoad /* = false */)
    {
        FUNCTION_ENTRY("LoadData");

        char entityKey[21] = {0};
        sprintf( entityKey, "%ld", m_EntityKey );

        //
        // Get the first and last date/times in the data series
        //
        COleDateTime firstTime, lastTime;
        GetEndTimes( firstTime, lastTime );
        
    #ifdef USE_OCCI
	        std::string sql( "SELECT TIMESTAMP, VALUE, STATUS, COMMS_ERROR FROM " );
    #else // USE_OCCI
           std::string sql( "SELECT TO_CHAR( TIMESTAMP, 'YYYYMMDDHH24MISS'), VALUE, STATUS, COMMS_ERROR FROM " );
    #endif // USE_OCCI
        
        sql += tableName;
        sql += " WHERE TIMESTAMP >= TO_DATE('";
        sql += TimeFunction::getInstance().ConvertToUTC( startTime );
        sql += "', 'DD/MM/YYYY HH24:MI') AND TIMESTAMP <= TO_DATE('";
        sql += TimeFunction::getInstance().ConvertToUTC( endTime );
        sql += "', 'DD/MM/YYYY HH24:MI') AND ENTITYKEY = ";
        sql += entityKey;
        sql += " ORDER BY TIMESTAMP";
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, sql.c_str());

        double dataValue;
        COleDateTime date;
        int status;
        int commsError;

    #ifdef USE_OCCI
        //
        // Get the oracle connection.  As errors in the oracle connection will have already
        // resulted in an error being displayed, don't display another one here.
        //
        oracle::occi::Connection *conn;
        try
        {
	        conn = DataBaseConnection::getInstance().GetConnection();
        }
        catch(...)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not create database connection.");
	        return;
        }

        oracle::occi::Statement *stmt = NULL;
        oracle::occi::ResultSet *resultSet = NULL;
        try
        {
            //
            // Create the statement and set to get a maximum of 500 rows at a time 
            // (doesn't mean that it is limited to 500 rows)
            //
	        stmt = conn->createStatement( sql );
            stmt->setPrefetchRowCount( 500 );
	        stmt->setPrefetchMemorySize( 0 ); // To ensure it uses the row count
            resultSet = stmt->executeQuery();

			int test = 0;
	        while( resultSet->next() )
	        {
				++test;
		        //date.ParseDateTime( resultSet->getString( 1 ).c_str() );
		        oracle::occi::Date inDate = resultSet->getDate( 1 );
		        
                //
                // Scale the value as a percentage.
                //
		        dataValue = ( resultSet->getDouble( 2 ) - m_EngLow ) * m_ScaleFactor;
                status = resultSet->getInteger( 3 );
                commsError = resultSet->getInteger( 4 );
                TimeFunction::getInstance().ConvertToLocal( inDate, date );

                //
                // If the value is beyond the graphical limits (the value should be between
                // 0 and 100 since it is calculated as a percentage) then reset the value
                // and set the quality to bad
                //
                if(dataValue > MAX_POINT || dataValue < MIN_POINT)
                {
                    dataValue = 0;
                    status = -1;
                }
				
				m_Series.AddXY( DATE(date), dataValue, "", m_LineColour );

                //
                // Colour the point a different colour if it is bad quality
                //
                m_commsError = false;
                m_badStatus = false;
                if(1 == commsError)
                {
                    setBadQualityPoint(COMMS_ERROR);
                }
                else
                {
                    if(isBadStatus(status))
                    {
                        setBadQualityPoint(BAD_STATUS);
                    }
                }   
	        }

	        stmt->closeResultSet( resultSet );
	        conn->terminateStatement( stmt );
        }
        catch ( oracle::occi::SQLException &sqlException )
        {
	        if( resultSet )
	        {
		        stmt->closeResultSet( resultSet );
	        }
	        if( stmt )
	        {
		        conn->terminateStatement( stmt );
	        }

	        // Log the Oracle Error Message (includes error number)
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not load datapoints.");
            LOGMORE(SourceInfo, sqlException.getMessage().c_str());

	        // Throw exception
	        CouldNotLoadException e;
	        e.SetErrorMsg( sqlException.getMessage() );
	        throw e;
        }
    #else // USE_OCCI
		SimpleDb* db = SimpleDBConnection::getInstance().GetConnection();
		SimpleDb::Buffer buffer;
		int columns = 4;

		try
		{
			db->Exec( sql.c_str(), columns, buffer );
		}
		catch( const DBException& e )
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
		}

        if (buffer.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No data found for datapoint.");
	        return;
        }

        double databaseValue = 0;
        bool hasMore = true;
        do 
        {
	        int rows = buffer.size();
	        for (int i=0; i<rows; ++i)
	        {
                //
		        // Scale the value as a percentage.
                //
                databaseValue = atof(buffer[i][1].c_str());
		        dataValue = ( atof(buffer[i][1].c_str()) - m_EngLow ) * m_ScaleFactor;
                try
                {
		            TimeFunction::getInstance().ConvertToLocal(buffer[i][0], date );
                }
                catch(const TA_Core::HistoryInitialisationException& ex)
                {
                    char buff[256] = {0};
                    sprintf(buff, "Error converting timstamp: %s: %s", buffer[i][0].c_str(), ex.what());
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, buff);
                    continue;
                }
                commsError = atoi(buffer[i][3].c_str());
                status = atoi(buffer[i][2].c_str());

                // Fix for TD2574 - Now that each trend has a custom axis, the value added using
                // AddXY() is plotted against the custom axis rather than the percentage axis.
                // So the value needs to be converted accordingly.
                DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(m_EntityKey);
                if(dataPoint.dpType == DP_BOOLEAN_TYPE)
                {
                    double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                    double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                    double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                    double diff = highValue - lowValue;
                    dataValue = (1/diff)*dataValue - (1/diff)*lowValue;
                }
                else
                {
                    dataValue = atof(buffer[i][1].c_str());
                }

                //
                // If the value is beyond the graphical limits (the value should be between
                // 0 and 100 since it is calculated as a percentage) then reset the value
                // and set the quality to bad
                //
                if(dataValue > dataPoint.engHigh || dataValue < dataPoint.engLow)
                {
                    dataValue = 0;
                    status = -1;
                }
		        
                m_Series.AddXY( DATE(date), dataValue, "", m_LineColour );

                //
                // Colour the point a different colour if it is bad quality
                //
                m_commsError = false;
                m_badStatus = false;
                if(1 == commsError)
                {
                    setBadQualityPoint(COMMS_ERROR);
                }
                else
                {
                    if(isBadStatus(status))
                    {
                        setBadQualityPoint(BAD_STATUS);
                    }
                }   
	        }

			try
			{
				if( (hasMore = db->HasMore()) == true )
				{
					db->FetchMore(columns, buffer);
				}
			}
			catch( const DBException& e )
			{
				LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
			}
        } while ( hasMore );
    #endif // USE_OCCI

        if(firstLoad)
        {
            //
            // The latest value is only saved on the first load. This is where this value
            // is first set. Every other time it is set whenever there is a real-time update.
            // If we set it everytime data is loaded, the latest value that was set by a
            // real-time update could be overwritten by the last sample value that is older
            // than the latest value.
            //
            //m_latestGraphValue = dataValue / m_ScaleFactor + m_EngLow;
            m_latestGraphValue = databaseValue;

            // We could also set m_lastestTime here, but the constructor has already set
            // it to 0, and we want the first real-time update to update the latestGraphValue
            // anyway. So by not setting it here, we will ensure that timeReceived > m_latestTime.
        }

        //
        // Add nulls where necessary
        //
        NullData( firstTime, lastTime ); 

        LOG(SourceInfo, DebugUtil::FunctionExit, "LoadData2");
    }


    //
    // AdjustSeriesNumber
    //
    void GraphDataPoint::AdjustSeriesNumber(long seriesNum)
    {
        FUNCTION_ENTRY("AdjustSeriesNumber");
        
        if( m_SeriesNum>seriesNum ) 
        {
            m_SeriesNum--;
        }

        FUNCTION_EXIT;
    }


    // 
    // InRange
    //
    bool GraphDataPoint::InRange(const COleDateTime& startTime, const COleDateTime& endTime) const
    {
        FUNCTION_ENTRY("InRange");

        TA_ASSERT(startTime < endTime, "Start time is greater than End time");

        if(startTime > m_DataStartDateTime && endTime < m_DataEndDateTime)
        {
	        return true;
        }

        return false;
    }


    // 
    // IsSameHistoryType
    //
    bool GraphDataPoint::IsSameHistoryType(unsigned long histConfigKey) const
    {
        FUNCTION_ENTRY("IsSameHistoryType");

        return( m_HistConfigKey == static_cast<long>(histConfigKey) );
    }


    // 
    // EmptySeries
    //
    void GraphDataPoint::EmptySeries()
    {
        FUNCTION_ENTRY("EmptySeries");

        m_Series.Clear();
        m_DataStartDateTime = 0.0;
        m_DataEndDateTime = 0.0;

        FUNCTION_EXIT;
    }


    // 
    // CheckExpansion
    //
    void GraphDataPoint::CheckExpansion(const COleDateTime& startTime, const COleDateTime& endTime, long samplePeriod)
    {
        FUNCTION_ENTRY("CheckExpansion");

        COleDateTime expandedStartTime = __min(m_DataStartDateTime, startTime);
        COleDateTime expandedEndTime = __max(m_DataEndDateTime, endTime);
        COleDateTimeSpan expandedRange = expandedEndTime - expandedStartTime;
        long expandedNumSamples = static_cast<long>(expandedRange.GetTotalSeconds())/samplePeriod;

        //
        // If the sample limit is not exceeded by expanding the range, return.
        // Allow an extra couple of samples on each end.
        //
        if(expandedNumSamples <= sampleLimit+4)
        {
            //
	        // If the bottom of the requested region is higher than the top of
	        // the loaded region and expanding would result in a large number of
	        // samples, then delete all data and start fresh.
            //
	        if( startTime > m_DataEndDateTime )
	        {
		        COleDateTimeSpan gapInRange = startTime - m_DataEndDateTime;
		        long gapNumSamples = static_cast<long>(gapInRange.GetTotalSeconds())/samplePeriod;
		        if( gapNumSamples > (long)(sampleWarning/2) )
		        {
			        EmptySeries();
		        }
	        }

	        return;
        }

        //
        // Delete some data.  Three possibilities...
        // 1) Scrolled forward - delete data from start
        //
        if(startTime >= m_DataStartDateTime && startTime < m_DataEndDateTime)
        {
            //
	        // Given the end time, the sampleLimit and the samplePeriod, calculate
	        // the new start time for the data.  That is, don't delete more data
	        // than necessary.
            //
	        COleDateTimeSpan allowedRange = ((double)samplePeriod * (double)sampleLimit)/(double)secInDay;
	        COleDateTime newStartTime = expandedEndTime - allowedRange;

            //
	        // Delete points earlier than the calculated date/time
            //
	        DeleteFromStart( newStartTime );
        }

        //
        // 2) Scrolled backwards - delete data from end
        //
        else if(endTime > m_DataStartDateTime && endTime <= m_DataEndDateTime)
        {
            //
	        // Given the start time, the sampleLimit and the samplePeriod, calculate
	        // the new end time for the data.  That is, don't delete more data
	        // than necessary.
            //
	        COleDateTimeSpan allowedRange = ((double)samplePeriod * (double)sampleLimit)/(double)secInDay;
	        COleDateTime newEndTime = expandedStartTime + allowedRange;

            //
	        // Delete points later than the calculated end date/time
            //
	        DeleteFromEnd( newEndTime );
        }

        //
        // 3) Moved somewhere else entirely - Delete all and start again
        //
        else 
        {
	        EmptySeries();
        }

        FUNCTION_EXIT;
    }


    // 
    // DeleteFromStart
    //
    void GraphDataPoint::DeleteFromStart(const COleDateTime& startTime)
    {
        FUNCTION_ENTRY("DeleteFromStart");

        CValueList valList = m_Series.GetXValues();
        long numValues = valList.GetCount();
        long keepValue;

        //
        // Find the first value after the start time
        //
        for( keepValue=0; keepValue<numValues; keepValue++ )
        {
	        if( valList.GetValue( keepValue ) >= DATE(startTime) )
	        {
		        break;
	        }
        }

        //
        // Delete the points before the start time
        //
        for( int i=0; i < keepValue; i++ )
        {
	        m_Series.Delete( 0 );
        }

        //
        // Adjust data start time
        //
        m_DataStartDateTime = startTime;

        FUNCTION_EXIT;
    }


    // 
    // DeleteFromEnd
    //
    void GraphDataPoint::DeleteFromEnd(const COleDateTime& endTime)
    {
        FUNCTION_ENTRY("DeleteFromEnd");

        CValueList valList = m_Series.GetXValues();
        long numValues = valList.GetCount();
        long keepValue;

        //
        // Find the first value before the end time
        //
        for(keepValue=numValues-1; keepValue >= 0; --keepValue)
        {
	        if(valList.GetValue(keepValue) <= DATE(endTime))
	        {
		        break;
	        }
        }

        //
        // Delete the points after the end time
        //
        for(int i=numValues-1; i > keepValue; --i)
        {
	        m_Series.Delete(i);
        }

        //
        // Adjust data end time
        //
        m_DataEndDateTime = endTime;

        FUNCTION_EXIT;
    }

    
    // 
    // GetEndTimes
    //
    void GraphDataPoint::GetEndTimes(COleDateTime& firstTime, COleDateTime& lastTime)
    {
        FUNCTION_ENTRY("GetEndTimes");

        firstTime = 0.0;
        lastTime = 0.0;
        long valueCount;

        //
        // Return if there are no values
        //
        if((valueCount = m_Series.GetCount()) == 0)
        {
	        return;
        }

        //
        // Get the first date/time in the series
        //
        firstTime = m_Series.GetXValues().GetValue( 0 );

        //
        // Get the last date/time in the series
        //
        lastTime = m_Series.GetXValues().GetValue( valueCount-1 );

        FUNCTION_EXIT;
    }


    // 
    // NullData
    //
    void GraphDataPoint::NullData(const COleDateTime& firstTime, const COleDateTime& lastTime)
    {
        FUNCTION_ENTRY("NullData");

        CValueList valList = m_Series.GetXValues();
        bool isFirstPoint = true;
        COleDateTime addTime, prevTime, currTime=0.0;
        COleDateTimeSpan timeInterval;

        //
        // Get the sample period in seconds (given the history config key)
        //
        long samplePeriod = HistoryConfig::getInstance().GetSamplePeriod( m_HistConfigKey );
        COleDateTimeSpan minInterval( ((double)1.5*(double)samplePeriod)/(double)secInDay );
        COleDateTimeSpan addInterval( (double)samplePeriod/(double)secInDay );

        //
        // Find the first value after the start time
        //
        for( long keepValue=0; keepValue<valList.GetCount(); keepValue++ )
        {
	        prevTime = currTime;
	        currTime = valList.GetValue( keepValue );
	        if( isFirstPoint )
	        {
		        isFirstPoint = false;
		        continue;
	        }

	        if( firstTime.m_dt == 0.0  || // If there is no existing data
		        (prevTime < firstTime) || // or new values before existing data
		        (currTime > lastTime)  )  // or new values after existing data
	        {
                //
		        // Calculate the time interval
                //
		        timeInterval = currTime - prevTime;

                //
		        // If the time interval is too big, add null value to break line
                //
		        if( timeInterval > minInterval )
		        {
			        addTime = prevTime + addInterval;
			        m_Series.AddNullXY( DATE(addTime), 0.0, "" );
			        keepValue++;
		        }
	        }
        }

        FUNCTION_EXIT;
    }

    
    //
    // updatePlot
    //
    void GraphDataPoint::updatePlot(time_t timestamp, double value, unsigned long status, int badQuality /* = 0 */)
    {
        FUNCTION_ENTRY("updatePlot");

        ThreadGuard guard(m_lock);

        //
        // Need to change the time to gmtime. This is because the data recording agent
        // writes the timestamps to the database in gmtime and all the graphing
        // calculations are done by converting the times from gmtime. Hence we need
        // to convert the time to gmtime here to keep the times consistent.
        //
        struct tm newtime;
        ACE_OS::gmtime_r(&timestamp, &newtime);
        timestamp = mktime(&newtime);

        COleDateTime date;

        //
        // Scale the value as a percentage.
        //
		double dataValue = ( value - m_EngLow ) * m_ScaleFactor;

        // Fix for TD2574 - Now that each trend has a custom axis, the value added using
        // AddXY() is plotted against the custom axis rather than the percentage axis.
        // So the value needs to be converted accordingly.
        DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(m_EntityKey);
        if(dataPoint.dpType == DP_BOOLEAN_TYPE)
        {
            double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
            double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
            double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
            double diff = highValue - lowValue;
            dataValue = (1/diff)*dataValue - (1/diff)*lowValue;
        }
        else
        {
            dataValue = value;
        }


		TimeFunction::getInstance().ConvertToLocal( timestamp, date );

        CString testTime = date.Format( "%d/%m/%Y %H:%M:%S" );
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Plotting at: %s", testTime);

        char label[100] = {0};
        sprintf(label, "%lf", value);

        //
        // If the value is beyond the graphical limits (the value should be between
        // 0 and 100 since it is calculated as a percentage) then reset the value
        // and set the quality to bad
        //
        if(dataValue > dataPoint.engHigh || dataValue < dataPoint.engLow)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Data value: %f beyond graphical limits. Plotting at 0.", dataValue);
            dataValue = 0;
            status = -1;
        }

		m_Series.AddXY( DATE(date), dataValue, "", m_LineColour );

        if(badQuality == COMMS_ERROR_UNKNOWN)
        {
            //
            // This update came from refreshGraph - the status and comms error
            // are unknown so use the previous states. CommsError takes precedence
            // over BadStatus.
            //
            if(m_commsError)
            {
                setBadQualityPoint(COMMS_ERROR);
            }
            else
            {
                if(m_badStatus)
                {
                    setBadQualityPoint(BAD_STATUS);
                }
            }
        }
        else // Update came from updateRealTimePlot or adding of datapoint to graph
        {
            if(badQuality == COMMS_ERROR)
            {
                setBadQualityPoint(COMMS_ERROR);
            }
            else
            {
                m_commsError = false;

                if(isBadStatus(status))
                {
                    setBadQualityPoint(BAD_STATUS);
                }
                else
                {
                    m_badStatus = false;
                }
            }
        }

        //
        // Could NULL the data here. But if data is NULLed here then the latest value
        // plotted would still be the latest value that was inserted into the database.
        // If the Data Recording Agent has actually stopped and not writing to the
        // database, then plotting the most recent value (which was a long time ago) is
        // not accurate.
        // So what we're assuming here is that the agent will always be running and hence
        // always be writing data to the database for those datapoints with trending
        // enabled. Hence we should get areas in the graph with long lines plotted at the
        // same value from when the last value was written to the database to the most
        // current time.
        //
        /*
        CValueList valList = m_Series.GetXValues();
        int count = valList.GetCount();

        COleDateTime currTime = date;
        COleDateTime prevTime = valList.GetValue(count - 2);

        COleDateTime addTime;
        COleDateTimeSpan timeInterval;

        timeInterval = currTime - prevTime;

        long samplePeriod = HistoryConfig::getInstance().GetSamplePeriod( m_HistConfigKey );
        COleDateTimeSpan minInterval( ((double)1.5*(double)samplePeriod)/(double)secInDay );
        COleDateTimeSpan addInterval( (double)samplePeriod/(double)secInDay );

		// If the time interval is too big, add null value to break line
		if( timeInterval > minInterval )
		{
			addTime = prevTime + addInterval;
			m_Series.AddNullXY( DATE(addTime), 0.0, "" );
		}
        */

        //
        // Save the latest value to be used for refreshing the graph
        //
        if(timestamp > m_latestTime)
        {
            m_latestGraphValue = value;
            m_latestTime = timestamp;
        }

        FUNCTION_EXIT;
    }


    //
    // setBadQualityPoint
    //
    void GraphDataPoint::setBadQualityPoint(eQualityReason reason)
    {
        FUNCTION_ENTRY("setBadQualityPoint");

        CValueList valList = m_Series.GetXValues();
        long numValues = valList.GetCount();
        
        switch(reason)
        {
            case COMMS_ERROR:   m_Series.SetPointColor(numValues-1, badQualityColourCommsError);
                                m_commsError = true;
                                break;

            case BAD_STATUS:    m_Series.SetPointColor(numValues-1, badQualityColourBadStatus);
                                m_badStatus = true;
                                break;

            default:            break;  
        }

        FUNCTION_EXIT
    }


    //
    // isBadStatus
    //
    bool GraphDataPoint::isBadStatus(int status) const
    {
        FUNCTION_ENTRY("isBadStatus");

        int i = 0;
        for(; i < NUM_GOOD_QUALITY_WORDS; ++i)
        {
            if(DP_GOOD_QUALITY_WORDS[i] == static_cast<unsigned int>(status))
            {
                return false;
            }
        }

        return true;
    }


    //
    // getActualValue
    //
    double GraphDataPoint::getActualValue(int index)
    {
        FUNCTION_ENTRY("getActualValue");

        CValueList valList = m_Series.GetYValues();
        double value = valList.GetValue(index);

        // Fix for TD2574 - Now that each trend has a custom axis, the value added using
        // AddXY() is plotted against the custom axis rather than the percentage axis.
        // So the value needs to be converted accordingly.
        DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(m_EntityKey);
        if(dataPoint.dpType == DP_BOOLEAN_TYPE)
        {
            double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
            double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
            double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
            double diff = highValue - lowValue;
            value = (value + (1/diff)*lowValue)/(1/diff);

            return (value / m_ScaleFactor + m_EngLow);
        }
        else
        {
            return value;
        }
                
        

        FUNCTION_EXIT
    }


    //
    // updatePenStyle
    //
    void GraphDataPoint::updatePenStyle(long style)
    {
        FUNCTION_ENTRY("updatePenStyle");

        m_Series.GetAsLine().GetLinePen().SetStyle( style );

        FUNCTION_EXIT
    }

} // TA_History



