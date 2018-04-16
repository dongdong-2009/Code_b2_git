#ifndef SDH_SIMULATOR_COMMAND_PROCESSOR_H
#define SDH_SIMULATOR_COMMAND_PROCESSOR_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioSimulatorCommandProcessor.h $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */

#include "app/radio/radio_simulator/src/RadioSimulator.h"
#include "bus/generic_simulator/src/ISimulationCommandProcessor.h"


namespace TA_IRS_App
{

    /**
      * RadioSimulatorCommandProcessor implements ISimulationCommandProcessor,
      * for use with the generic simulator.
      *
      * There is no default constructor, as this should only be instantiated by
      * the owning simulator
      */
    class RadioSimulatorCommandProcessor : public TA_Base_Bus::ISimulationCommandProcessor
    {
    public:

        /**
          * Preferred constructor
          *
          * @param parentSimulator the owner of this object
          */
        RadioSimulatorCommandProcessor( TA_IRS_App::RadioSimulator* parentSimulator );

        /**
          * Preferred constructor
          *
          * @param parentSimulator the owner of this object
          */
        virtual ~RadioSimulatorCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                    ISimulationCommandProcessor methods                   //
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
          *
          */
        bool hasCommandName(const std::string &name);


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
          *
          */
        bool execute(const std::string &name, const std::string &params);


        /**
          * errorMsg
          * 
          * returns the error message to be displayed if the command fails to execute successfully
          *
          * @return std::string errorMsg The error message
          *
          */
        std::string errorMsg();


        /**
          * usageString
          * 
          * returns the usage string to be displayed if the command is called with incorrect parameters
          *
          * @return std::string usageString The usage string
          *
          */
        std::string usageString(const std::string &name);



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
          *
          */
        bool hasCommand(const std::string &name);

        /**
          * cmdUsage
          * 
          * returns the usage string of the specified command.  This is only called by usageString()
          *
          * @return std::string usageString The usage string
          *
          */
        std::string cmdUsage(const std::string &name);



        /**
          * cmdUsage
          * 
          * returns the usage string of the specified command.  This is only called by usageString()
          *
          * @return std::string usageString The usage string
          *
          */
        TA_IRS_App::RadioSimulator * m_parentSimulator;

        /**
          * defs
          *
          * An enumerated value to indicate the number of commands supported.
          *
          * @deprecated Replaced by m_commands and m_commandUsage
          */
        enum defs { NUM_COMMANDS = 18};

        /**
          * commands
          *
          * An array of strings representing the commands supported
          *
          * @deprecated Replaced by m_commands
          */
        static std::string commands[NUM_COMMANDS];

        /**
          * commandUsage
          *
          * An array of strings representing the usage of the commands supported
          *
          * @deprecated Replaced by m_commandUsage
          */
        static std::string commandUsage[NUM_COMMANDS];
        
        /** 
          * updated every time a command is executed.
          */
        std::string m_errMsg;


        /**
          * A vector of strings representing the commands supported
          */
        std::vector< std::string > m_commands;

        /**
          * A vector of strings containing the usage the commands supported
          */
        std::vector< std::string > m_commandUsage;
    };

}//close namespace
#endif // SDH_SIMULATOR_COMMAND_PROCESSOR_H
