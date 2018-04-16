Multi-Deployment User Guide
----------------------------

Solaris:

install_transactive2.sh - must reside on each of the Solaris servers (destination of deployment). Logically, should be in the Transactive home directory. If not, change RemoteInstallSolaris.bat accordingly.

Its usage is provided in XXX document found in the repository as well as within the script itself.

NB. At present, install_transactive2.sh retrieves transactive/transactive.station, watson and versions files from an existing TransActive software on the server via the symbolic link.

e.g.

drwxr-xr-x   5 transactive scada        512 Jun 11 12:53 00.16.00.04
lrwxrwxrwx   1 transactive scada         15 Jun 24 09:51 bin -> 00.16.00.04/bin

A consequence is that if there is no TransActive on the server and/or the symbolic link to 'bin' does not exist, then the 3 above mentioned files will fail to copy. 


RemoteInstallSolaris.bat, DeployAllSolaris.bat and servers.txt - need to reside on the machine from which deployment is performed (e.g. your own PC perhaps). By default, these 3 files need to be in the same directory as each other. If any of them sit in different directories for some reason, changes are required in the two bat files.

servers.txt - lists all IP addresses of servers for deployment.

RemoteInstallSolaris.bat - telnets to one of the servers listed in servers.txt and executes install_transactive2.sh.

DeployAllSolaris.bat - executes RemoteInstallSolaris.bat for every server listed in servers.txt (that is not commented out).



Windows:

Pre-requisites - (1) Telnet server service is available on the MFTs; (2) 'kill' command is available on the MFTs.

InstallTransactive.bat and superKillTransActive.bat - need to reside on each of the Windows MFTs (destination of deployment). By default, they need to be in c:\TransActive.

superKillTransActive.bat - kills all TransActive-related processes and stop TransActive-related services.

NB. Since the services haves been configured to restart if terminated by means other than "stopping" the services properly, killing them MAY NOT completely stop them. Hence, 'net stop' command is used. Also, in order for the services to be successfully removed from the Windows service list, they must be stopped first.


InstallTransactive.bat - calls superKillTransActive.bat and then registers various components of TransActive, as well as adding and starting the TransActive-related services.

DeployAllWindows.bat, RemoteInstallWindows.bat, mfts.txt and CopyTransactive.bat - need to reside on the machine from which deployment is performed. A requirement is that the machine needs to be NTLM authorised, otherwise it will not be able to telnet to the MFTs. Hence, deployment is ideally done from one of the MFTs themselves. By default, these 4 files need to be in the same directory as each other. If any of them sit in different directories for some reason, changes are required in the 3 bat files.


mfts.txt - lists all IP addresses of MFTs for deployment (as well as MFT names for registering and mount drive letters for copying).

CopyTransactive.bat - mounts an MFT using the IP address and corresponding mount drive letter from mfts.txt, and copies the TransActive software to a temporary directory (on the MFT) before moving it to c:\TransActive. TransActive is firstly copied to a temp directory as a safety measure. Copying/Moving to c:\TransActive will fail if somehow any of the files from the existing version is still running/opened.

RemoteInstallWindows.bat - telnets to one of the MFTs listed in mfts.txt and executes InstallTransactive.bat.

DeployAllWindows.bat - spawns processes, each executing CopyTransactive.bat for each MFT. Then, RemoteInstallWindows.bat is called for each MFT listed in mfts.txt.
