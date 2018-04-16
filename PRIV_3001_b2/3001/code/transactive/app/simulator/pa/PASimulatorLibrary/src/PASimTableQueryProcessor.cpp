/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * PASimTableQueryProcessor is a thread which holds a queue of incoming PASim Table
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
#include <sstream>
#include <iomanip>
#include <string>

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommsObserver.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableLogSupport.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardTCPModbusTableManager.h"
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"
#include "core/synchronisation/src/ThreadGuard.h"

//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePADSS.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePAPlatform.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePATrain.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWILD.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWash.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWatchdog.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsAlarm.h" 
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsElectricalSubsection.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsPlatform.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsTrain.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsWash.h"
//#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsWatchdog.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

PASimTableQueryProcessor::PASimTableQueryProcessor
(   PASimulator * parent,
    LocationType loc
)
: m_doThreadTerminate(false)
, m_threadIsRunning(false)
, m_parent(parent)
, m_location(loc)
, m_logSupport(0)
, m_outputFormat(f_default)
, m_tableManager(0)
{
    FUNCTION_ENTRY("PASimTableQueryProcessor::PASimTableQueryProcessor");

//     PASimTable * t = 0;
// 
//     //
//     // Don't need to use the t pointer after construction, as each table adds
//     // itself to "this" map of tables (the PASimTable constructor does that)
//     switch (m_location)
//     {
//         case OCC:
//         {
//             // PA -> ISCS tables
//             t = new PASimTablePADSS(this, m_location);
//             t = new PASimTablePAPlatform(this, m_location);
//             t = new PASimTablePATrain(this, m_location);
//             t = new PASimTablePAWILD(this, m_location);
//             t = new PASimTablePAWatchdog(this, m_location);
//             // ISCS -> PA tables
//             t = new PASimTableIscsAtcData(this, m_location);
//             t = new PASimTableIscsElectricalSubsection(this, m_location);
//             t = new PASimTableIscsTrain(this, m_location);
//             t = new PASimTableIscsWatchdog(this, m_location);
//             break;
//         }
//         case STATION:
//         {
//             // PA -> ISCS tables
//             t = new PASimTablePADSS(this, m_location);
//             PASimTablePAPlatform * platformtable = new PASimTablePAPlatform(this, m_location);
//             t = new PASimTablePATrain(this, m_location);
//             t = new PASimTablePAWatchdog(this, m_location);
//             // MSS ISCS -> PA tables
//             t = new PASimTableIscsAtcData(this, MSS);
//             t = new PASimTableIscsElectricalSubsection(this, MSS);
//             t = new PASimTableIscsPlatform(this, MSS, platformtable);
//             t = new PASimTableIscsTrain(this, MSS);
//             t = new PASimTableIscsWatchdog(this, MSS);
//             // SS1 ISCS -> PA tables
//             t = new PASimTableIscsAtcData(this, SS1);
//             t = new PASimTableIscsElectricalSubsection(this, SS1);
//             t = new PASimTableIscsPlatform(this, SS1, platformtable);
//             t = new PASimTableIscsTrain(this, SS1);
//             t = new PASimTableIscsWatchdog(this, SS1);
//             // SS2 ISCS -> PA tables
//             t = new PASimTableIscsAtcData(this, SS2);
//             t = new PASimTableIscsElectricalSubsection(this, SS2);
//             t = new PASimTableIscsPlatform(this, SS2, platformtable);
//             t = new PASimTableIscsTrain(this, SS2);
//             t = new PASimTableIscsWatchdog(this, SS2);
//             break;
//         }
//         case DEPOT:
//         {
//             // PA -> ISCS tables
//             t = new PASimTablePADSS(this, m_location);
//             t = new PASimTablePATrain(this, m_location);
//             t = new PASimTablePAWash(this, m_location);
//             t = new PASimTablePAWatchdog(this, m_location);
//             // ISCS -> PA tables
//             t = new PASimTableIscsAtcData(this, m_location);
//             t = new PASimTableIscsTrain(this, m_location);
//             t = new PASimTableIscsWash(this, m_location);
//             t = new PASimTableIscsWatchdog(this, m_location);
//             break;
//         }
//         default:
//             TA_ASSERT( false, "Invalid location specified");
//             break;
//     }
// 
// 
//     //
//     // Once all the tables that are going to exist do, "initialise" them.
//     // Can't do this stuff during construction since the method called is an
//     // overridden virtual, which hence ain't necessarily accessible at that time
//     //
//     // NOTE: for the PA, there aren't really any dependent tables yet, however
//     // once the trains start meandering about, there might well be.
//     //
//     for ( PASimTableMap::iterator iter = m_tables.begin() ;
//           iter != m_tables.end() ; iter++ )
//     {
//         ((*iter).second)->setupDependents();
//     }
// 
//     m_logSupport = new PASimTableLogSupport(this);
//     switch (m_location)
//     {
//     case OCC:
//         break;
//     case DEPOT:
//     case STATION:
//         break;
//     default:
//         TA_ASSERT( false, "Invalid location specified");
//         break;
//     }

    m_tableManager = new PASimStandardTcpModbusTablesManager(this, m_location);
    m_tableManager->doInitialize();
    start();

    FUNCTION_EXIT;
}

