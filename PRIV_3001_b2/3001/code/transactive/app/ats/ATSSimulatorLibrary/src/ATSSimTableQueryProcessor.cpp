/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * ATSSimTableQueryProcessor is a thread which holds a queue of incoming ATSSim Table
  * Query "packets".  The main method of the class, run(), loops checking the
  * queue for packets to be processed.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <limits.h>

// #include <iostream>
// #include <sstream>
// #include <iomanip>
// #include <mbstring.h>
// #include <functional>

#include <string>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommsObserver.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableLogSupport.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsDSS.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsPlatform.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsTrain.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWILD.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWash.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWatchdog.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsAlarm.h" 
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsElectricalSubsection.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsPlatform.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsTrain.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWash.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWatchdog.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableQueryProcessor::ATSSimTableQueryProcessor
(   ATSSimulator * parent,
    LocationType loc
)
: m_doThreadTerminate(false)
, m_threadIsRunning(false)
, m_parent(parent)
, m_location(loc)
, m_logSupport(0)
, m_outputFormat(f_default)
{
    FUNCTION_ENTRY("ATSSimTableQueryProcessor::ATSSimTableQueryProcessor");

    ATSSimTable * t = 0;

    //
    // Don't need to use the t pointer after construction, as each table adds
    // itself to "this" map of tables (the ATSSimTable constructor does that)
    switch (m_location)
    {
        case OCC:
        {
            // ATS -> ISCS tables
            t = new ATSSimTableAtsDSS(this, m_location);
            t = new ATSSimTableAtsPlatform(this, m_location);
            t = new ATSSimTableAtsTrain(this, m_location);
            t = new ATSSimTableAtsWILD(this, m_location);
            t = new ATSSimTableAtsWatchdog(this, m_location);
            // ISCS -> ATS tables
            t = new ATSSimTableIscsAtcData(this, m_location);
            t = new ATSSimTableIscsElectricalSubsection(this, m_location);
            t = new ATSSimTableIscsTrain(this, m_location);
            t = new ATSSimTableIscsWatchdog(this, m_location);
            break;
        }
        case STATION:
        {
            // ATS -> ISCS tables
            t = new ATSSimTableAtsDSS(this, m_location);
            ATSSimTableAtsPlatform * platformtable = new ATSSimTableAtsPlatform(this, m_location);
            t = new ATSSimTableAtsTrain(this, m_location);
            t = new ATSSimTableAtsWatchdog(this, m_location);
            // MSS ISCS -> ATS tables
            t = new ATSSimTableIscsAtcData(this, MSS);
            t = new ATSSimTableIscsElectricalSubsection(this, MSS);
            t = new ATSSimTableIscsPlatform(this, MSS, platformtable);
            t = new ATSSimTableIscsTrain(this, MSS);
            t = new ATSSimTableIscsWatchdog(this, MSS);
            // SS1 ISCS -> ATS tables
            t = new ATSSimTableIscsAtcData(this, SS1);
            t = new ATSSimTableIscsElectricalSubsection(this, SS1);
            t = new ATSSimTableIscsPlatform(this, SS1, platformtable);
            t = new ATSSimTableIscsTrain(this, SS1);
            t = new ATSSimTableIscsWatchdog(this, SS1);
            // SS2 ISCS -> ATS tables
            t = new ATSSimTableIscsAtcData(this, SS2);
            t = new ATSSimTableIscsElectricalSubsection(this, SS2);
            t = new ATSSimTableIscsPlatform(this, SS2, platformtable);
            t = new ATSSimTableIscsTrain(this, SS2);
            t = new ATSSimTableIscsWatchdog(this, SS2);
            break;
        }
        case DEPOT:
        {
            // ATS -> ISCS tables
            t = new ATSSimTableAtsDSS(this, m_location);
            t = new ATSSimTableAtsTrain(this, m_location);
            t = new ATSSimTableAtsWash(this, m_location);
            t = new ATSSimTableAtsWatchdog(this, m_location);
            // ISCS -> ATS tables
            t = new ATSSimTableIscsAtcData(this, m_location);
            t = new ATSSimTableIscsTrain(this, m_location);
            t = new ATSSimTableIscsWash(this, m_location);
            t = new ATSSimTableIscsWatchdog(this, m_location);
            break;
        }
        default:
            TA_ASSERT( false, "Invalid location specified");
            break;
    }


    //
    // Once all the tables that are going to exist do, "initialise" them.
    // Can't do this stuff during construction since the method called is an
    // overridden virtual, which hence ain't necessarily accessible at that time
    //
    // NOTE: for the ATS, there aren't really any dependent tables yet, however
    // once the trains start meandering about, there might well be.
    //
    for ( ATSSimTableMap::iterator iter = m_tables.begin() ;
          iter != m_tables.end() ; iter++ )
    {
        ((*iter).second)->setupDependents();
    }

    m_logSupport = new ATSSimTableLogSupport(this);

    start();

    FUNCTION_EXIT;
}

