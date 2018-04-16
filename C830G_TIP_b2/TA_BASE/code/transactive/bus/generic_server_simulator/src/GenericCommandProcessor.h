#ifndef GenericCommandProcessor_H
#define GenericCommandProcessor_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/GenericCommandProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#include "bus/generic_server_simulator/src/GssCommandProcessor.h"


namespace TA_Base_Bus
{
    class CommandMonitor;

    /**
      * GenericCommandProcessor derives from GssCommandProcessor, merely
      * instantiating some basic ICommand classes for general use within the
      * Generic Simulator CommandMonitor
      */
    class GenericCommandProcessor
        : public GssCommandProcessor
    {
    public:

        /** Default constructor
          *
          * Needed to pass the base class a null parent
          */
        GenericCommandProcessor( CommandMonitor * commandMonitor );

    private:
        ///
        ///
        CommandMonitor * m_commandMonitor;
    }; // class GenericCommandProcessor
}// namespace TA_Base_Bus

#endif // GenericCommandProcessor_H
