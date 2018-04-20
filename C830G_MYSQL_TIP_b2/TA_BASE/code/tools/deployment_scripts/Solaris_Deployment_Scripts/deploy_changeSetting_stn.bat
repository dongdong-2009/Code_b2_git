@ECHO ON

CALL GLOBAL.BAT
date /T > logs\deploy_changeSetting-%1.log


.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "cd %2%SOURCEFOLDER%; gunzip -c %BINFILES% | tar -xvf -"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chown transactive %2%SOURCEFOLDER%/bin"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "%SCRIPTDIR%/changeSetting.sh %2%SOURCEFOLDER%/bin %INSTALL_STUBBED_MMS% %3 %SCHEMA% /"

echo "Finish deploy_changesetting." >> logs\deploy_changeSetting-%1.log
pause
exit