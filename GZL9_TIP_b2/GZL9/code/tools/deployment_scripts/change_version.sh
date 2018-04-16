#!/bin/sh
version_file=/u01/transactive/bin/versions.`hostname`
command=$1
argument=$2
extension_argument=$3

# show usage
if [ "$*" = "" ]; then
     echo "Usage:"
     echo "      changeversion.sh <command> <argument> [extension_argument]"
     echo ""
     echo "Examples:"
     echo "      changeversion.sh change-version 00.00.00.00"
     echo "      changeversion.sh add-agent TransactiveNewAgent 00.00"
     echo "      changeversion.sh remove-agent TransactiveNewAgent"
     exit 0
fi

# check existance of tool 'sed'
is_sed_exist=`sed`
if [ "$is_sed_exist" != "" ]; then
    echo "error: can not find tool 'sed', error is: $is_sed_exist."
    exit 1
fi

# check existance of version file
if [ ! -f "$version_file" ]; then
    echo "error: version file '$version_file' doesn't exist."
    exit 1
fi

######################################################
# Version:00.07                                      #
#        |--|                                      #
# Occ COM Alarm Agent:01.00:=Occ_comAlarmAgent       #
# |--2--| |---3-----| |-4-|  |--5--|                 #
#                                                    #
# 1 big version                                      #
# 2 server name                                      #
# 3 agent description                                #
# 4 little version                                   #
# 5 agent prefix                                     #
######################################################

#extract parameters from version file
big_version_current=`sed -n 's/Version://p' $version_file`
server_name=`sed -n 's/ Alarm Agent.*$//p' $version_file`
little_version_current=`sed -n -e 's/System Controller://' -e 's/:SystemController//p' $version_file`
agent_prefix=`sed -n -e 's/^.*Alarm Agent.*=//' -e 's/AlarmAgent//p' $version_file`

# validate the version file
if [ "$server_name" = "" ] || [ "$agent_prefix" = "" ] || [ "$big_version_current" = "" ] || [ "$little_version_current" = "" ]; then
    echo "error: cannot find key string(s) from file '$version_file':"
    echo "big version: $big_version_current"
    echo "server name: $server_name"
    echo "little version: $little_version_current"
    echo "agent prefix: $agent_prefix"
    exit 1
fi

# execute command
case "$command" in
change-version)
#     echo "BEGIN changing version to '$argument'."

    # check version format
    check_version_format=`echo $argument | sed -n 's/^[0-9][0-9].[0-9][0-9].[0-9][0-9].[0-9][0-9]$/&/p'`

    if [ "$check_version_format" = "" ]; then
         echo "error: the format of version '$argument' is not correct."
         exit 1
    fi

    # split version to big version and little version
    big_version_new=`echo "$argument" | awk -F"." '{print $1"."$2}'`
    little_version_new=`echo "$argument" | awk -F"." '{print $3"."$4}'`

    # change version
    if [ "$big_version_current" != "$big_version_new" ] || [ "$little_version_current" != "$little_version_new" ]; then
        sed -e "s/Version:$big_version_current/Version:$big_version_new/" -e "s/:$little_version_current:/:$little_version_new:/" $version_file > changeversion.temp

        # update version file
        rm -f $version_file; mv changeversion.temp $version_file
    fi
    ;;

add-agent)
#     echo "BEGIN adding agent '$argument'."

    # check if agent name is empty
    if [ "$argument" = "" ]; then
        echo "error: agent name is empty."
        exit 1
    fi

    # check if the agent version is empty
    if [ "$extension_argument" = "" ]; then
        echo "error: the version is empty."
        exit 1
    fi

    #check the format of the version
    check_version_format=`echo $extension_argument | sed -n 's/^[0-9][0-9].[0-9][0-9]$/&/p'`
    if [ "$check_version_format" = "" ]; then
        echo "error: the format of the version '$extension_argument' is not correct."
        exit 1
    fi

    # check existance of agent to add
    is_agent_exist=`grep $argument $version_file`

    if [ "$is_agent_exist" = "" ]; then
        sNewAgent=$argument
        awk 'BEGIN{agtVer="";lastSta="";} \
            agtVer!=""{if(lastSta != $1){ if(lastSta!=""){printf("%s %s:%s:=%s%s\n", lastSta,agtName, theVer, lastSta, newAgt);};lastSta=$1;}} \
            agtVer==""&&/System Controller/{ split($2, a, ":"); agtVer=a[2];n=length(newAgt);agtName="";for(i=0; i<n; ++i){t=substr(newAgt,i+1,1);if(t>="A" && t<="Z" && agtName !="")agtName=agtName" ";agtName=agtName""t};} \
            {print} \
            END{printf("%s %s:%s:=%s%s\n", lastSta,agtName, theVer, lastSta, newAgt);}' theVer="$extension_argument" newAgt="$sNewAgent" $version_file > changeversion.temp
            
            # update version file
            rm -f $version_file; mv changeversion.temp $version_file
    fi
    ;;

remove-agent)
#     echo "BEGIN removing agent '$argument'."

    # check if agent name is empty
    if [ "$argument" = "" ]; then
        echo "error: agent name is empty."
        exit 1
    fi

    # check existance of agent to add
    is_agent_exist=`grep $argument $version_file`

    if [ "$is_agent_exist" != "" ]; then
        # delete the line contains agent string
        sed "/$argument/d" $version_file > changeversion.temp

        # update version file
        rm -f $version_file; mv changeversion.temp $version_file
    fi
    ;;

*)
    echo "error: wrong command '$command'."
    exit 1
    ;;

esac

# echo "DONE"
exit 0
