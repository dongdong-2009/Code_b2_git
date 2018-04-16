/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/TVEvents/src/sendDataPointStateUpdateEvent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
/*
 * This is a basic application that inserts data into
 * the HISTORY_BASE table (base samples) and sends
 * an event (consumed by the Trend Viewer) to notify
 * of the data insertion - mimicking the Data Recording
 * Agent. A time interval is used to determine how
 * often to insert data into the database. This can be
 * set as a commandline parameter (seconds), the default
 * interval is 60 seconds.
 *
 * Usage:
 *
 *  TVSendEvent --EntityKey=<entitykey of datapoint>
 *              --LowRange=<low eng limit>
 *              --HighRange=<high eng limit>
 *              --DbConnection<database connection string>
 *              --NotifyHosts=<hostname>
 *              [--TimeInterval=<seconds>]
 *
 * Type "quit" to terminate app.
 *
 */


#include <iostream>
#include <time.h>
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/database/src/SimpleDb.h"
//#include "app/history/scada_historian/scada_historian_dmif/src/ScadaHistorianProcessTimeUpdateCorbaDef.h"
#include "MonitorInputThread.h"
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"


const int DEFAULT_TIME_INTERVAL = 60;
const int DEFAULT_STATUS = 192;

using namespace TA_Core;
using TA_Bus::DataPointCorbaDef;

//
// Forward declarations
//
bool isValidCommandLine(TA_Core::RunParams, const char*);
void setUpCorba(TA_Core::RunParams);
void runProcessingLoop(SimpleDb&, CommsMessageSender*, unsigned long, int, int, int, unsigned long, int);


///////////////////////////////////
//
// Main
//
///////////////////////////////////

int main( int argc, char* argv[] )
{
    //
    // Parse command lines and check they are valid
    //
	TA_Core::RunParams params = TA_Core::RunParams::getInstance();
	params.parseCmdLine( argc, argv );

	if(!isValidCommandLine(params, argv[0]))
    {
        return 0;
    }

    //
	// Retrieve parameters from command line
	//
	unsigned long entitykey = atol(params.get( "EntityKey" ).c_str());
    int lowRange = atol(params.get("LowRange").c_str());
    int highRange = atol(params.get("HighRange").c_str());
    int timeInterval = params.isSet( "TimeInterval" ) ? atol( params.get( "TimeInterval" ).c_str() ) : DEFAULT_TIME_INTERVAL;
    int status = params.isSet( "Status" ) ? atoi( params.get( "Status" ).c_str() ) : DEFAULT_STATUS;

    if(params.isSet("Boolean"))
    {
        RunParams::getInstance().set("Boolean", "");
    }

    //
    // Initialise corba
    //
    setUpCorba(params);

    //
    // Create Sender
    //
    CommsMessageSender* sender = MessagePublicationManager::getInstance().getCommsMessageSender( TA_Core::DataPointStateUpdate::Context );

    //
    // Set up Database connection
    //
    std::string dbConnect = params.get( "DbConnection" );
    std::vector<std::string> components;
    SimpleDb::ParseConnectStr(dbConnect, components);
    SimpleDb db(components[0].c_str(), components[1].c_str(), components[2].c_str());
    if(!db.Open())
    {
      std::cout << std::endl << std::endl
	      << "Problem connecting with database."
	      << std::endl;
      return 0;
    }

    //
    // Get the entity key of the base sample entity
    //
    std::string execute("select pkey from entity where name='BaseSample'");
    SimpleDb::Buffer data;
    db.Exec(execute.c_str(), 1, data);

    if(data.empty())
    {
        std::cout << std::endl << std::endl
	          << "Error: No BaseSample entity exists."
	          << std::endl;
        return 0;
    }

    unsigned long bsKey = atol((data[0][0]).c_str());

    //
    // Run the loop that enters values into database
    // and sends update messages to Trend Viewer
    //
    runProcessingLoop(db, sender, entitykey, lowRange, highRange, timeInterval, bsKey, status);

    //
    // "quit" has been called so shut everything down
    //
    db.Close();

    delete sender;
    sender = NULL;

	return 0;
}


//
// isValidCommandLine
//
bool isValidCommandLine(TA_Core::RunParams params, const char* progname)
{
    //
	// Parse args for correctness
	//
	if ( (!params.isSet( "EntityKey") ) || (!params.isSet( "LowRange" ) )
        || (!params.isSet( "HighRange") ) || (!params.isSet( "DbConnection") ) )
	{
		std::cout << std::endl
				  << "Usage: " << progname << " with following parameters: " << std::endl
				  << "\t--EntityKey=<pkey of datapoint>" << std::endl
				  << "\t--LowRange=<lower limit of random gen value>" << std::endl
				  << "\t--HighRange=<high limit of random gen value>" << std::endl
                  << "\t--DbConnection=<database connection string>" << std::endl
                  << "\t--NotifyHosts=<hostname>" << std::endl
                  << std::endl
				  << "=== Optional parameters ===" << std::endl
				  << std::endl
				  << "\t--TimeInterval=<seconds between generating value> [Default:" << DEFAULT_TIME_INTERVAL << "]" << std::endl
                  << "\t--Status=<datapoint status> [Default:" << DEFAULT_STATUS << "]" << std::endl
				  << std::endl << std::endl;

		return false;
	}

    return true;
}


