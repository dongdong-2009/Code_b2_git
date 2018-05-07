#
# Simple profile places /usr/bin at front, followed by /usr/sbin.
#
# Use less(1) or more(1) as the default pager for the man(1) command.
#
export PATH=/usr/bin:/usr/sbin

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


#
# alias
#
alias ls='ls -lrt'
alias start='transactive start'
alias START='start'
alias stop='watson kill'
alias STOP='stop'
alias rm='rm -f'
alias PWD='pwd'


#
# system
#
ulimit -n 65535

#
# core-dump
#
coreadm -p /u01/transactive/core_files/%n.%f.%p $$

#
# /export/home/local
#
export PATH=/export/home/local/bin:$PATH

#
# transactive
#
export PATH=/u01/transactive/bin:/u01/transactive:$PATH
export LD_LIBRARY_PATH=/u01/transactive/lib:$LD_LIBRARY_PATH

#
# p4
#
export P4USER=limin.zhu
export P4PASSWD=Coreteam
export P4CLIENT=limin.zhu_solaris11
export P4PORT=192.168.253.15:1666

#
# oracle
#
export ORACLE_HOME=/u01/app/oracle/instantclient_11_2
export TNS_ADMIN=$ORACLE_HOME
export LD_LIBRARY_PATH=$ORACLE_HOME:$LD_LIBRARY_PATH
export NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS32GB18030"
export PATH=$ORACLE_HOME:$PATH
