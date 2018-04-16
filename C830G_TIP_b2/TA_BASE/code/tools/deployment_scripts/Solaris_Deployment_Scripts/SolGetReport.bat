@ECHO OFF

set SCRIPT_FILE=SolPutAsc-%3-%random%.ftp
SET LOGS=logs\SolGetReport.log

If "%1"=="" GOTO Syntax
  ECHO "Creating %SCRIPT_FILE% %1 %2"
  echo option batch on > %SCRIPT_FILE%
  echo option confirm off >> %SCRIPT_FILE%
  echo option transfer ascii >> %SCRIPT_FILE%
  echo open ftp://%1:%2@%3 >> %SCRIPT_FILE%
  echo cd %4 >> %SCRIPT_FILE%
  echo lcd .\report >> %SCRIPT_FILE%
  echo get Agents-*.csv >> %SCRIPT_FILE%
  echo close >> %SCRIPT_FILE%
  echo exit >> %SCRIPT_FILE%
  
  .\sleep 3
  
  call .\winscp419.exe /console /script=%SCRIPT_FILE%

  if errorlevel 1 goto error
    echo "" >> %LOGS%
    date /t >> %LOGS%
    ECHO "Retrieve file from %1:%2@%3 was successfully at %2."
    REM TYPE NUL > %SCRIPT_FILE%
    DEL %SCRIPT_FILE%
pause
  goto End

  error:
    ECHO "Retrieve file %5 from %1:%2@%3 failed."
    goto END


  TYPE NUL > %SCRIPT_FILE%
  DEL %SCRIPT_FILE%
  GOTO End

  :Syntax
    Echo Usage: %0 password

:End

