
SUMMARY
-------

This ReadMe.txt contains basic usage instructions for the PASimulator.

QUICKSTART
----------

1.
Start the PASimulator from a command line.  It gives a generic simulator
commands and warns "There are no modules loaded".  At the prompt, enter:

    addmod <name> PA <hostname> <port>
    start

Substituting where appropriate:
<name> an arbitrary name you give to the PA module
<hostname> <port> is the ip_address:port that the Simulator will bind to.

NOTE: 502 is the specified port for the PAAgent to connect with.

For example:
	addmod test PA OCC 192.168.253.168 502
	addmod test PA STATION 192.168.253.168 502


2.
Check specified registers. Enter:

	show register <start address> <end address>

3.
Set specified registers. Enter:
	
	set register <start address> <end address> <value> ...

NOTE: <start address>, <end address> and <value> can be hex or decimal number.

4.
set LiveBroadcast reponse time. Enter:

	set LRT <interval in second>