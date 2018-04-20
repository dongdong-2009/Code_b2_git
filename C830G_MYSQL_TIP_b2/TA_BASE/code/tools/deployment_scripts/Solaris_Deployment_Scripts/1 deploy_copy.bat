@ECHO OFF

CALL GLOBAL.BAT
date /T > copy.log

for /F "tokens=1,2 delims= " %%i in (%SERVER%) do @(
    echo "Copy transactive files to server to server %%i" >> copy.log
    start deploy_copy_stn.bat %%i %%j
    .\sleep 10
)


echo "File copy finished." >> config.log
date /T >> copy.log

pause
