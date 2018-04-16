REM
REM This batch script setups the Windows build enironment for C830 (4669) project. 
REM It will setup the necessary directories, creates config.ini and build.bat files. 
REM 
REM The CONFIGURATION VARIABLES below should be examined and in some
REM cases changed.
REM
REM Prerequisites
REM      1. Checkout SetupWindowsBuildEnv.bat from perforce 
REM			   and store it in the root drive that you want to have your build
REM      2. Checkout Windows_build.xml in //depot/4669_TIP/TA_BASE/tools/build"
REM                        and rename to "Base_Windows_build.xml (Also store this file in the same directory as SetupWindowsBuildEnv.bat)
REM      3. Checkout Windows_build.xml in //depot/4669_TIP/4669/tools/build"
REM                        and rename to "Project_Windows_build.xml (Also store this file in the same directory as SetupWindowsBuildEnv.bat)
REM      4. Checkout Window_Versioning.pl and VersioningDll.pl  in //depot/4669_TIP/TA_BASE/tools/build
REM                        and store it in the same foler where SetupWindowsBuildEnv.bat is stored. Rename Window_Versioning.pl to Versioning.pl
REM	 6. Create a Client Workspace in Perforce for this build.
REM      7. Checkout the MFT.Installer_UI.ini and Server_Installer_UI.ini 
REM                        and store it in the same folder where SetupWindowsBuildEnv.bat is stored.
REM

REM ##########################################################################
REM #############                                                    #########
REM #############      CONFIGURATION VARIABLES: CHANGE AS NEEDED     #########
REM #############                     (For TA_BASE)                  #########

REM
REM Setup root drive - the drive that you want to setup the build environment.
REM 
REM Example:
REM set RootDrive=F: means the build will be setup in F drive
REM

set RootDrive=F:

REM
REM Setup Home Directory - This will be the directory where the BaseDir and ProjectDir located.
REM 
REM Example:
REM set RootDir=TIP_Build
REM
REM If you dont need, leave it as empty
REM

set RootDir=TIPTest

REM 
REM
REM Setup directory where "build.bat" and "config.ini" are stored.
REM 

set BaseDir=Base_4669

REM
REM Setup directory where build.xml is stored. This directory is the sub-directory of BaseDir
REM 

set BaseCodeDir=Base

REM
REM Define the configuration information needed for "config.ini" file
REM 

REM
REM Define perforce configuration
REM 
REM Example:
REM P4.View.CODE_TA_BASE_4669=//depot/CODE_TA_BASE_DDB - the branch that you want to checkout the base codes
REM P4.View.COTS=//depot/COTS - the COTS branch - (Dont need to change, it will always be this branch)
REM

set P4.View.CODE_TA_BASE_4669=//depot/4669_TIP/TA_BASE
set P4.View.COTS=//depot/COTS

REM
REM Define Version Information 
REM
REM This information is mainly used by Build team during formal release of software to customer.
REM For developer, you dont have change these fields.
REM 
REM NOTE: For developer, you can ONLY use 99 for all field. 
REM 

set Version.ProjectName="Base"
set Version.Mode=99
set Version.Phase=99
set Version.Major=99
set Version.Minor=99

REM 
REM Setup the build configuration to either Debug or Release. To build the codes in either debug mode or release mode
REM 
REM Example:
REM   Build.Configuration=Debug
REM 

set Build.Configuration=Debug
set ACE.Build.Configuration=MFC Debug

REM
REM Define the configuration information needed for "build.bat" file
REM 

REM 
REM Setup the Perforce user, password and client workspace to use
REM 
REM Example
REM p4USER=builder				- the perforce user account name
REM p4CLIENT=grace.koh_grace.koh_TestScripts	- the client workspace name
REM p4PASSWD=empty				- the perforce user password
REM

set p4USER=builder
set p4CLIENT=grace.koh_grace.koh_TestScripts
set p4PASSWD=empty

REM #############                                                    ##########
REM #############           END OF CONFIGURATION VARIABLES           ##########
REM #############                                                    ##########
REM ###########################################################################


REM*********************************** Start: DO NOT Change anything within this block **********************************
REM 
REM Dont need to change these field.
REM 

set ROOT_BASE=ROOT_BASE

REM 
REM This will create the "config.ini" file
REM

REM 
REM Change directory to where the config.ini and build.bat file is stored.
REM 

%RootDrive%
cd\
mkdir %RootDir%\%BaseDir%\%BaseCodeDir%
cd %RootDir%\%BaseDir%

REM 
REM Create config.ini file 
REM 

del config.ini

echo # P4 information>> config.ini
echo P4.View.CODE_TA_BASE_4669=%P4.View.CODE_TA_BASE_4669%>> config.ini
echo P4.View.COTS=%P4.View.COTS%>> config.ini
echo.>> config.ini

