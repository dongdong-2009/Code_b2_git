@ECHO OFF

call global.bat
SET LOGS=logs\deploy_start_control-%1.log
date /T > %LOGS%

if "%3" == "OCC" (
  .\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/TAStart --deploy-host-names=`hostname` --normal-operation-mode=control --start-or-stop=start --wait-system-controller-startup-in-seconds=20 --debug-level=INFO --debug-file=../log/TAStart.log"
) else (
  .\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/u01/transactive/bin/TAStart --deploy-host-names=`hostname` --normal-operation-mode=control --start-or-stop=start --wait-system-controller-startup-in-seconds=20 --debug-level=INFO --debug-file=../log/TAStart.log"
)

pause
exit
