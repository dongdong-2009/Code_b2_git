#ifndef GenericCommandProcessor_H
#define GenericCommandProcessor_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_server_simulator/src/GenericCommandProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
