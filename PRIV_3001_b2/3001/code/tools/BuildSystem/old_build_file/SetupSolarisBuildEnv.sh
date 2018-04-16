#!/bin/bash

## This warning are acceptable when you perforce the script when the directories are not yet created.
## 
## rm: cannot lstat `.p4setting': No such file or directory
## rm: cannot lstat `config.ini': No such file or directory
## rm: cannot lstat `build.sh': No such file or directory
## mkdir: cannot create directory `T01271300': File exists
## rm: cannot lstat `config.ini': No such file or directory
## mkdir: cannot create directory `/disk3_2/trainteam/temp': File exists
## rm: cannot lstat `build.sh': No such file or directory
##
## This warnings are acceptable when the reported directories already existed in your home directory
## 
## mkdir: cannot create directory `T01271300': File exists
## mkdir: cannot create directory `T01271300/Base_4669': File exists
## mkdir: cannot create directory `T01271300/Base_4669/Base': File exists
## mkdir: cannot create directory `T01271300': File exists
## mkdir: cannot create directory `T01271300/C830_Build': File exists
## mkdir: cannot create directory `T01271300/C830_Build/4669': File exists
## mkdir: cannot create directory `/disk3_2/trainteam/temp': File exists
##

##
## Prerequisites
##      1. Checkout SetupSolarisBuildEnv.sh from perforce 
##			   and store $HOME directory
##      2. Checkout Solaris_build.xml in //depot/<base branch name>/tools/build"
##                        and rename to "Base_Solaris_build.xml (Also store this file in the same directory as SetupSolarisBuildEnv.sh)
##      3. Checkout Solaris_build.xml in //depot/<project branch name>/tools/build"
##                        and rename to "Project_Solaris_build.xml (Also store this file in the same directory as SetupSolarisBuildEnv.sh)
##      4. Checkout Solaris_Versioning.pl  in //depot/4669_TIP/TA_BASE/tools/build
##                        and store it in the same foler where SetupWindowsBuildEnv.bat is stored. Rename it to Versioning.pl
##	5. Create a Client Workspace in Perforce for this build.
##

##########################################################################
#############                                                    #########
#############      CONFIGURATION VARIABLES: CHANGE AS NEEDED     #########
#############                     (For TA_BASE)                  #########

##
## Define Home Directory - This will be the directory where the BaseDir and ProjectDir located.
##

export RootDir=T01271300

##
## Define directory where "build.sh" and "config.ini" are stored for base 
##

export BaseDir=Base_4669
export BaseCodeDir=Base

##
## Define directory where "build.sh" and "config.ini" are stored for project
##

export ProjectDir=C830_Build
export ProjectCodeDir=4669

##
## Define Perforce information for creating p4setting
##
## You need to modify the P4CLIENT, P4USER and P4PASSWD only.
##
## Example
## p4USER=builder				- the perforce user account name
## p4CLIENT=grace.koh_grace.koh_TestScripts	- the client workspace name
## p4PASSWD=empty				- the perforce user password
## P4PORT=192.168.253.15:1666			- the ip address and port number to connect to perforce server
##

export P4PORT=192.168.253.15:1666 #Dont need to change this.
export P4CLIENT=grace.koh_grace.koh_Solaris_TestScripts
export P4USER=builder
export P4PASSWD=empty

#############                                                    ##########
#############           END OF CONFIGURATION VARIABLES           ##########
#############                                                    ##########
###########################################################################

##
## ************************* Start: DO NOT Change anything within this block ************************
##

##
## create .p4setting
##

mkdir $RootDir
mkdir $RootDir/$BaseDir
mkdir $RootDir/$BaseDir/$BaseCodeDir

cd $HOME
cd $RootDir

rm .p4setting

echo export P4PORT=$P4PORT>> .p4setting
echo export P4CLIENT=$P4CLIENT>> .p4setting
echo export P4USER=$P4USER>> .p4setting
echo export P4PASSWD=$P4PASSWD>> .p4setting


## ************************* End: DO NOT Change anything within this block ************************

##########################################################################
#############                                                    #########
#############      CONFIGURATION VARIABLES: CHANGE AS NEEDED     #########
#############                     (For TA_BASE)                  #########

##
## Define configuration information needed for config.ini
##

##
## Define Version Information 
##
## This information is mainly used by Build team during formal release of software to customer.
## For developer, you dont have change these fields.
## 
## NOTE: For developer, you can ONLY use 99 for all field. 
## 

export VersionProjectName="Singapore Circle Line"
export VersionMajor=99.99
export VersionMinor=99.99
export VersionIteration=99999999

