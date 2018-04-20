@ECHO OFF

call global.bat
SET LOGS=logs\config-%1.log
date /T > %LOGS%

.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "%SCRIPTDIR%/getVersion.sh %2%SOURCEFOLDER%/bin %2%SOURCEFOLDER%/%BINFILES%"
call .\SolGetReport.bat %TRAUSER% %TRAPWD% %1 %2%SOURCEFOLDER%/bin


echo "Configuration finished." >> %LOGS%
date /T >> %LOGS%


exit