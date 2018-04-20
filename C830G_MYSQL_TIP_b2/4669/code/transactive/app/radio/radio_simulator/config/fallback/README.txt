NOTES on using these fallback mode RadioSimulator files

Preamble
--------

These files are intended to introduce into the RadioSimulator the ability to 
simulate "Radio Fallback Mode".  This is where the OTE Radio System Base
Station (BS) goes into "Fallback".  There are two significant aspects to this
mode:

* the local operator ("dispatcher" in OTE terminology) can only use telephonic
  services among users connected to the same BS. This is transparent to Iscs
  and is only discovered by attempting a call.

* The set of callable APIs is reduced - in particular, no subscriber management
  is allowed.


  
How To Use
----------
Start the radio simulator in either normal or fallback mode, using the
start_normal_xxx.bat or start_fallback_xxx.bat file appropriate for the
location you are simulating.



If you start in normal mode, you should do the following to change to fallback

1. runsim the entry that corresponds to :

   SystemError Event Sessions 0-15 (GWLD_LINK_LOST)

   which should be near the bottom of the showsim list.

2. exec XxxFallbackReload.txt

   Where "Xxx" is the three-letter abbreviation of your location




If you start in fallback mode, you can return to normal mode by ONE of :

1. a) exec XxxNormalReload.txt

   b) Do whatever to let the TrainAgent know that duty has reverted to Occ.
      That should provoke a reconnection. 


2. Terminate the simulator and restart with a start_normal_xxx.bat
   (Should provoke a relogin)



What The Files Do
-----------------
  
These batch, txt (simulator scripts) and ini files provide very basic fallback
behaviour.  They deviate from the normal-mode files in the following ways:
* all SeachSubscriber api calls return a status=STATUS_KO_SESSIONNOTFOUND, as
  the real system does.
* the Login method returns status=STATUS_OK_FALL_BACK
* There is an entry which provides for the unsolicited system error with status
  STATUS_KO_GWLD_LINK_LOST, which is sent to sessions 0-15