echo # Versioning Information.>> config.ini
echo Version.ProjectName=%Version.ProjectName%>> config.ini
echo Version.Mode=%Version.Mode%>> config.ini
echo Version.Phase=%Version.Phase%>> config.ini
echo Version.Major=%Version.Major%>> config.ini
echo Version.Minor=%Version.Minor%>> config.ini
echo.>> config.ini

echo Version.Iteration=${Version.Mode}${Version.Phase}${Version.Major}${Version.Minor}>> config.ini
echo.>> config.ini

echo Version=${Version.Mode}.${Version.Phase}.${Version.Major}.${Version.Minor}>> config.ini
echo.>> config.ini

echo Build.Configuration=%Build.Configuration%>> config.ini
echo ACE.Build.Configuration=%ACE.Build.Configuration%>> config.ini
echo.>> config.ini

echo #Directories>> config.ini
echo Root.Dir=%RootDrive%>> config.ini
echo Base.Dir=%RootDrive%/%RootDir%/%BaseDir%/%BaseCodeDir%>> config.ini
echo Base.Dir.Staging=${Base.Dir}/staging>> config.ini

REM 
REM Create the "build.ini" file
REM 

del build.bat

echo @echo off>> build.bat
echo.>> build.bat

echo call VCVARS32.bat>> build.bat
echo.>> build.bat

echo set ANT_ARGS="-logger" "buildsystem.log.ParallelLogRedirector">> build.bat
echo.>> build.bat

echo set p4USER=%p4USER%>> build.bat
echo set p4CLIENT=%p4CLIENT%>> build.bat
echo set p4PASSWD=%p4PASSWD%>> build.bat
echo.>> build.bat

echo REM Make sure we're on the right drive.>> build.bat
echo %RootDrive%>> build.bat
echo.>> build.bat

echo REM The directories>> build.bat
echo set ROOT_BASE="%RootDrive%\%RootDir%\%BaseDir%\%BaseCodeDir%">> build.bat
echo.>> build.bat

echo cd %%ROOT_BASE%%>> build.bat
echo.>> build.bat

echo REM Checkout the code here.>> build.bat
echo call ant clean_all p4checkoutCode version build stage>> build.bat

REM 
REM Copy and renmame "Base_Windows_build.xml" to "build.xml" to %RootDrive%\%RootDir%\%BaseDir%\%BaseCodeDir%
REM This is the directory that build.xml is stored.
REM 

cd\
del %RootDrive%\%RootDir%\%BaseDir%\%BaseCodeDir%\build.xml
copy Base_Windows_build.xml %RootDrive%\%RootDir%\%BaseDir%\%BaseCodeDir%\build.xml

REM
REM rename Window_Versioning.pl to Versioning.pl
REM

cd\
del Versioning.pl
copy Window_Versioning.pl Versioning.pl

REM ****************************** End: DO NOT Change anything within this block ******************************

REM ##########################################################################
REM #############                                                    #########
REM #############      CONFIGURATION VARIABLES: CHANGE AS NEEDED     #########
REM #############                        (For 4669)                  #########

REM
REM Setup directory where "build.bat" and "config.ini" are stored.
REM

set ProjectDir=C830_Build

REM
REM Setup directory where "build.xml" is stored. This directory is the sub-directory of ProjectDir
REM 

set ProjectCodeDir=4669

REM
REM Define configuration information needed for "config.ini" file
REM 

REM
REM Define perforce configuration
REM 
REM Example:
REM P4.View.CODE_4669 - the branch that you want to checkout the project codes
REM P4.View.CONFIG_4669=//depot/CONFIG_4669/...- the branch that you want to checkout the config
REM 

set P4.View.CODE_4669=//depot/CODE_4669
set P4.View.CONFIG_4669=//depot/CONFIG_4669/...

REM
REM Version Information 
REM
REM This information is mainly used by Build team during formal release of software to customer.
REM For developer, you dont have change these fields.
REM 
REM NOTE: For developer, you can ONLY use 99 for all field. 
REM 

set Version.ProjectName="Project"
set Version.Mode=99
set Version.Phase=99
set Version.Major=99
set Version.Minor=99
set Version.Compile.Mode=T

REM 
REM Define the build configuration to either Debug or Release
REM 
REM Example:
REM   Build.Configuration=Debug
REM 

set Build.Configuration=Debug

REM 
REM Define the debug or release bin folder. 
REM If Build.Configuration is set to Debug, Bin.Folder must be win32_nd
REM If Build.Configuration is set to Release, Bin.Folder must be win32_nr
REM 
set Bin.Folder=win32_nd

