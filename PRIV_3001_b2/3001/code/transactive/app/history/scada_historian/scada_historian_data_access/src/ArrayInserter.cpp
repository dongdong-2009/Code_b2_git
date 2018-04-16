/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/ArrayInserter.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "app/history/scada_historian/scada_historian_data_access/src/ArrayInserter.h"
#include "core/utilities/src/DebugUtil.h"


// Cannot include full ACE header because of conflicting typedef between
// fstream.h (which ACE includes) and oratypes.h (which SimpleDb imports).
// We're going to trick the OS.h file that we have already included fstream.h
#define _INC_FSTREAM
#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

using namespace TA_Core;


namespace TA_App
{
    
    ArrayInserter::ArrayInserter(const char* tablename, const char* connection, const char* user, const char* passwd, const char* hostname /*=""*/)
	: OCIAccessExtended(hostname, connection, user, passwd, MAX_ITERATIONS), 
	  m_IsOpen(false),
	  m_tableName(tablename)

    {
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Creating ArrayInserter...");
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "ArrayInserter(): connection='%s', user='%s', passwd='%s', hostname='%s'",
		    connection, user, passwd, hostname);

	    m_bindVars = new BindVars;
    }

    ArrayInserter::~ArrayInserter()
    {
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Destroying ArrayInserter...");

	    Close();

        if(m_bindVars != NULL)
        {
            delete m_bindVars;
            m_bindVars = NULL;
        }
    }


    //
    // Open()
    //
    // Delegate to OCIAccess
    //

    bool ArrayInserter::Open()
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ArrayInserter::Open()");
        if (!m_IsOpen)
        {
            m_IsOpen = open();
        }
        return m_IsOpen;
    }


    //
    // Close()
    //

    void ArrayInserter::Close()
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ArrayInserter::Close()");
        if (m_IsOpen)
        {
            close();
            m_IsOpen = false;
        }
    }


    //
    // IsOpen()
    //
    // Do we have an open connection?
    //

    bool ArrayInserter::IsOpen()
    {
        return m_IsOpen;
    }


    bool ArrayInserter::insert(DataPointDataList& DpDataList, time_t timestamp)
    {
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ArrayInserter::insert()");

        //
	    // Get the current time
        //
        // 8/07/02 Craig wants GMT instead of local time 
        //
	    time_t currentTime = timestamp;
	    struct tm t;
	    
        char str[22];
        if(ACE_OS::gmtime_r(&currentTime, &t) != NULL)
	    {
            //
		    // Ensure that seconds will be inserted into database as 0.
            //
		    int sec = 0;
		    sprintf(str, "%02d/%02d/%04d %02d:%02d:%02d", 
                t.tm_mday, t.tm_mon + 1, t.tm_year + 1900, t.tm_hour, t.tm_min, sec);
	    }
	    
	    DataPointDataList::iterator iter = DpDataList.begin();
	    bool failure = false;

	    ////////// Debug purposes /////////////
	    int size = DpDataList.size();
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "DataList size: %d", size);
	    ///////////////////////////////////////
	    
        //
	    // Loop through the list of data points, inserting a chunk at a time
	    // into the database using an array insert.
        //
	    while(iter != DpDataList.end())
	    {
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "New Iteration...");

            //
		    // The arrays to insert the data. Each field is inserted
		    // into a separate array.
            //
		    unsigned long entitykey[MAX_ITERATIONS];
		    double value[MAX_ITERATIONS];
		    char timestamp[MAX_ITERATIONS][20+2];
		    char status[MAX_ITERATIONS][4+2];
            int commsError[MAX_ITERATIONS];

		    int count = 0; // Count of how many data points stored in array

            //
		    // Loop through the list of data points, inserting the data into
		    // the appropriate arrays. Keep a count of how many records are 
		    // inserted, and if the maximum amount is reached, then execute 
		    // an array insert, before continuing with the rest of the data.
            //
		    for(; iter != DpDataList.end(); ++iter)
		    {
                //
			    // Status
                //
			    char buffer[6];
			    sprintf(buffer, "%lu", iter->dataPointStatus());
                
                //
                // ANITA: 2/5/03: Have to copy each element at a time because
                //        ISO C++ forbids assignments of arrays.
                //
                for(int i=0; i<6; ++i)
                {
                    status[count][i] = buffer[i];
                }
    
			    std::string statusStr = buffer;
			    int size = statusStr.size();
			    status[count][size] = '\0';
			    
                //
			    // Entitykey
                //
			    entitykey[count] = iter->dataPointId();
			    
                //
			    // Value
                //
			    value[count] = iter->getValueAsDouble();
			    
                //
			    // Timestamp
                //

                //
                // ANITA: 2/5/03: Have to copy each element at a time because
                //        ISO C++ forbids assignments of arrays.
                //
                for(int j=0; j<22; ++j)
                {
                    timestamp[count][j] = str[j];
                }
			    
			    std::string timeStr = str;
			    size = timeStr.size();
			    timestamp[count][size] = '\0';

                //
                // CommsError
                //
                if(iter->getError())
                {
                    commsError[count] = 1;
                }
                else
                {
                    commsError[count] = 0;
                }
			    
			    ++count;

                //
			    // Check if we have reached maximum capacity
                //
			    if(count == MAX_ITERATIONS)
			    {
				    ++iter;
				    break;
			    }
		    } // end for

            if(count != 0)
            {
                //
	            // Set the number of iterations for the array insert. This is so the
	            // array insert won't continue looking at the next elements if count
	            // hasn't reached maximum capacity.
                //
	            if(iter == DpDataList.end())
                {
		            setMaxIterations(count);
                }

                //
	            // Add bind vars
                //
	            m_bindVars->add(TIMESTAMP_FNAME.c_str(),    static_cast<const void*>(&timestamp),       DBA_STRING, sizeof(timestamp[0]), NULL, 0);
	            m_bindVars->add(ENTITYKEY_FNAME.c_str(),    static_cast<const void*>(&entitykey[0]),    DBA_INT,    sizeof(entitykey[0]), NULL, 0);
	            m_bindVars->add(STATUS_FNAME.c_str(),       static_cast<const void*>(&status),          DBA_STRING, sizeof(status[0]), NULL, 0);
	            m_bindVars->add(VALUE_FNAME.c_str(),        static_cast<const void*>(&value[0]),        DBA_NUM,    sizeof(value[0]), NULL, 0); 
                m_bindVars->add(COMMS_ERROR_FNAME.c_str(),  static_cast<const void*>(&commsError[0]),   DBA_INT,    sizeof(commsError[0]), NULL, 0); 

                //
	            // Construct sql string
                //
	            std::string sql("insert into ");
	            sql += m_tableName;
                sql += " (TIMESTAMP, ENTITYKEY, VALUE, STATUS, COMMS_ERROR) VALUES (TO_DATE(";
	            sql += TIMESTAMP_FNAME;
                sql += ", 'DD/MM/YYYY HH24:MI:SS'), ";
                sql += ENTITYKEY_FNAME;
                sql += ", ";
	            sql += VALUE_FNAME;
                sql += ", ";
                sql += STATUS_FNAME;
                sql += ", ";
                sql += COMMS_ERROR_FNAME;
                sql += ")";
	            
                //
	            // Do the insertion thing
                //
	            long rc;
	            if(!execArrayInsert(sql.c_str(), rc, m_bindVars))
	            {
		            failure = true;
	            }
            }

	    } // end while

	    return !failure;
    }

 
} // TA_App


