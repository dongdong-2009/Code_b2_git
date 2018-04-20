@ECHO OFF

call global.bat
SET LOGS=logs\deploy_revert-%1.log
date /T > %LOGS%


.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3

.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/sbin/unlink /u01/transactive/bin; /usr/bin/mv /u01/transactive/bin.rev /u01/transactive/bin"

pause
exit