#ifndef ATS_SIM_TABLE_REQUEST_PROCESSOR_H
#define ATS_SIM_TABLE_REQUEST_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * ATSSimTableQueryProcessor is a thread which holds a queue of incoming packets. The
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

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimulator;
	class ATSSimTable;
    class ATSSimTableLogSupport;
	class UserQuery;

    class ATSSimTableQueryProcessor : public virtual TA_Base_Core::Thread
    {

    public:

        /**
          * Preferred constructor
          *
          * @param owner The parent
          */
        ATSSimTableQueryProcessor( TA_IRS_App::ATS_Sim::ATSSimulator * owner, LocationType loc );


        /**
          * Destructor
          */
        virtual ~ATSSimTableQueryProcessor();


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
        //                        ATSSimTableQueryProcessor methods                   //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * addModbusQuery
          *
          * Called by ATSSimCommsObserver when a packet arrives on its socket.  It
          * adds the packet to the table query processor's queue.
          *
          * @param query the packet from the ISCS to add to the queue.
          */
        void addModbusQuery( ModbusQuery * query );


        /** addUserQuery
          *
          * This method allows users (via ATSSimulatorCommandObserver) to add
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
        ATSSimTable * getTable(int tAddress);

        /**
          * Called in the ATSSimTable constructor, adds the table to the TableMap
          *
          * @param the table to add to the map
          */
        void addTable(ATSSimTable * t);

        /**
          * Adds the given table pointer to the list of dependents of the
          * numbered table. If the queryed numbered table does not exist,
          * nothing is done.
          *
          * @param t the table that is querying dependency
          * @param tableAddress the number of the table that is going to be
          *        depended upon
          */
        void dependUponTable(ATSSimTable * t, int tableAddress);

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
        void sendPacket( const ByteVector & packet, TA_Base_Core::ISocket * socket);


        ///
        ///
        ATSSimulator * m_parent;

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
        typedef std::map< int, ATSSimTable * > ATSSimTableMap;

        ///
        /// A map of tables, keyed by table "address"
        ATSSimTableMap m_tables;

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
        ATSSimTableLogSupport * m_logSupport;

        ///
        /// The format to output table data with the show, dump and log
        /// commands
        TableOutputFormat m_outputFormat;
    };

    inline LocationType ATSSimTableQueryProcessor::getLocationType() const
    {
        return m_location;
    }

    inline TableOutputFormat ATSSimTableQueryProcessor::getTableOutputFormat() const
    {
        return m_outputFormat;
    }
} // namespace ATS_Sim
}
#endif // ifndef (ATSSim_TABLE_REQUEST_PROCESSOR_H)