ATSSimTableQueryProcessor::~ATSSimTableQueryProcessor()
{

    FUNCTION_ENTRY("~ATSSimTableQueryProcessor");
    m_doThreadTerminate = true;
    while( true == m_threadIsRunning )
    {
        sleep(100);
    }
    terminateAndWait();

    // Sadly, the following doesn't work on queue's
    // m_userQueries.erase(m_userQueries.begin(), m_userQueries.end());
    //
    // but this does...
    while (!m_userQueries.empty())
    {
        UserQuery * u = m_userQueries.front();
        m_userQueries.pop();
        delete u;
    }

    //
    // m_incomingQueries are also on a std::queue
    while (!m_incomingQueries.empty())
    {
        ModbusQuery * m = m_incomingQueries.front();
        m_incomingQueries.pop();
        delete m;
    }

    // 
    // the contents of m_tables are pointers, so we must destroy them
    // explicitly, also
    // 
    //
    for ( ATSSimTableMap::iterator iter = m_tables.begin() ;
          iter != m_tables.end() ;
          iter++
        )
    {
        ATSSimTable * t = (*iter).second;
        delete t;
    }

    //
    // done implicitly during destruction:
    //
    // m_tables.erase(m_tables.begin(), m_tables.end());
    // m_tables.erase();

    delete m_logSupport;

    FUNCTION_EXIT;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                       TA_Base_Core::Thread methods                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void ATSSimTableQueryProcessor::run()
{
    FUNCTION_ENTRY("ATSSimTableQueryProcessor::run");
    m_threadIsRunning = true;

    while ( ! m_doThreadTerminate )
    {
        processIscsQueries( );

        processUserQueries( );

        sleep(100);
    }
    m_threadIsRunning = false;
    std::cout << "Table Query Processor stopping run()" << std::endl;
    FUNCTION_EXIT;

}


void ATSSimTableQueryProcessor::processIscsQueries( )
{
    TA_Base_Core::ThreadGuard q_guard(m_modbusQueryLock);

    while ( ! m_incomingQueries.empty() )
    {
        ModbusQuery * query = m_incomingQueries.front();

        if ( query->m_functionCode == ATSSim_FC_Read )
        {
            TA_Base_Core::ThreadGuard rg(m_tableLock);
            ATSSimTableMap::iterator found = m_tables.find(query->m_tableAddress);
            if ( found != m_tables.end() )
            {
                ((*found).second)->processReadQuery(query);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Processed Read Query for table %d",query->m_tableAddress);
            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                ModbusReadResponse * response = new ModbusReadResponse;
                response->m_functionCode |= 0x80;
                response->m_tableAddress = query->m_tableAddress;
                response->m_tableSize = 1; // for the exception code "data"
                response->m_exceptionCode = ATSSim_EC_AddressNotRecognised;
                response->m_tableData.resize(0);
                response->setSocket(query->getSocket());
                sendReadResponse(response);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                   "Invalid Read Query for table %d: does not exist",query->m_tableAddress);
                delete response;
            }

            delete query;
        }
        else if ( query->m_functionCode == ATSSim_FC_Write )
        {
            TA_Base_Core::ThreadGuard wg(m_tableLock);
            ATSSimTableMap::iterator found = m_tables.find(query->m_tableAddress);
            if ( found != m_tables.end() )
            {
                ATSSimTable * t = (*found).second;
                t->processWriteQuery(query);
                // ((*found).second)->processWriteQuery(w);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Processed Write Query for table %d",query->m_tableAddress);

                m_logSupport->doLogTable(t);

            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                ModbusWriteResponse * response = new ModbusWriteResponse;
                response->m_functionCode |= 0x80;
                response->m_tableAddress = query->m_tableAddress;
                response->m_exceptionCode = ATSSim_EC_AddressNotRecognised;
                response->m_tableData.resize(0);
                response->setSocket(query->getSocket());
                sendWriteResponse(response);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Invalid Write Query for table %d: does not exist",query->m_tableAddress);
                delete response;
            }

            delete query;
        }

        // Remove the packet from the queue.
        m_incomingQueries.pop();
    }
}

