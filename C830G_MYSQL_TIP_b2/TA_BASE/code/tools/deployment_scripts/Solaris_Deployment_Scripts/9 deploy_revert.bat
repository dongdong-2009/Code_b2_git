@ECHO OFF

CALL GLOBAL.BAT
date /T > logs\deploy_revert.log

for /F "tokens=1,2 delims= " %%i in (%SERVER%) do @(
    start deploy_revert_stn.bat %%i %%j
    .\sleep 5
)

pause
