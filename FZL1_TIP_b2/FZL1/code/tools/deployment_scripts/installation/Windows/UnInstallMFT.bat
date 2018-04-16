::UnInstallMFT.bat z: cpptest2 grace dingling
@echo off
title Copy MFT Installers to different machine

:: $1 = drive to mount
:: $2 = remote PC ip address or host name
:: $3 = user name to login to $2
:: $4 = user password
:: UnInstallMFT.bat z: cpptest2 operator op1

:: mount directory
echo net use %1 \\%2\c$ /user:%3 %4
net use %1 \\%2\c$ /user:%3 %4

c:\temp\psexec.exe \\cpptest2 -i -d c:\transActive\uninst.exe cmd /S

:: unmount mft transactive directory
echo   net use %1: /delete
net use %1 /delete