void ATSSimTableQueryProcessor::processUserQueries( )
{
    while ( ! m_userQueries.empty() )
    {
        UserQuery * query = m_userQueries.front();

        if (    ( query->getType() == Show )
             || ( query->getType() == Dump )
           )
        {
            TA_Base_Core::ThreadGuard rg(m_tableLock);
            ATSSimTableMap::iterator found = m_tables.find(query->getTableAddress());
            if ( found != m_tables.end() )
            {
                ATSSimTable * t = ((*found).second);
                UserResponse * r = t->processUserRead(query);
                if ( r != 0 )
                {
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                        "Processed User Read for table %d",query->getTableAddress());
                    if ( query->getType() == Show )
                    {
                        std::cout << std::endl << r->m_responseData << std::endl;
                    }
                    else if ( query->getType() == Dump )
                    {
                        std::cerr << "Dump command not yet fully implemented - file not available" << std::endl;
                        // m_dumpFile << r->m_responseData << std::endl;
                    }
                    delete r;
                }
            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Invalid User Read for table %d: does not exist",query->getTableAddress());
                std::cout << "Invalid User Write for table " << query->getTableAddress() << ": does not exist" << std::endl;
            }
        }
        else if ( query->getType() == Set )
        {
            TA_Base_Core::ThreadGuard wg(m_tableLock);
            ATSSimTableMap::iterator found = m_tables.find(query->getTableAddress());
            if ( found != m_tables.end() )
            {
                ATSSimTable * t = (*found).second;
                UserResponse * r = t->processUserWrite(query);
                if ( r != 0 )
                {
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                        "Processed User Write for table %d",query->getTableAddress());
                    std::cout << r->m_responseData << std::endl;
                    delete r;
                }
            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Invalid User Write for table %d: does not exist",query->getTableAddress());
                std::cout << "Invalid User Write for table " << query->getTableAddress() << ": does not exist" << std::endl;
            }
        }
        else if ( ( query->getType() == LogStart )
                || ( query->getType() == LogStop )
                || ( query->getType() == LogMode ) )
        {
            // TA_Base_Core::ThreadGuard wg(m_tableLock);
            //
            // No preliminary checking of the table requested, as the LogModeCommand
            // class has already validated the table address. If it doesn't
            // exist, we want to pass it on, as the LogSupport uses the two
            // invalid addresses.
            UserResponse * r = m_logSupport->processLogQuery( query );
            if ( r != 0 )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Processed User Log Request for table %d",query->getTableAddress());
                std::cout << r->m_responseData << std::endl;
                delete r;
            }
            else
            {

                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "User Log Request resulted in null response");
                std::cout << "(No response to Log request)" << std::endl;
            }
        }
        else if ( query->getType() == Mode )
        {
			if (query->moreArgs())
			{
				m_outputFormat = query->getOutputFormat();
			}
            
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Processed show mode command");
            std::cout << "Current show mode is " << ATSSimUtility::outputFormat2String(m_outputFormat) << std::endl;
        }

        // Remove the packet from the queue.
        m_userQueries.pop();
        // and delete it
        delete query;
    }
}


