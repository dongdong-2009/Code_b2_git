#	This is the default standard profile provided to a user.
#	They are expected to edit it to meet their own needs.

export PATH=/usr/bin:/usr/sbin

if [ -d /opt/samba/bin/ ]; then
        export PATH=${PATH}:/opt/samba/bin
fi

export EDITOR=vi

# Source this to set the environment up properly

export ANT_HOME=/usr/local/ant
export PYTHONPATH=/usr/local/lib/python2.6
export LD_LIBRARY_PATH=/usr/local/lib
export CLASSPATH=$ANT_HOME/lib

export PATH=$PATH:$ANT_HOME/bin:/usr/ccs/bin

export ANT_ARGS="-logger buildsystem.log.ParallelLogRedirector"

###########
#
# limin
#
###########
export U01=/export/home/core/limin/u01
export TRA_HOME=$U01/transactive
export PATH=/export/home/core/limin/local/bin:/export/home/core/limin/local/gcc-4.8.5/bin:/usr/bin:/usr/sbin:/usr/local/ant/bin:/usr/ccs/bin:/usr/sfw/bin
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:/export/home/core/limin/local/gcc-4.8.5/lib:/export/home/core/limin/local/lib:/usr/sfw/lib

#
# P4
#
export P4USER=limin.zhu
export P4PASSWD=Coreteam
export P4CLIENT=limin.zhu_192.168.253.66_verify
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
