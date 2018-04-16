The Control Station has some compile-time preprocessor flags that can be set. These are:


LAUNCH_BAR
----------
This will include the ControlStationBar class. When the user logs in the Control Station launch bar will appear down the side of the screen.

Without this flag, once logged in, the Control Station itself has no user interface. Communication must be performed through the COM interface by using GraphWorX or something similar.




CS_LOGOUT
---------
This will display the 'Logout' button in the Logout dialog for the user to logout from the session.  

CS_EXIT
-------
This will display the 'Exit' button in the Logout dialog for the user to terminate control station.

CS_RESTART
----------
This will display the 'Restart' button in the Logout as well as the Login dialog for the user to restart the workstation.

At least one of CS_LOGOUT, CS_EXIT, CS_RESTART must be defined as a compile-time preprocessor.



So for the C830 project the Control Station uses the flags:
CS_LOGOUT, CS_EXIT, CS_RESTART

For the KL project the Control Station uses the flags:
LAUNCH_BAR, CS_EXIT, CS_LOGOUT


-----------

To show an AVI in the login dialogue the file TransActiveIntro.avi must be located in the same directory as the Control Station.