void ATSSimTableQueryProcessor::terminate()
{
    FUNCTION_ENTRY("ATSSimTableQueryProcessor::terminate");
    FUNCTION_EXIT;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                        ATSSimTableQueryProcessor methods                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void ATSSimTableQueryProcessor::addModbusQuery( ModbusQuery * query )
{
    TA_Base_Core::ThreadGuard qGuard(m_modbusQueryLock);

    // FUNCTION_ENTRY("ATSSimTableQueryProcessor::addModbusQuery");
    m_incomingQueries.push(query);
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "ATSSimTableQueryProcessor::addModbusQuery(%d) successful",query->m_tableAddress);
    // FUNCTION_EXIT;
}


void ATSSimTableQueryProcessor::addUserQuery( UserQuery * query )
{
    // FUNCTION_ENTRY("ATSSimTableQueryProcessor::addUserQuery");
    m_userQueries.push( query );
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "ATSSimTableQueryProcessor::addUserQuery(%d) successful",query->getTableAddress());
    // FUNCTION_EXIT;
}


ATSSimTable * ATSSimTableQueryProcessor::getTable(int tAddress)
{
    ATSSimTable * t = 0;

    ATSSimTableMap::iterator fnd = m_tables.find(tAddress);
    if (fnd != m_tables.end())
    {
        t = (*fnd).second;
    }

    return t;
}


void ATSSimTableQueryProcessor::sendReadResponse(ModbusResponse * frame)
{
    ByteVector packet;
    frame->toNetwork(packet);
    sendPacket(packet,frame->getSocket());
}


void ATSSimTableQueryProcessor::sendWriteResponse(ModbusResponse * frame)
{
    ByteVector packet;
    frame->toNetwork(packet);
    sendPacket(packet,frame->getSocket());
}


void ATSSimTableQueryProcessor::addTable(ATSSimTable * t)
{
    int tAddr = t->getTableAddress();
    ATSSimTableMap::iterator fnd = m_tables.find(tAddr);
    //
    // only add the table if it hasn't been already.
    if (fnd == m_tables.end())
    {
        m_tables.insert(ATSSimTableMap::value_type(tAddr,t));
        // LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ATSSimTableQueryProcessor::addTable(address=%d) successful",tAddr);
    }
    else
    {
        ATSSimTable * x = (*fnd).second;
        //
        // if the new pointer doesn't point to the same object as the first,
        // dump the new one.
        if ( x != t )
        {
            delete t;
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "ATSSimTableQueryProcessor::addTable(%d) Table already exists",t->getTableAddress());
    }
}


void ATSSimTableQueryProcessor::dependUponTable(ATSSimTable * t, int tableAddress)
{
    ATSSimTableMap::iterator fnd = m_tables.find(tableAddress);

    if (fnd != m_tables.end())
    {
        ((*fnd).second)->addDependent(t);
    }

}

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                       Packet Processing & Sending Methods                        //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////


void ATSSimTableQueryProcessor::sendPacket( const ByteVector & packet, TA_Base_Core::ISocket * socket )
{
    FUNCTION_ENTRY("ATSSimTableQueryProcessor::sendPacket");

	if ( *((unsigned long *)socket) == 0xCDCDCDCD )
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Prevented from writing to a [probably deleted] socket");
		std::cerr << "Prevented from writing to a [probably deleted] socket" << std::endl;
		return;
	}

	// sometimes, by the time that we send a response packet to the AtsAgent
	// it has closed the connection, and the socket object that received the
	// initial read or write query has been deleted BUT we still have a pointer
	// to it. Since VC++ cleverly sets deleted pointer regions to 0xCD, this
	// correctly invokes an exception.  The best we can do here is just ignore
	// the problem, as it will soon go away.
	try
	{
		// send the packet out on the socket it arrived on.
		socket->write( (char*)&(packet[0]), packet.size() );
	}
	catch (...)
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Caught an exception while writing to a [probably deleted] socket");
		std::cerr << "Caught an exception while writing to a [probably deleted] socket" << std::endl;
	}

    FUNCTION_EXIT;
}


