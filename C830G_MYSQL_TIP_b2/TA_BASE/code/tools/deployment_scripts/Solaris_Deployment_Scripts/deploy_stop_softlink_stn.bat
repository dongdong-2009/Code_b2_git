@ECHO OFF

call global.bat
SET LOGS=logs\deploy_stop_softlink-%1.log
date /T > %LOGS%


.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3

.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/mv /u01/transactive/bin /u01/transactive/bin.rev"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/ln -s %2%SOURCEFOLDER%/bin /u01/transactive/bin"

pause
exit