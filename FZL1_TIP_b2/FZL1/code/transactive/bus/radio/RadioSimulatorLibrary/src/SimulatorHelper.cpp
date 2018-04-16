/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioSimulatorLibrary/src/SimulatorHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  */

#include "bus/radio/RadioSimulatorLibrary/src/SimulatorHelper.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioCommsObserver.h"

#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

#include <sstream>
#include <iomanip>
#include <ios>

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{

    SimulatorHelper* SimulatorHelper::s_instance = 0;
    TA_Base_Core::NonReEntrantThreadLockable SimulatorHelper::m_singletonLock;

    
    SimulatorHelper& SimulatorHelper::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        ThreadGuard guard( m_singletonLock );
        
        if (s_instance == NULL)
        {
            s_instance = new SimulatorHelper();
        }
        
        FUNCTION_EXIT;
        return *s_instance;
    }

    
    SimulatorHelper::SimulatorHelper()
        : m_verbose( true )
    {
        FUNCTION_ENTRY( "SimulatorHelper" );
        FUNCTION_EXIT;
    }

    
    SimulatorHelper::~SimulatorHelper()
    {
        FUNCTION_ENTRY( "~SimulatorHelper" );
        FUNCTION_EXIT;
    }

    
    SimulatorUseCaseList& SimulatorHelper::getUseCaseListRef()
    {
        FUNCTION_ENTRY( "getUseCaseListRef" );
        FUNCTION_EXIT;
        return m_ucList;
    }

    
    void SimulatorHelper::loadTable( const std::string& filename )
    {
        FUNCTION_ENTRY( "loadTable" );

        SimulatorParser p( filename );
        p.parse();
        
        SimulatorUseCaseList& x = p.getUseCases();
        std::copy( x.begin(), x.end(), std::back_inserter( m_ucList ) );

        FUNCTION_EXIT;
    }

    void SimulatorHelper::clearTable()
    {
        FUNCTION_ENTRY( "clearTable" );

        m_ucList.clear();

        FUNCTION_EXIT;
    }

    void SimulatorHelper::showTable()
    {
        FUNCTION_ENTRY( "showTable" );

        std::ostringstream output;

        output << "UC ID\tUC name" << std::endl;
        
        SimulatorUseCaseList& ucList = SimulatorHelper::getUseCaseListRef();
        
        for ( unsigned int uc = 0; uc < ucList.size(); ++uc )
        {
            output << uc << "\t" << ucList[uc].name << std::endl;
        }

        COUT( output.str() );

        FUNCTION_EXIT;
    }

    void SimulatorHelper::setVerbose( bool verbose )
    {
        FUNCTION_ENTRY( "setVerbose" );
        FUNCTION_EXIT;
        m_verbose = verbose;
    }

    void SimulatorHelper::showEntry( unsigned int entry )
    {
        FUNCTION_ENTRY( "showTable" );

        if (entry >= m_ucList.size())
        {
            CERR( "Error: no entry [" << entry << "]" << std::endl );

            FUNCTION_EXIT;
            return;
        }

        COUT( m_ucList[entry] << std::endl );

        FUNCTION_EXIT;
    }


    SimulatorUseCaseList SimulatorHelper::findUseCases( std::vector<unsigned char>& data )
    {
        FUNCTION_ENTRY( "findUseCases" );

        SimulatorUseCaseList ucMatch;
        SimulatorUseCaseList ucMulti;
        unsigned int countNonContinues = 0;
        bool first = true;

        // overall bounds
        SimulatorUseCaseList::iterator useCaseListStart = SimulatorHelper::getInstance().getUseCaseListRef().begin();
        SimulatorUseCaseList::iterator useCaseListEnd = SimulatorHelper::getInstance().getUseCaseListRef().end();

        // find first match
        SimulatorUseCaseList::iterator firstMatch = std::find_if( useCaseListStart, useCaseListEnd,
                                                                  SimulatorHelper::FindUseCases( SimulatorUseCase::Mask( data ) ) );

        for ( SimulatorUseCaseList::iterator i = firstMatch; // start at first match
              i != useCaseListEnd;                           // check end condition
              i = std::find_if( ++i, useCaseListEnd, SimulatorHelper::FindUseCases( SimulatorUseCase::Mask( data ) ) )   // find the next match
            )
        {
            SimulatorUseCaseList::iterator match = i;
                
            if ( match != useCaseListEnd )
            {
                if ( ( false == (*match).continueMode ) &&
                     ( false == (*match).continueOnlyIfFirst ) )
                {
                    ++countNonContinues;
                }
                else if ( ( true == (*match).continueOnlyIfFirst ) &&
                          ( false == first ) )
                {
                    // skip
                    continue;
                }

                if ( countNonContinues <= 1 )
                {
                    ucMatch.push_back( *match );
                    
                    if ( countNonContinues == 1 )
                    {
                        ucMulti.push_back( *match );
                    }
                }
                else 
                {
                    ucMulti.push_back( *match );
                }
            }

            if (true == first)
            {
                first = false;
            }
        }

        if ( countNonContinues > 1 )
        {
            std::ostringstream output;
            output << "Warning: Multiple non-continuation matches found, check configuration..." << std::endl;
            
            for ( SimulatorUseCaseList::iterator i = ucMulti.begin(); i != ucMulti.end(); ++i )
            {
                output << "\t'" << (*i).name << "'" << std::endl;
            }

            COUT( output.str() );
        }

        FUNCTION_EXIT;
        return ucMatch;
    }


    std::string SimulatorHelper::displayLoggable( const SimulatorUseCase& uc )
    {
        FUNCTION_ENTRY( "displayLoggable" );

        if ( true == uc.ilog.empty() )
        {
            FUNCTION_EXIT;
            return "";
        }

        std::ostringstream loggable;

        loggable << "Displaying selected field values..." << std::endl;

        for ( std::map<std::string, SimulatorUseCase::Var>::const_iterator i = uc.ilog.begin();
              uc.ilog.end() != i; ++i )
        {
            SimulatorUseCase::Mask field;
            
            if ( uc.verboseMode >= 9 )
            {
                loggable << "Composing Ilog[" << (*i).first << "]" << std::endl;
            }
            
            // basic checks first...
            if (uc.data.size() < (*i).second.inpos+(*i).second.length)
            {
                loggable << "Warning: Config for Ilog [" << (*i).first << "] incorrect, not displaying" << std::endl;
                continue;
            }

            std::copy( &uc.data[ (*i).second.inpos ],
                       &uc.data[ (*i).second.inpos + (*i).second.length ],
                       std::back_inserter( field ) );

            loggable << "\t[" << (*i).first << "] = [";

            if ( (*i).second.render == 'h' )
            {
                loggable << field.renderAsHexString();
            } 
            else if ( (*i).second.render == 'a' )
            {
                loggable << field.renderAsPrintableString();
            }
            else if ( (*i).second.render == 'n' )
            {
                // assume little endian
                if ( field.size() == 1 ) 
                {
                    loggable << static_cast< unsigned int >( field[0] );
                }
                else if ( field.size() == 2 ) 
                {
                    loggable << static_cast< unsigned int >(  field[0] +
                                                            ( field[1] << 8 ) );
                }
                else if (field.size() == 4) 
                {
                    loggable << static_cast< unsigned int >(   field[0] +
                                                             ( field[1] << 8 ) +
                                                             ( field[2] << 16 ) +
                                                             ( field[3] << 24 ) );
                }
                else 
                { 
                    loggable << "<incorrectly configured length, cannot display value>";
                }
            }
            else 
            {
                loggable << "<unknown type> " << field.renderAsHexString();
            }

            loggable << "]" << std::endl;
        }

        FUNCTION_EXIT;
        return loggable.str();
    }


    void SimulatorHelper::sendMessage( const SimulatorUseCase& uc,
                                       ISimulatorSender& sender )
    {
        FUNCTION_ENTRY( "displayLoggable" );

        bool clearToSend = true;

        if ( true == uc.resp.empty() )
        {
            COUT( "OK: Drained message [" << uc.name << "], no reply sent" << std::endl );

            FUNCTION_EXIT;
            return;
        }

        for ( unsigned int respNo = 0; respNo < uc.resp.size(); ++respNo )
        {
            if ( uc.delay[respNo] > 0 )
            {
                // wait before sending
                COUT( "Waiting " << uc.delay[respNo] << "ms..." << std::endl );
                TA_Base_Core::Thread::sleep( uc.delay[respNo] );
            }

            std::ostringstream outputStream;

            const unsigned int bufferMax = uc.respSize[respNo];
            char* buffer = new char[bufferMax];
        
            // Just copy the response as an initial starting point, then tweak it later...
            std::fill( buffer, buffer + bufferMax, 0 );
            std::copy( uc.resp[respNo].begin(), uc.resp[respNo].end(), buffer);

            // Let's mangle the buffer with the Ivar's
            for ( std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator i = uc.ivar.begin();
                  i != uc.ivar.end(); ++i ) 
            {
                if (uc.verboseMode >= 9)
                {
                    outputStream << "Mapping Ivar[" << (*i).first << "]" << std::endl;
                }
                
                // basic checks first...
                if ( ( uc.data.size() < (*i).second.inpos+(*i).second.length ) ||
                     ( bufferMax < (*i).second.outpos+(*i).second.length ) )
                {
                    outputStream << "Error: either received data or output buffer too small" << std::endl;
                    clearToSend = false;
                    continue;
                }

                std::copy( &uc.data[ (*i).second.inpos ],
                           &uc.data[ (*i).second.inpos + (*i).second.length ], 
                           buffer + (*i).second.outpos );
            }
            
            // Then the Ovar's
            for ( std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator j = uc.ovar[respNo].begin();
                  j != uc.ovar[respNo].end(); ++j ) 
            {
                if ( (*j).first == '#' ) // do this last, as it depends on all data being present
                {
                    continue;
                }
                if (uc.verboseMode >= 9)
                {
                    outputStream << "Composing Ovar[" << (*j).first << "]" << std::endl;
                }
                
                // basic checks first...
                if ( bufferMax < (*j).second.outpos+(*j).second.length )
                {
                    outputStream << "Error: output buffer too small" << std::endl;
                    clearToSend = false;
                    continue;
                }
    
                outputStream << "Error: Don't know how to generate token [" << (*j).first << "], so ignoring it." << std::endl; 
            }
            
            std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator sumIter = uc.ovar[respNo].find('#');
            
            if ( sumIter != uc.ovar[respNo].end() )
            {
                std::vector<unsigned char> cc;
                std::copy( &buffer[96], &buffer[ 96 + (unsigned char) buffer[96+3] - 2 ], std::back_inserter(cc) );
            
                unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate( cc, cc.size() );

                if ( uc.verboseMode >= 9 )
                {
                    outputStream << "Train Checksum: " << (0xff & calcCRC) << "," << ((0xff00 & calcCRC)>>8) << std::endl;
                }

                buffer[ 96 + buffer[96+3] - 2 ] = (unsigned char)(0xff & calcCRC);
                buffer[ 96 + buffer[96+3] - 1 ] = (unsigned char)((0xff00 & calcCRC)>>8);
            }

            if ( true == clearToSend )
            {
                outputStream << "Sending [" << respNo << "][" << uc.name << "]" << std::endl;
                
                if ( uc.verboseMode >= 1 ) 
                {
                    std::vector<unsigned char> x;
                    std::copy( buffer, buffer + bufferMax, std::back_inserter( x ) );
                    
                    outputStream << "Detailed Response: [" << SimulatorUseCase::Mask( x );
                    
                    if (uc.verboseMode >= 2)
                    {
                        outputStream << uc.data.renderAsPrintableString();
                    }
                    
                    outputStream << "]" << std::endl;
                }

                // To store session IDs
                char* sessionBuff = new char[4];
                memcpy( sessionBuff, &buffer[4], 4 );

                unsigned long sessionRef = (static_cast<unsigned char>(sessionBuff[0])) + 
                                           ((static_cast<unsigned char>(sessionBuff[1])) << 8) + 
                                           ((static_cast<unsigned char>(sessionBuff[2])) << 16) +
                                           ((static_cast<unsigned char>(sessionBuff[3])) << 24);

                std::string nstr = uc.name;

                if ( uc.name.find( "Attach session request" ) != std::string::npos )
                {
                    RadioCommsObserver* observer = dynamic_cast<RadioCommsObserver*>( &sender );
                    
                    TA_ASSERT( observer != NULL, "Sender is NULL." );
                    
                    observer->addSessionReference( sessionRef );
                }
                else if ( uc.name.find( "Detach session request" ) != std::string::npos )
                {
                    RadioCommsObserver* observer = dynamic_cast<RadioCommsObserver*>( &sender );
                    
                    TA_ASSERT( observer != NULL, "Sender is NULL." );
                    
                    observer->deleteSessionReference( sessionRef );
                }
                else if ( uc.name.find( "Radio login request" ) != std::string::npos )
                {
                    RadioCommsObserver* observer = dynamic_cast<RadioCommsObserver*>( &sender );
                    
                    TA_ASSERT(observer != NULL, "Sender is NULL.");
                    
                    observer->updateSessionReference( sessionRef, "Logged In" );
                }

                delete[] sessionBuff;

                COUT( outputStream.str() );
                
                // Send out to session observers
                sender.simulatorSendEvent( buffer, bufferMax );
            }
            else 
            {
                COUT( outputStream.str() );

                CERR( "Error: Could not send message due to errors composing it." << std::endl );
            }

            delete[] buffer;
            buffer = NULL;
        }

        COUT( std::endl );

        FUNCTION_EXIT;
    }

}
