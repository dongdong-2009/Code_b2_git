#ifndef IntermediateCommand_H
#define IntermediateCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/IntermediateCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for classes used by PASimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/HierarchicalCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{

    /** Intermediate node of the command hierarchy
      */
    class IntermediateCommand
        : public HierarchicalCommand
        , public CommandOwner
    {

    public:
        /** constructor */
        IntermediateCommand(CommandOwner * owner, const char * name);

        /** destructor */
        ~IntermediateCommand( );

        //////////////////////////////////////////////////////////////////
        //                                                              //
        //                HierarchicalCommand methods                   //
        //                                                              //
        //////////////////////////////////////////////////////////////////

        bool isCommand(const std::string & name);

        bool hasSubCommand(const std::string & name);

        bool makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err);

        void usage(const std::string & name, std::ostream & os) const;
    }; // class IntermediateCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef IntermediateCommand_H
