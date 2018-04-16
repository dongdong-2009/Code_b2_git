
:: %1 is the file path, with text '_I.txt"' or '_T.txt"' at the end.
::eg. %1:"c:\ATSCmd_20121029_142458_T.txt" or "c:\ATSCmd_20121029_142458_I.txt"

echo off
echo parameter is:%1
:: %~dp0 with '\' at the end.
echo "%~dp0FtpSendFile.bat" %1 "%~dp0log.txt"
call "%~dp0FtpSendFile.bat" %1>>"%~dp0log.txt"
