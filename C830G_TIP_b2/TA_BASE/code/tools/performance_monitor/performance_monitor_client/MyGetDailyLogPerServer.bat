@ECHO OFF


If "%1"=="" GOTO Syntax
  ECHO "Creating myscript.ftp %1 %2"
  echo option batch on > myscript.ftp
  echo option confirm off >> myscript.ftp
  echo option transfer ascii >> myscript.ftp
  echo open ftp://transactive:%1@%2 >> myscript.ftp
  echo lcd my_daily_logs >> myscript.ftp
  echo cd /tmp/log >> myscript.ftp
  echo get *.csv >> myscript.ftp
  echo close >> myscript.ftp
  echo exit >> myscript.ftp    
  
  .\sleep 3
  
  call .\winscp419.exe /console /script=myscript.ftp

  if errorlevel 1 goto error
    ECHO "File downloaded successfully at %2"
    REM TYPE NUL > myscript.ftp
    DEL script.ftp
  goto End

  error:
    ECHO "File downloaded failed at %2"
    goto END

  REM TYPE NUL > myscript.ftp
  REM DEL script.ftp
  GOTO End

  :Syntax
    Echo Usage: %0 password

:End