##
## Define perforce configuration
## 
## Example:
## P4.View.CODE_TA_BASE=//depot/CODE_TA_BASE_DDB - the branch that you want to checkout the base codes
## P4.View.COTS=//depot/COTS - the COTS branch - (Dont need to change, it will always be this branch)
##

export P4ViewCODE_TA_BASE=//depot/CODE_TA_BASE_4669_T01271300
export P4ViewCOTS=//depot/COTS

## 
## Define the build configuration to either Debug or Release. To build the codes in either debug mode or release mode
## 
## Example:
##   Build.Configuration=Debug
## 

export BuildConfiguration=Debug

#############                                                    ##########
#############           END OF CONFIGURATION VARIABLES           ##########
#############                                                    ##########
###########################################################################

##
## ************************* Start: DO NOT Change anything within this block ************************
##

export BaseDirectory=$HOME/$RootDir
export COTSDir=$HOME/
export BaseDirStaging=$HOME/$RootDir/$BaseDir/$BaseCodeDir/staging

##
## create config.ini
##

cd $HOME
cd $RootDir
cd $BaseDir

rm config.ini
echo "# SOLARIS Build Configuration">> config.ini
printf "\n" >> config.ini

echo "# Versioning Information.">> config.ini
echo Version.ProjectName=\"$VersionProjectName\">> config.ini
echo Version.Major=$VersionMajor>> config.ini
echo Version.Minor=$VersionMinor>> config.ini
echo Version.Iteration=$VersionIteration>> config.ini
echo "Version=\${Version.Major}.\${Version.Minor}">>config.ini


printf "\n" >> config.ini
echo "# Perforce Configuration">> config.ini
echo P4.View.CODE_TA_BASE=$P4ViewCODE_TA_BASE>> config.ini
echo P4.View.COTS=$P4ViewCOTS>>config.ini

printf "\n" >> config.ini
echo "# Build Configuration">> config.ini
echo Build.Configuration=$BuildConfiguration>>config.ini

printf "\n" >> config.ini
echo "# Directories">>config.ini
echo Base.Dir=$BaseDirectory>>config.ini
echo COTS.Dir=$COTSDir>>config.ini
echo Base.Dir.Staging=$BaseDirStaging>>config.ini

##
## Create build.sh file
##

cd $HOME
cd $RootDir
cd $BaseDir

rm build.sh

echo "#!/bin/bash">>build.sh
printf "\n" >> build.sh

echo export ROOT_BASE=\"$HOME/$RootDir/$BaseDir/$BaseCodeDir\">>build.sh
printf "\n" >> build.sh

echo "# Prepare the environment.">>build.sh
echo source $HOME/.profile>>build.sh
echo source $HOME/$RootDir/.p4setting>>build.sh
printf "\n" >> build.sh

echo cd \$ROOT_BASE>>build.sh
echo ant  clean_all clean_cots>>build.sh

printf "\n" >> build.sh
echo mkdir code>>build.sh
echo cd code>>build.sh

printf "\n" >> build.sh
echo "# syn link to the cots.">>build.sh
echo rm -rf cots>>build.sh
echo ln -s $HOME/cots cots>>build.sh

printf "\n" >> build.sh
echo "# Execute the base build.">>build.sh
echo cd \$ROOT_BASE>>build.sh
echo ant p4checkoutCots p4checkoutTools p4checkoutTransactive version build stage>>build.sh

chmod +x build.sh

##
## ************************* End: DO NOT Change anything within this block ************************
##

## 
## Setup build.xml. Copy and rename "Base_Solaris_build.xml" to "build.xml" 
## 

cd $HOME

cp Base_Solaris_build.xml $HOME/$RootDir/$BaseDir/$BaseCodeDir/build.xml

##
## rename Solaris_Versioning.pl to Versioning.pl
##

cd $HOME
rm Solaris_Versioning.pl Versioning.pl

##
## ************************ End: Setup base code build environment ***********************
##

##
## ************************ Start: Setup project code build environment ***********************
##

##########################################################################
#############                                                    #########
#############      CONFIGURATION VARIABLES: CHANGE AS NEEDED     #########
#############                     (For 4669)                     #########

##
## Define configuration information needed for config.ini
##

##
## Version Information 
##
## This information is mainly used by Build team during formal release of software to customer.
## For developer, you dont have change these fields.
## 
## NOTE: For developer, you can ONLY use 99 for all field. 
## 

export VersionProjectName="Singapore Circle Line"
export VersionMajor=99.99
export VersionMinor=99.99
export VersionIteration=99999999
export VersionCompileMode=T

##
## Define perforce configuration
## 
## Example:
## P4.View.CODE_4669 - the branch that you want to checkout the project codes
##