REM
REM Define configuraiton information needed for "build.bat" file
REM 

REM 
REM Define the Perforce user, password and client workspace to use
REM 
REM Example
REM p4USER=builder				- the perforce user account name
REM p4CLIENT=grace.koh_grace.koh_TestScripts	- the client workspace name
REM p4PASSWD=empty				- the perforce user password
REM 

set p4USER=builder
set p4CLIENT=grace.koh_grace.koh_TestScripts
set p4PASSWD=empty

REM #############                                                    ##########
REM #############           END OF CONFIGURATION VARIABLES           ##########
REM #############                                                    ##########
REM ###########################################################################

REM 
REM ****************************** Start: DO NOT Change anything within this block ******************************
REM 

REM 
REM Dont need to change these field.
REM 

set ROOT_BASE=ROOT_BASE
set ROOT_4669=ROOT_4669

%RootDrive%

cd\
mkdir %RootDir%\%ProjectDir%\%ProjectCodeDir%
cd %RootDir%\%ProjectDir%

REM 
REM Create "config.ini" file
REM

del config.ini

echo # P4 information>> config.ini
echo P4.View.CODE_4669=%P4.View.CODE_4669%>> config.ini
echo P4.View.COTS=%P4.View.COTS%>> config.ini
echo P4.View.CONFIG_4669=%P4.View.CONFIG_4669% >> config.ini
echo.>> config.ini

echo # Versioning Information.>> config.ini
echo Version.ProjectName=%Version.ProjectName%>> config.ini
echo Version.Mode=%Version.Mode%>> config.ini
echo Version.Phase=%Version.Phase%>> config.ini
echo Version.Major=%Version.Major%>> config.ini
echo Version.Minor=%Version.Minor%>> config.ini
echo Version.Compile.Mode=%Version.Compile.Mode%>> config.ini
echo.>> config.ini

echo Version.Iteration=${Version.Mode}${Version.Phase}${Version.Major}${Version.Minor}>> config.ini
echo.>> config.ini

echo Version=${Version.Mode}.${Version.Phase}.${Version.Major}.${Version.Minor}>> config.ini
echo.>> config.ini

echo Build.Configuration=%Build.Configuration%>> config.ini
echo Bin.Folder=%Bin.Folder%>> config.ini
echo.>> config.ini

echo #Directories>> config.ini
echo Root.Dir=%RootDrive%>> config.ini
echo Base.Dir=%RootDrive%/%RootDir%/%BaseDir%/%BaseCodeDir%>> config.ini
echo 4669.Dir=%RootDrive%/%RootDir%/%ProjectDir%/%ProjectCodeDir%>> config.ini
echo 4669.Dir.Staging=${4669.Dir}/staging/${Version.Iteration}>> config.ini
echo 4669.Dir.Package=${4669.Dir}/package/${Version.Iteration}>> config.ini
echo Base.Dir.Staging=${Base.Dir}/staging>> config.ini

REM 
REM Create "build.bat" file
REM 

del build.bat

echo @echo off>> build.bat
echo.>> build.bat

echo call VCVARS32.bat>> build.bat
echo.>> build.bat

echo set ANT_ARGS="-logger" "buildsystem.log.ParallelLogRedirector">> build.bat
echo.>> build.bat

echo set p4USER=%p4USER%>> build.bat
echo set p4CLIENT=%p4CLIENT%>> build.bat
echo set p4PASSWD=%p4PASSWD%>> build.bat
echo.>> build.bat

echo REM Make sure we're on the right drive.>> build.bat
echo %RootDrive%>> build.bat
echo.>> build.bat

echo REM The directories>> build.bat
echo set ROOT_BASE="%RootDrive%\%RootDir%\%BaseDir%\%BaseCodeDir%">> build.bat
echo set ROOT_4669="%RootDrive%\%RootDir%\%ProjectDir%\%ProjectCodeDir%">> build.bat
echo.>> build.bat

echo cd %%ROOT_4669%%>> build.bat
echo.>> build.bat

echo REM Checkout the code here.>> build.bat
echo call ant clean_all p4checkoutCode p4checkoutConfig copySdk version build>> build.bat

REM 
REM Copy and renmame "Project_Windows_build.xml" to "build.xml" to %RootDrive%\%RootDir%\%ProjectDir%\%ProjectCodeDir%
REM This is the directory that build.xml is stored.
REM 

cd\
del %RootDrive%\%RootDir%\%ProjectDir%\%ProjectCodeDir%\build.xml

copy Project_Windows_build.xml %RootDrive%\%RootDir%\%ProjectDir%\%ProjectCodeDir%\build.xml

REM 
REM ****************************** End: DO NOT Change anything within this block ******************************
REM 

