@echo off

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\VCVARS32.bat"

set p4USER=builder
set p4CLIENT=Builder_Window_3001_Win7_TIPNEW-Windows
set p4PASSWD=builderbuilder

set ANT_ARGS="-logger" "buildsystem.log.ParallelLogRedirector"

REM Make sure we're on the right drive.

REM The directories
 set ROOT_BASE="C:\955_TIPNEW\Base_3001\Base"
 set ROOT_3001="C:\955_TIPNEW\C955_Build\3001"

cd %ROOT_3001%
call ant clean_all p4checkoutCots p4checkoutTools p4checkoutTransactive p4checkoutConfig copySdk version

REM Copy useful Base cots
xcopy C:\955_TIPNEW\Base_3001\Base\code\cots\ACE C:\955_TIPNEW\C955_Build\3001\code\cots\ACE /e /i /h
xcopy C:\955_TIPNEW\Base_3001\Base\code\cots\boost C:\955_TIPNEW\C955_Build\3001\code\cots\boost /e /i /h
xcopy C:\955_TIPNEW\Base_3001\Base\code\cots\omniORB C:\955_TIPNEW\C955_Build\3001\code\cots\omniORB /e /i /h

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\VCVARS32.bat"

call ant build_Net2008 stageMFTs stageMFTs_new package-MFT package-MFT_bin-lib
