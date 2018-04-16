@ECHO OFF

call global.bat
SET LOGS=logs\config-%1.log
date /T > %LOGS%


.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/mkdir -p %2%SOURCEFOLDER%"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chown transactive %2%SOURCEFOLDER%"
.\plink.exe -ssh %TRAUSER%@%1 -pw %TRAPWD% "/usr/bin/chmod 755 %2%SOURCEFOLDER%"
call .\SolPutBin.bat %TRAUSER% %TRAPWD% %1 %2%SOURCEFOLDER% %SOURCEDIR%%SOURCEFOLDER%\%BINFILES%

pause
exit