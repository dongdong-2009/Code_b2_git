@echo off
title Copy MFT Installers to different machine

:: $1 = drive to mount
:: $2 = remote PC ip address or host name
:: $3 = user name to login to $2
:: $4 = user password
:: e.g InstallMFT.bat z: cpptest2 operator op1
:: mount directory
echo net use %1 \\%2\c$ /user:%3 %4
net use %1 \\%2\c$ /user:%3 %4

copy c:\temp\mft.ini %1\temp /Y

c:\temp\psexec.exe \\cpptest2 -i -d -c install.exe cmd /S --SetupFile=%1\mft.ini

:: unmount mft transactive directory
net use %1 /delete