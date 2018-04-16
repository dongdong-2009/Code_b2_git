/**
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/bus/generic_server_simulator/examples/echo_server/ReadMe.txt,v $
  * @author:  Robert van Hugten
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2004/08/06 09:15:26 $
  * Last modified by:  $Author: robertvh $
  */

Echo Server is an example of the use of the TA_GenericServerSimulator
framework.

Using The Library
-----------------
To compile the library into your exe, use

code/bus/generic_server_simulator/examples/main/main.dsw

on Windows, or on Solaris/Linux chdir to

code/bus/generic_server_simulator/examples/main

and type "make", then "obj/main"

When the executable launches, type "addmod oh_yeah ECHO <host ip> <port list>"

where <host ip> is the ip address of the machine the exe is running on.
      <port list> is a comma-seperated list of ports on which you want the
                echo server to listen on

The only thing that is required in the main.cpp is including and instantiating 
the EchoModuleCreator.

What It Does
------------
Implements RFC862 Echo Protocol, the text of which is included in this
directory as rfc862.txt. The exception to the protocol being that it does not
listen on port 7 but those you give it.  The echo sever echoes back the data
sent to it. In the case of this echo server, it also prints the string and a
hex dump of the received characters.

Connect to one of the sockets you've set to listen with a telnet client (with
protocol set to "none" preferably, or just use the MS Telnet client) and type
keys. 


How Is It Useful
----------------
Provides the least extra code required to implement a server simulator with
the Generic Server Simulator framework:

* implements a concrete AbstractModuleCreator
* implements a concrete GssCommsObserver by overriding processReceivedData()
* implements a concrete GssSimulationModule by overriding createCommsObserver()
* uses a couple of GssUtility methods so you know how good they are for you.

And so that you can see how [relatively] simple it is.
