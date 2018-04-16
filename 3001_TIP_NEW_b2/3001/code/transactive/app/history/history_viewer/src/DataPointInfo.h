#if !defined(AFX_DATAPOINTINFO_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_DATAPOINTINFO_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * DataPointInfo.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/DataPointInfo.h $
  * @author:  Craig Grant
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class loads the data points and maintains a store of them.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"


namespace TA_App
{
    struct DataPoint
    {
        std::string entityName;
        long entityKey;
        std::string description;
        std::string dpType;
        double engLow;
        double engHigh;
        std::string	engUnits;
        bool configIsLoaded;
        bool trendingEnabled;
        double scaleLow;        // The scaleLow and scaleHigh attributes are only used for boolean
        double scaleHigh;       // datapoints. This values are used for determining values on the custom axes.
        std::string subsystem;  // This can be removed later when a database interface
                                // has been created for datapoint.
        double scaleFactor;
    };
    
    typedef std::map<unsigned long, DataPoint> DataPointMap;

    //
    // CDataPointInfo
    //
    class CDataPointInfo  
    {
    public:
        
        /**
		  * getInstance
		  * 
		  * Returns the one and only instance of CDataPointInfo.
		  *
		  */
        static CDataPointInfo& getInstance()
        {
	        static CDataPointInfo e;
	        return e;
        }

        /**
		  * ~CDataPointInfo
		  * 
		  * Destructor.
		  *
		  */
        virtual ~CDataPointInfo();

        /**
		  * LoadDataPoints
		  * 
		  * This gets the connection string from RunParams and opens the database.
          * If this is successful it then loads all the required information for
          * the datapoints.
		  *
          * @exception CouldNotLoadException - thrown if it cannot get 
          * connection parameters or connect and open the database.
          *
		  */
        void LoadDataPoints();

        /**
		  * GetDataPointMap
		  * 
		  * Retrieve the map of datapoints
		  *
          */
        const DataPointMap& GetDataPointMap();

        /**
		  * AddToGraph
		  * 
		  * Add the datapoints named in the list to the map of graphed datapoints.
          *
          * @param dataPointNameList - reference to a vector of datapoints that are added
		  *
          */
        void AddToGraph(std::vector <unsigned long>& dataPointNameList);

        /**
		  * DeleteFromGraph
		  * 
		  * Delete the datapoints named from the list of graphed datapoints.
		  *
          * @param dataPointKeyList - reference to a vector of datapoints that are removed
          *
          */
        void DeleteFromGraph(std::vector <unsigned long>& dataPointKeyList);

        /**
		  * GetDataPoint
		  * 
		  * Retreive the datapoint given the datapoint key.
		  *
          * @param dataPointKey - the data point key
          *
          * @exception DatapointNotFoundException - thrown if it cannot find
          * the datapoint key in either list.
          *
          */
        DataPoint& GetDataPoint(const unsigned long& dataPointKey);

        /**
		  * DeleteDataPoint
		  * 
		  * Delete a data point from the store.
		  *
          * @param dataPointKey - data point key
          *
          */
        void DeleteDataPoint(unsigned long dataPointKey);

        /**
		  * InsertDataPoint
		  * 
		  * Insert a data point into the store if it doesn't already exist.
		  *
          * @param datapoint - structure containing data point attributes
          *
          */
        void InsertDataPoint(const DataPoint& datapoint);

        /**
		  * UpdateDataPoint
		  * 
		  * Updates the data point attributes of a data point in the store.
		  *
          * @param datapoint - structure containing updated data point attributes
          *
          */
        void UpdateDataPoint(const DataPoint& datapoint);

    #ifndef USE_OCCI
        /**
		  * LoadConfig
		  * 
		  * Load configuration data for a datapoint given the data point key.
		  *
          * @param dpKey - data point key
          *
          */
        void LoadConfig(const unsigned long& dpKey);

        /**
		  * LoadConfig
		  * 
		  * Loads configuration data for a data point using SimpleDb.
		  *
          * @param dataPoint - structure containing updated data point attributes
          *
          */
        void LoadConfig(DataPoint& dataPoint);
    #endif

    private:

        CDataPointInfo(CDataPointInfo&) {}
        CDataPointInfo& operator=(const CDataPointInfo &) {}

        void parseStationNames(const std::string& runParamString, std::vector<std::string>& stationNames);

    public:

        const DataPointMap& GetGraphingDataPointMap() 
        {
            return m_GraphingDataPointMap;
        }

        void stopLoad()
        {
            m_stopLoad = true;
        }

        class DatapointNotFoundException{};

        class DatapointNotBooleanException{};

        class CouldNotLoadException
        {
        public:
	        const std::string GetErrorMsg() 
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

        CDataPointInfo() : m_BoolCount( 0 ), m_stopLoad(false) 
        {
            m_Loaded = false;
        }
    
    private:

        DataPointMap m_DataPointMap;            // Store of data points, except those on the graph
        DataPointMap m_GraphingDataPointMap;    // Data points currently on the graph
        bool m_Loaded;                          // Flag indicating if list has been loaded from database
        short m_BoolCount;                      // Number of booleans added to the plot
        static TA_Core::ReEntrantThreadLockable m_lock;
        bool m_stopLoad;
    };

    //
    // DataPointLoaderThread class
    //
    class DataPointLoaderThread : public TA_Core::Thread
    {
    public:
        DataPointLoaderThread() : m_loaded(false), m_terminated(false) {}
        virtual ~DataPointLoaderThread() {}

        virtual void run();

        virtual void terminate();
       
        bool isFinishedLoading()
        {
            return m_loaded;
        }

    private:
        DataPointLoaderThread( const DataPointLoaderThread& theThread);
	    DataPointLoaderThread& operator=(const DataPointLoaderThread &);

        bool m_loaded;
        bool m_terminated;
    };
}

#endif // !defined(AFX_DATAPOINTINFO_H__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)




