@ECHO OFF

CALL GLOBAL.BAT
date /T > logs\deploy_changeSetting.log

for /F "tokens=1,2,3 delims= " %%i in (%SERVER%) do @(
    echo "Change setting in server %%i" >> logs\deploy_changeSetting.log
    start deploy_changeSetting_stn.bat %%i %%j %%k
    .\sleep 1
)


echo "Change setting finished." >> logs\deploy_changeSetting.log
date /T >> logs\deploy_changeSetting.log

pause
