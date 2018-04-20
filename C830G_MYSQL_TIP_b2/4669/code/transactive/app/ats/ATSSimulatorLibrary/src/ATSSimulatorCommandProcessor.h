#ifndef ATS_SIM_SIMULATOR_COMMAND_PROCESSOR_H
#define ATS_SIM_SIMULATOR_COMMAND_PROCESSOR_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimulatorCommandProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */

#include "app/ats/ATSSimulatorLibrary/src/ATSSimulator.h"
#include "bus/generic_simulator/src/ISimulationCommandProcessor.h"
#include "app/ats/ATSSimulatorLibrary/cmd/CommandOwner.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/LogCommand.h"


namespace TA_IRS_App
{
namespace ATS_Sim
{

    /**
      * ATSSimulatorCommandProcessor implements ISimulationCommandProcessor,
      * for use with the generic simulator.
      *
      * There is no default constructor, as this should only be instantiated by
      * the owning simulator
      */
    class ATSSimulatorCommandProcessor
		: public TA_Base_Bus::ISimulationCommandProcessor
        , public CommandOwner
    {
    public:

        /**
          * Preferred constructor
          *
          * @param parent the owner of this object
          */
        ATSSimulatorCommandProcessor( TA_IRS_App::ATS_Sim::ATSSimulator* parent, LocationType location );

        /**
          * Preferred constructor
          *
          * @param parent the owner of this object
          */
        virtual ~ATSSimulatorCommandProcessor();


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


        /**
          *
          * @return 
          */
        LocationType getShowLocation() const;


        /**
          *
          * @return 
          */
        LocationType getLocation() const;

        /**
          *
          */
        void setShowLocation(LocationType sLocation);


        private:

        /**
          * hasCommand
          * 
          * hasCommand is called by the hasCommandName method.  This method searches
          * the list of methods specific to this class
          *
          * @param std::string &name The name of the command to search for
          * @return bool hasCommand True if the command exists, false otherwise.
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

        /** executes the show command. Or the dump command. Same effect, different destination
          */
        bool execShow( const std::string& params, Byte qType );

        /** executes the set command
          */
        bool execSet( const std::string& params );

        /** executes the log command
          */
        bool execLog( const std::string& params );

        /** executes the set command
          */
        bool execStatus( const std::string& params );


        /** Upward navigation. Holds the instance of the parent simulator.  */
        ATSSimulator * m_parent;

        ///
        LocationType m_location;

        ///
        LocationType m_showLocation;

        /** 
          * updated every time a command is executed.
          */
        std::string m_errMsg;

        ///
        HierarchicalCommand * m_statusCmd;

        ///
        ShowCommand * m_showCmd;

        ///
        SetCommand * m_setCmd;

        ///
        LogCommand * m_logCmd;

    };

} // namespace ATS_Sim
}// namespace TA_IRS_App
#endif // ATS_SIM_SIMULATOR_COMMAND_PROCESSOR_H
