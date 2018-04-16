/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/Generator.cpp,v $
 * @author:  <Cameron Rochester>
 * @version: $Revision: 1.1.2.2 $
 *
 * Last modification: $Date: 2003/09/10 03:42:39 $
 * Last modified by:  $Author: andrewdc $
 *
 * A tester application for passing a large volume of alarms through the alarm processor.
 */

#include "tools/integration/AlarmGenerator/src/Generator.h"
#include "tools/integration/AlarmGenerator/src/AlarmGeneratorDlg.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Timer.h"

#include "core/threads/src/Thread.h"

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/corba/src/CorbaUtil.h"

#include "tools/integration/AlarmGenerator/src/SubmitAlarmThread.h"
#include "tools/integration/AlarmGenerator/src/ReceiveAlarmThread.h"
#include "tools/integration/AlarmGenerator/src/GetAlarmThread.h"

#include "core/alarm/src/AlarmHelperManager.h"

namespace TA_Core
{
    namespace TA_DataAccessInterface
    {
        class IDatabase;
        class IData;
    }
}


using TA_Core::TA_Utilities::DebugUtil;
using TA_Core::TA_Utilities::RunParams;
using TA_Core::TA_DataAccessInterface::IDatabase;
using TA_Core::TA_DataAccessInterface::DatabaseFactory;
using TA_Core::TA_DataAccessInterface::IData;

Generator::Generator(CAlarmGeneratorDlg* dialog)
{
    LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::Generator");
	m_dialog = dialog;
	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::Generator");
    
}

void Generator::outputToDialog(std::string outputString)
{
   LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::outputToDialog");

	m_dialog->setDialogOutput(outputString);

	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::outputToDialog");
    
}

int Generator::setUpEntityType()
{
   LOG ( SourceInfo, DebugUtil::FunctionEntry, "setupEntityType");

    // Get a database connection
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();

    // find out if the entity type is currently in the database
    std::string sql = "select pkey from entitytype where name = 'AutoAlarmsEntityType'";
	std::string outputText = sql + "\r\r\n";
	outputToDialog(outputText);
    // set up the column names vector
    std::vector<std::string> columnNames;
    columnNames.push_back("pkey");

    IData* data = databaseConnection->executeQuery(sql,columnNames);

    if (0 == data->getNumRows())
    { // Need to add the entity type
        sql = "insert into entitytype (PKEY,NAME,GUIAPPLICATIONKEY,ET_PARENTKEY)";
        sql += " values (entitytype_seq.nextval,'AutoAlarmsEntityType',";
        sql += "0,0)";

        databaseConnection->executeModification(sql);

        // Delete the data pointer so we can use it again
        delete data;
        data = NULL;

        // Now select it again to get the pkey
        sql = "select pkey from entitytype where name = 'AlarmAutoAlarmsEntityType'";

        data = databaseConnection->executeQuery(sql,columnNames);

        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;

    }
    else if (1 == data->getNumRows())
    { // Just return the key of the type
        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;
    }
    else
    { // Bad...abort
        std::cerr << "More than one entity type named \"AlarmAutoAlarmsEntityType\" found in "
            << "database." << std::endl;
        abort();
    }
	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::setUpEntityType");
    
}

