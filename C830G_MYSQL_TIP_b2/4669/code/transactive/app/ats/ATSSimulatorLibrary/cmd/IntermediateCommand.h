#ifndef IntermediateCommand_H
#define IntermediateCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/cmd/IntermediateCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for classes used by ATSSimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
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

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef IntermediateCommand_H