//
// setUpCorba
//
void setUpCorba(TA_Core::RunParams params)
{
    //
   	// Set up CORBA
   	//
   	TA_Core::CorbaUtil::getInstance().initialise();
   	TA_Core::CorbaUtil::getInstance().activate();

	TA_Core::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS, params.get( RPARAM_NOTIFYHOSTS ).c_str() );
	TA_Core::DebugUtil::getInstance().setLevel( TA_Core::DebugUtil::DebugDebug );
}


//
// runProcessingLoop
//
void runProcessingLoop
(
    SimpleDb& db,
    CommsMessageSender* sender,
    unsigned long entitykey,
    int lowRange,
    int highRange,
    int timeInterval,
    unsigned long baseSampleKey,
    int status
)
{
    //
    // Create and start Input monitoring loop - this loop
    // waits for the user to terminate the program.
    //
    MonitorInputThread* inputThread = new MonitorInputThread();
    inputThread->start();

    //
    // Main processing loop - this loop will write to the database
    // periodically based on the TimeInterval, and send a
    // HistoryUpdate message each time the database has been
    // updated. This loop will continue running until the user
    // has terminated the program.
    //
    int lastValue = 50;
    do
    {
        //
        // Retrieve the time to write to the database and
        // send in the message.
        //
        time_t timeNow;
        time(&timeNow);

        //
        // Convert time to string. Must be gmtime.
        //
        struct tm* t = localtime(&timeNow);
        char str[22] = {0};
        if (t != NULL)
        {
            //
	        // Just to ensure that seconds will be 0.
            //
	        int sec;
	        if(t->tm_sec != 0)
		        sec = 0;
	        else
		        sec = t->tm_sec;

	        sprintf(str, "%02d/%02d/%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon + 1,
		        t->tm_year + 1900, t->tm_hour, t->tm_min, sec);
        }
        std::string timestamp(str);

        //
        // Generate the random value. So that the value isn't
        // constantly changing, if the number is even, don't
        // change it, if it's odd, then write the new value.
        //
        bool bValue = false;
        srand(time(0));
        int dpValue = rand() % (highRange - lowRange) + lowRange + 1;

        if(RunParams::getInstance().isSet("Boolean"))
        {
            if(dpValue % 2 == 1)
            {
                bValue = true;
            }
            else
            {
                bValue = false;
            }
        }
        else
        {
            char buff[100] = {0};
            if(dpValue % 2 == 1)
            {
                lastValue = dpValue;
                sprintf(buff, "%d", dpValue);
            }
            else
            {
                sprintf(buff, "%d", lastValue);
            }
        }

        //std::string value(buff);

        //sprintf(buff, "%d", entitykey);
        //std::string entitykeyStr(buff);


        //
        // Write new value to database
        // Do the insertion thing
        //
	    //std::string sql("insert into HISTORY_BASE (TIMESTAMP, ENTITYKEY, VALUE, STATUS) VALUES (TO_DATE('");
	    //sql += timestamp + "', 'DD/MM/YYYY HH24:MI:SS'), " + entitykeyStr + ", ";
	    //sql += value + ", 0)";

	    //if(!db.Exec(sql.c_str()))
	    //{
		//    std::cout << std::endl << std::endl
	    //      << "Problem inserting data into database."
	    //      << std::endl;
	    //}

        //
        // Send event to trend viewer
        //
		CORBA::Any stateUpdateData;
        //TA_App::ScadaHistorianProcessTimeUpdateCorbaDef* timeUpdateData = new TA_App::ScadaHistorianProcessTimeUpdateCorbaDef;
        TA_Bus::DataPointCorbaDef::DataPointState* dpState = new TA_Bus::DataPointCorbaDef::DataPointState;
        dpState->timeStamp = timeNow;
        //dpState->timeStamp = 0;
        dpState->dataPointStatus = status;
        if(RunParams::getInstance().isSet("Boolean"))
        {
            dpState->value.booleanValue(bValue);
        }
        else
        {
            dpState->value.floatValue(dpValue);
        }
        stateUpdateData <<= dpState;

		sender->sendCommsMessage( TA_Core::DataPointStateUpdate::DataPointStateUpdate, entitykey, stateUpdateData, 0, 0 );

        if(RunParams::getInstance().isSet("Boolean"))
        {
            std::string value = bValue ? "true" : "false";
            std::cout << "Value generated is: " << value.c_str() << std::endl;
        }
        else
        {
            std::cout << "Value generated is: " << dpValue << std::endl;
        }

		std::cout << std::endl << std::endl
		  << "****************************" << std::endl
          << "Completed sending" << std::endl
		  << "****************************" << std::endl
		  << std::endl;

        //
        // Put the sleep in a for loop so the program can be
        // terminated within a reasonable time of 'quit'
        // being called
        //
        for(int i = 0; i < timeInterval/2; ++i)
        {
            if(inputThread->quitCalled())
            {
                break;
            }

            //
            // Sleep for 2 seconds
            //
            TA_Core::Thread::sleep(2000);
        }

    } while(!inputThread->quitCalled());

    //
    // terminate monitoring loop
    //
    inputThread->terminateAndWait();
    delete inputThread;
    inputThread = NULL;
}