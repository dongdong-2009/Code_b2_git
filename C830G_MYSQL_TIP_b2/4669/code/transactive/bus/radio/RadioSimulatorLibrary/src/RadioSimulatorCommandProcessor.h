/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioSimulatorLibrary/src/RadioSimulatorCommandProcessor.h $
 * @author:  Ross Mckillop
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifndef RadioSimulatorCommandProcessor_H
#define RadioSimulatorCommandProcessor_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_simulator/src/ISimulationCommandProcessor.h"

namespace TA_IRS_Bus
{
    class RadioSimulator;

    class RadioSimulatorCommandProcessor : public TA_Base_Bus::ISimulationCommandProcessor
    {

    public:

            /**
             * Constructor
             */
            RadioSimulatorCommandProcessor( RadioSimulator& parentSimulator );
            
            
            /**
             * Destructor
             */
            virtual ~RadioSimulatorCommandProcessor();


            //////////////////////////////////////////////////////////////////////////////
            //                                                                          //
            //                 ISimulationCommandProcessor methods                      //
            //                                                                          //
            //////////////////////////////////////////////////////////////////////////////

            /**
             * hasCommandName
             * 
             * hasCommandName indicates whether or not the CommandProcessor has a command
             * with the name specified in the name parameter.  Returns true if a command
             * does exist with that name, false otherwise.
             *
             * @param std::string &name The name of the command to search for
             *
             * @return bool hasCommand True if the command exists, false otherwise.
             */
            bool hasCommandName( const std::string& name );


            /**
             * execute
             * 
             * performs the processing required to run the command specified in the name parameter.
             * The params string is a space separated list of parameters to the command.
             *
             * @param std::string &name The name of the command to execute
             *
             * @param std::string &params Parameters to the command
             *
             * @return bool successful True if the command executes successfully, false otherwise.
             */
            bool execute( const std::string& name,
                          const std::string& params );


            /**
             * errorMsg
             * 
             * returns the error message to be displayed if the command fails to execute successfully
             *
             * @return std::string errorMsg The error message
             */
            std::string errorMsg();


            /**
             * usageString
             * 
             * returns the usage string to be displayed if the command is called with incorrect parameters
             *
             * @return std::string usageString The usage string
             */
            std::string usageString( const std::string& name );



        private:

            /**
             * hasCommand
             * 
             * hasCommand is called by the hasCommandName method.  This method searches
             * the list of methods specific to this class, not 
             *
             * @param std::string &name The name of the command to search for
             *
             * @return bool hasCommand True if the command exists, false otherwise.
             */
            bool hasCommand( const std::string& name );

            /**
             * cmdUsage
             * 
             * returns the usage string of the specified command.  This is only called by usageString()
             *
             * @return std::string usageString The usage string
             */
            std::string cmdUsage( const std::string& name );


            /**
             * Main radio simulator class
             */
            RadioSimulator& m_parentSimulator;

            
            /**
             * the number of commands defined
             */
            enum defs { NUM_COMMANDS = 10};
            

            /**
             * this is the list of commands
             */
            static std::string commands[NUM_COMMANDS];
            

            /**
             * and the associated list of usage messages.
             */
            static std::string commandUsage[NUM_COMMANDS];


            /**
             * the error message variable is updated every time a command is executed.
             */
            std::string m_errMsg;
    };

} // TA_IRS_Bus

#endif // RadioSimulatorCommandProcessor_H