std::vector<int> Generator::setUpSubmitterEntities(const int count,const int entityTypeKey)
{
   LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::setUpSubmitterEntities");

    // Set up the return vector
    std::vector<int> keyVector;

    // Convert the entity type key to a string
    char entityTypeKeyString[10];
    sprintf(entityTypeKeyString,"%i",entityTypeKey);

    // Get a database connection
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();

    // For each entity required
    for (int i = 0;i < count;i++)
    {
        // See if one exists already
        char iString[10];
        sprintf(iString,"%i",i);
        std::string sql = "select pkey from entity where name = 'AlarmAutoAlarmsEntity"
            + std::string(iString) + "'";

        std::vector<std::string> columnNames;
        columnNames.push_back("pkey");

        // run the query
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        if (0 == data->getNumRows())
        { // create one

            // Want to use the data pointer again, so delete it to prevent memory loss
            delete data;
            data = NULL;

            // Get the pkey
            sql = "select entity_seq.nextval from dual";
            data = databaseConnection->executeQuery(sql,columnNames);
            std::string keyString = data->getStringData(0,"pkey");

            // Add the pkey to the vector
            keyVector.push_back(data->getIntegerData(0,"pkey"));

            // Delete the data pointer now that it isn't needed
            delete data;
            data = NULL;

            // Add the new entity to the database
            sql = "insert into entity (pkey,name,address,description,subsystemkey,locationkey,";
            sql += "typekey,groupkey,parentkey) values (" + keyString + ",";
            sql += "'AlarmAutoAlarmsEntity" + std::string(iString) + "','Unused',";
            sql += "'Entity used in volume testing alarms',0,0,";
            sql += std::string(entityTypeKeyString) + ",0,0)";

            // Execute the statement
            databaseConnection->executeModification(sql);
        }
        else if (1 == data->getNumRows())
        { // Add the number to the vector
            keyVector.push_back(data->getIntegerData(0,"pkey"));
            delete data;
            data = NULL;
        }
        else
        { // bad...abort
            std::cerr << "More than one entity named \"AlarmAutoAlarmsEntity" << i
                << "\" found in database." << std::endl;
            abort();
        }
    }
	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::setUpSubmitterEntities");
    
    return keyVector;
}

int Generator::setUpTestOperator()
{
	LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::setUpTestOperator");	
    // Get a database connection
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();

    // find out if the operator is currently in the database
    std::string sql = "select pkey from operator where name = 'AlarmSubmittingOperator'";

    // set up the column names vector
    std::vector<std::string> columnNames;
    columnNames.push_back("pkey");

    IData* data = databaseConnection->executeQuery(sql,columnNames);

    if (0 == data->getNumRows())
    { // Need to add the operator
        sql = "insert into operator (PKEY,NAME,ACCESSLEVEL,DESCRIPTION)";
        sql += " values (OPERATOR_SEQ.nextval,'AlarmSubmittingOperator',";
        sql += "60,'Unit Test Good Operator')";

        databaseConnection->executeModification(sql);

        // Delete the data pointer so we can use it again
        delete data;
        data = NULL;

        // Now select it again to get the pkey
        sql = "select pkey from operator where name = 'AlarmSubmittingOperator'";

        data = databaseConnection->executeQuery(sql,columnNames);

        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;

    }
    else if (1 == data->getNumRows())
    { // Just return the key of the type
        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;
    }
    else
    { // Bad...abort
        std::cerr << "More than one entity type named \"AlarmAutoAlarmsEntityType\" found in "
            << "database." << std::endl;
        abort();
    }

	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::setUpTestOperator");
    
}

int Generator::setUpTestConsole()
{
	LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::setUpTestConsole");
    // Get a database connection
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();

    // find out if the operator is currently in the database
    std::string sql = "select pkey from entity where name = 'AlarmSubmittingConsole'";

    // set up the column names vector
    std::vector<std::string> columnNames;
    columnNames.push_back("pkey");

    IData* data = databaseConnection->executeQuery(sql,columnNames);

    int consoleKey = 0;

    if (0 == data->getNumRows())
    { // Add a new console

        // Want to use the data pointer again, so delete it to prevent memory loss
        delete data;
        data = NULL;

        // Get the pkey
        sql = "select entity_seq.nextval from dual";
        data = databaseConnection->executeQuery(sql,columnNames);
        std::string keyString = data->getStringData(0,"pkey");

        // Add the pkey to the vector
        consoleKey = data->getIntegerData(0,"pkey");

        // Delete the data pointer now that it isn't needed
        delete data;
        data = NULL;

        // Add the new console to the database
        sql = "insert into entity (pkey,name,address,description,subsystemkey,locationkey,";
        sql += "typekey,groupkey,parentkey) values (" + keyString + ",";
        sql += "'AlarmSubmittingConsole','Unused',";
        sql += "'Unit Test Good Console',0,0,0,0,0)";

        // Execute the statement
        databaseConnection->executeModification(sql);

    }
    else if (1 == data->getNumRows())
    { // Just return the key of the type
        consoleKey = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
    }
    else
    { // Bad...abort
        std::cerr << "More than one entity type named \"AlarmAutoAlarmsEntityType\" found in "
            << "database." << std::endl;
        abort();
    }
	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::setUpTestConsole");
    
    return consoleKey;
}

