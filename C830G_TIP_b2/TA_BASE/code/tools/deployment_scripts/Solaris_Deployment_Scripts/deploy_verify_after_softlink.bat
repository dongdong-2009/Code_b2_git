@ECHO OFF

call global.bat
SET LOGS=logs\deploy_verify.log
date /T > %LOGS%

for /F "tokens=1,2 delims= " %%i in (%SERVER%) do @(
    echo "Setup configuration for server %%i" 
    start deploy_verify_after_softlink_stn.bat %%i %%j
    sleep 1
)


echo "Verification finished."


pause
