#ifndef PA_SIM_TABLE_REQUEST_PROCESSOR_H
#define PA_SIM_TABLE_REQUEST_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * PASimTableQueryProcessor is a thread which holds a queue of incoming packets. The
  * main method of the class loops, checking the queue for packets to be processed.
  */
#if defined( WIN32 )
// #pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // !WIN32

#include <queue>
#include <map>
#include <set>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    class PASimulator;
	class PASimTable;
    class PASimTableLogSupport;
	class UserQuery;
    class PASimStandardTcpModbusTablesManager;

    class PASimTableQueryProcessor : public virtual TA_Base_Core::Thread
    {

    public:

        /**
          * Preferred constructor
          *
          * @param owner The parent
          */
        PASimTableQueryProcessor( TA_IRS_App::PA_Sim::PASimulator * owner, LocationType loc );


        /**
          * Destructor
          */
        virtual ~PASimTableQueryProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       TA_Base_Core::Thread methods                            //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * run
          *
          * The run method must be implemented in any class that inherits from TA_Base_Core::Thread.
            *
          */
        void run();


        /**
          * terminate
          *
          * The terminate method must be implemented in any class that inherits
          * from TA_Base_Core::Thread.
          *
          */
        void terminate();



        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                        PASimTableQueryProcessor methods                   //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * addModbusQuery
          *
          * Called by PASimCommsObserver when a packet arrives on its socket.  It
          * adds the packet to the table query processor's queue.
          *
          * @param query the packet from the ISCS to add to the queue.
          */
        void addModbusQuery( ModbusQuery * query );


        /** addUserQuery
          *
          * This method allows users (via PASimulatorCommandObserver) to add
          * queries to the queue.
          *
          * @param query pointer to the UserQuery that contains the information
          *              to be processed
          */
        void addUserQuery( UserQuery * query );

        /**
          * sendReadResponse
          *
          * Used by concrete table classes to send a response packet to a
          * read query
          */
        void sendReadResponse(ModbusResponse * frame);

        /**
          * sendWriteResponse
          *
          * Used by concrete table classes to send a response packet to a
          * write query
          */
        void sendWriteResponse(ModbusResponse * frame);

        /**
          * retrieves the pointer to the table with the given number.
          *
          * @param the number of the table to attempt to fetch
          * @return the pointer to the Table if one exists, nil otherwise
          */
        PASimTable * getTable(int tAddress);

        /**
          * Called in the PASimTable constructor, adds the table to the TableMap
          *
          * @param the table to add to the map
          */
        void addTable(PASimTable * t);

        /**
          * Adds the given table pointer to the list of dependents of the
          * numbered table. If the queryed numbered table does not exist,
          * nothing is done.
          *
          * @param t the table that is querying dependency
          * @param tableAddress the number of the table that is going to be
          *        depended upon
          */
        void dependUponTable(PASimTable * t, int tableAddress);

        /**
          * accessor for the m_location data member
          *
          * @return the value of the m_location data member
          */
        LocationType getLocationType() const;


        /**
          * accessor for the m_outputFormat data member
          *
          * @return the value of the m_outputFormat data member
          */
        TableOutputFormat getTableOutputFormat() const;

        void resetModbusQuery();
    private:

        /** 
          * Clears the contents of the the m_incomingQueries queue
          */
        void processIscsQueries( );

        /** 
          * Clears the contents of the the m_userQueries queue
          */
        void processUserQueries( );

        /**
          * sendPacket
          * 
          * Sends the packet to either the required socket or std::cout.
          *
          * @param packet The packet to be sent.
          *
          */
/*        void sendPacket( const ByteVector & packet, TA_Base_Core::ISocket * socket);*/
        void sendPacket( const ByteVector & packet, ACE_SOCK_Stream * socket);


        ///
        ///
        PASimulator * m_parent;

        ///
        /// The queries that come from the ISCS are stored in a fifo
        typedef std::queue< ModbusQuery * > ModbusQueryQueue;

        ///
        /// A queue of incoming table queries.
        ModbusQueryQueue m_incomingQueries;

        ///
        /// Lock for a ThreadGuard to only allow one thread at a time to
        /// access the iscs/modbus queries.
        TA_Base_Core::NonReEntrantThreadLockable m_modbusQueryLock;

        ///
        /// Whether the tables are for OCC, DEPOT, STATION
        LocationType m_location;

        ///
        ///
        volatile bool m_doThreadTerminate;

        ///
        ///
        volatile bool m_threadIsRunning;

        ///
        /// typedef for a map of tables, keyed by table number
        typedef std::map< int, PASimTable * > PASimTableMap;

        ///
        /// A map of tables, keyed by table "address"
        PASimTableMap m_tables;

        ///
        /// Lock for a ThreadGuard to only allow one thread at a time to
        /// access the tables.
        TA_Base_Core::NonReEntrantThreadLockable m_tableLock;


        /// The queries that come from the simulation user are stored in a fifo
        typedef std::queue< UserQuery * > UserQueryQueue;

        ///
        /// Queue of incoming user commands
        UserQueryQueue m_userQueries;

        ///
        /// Support for table logging.
        PASimTableLogSupport * m_logSupport;

        PASimStandardTcpModbusTablesManager* m_tableManager;

        ///
        /// The format to output table data with the show, dump and log
        /// commands
        TableOutputFormat m_outputFormat;

        
    };

    inline LocationType PASimTableQueryProcessor::getLocationType() const
    {
        return m_location;
    }

    inline TableOutputFormat PASimTableQueryProcessor::getTableOutputFormat() const
    {
        return m_outputFormat;
    }
} // namespace PA_Sim
}
#endif // ifndef (PASim_TABLE_REQUEST_PROCESSOR_H)
