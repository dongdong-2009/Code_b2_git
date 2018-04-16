#ifndef CommandOwner_H
#define CommandOwner_H


/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/CommandOwner.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */

#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include <map>

namespace TA_IRS_App
{
namespace ATS_Sim
{
    ///
    class HierarchicalCommand;

    ///
    typedef std::map< std::string, HierarchicalCommand * > CommandMap;

    /** Base class for objects that wish to own a container of Hierarchihcal
      * Commands.
      *
      * Implemented by HierarchicalCommand and ATSSimulator.
      */
    class CommandOwner
    {
	public:
        /** enables adding a command to the map. Normally called during the
          * HierarchicalCommand constructor
          * @param cmd the Hierarchical command.
          */
        void addCommand(HierarchicalCommand * cmd);

    protected:
        /** Default constructor */
        CommandOwner();

        /** Virtual destructor */
        virtual ~CommandOwner();

        /** Fascade accessor for m_commands.find().second
          * @name the name of the command to find
          * @return pointer to the found command if it exists, nil otherwise
          */
        HierarchicalCommand * find(const std::string & name);

        /** Accessor for the entire map. Shouldn't really be necessary as
          * the first(), last() and find() methods allow for regular iterating.
          */
        const CommandMap & getCommands() const;

    private:
        /** Container of HierarchicalCommand pointers, key'd by command name.
          */
        CommandMap m_commands;
    }; // class CommandOwner

} // namespace ATS_Sim
}//close namespace TA_IRS_App
#endif // CommandOwner_H