export P4ViewCODE_4669=//depot/CODE_4669_T01271300
export P4ViewCOTS=//depot/COTS #dont need to change this

## 
## Define the build configuration to either Debug or Release
## 
## Example:
##   BuildConfiguration=Debug
## 

export BuildConfiguration=Debug

## 
## Define the debug or release bin folder. 
## If BuildConfiguration is set to Debug, Bin.Folder must be SOLARIS_nd
## If BuildConfiguration is set to Release, Bin.Folder must be SOLARIS_nr
## 

export BinFolder=SOLARIS_nd

#############                                                    ##########
#############           END OF CONFIGURATION VARIABLES           ##########
#############                                                    ##########
###########################################################################

##
## ************************ End: Setup project code build environment ***********************
##

##
## Define project directory, base staging directory, project staging directory and temp directory
##
export ProjectDirectory=$HOME
export BaseDirStaging=$HOME/$RootDir/$BaseDir/$BaseCodeDir/staging
export ProjectDirStaging=$HOME/$RootDir/$ProjectDir/$ProjectCodeDir/staging
export TempPackageDir=$HOME/temp

##
## ************************* End: DO NOT Change anything within this block ************************
##

##
## create config.ini
##

mkdir $RootDir
mkdir $RootDir/$ProjectDir
mkdir $RootDir/$ProjectDir/$ProjectCodeDir

cd $HOME
cd $RootDir
cd $ProjectDir

rm config.ini
echo "# SOLARIS Build Configuration">> config.ini
printf "\n" >> config.ini

echo "# Versioning Information.">> config.ini
echo Version.ProjectName=\"$VersionProjectName\">> config.ini
echo Version.Major=$VersionMajor>> config.ini
echo Version.Minor=$VersionMinor>> config.ini
echo Version.Iteration=$VersionIteration>> config.ini
echo "Version=\${Version.Major}.\${Version.Minor}">>config.ini
echo Version.Compile.Mode=${VersionCompileMode}>>config.ini


printf "\n" >> config.ini
echo "# Perforce Configuration">> config.ini
echo P4.View.CODE_4669=$P4ViewCODE_4669>> config.ini
echo P4.View.COTS=$P4ViewCOTS>>config.ini

printf "\n" >> config.ini
echo "# Build Configuration">> config.ini
echo Build.Configuration=$BuildConfiguration>>config.ini
echo Bin.Folder=$BinFolder>>config.ini

printf "\n" >> config.ini
echo "# Directories">>config.ini
echo 4669.Dir=$ProjectDirectory>>config.ini
echo Base.Dir.Staging=$BaseDirStaging>>config.ini
echo 4669.Dir.Staging=$ProjectDirStaging/CODE_4669_\${Version.Compile.Mode}\${Version.Iteration}>>config.ini
echo Temp.Package.Dir=$TempPackageDir>>config.ini

##
## create temp folder for doing packaging
##

mkdir $HOME/temp


##
## create build.sh
##

cd $HOME
cd $RootDir
cd $ProjectDir

rm build.sh

echo "#!/bin/bash">>build.sh
printf "\n" >> build.sh

echo export ROOT_BASE=\"$HOME/$RootDir/$BaseDir/$BaseCodeDir\">>build.sh
echo export ROOT_4669=\"$HOME/$RootDir/$ProjectDir/$ProjectCodeDir\">>build.sh
printf "\n" >> build.sh

echo "# Prepare the environment.">>build.sh
echo source $HOME/.profile>>build.sh
echo source $HOME/$RootDir/.p4setting>>build.sh
printf "\n" >> build.sh

echo cd \$ROOT_4669>>build.sh
echo ant  clean_all>>build.sh

printf "\n" >> build.sh
echo mkdir code>>build.sh
echo cd code>>build.sh

printf "\n" >> build.sh
echo "# syn link to the cots.">>build.sh
echo rm -rf cots>>build.sh
echo ln -s $HOME/cots cots>>build.sh

printf "\n" >> build.sh
echo cd \$ROOT_4669>>build.sh

printf "\n" >> build.sh
echo "# syn link to the appropriate base sdk.">>build.sh
echo rm -rf sdk>>build.sh
echo ln -s \$ROOT_BASE/staging/sdk sdk>>build.sh

printf "\n" >> build.sh
echo "# Execute the base build.">>build.sh
echo ant clean_all p4checkoutCode version build stage package>>build.sh

chmod +x build.sh

## 
## Setup build.xml. Copy and rename "Project_Solaris_build.xml" to "build.xml" 
## 

cd $HOME

cp Project_Solaris_build.xml $HOME/$RootDir/$ProjectDir/$ProjectCodeDir/build.xml

##
## ************************* END: DO NOT Change anything within this block ************************
##
