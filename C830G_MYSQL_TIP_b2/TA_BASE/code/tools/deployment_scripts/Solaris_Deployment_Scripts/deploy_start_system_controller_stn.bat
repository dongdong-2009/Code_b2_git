@ECHO OFF

call global.bat
SET LOGS=logs\deploy_start_control-%1.log
date /T > %LOGS%

if "%3" == "OCC" (
  .\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/transactive_system_controller start"
) else (
  .\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/transactive_system_controller.station start"
)

pause
exit