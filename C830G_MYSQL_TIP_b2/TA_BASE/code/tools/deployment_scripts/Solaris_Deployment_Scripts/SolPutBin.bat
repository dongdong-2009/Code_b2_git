@ECHO OFF

set SCRIPT_FILE=SolPutBin-%3-%random%.ftp
SET LOGS=logs\SolPutBin.log

If "%1"=="" GOTO Syntax
  ECHO "Creating %SCRIPT_FILE% %1 %2"
  echo option batch on > %SCRIPT_FILE%
  echo option confirm off >> %SCRIPT_FILE%
  echo option transfer binary >> %SCRIPT_FILE%
  echo open ftp://%1:%2@%3 >> %SCRIPT_FILE%
  echo cd %4 >> %SCRIPT_FILE%
  echo put %5 >> %SCRIPT_FILE%
  echo close >> %SCRIPT_FILE%
  echo exit >> %SCRIPT_FILE%
  
  .\sleep 3
  
  call .\winscp419.exe /console /script=%SCRIPT_FILE%

  if errorlevel 1 goto error
    echo "" >> %LOGS%
    date /t >> %LOGS%
    ECHO "File %5 upload to %1:%2@%3 in %4 was successfully at %2." >> %LOGS%
    REM TYPE NUL > %SCRIPT_FILE%
    DEL %SCRIPT_FILE%
  goto End

  error:
    echo "" >> %LOGS%
    date /t >> %LOGS%
    ECHO "File %5 upload to %1:%2@%3 in %4 failed." >> %LOGS%
    goto END

  TYPE NUL > %SCRIPT_FILE%
  DEL %SCRIPT_FILE%
  GOTO End

  :Syntax
    Echo Usage: %0 password

:End