int Generator::setUpTestSession(int operatorKey,int consoleKey)
{
	LOG ( SourceInfo, DebugUtil::FunctionEntry, "Generator::setUpTestSession");
    // Get a database connection
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();

    // Convert the ints to strings
    char operatorString[10];
    char consoleString[10];
    sprintf(operatorString,"%i",operatorKey);
    sprintf(consoleString,"%i",consoleKey);

    // find out if a session is currently in the database
    std::string sql = "select pkey from pmsession where operatorkey = ";
    sql += std::string(operatorString) + " and consolekey = " + std::string(consoleString);

    // set up the column names vector
    std::vector<std::string> columnNames;
    columnNames.push_back("pkey");

    IData* data = databaseConnection->executeQuery(sql,columnNames);

    if (0 == data->getNumRows())
    { // Need to add the session
        sql = "insert into pmsession (PKEY,OPERATORKEY,CONSOLEKEY)";
        sql += " values (pmsession_seq.nextval," + std::string(operatorString) + ",";
        sql += std::string(consoleString) + ")";

        databaseConnection->executeModification(sql);

        // Delete the data pointer so we can use it again
        delete data;
        data = NULL;

        // Now select it again to get the pkey
        sql = "select pkey from pmsession where operatorkey = " + std::string(operatorString);
        sql += " and consolekey = " + std::string(consoleString);

        data = databaseConnection->executeQuery(sql,columnNames);

        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;

    }
    else
    { // Just return the key of the first session found
        int key = data->getIntegerData(0,"pkey");
        delete data;
        data = NULL;
        return key;
    }
	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::setUpTestSession");
    
}

