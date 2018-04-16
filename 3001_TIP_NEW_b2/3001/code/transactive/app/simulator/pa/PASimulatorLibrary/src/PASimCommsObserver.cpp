/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimCommsObserver.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  * CommsObserver allows the CommsHandler to delegate incoming socket connections.
  * Each new connection is passed to a new CommsObserver object, a vector of which
  * is held by the WattsSim singleton object.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <algorithm>

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommsObserver.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h"

#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

//////////////////////////////////////////////////////////////////////////////
//            Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

PASimCommsObserver::PASimCommsObserver
(   ObservedSocketType * socket,
    LocationType loc,
    PASimTableQueryProcessor * q_processor)
: m_clientSocket(socket)
, m_location(loc)
, m_stillConnected(true)
, m_queryProcessor(q_processor)
{

    m_clientSocket->addObserver(* this);
    m_clientSocket->start();
}

PASimCommsObserver::~PASimCommsObserver()
{
    FUNCTION_ENTRY("~PASimCommsObserver");
    if( m_clientSocket != 0 )
    {
        //terminate the thread the socket is holding.
        m_clientSocket->terminateAndWait();
        delete m_clientSocket;
    }

    FUNCTION_EXIT;
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                            ITcpObserver methods                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
/**
  * processReceivedData
  *
  * Parses the received stream of data into zero or more valid table querys
  * and adds them to the message processor's queue.
  *
  * @param pData the tcp data stream
  * @param pSocket the ISocket that received the data
  */

/*void PASimCommsObserver::processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket )*/
void PASimCommsObserver::processReceivedData( std::vector<unsigned  char> & pData, TA_Base_Core::ISocket * pSocket)
{
//     LOG( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
//             TA_Base_Core::DebugUtil::DebugDebug, "Data received" );
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Data receive");

    bool keep_processing = true;
    std::vector<unsigned char>::iterator iter=pData.begin();

    std::vector<unsigned char>::iterator first=pData.end(); // will be set properly by the first valid packet
    std::vector<unsigned char>::iterator last=pData.begin(); // will be set properly by the last valid packet
    int dumped_chars = 0;

    //
    // it'd be nice to use std::find to get the first packet, however there
    // isn't any reliable stl-friendly way to make sure that if we have
    // multiple R and W querys that they'll be handled in the order they
    // arrived. 
    //
    // As it happens, for a W query we need to scorch through the stream
    // decyphering the packet for the table size anyhow, so we'll just sift
    // through it all byte by byte.
    //

    // checking iter against pData.end() isn't effective.
    // while ( ( iter <= pData.end() ) || ( keep_processing ) )
    while ( keep_processing )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "process data entry");
        //
        // updated Alstom Modbus Packet has first (Device Address) byte == 0
        Byte functionCode= iter[7];
        if ( ( functionCode == PASim_FC_Read ) || ( functionCode == PASim_FC_Write ) )
        {
            // adjust the iterator pointing to the first element that we'll
            // eventually erase from.
            if ( first == pData.end() )
            {
                first = iter;
            }

            ModbusQuery * query = 0;

            if ( ( functionCode == PASim_FC_Read ) && ( pData.size() >= 10 ) )
            {
                query = processReceivedRead(iter);
            }
            else if ( functionCode == PASim_FC_Write )
            {
                if ( pData.size() > 13 )
                {
                    query = processReceivedWrite(iter);
                }
                else
                {
                    return;
                    // keep_processing = false;
                    // continue;
                }
            }

            last = iter;
            // send the query to the processor
            //query->setSocket(pSocket);
            m_queryProcessor->addModbusQuery(query);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "bad data");
            keep_processing = false;
        }
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "process data out");
    } // while ( keep_processing )
            
    //
    // For some ungodly unknown reason the "first" iterator eventually (soon)
    // becomes greateer than "last".  Thus the pData.erase call loops till the
    // end of usable memory.
    //
    // Note that this is the same problem as the PASimulator.  Seems to be
    // a vector<> thing.
    // 
    // pData.erase( first, last );
    // 
    pData.clear();
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Data receive out");
}

ModbusQuery * PASimCommsObserver::processReceivedRead(std::vector<unsigned char>::iterator & iter)
{
    ModbusReadQuery * r = new ModbusReadQuery;
    r->fromNetwork(iter);

    return r;
}

ModbusQuery * PASimCommsObserver::processReceivedWrite(std::vector<unsigned char>::iterator & iter)
{
    ModbusWriteQuery * w = new ModbusWriteQuery;
    w->fromNetwork(iter);

    return w;
}


//
//writeFailed
//
void PASimCommsObserver::writeFailed()
{
        //should probably throw some sort of exception.
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Write failed!" );
}


//
//connectionLost
//
bool PASimCommsObserver::connectionLost()
{
    m_stillConnected = false;
    // Do NOT call removeObserver. Its not threadsafe, and besides you don't
    // want to stop observing just because a connection is lost.
    // m_clientSocket->removeObserver ( * this );

    return true;
    //the TcpObservedSocket run() method checks this value to see 
    //if it should try to reconnect.
    //set the socket pointer to null.
}


//
//connectionEstablished
//
void PASimCommsObserver::connectionEstablished()
{
    m_stillConnected = true;
}


std::string PASimCommsObserver::getClientName() const
{
    //
    // Get the name of the client. Hacked version of TcpSocket function of
    // same name, using functions other than
    //
    struct sockaddr_in lvClient;
    int lvClientSize = sizeof( lvClient );
    memset( & lvClient, 0, sizeof( lvClient ) );
    int socketId = m_clientSocket->getSocketId();

    #if !defined( WIN32 )
    const int lvStatus = getpeername( socketId, ( struct sockaddr * )& lvClient,
                                  ( socklen_t * )& lvClientSize );
    #else // defined( WIN32 )
    const int lvStatus = getpeername( socketId, ( struct sockaddr * )& lvClient,
                                  & lvClientSize );
    #endif // defined( WIN32 )

    if ( 0 > lvStatus )
    {
        TA_THROW( TA_Base_Core::TcpSocketException( "Unable to determine peer name" ) );
    }

    //
    // Resolve the network address.
    //
    std::string lvAddress = inet_ntoa( lvClient.sin_addr );
    struct hostent *lvHostPtr = 0;
    int lvAddr = inet_addr( lvAddress.c_str() );
#if defined GETHOSTBYADDR_OK
    #if defined( SOLARIS )
    struct hostent lvHost;
    char lvData[ 256 ];
    int lvError = 0;
    lvHostPtr = gethostbyaddr_r( ( char * )& lvAddr, sizeof( lvAddr ),
                             AF_INET, & lvHost, lvData,
                             sizeof( lvData ), & lvError );
    #else // !defined( SOLARIS )
    lvHostPtr = gethostbyaddr( ( const char * )& lvAddr, sizeof( lvAddr ),
                           AF_INET );
    #endif // !defined( SOLARIS )
#endif // defined GETHOSTBYADDR_OK

    //
    // Now construct the string.
    //
    std::string lvAddressString;

    // If we can't resolve, just use the raw IP... an exception isn't required.
#if defined GETHOSTBYADDR_OK
    if ( lvHostPtr == NULL )
    {
        lvAddressString.append( lvAddress );
    }
    else
    {
        lvAddressString.append( lvHostPtr->h_name );
    }
#else // !defined GETHOSTBYADDR_OK
    lvAddressString.append( lvAddress );
#endif // !defined GETHOSTBYADDR_OK

    lvAddressString.append( " (" );
    lvAddressString.append( lvAddress );
    lvAddressString.append( ")" );
    return lvAddressString;
}

