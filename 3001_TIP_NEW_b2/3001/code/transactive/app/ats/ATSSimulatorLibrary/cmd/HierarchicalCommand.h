#ifndef IHIERARCHICAL_COMMAND_H
#define IHIERARCHICAL_COMMAND_H


/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  */
#include "app/ats/ATSSimulatorLibrary/cmd/CommandOwner.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include <map>

namespace TA_IRS_App
{
namespace ATS_Sim
{
    ///
	class UserQuery;

    /**
      * HierarchicalCommand allows for implementing commands hierarchically.
      * That is, a hierarchical command may or may not aggregate other
      * HierarchicalCommands which may be one of either derived
      * subclass CommandContainer or SubCommand.
      *
      * There is no default constructor, as this should only be instantiated by
      * the owning simulator (or CommandContainer), which itself passes the
      * parent pointer through.
      */
    class HierarchicalCommand
    {
    public:

        /**
          * Default constructor
          */
        HierarchicalCommand(CommandOwner * owner, const char * name);

        /**
          * destructor
          */
        virtual ~HierarchicalCommand( );


        /** Indicates whether or not this HierarchicalCommand is the command
          * with the specified name.
          *
          * @todo TODO make const
          * @param name the std::string name of the command to indicate
          * @return bool true if this is the command with that name, false otherwise.
          */
        virtual bool isCommand(const std::string & name) = 0;


        /** Identifies if this command has a further subcommand of the given name
          *
          * @todo TODO make const
          * @param name the std::string name of the sub command to search for
          * @return true if the sub-command exists, false otherwise.
          */
        virtual bool hasSubCommand(const std::string & name) = 0;


        /**
          * The primary function for which this class exists. Causes this
          * instance to act on the arguments to produce a relevant UserQuery
          *
          * @param args the list of pre-tokenised arguments
          * @param argstr the original un-tokenised argument string
          * @param query the UserQuery to populate
          * @param location the LocationType of the current module
          * @param err the stream to which to print errors
          * @return bool true if the query was successfully created, false otherwise
          */
        virtual bool makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err) = 0;

        /**
          * usageString
          * 
          * returns the usage string to be displayed if the command is called with incorrect parameters
          *
          * @return std::string usageString The usage string
          *
          */
        virtual void usage(const std::string & name, std::ostream & os) const = 0;

        /** Alternative to using isCommand(), which is provided mostly for sake
          * of compatibility with the generic simulator CommandProcessor. This
          * method is provided so that this instance of IHP can be added to its
          * CommandOwner's container.
          *
          * @return the name of this command
          */
        virtual const std::string & getName() const;

    private:
        ///
        ///
        std::string m_name;

        ///
        ///
        CommandOwner * m_owner;
    };


    inline HierarchicalCommand::HierarchicalCommand(CommandOwner * owner, const char * name)
        : m_name(name)
        , m_owner(owner)
    {
        owner->addCommand(this);
    }

    inline HierarchicalCommand::~HierarchicalCommand()
    {
    }

    inline const std::string & HierarchicalCommand::getName() const
    {
        return m_name;
    }
} // namespace ATS_Sim
}//close namespace TA_IRS_App
#endif // IHIERARCHICAL_COMMAND_H