PASimTableQueryProcessor::~PASimTableQueryProcessor()
{

    FUNCTION_ENTRY("~PASimTableQueryProcessor");
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
    for ( PASimTableMap::iterator iter = m_tables.begin() ;
          iter != m_tables.end() ;
          iter++
        )
    {
        PASimTable * t = (*iter).second;
        delete t;
    }

    //
    // done implicitly during destruction:
    //
    // m_tables.erase(m_tables.begin(), m_tables.end());
    // m_tables.erase();

    delete m_logSupport;
    delete m_tableManager;
    FUNCTION_EXIT;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                       TA_Base_Core::Thread methods                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void PASimTableQueryProcessor::run()
{
    FUNCTION_ENTRY("PASimTableQueryProcessor::run");
    m_threadIsRunning = true;

    while ( ! m_doThreadTerminate )
    {
/*        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "processIscsQueries before");*/
        processIscsQueries( );
        // LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "processIscsQueries after");
        processUserQueries( );
        // LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "processUserQueries after");
        Sleep(100);
    }
    m_threadIsRunning = false;
    std::cout << "Table Query Processor stopping run()" << std::endl;
    FUNCTION_EXIT;

}


void PASimTableQueryProcessor::processIscsQueries( )
{
    TA_Base_Core::ThreadGuard q_guard(m_modbusQueryLock);

    while ( ! m_incomingQueries.empty() )
    {
        ModbusQuery * query = m_incomingQueries.front();

        if ( query->m_functionCode == PASim_FC_Read )
        {
            TA_Base_Core::ThreadGuard rg(m_tableLock);
//             PASimTableMap::iterator found = m_tables.find(query->m_tableAddress);
            //PASimTableMap::iterator found; //TODO
            PASimTable* found = m_tableManager->find(query->m_startAddress);
//            if ( found != m_tables.end() )
            if (found != NULL)
            {
                //((*found).second)->processReadQuery(query);
                found->processReadQuery(query);
//                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
//                     "Processed Read Query for table %d",query->m_tableAddress);//TODO
                try
                {
					m_tableManager->getScenario()->processQuery(&TA_IRS_App::AbstractScenario::reponseReadQuery, query);
                }
                catch(TA_Base_Core::TransactiveException& e) // TODO
                {
                    
                }
                catch(...)
                {
                    
                }
            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                ModbusReadResponse * response = new ModbusReadResponse;
                response->m_functionCode |= 0x80;
//                 response->m_tableAddress = query->m_tableAddress; //TODO
//                 response->m_tableSize = 1; // for the exception code "data"
//                 response->m_exceptionCode = PASim_EC_AddressNotRecognised;
//                 response->m_tableData.resize(0);
                response->setSocket(query->getSocket());
                sendReadResponse(response);
//                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
//                    "Invalid Read Query for table %d: does not exist",query->m_tableAddress);
//                 delete response; //TODO
            }

            delete query;
        }
        else if ( query->m_functionCode == PASim_FC_Write )
        {
            TA_Base_Core::ThreadGuard wg(m_tableLock);
//             PASimTableMap::iterator found = m_tables.find(query->m_tableAddress);
            PASimTable* found = m_tableManager->find(query->m_startAddress);
            //PASimTableMap::iterator found;
            //if ( found != m_tables.end() )
            if( NULL != found)
            {
//                 PASimTable * t = (*found).second;
//                 t->processWriteQuery(query);
                found->processWriteQuery(query);
                // ((*found).second)->processWriteQuery(w);
//                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
//                     "Processed Write Query for table %d",query->m_tableAddress);

                //m_logSupport->doLogTable(t);
                try
                {
					m_tableManager->getScenario()->processQuery(&TA_IRS_App::AbstractScenario::reponseWriteQuery, query);
                }
                catch(TA_Base_Core::TransactiveException& e) // TODO
                {
                    
                }
                catch(...)
                {
                    
                }

            }
            else
            {
                // the table in question doesn't exist, perhaps only 'yet'
                ModbusWriteResponse * response = new ModbusWriteResponse;
                response->m_functionCode |= 0x80;
//                 response->m_tableAddress = query->m_tableAddress;//TODO
//                 response->m_exceptionCode = PASim_EC_AddressNotRecognised;
//                 response->m_tableData.resize(0);
                response->setSocket(query->getSocket());
                sendWriteResponse(response);
//                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
//                     "Invalid Write Query for table %d: does not exist",query->m_tableAddress);//TODO
                delete response;
            }

            delete query;
        }

        // Remove the packet from the queue.
        m_incomingQueries.pop();
    }
}

void PASimTableQueryProcessor::processUserQueries( )
{
    time_t t;
    while ( ! m_userQueries.empty() )
    {
        UserQuery * query = m_userQueries.front();
        time(&t);

        if ( t > query->m_timeStamp)
        {
                if (    ( query->getType() == Show )
                 || ( query->getType() == Dump )
               )
                {
                TA_Base_Core::ThreadGuard rg(m_tableLock);
    /*            PASimTableMap::iterator found = m_tables.find(query->getTableAddress());*/
                PASimTable* found = m_tableManager->find(query->m_startAddress);
                if ( found != NULL )
                {
                    //PASimTable * t = ((*found).second);
                    UserResponse * r = found->processUserRead(query);
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
    //                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
    //                     "Invalid User Read for table %d: does not exist",query->getTableAddress());
    //                 std::cout << "Invalid User Write for table " << query->getTableAddress() << ": does not exist" << std::endl;
                }
            }
			else if ( query->getType() == Inc )
			{
				PASimTable* found = m_tableManager->find(query->m_startAddress);
				if ( found != NULL )
				{
					UserResponse * r = found->processUserIncQuery(query);
					if ( r != 0 )
					{
						delete r;
					}
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Table is not exist" );
				}
			}
            else if ( query->getType() == Set )
            {
                PASimTable* found = m_tableManager->find(query->m_startAddress);
                if ( found != NULL )
                {
                    UserResponse * r = found->processUserWrite(query);
                    if ( r != 0 )
                    {
                        delete r;
                    }
                }
                else
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Table is not exist" );
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
                std::cout << "Current show mode is " << PASimUtility::outputFormat2String(m_outputFormat) << std::endl;
            }

            // Remove the packet from the queue.
            m_userQueries.pop();
            // and delete it
            delete query;
        }
        else
        {
            m_userQueries.push(query);
            m_userQueries.pop();
            break;
            // and delete it
/*            delete query;*/
        }
    }
}


void PASimTableQueryProcessor::terminate()
{
    FUNCTION_ENTRY("PASimTableQueryProcessor::terminate");
    FUNCTION_EXIT;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                        PASimTableQueryProcessor methods                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void PASimTableQueryProcessor::addModbusQuery( ModbusQuery * query )
{
    TA_Base_Core::ThreadGuard qGuard(m_modbusQueryLock);

    // FUNCTION_ENTRY("PASimTableQueryProcessor::addModbusQuery");
    m_incomingQueries.push(query);
//     LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
//         "PASimTableQueryProcessor::addModbusQuery(%d) successful",query->m_tableAddress);//TODO
    // FUNCTION_EXIT;
}


void PASimTableQueryProcessor::addUserQuery( UserQuery * query )
{
    // FUNCTION_ENTRY("PASimTableQueryProcessor::addUserQuery");
    m_userQueries.push( query );
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "PASimTableQueryProcessor::addUserQuery(%d) successful",query->getTableAddress());
    // FUNCTION_EXIT;
}


PASimTable * PASimTableQueryProcessor::getTable(int tAddress)
{
    PASimTable * t = 0;

    PASimTableMap::iterator fnd = m_tables.find(tAddress);
    if (fnd != m_tables.end())
    {
        t = (*fnd).second;
    }

    return t;
}


void PASimTableQueryProcessor::sendReadResponse(ModbusResponse * frame)
{
    ByteVector packet;
    frame->toNetwork(packet);
    sendPacket(packet,frame->getSocket());
}


void PASimTableQueryProcessor::sendWriteResponse(ModbusResponse * frame)
{
    ByteVector packet;
    frame->toNetwork(packet);
    sendPacket(packet,frame->getSocket());
}


void PASimTableQueryProcessor::addTable(PASimTable * t)
{
    int tAddr = t->getTableAddress();
    PASimTableMap::iterator fnd = m_tables.find(tAddr);
    //
    // only add the table if it hasn't been already.
    if (fnd == m_tables.end())
    {
        m_tables.insert(PASimTableMap::value_type(tAddr,t));
        // LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PASimTableQueryProcessor::addTable(address=%d) successful",tAddr);
    }
    else
    {
        PASimTable * x = (*fnd).second;
        //
        // if the new pointer doesn't point to the same object as the first,
        // dump the new one.
        if ( x != t )
        {
            delete t;
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "PASimTableQueryProcessor::addTable(%d) Table already exists",t->getTableAddress());
    }
}


void PASimTableQueryProcessor::dependUponTable(PASimTable * t, int tableAddress)
{
    PASimTableMap::iterator fnd = m_tables.find(tableAddress);

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

// 
// void PASimTableQueryProcessor::sendPacket( const ByteVector & packet, TA_Base_Core::ISocket * socket )
void PASimTableQueryProcessor::sendPacket( const ByteVector & packet, ACE_SOCK_Stream * socket )
{
    FUNCTION_ENTRY("PASimTableQueryProcessor::sendPacket");

    if (0x10 == packet[7])
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send write data");
    }
	
	std::stringstream outputSS;
	outputSS << "Send data:" << std::endl; 
    for (ByteVector::const_iterator it = packet.begin(); it != packet.end(); ++it)
    {
		outputSS << std::hex << std::setw(2) << std::setfill('0')<< *it << " ";
    }
    outputSS << std::endl;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s",outputSS.str());

	if ( *((unsigned long *)socket) == 0xCDCDCDCD )
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Prevented from writing to a [probably deleted] socket");
		std::cerr << "Prevented from writing to a [probably deleted] socket" << std::endl;
		return;
	}

	// sometimes, by the time that we send a response packet to the PAAgent
	// it has closed the connection, and the socket object that received the
	// initial read or write query has been deleted BUT we still have a pointer
	// to it. Since VC++ cleverly sets deleted pointer regions to 0xCD, this
	// correctly invokes an exception.  The best we can do here is just ignore
	// the problem, as it will soon go away.
	try
	{
		// send the packet out on the socket it arrived on.
/*        socket->write( (char*)&(packet[0]), packet.size() );*/
        socket->send_n((char*)&(packet[0]), packet.size());
	}
	catch (...)
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Caught an exception while writing to a [probably deleted] socket");
		std::cerr << "Caught an exception while writing to a [probably deleted] socket" << std::endl;
	}

    FUNCTION_EXIT;
}



void TA_IRS_App::PA_Sim::PASimTableQueryProcessor::resetModbusQuery()
{
    TA_Base_Core::ThreadGuard q_guard(m_modbusQueryLock);
    while (!m_incomingQueries.empty()) // memory leak
    {
        m_incomingQueries.pop();
    }
}
