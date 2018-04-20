@ECHO OFF

call global.bat
SET LOGS=logs\deploy_stop_softlink-%1.log
date /T > %LOGS%


.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/watson kill"
sleep 3

pause
exit