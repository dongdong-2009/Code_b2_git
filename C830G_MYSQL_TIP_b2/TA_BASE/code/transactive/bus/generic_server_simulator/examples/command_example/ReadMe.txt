/**
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/bus/generic_server_simulator/examples/command_example/ReadMe.txt,v $
  * @author:  Robert van Hugten
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2004/08/06 09:14:04 $
  * Last modified by:  $Author: robertvh $
  */

CommandExample is an example of the use of the ICommand hierarchy in the
TA_GenericServerSimulator framework.

Using The Library
-----------------
On Windows, to compile the library into your exe, insert the following dsp
into your dsw and have the main executable project depend on it

code/bus/generic_server_simulator/examples/command_example/CommandExample.dsp

On Solaris/Linux add to the relevant make variables:

* to PROJ_LIBDIRS add
	$(PROJ_ROOT_DIR)/bus/generic_server_simulator/examples/command_example/obj

* to PROJ_INCDIRS add
	$(PROJ_ROOT_DIR)/bus/generic_server_simulator/examples/command_example/src

* to PROJ_LIBS add "CommandExample"

* to DEPENDENT_LIBRARIES add
	$(PROJ_ROOT_DIR)/bus/generic_server_simulator/examples/command_example


Two lines required in your main.cpp are:

#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleModuleCreator.h"
...
    new TA_Bus::CommandExampleModuleCreator("COMMANDEXAMPLE");


When the executable launches, type "addmod cmdex COMMANDEXAMPLE". The server
is now listening on localhost port 2222. If you execute "set socket verbose"
you can see stuff that is arriving on the socket. Telnet to this port with
your favourite telnet client to see what happens.


What It Does
------------
Implements a few commands just for the sake of implementing a few commands:
list directory [pattern]
list file [pattern]
set socket [verbose|quiet]


How Is It Useful
----------------
Provides the least extra code required to implement a server simulator with
the Generic Server Simulator framework:

* implements some ICommands: an IntermediateCommand and some LeafCommands
* implements a concrete AbstractModuleCreator
* implements a concrete GssCommsObserver by overriding processReceivedData()
* implements a concrete GssSimulationModule by overriding createCommsObserver()
* uses a couple of GssUtility methods so you know how good they are for you.

And so that you can see how [relatively] simple it is.
