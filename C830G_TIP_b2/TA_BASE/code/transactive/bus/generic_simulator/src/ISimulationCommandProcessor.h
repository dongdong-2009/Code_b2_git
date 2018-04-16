/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_simulator/src/ISimulationCommandProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// ISimulationCommandProcessor
//
// For each ISimulationModule descendant there's a corresponding CommandProcessor.
// This can interpret text commands and set/reset appropriate parameters of the
// module - or whatever.
//
// The idea is to decouple the command processing from the module itself as in the
// future it may be desirable to construct new ways to control the module - like
// GUIs.

#ifndef ISIMULATION_COMMAND_PROCESSOR_H
#define ISIMULATION_COMMAND_PROCESSOR_H

#include <string>

namespace TA_Base_Bus
{
	class ISimulationCommandProcessor
	{
		public:
			virtual bool hasCommandName(const std::string &name) = 0;

			virtual bool execute(const std::string &name, const std::string &params) = 0;

			virtual std::string errorMsg() = 0;

			virtual std::string usageString(const std::string &name) = 0;

			virtual ~ISimulationCommandProcessor(){ };
	};
};

#endif
