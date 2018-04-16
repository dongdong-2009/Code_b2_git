#ifndef BackgroundCommsObserver_H
#define BackgroundCommsObserver_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/BackgroundCommsObserver.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssCommsObserver.h"
#include "bus/generic_server_simulator/src/TelnetCommsObserver.h"


namespace TA_Base_Bus
{
    /** Constants 0 and 1 for READ and WRITE ends of a pipe **/
    enum PipeEnds
    {
        ReadEnd     = 0,
        WriteEnd    = 1
    };

    /**
      * BackgroundCommsObserver allows the BackgroundSimulationModule to delegate
      * incoming socket connections.  Each new connection is passed to a new
      * BackgroundCommsObserver object.
      *
      * BackgroundCommsObserver passes all input through to the input end of
      * the stdin replacement file descriptor, and takes stuff from the output
      * end of the stdout and stderr replacements and sends them back down
      * the socket.
      */
    class BackgroundCommsObserver : public TelnetCommsObserver
    {
    public:

        // Constructor.
        BackgroundCommsObserver(GssCommsObserver::ObservedSocketType * socket);

        
        // Virtual destructor
        virtual ~BackgroundCommsObserver();
        
        /**
          *
          */
        virtual void processLine(const TelnetBuffer & line);

    }; // class BackgroundCommsObserver

} // namespace TA_Base_Bus
#endif // ifndef BackgroundCommsObserver_H

