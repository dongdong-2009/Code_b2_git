#
# Simple profile places /usr/bin at front, followed by /usr/sbin.
#
# Use less(1) or more(1) as the default pager for the man(1) command.
#
export PATH=/usr/bin:/usr/sbin:/usr/local/bin:/export/home/core/app/oracle/instantclient_11_2:/export/home/core/transactive/bin
export ANT_HOME=/usr/local/ant
export CLASSPATH=$ANT_HOME/lib
export LD_LIBRARY_PATH=/usr/local/lib:/export/home/COTS/icu/4.2.1/SOLARIS_5.11_sun4/lib:/export/home/core/app/oracle/instantclient_11_2:/export/home/core/transactive/lib
export TNS_ADMIN=/export/home/core/app/oracle/instantclient_11_2
export ORACLE_HOME=/export/home/core/app/oracle/instantclient_11_2
export NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS32GB18030"
export LC_ALL=C
export LANG=C

if [ -f /usr/bin/less ]; then
    export PAGER="/usr/bin/less -ins"
elif [ -f /usr/bin/more ]; then
    export PAGER="/usr/bin/more -s"
fi

#
# Define default prompt to <username>@<hostname>:<path><"($|#) ">
# and print '#' for user "root" and '$' for normal users.
#
# Currently this is only done for bash/pfbash(1).
# 

case ${SHELL} in
*bash)
    typeset +x PS1="\u@\h:\w\\$ "
    ;;
esac 

###########
#
# limin
#
###########
export U01=/export/home/core/limin/u01
export TRA_HOME=$U01/transactive
export PATH=$PATH:/export/home/core/limin/local/bin

#
# P4
#
export P4USER=limin.zhu
export P4PASSWD=Coreteam
export P4CLIENT=limin.zhu_192.168.253.64_verify
export P4PORT=192.168.253.15:1666

#
# alias
#
alias start='transactive start'
alias stop='watson kill'
alias START='start'
alias STOP='stop'
alias PWD='pwd'
alias ls='ls -lrt'

#
# transactive
#
export PATH=$TRA_HOME:$TRA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$TRA_HOME/lib:$LD_LIBRARY_PATH

#
# oracle
#
export ORACLE_HOME=$U01/app/oracle/instantclient_11_2
export TNS_ADMIN=$ORACLE_HOME
export LD_LIBRARY_PATH=$ORACLE_HOME:$LD_LIBRARY_PATH
export NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS32GB18030"
export PATH=$ORACLE_HOME:$PATH

#
# core-dump
#
coreadm -p $TRA_HOME/core_files/%f.%p $$