int Generator::start()
{
	TA_Core::TA_Corba::CorbaUtil::getInstance().initialise();
	TA_Core::TA_Corba::CorbaUtil::getInstance().activate();

    DebugUtil::getInstance().setLevel(DebugUtil::DebugTrace);
    DebugUtil::getInstance().setFile("./AlarmGenerator.log");
    DebugUtil::getInstance().setMaxSize(10000000);


    LOG ( SourceInfo, DebugUtil::FunctionEntry, "start");

    //
    // process arguments.
    //

    int lvSubmitterCount = 10;
    int lvAlarmCount = 100;
    int lvSubmitDelay = 0;
    int lvReceiverCount = 1;
    int lvGetCount = 1;
    bool lvAckAlarms = RunParams::getInstance().isSet("AckAlarms");
    //if( false == RunParams::getInstance().parseCmdLine( argc, argv ) )
    //{
     //   std::cout << "Unable to parse command line - exitting." << std::endl;
      //  usage();
       // return -1;
    //}
    if( true == RunParams::getInstance().isSet("SubmitterCount") )
    {
        std::string submitterCountString = RunParams::getInstance().get("SubmitterCount");
        lvSubmitterCount = atoi(submitterCountString.c_str());
    }
    if ( true == RunParams::getInstance().isSet("AlarmCount") )
    {
        std::string alarmCountString = RunParams::getInstance().get("AlarmCount");
        lvAlarmCount = atoi(alarmCountString.c_str());
    }
    if ( true == RunParams::getInstance().isSet("SubmitDelay") )
    {
        std::string submitDelayString = RunParams::getInstance().get("SubmitDelay");
        lvSubmitDelay = atoi(submitDelayString.c_str());
    }
    if ( true == RunParams::getInstance().isSet("GetCount") )
    {
        std::string getCountString = RunParams::getInstance().get("GetCount");
        lvGetCount = atoi(getCountString.c_str());
    }
    if ( true == RunParams::getInstance().isSet("ReceiverCount") )
    {
        std::string receiverCountString = RunParams::getInstance().get("ReceiverCount");
        lvReceiverCount = atoi(receiverCountString.c_str());
    }
    if ( false == RunParams::getInstance().isSet("DbConnection") )
    {
        outputToDialog("Missing mandatory parameter: db-connetion\r\r\n");
        usage();
        return -1;
    }
    if ( false == RunParams::getInstance().isSet("NotifyHosts") )
    {
        outputToDialog("Missing mandatory parameter: notify-hosts\r\r\n");
        usage();
        return -1;
    }

	char buffer[128];
	std::string output("There will be ");
	itoa(lvSubmitterCount, buffer, 10);
	output += buffer;
	output+= " alarm submitter threads\r\r\n" ;
    outputToDialog(output);

    // Start up CORBA


    //TA_Core::TA_Utilities::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS,
	//	"jupiter");

    /////////////////////////////////////////////////////////////////////////////
    // Database Setup
    /////////////////////////////////////////////////////////////////////////////

    // Add a test entity type if necessary

    int lvTestEntityType = 0;
    try
    {
        lvTestEntityType = setUpEntityType();
    }
    catch (...)
    { // Bad...abort
        std::cerr << "An error occurred while trying to set up the test entity type in the database."
            << std::endl;
        return 3;
    }

    // Set up the entities for which the alarms will be generated
    std::vector<int> submitterKeys = setUpSubmitterEntities(lvSubmitterCount,lvTestEntityType);

    // Ensure that the right number of database entires has been created
    TA_ASSERT(submitterKeys.size() == (unsigned int)lvSubmitterCount,
        "Number of submitter keys does not match the submitter count");

    // Set up the session ID. If the AlarmSubmitting operator does not exist then we can't
    // run, so throw and exception and exit.

    // first, the operator
    int lvOperatorKey = 0;
    try
    {
        lvOperatorKey = setUpTestOperator();
    }
    catch (...)
    { // Bad...abort
        std::cerr << "An error occurred while trying to set up the test operator."
            << std::endl;
        return 3;
    }

    // now, the console
    int lvConsoleKey = 0;
    try
    {
        lvConsoleKey = setUpTestConsole();
    }
    catch (...)
    { // Bad...abort
        std::cerr << "An error occurred while trying to set up the test console."
            << std::endl;
        return 3;
    }

    // finally, the session
    int lvSessionId = 0;
    try
    {
        lvSessionId = setUpTestSession(lvOperatorKey,lvConsoleKey);
    }
    catch (...)
    { // Bad...abort
        std::cerr << "An error occurred while trying to set up the test session."
            << std::endl;
        return 3;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Create Threads
    /////////////////////////////////////////////////////////////////////////////
	outputToDialog("threads being created\r\r\n");
        
    // Local vector to hold ALL threads we create
    std::vector<TA_Core::TA_Thread::Thread*> threadVector;

    // Create an instance of all submit alarm threads and add it to the vector


	output = "Creating ";
	itoa(lvSubmitterCount, buffer, 10);
	output += buffer;
	output+= " alarm submission threads.\r\r\n" ;
    outputToDialog(output);

    LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Create submitter threads" );
    int i;
    for (i = 0; i < lvSubmitterCount; i++)
    {
        threadVector.push_back(new SubmitAlarmThread(submitterKeys[i],lvTestEntityType,lvAlarmCount,lvSubmitDelay));
    }



    /////////////////////////////////////////////////////////////////////////////
    // Start Threads
    /////////////////////////////////////////////////////////////////////////////

    // Iterate through the thread vector to start all the threads.
	output = "Starting ";
	itoa(threadVector.size(), buffer, 10);
	output += buffer;
	output += " threads.\r\r\n" ;
    outputToDialog(output);
    
    // Set up a timer to give us a rough idea of time taken for all threads
    TA_Core::TA_Utilities::Timer timer;
    timer.restart();

    // Now start the other threads - backwards so we start the receiver threads first.
    LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Start all threads" );
    
	std::vector<TA_Core::TA_Thread::Thread*>::iterator it = threadVector.begin();
	
	for(; it != threadVector.end(); it++)
	{
		(*it)->start();
    }

    double timeSoFar = timer.elapsedTime();

    TA_Core::TA_Thread::Thread::sleep(100);

    timer.restart();

    /////////////////////////////////////////////////////////////////////////////
    // Terminate Threads
    /////////////////////////////////////////////////////////////////////////////

    // Iterate through the thread vector to terminate all the threads.
    // The receiver threads will automtically terminate after the submitter threads, as they
    // are "behind" them in the vector.
	output = "Terminating ";
	itoa(threadVector.size(), buffer, 10);
	output += buffer;
	output += " threads.\r\r\n" ;
    outputToDialog(output);


    LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Terminate all threads" );
 	
	it = threadVector.begin();
	for(; it != threadVector.end(); it++)
	{
		(*it)->terminateAndWait();
    }
    double totalTime = timer.elapsedTime() + timeSoFar;


    // Print out the total time
	output = "Time taken to submit and close ";
	itoa((lvAlarmCount*lvSubmitterCount), buffer, 10);
	output += buffer;
	output += " alarms: ";
	gcvt( totalTime, 3, buffer );
	output += buffer;
	output += "s\r\r\n";

    outputToDialog(output);
   
    /////////////////////////////////////////////////////////////////////////////
    // Delete Threads
    /////////////////////////////////////////////////////////////////////////////

    // Wait for the user to hit q

	while(AfxMessageBox( "Alarms subbmitted sucessfully", MB_OK|MB_ICONSTOP)!=IDOK);

    TA_Core::TA_Alarm::AlarmHelperManager::getInstance().cleanUp();

    // Iterate through the thread vector to delete all thread objects.
	output = "Deleting ";
	itoa(threadVector.size(), buffer, 10);
	output += buffer;
	output += " threads.\r\r\n";
    outputToDialog(output);

    
	LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Delete all threads" );
    
			
	while((unsigned int)0 < threadVector.size())
    {
		threadVector.erase(threadVector.end() - 1);
    }

	threadVector.clear();	
    // Delete the "get alarm" threads


	LOG ( SourceInfo, DebugUtil::FunctionExit, "Generator::start");
    return 0;
}


void Generator::usage()
{
    outputToDialog("Usage: \r\r\n");
    outputToDialog("AutoAlarms --db-connection --notify-hosts [--submitter-count] [--alarm-count]\r\r\n");
    outputToDialog("[--submit-delay] [--receiver-count] [--get-count] [--ack-alarms]\r\r\n");
    outputToDialog("Where:\r\r\n");
    outputToDialog("\tdb-connection:\tThe database connection string in TransActive format.\r\r\n");
    outputToDialog("\t\t\te.g. --db-connection=TCMS:ITS:ITS\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\tnotify-hosts:\tThe server on which the notification agent is running.\r\r\n");
    outputToDialog("\t\t\te.g. --notify-hosts=jupiter\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\tsubmitter-count:The number of threads that will be submitting\r\r\n");
    outputToDialog("\t\t\talarms simultaneously.\r\r\n");
    outputToDialog("\t\t\tDefault setting: 10\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\talarm-count:\tThe number of alarms that each thread will submit.\r\r\n");
    outputToDialog("\t\t\tDefault setting: 100\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\tsubmit-delay:\tThe number of mili-seconds between each\r\r\n");
    outputToDialog("\t\t\talarm submission.\r\r\n");
    outputToDialog("\t\t\tDefault setting: 0\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\treceiver-count:\tThe number of threads that will be recieving alarms.\r\r\n");
    outputToDialog("\t\t\tDefault setting: 1\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\tget-count:\tThe number of threads to call getAlarms()\r\r\n");
    outputToDialog("\t\t\tDefault setting: 1\r\r\n");
    outputToDialog("\r\r\n");
    outputToDialog("\tack-alarms:\tIf specified will ack all alarms retrieved by the\r\r\n");
    outputToDialog("\t\t\tgetAlarms() threads.\r\r\n");
    outputToDialog("\t\t\tDefault setting: not enabled\r\r\n");
    outputToDialog("\r\r\n");
}