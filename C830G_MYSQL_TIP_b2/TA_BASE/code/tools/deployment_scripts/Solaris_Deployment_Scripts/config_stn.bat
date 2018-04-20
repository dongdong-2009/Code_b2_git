@ECHO OFF

call global.bat
SET LOGS=logs\config-%1.log
date /T > %LOGS%

.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/mkdir -p %SCRIPTDIR%"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chown transactive %SCRIPTDIR%"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chmod 755 %SCRIPTDIR%"
call .\SolPutAsc.bat %TRAUSER% %TRAPWD% %1 %SCRIPTDIR% getVersion.sh
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chmod 755 %SCRIPTDIR%/getVersion.sh"
call .\SolPutAsc.bat %TRAUSER% %TRAPWD% %1 %SCRIPTDIR% changeSetting.sh
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chmod 755 %SCRIPTDIR%/changeSetting.sh"


echo "Configuration finished." >> %LOGS%
date /T >> %LOGS%
echo "" >> %LOGS%
echo "" >> %LOGS%

pause
exit