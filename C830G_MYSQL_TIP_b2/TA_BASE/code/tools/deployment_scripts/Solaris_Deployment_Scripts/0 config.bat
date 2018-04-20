@ECHO OFF

call global.bat
SET LOGS=logs\config.log
date /T > %LOGS%

for /F "tokens=1,2 delims= " %%i in (%SERVER%) do @(
    echo "Setup configuration for server %%i" >> %LOGS%  
    start config_stn.bat %%i
    sleep 1
)


echo "Configuration finished." >> %LOGS%
date /T >> %LOGS%

pause
