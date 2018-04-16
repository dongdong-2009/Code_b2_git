
:: %1 is the file path, with text '_I.txt"' or '_T.txt"' at the end.
::eg. %1:"c:\ATSCmd_20121029_142458_T.txt" or "c:\ATSCmd_20121029_142458_I.txt"

echo off
echo start ------------------------------------
echo parameter is %1
set Icmd=nofile.txt
set Tcmd=nofile.txt
set cmdFile=%1

set cmdType=%cmdFile:~-7,6%
echo cmdType is %cmdType%
if _I.txt == %cmdType% (
	set Icmd=%1
)
if _T.txt == %cmdType% (
	set Tcmd=%1
)

set ftpScriptFile="%~dp0ftpScriptTempFile.txt"
echo %ftpScriptFile%
echo user>%ftpScriptFile%
echo transactive>>%ftpScriptFile%
echo tra>>%ftpScriptFile%
if exist %Icmd% (
	echo pwd>>%ftpScriptFile%
	echo cd log>>%ftpScriptFile%
	echo put %Icmd%>>%ftpScriptFile%
	echo by>>%ftpScriptFile%
	call ftp -n -s:%ftpScriptFile% 192.168.123.59
)

echo user>%ftpScriptFile%
echo transactive>>%ftpScriptFile%
echo tra>>%ftpScriptFile%
if exist %Tcmd% (
	echo pwd>>%ftpScriptFile%
	echo cd log>>%ftpScriptFile%
	echo put %Tcmd%>>%ftpScriptFile%
	echo by>>%ftpScriptFile%
	call ftp -n -s:%ftpScriptFile% 192.168.123.59
)
